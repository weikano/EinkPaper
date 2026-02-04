#!/bin/bash

# --- 参数校验 ---
if [ "$#" -ne 3 ]; then
    echo "用法: $0 <字体文件.ttf> <字号> <输出文件名.vlw>"
    echo "示例: $0 Alipuhui.ttf 24 myfont.vlw"
    exit 1
fi

INPUT_TTF=$1
FONT_SIZE=$2
OUTPUT_VLW=$3
TEMP_CHARS="temp_8000_chars.txt"

# 检查输入文件是否存在
if [ ! -f "$INPUT_TTF" ]; then
    echo "错误: 找不到字体文件 '$INPUT_TTF'"
    exit 1
fi

echo "--------------------------------------------"
echo "输入字体: $INPUT_TTF"
echo "目标字号: $FONT_SIZE px"
echo "输出文件: $OUTPUT_VLW"
echo "--------------------------------------------"

# --- Step 1: 生成 8000 字精简字符集 ---
echo "正在提取常用字符集 (ASCII + 通用规范汉字)..."

python3 <<EOF > $TEMP_CHARS
import sys

# 1. 基础 ASCII (32-126)
chars = [chr(i) for i in range(32, 127)]

# 2. 常用标点符号
punctuations = "，。！？；：、“”（）《》【】—…·"
chars.extend(list(punctuations))

# 3. 核心汉字区 (涵盖通用规范汉字表常用区间)
# 这里取 CJK 统一汉字区的前 8000 个字，涵盖了 99.9% 的日常文件名
for i in range(0x4E00, 0x4E00 + 8000):
    chars.append(chr(i))

# 去重并打印
print("".join(dict.fromkeys(chars)))
EOF

# --- Step 2: 调用 otf2vlw 转换 ---
echo "开始转换 VLW 格式..."

# 运行转换命令
otf2vlw "$INPUT_TTF" "$FONT_SIZE" "$OUTPUT_VLW" -c "$TEMP_CHARS"

# --- Step 3: 结果处理 ---
if [ $? -eq 0 ]; then
    FILE_SIZE=$(ls -lh "$OUTPUT_VLW" | awk '{print $5}')
    echo "Success! 生成成功。"
    echo "文件大小: $FILE_SIZE"
    echo "提示: 请将 $OUTPUT_VLW 放入 ESP32 的 SPIFFS/SD 卡中。"
else
    echo "错误: 转换过程中出现问题。请确保已安装 'pip install otf2vlw'。"
fi

# 清理临时文件
rm -f "$TEMP_CHARS"