#!/bin/bash

# 检查参数数量
if [ "$#" -lt 3 ]; then
    echo "用法: $0 <字体路径> <字号> <输出名称>"
    echo "示例: $0 ./FZHeiti.ttf 16 my_font_fz"
    exit 1
fi

# 获取输入参数
FONT_PATH=$1
FONT_SIZE=$2
OUTPUT_NAME=$3

# 检查字体文件是否存在
if [ ! -f "$FONT_PATH" ]; then
    echo "错误: 找不到字体文件 '$FONT_PATH'"
    exit 1
fi

echo "正在转换字体: $FONT_PATH (大小: $FONT_SIZE)..."

# 执行转换
# --bpp 1: 针对墨水屏优化，保持笔画锐利
# --range: 包含常用 ASCII 和 常用中文字符集
lv_font_conv \
  --font "$FONT_PATH" \
  --size "$FONT_SIZE" \
  --format lvgl \
  --bpp 1 \
  --no-compress \
  --no-prefilter \
  --symbols "确定取消关闭" \
  --range 0x20-0x7E \
  --range 0x4E00-0x9FA5 \
  --output "${OUTPUT_NAME}.c" \
  --lv-font-name "$OUTPUT_NAME"

if [ $? -eq 0 ]; then
    echo "--------------------------------------"
    echo "转换成功！"
    echo "生成文件: ${OUTPUT_NAME}.c"
    echo "代码引用: LV_FONT_DECLARE($OUTPUT_NAME);"
    echo "将 ${OUTPUT_NAME}.c 复制到main/fonts下，在main/CMakeLists.txt中添加对应的源文件"
    echo "--------------------------------------"
else
    echo "转换失败，请检查是否安装了 lv_font_conv (npm install -g lv_font_conv)"
fi