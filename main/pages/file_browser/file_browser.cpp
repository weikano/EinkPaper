#include "file_browser.h"
#include "ui_kit/UIKIT.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>

static const char *TAG = "FileBrowser";

// 最大路径长度
#define MAX_PATH_LEN 512
// 每次读取的目录项数量
#define DIR_READ_BATCH_SIZE 50
// 显示的文件名最大长度
#define MAX_DISPLAY_NAME_LEN 64

// 文件浏览器状态结构
typedef struct {
    char current_path[MAX_PATH_LEN];
    LinearLayout* screen_layout;
    ListView* file_list_view;
    std::vector<std::string> file_items;          // 存储文件/目录项
    std::vector<std::string> file_full_paths;     // 存储完整路径
    std::vector<bool> is_directory;               // 标记是否为目录
} file_browser_t;

static file_browser_t g_file_browser = {};
static SemaphoreHandle_t file_browser_mutex = NULL;

// 回调函数指针定义
static FileSelectedCallback file_selected_callback = nullptr;

/**
 * @brief 检查文件扩展名是否为允许的类型
 * @param filename 文件名
 * @return true 如果是允许的文件类型，否则 false
 */
static bool is_allowed_file_type(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (ext != NULL) {
        if (strcasecmp(ext, ".txt") == 0 || strcasecmp(ext, ".epub") == 0) {
            return true;
        }
    }
    return false;
}

/**
 * @brief 比较函数，用于排序目录项（目录在前，文件在后，按字母顺序）
 */
static bool compare_entries(const std::pair<std::string, std::string>& a, const std::pair<std::string, std::string>& b) {
    struct stat st_a, st_b;
    bool is_dir_a = (stat(a.second.c_str(), &st_a) == 0 && S_ISDIR(st_a.st_mode));
    bool is_dir_b = (stat(b.second.c_str(), &st_b) == 0 && S_ISDIR(st_b.st_mode));
    
    // 目录优先于文件
    if (is_dir_a && !is_dir_b) return true;
    if (!is_dir_a && is_dir_b) return false;
    
    // 同类型按名称排序
    return a.first < b.first;
}

/**
 * @brief 加载目录内容到列表
 * @param path 目录路径
 */
static void load_directory_content(const char *path) {
    DIR *dir;
    struct dirent *entry;
    
    // 清空现有列表
    g_file_browser.file_items.clear();
    g_file_browser.file_full_paths.clear();
    g_file_browser.is_directory.clear();
    
    dir = opendir(path);
    if (dir == NULL) {
        ESP_LOGE(TAG, "Cannot open directory: %s", path);
        return;
    }
    
    // 收集所有符合条件的目录项
    std::vector<std::pair<std::string, std::string>> entries; // {name, full_path}
    
    while ((entry = readdir(dir)) != NULL) {
        // 跳过当前目录和父目录
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        // 检查路径长度以避免溢出
        int len_path = strlen(path);
        int len_entry = strlen(entry->d_name);
        if (len_path + 1 + len_entry >= MAX_PATH_LEN) {
            continue; // 跳过过长的路径
        }
        
        char full_path[MAX_PATH_LEN];
        strcpy(full_path, path);
        strcat(full_path, "/");
        strcat(full_path, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) == 0) {
            if (S_ISDIR(st.st_mode) || is_allowed_file_type(entry->d_name)) {
                entries.push_back(std::make_pair(std::string(entry->d_name), std::string(full_path)));
            }
        }
    }
    
    closedir(dir);
    
    // 排序目录项
    std::sort(entries.begin(), entries.end(), compare_entries);
    
    // 添加返回上级目录选项（如果不是根目录）
    if (strcmp(path, "/sdcard/books") != 0) {
        char parent_path[MAX_PATH_LEN];
        strcpy(parent_path, path);
        char *last_slash = strrchr(parent_path, '/');
        if (last_slash && last_slash != parent_path) {
            *(last_slash) = '\0';
        } else {
            strcpy(parent_path, "/"); // 根目录情况
        }
        
        g_file_browser.file_items.push_back(".. (上级目录)");
        g_file_browser.file_full_paths.push_back(std::string(parent_path));
        g_file_browser.is_directory.push_back(true);
    }
    
    // 添加所有目录项到列表
    for (auto& entry : entries) {
        struct stat st;
        bool is_dir = (stat(entry.second.c_str(), &st) == 0 && S_ISDIR(st.st_mode));
        
        std::string display_name = entry.first;
        if (is_dir) {
            display_name = "[DIR] " + display_name;
        }
        printf("display_name: %s\n", display_name.c_str());
        g_file_browser.file_items.push_back(display_name);
        g_file_browser.file_full_paths.push_back(entry.second);
        g_file_browser.is_directory.push_back(is_dir);
    }
    
    // 更新UI
    if (g_file_browser.file_list_view) {
        g_file_browser.file_list_view->setItems(g_file_browser.file_items);
        g_file_browser.file_list_view->markDirty();        
    }
}

