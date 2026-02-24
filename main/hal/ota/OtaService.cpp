#include "OtaService.h"

#include "../version.h"
#include "cJSON.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <mbedtls/sha256.h>
#include <memory>
#include <vector>

static const char* TAG = "OtaService";

namespace {

std::string normalizeHex(std::string hex) {
    std::string out;
    out.reserve(hex.size());
    for (char c : hex) {
        if (std::isxdigit(static_cast<unsigned char>(c))) {
            out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
    }
    return out;
}

std::string toHex(const unsigned char* data, size_t len) {
    static const char* kHex = "0123456789abcdef";
    std::string out;
    out.reserve(len * 2);
    for (size_t i = 0; i < len; ++i) {
        unsigned char b = data[i];
        out.push_back(kHex[(b >> 4) & 0x0F]);
        out.push_back(kHex[b & 0x0F]);
    }
    return out;
}

esp_err_t httpGetString(const std::string& url, std::string& body, int& statusCode) {
    esp_http_client_config_t config = {};
    config.url = url.c_str();
    config.method = HTTP_METHOD_GET;
    config.timeout_ms = 20000;
    config.crt_bundle_attach = esp_crt_bundle_attach;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        return ESP_FAIL;
    }

    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        esp_http_client_cleanup(client);
        return err;
    }

    statusCode = esp_http_client_get_status_code(client);
    char buffer[512];
    while (true) {
        int read = esp_http_client_read(client, buffer, sizeof(buffer));
        if (read < 0) {
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            return ESP_FAIL;
        }
        if (read == 0) {
            break;
        }
        body.append(buffer, read);
    }

    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    return ESP_OK;
}

} // namespace

std::string OtaService::getCurrentVersionString() {
    return std::string(GIT_COMMIT_TIME) + "+" + GIT_COMMIT_HASH;
}

int OtaService::getCurrentCommitCount() { return std::atoi(GIT_COMMIT_COUNT); }

std::string OtaService::getDeviceType() {
#ifdef CONFIG_IDF_TARGET
    return CONFIG_IDF_TARGET;
#else
    return "unknown";
#endif
}

esp_err_t OtaService::checkForUpdate(const std::string& endpoint, OtaCheckResult& outResult) {
    outResult = OtaCheckResult{};
    const std::string requestUrl = endpoint + "?device=" + getDeviceType() + "&version=" + getCurrentVersionString();
    ESP_LOGI(TAG, "Checking OTA: %s", requestUrl.c_str());

    std::string body;
    int statusCode = 0;
    esp_err_t err = httpGetString(requestUrl, body, statusCode);
    if (err != ESP_OK) {
        outResult.message = "request failed";
        return err;
    }
    if (statusCode != 200) {
        outResult.message = "http status: " + std::to_string(statusCode);
        return ESP_FAIL;
    }

    std::unique_ptr<cJSON, decltype(&cJSON_Delete)> root(cJSON_Parse(body.c_str()), cJSON_Delete);
    if (!root) {
        outResult.message = "invalid json response";
        return ESP_FAIL;
    }

    const cJSON* latest = cJSON_GetObjectItemCaseSensitive(root.get(), "latest");
    const cJSON* force = cJSON_GetObjectItemCaseSensitive(root.get(), "force");
    const cJSON* url = cJSON_GetObjectItemCaseSensitive(root.get(), "url");
    const cJSON* sha256 = cJSON_GetObjectItemCaseSensitive(root.get(), "sha256");

    if (!latest || (!cJSON_IsNumber(latest) && !cJSON_IsString(latest)) || !cJSON_IsString(url) ||
        !cJSON_IsString(sha256)) {
        outResult.message = "missing required json fields";
        return ESP_FAIL;
    }

    int latestCount = 0;
    if (cJSON_IsNumber(latest)) {
        latestCount = latest->valueint;
    } else {
        latestCount = std::atoi(latest->valuestring);
    }

    outResult.latestCommitCount = latestCount;
    outResult.force = force ? cJSON_IsTrue(force) : false;
    outResult.url = url->valuestring ? url->valuestring : "";
    outResult.sha256 = sha256->valuestring ? sha256->valuestring : "";

    const int currentCount = getCurrentCommitCount();
    outResult.updateAvailable = latestCount > currentCount;
    outResult.message =
        outResult.updateAvailable ? "update available" : "already latest or server version is not newer";
    return ESP_OK;
}

