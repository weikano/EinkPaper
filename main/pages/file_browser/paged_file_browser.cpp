#include "paged_file_browser.h"
#include "ui_kit/UIKIT.h"
#include "ui_kit/PagedListView.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>

static const char *TAG = "PagedFileBrowser";

// 最大路径长度
#define MAX_PATH_LEN 512
// 每页显示的项目数量
#define PAGE_SIZE 6
// 显示的文件名最大长度
#define MAX_DISPLAY_NAME_LEN 64

// 分页文件浏览器状态结构
typedef struct {
    char current_path[MAX_PATH_LEN];
    LinearLayout* screen_layout;
    PagedListView* file_paged_list_view;
    TextView* title_view;
    std::vector<std::string> all_file_items;          // 存储所有文件/目录项
    std::vector<std::string> all_file_full_paths;     // 存储所有完整路径
    std::vector<bool> all_is_directory;               // 标记是否为目录
} paged_file_browser_t;

static paged_file_browser_t g_paged_file_browser = {};
static SemaphoreHandle_t paged_file_browser_mutex = NULL;

// 回调函数指针定义
static FileSelectedCallback paged_file_selected_callback = nullptr;
static void (*back_callback)(void) = nullptr;

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
    g_paged_file_browser.all_file_items.clear();
    g_paged_file_browser.all_file_full_paths.clear();
    g_paged_file_browser.all_is_directory.clear();
    
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
        
        g_paged_file_browser.all_file_items.push_back(".. (上级目录)");
        g_paged_file_browser.all_file_full_paths.push_back(std::string(parent_path));
        g_paged_file_browser.all_is_directory.push_back(true);
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
        g_paged_file_browser.all_file_items.push_back(display_name);
        g_paged_file_browser.all_file_full_paths.push_back(entry.second);
        g_paged_file_browser.all_is_directory.push_back(is_dir);
    }
    
    // 如果分页列表视图已存在，通知它重新加载数据
    if (g_paged_file_browser.file_paged_list_view) {
        g_paged_file_browser.file_paged_list_view->refreshData();
        g_paged_file_browser.file_paged_list_view->markDirty();
    }
}

/**
 * @brief 更新标题栏显示当前路径
 */
static void update_title(void) {
    if (g_paged_file_browser.title_view) {
        std::string title = "文件浏览器 - ";
        title += g_paged_file_browser.current_path;
        g_paged_file_browser.title_view->setText(title.c_str());
        // 标题内容变化，需要重绘
        g_paged_file_browser.title_view->markDirty();
    }
}

/**
 * @brief 文件列表项点击回调
 */
static void on_file_item_click(int index) {
    if (index < 0 || index >= (int)g_paged_file_browser.all_file_full_paths.size()) {
        return;
    }
    
    const std::string& path = g_paged_file_browser.all_file_full_paths[index];
    bool is_dir = g_paged_file_browser.all_is_directory[index];
    
    if (is_dir) {
        // 是目录，进入该目录
        paged_file_browser_open_directory(path.c_str());
    } else {
        // 是文件，触发文件选择回调
        ESP_LOGI(TAG, "Selected file: %s", path.c_str());        
        // 这里可以添加打开文件的逻辑
        if (paged_file_selected_callback) {
            paged_file_selected_callback(path.c_str());
        }
    }
}

/**
 * @brief 数据源加载器回调
 */
static std::vector<std::string> data_source_loader(int page, int pageSize) {
    std::vector<std::string> result;
    
    int startIndex = page * pageSize;
    int endIndex = startIndex + pageSize;
    
    if (startIndex < (int)g_paged_file_browser.all_file_items.size()) {
        endIndex = std::min(endIndex, (int)g_paged_file_browser.all_file_items.size());
        
        for (int i = startIndex; i < endIndex; i++) {
            result.push_back(g_paged_file_browser.all_file_items[i]);
        }
    }
    
    return result;
}

/**
 * @brief 项目渲染器回调
 */
