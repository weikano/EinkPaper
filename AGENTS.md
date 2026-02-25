# Prompt: 基于 M5PaperS3 的模块化电子书阅读器开发

## 1. 项目背景与技术栈
请协助我开发一个基于 **M5PaperS3** 的电子书阅读器。
- **硬件**: ESP32-S3 (配备 8MB OPI PSRAM), IT8951 驱动的 540x960 墨水屏。
- **环境**: VSCode + ESP-IDF v5.x 插件。
- **核心库**: 
  - `m5stack/m5unified`: "^0.2.2" (确保版本在 0.2.0 以上)
  - `m5stack/M5GFX`: "^0.2.17" （确保版本），用于绘制基础的ui_kit
- **编程规范**: 使用 C++ 开发，`app_main` 使用 `extern "C"` 链接，代码需保持高度模块化。屏幕宽高等硬件特性需要使用M5unified来获取，不能固定。

## 2. 项目架构要求
不使用 ESP-IDF Component 形式，通过文件夹隔离功能模块，并在 `main/CMakeLists.txt` 中手动管理。参考目录结构如下：
- `main/gestures/`: 手势相关操作， 包括滑动、点击等。
- `main/ui_kit/`: UI 管理层 (负责刷新回调）
- `main/hal/`: 硬件抽象层 (负责与 ESP32-S3 交互，包括内存管理、墨水屏驱动等）。
- `main/page/`: 电子书页面管理，类似`Android`的`Activity`
- `main/main.cpp`: 入口点，协调各模块初始化。

## 3. 核心技术要求
### A. 内存管理 (Memory Optimization)
- **PSRAM 优先**: 必须显式使用 `MALLOC_CAP_SPIRAM`。

### B. 墨水屏优化 (E-Ink Optimization)
- **刷新频率**: 针对墨水屏特性，刷新调用间隔需增大（建议 20ms-100ms），降低 CPU 占用。
- **禁用动画**: 在样式中全局禁用所有位移动画，防止产生严重残影。
- **局部刷新**: 仅刷新改变的区域，避免全屏刷新。
- **双缓冲**: 不直接使用`M5GFX`的绘制屏幕， 而是使用`M5Canvas`来绘制 。

## 4. UI KIT
### 参考`Android`的`View`体系，实现对应的`View.h`, `ViewGroup.h`，`TextView.h`，`ProgressBar.h`，`Button.h`，`ImageView.h`等。
- **Button**: 支持点击、长按、滑动等手势。
- **TextView**: 用于显示电子书内容。
- **ProgressBar**: 显示当前阅读进度。
- **ImageView**: 用于显示图片（如封面）。