/**
 * @brief 更新标题栏显示当前路径 - 已移除标题栏
 */
static void update_title(void) {
    // 标题栏已移除，此函数为空操作
}

/**
 * @brief 文件列表项点击回调
 */
static void on_file_item_click(int index) {
    if (index < 0 || index >= (int)g_file_browser.file_full_paths.size()) {
        return;
    }
    
    const std::string& path = g_file_browser.file_full_paths[index];
    bool is_dir = g_file_browser.is_directory[index];
    
    if (is_dir) {
        // 是目录，进入该目录
        file_browser_open_directory(path.c_str());
    } else {
        // 是文件，触发文件选择回调
        ESP_LOGI(TAG, "Selected file: %s", path.c_str());        
        // 这里可以添加打开文件的逻辑
        if (file_selected_callback) {
            file_selected_callback(path.c_str());
        }
    }
}

void file_browser_init(LinearLayout* parent) {
    // 创建互斥锁
    if (!file_browser_mutex) {
        file_browser_mutex = xSemaphoreCreateMutex();
    }
    
    // 初始化当前路径
    strcpy(g_file_browser.current_path, "/sdcard/books");
    
    // 初始化UI组件
    g_file_browser.screen_layout = parent ? parent : nullptr;
    
    if (g_file_browser.screen_layout) {
        // 创建文件列表视图 - 使用全部可用高度
        int list_height = g_file_browser.screen_layout->getHeight() - 20; // 减少边距
        g_file_browser.file_list_view = new ListView(
        g_file_browser.screen_layout->getWidth() - 20, list_height);
        g_file_browser.file_list_view->setRowCount(12); // 设置显示行数     
        g_file_browser.file_list_view->setOnItemClickListener(on_file_item_click);
        g_file_browser.file_list_view->setPadding(20, 20, 20, 20); // 设置内边距
        g_file_browser.screen_layout->addChild(g_file_browser.file_list_view);
    }
    
    // 加载初始目录内容
    file_browser_open_directory(g_file_browser.current_path);
}

void file_browser_open_directory(const char * path) {
    if (!xSemaphoreTake(file_browser_mutex, portMAX_DELAY)) {
        return;
    }
    
    // 更新当前路径
    strncpy(g_file_browser.current_path, path, MAX_PATH_LEN - 1);
    g_file_browser.current_path[MAX_PATH_LEN - 1] = '\0';
    
    // 更新标题
    update_title();
    
    // 加载目录内容
    load_directory_content(g_file_browser.current_path);
    
    xSemaphoreGive(file_browser_mutex);
}

const char * file_browser_get_current_path(void) {
    if (!xSemaphoreTake(file_browser_mutex, portMAX_DELAY)) {
        return NULL;
    }
    
    static char path_copy[MAX_PATH_LEN];
    strcpy(path_copy, g_file_browser.current_path);
    
    xSemaphoreGive(file_browser_mutex);
    
    return path_copy;
}

void register_file_selected_callback(FileSelectedCallback callback) {
    file_selected_callback = callback;
}

void file_browser_deinit(void) {
    if (file_browser_mutex) {
        vSemaphoreDelete(file_browser_mutex);
        file_browser_mutex = NULL;
    }
}

/**
 * @brief 手动触发界面刷新
 */
void file_browser_force_refresh(void) {
    if (g_file_browser.screen_layout) {
        g_file_browser.screen_layout->forceRedraw();
    }
}