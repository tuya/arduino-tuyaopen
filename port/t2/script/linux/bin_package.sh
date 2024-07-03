#!/bin/bash

# 关闭命令回显
set +x

# 检查是否提供了参数
if [ -z "$1" ]; then
    echo "错误: 请提供编译器的绝对路径作为第一个参数。"
    exit 1
fi

if [ -z "$2" ]; then
    echo "错误: 请提供 .axf 文件的绝对路径作为第二个参数。"
    exit 1
fi

if [ -z "$3" ]; then
    echo "错误: 请提供输出文件路径作为第三个参数。"
    exit 1
fi

# 获取参数
COMPILER_PATH="$1"
AXF_PATH="$2"
OUTPUT_PATH="$3"

# 打印路径信息
echo "编译器的绝对路径: $COMPILER_PATH"
echo ".axf 文件的绝对路径: $AXF_PATH"
echo "输出文件路径: $OUTPUT_PATH"

# 从 AXF_PATH 中提取文件名
AXF_NAME=$(basename "$AXF_PATH" .axf)
echo "AXF 文件名: $AXF_NAME"

# .axf -> .bin
"$COMPILER_PATH"arm-none-eabi-objcopy -O binary "$AXF_PATH" "${OUTPUT_PATH}/${AXF_NAME}.bin"

# 执行 bin_pack.sh, 参数一当前路径，参数二输出文件名称，无后缀
"${0%/*}/bin_pack.sh" "${0%/*}" "$OUTPUT_PATH" "$AXF_NAME"

# 结束脚本
exit 0