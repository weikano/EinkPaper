#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化文件管理器界面
 * @param parent 父对象，如果为NULL则使用当前活动屏幕
 */
void file_manager_init(lv_obj_t * parent);

/**
 * @brief 打开指定目录
 * @param path 要打开的目录路径
 */
void file_manager_open_directory(const char * path);

/**
 * @brief 获取当前所在目录
 * @return 当前目录路径字符串
 */
const char * file_manager_get_current_path(void);

#ifdef __cplusplus
}
#endif

#endif // FILE_MANAGER_H