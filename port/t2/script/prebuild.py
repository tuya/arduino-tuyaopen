import os
import sys
import shutil
import logging
import argparse
import subprocess
import time
import hashlib

enableBuildPlatform = False
enableBuildOpenSDK = False
enableBuildTKL = False

#
## log config
#
logging.basicConfig(
    level=logging.INFO,
    format='[%(levelname)s][%(asctime)s][%(filename)s:%(lineno)d]%(message)s',
    datefmt='%H:%M:%S'
)

def checkPath(path):
    """
    Check if the given path exists.

    Args:
        path (str): The path to check.

    Returns:
        bool: True if the path exists, False otherwise.
    """
    if not os.path.exists(path):
        logging.error("Path not exist: " + path)
        return False
    else:
        return True

logging.info("=== prebuild.py begin ===")

def calculateFileHash(filename, hash_algorithm=hashlib.sha256):
    """ 计算文件的哈希值 """
    hash_obj = hash_algorithm()
    with open(filename, "rb") as file:
        for chunk in iter(lambda: file.read(4096), b""):
            hash_obj.update(chunk)
    return hash_obj.hexdigest()

def compareFileHash(file1, file2):
    """ 比较两个文件的哈希值 """
    if not os.path.exists(file1) or not os.path.exists(file2):
        logging.error("file not exist: " + file1 + " or " + file2)
        return False
    hash1 = calculateFileHash(file1)
    hash2 = calculateFileHash(file2)
    return hash1 == hash2

#
## get path
#
parser = argparse.ArgumentParser()
parser.add_argument('--compiler-path', type=str, required=True, help='compiler path')
parser.add_argument('--port-path', type=str, required=True, help='port path')
parser.add_argument('--platform-path', type=str, required=True, help='platform path, the folder containing boards.txt in Arduino IDE')
parser.add_argument('--vendor-path', type=str, required=True, help='vendor path, chip vendor path')

args = parser.parse_args()
compilerPath = args.compiler_path
portPath = args.port_path
platformPath = args.platform_path
vendorPath = args.vendor_path

# 标准化路径
compilerPath = os.path.normpath(compilerPath)
portPath = os.path.normpath(portPath)
platformPath = os.path.normpath(platformPath)
vendorPath = os.path.normpath(vendorPath)

# 打印路径信息
logging.debug("compilerPath: " + compilerPath)
logging.debug("portPath: " + portPath)
logging.debug("platformPath: " + platformPath)
logging.debug("vendorPath: " + vendorPath)

# 检查路径是否存在
if not checkPath(compilerPath):
    sys.exit(1)
if not checkPath(portPath):
    sys.exit(1)
if not checkPath(platformPath):
    sys.exit(1)
if not checkPath(vendorPath):
    sys.exit(1)

#
## Create build directory
#
buildPath = os.path.join(portPath, 'build')
if not os.path.exists(buildPath):
    logging.debug("Create build directory: " + buildPath)
    os.makedirs(buildPath)

from FileLock import FileLock

# 获取当前Python解释器路径
pythonExecutable = sys.executable
pythonExecutable = os.path.normpath(pythonExecutable)
logging.debug("pythonExecutable: " + pythonExecutable)

#
## build platform, get libt2Vendor.a
#
if enableBuildPlatform:
    buildPlatformScriptPath = os.path.join(portPath, 'script', 'build_platform.py')
    logging.debug("buildPlatformScriptPath: " + buildPlatformScriptPath)

    buildVendorPath = os.path.join(buildPath, 'vendor')
    if not os.path.exists(buildVendorPath):
        logging.debug("Create build vendor directory: " + buildVendorPath)
        os.makedirs(buildVendorPath)

    buildPlatformScriptArgs = (
        "--compiler-path " + compilerPath +
        " --vendor-path " + os.path.join(vendorPath, "beken_os", "beken378") +
        " --flags-path " + os.path.join(portPath, "flags") +
        " --open-sdk-path " + os.path.join(vendorPath, "tuya_open_sdk") +
        " --tkl-path " + os.path.join(vendorPath, "tuyaos_adapter") +
        " --object-output-dir " + buildVendorPath +
        " --lib-output-dir " + os.path.join(portPath, "libs")
    )
    logging.debug("buildPlatformScriptArgs: " + buildPlatformScriptArgs)

    os.chdir(buildVendorPath) # 为了使 .sconsign.dblite 生成在 buildVendorPath 目录下
    buildPlatformFileLock = FileLock("buildPlatform.lock", buildPath)

    if buildPlatformFileLock.isLocked():
        # wait build complete
        while buildPlatformFileLock.isLocked():
            logging.info("Wait build platform complete...")
            time.sleep(3)
        logging.info("Build platform complete")

    buildPlatformFileLock.lock()

    # 构建命令
    result = subprocess.run([pythonExecutable, '-u', '-m', 'SCons', '-f', buildPlatformScriptPath, 'CUSTOM_ARGS=' + buildPlatformScriptArgs])

    # 获取返回码
    logging.info("result.returncode: " + str(result.returncode))

    if result.returncode == 0:
        logging.debug("build platform success")
        # 比较lib文件是否有变化
        buildVendorLibPath = os.path.join(buildVendorPath, "libt2Vendor.a")
        useVendorLibPath = os.path.join(portPath, 'libs', "libt2Vendor.a")
        if not compareFileHash(buildVendorLibPath, useVendorLibPath):
            # copy buildVendorLibPath -> useVendorLibPath
            logging.debug("copy " + buildVendorLibPath + " -> " + useVendorLibPath)
            shutil.copy(buildVendorLibPath, useVendorLibPath)

    buildPlatformFileLock.unlock()

