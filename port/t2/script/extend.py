#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# extend.py
# @desc: t2 extend script, package all the files to a bin file, and generate ota bin

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
# parser.add_argument('--app-version', type=str, help='tuya app version')

args = parser.parse_args()
compilerPath = args.compiler_path
portPath = args.port_path
outputPath = args.output_path

outputName = args.output_name
# # Remove '.ino'
# rmOutputNameSuffix='.ino'
# if outputName.endswith(rmOutputNameSuffix):
#     outputName=outputName[:-len(rmOutputNameSuffix)]

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
if os.path.exists(binFilePath):
    shutil.rmtree(binFilePath)

os.makedirs(binFilePath)
# change to binFilePath
os.chdir(binFilePath)

# Copy axf/map to binFilePath
axfFileSource = os.path.join(outputPath, outputName + ".axf")
axfFile = os.path.join(binFilePath, outputName + ".axf")
shutil.copy2(axfFileSource, axfFile)

mapFileSource = os.path.join(outputPath, outputName + ".map")
mapFile = os.path.join(binFilePath, outputName + ".map")
shutil.copy2(mapFileSource, mapFile)

# Gets the operating system
platform = sys.platform

t2Bootloader = os.path.join(portPath, 'tools', 't2_bootloader_enc.bin')
objcopyTool = ""
bekenPackTool = ""
encryptTool = ""

if platform == "linux" or platform == "linux2":
    # Linux
    objcopyTool = os.path.join(compilerPath, 'arm-none-eabi-objcopy')
    bekenPackTool = os.path.join(portPath, 'tools', 'linux', 'beken_packager')
    encryptTool = os.path.join(portPath, 'tools', 'linux', 'encrypt')
    otaPackageTool = os.path.join(portPath, 'tools', 'linux', 'rt_ota_packaging_tool_cli')
    tuyaPackageTool = os.path.join(portPath, 'tools', 'linux', 'package')
elif platform == "win32":
    # Windows
    objcopyTool = os.path.join(compilerPath, 'arm-none-eabi-objcopy.exe')
    bekenPackTool = os.path.join(portPath, 'tools', 'windows', 'beken_packager.exe')
    encryptTool = os.path.join(portPath, 'tools', 'windows', 'encrypt.exe')
    otaPackageTool = os.path.join(portPath, 'tools', 'windows', 'rt_ota_packaging_tool_cli.exe')
    tuyaPackageTool = os.path.join(portPath, 'tools', 'windows', 'package.exe')
elif platform == "darwin":
    # macOS
    logging.error("Running on macOS, not supported")
    sys.exit(1)
else:
    logging.error(f"Unknown platform: {platform}")
    sys.exit(1)

# Print all information
logging.debug("system: " + platform)
logging.debug("AXF file: " + axfFile)
logging.debug("t2Bootloader: " + t2Bootloader)
logging.debug("objcopyTool: " + objcopyTool)
logging.debug("bekenPackTool: " + bekenPackTool)
logging.debug("encryptTool: " + encryptTool)
logging.debug("outputPath: " + outputPath)
logging.debug("outputName: " + outputName)
logging.debug("appVersion: " + appVersion)

# Check if the beken_packager exists
if not os.path.exists(t2Bootloader):
    logging.error("t2 bootloader not found")
    sys.exit(1)
if not os.path.exists(axfFile):
    logging.error("AXF file not found")
    sys.exit(1)
if not os.path.exists(objcopyTool):
    logging.error("objcopy not found")
    sys.exit(1)
if not os.path.exists(bekenPackTool):
    logging.error("beken_packager not found")
    sys.exit(1)
if not os.path.exists(encryptTool):
    logging.error("encrypt not found")
    sys.exit(1)
if not os.path.exists(otaPackageTool):
    logging.error("otaPackageTool not found")
    sys.exit(1)
if not os.path.exists(tuyaPackageTool):
    logging.error("tuyaPackageTool not found")
    sys.exit(1)


# Convert axf to bin
binFile = os.path.join(binFilePath, outputName + ".bin")
objcopyCommand = [
    objcopyTool,
    '-O', 'binary',
    axfFile,
    binFile
]
logging.debug("objcopyCommand: " + ' '.join(objcopyCommand))
os.system(' '.join(objcopyCommand))

# encrypt bin
encryptCommand = [
    encryptTool,
    binFile,
    '510fb093',
    'a3cbeadc',
    '5993a17e',
    'c7adeb03',
    '10000',
]
logging.debug("encryptCommand: " + ' '.join(encryptCommand))
os.system(' '.join(encryptCommand))

# encrypt bin file
encBinFile = os.path.join(binFilePath, outputName + "_enc.bin")
if not os.path.exists(encBinFile):
    logging.error("encBinFile: " + encBinFile)
    logging.error("encrypt failed")
    sys.exit(1)

