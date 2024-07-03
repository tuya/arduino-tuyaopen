import os
import sys
import argparse
import subprocess

print("=== prebuild.py begin ===")

# 获取当前Python解释器路径
pythonExecutable = sys.executable

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
pythonExecutable = os.path.normpath(pythonExecutable)
compilerPath = os.path.normpath(compilerPath)
portPath = os.path.normpath(portPath)
platformPath = os.path.normpath(platformPath)
vendorPath = os.path.normpath(vendorPath)


# 打印所有路径信息
print("pythonExecutable: " + pythonExecutable)
print("compilerPath: " + compilerPath)
print("portPath: " + portPath)
print("platformPath: " + platformPath)
print("vendorPath: " + vendorPath)

#
## build platform, get libt2Vendor.a
#
buildPlatformScriptPath = os.path.join(portPath, 'script', 'build_platform.py')
print("buildPlatformScriptPath: " + buildPlatformScriptPath)

buildPlatformScriptArgs = (
    "--compiler-path " + compilerPath +
    " --vendor-path " + os.path.join(vendorPath, "beken_os", "beken378") +
    " --flags-path " + os.path.join(portPath, "flags") +
    " --open-sdk-path " + os.path.join(platformPath, "open-sdk") +
    " --tkl-path " + os.path.join(vendorPath, "tuyaos", "tuyaos_adapter") +
    " --object-output-dir " + os.path.join(portPath, "build") +
    " --lib-output-dir " + os.path.join(portPath, "libs")
)

print("buildPlatformScriptArgs: " + buildPlatformScriptArgs)

exit(0)

# 构建命令
command = [
    pythonExecutable,
    '-m', 'SCons',
    '-f', buildPlatformScriptPath,
    'CUSTOM_ARGS=' + buildPlatformScriptArgs
]
print("command: " + ' '.join(command))

# 运行命令并实时捕获输出
process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

# 实时打印标准输出
for line in process.stdout:
    print(line, end='')

# 实时打印标准错误
for line in process.stderr:
    print(line, end='')

# 等待 command 完成
process.wait()

# 获取返回码
print("Return Code:", process.returncode)

print("=== prebuild.py end ===")