static void item_renderer(m5gfx::M5GFX& display, int index, const std::string& item, 
                         int16_t x, int16_t y, int16_t width, int16_t height) {
    // 确保尺寸有效
    if (width <= 0 || height <= 0) {
        return;
    }
    
    // // 绘制项目背景
    // display.fillRect(x, y, width, height, TFT_WHITE);
    // display.drawRect(x, y, width, height, TFT_BLACK);
    
    // 绘制项目文本
    display.setTextColor(TFT_BLACK);
    display.setTextSize(1.5);
    
    // 简单的文本绘制，带省略号处理
    std::string text = item;
    int16_t text_width = display.textWidth(text.c_str());
    int16_t max_width = width - 10; // 考虑内边距
    
    if (text_width > max_width && max_width > 0) {
        std::string ellipsis = "...";
        int16_t ellipsis_width = display.textWidth(ellipsis.c_str());
        
        int left = 0;
        int right = text.length();
        std::string result = ellipsis;
        
        while (left <= right && max_width > ellipsis_width) {
            int mid = left + (right - left) / 2;
            std::string substr = text.substr(0, mid);
            std::string candidate = substr + ellipsis;
            
            if (display.textWidth(candidate.c_str()) <= max_width) {
                result = candidate;
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        
        text = result;
    }
    
    int16_t textX = x + 5;
    int16_t textY = y + (height - display.fontHeight()) / 2;
    printf("Rendering item: %s at (%d, %d, %d, %d)\n", text.c_str(), textX, textY, width, height);
    display.setCursor(textX, textY);
    display.print(text.c_str());
}

void paged_file_browser_init(LinearLayout* parent) {
    // 创建互斥锁
    if (!paged_file_browser_mutex) {
        paged_file_browser_mutex = xSemaphoreCreateMutex();
    }
    
    // 初始化当前路径
    strcpy(g_paged_file_browser.current_path, "/sdcard/books");
    
    // 初始化UI组件
    g_paged_file_browser.screen_layout = parent ? parent : nullptr;
    
    if (g_paged_file_browser.screen_layout) {
        // 创建标题视图
        g_paged_file_browser.title_view = new TextView(
            g_paged_file_browser.screen_layout->getWidth() - 20, 40);
        g_paged_file_browser.title_view->setTextColor(TFT_BLACK);
        g_paged_file_browser.title_view->setTextSize(2);
        g_paged_file_browser.title_view->setTextAlign(1); // 居中对齐        
        g_paged_file_browser.title_view->setPadding(10, 10, 10, 10); // 设置内边距
        g_paged_file_browser.screen_layout->addChild(g_paged_file_browser.title_view);
        
        // 创建分页文件列表视图
        int list_height = g_paged_file_browser.screen_layout->getHeight() - 70;
        g_paged_file_browser.file_paged_list_view = new PagedListView(
            g_paged_file_browser.screen_layout->getWidth() - 20, list_height);
        
        // 设置每页显示3行2列（共6个项目）
        g_paged_file_browser.file_paged_list_view->setRowCount(12);
        g_paged_file_browser.file_paged_list_view->setColumnCount(1);
        
        // 设置数据源加载器
        g_paged_file_browser.file_paged_list_view->setDataSourceLoader(data_source_loader);
        
        // 设置项目渲染器
        g_paged_file_browser.file_paged_list_view->setItemRenderer(item_renderer);
        
        // 设置项目点击监听器
        g_paged_file_browser.file_paged_list_view->setOnItemClickListener(on_file_item_click);
        
        g_paged_file_browser.file_paged_list_view->setPadding(20, 20, 20, 20); // 设置内边距
        g_paged_file_browser.screen_layout->addChild(g_paged_file_browser.file_paged_list_view);
    }
    
    // 加载初始目录内容
    load_directory_content(g_paged_file_browser.current_path);
    
    // 更新标题
    update_title();
}

void paged_file_browser_open_directory(const char * path) {
    if (!xSemaphoreTake(paged_file_browser_mutex, portMAX_DELAY)) {
        return;
    }
    
    // 更新当前路径
    strncpy(g_paged_file_browser.current_path, path, MAX_PATH_LEN - 1);
    g_paged_file_browser.current_path[MAX_PATH_LEN - 1] = '\0';
    
    // 更新标题
    update_title();
    
    // 加载目录内容
    load_directory_content(g_paged_file_browser.current_path);
    
    xSemaphoreGive(paged_file_browser_mutex);
}

const char * paged_file_browser_get_current_path(void) {
    if (!xSemaphoreTake(paged_file_browser_mutex, portMAX_DELAY)) {
        return NULL;
    }
    
    static char path_copy[MAX_PATH_LEN];
    strcpy(path_copy, g_paged_file_browser.current_path);
    
    xSemaphoreGive(paged_file_browser_mutex);
    
    return path_copy;
}

void register_paged_file_selected_callback(FileSelectedCallback callback) {
    paged_file_selected_callback = callback;
}

void paged_file_browser_deinit(void) {
    if (paged_file_browser_mutex) {
        vSemaphoreDelete(paged_file_browser_mutex);
        paged_file_browser_mutex = NULL;
    }
}

/**
 * @brief 手动触发界面刷新
 */
void paged_file_browser_force_refresh(void) {
    if (g_paged_file_browser.screen_layout) {
        g_paged_file_browser.screen_layout->forceRedraw();
    }
}

/**
 * @brief 跳转到下一页
 */
bool paged_file_browser_next_page(void) {
    if (g_paged_file_browser.file_paged_list_view) {
        return g_paged_file_browser.file_paged_list_view->nextPage();
    }
    return false;
}

/**
 * @brief 跳转到上一页
 */
bool paged_file_browser_prev_page(void) {
    if (g_paged_file_browser.file_paged_list_view) {
        return g_paged_file_browser.file_paged_list_view->prevPage();
    }
    return false;
}

/**
 * @brief 获取总页数
 */
int paged_file_browser_get_total_pages(void) {
    if (g_paged_file_browser.file_paged_list_view) {
        return g_paged_file_browser.file_paged_list_view->getTotalPages();
    }
    return 0;
}

/**
 * @brief 获取当前页码
 */
int paged_file_browser_get_current_page(void) {
    if (g_paged_file_browser.file_paged_list_view) {
        return g_paged_file_browser.file_paged_list_view->getCurrentPage();
    }
    return 0;
}

/**
 * @brief 设置返回按钮点击回调
 */
void set_paged_file_browser_back_callback(void (*callback)(void)) {
    back_callback = callback;
    // 如果PagedListView已经初始化，设置其返回回调
    if (g_paged_file_browser.file_paged_list_view) {
        g_paged_file_browser.file_paged_list_view->setOnBackCallback([callback]() {
            if (callback) {
                callback();
            }
        });
    }
}