#include "UploadHandler.h"
#include "esp_log.h"
#include "sys/param.h"
#include "esp_http_server.h"
#include "esp_heap_caps.h"
#include "../../assets/assets.h"
#include <string>
#include <cstring>
#include <cstdio>
#include <unistd.h>

static const char *TAG = "UploadHandler";

UploadHandler::UploadHandler(const std::string& uri_prefix) : AbstractHttpReqHandler(uri_prefix) {}

UploadHandler::~UploadHandler() {}

esp_err_t UploadHandler::handleGetRequest(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handling GET request for URI: %s", req->uri);
    
    // 返回上传页面HTML
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "identity");
    
    // 使用嵌入的HTML资源
    const size_t html_size = binary_assets_upload_html_end - binary_assets_upload_html_start;
    const char* html_content = reinterpret_cast<const char*>(binary_assets_upload_html_start);
    
    return httpd_resp_send(req, html_content, html_size);
}

esp_err_t UploadHandler::handlePostRequest(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handling POST request for URI: %s", req->uri);
    
    char filename[256] = {0};
    char filepath[512] = {0};
    int remaining = req->content_len;
    
    // 从Content-Type头中提取boundary
    char boundary[128] = {0};
    size_t content_type_len = httpd_req_get_hdr_value_len(req, "Content-Type");
    if (content_type_len > 0) {
        char *content_type = (char *)malloc(content_type_len + 1);
        if (content_type) {
            httpd_req_get_hdr_value_str(req, "Content-Type", content_type, content_type_len + 1);
            
            char *boundary_pos = strstr(content_type, "boundary=");
            if (boundary_pos) {
                boundary_pos += 9; // 跳过 "boundary="
                strncpy(boundary, boundary_pos, sizeof(boundary) - 1);
                boundary[sizeof(boundary) - 1] = '\0';
                
                // 移除可能的额外参数
                char *semicolon = strchr(boundary, ';');
                if (semicolon) {
                    *semicolon = '\0';
                }
                // 移除可能的引号
                if (boundary[0] == '"') {
                    memmove(boundary, boundary + 1, strlen(boundary));
                }
                char *quote = strchr(boundary, '"');
                if (quote) {
                    *quote = '\0';
                }
            }
            free(content_type);
        }
    }
    
    if (strlen(boundary) == 0) {
        ESP_LOGE(TAG, "Could not extract boundary from Content-Type");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid boundary in Content-Type");
        return ESP_FAIL;
    }
    
    ESP_LOGD(TAG, "Boundary: '--%s'", boundary);
    
    // 分配缓冲区
    char* buffer = (char*)heap_caps_malloc(2048, MALLOC_CAP_SPIRAM);
    if (!buffer) {
        ESP_LOGE(TAG, "Failed to allocate buffer with SPIRAM");
        httpd_resp_send_500(req);
        return ESP_ERR_NO_MEM;
    }
    
    // 读取第一部分数据
    int recv_len = httpd_req_recv(req, buffer, MIN(2047, remaining));
    if (recv_len < 0) {
        if (recv_len == HTTPD_SOCK_ERR_TIMEOUT) {
            heap_caps_free(buffer);
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        ESP_LOGE(TAG, "Failed to read initial request");
        heap_caps_free(buffer);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    
    buffer[recv_len] = '\0';
    remaining -= recv_len;
    
    ESP_LOGD(TAG, "Received initial data:\n%.*s", recv_len > 200 ? 200 : recv_len, buffer);
    
    // 查找第一个boundary - 需要在整个缓冲区中查找
    char search_boundary[130];
    snprintf(search_boundary, sizeof(search_boundary), "--%s", boundary);
    
    // 查找第一个boundary
    char *first_boundary = NULL;
    int boundary_len = strlen(search_boundary);
    for (int i = 0; i <= recv_len - boundary_len; i++) {
        if (memcmp(&buffer[i], search_boundary, boundary_len) == 0) {
            first_boundary = &buffer[i];
            break;
        }
    }
    
    if (!first_boundary) {
        ESP_LOGE(TAG, "Could not find initial boundary '%s'", search_boundary);
        heap_caps_free(buffer);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid multipart data - no boundary found");
        return ESP_FAIL;
    }
    
    // 从boundary后开始查找filename
    char *part_header_start = first_boundary + boundary_len;
    
    // 查找CRLF或LF结束符，以确定头部区域
    char *header_end = NULL;
    for (int i = 0; i < recv_len - (part_header_start - buffer) - 4; i++) {
        if (part_header_start[i] == '\r' && part_header_start[i+1] == '\n' && 
            part_header_start[i+2] == '\r' && part_header_start[i+3] == '\n') {
            header_end = part_header_start + i + 4;
            break;
        } else if (part_header_start[i] == '\n' && part_header_start[i+1] == '\n') {
            header_end = part_header_start + i + 2;
            break;
        }
    }
    
    if (!header_end) {
        ESP_LOGE(TAG, "Could not find end of headers");
        heap_caps_free(buffer);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid multipart data - no header end found");
        return ESP_FAIL;
    }
    
    // 在头部区域内查找filename
    char *filename_pos = strstr(part_header_start, "filename=\"");
    if (!filename_pos || filename_pos >= header_end) {
        ESP_LOGE(TAG, "Could not find filename in part header");
        heap_caps_free(buffer);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "No filename found in multipart data");
        return ESP_FAIL;
    }
    
    filename_pos += 10; // 跳过 "filename=\""
    char *end_quote = strchr(filename_pos, '"');
    if (!end_quote || end_quote >= header_end) {
        ESP_LOGE(TAG, "Could not find end quote for filename");
        heap_caps_free(buffer);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid filename format");
        return ESP_FAIL;
    }
    
    *end_quote = '\0';
    strncpy(filename, filename_pos, sizeof(filename) - 1);
    filename[sizeof(filename) - 1] = '\0';
    
    // 构建文件路径
    snprintf(filepath, sizeof(filepath), "/sdcard/books/%s", filename);
    
    // 内容开始位置就是header结束位置
    char *content_start = header_end;
    
    // 打开文件进行写入
    FILE* file = fopen(filepath, "wb");
    if (!file) {
        ESP_LOGE(TAG, "Failed to open file for writing: %s", filepath);
        heap_caps_free(buffer);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    
    // 计算文件内容长度
    int header_len = content_start - buffer;
    int file_part_len = recv_len - header_len;
    
    // 写入第一部分文件内容
    if (file_part_len > 0) {
        fwrite(content_start, 1, file_part_len, file);
    }
    
    // 读取剩余的文件内容
    while (remaining > 0) {
        int to_read = MIN(2048, remaining);
        int actual_recv = httpd_req_recv(req, buffer, to_read);
        if (actual_recv < 0) {
            if (actual_recv == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;  // 继续尝试接收
            }
            ESP_LOGE(TAG, "Failed to read file content");
            fclose(file);
            unlink(filepath); // 删除部分写入的文件
            heap_caps_free(buffer);
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        
        fwrite(buffer, 1, actual_recv, file);
        remaining -= actual_recv;
    }
    
    fclose(file);
    heap_caps_free(buffer);
    
    ESP_LOGI(TAG, "File uploaded successfully: %s", filepath);
    
    // 返回成功响应
    httpd_resp_set_type(req, "application/json");
    const char* resp_str = "{\"status\":\"success\",\"message\":\"File uploaded successfully\"}";
    httpd_resp_sendstr(req, resp_str);
    
    return ESP_OK;
}