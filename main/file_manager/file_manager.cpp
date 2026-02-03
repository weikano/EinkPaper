#include "file_manager.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <cstring>
#include <cstdlib>

static const char *TAG = "FileManager";

// 最大路径长度
#define MAX_PATH_LEN 512
// 每次读取的目录项数量
#define DIR_READ_BATCH_SIZE 50
// 显示的文件名最大长度
#define MAX_DISPLAY_NAME_LEN 64

// 文件管理器状态结构
typedef struct {
    char current_path[MAX_PATH_LEN];
    lv_obj_t *screen;
    lv_obj_t *list;
    lv_obj_t *title_label;
} file_manager_t;

static file_manager_t g_file_manager = {0};
static SemaphoreHandle_t file_manager_mutex = NULL;

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
 * @brief 在列表中添加项目
 * @param list 列表对象
 * @param item_name 项目名称
 * @param path 项目完整路径
 */
static void add_list_item(lv_obj_t *list, const char *item_name, const char *path) {
    struct stat st;
    bool is_dir = (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
    
    lv_obj_t *btn = lv_list_add_btn(list, is_dir ? LV_SYMBOL_DIRECTORY : LV_SYMBOL_FILE, item_name);
    
    // 存储路径信息到按钮用户数据
    char *path_copy = (char*)heap_caps_malloc(strlen(path) + 1, MALLOC_CAP_SPIRAM);
    if (path_copy) {
        strcpy(path_copy, path);
        lv_obj_set_user_data(btn, path_copy);
        
        // 添加点击事件
        lv_obj_add_event_cb(btn, [](lv_event_t *e) {
            lv_obj_t *target_btn = static_cast<lv_obj_t *>(lv_event_get_target(e));
            const char *path = (const char*)lv_obj_get_user_data(target_btn);
            
            if (path) {
                struct stat st;
                if (stat(path, &st) == 0) {
                    if (S_ISDIR(st.st_mode)) {
                        // 是目录，进入该目录
                        file_manager_open_directory(path);
                    } else {
                        // 是文件，打印路径
                        printf("Selected file: %s\n", path);
                        // 这里可以添加打开文件的逻辑
                    }
                }
            }
        }, LV_EVENT_CLICKED, NULL);
    }
}

/**
 * @brief 比较函数，用于排序目录项（目录在前，文件在后，按字母顺序）
 */
static int compare_entries(const void *a, const void *b) {
    const struct dirent **da = (const struct dirent **)a;
    const struct dirent **db = (const struct dirent **)b;
    
    struct stat st_a, st_b;
    char path_a[MAX_PATH_LEN], path_b[MAX_PATH_LEN];
    
    // 使用更安全的方式拼接路径
    int len_a = strlen(g_file_manager.current_path);
    int len_da = strlen((*da)->d_name);
    if (len_a + 1 + len_da >= MAX_PATH_LEN) {
        // 路径太长，使用文件名比较
        return strcmp((*da)->d_name, (*db)->d_name);
    }
    
    int len_b = strlen(g_file_manager.current_path);
    int len_db = strlen((*db)->d_name);
    if (len_b + 1 + len_db >= MAX_PATH_LEN) {
        // 路径太长，使用文件名比较
        return strcmp((*da)->d_name, (*db)->d_name);
    }
    
    strcpy(path_a, g_file_manager.current_path);
    strcat(path_a, "/");
    strcat(path_a, (*da)->d_name);
    
    strcpy(path_b, g_file_manager.current_path);
    strcat(path_b, "/");
    strcat(path_b, (*db)->d_name);
    
    bool is_dir_a = (stat(path_a, &st_a) == 0 && S_ISDIR(st_a.st_mode));
    bool is_dir_b = (stat(path_b, &st_b) == 0 && S_ISDIR(st_b.st_mode));
    
    // 目录优先于文件
    if (is_dir_a && !is_dir_b) return -1;
    if (!is_dir_a && is_dir_b) return 1;
    
    // 同类型按名称排序
    return strcmp((*da)->d_name, (*db)->d_name);
}

/**
 * @brief 加载目录内容到列表（优化版，支持大量文件）
 * @param path 目录路径
 */
static void load_directory_content(const char *path) {
    DIR *dir;
    struct dirent *entry;
    
    // 清空现有列表
    lv_obj_clean(g_file_manager.list);
    
    dir = opendir(path);
    if (dir == NULL) {
        ESP_LOGE(TAG, "Cannot open directory: %s", path);
        return;
    }
    
    // 收集所有符合条件的目录项
    struct dirent **entries = NULL;
    int entry_count = 0;
    int allocated_size = 0;
    
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
                // 扩展数组大小
                if (entry_count >= allocated_size) {
                    allocated_size += 50; // 每次增加50个槽位
                    entries = (struct dirent**)realloc(entries, allocated_size * sizeof(struct dirent*));
                }
                
                // 复制目录项信息
                entries[entry_count] = (struct dirent*)malloc(sizeof(struct dirent));
                memcpy(entries[entry_count], entry, sizeof(struct dirent));
                entry_count++;
            }
        }
    }
    
    closedir(dir);
    
    // 排序目录项
    if (entries && entry_count > 0) {
        qsort(entries, entry_count, sizeof(struct dirent*), compare_entries);
    }
    
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
        
        lv_obj_t *parent_btn = lv_list_add_btn(g_file_manager.list, LV_SYMBOL_NEW_LINE " ..", "上一级");
        char *path_copy = (char*)heap_caps_malloc(strlen(parent_path) + 1, MALLOC_CAP_SPIRAM);
        if (path_copy) {
            strcpy(path_copy, parent_path);
            lv_obj_set_user_data(parent_btn, path_copy);
            
            lv_obj_add_event_cb(parent_btn, [](lv_event_t *e) {
                lv_obj_t *target_btn = static_cast<lv_obj_t *>(lv_event_get_target(e));
                const char *path = (const char*)lv_obj_get_user_data(target_btn);
                
                if (path) {
                    file_manager_open_directory(path);
                }
            }, LV_EVENT_CLICKED, NULL);
        }
    }
    
    // 添加所有目录项到列表
    for (int i = 0; i < entry_count; i++) {
        // 检查路径长度以避免溢出
        int len_path = strlen(path);
        int len_entry = strlen(entries[i]->d_name);
        if (len_path + 1 + len_entry >= MAX_PATH_LEN) {
            continue; // 跳过过长的路径
        }
        
        char full_path[MAX_PATH_LEN];
        strcpy(full_path, path);
        strcat(full_path, "/");
        strcat(full_path, entries[i]->d_name);
        add_list_item(g_file_manager.list, entries[i]->d_name, full_path);
        
        // 释放临时存储的目录项
        free(entries[i]);
    }
    
    // 释放目录项数组
    if (entries) {
        free(entries);
    }
}

