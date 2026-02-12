#include "CrashLogApiAllHandler.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_vfs.h"
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <cstring>
#include <vector>
#include "../../../crash_report/CrashReport.h"

static const char *TAG = "CrashLogApiAllHandler";

CrashLogApiAllHandler::CrashLogApiAllHandler(const std::string& uri) : AbstractHttpReqHandler(uri) {
}

CrashLogApiAllHandler::~CrashLogApiAllHandler() {
}

esp_err_t CrashLogApiAllHandler::handleGetRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "Handling coredump list request for URI: %s", req->uri);

  // 获取coredump目录
  std::string crash_report_dir = CrashReport::getInstance().getCrashReportDir();
  
  // 打开目录
  DIR *dir = opendir(crash_report_dir.c_str());
  if (!dir) {
    ESP_LOGE(TAG, "Failed to open directory: %s", crash_report_dir.c_str());
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Directory not found");
    return ESP_FAIL;
  }

  // 收集所有.bin文件
  struct dirent *entry;
  std::vector<std::string> bin_files;
  
  while ((entry = readdir(dir)) != NULL) {
    std::string filename = entry->d_name;
    // 检查是否是.bin文件
    if (filename.length() >= 4 && filename.substr(filename.length() - 4) == ".bin") {
      bin_files.push_back(filename);
    }
  }
  closedir(dir);

  // 构建JSON响应
  std::string json_response = "[";
  for (size_t i = 0; i < bin_files.size(); ++i) {
    if (i > 0) {
      json_response += ",";
    }
    json_response += "{\"name\":\"" + bin_files[i] + "\"}";
  }
  json_response += "]";

  // 发送响应
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  httpd_resp_send(req, json_response.c_str(), json_response.length());

  ESP_LOGI(TAG, "Sent list of %zu coredump files", bin_files.size());
  return ESP_OK;
}

esp_err_t CrashLogApiAllHandler::handleDeleteRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "Handling coredump folder clear request for URI: %s", req->uri);

  // 获取coredump目录
  std::string crash_report_dir = CrashReport::getInstance().getCrashReportDir();
  
  // 打开目录
  DIR *dir = opendir(crash_report_dir.c_str());
  if (!dir) {
    ESP_LOGE(TAG, "Failed to open directory: %s", crash_report_dir.c_str());
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Directory not found");
    return ESP_FAIL;
  }

  // 删除目录中的所有.bin文件
  struct dirent *entry;
  int deleted_count = 0;
  int error_count = 0;
  
  while ((entry = readdir(dir)) != NULL) {
    std::string filename = entry->d_name;
    // 检查是否是.bin文件
    if (filename.length() >= 4 && filename.substr(filename.length() - 4) == ".bin") {
      std::string filepath = crash_report_dir + filename;
      
      if (unlink(filepath.c_str()) == 0) {
        deleted_count++;
        ESP_LOGI(TAG, "Deleted file: %s", filepath.c_str());
      } else {
        error_count++;
        ESP_LOGE(TAG, "Failed to delete file: %s", filepath.c_str());
      }
    }
  }
  closedir(dir);

  // 构建响应消息
  std::string response_msg = "Successfully cleared " + std::to_string(deleted_count) + " coredump files";
  if (error_count > 0) {
    response_msg += ", failed to delete " + std::to_string(error_count) + " files";
  }

  // 发送响应
  httpd_resp_send(req, response_msg.c_str(), response_msg.length());

  ESP_LOGI(TAG, "Cleared coredump folder: %d files deleted, %d errors", deleted_count, error_count);
  return ESP_OK;
}

esp_err_t CrashLogApiAllHandler::handlePostRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "CrashLogApiAllHandler handlePostRequest for URI: %s", req->uri);
  return ESP_OK;
}