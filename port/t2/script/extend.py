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

logging.basicConfig(
    level=logging.WARNING,
    format='[%(levelname)s][%(filename)s:%(lineno)d]%(message)s')

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

# Gets the operating system
platform = sys.platform

t2Bootloader = os.path.join(portPath, 'tools', 't2_bootloader_enc.bin')
axfFile = os.path.join(outputPath, outputName + ".axf")
objcopyTool = ""
bekenPackTool = ""
encryptTool = ""

if platform == "linux" or platform == "linux2":
    # Linux
    objcopyTool = os.path.join(compilerPath, 'arm-none-eabi-objcopy')
    bekenPackTool = os.path.join(portPath, 'tools', 'linux', 'beken_packager')
    encryptTool = os.path.join(portPath, 'tools', 'linux', 'encrypt')
elif platform == "win32":
    # Windows
    objcopyTool = os.path.join(compilerPath, 'arm-none-eabi-objcopy.exe')
    bekenPackTool = os.path.join(portPath, 'tools', 'windows', 'beken_packager.exe')
    encryptTool = os.path.join(portPath, 'tools', 'windows', 'encrypt.exe')
elif platform == "darwin":
    # macOS
    logging.error("Running on macOS, not supported")
    exit(1)
else:
    logging.error(f"Unknown platform: {platform}")
    exit(1)

# Print all information
logging.debug("system: " + platform)
logging.debug("AXF file: " + axfFile)
logging.debug("t2Bootloader: " + t2Bootloader)
logging.debug("objcopyTool: " + objcopyTool)
logging.debug("bekenPackTool: " + bekenPackTool)
logging.debug("encryptTool: " + encryptTool)
logging.debug("outputPath: " + outputPath)
logging.debug("outputName: " + outputName)

# Check if the beken_packager exists
if not os.path.exists(t2Bootloader):
    logging.error("t2 bootloader not found")
    exit(1)
if not os.path.exists(axfFile):
    logging.error("AXF file not found")
    exit(1)
if not os.path.exists(objcopyTool):
    logging.error("objcopy not found")
    exit(1)
if not os.path.exists(bekenPackTool):
    logging.error("beken_packager not found")
    exit(1)
if not os.path.exists(encryptTool):
    logging.error("encrypt not found")
    exit(1)

# output file path
binFilePath = os.path.join(outputPath, 'output')

# delete binFilePath
if os.path.exists(binFilePath):
    shutil.rmtree(binFilePath)

os.makedirs(binFilePath)
# change to binFilePath
os.chdir(binFilePath)

binFile = os.path.join(binFilePath, outputName + ".bin")

# Convert axf to bin
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
    exit(1)

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

# rename bin file
allBinFile = os.path.join(binFilePath, "all_1.00.bin")
QIOBinFile = os.path.join(binFilePath, outputName + "_QIO.bin")
encUartBinFile = os.path.join(binFilePath, outputName + "_enc_uart_1.00.bin")
UABinFile = os.path.join(binFilePath, outputName + "_UA.bin")

if not os.path.exists(allBinFile):
    logging.debug("allBinFile: " + allBinFile)
    logging.debug("pack failed")
    exit(1)
if not os.path.exists(encUartBinFile):
    logging.debug("encUartBinFile: " + encUartBinFile)
    logging.debug("pack failed")
    exit(1)

shutil.move(allBinFile, QIOBinFile)
shutil.move(encUartBinFile, UABinFile)
