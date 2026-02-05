#ifndef PAGED_FILE_BROWSER_H
#define PAGED_FILE_BROWSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include <algorithm>

// 前向声明UI组件类
class LinearLayout;
class PagedListView;

#ifdef __cplusplus
extern "C" {
#endif

// 文件选择回调函数类型定义
typedef void (*FileSelectedCallback)(const char* filepath);

/**
 * @brief 初始化分页文件浏览器界面
 * @param parent 父布局对象，如果为NULL则创建新的根布局
 */
void paged_file_browser_init(LinearLayout* parent);

/**
 * @brief 打开指定目录
 * @param path 要打开的目录路径
 */
void paged_file_browser_open_directory(const char * path);

/**
 * @brief 获取当前所在目录
 * @return 当前目录路径字符串
 */
const char * paged_file_browser_get_current_path(void);

/**
 * @brief 注册文件选择回调函数
 * @param callback 文件选择时的回调函数
 */
void register_paged_file_selected_callback(FileSelectedCallback callback);

/**
 * @brief 释放分页文件浏览器资源
 */
void paged_file_browser_deinit(void);

/**
 * @brief 手动触发界面刷新
 */
void paged_file_browser_force_refresh(void);

/**
 * @brief 跳转到下一页
 */
bool paged_file_browser_next_page(void);

/**
 * @brief 跳转到上一页
 */
bool paged_file_browser_prev_page(void);

/**
 * @brief 获取总页数
 */
int paged_file_browser_get_total_pages(void);

/**
 * @brief 获取当前页码
 */
int paged_file_browser_get_current_page(void);

/**
 * @brief 设置返回按钮点击回调
 */
void set_paged_file_browser_back_callback(void (*callback)(void));

#ifdef __cplusplus
}
#endif

#endif // PAGED_FILE_BROWSER_H