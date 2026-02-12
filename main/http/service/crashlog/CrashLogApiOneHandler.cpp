#include "CrashLogApiOneHandler.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_vfs.h"
#include <sys/stat.h>
#include <string>
#include <cstring>
#include "../../../crash_report/CrashReport.h"

static const char *TAG = "CrashLogApiOneHandler";

// 辅助函数：从HTTP请求中提取查询参数中的文件名
static esp_err_t extract_filename_from_query(httpd_req_t *req, char *filename, size_t filename_size) {
  // 首先获取查询字符串的长度
  size_t query_len = httpd_req_get_url_query_len(req);
  
  if (query_len > 0) {
    // 分配缓冲区存储查询字符串
    char *query_str = (char *)malloc(query_len + 1);
    if (!query_str) {
      ESP_LOGE(TAG, "Failed to allocate memory for query string");
      return ESP_ERR_NO_MEM;
    }
    
    // 获取查询字符串
    esp_err_t err = httpd_req_get_url_query_str(req, query_str, query_len + 1);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to get query string");
      free(query_str);
      return err;
    }
    
    // 从查询字符串中提取 name 参数
    err = httpd_query_key_value(query_str, "name", filename, filename_size);
    free(query_str);
    
    if (err == ESP_OK) {
      return ESP_OK;
    } else {
      // 如果查询参数不存在，返回错误
      ESP_LOGE(TAG, "Missing 'name' parameter in query string");
      return ESP_ERR_INVALID_ARG;
    }
  } else {
    // 如果没有查询字符串，返回错误
    ESP_LOGE(TAG, "Missing query string");
    return ESP_ERR_INVALID_ARG;
  }
}

CrashLogApiOneHandler::CrashLogApiOneHandler(const std::string& uri) : AbstractHttpReqHandler(uri) {
}

CrashLogApiOneHandler::~CrashLogApiOneHandler() {
}

esp_err_t CrashLogApiOneHandler::handleGetRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "Handling coredump download request for URI: %s", req->uri);

  // 提取文件名
  char filename[256];
  esp_err_t err = extract_filename_from_query(req, filename, sizeof(filename));
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing 'name' parameter");
    return ESP_FAIL;
  }
  const char *file_start = filename;

  // 构建完整的文件路径
  std::string crash_report_dir = CrashReport::getInstance().getCrashReportDir();
  std::string filepath = crash_report_dir + std::string(file_start);

  // 检查文件是否存在
  struct stat st;
  if (stat(filepath.c_str(), &st) != 0) {
    ESP_LOGE(TAG, "File does not exist: %s", filepath.c_str());
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
    return ESP_FAIL;
  }

  // 设置响应头
  httpd_resp_set_type(req, "application/octet-stream");
  std::string content_disposition = "attachment; filename=\"" + std::string(file_start) + "\"";
  httpd_resp_set_hdr(req, "Content-Disposition", content_disposition.c_str());

  // 发送文件内容
  FILE *file = fopen(filepath.c_str(), "rb");
  if (!file) {
    ESP_LOGE(TAG, "Failed to open file: %s", filepath.c_str());
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to open file");
    return ESP_FAIL;
  }

  char *buf = (char*)malloc(1024);
  if (!buf) {
    fclose(file);
    ESP_LOGE(TAG, "Failed to allocate buffer");
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory allocation failed");
    return ESP_FAIL;
  }

  int read_len;
  while ((read_len = fread(buf, 1, 1024, file)) > 0) {
    if (httpd_resp_send_chunk(req, buf, read_len) != ESP_OK) {
      ESP_LOGE(TAG, "Failed to send chunk");
      free(buf);
      fclose(file);
      return ESP_FAIL;
    }
  }

  free(buf);
  fclose(file);

  ESP_LOGI(TAG, "Successfully sent file: %s", filepath.c_str());
  return ESP_OK;
}

esp_err_t CrashLogApiOneHandler::handleDeleteRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "Handling coredump delete request for URI: %s", req->uri);

  // 提取文件名
  char filename[256];
  esp_err_t err = extract_filename_from_query(req, filename, sizeof(filename));
  if (err != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing 'name' parameter");
    return ESP_FAIL;
  }
  const char *file_start = filename;

  // 构建完整的文件路径
  std::string crash_report_dir = CrashReport::getInstance().getCrashReportDir();
  std::string filepath = crash_report_dir + std::string(file_start);

  // 检查文件是否存在
  struct stat st;
  if (stat(filepath.c_str(), &st) != 0) {
    ESP_LOGE(TAG, "File does not exist: %s", filepath.c_str());
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
    return ESP_FAIL;
  }

  // 删除文件
  if (unlink(filepath.c_str()) == 0) {
    ESP_LOGI(TAG, "Successfully deleted file: %s", filepath.c_str());
    const char *msg = "File deleted successfully";
    esp_err_t ret = httpd_resp_send(req, msg, strlen(msg));
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to send response");
      return ret;
    }
    return ESP_OK;
  } else {
    ESP_LOGE(TAG, "Failed to delete file: %s", filepath.c_str());
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to delete file");
    return ESP_FAIL;
  }
}

esp_err_t CrashLogApiOneHandler::handlePostRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "CrashLogApiOneHandler handlePostRequest for URI: %s", req->uri);
  return ESP_OK;
}