esp_err_t OtaService::downloadAndInstall(const OtaCheckResult& result, std::string& errMsg) {
    errMsg.clear();
    if (result.url.empty() || result.sha256.empty()) {
        errMsg = "missing firmware url or sha256";
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t* updatePartition = esp_ota_get_next_update_partition(nullptr);
    if (!updatePartition) {
        errMsg = "no ota partition found";
        return ESP_FAIL;
    }

    esp_ota_handle_t otaHandle = 0;
    esp_err_t err = esp_ota_begin(updatePartition, OTA_SIZE_UNKNOWN, &otaHandle);
    if (err != ESP_OK) {
        errMsg = "esp_ota_begin failed";
        return err;
    }

    esp_http_client_config_t config = {};
    config.url = result.url.c_str();
    config.method = HTTP_METHOD_GET;
    config.timeout_ms = 30000;
    config.crt_bundle_attach = esp_crt_bundle_attach;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        esp_ota_abort(otaHandle);
        errMsg = "esp_http_client_init failed";
        return ESP_FAIL;
    }

    err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        esp_http_client_cleanup(client);
        esp_ota_abort(otaHandle);
        errMsg = "open firmware url failed";
        return err;
    }

    int statusCode = esp_http_client_get_status_code(client);
    if (statusCode != 200) {
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        esp_ota_abort(otaHandle);
        errMsg = "firmware http status: " + std::to_string(statusCode);
        return ESP_FAIL;
    }

    mbedtls_sha256_context shaCtx;
    mbedtls_sha256_init(&shaCtx);
    mbedtls_sha256_starts(&shaCtx, 0);

    std::vector<char> buffer(2048);
    while (true) {
        int read = esp_http_client_read(client, buffer.data(), static_cast<int>(buffer.size()));
        if (read < 0) {
            mbedtls_sha256_free(&shaCtx);
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            esp_ota_abort(otaHandle);
            errMsg = "read firmware stream failed";
            return ESP_FAIL;
        }
        if (read == 0) {
            break;
        }

        err = esp_ota_write(otaHandle, buffer.data(), read);
        if (err != ESP_OK) {
            mbedtls_sha256_free(&shaCtx);
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            esp_ota_abort(otaHandle);
            errMsg = "esp_ota_write failed";
            return err;
        }

        mbedtls_sha256_update(&shaCtx, reinterpret_cast<const unsigned char*>(buffer.data()), read);
    }

    unsigned char digest[32] = {0};
    mbedtls_sha256_finish(&shaCtx, digest);
    mbedtls_sha256_free(&shaCtx);

    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    const std::string actualSha = toHex(digest, sizeof(digest));
    const std::string expectedSha = normalizeHex(result.sha256);
    if (actualSha != expectedSha) {
        esp_ota_abort(otaHandle);
        errMsg = "sha256 mismatch";
        ESP_LOGE(TAG, "sha256 mismatch expected=%s actual=%s", expectedSha.c_str(), actualSha.c_str());
        return ESP_ERR_INVALID_CRC;
    }

    err = esp_ota_end(otaHandle);
    if (err != ESP_OK) {
        errMsg = "esp_ota_end failed";
        return err;
    }

    err = esp_ota_set_boot_partition(updatePartition);
    if (err != ESP_OK) {
        errMsg = "esp_ota_set_boot_partition failed";
        return err;
    }

    return ESP_OK;
}
