#coding=utf-8

import os
import sys
import shutil
import argparse
import zipfile
import hashlib

zip_file_name = "arduino_tuya_open"
zip_file_suffix = ".zip"

def zip_folder(folder_path, output_path):
    # 创建一个ZipFile对象，并设置模式为写，指定压缩方式
    with zipfile.ZipFile(output_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
        # os.walk遍历文件夹
        for root, dirs, files in os.walk(folder_path):
            for file in files:
                # 创建文件的绝对路径
                file_path = os.path.join(root, file)
                # 添加文件到zip文件中
                # arcname参数去掉文件路径的部分，只保留从folder_path之后的路径
                zipf.write(file_path, arcname=os.path.relpath(file_path, os.path.dirname(folder_path)))

def get_sha256_hash(file_path):
    """ 计算文件的SHA-256哈希值 """
    sha256_hash = hashlib.sha256()
    with open(file_path, "rb") as f:
        # 读取并更新哈希字符串，使用较小的块大小防止大文件占用太多内存
        for byte_block in iter(lambda: f.read(4096), b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()

parser = argparse.ArgumentParser(description='Package arduino-tuya-open')
parser.add_argument('--input', type=str, required=True, help='arduino tuya open path')
parser.add_argument('--output', type=str, required=True, help='output path')
parser.add_argument('--version', type=str, required=True, help='version')

args = parser.parse_args()
input_path = args.input
output_path = args.output
version = args.version

# 路径改为绝对路径
input_path = os.path.abspath(input_path)
output_path = os.path.abspath(output_path)

input_path = os.path.normpath(input_path)
output_path = os.path.normpath(output_path)

print('input_path:', input_path)
print('output_path:', output_path)

# 创建输出目录
if os.path.exists(output_path):
    shutil.rmtree(output_path)
os.makedirs(output_path)

# 复制文件夹
copy_folder = os.path.join(output_path, 'tuya_open')
shutil.copytree(input_path, copy_folder, dirs_exist_ok=True)

# 删除不需要的文件
## git
shutil.rmtree(os.path.join(copy_folder, '.git'))
os.remove(os.path.join(copy_folder, '.gitignore'))
os.remove(os.path.join(copy_folder, '.gitmodules'))
## ArduinoCore-API
shutil.rmtree(os.path.join(copy_folder, 'ArduinoCore-API', '.github'))
os.remove(os.path.join(copy_folder, 'ArduinoCore-API', '.git'))
os.remove(os.path.join(copy_folder, 'ArduinoCore-API', '.gitignore'))
os.remove(os.path.join(copy_folder, 'ArduinoCore-API', '.codespellrc'))
os.remove(os.path.join(copy_folder, 'ArduinoCore-API', 'README.md'))
shutil.rmtree(os.path.join(copy_folder, 'ArduinoCore-API', 'test'))
## script
shutil.rmtree(os.path.join(copy_folder, 'script'))
## package.json
os.remove(os.path.join(copy_folder, 'package.json'))
os.remove(os.path.join(copy_folder, 'package_cn.json'))
## CMakeLists.txt
os.remove(os.path.join(copy_folder, 'CMakeLists.txt'))

# 压缩文件夹
need_zip_folder = os.path.join(output_path, 'tuya_open')
ouput_zip = os.path.join(output_path, zip_file_name + '-' + version + zip_file_suffix)
zip_folder(need_zip_folder, ouput_zip)

print('zip file:', ouput_zip)

# 计算 zip 文件大小和 SHA256 值
print('zip size:', os.path.getsize(ouput_zip))

zip_sha256 = get_sha256_hash(ouput_zip)
print('zip sha256:', zip_sha256)