# Generate json file
configStr = '''
{
    "count": 4,
    "magic": "RT-Thread",
    "section": [
        {
            "firmware": "t2_bootloader_enc.bin",
            "partition": "bootloader",
            "size": "68K",
            "start_addr": "0x00000000",
            "version": "1.00"
        },
        {
            "firmware": "t2_common_1.0.1_enc.bin",
            "partition": "app",
            "size": "1150832",
            "start_addr": "0x00011000",
            "version": "1.00"
        }
    ],
    "version": "0.1"
}
'''
configJson = json.loads(configStr)
configJson["section"][0]["firmware"] = t2Bootloader
configJson["section"][1]["firmware"] = encBinFile
logging.debug("configJson: " + json.dumps(configJson, indent=4))

configFile = os.path.join(binFilePath, "config.json")
with open(configFile, 'w') as f:
    json.dump(configJson, f, indent=4)

# Package bin
bekenPackCommand = [
    bekenPackTool,
    configFile
]
logging.debug("bekenPackCommand: " + ' '.join(bekenPackCommand))
os.system(' '.join(bekenPackCommand))

# Output binary name
allBinFile = os.path.join(binFilePath, "all_1.00.bin")
QIOBinFile = os.path.join(binFilePath, outputName + "_QIO_" + appVersion + ".bin")
encUartBinFile = os.path.join(binFilePath, outputName + "_enc_uart_1.00.bin")
UABinFile = os.path.join(binFilePath, outputName + "_UA_" + appVersion + ".bin")

rblFile = os.path.join(binFilePath, outputName + appVersion + ".rbl")

rtOTAPackageCommand = [
    otaPackageTool,
    '-f', binFile,
    '-v', ' '
    '-o', rblFile,
    '-p', 'app',
    '-c', 'gzip',
    '-s', 'aes',
    '-k', '0123456789ABCDEF0123456789ABCDEF',
    '-i', '0123456789ABCDEF'
]
subprocess.run(rtOTAPackageCommand)

ugFile = outputName + '_UG_' + appVersion + '.bin'

tuyaPackageCommand = [
    tuyaPackageTool,
    os.path.join(binFilePath, outputName + '.rbl'),
    outputName + '_UG_' + appVersion + '.bin',
    appVersion
]

# 将命令列表转换为字符串
command_str = ' '.join(tuyaPackageCommand)

# 打印命令
print(command_str)

subprocess.run(tuyaPackageCommand)

# Check allBin and UartBin
if not os.path.exists(allBinFile):
    logging.debug("allBinFile: " + allBinFile)
    logging.debug("pack failed")
    sys.exit(1)
if not os.path.exists(encUartBinFile):
    logging.debug("encUartBinFile: " + encUartBinFile)
    logging.debug("pack failed")
    sys.exit(1)

t2_ate_path = os.path.join(portPath, 'tools', 't2_ate')
if os.path.exists(t2_ate_path):
    output_bin = os.path.join(binFilePath, 'QIO_no_ate')
    combine_qio_ate_path = os.path.join(portPath, 'script', 'combine_qio_ate.py')
    result = subprocess.run([pythonExecutable, combine_qio_ate_path, output_bin, "2097152", allBinFile, "0", t2_ate_path, "1253376"], check=True)
    if result.returncode == 0:
        shutil.move(output_bin, QIOBinFile)
        os.remove(allBinFile)
    else:
        logging.error("combine allBin and t2_ate fail")
        shutil.move(allBinFile, QIOBinFile)
else:
    shutil.move(allBinFile, QIOBinFile)

# rename UA Bin
shutil.move(encUartBinFile, UABinFile)

# Copy QIO binary for Arduino upload
arduinoUploadBinary=os.path.join(outputPath, outputName + "_QIO" + ".bin")
if os.path.exists(arduinoUploadBinary):
    os.remove(arduinoUploadBinary)

shutil.copy2(QIOBinFile, arduinoUploadBinary)

# Remove unnecessary files
## config.json
if os.path.exists(os.path.join(binFilePath, "config.json")):
    os.remove(os.path.join(binFilePath, "config.json"))
## {sketchName}.bin
if os.path.exists(os.path.join(binFilePath, outputName + ".bin")):
    os.remove(os.path.join(binFilePath, outputName + ".bin"))
## {sketchName}.cpr
if os.path.exists(os.path.join(binFilePath, outputName + ".cpr")):
    os.remove(os.path.join(binFilePath, outputName + ".cpr"))
## {sketchName}.out
if os.path.exists(os.path.join(binFilePath, outputName + ".out")):
    os.remove(os.path.join(binFilePath, outputName + ".out"))
## {sketchName}.rbl
if os.path.exists(os.path.join(binFilePath, outputName + ".rbl")):
    os.remove(os.path.join(binFilePath, outputName + ".rbl"))
## {sketchName}_enc.bin
if os.path.exists(os.path.join(binFilePath, outputName + "_enc.bin")):
    os.remove(os.path.join(binFilePath, outputName + "_enc.bin"))