if enableBuildOpenSDK:
    buildOpenSDKScriptPath = os.path.join(portPath, 'script', 'build_open_sdk.py')

    buildOpenSDKPath = os.path.join(buildPath, 'open_sdk')
    if not os.path.exists(buildOpenSDKPath):
        logging.debug("Create build open sdk directory: " + buildOpenSDKPath)
        os.makedirs(buildOpenSDKPath)

    buildOpenSDKScriptArgs = (
        "--compiler-path " + compilerPath +
        " --vendor-path " + os.path.join(vendorPath, "beken_os", "beken378") +
        " --flags-path " + os.path.join(portPath, "flags") +
        " --open-sdk-path " + os.path.join(vendorPath, "tuya_open_sdk") +
        " --tkl-path " + os.path.join(vendorPath, "tuyaos_adapter") +
        " --object-output-dir " + buildOpenSDKPath +
        " --lib-output-dir " + os.path.join(portPath, "libs")
    )
    logging.debug("buildOpenSDKScriptArgs: " + buildOpenSDKScriptArgs)

    os.chdir(buildOpenSDKPath) # 为了使 .sconsign.dblite 生成在 buildOpenSDKPath 目录下
    buildOpenSDKFileLock = FileLock("buildOpenSDK.lock", buildPath)

    if buildOpenSDKFileLock.isLocked():
        # wait build complete
        while buildOpenSDKFileLock.isLocked():
            logging.info("Wait build open sdk complete...")
            time.sleep(3)
        logging.info("Build open sdk complete")

    buildOpenSDKFileLock.lock()

    # 构建命令
    result = subprocess.run([pythonExecutable, '-u', '-m', 'SCons', '-f', buildOpenSDKScriptPath, 'CUSTOM_ARGS=' + buildOpenSDKScriptArgs])

    # 获取返回码
    logging.info("result.returncode: " + str(result.returncode))

    if result.returncode == 0:
        logging.debug("build platform success")
        # 比较lib文件是否有变化
        buildOpenSDKLibPath = os.path.join(buildOpenSDKPath, "libtuyaos.a")
        useOpenSDKLibPath = os.path.join(portPath, 'libs', "libtuyaos.a")
        if not compareFileHash(buildOpenSDKLibPath, useOpenSDKLibPath):
            # copy buildOpenSDKLibPath -> useOpenSDKLibPath
            logging.debug("copy " + buildOpenSDKLibPath + " -> " + useOpenSDKLibPath)
            shutil.copy(buildOpenSDKLibPath, useOpenSDKLibPath)

    buildOpenSDKFileLock.unlock()

if enableBuildTKL:
    buildTKLScriptPath = os.path.join(portPath, 'script', 'build_tkl.py')
    logging.debug("buildTKLScriptPath: " + buildTKLScriptPath)

    buildTklPath = os.path.join(buildPath, 'tkl')
    if not os.path.exists(buildTklPath):
        logging.debug("Create build tkl directory: " + buildTklPath)
        os.makedirs(buildTklPath)
    
    buildTKLScriptArgs = (
        "--compiler-path " + compilerPath +
        " --vendor-path " + os.path.join(vendorPath, "beken_os", "beken378") +
        " --flags-path " + os.path.join(portPath, "flags") +
        " --open-sdk-path " + os.path.join(vendorPath, "tuya_open_sdk") +
        " --tkl-path " + os.path.join(vendorPath, "tuyaos_adapter") +
        " --object-output-dir " + buildTklPath +
        " --lib-output-dir " + os.path.join(portPath, "libs")
    )
    logging.debug("buildTKLScriptArgs: " + buildTKLScriptArgs)

    os.chdir(buildTklPath) # 为了使 .sconsign.dblite 生成在 buildTklPath 目录下
    buildTKLFileLock = FileLock("buildTKL.lock", buildPath)

    if buildTKLFileLock.isLocked():
        # wait build complete
        while buildTKLFileLock.isLocked():
            logging.info("Wait build tkl complete...")
            time.sleep(3)
        logging.info("Build tkl complete")

    buildTKLFileLock.lock()

    # 构建命令
    result = subprocess.run([pythonExecutable, '-u', '-m', 'SCons', '-f', buildTKLScriptPath, 'CUSTOM_ARGS=' + buildTKLScriptArgs])

    # 获取返回码
    logging.info("result.returncode: " + str(result.returncode))

    if result.returncode == 0:
        logging.debug("build tkl success")
        # 比较lib文件是否有变化
        buildTklLibPath = os.path.join(buildTklPath, "libt2Tkl.a")
        useTklLibPath = os.path.join(portPath, 'libs', "libt2Tkl.a")
        if not compareFileHash(buildTklLibPath, useTklLibPath):
            # copy buildTklLibPath -> useTklLibPath
            logging.debug("copy " + buildTklLibPath + " -> " + useTklLibPath)
            shutil.copy(buildTklLibPath, useTklLibPath)

    buildTKLFileLock.unlock()

logging.info("=== prebuild.py end ===")
