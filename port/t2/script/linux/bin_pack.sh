#!/bin/bash

# 接受参数：packtools路径，构建临时路径，项目名称
pack_tools_path="$1"
build_temp_path="$2"
project_name="$3"

# 检查输入参数
if [ ! -d "${pack_tools_path}" ]; then
    echo "${pack_tools_path} not exist"
    exit 1
fi

if [ ! -d "${build_temp_path}" ]; then
    echo "${build_temp_path} not exist"
    exit 1
fi

# 切换工作路径到构建临时路径
cd "${build_temp_path}" || exit 1

# 加密 bin 文件
"${pack_tools_path}/encrypt" "${build_temp_path}/${project_name}.bin" 510fb093 a3cbeadc 5993a17e c7adeb03 10000

# 生成 package json
# 如果存在 config.json 文件，则删除
if [ -f "${build_temp_path}/config.json" ]; then
    rm "${build_temp_path}/config.json"
fi
cp "${pack_tools_path}/pack_config_def.json" "${build_temp_path}/config.json"

t2_bootloader_path="${pack_tools_path}/t2_bootloader_enc.bin"
build_app_path="${build_temp_path}/${project_name}_enc.bin"
sed -i "s#t2_bootloader_enc.bin#$t2_bootloader_path#g" ${build_temp_path}/config.json
sed -i "s#bk7231_common_1.0.1_enc.bin#$build_app_path#g" ${build_temp_path}/config.json

# 打包 bin 文件
${pack_tools_path}/beken_packager "${build_temp_path}/config.json"

# 重命名文件
mv "all_1.00.bin" "${project_name}_QIO.bin"

# 重命名文件
mv "${project_name}_enc_uart_1.00.bin" "${project_name}_UA.bin"
