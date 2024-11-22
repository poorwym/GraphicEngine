#!/bin/bash

# 定义目标目录和输出文件
SRC_DIR="./src"
OUTPUT_FILE="files.txt"

# 检查 src 目录是否存在
if [ ! -d "$SRC_DIR" ]; then
    echo "目录 $SRC_DIR 不存在，请确认路径！"
    exit 1
fi

# 清空或创建输出文件
> "$OUTPUT_FILE"

# 查找 src 目录下的 .cpp 和 .h 文件（不包括子目录）
FILES=$(find "$SRC_DIR" -maxdepth 1 \( -name "*.cpp" -o -name "*.h" \))

# 遍历文件并将内容写入输出文件
for FILE in $FILES; do
    echo ">>> 文件: $FILE <<<" >> "$OUTPUT_FILE"

    # 动态转换文件编码为 UTF-8
    file_encoding=$(file -I "$FILE" | sed -n 's/.*charset=\(.*\)$/\1/p')
    if [ "$file_encoding" != "utf-8" ]; then
        iconv -f "$file_encoding" -t utf-8 "$FILE" >> "$OUTPUT_FILE" 2>/dev/null || echo "（无法转换文件编码：$FILE）" >> "$OUTPUT_FILE"
    else
        cat "$FILE" >> "$OUTPUT_FILE"
    fi

    echo -e "\n\n" >> "$OUTPUT_FILE" # 添加换行分隔符
done

# 提示完成
echo "已将 $SRC_DIR 下的 .cpp 和 .h 文件内容写入 $OUTPUT_FILE"