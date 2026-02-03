# SD卡功能模块说明

## 概述
此模块实现了针对 M5PaperS3 的SD卡挂载和管理功能，专门用于电子书阅读器的文件存储需求。

## 功能特点

### 1. 自动挂载
- 自动检测并挂载SD卡到 `/sdcard` 目录
- 自动创建 `/sdcard/books` 专用目录

### 2. 容量管理
- 获取SD卡总容量和可用容量
- 提供容量信息供上层应用使用

### 3. 安全卸载
- 提供安全卸载功能，防止数据损坏

### 4. 状态监控
- 提供挂载状态查询功能

## API 接口

### sdcard_init()
初始化SD卡系统
```c
esp_err_t sdcard_init(void);
```

### sdcard_mount()
挂载SD卡
```c
esp_err_t sdcard_mount(void);
```

### sdcard_unmount()
卸载SD卡
```c
esp_err_t sdcard_unmount(void);
```

### sdcard_is_mounted()
检查SD卡是否已挂载
```c
bool sdcard_is_mounted(void);
```

### sdcard_get_total_bytes()
获取SD卡总容量（字节）
```c
uint64_t sdcard_get_total_bytes(void);
```

### sdcard_get_free_bytes()
获取SD卡可用容量（字节）
```c
uint64_t sdcard_get_free_bytes(void);
```

### sdcard_ensure_books_dir()
确保书籍目录存在
```c
esp_err_t sdcard_ensure_books_dir(void);
```

## 预定义常量

- `SDCARD_MOUNT_POINT` = "/sdcard"
- `SDCARD_BOOKS_DIR` = "/sdcard/books"

## 技术细节

### 引脚配置
- CLK: GPIO 11
- CMD: GPIO 12
- D0: GPIO 13
- D1: GPIO 14
- D2: GPIO 15
- D3: GPIO 16

### 参数配置
- 4线模式传输
- 最大文件数：8
- 分配单元大小：16KB

## 使用方法

在主程序中依次调用：
1. `sdcard_init()` - 初始化SD卡系统
2. `sdcard_mount()` - 挂载SD卡
3. 使用SD卡功能
4. `sdcard_unmount()` - 程序结束前安全卸载

## 注意事项
- 确保SD卡物理连接正确
- 挂载前应先调用初始化
- 使用完毕后应安全卸载
- 遵循ESP32-S3的电源管理规范