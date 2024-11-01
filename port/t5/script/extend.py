#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# extend.py
# @desc: t5 extend script, package all the files to a bin file, and generate ota bin

import os
import sys
import argparse
import shutil
import json
import logging
import subprocess

logging.basicConfig(
    level=logging.INFO,
    format='[%(levelname)s][%(filename)s:%(lineno)d]%(message)s'
)

#
## 将当前脚本所在路径添加为搜索模块的路径
#
scriptSelfPath = os.path.dirname(os.path.realpath(__file__))
logging.debug("scriptSelfPath: " + scriptSelfPath)
sys.path.append(scriptSelfPath)

from appVersionParse import *

# 获取当前Python解释器路径
pythonExecutable = sys.executable
pythonExecutable = os.path.normpath(pythonExecutable)
logging.debug("pythonExecutable: " + pythonExecutable)

parser = argparse.ArgumentParser()
parser.add_argument('--compiler-path', type=str, help='compiler path')
parser.add_argument('--port-path', type=str, help='port path')
parser.add_argument('--output-path', type=str, help='output path')
parser.add_argument('--output-name', type=str, help='output bin file name')

args = parser.parse_args()
compilerPath = args.compiler_path
portPath = args.port_path
outputPath = args.output_path

outputName = args.output_name

# Normalize file paths
compilerPath = os.path.normpath(compilerPath)
portPath = os.path.normpath(portPath)
outputPath = os.path.normpath(outputPath)

# Get binary version
appConfigTmpFile = os.path.join(outputPath, 'tuyaTmp', 'appConfig', 'appConfig.json')
appVersion = appVersionParse(appConfigTmpFile)

# output file path
binFilePath = os.path.join(outputPath, 'tuyaTmp', 'output', appVersion)

# delete binFilePath
def deleteDirectoryContents(directory_path):
    for item in os.listdir(directory_path):
        item_path = os.path.join(directory_path, item)
        if os.path.isfile(item_path):
            os.remove(item_path)
        elif os.path.isdir(item_path):
            shutil.rmtree(item_path)

if os.path.exists(binFilePath):
    deleteDirectoryContents(binFilePath)
else:
    os.makedirs(binFilePath)

# change to binFilePath
os.chdir(binFilePath)

# # Copy axf/map to binFilePath
elfFileSource = os.path.join(outputPath, outputName + ".elf")
elfFile = os.path.join(binFilePath, outputName + ".elf")
shutil.copy2(elfFileSource, elfFile)

# mapFileSource = os.path.join(outputPath, outputName + ".map")
# mapFile = os.path.join(binFilePath, outputName + ".map")
# shutil.copy2(mapFileSource, mapFile)

# Gets the operating system
platform = sys.platform

t5Bootloader = os.path.join(portPath, 'tools', 'bootloader.bin')
objcopyTool = ""
t5PackTool = ""
t5AllAppCrcTool = ""
# encryptTool = ""

if platform == "linux" or platform == "linux2":
    # Linux
    objcopyTool = os.path.join(compilerPath, 'arm-none-eabi-objcopy')
    t5PackTool = os.path.join(portPath, 'tools', 'linux', 'cmake_Gen_image')
    t5AllAppCrcTool = os.path.join(portPath, 'tools', 'linux', 'cmake_encrypt_crc')
elif platform == "win32":
    # Windows
    objcopyTool = os.path.join(compilerPath, 'arm-none-eabi-objcopy.exe')
    t5PackTool = os.path.join(portPath, 'tools', 'windows', 'cmake_Gen_image.exe')
    t5AllAppCrcTool = os.path.join(portPath, 'tools', 'windows', 'cmake_encrypt_crc.exe')
elif platform == "darwin":
    # macOS
    logging.error("Running on macOS, not supported")
    sys.exit(1)
else:
    logging.error(f"Unknown platform: {platform}")
    sys.exit(1)

# Print all information
# logging.debug("system: " + platform)
logging.debug("elf file: " + elfFile)
logging.debug("t5Bootloader: " + t5Bootloader)
logging.debug("objcopyTool: " + objcopyTool)
logging.debug("t5PackTool: " + t5PackTool)
logging.debug("t5AllAppCrcTool: " + t5AllAppCrcTool)
logging.debug("outputPath: " + outputPath)
logging.debug("outputName: " + outputName)
logging.debug("appVersion: " + appVersion)

# Check if the beken_packager exists
if not os.path.exists(elfFile):
    logging.error("AXF file not found")
    sys.exit(1)
if not os.path.exists(objcopyTool):
    logging.error("objcopy not found")
    sys.exit(1)
if not os.path.exists(t5PackTool):
    logging.error("t5PackTool not found")
    sys.exit(1)
# if not os.path.exists(t5AllAppCrcTool):
#     logging.error("t5AllAppCrcTool not found")
#     sys.exit(1)

# Convert axf to bin
appBinFile = os.path.join(binFilePath, "app.bin")
objcopyCommand = [
    objcopyTool,
    '-O', 'binary',
    elfFile,
    appBinFile
]
logging.debug("objcopyCommand: " + ' '.join(objcopyCommand))
subprocess.run(objcopyCommand)

# Package
# Generate json file
t5ConfigStr = '''
{
    "magic": "FreeRTOS",
    "version": "0.1",
    "count": 2,
    "section": [
        {
            "firmware": "bootloader.bin",
            "version": "2M.1220",
            "partition": "bootloader",
            "start_addr": "0x0",
            "size": "64K"
        },
        {
            "firmware": "app.bin",
            "version": "2M.1220",
            "partition": "app",
            "start_addr": "0x10000",
            "size": "3520K"
        }
    ]
}
'''
configJson = json.loads(t5ConfigStr)
configJson["section"][0]["firmware"] = t5Bootloader
configJson["section"][1]["firmware"] = appBinFile
logging.debug("configJson: " + json.dumps(configJson, indent=4))
configFile = os.path.join(binFilePath, "config.json")
with open(configFile, 'w') as f:
    json.dump(configJson, f, indent=4)

# Package bin
allAppPack = os.path.join(binFilePath, 'all_app_pack.bin')
t5PackCommand = [
    t5PackTool,
    'genfile',
    '-injsonfile',
    configFile,
    '-infile',
    t5Bootloader,
    appBinFile,
    '-outfile',
    allAppPack
]
logging.debug("t5PackCommand: " + ' '.join(t5PackCommand))
subprocess.run(t5PackCommand)

# Crc package binary
CrcAllAppPack = os.path.join(binFilePath, 'all_app_pack_crc.bin')
t5CrcPackCommand = [
    t5AllAppCrcTool,
    '-crc',
    allAppPack
]
logging.debug("t5CrcPackCommand: " + ' '.join(t5CrcPackCommand))
subprocess.run(t5CrcPackCommand)

# rename
QIOBinFile = os.path.join(binFilePath, outputName + "_QIO_" + appVersion + ".bin")
arduinoUploadBinary=os.path.join(outputPath, outputName + "_QIO" + ".bin")
if os.path.exists(CrcAllAppPack):
    shutil.move(CrcAllAppPack, QIOBinFile)
else:
    logging.error(f"{CrcAllAppPack} not found")
    sys.exit(1)

# Copy QIO binary for Arduino upload
if not os.path.exists(QIOBinFile):
    logging.error(f"{QIOBinFile} not found")
    sys.exit(1)

arduinoUploadBinary=os.path.join(outputPath, outputName + "_QIO" + ".bin")
if os.path.exists(arduinoUploadBinary):
    os.remove(arduinoUploadBinary)
shutil.copy2(QIOBinFile, arduinoUploadBinary)
