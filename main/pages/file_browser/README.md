# 文件浏览器模块说明

## 概述
此模块实现了针对 M5PaperS3 的文件浏览器功能，专门用于浏览 `/sdcard/books` 目录下的 `.txt`、`.epub`、`.pdf` 和 `.mobi` 文件。

## 功能特点

### 1. 文件过滤
- 只显示 `.txt`、`.epub`、`.pdf` 和 `.mobi` 文件
- 显示目录和允许的文件类型
- 跳过隐藏文件和其他类型的文件

### 2. 内存优化
- 使用 SPIRAM 分配路径字符串，避免内部 RAM 不足
- 采用批量读取和排序机制，减少内存占用
- 对大量文件进行高效处理

### 3. 用户界面
- 使用 UI Kit 列表组件显示文件
- 目录和文件使用不同标识区分
- 支持返回上级目录功能

## API 接口

### file_browser_init
初始化文件浏览器界面

### file_browser_open_directory
打开指定目录

### file_browser_get_current_path
获取当前所在目录

### register_file_selected_callback
注册文件选择回调函数

### file_browser_deinit
释放文件浏览器资源

### file_browser_force_refresh
手动触发界面刷新