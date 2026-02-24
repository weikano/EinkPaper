#include "PageFileResponder.h"

#include "esp_log.h"
#include <cstdio>

static const char* TAG = "PageFileResponder";

esp_err_t sendFileResponse(httpd_req_t* req, const char* filePath, const char* contentType) {
    FILE* file = fopen(filePath, "rb");
    if (!file) {
        ESP_LOGE(TAG, "Failed to open file: %s", filePath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "file not found");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, contentType);
    httpd_resp_set_hdr(req, "Content-Encoding", "identity");

    char buffer[1024];
    while (true) {
        size_t read = fread(buffer, 1, sizeof(buffer), file);
        if (read > 0) {
            esp_err_t err = httpd_resp_send_chunk(req, buffer, read);
            if (err != ESP_OK) {
                fclose(file);
                return err;
            }
        }
        if (read < sizeof(buffer)) {
            break;
        }
    }

    fclose(file);
    return httpd_resp_send_chunk(req, nullptr, 0);
}

