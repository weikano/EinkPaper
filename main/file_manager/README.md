# 文件管理器模块说明

## 概述
此模块实现了针对 M5PaperS3 的文件浏览器功能，专门用于浏览 `/sdcard/books` 目录下的 `.txt` 和 `.epub` 文件。

## 功能特点

### 1. 文件过滤
- 只显示 `.txt` 和 `.epub` 文件
- 显示目录和允许的文件类型
- 跳过隐藏文件和其他类型的文件

### 2. 内存优化
- 使用 SPIRAM 分配路径字符串，避免内部 RAM 不足
- 采用批量读取和排序机制，减少内存占用
- 对大量文件进行高效处理

### 3. 用户界面
- 使用 LVGL 列表组件显示文件
- 目录和文件使用不同图标区分
- 显示当前路径
- 支持导航到上级目录

### 4. 操作方式
- 点击目录：进入该目录
- 点击文件：打印文件路径到串口
- 点击"上一级"：返回上级目录

## API 接口

### file_manager_init()
初始化文件管理器界面
```c
void file_manager_init(lv_obj_t * parent);
```

### file_manager_open_directory()
打开指定目录
```c
void file_manager_open_directory(const char * path);
```

### file_manager_get_current_path()
获取当前所在目录
```c
const char * file_manager_get_current_path(void);
```

## 技术细节

### 内存管理
- 所有路径字符串都使用 `heap_caps_malloc(..., MALLOC_CAP_SPIRAM)` 分配到 PSRAM
- 临时目录项在使用后立即释放

### 文件排序
- 目录优先显示
- 同类型项目按字母顺序排序
- 使用 `qsort()` 实现高效排序

### 安全性
- 使用互斥锁保护共享资源
- 路径长度预检查防止缓冲区溢出
- 错误处理确保稳定性

## 使用方法

在主程序中调用 `file_manager_init(NULL)` 即可初始化文件浏览器，它会自动打开 `/sdcard/books` 目录。

## 注意事项
- 确保 SD 卡正确连接并能被 ESP32-S3 识别
- 需要预先在 SD 卡上创建 `/books` 目录
- 大量文件时可能需要稍长的加载时间