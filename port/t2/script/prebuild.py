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

appConfigJsonName = 'appConfig.json'

#
## log config
#
FORMAT = '[%(levelname)s][%(asctime)s][%(filename)s:%(lineno)d]%(message)s'
logging.basicConfig(
    level=logging.INFO,
    format=FORMAT,
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
parser.add_argument('--build-path', type=str, help='Build path')
parser.add_argument('--sketch-path', type=str, help='Sketch path')
parser.add_argument('--use-python-venv', type=int, required=True, help='0: use host python, dependencies may be missing')

args = parser.parse_args()
compilerPath = args.compiler_path
portPath = args.port_path
platformPath = args.platform_path
vendorPath = args.vendor_path
sketchBuildPath = args.build_path
sketchPath = args.sketch_path
usePythonVenv = args.use_python_venv

# 标准化路径
compilerPath = os.path.normpath(compilerPath)
portPath = os.path.normpath(portPath)
platformPath = os.path.normpath(platformPath)
vendorPath = os.path.normpath(vendorPath)
sketchBuildPath = os.path.normpath(sketchBuildPath)
sketchPath = os.path.normpath(sketchPath)

# 打印路径信息
logging.debug("compilerPath: " + compilerPath)
logging.debug("portPath: " + portPath)
logging.debug("platformPath: " + platformPath)
logging.debug("vendorPath: " + vendorPath)
logging.debug("sketchBuildPath: " + sketchBuildPath)
logging.debug("sketchPath: " + sketchPath)
logging.debug(f"usePythonVenv: {usePythonVenv}")

# 检查路径是否存在
if not checkPath(compilerPath):
    sys.exit(1)
if not checkPath(portPath):
    sys.exit(1)
if not checkPath(platformPath):
    sys.exit(1)
if not checkPath(vendorPath):
    sys.exit(1)

# 获取当前Python解释器路径
pythonExecutable = sys.executable
pythonExecutable = os.path.normpath(pythonExecutable)
logging.debug("pythonExecutable: " + pythonExecutable)

#
## Create build directory
#
buildPath = os.path.join(portPath, 'build')
if not os.path.exists(buildPath):
    logging.debug("Create build directory: " + buildPath)
    os.makedirs(buildPath)

#
## 将当前脚本所在路径添加为搜索模块的路径
#
scriptSelfPath = os.path.dirname(os.path.realpath(__file__))
logging.debug("scriptSelfPath: " + scriptSelfPath)
sys.path.append(scriptSelfPath)

from FileLock import FileLock
from appVersionParse import *

#
## App version parse
#
tuyaTmpDir = os.path.join(sketchBuildPath, 'tuyaTmp')
if not os.path.exists(tuyaTmpDir):
    logging.debug("Create build directory: " + tuyaTmpDir)
    os.makedirs(tuyaTmpDir)

appConfigDir = os.path.join(tuyaTmpDir, 'appConfig')
if os.path.exists(appConfigDir):
    shutil.rmtree(appConfigDir)
os.makedirs(appConfigDir)

# Copy sketch/appConfig.json -> /tmp/.../tuyaTmp/appConfig/appConfig.json
appConfigJson = os.path.join(sketchPath, appConfigJsonName)
appConfigJsonTmp = os.path.join(appConfigDir, appConfigJsonName)
logging.debug("appConfigJson: " + appConfigJson)
if os.path.exists(appConfigJson):
    shutil.copy(appConfigJson, appConfigJsonTmp)

appBuildDefineFile = os.path.join(appConfigDir, 'appBuildDefine.txt')
appVersionWrite(appConfigJsonTmp, appBuildDefineFile)

#
## build platform, get libt2Vendor.a
#
vendorLibName = 'libt2Vendor.a'
if enableBuildPlatform and usePythonVenv: # usePythonVenv==1, 使用虚拟环境可以保证不缺少依赖
    buildPlatformScriptPath = os.path.join(portPath, 'script', 'build_platform.py')
    logging.debug("buildPlatformScriptPath: " + buildPlatformScriptPath)

    buildVendorPath = os.path.join(buildPath, 'vendor')
    if not os.path.exists(buildVendorPath):
        logging.debug("Create build vendor directory: " + buildVendorPath)
        os.makedirs(buildVendorPath)

    buildVendorLibPath = os.path.join(buildVendorPath, vendorLibName)
    useVendorLibPath = os.path.join(portPath, 'libs', vendorLibName)

    buildPlatformFileLock = FileLock("buildPlatform.lock", buildVendorPath)

    if buildPlatformFileLock.isLocked():
        lastLibSize = 0
        errCnt = 0
        # wait build complete
        while buildPlatformFileLock.isLocked():
            logging.info("Wait build platform complete...")
            time.sleep(3)

            if os.path.exists(buildVendorLibPath):
                curLibSize = os.stat(buildVendorLibPath).st_size
                logging.debug(f"curLibSize: {buildVendorLibPath} {curLibSize}")
                if lastLibSize != curLibSize:
                    lastLibSize = curLibSize
                    errCnt = 0
                    logging.debug(f'update vendor lib size, errCnt {errCnt}')
                else:
                    errCnt += 1
                    logging.debug(f'vendor errCnt {errCnt}')
            else:
                errCnt += 1
                logging.debug(f'vendor errCnt {errCnt}')

            if errCnt >= 3:
                buildPlatformFileLock.unlock()
                shutil.rmtree(buildVendorPath)
                os.makedirs(buildVendorPath)

    buildPlatformFileLock.lock()

    # 为了使 .sconsign.dblite 生成在 buildVendorPath 目录下
    os.chdir(buildVendorPath)

    buildPlatformScriptArgs = (
        "--compiler-path " + compilerPath +
        " --vendor-path " + os.path.join(vendorPath, "beken_os", "beken378") +
        " --flags-path " + os.path.join(portPath, "flags") +
        " --open-sdk-path " + os.path.join(vendorPath, "tuya_open_sdk") +
        " --tkl-path " + os.path.join(vendorPath, "tuyaos_adapter") +
        " --object-output-dir " + buildVendorPath +
        " --lib-output-dir " + os.path.join(portPath, "libs") +
        " --lib-output-name " + vendorLibName
    )
    logging.debug("buildPlatformScriptArgs: " + buildPlatformScriptArgs)

    # 构建命令
    result = subprocess.run([pythonExecutable, '-u', '-m', 'SCons', '-f', buildPlatformScriptPath, 'CUSTOM_ARGS=' + buildPlatformScriptArgs])

    # 获取返回码
    logging.info("result.returncode: " + str(result.returncode))

    if result.returncode == 0:
        logging.debug("build platform success")
        # 比较lib文件是否有变化
        if not compareFileHash(buildVendorLibPath, useVendorLibPath):
            logging.info("copy " + buildVendorLibPath + " -> " + useVendorLibPath)
            shutil.copy(buildVendorLibPath, useVendorLibPath)

    buildPlatformFileLock.unlock()

#
## build open sdk, get libtuyaos.a
#
openSDKLibName = 'libtuyaos.a'
if enableBuildOpenSDK:
    buildOpenSDKScriptPath = os.path.join(portPath, 'script', 'build_open_sdk.py')

    buildOpenSDKPath = os.path.join(buildPath, 'open_sdk')
    if not os.path.exists(buildOpenSDKPath):
        logging.debug("Create build open sdk directory: " + buildOpenSDKPath)
        os.makedirs(buildOpenSDKPath)

    buildOpenSDKLibPath = os.path.join(buildOpenSDKPath, openSDKLibName)
    useOpenSDKLibPath = os.path.join(portPath, 'libs', openSDKLibName)

    buildOpenSDKFileLock = FileLock("buildOpenSDK.lock", buildOpenSDKPath)

    if buildOpenSDKFileLock.isLocked():
        lastLibSize = 0
        errCnt = 0
        # wait build complete
        while buildOpenSDKFileLock.isLocked():
            logging.info("Wait build open sdk complete...")
            time.sleep(3)

            if os.path.exists(buildOpenSDKLibPath):
                curLibSize = os.stat(buildOpenSDKLibPath).st_size
                logging.debug(f"curLibSize: {buildOpenSDKLibPath} {curLibSize}")
                if lastLibSize != curLibSize:
                    lastLibSize = curLibSize
                    errCnt = 0
                    logging.debug(f'update open sdk lib size, errCnt {errCnt}')
                else:
                    errCnt += 1
                    logging.debug(f'open sdk errCnt {errCnt}')
            else:
                errCnt += 1
                logging.debug(f'open sdk errCnt {errCnt}')

            if errCnt >= 3:
                buildOpenSDKFileLock.unlock()
                shutil.rmtree(buildOpenSDKPath)
                os.makedirs(buildOpenSDKPath)

    buildOpenSDKFileLock.lock()

    # 为了使 .sconsign.dblite 生成在 buildOpenSDKPath 目录下
    os.chdir(buildOpenSDKPath)

    buildOpenSDKScriptArgs = (
        "--compiler-path " + compilerPath +
        " --vendor-path " + os.path.join(vendorPath, "beken_os", "beken378") +
        " --flags-path " + os.path.join(portPath, "flags") +
        " --open-sdk-path " + os.path.join(vendorPath, "tuya_open_sdk") +
        " --tkl-path " + os.path.join(vendorPath, "tuyaos_adapter") +
        " --object-output-dir " + buildOpenSDKPath +
        " --lib-output-dir " + os.path.join(portPath, "libs") +
        " --lib-output-name " + openSDKLibName
    )
    logging.debug("buildOpenSDKScriptArgs: " + buildOpenSDKScriptArgs)

    # 构建命令
    result = subprocess.run([pythonExecutable, '-u', '-m', 'SCons', '-f', buildOpenSDKScriptPath, 'CUSTOM_ARGS=' + buildOpenSDKScriptArgs])

    # 获取返回码
    logging.info("result.returncode: " + str(result.returncode))

    if result.returncode == 0:
        logging.debug("build platform success")
        # 比较lib文件是否有变化
        if not compareFileHash(buildOpenSDKLibPath, useOpenSDKLibPath):
            # copy buildOpenSDKLibPath -> useOpenSDKLibPath
            logging.debug("copy " + buildOpenSDKLibPath + " -> " + useOpenSDKLibPath)
            shutil.copy(buildOpenSDKLibPath, useOpenSDKLibPath)

    buildOpenSDKFileLock.unlock()

#
## build tkl, get libt2Tkl.a
#
tklLibName = 'libt2Tkl.a'
if enableBuildTKL:
    buildTKLScriptPath = os.path.join(portPath, 'script', 'build_tkl.py')
    logging.debug("buildTKLScriptPath: " + buildTKLScriptPath)

    buildTklPath = os.path.join(buildPath, 'tkl')
    if not os.path.exists(buildTklPath):
        logging.debug("Create build tkl directory: " + buildTklPath)
        os.makedirs(buildTklPath)

    buildTklLibPath = os.path.join(buildTklPath, tklLibName)
    useTklLibPath = os.path.join(portPath, 'libs', tklLibName)

    buildTKLFileLock = FileLock("buildTKL.lock", buildTklPath)

    if buildTKLFileLock.isLocked():
        lastLibSize = 0
        errCnt = 0
        # wait build complete
        while buildTKLFileLock.isLocked():
            logging.info("Wait build tkl complete...")
            time.sleep(3)

            if os.path.exists(buildTklLibPath):
                curLibSize = os.stat(buildTklLibPath).st_size
                logging.debug(f"curLibSize: {buildTklLibPath} {curLibSize}")
                if lastLibSize != curLibSize:
                    lastLibSize = curLibSize
                    errCnt = 0
                    logging.debug(f'update tkl lib size, errCnt {errCnt}')
                else:
                    errCnt += 1
                    logging.debug(f'tkl errCnt {errCnt}')
            else:
                errCnt += 1
                logging.debug(f'tkl errCnt {errCnt}')

            if errCnt >= 3:
                buildTKLFileLock.unlock()
                shutil.rmtree(buildTklPath)
                os.makedirs(buildTklPath)


    buildTKLFileLock.lock()

    # 为了使 .sconsign.dblite 生成在 buildTklPath 目录下
    os.chdir(buildTklPath)

    buildTKLScriptArgs = (
        "--compiler-path " + compilerPath +
        " --vendor-path " + os.path.join(vendorPath, "beken_os", "beken378") +
        " --flags-path " + os.path.join(portPath, "flags") +
        " --open-sdk-path " + os.path.join(vendorPath, "tuya_open_sdk") +
        " --tkl-path " + os.path.join(vendorPath, "tuyaos_adapter") +
        " --object-output-dir " + buildTklPath +
        " --lib-output-dir " + os.path.join(portPath, "libs") +
        " --lib-output-name " + tklLibName
    )
    logging.debug("buildTKLScriptArgs: " + buildTKLScriptArgs)

    # 构建命令
    result = subprocess.run([pythonExecutable, '-u', '-m', 'SCons', '-f', buildTKLScriptPath, 'CUSTOM_ARGS=' + buildTKLScriptArgs])

    # 获取返回码
    logging.info("result.returncode: " + str(result.returncode))

    if result.returncode == 0:
        logging.debug("build tkl success")
        # 比较lib文件是否有变化
        if not compareFileHash(buildTklLibPath, useTklLibPath):
            # copy buildTklLibPath -> useTklLibPath
            logging.debug("copy " + buildTklLibPath + " -> " + useTklLibPath)
            shutil.copy(buildTklLibPath, useTklLibPath)

    buildTKLFileLock.unlock()

logging.info("=== prebuild.py end ===")