/**
 * @brief 更新标题栏显示当前路径
 */
static void update_title(void) {
    if (g_file_manager.title_label) {
        lv_label_set_text_fmt(g_file_manager.title_label, "路径: %s", g_file_manager.current_path);
    }
}

void file_manager_init(lv_obj_t * parent) {
    // 创建互斥锁
    if (!file_manager_mutex) {
        file_manager_mutex = xSemaphoreCreateMutex();
    }
    
    // 初始化当前路径
    strcpy(g_file_manager.current_path, "/sdcard/books");
    
    // 创建屏幕容器
    g_file_manager.screen = parent ? parent : lv_screen_active();
    
    // 创建标题标签
    g_file_manager.title_label = lv_label_create(g_file_manager.screen);
    lv_obj_set_width(g_file_manager.title_label, lv_obj_get_width(g_file_manager.screen));
    lv_obj_set_style_text_align(g_file_manager.title_label, LV_TEXT_ALIGN_CENTER, 0);
    update_title();
    
    // 创建列表
    g_file_manager.list = lv_list_create(g_file_manager.screen);
    lv_obj_set_size(g_file_manager.list, lv_obj_get_width(g_file_manager.screen), 
                   lv_obj_get_height(g_file_manager.screen) - lv_obj_get_height(g_file_manager.title_label) - 10);
    lv_obj_align_to(g_file_manager.list, g_file_manager.title_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    
    // 打开初始目录
    file_manager_open_directory(g_file_manager.current_path);
}

void file_manager_open_directory(const char * path) {
    if (!xSemaphoreTake(file_manager_mutex, portMAX_DELAY)) {
        return;
    }
    
    // 更新当前路径
    strncpy(g_file_manager.current_path, path, MAX_PATH_LEN - 1);
    g_file_manager.current_path[MAX_PATH_LEN - 1] = '\0';
    
    // 更新标题
    update_title();
    
    // 加载目录内容
    load_directory_content(g_file_manager.current_path);
    
    xSemaphoreGive(file_manager_mutex);
}

const char * file_manager_get_current_path(void) {
    if (!xSemaphoreTake(file_manager_mutex, portMAX_DELAY)) {
        return NULL;
    }
    
    static char path_copy[MAX_PATH_LEN];
    strcpy(path_copy, g_file_manager.current_path);
    
    xSemaphoreGive(file_manager_mutex);
    
    return path_copy;
}