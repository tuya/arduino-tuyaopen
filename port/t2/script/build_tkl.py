import os
import argparse
import sys
import logging
from SCons.Script import *

#
## log config
#
FORMAT = '[%(levelname)s][%(asctime)s][%(filename)s]%(message)s'
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


#
# 参数解析
#
parser = argparse.ArgumentParser(description='Build vendor library')
parser.add_argument('--compiler-path', type=str, required=True, help='Specify the compiler path')
parser.add_argument('--vendor-path', type=str, required=True, help='Specify the vendor path')
parser.add_argument('--flags-path', type=str, required=True, help='Specify the flags path')
parser.add_argument('--open-sdk-path', type=str, required=True, help='Specify the open-sdk path')
parser.add_argument('--tkl-path', type=str, required=True, help='Specify the tkl path')
parser.add_argument('--object-output-dir', type=str, required=True, help='Specify the object output directory')
parser.add_argument('--lib-output-dir', type=str, required=True, help='Specify the library output directory')
parser.add_argument('--lib-output-name', type=str, required=True, help='Specify the library output name')

# 使用SCons自定义参数
if 'CUSTOM_ARGS' in ARGUMENTS:
    args = parser.parse_args(ARGUMENTS['CUSTOM_ARGS'].split())
else:
    logging.error("Error: CUSTOM_ARGS is missing.")
    sys.exit(1)

compilerPath = os.path.join(args.compiler_path, '')
vendorPath = os.path.join(args.vendor_path, '')
flagsPath = os.path.join(args.flags_path, '')
openSdkPath = os.path.join(args.open_sdk_path, '')
tklPath = os.path.join(args.tkl_path, '')

objectOutputDir=args.object_output_dir
libOutputDir=objectOutputDir
openSdkIncludeFlags = os.path.join(flagsPath, "open_sdk_includes.txt")
vendorIncludeFlags = os.path.join(flagsPath, "vendor_includes.txt")
tklIncludeFlags = os.path.join(flagsPath, "port_includes.txt")
outputLibName = args.lib_output_name

# 打印输入参数
logging.debug("compiler path: " + compilerPath)
logging.debug("vendor path: " + vendorPath)
logging.debug("flags path: " + flagsPath)
logging.debug("open-sdk path: " + openSdkPath)
logging.debug("open-sdk port path: " + tklPath)
logging.debug("object output directory: " + objectOutputDir)
logging.debug("library output directory: " + libOutputDir)
logging.debug("openSdkIncludeFlags: " + openSdkIncludeFlags)
logging.debug("vendorIncludeFlags: " + vendorIncludeFlags)
logging.debug("tklIncludeFlags: " + tklIncludeFlags)
logging.debug("outputLibName: " + outputLibName)

# 路径和文件是否存在判断
if checkPath(compilerPath) == False:
    sys.exit(1)
if checkPath(vendorPath) == False:
    sys.exit(1)
if checkPath(flagsPath) == False:
    sys.exit(1)
if checkPath(openSdkPath) == False:
    sys.exit(1)
if checkPath(tklPath) == False:
    sys.exit(1)
if checkPath(objectOutputDir) == False:
    sys.exit(1)
if checkPath(libOutputDir) == False:
    sys.exit(1)
if checkPath(openSdkIncludeFlags) == False:
    sys.exit(1)
if checkPath(vendorIncludeFlags) == False:
    sys.exit(1)
if checkPath(tklIncludeFlags) == False:
    sys.exit(1)

#
# 设置 SCons 的默认参数
#
cross_compiler = "arm-none-eabi-"

def print_cmd_line(s, target, src, env):
    nameStr = os.path.basename(str(target[0]))
    if outputLibName in s :
        logging.info("AR {}...".format(nameStr))
    else :
        logging.info("CC {}...".format(nameStr))

# 
VariantDir(objectOutputDir, tklPath, duplicate=0)

# 不使用默认环境
DefaultEnvironment(tools=[])

# 创建交叉编译器环境
env = Environment(tools=['gcc', 'ar'])

env['PRINT_CMD_LINE_FUNC'] = print_cmd_line

for (tool, name) in [
    ('CC', 'gcc'),
    ('AR', 'ar')
]:
    if sys.platform == 'win32':
        env[tool] = os.path.join(compilerPath, cross_compiler + name + ".exe")
    elif sys.platform.startswith('linux'):
        env[tool] = os.path.join(compilerPath, cross_compiler + name)
    else:
        logging.error("Error: Unsupported platform.")
        sys.exit(1)

env['AS'] = env['CC']  # 使用C编译器作为汇编器

env['OBJSUFFIX'] = ".o"

env['LIBPREFIX'] = "lib"
env['LIBSUFFIX'] = ".a"

env.Replace(CCCOM = ['$CC -o $TARGET -c $CFLAGS $CCFLAGS $_CCCOMCOM $SOURCES'])
env.Replace(ARCOM = ['$AR $ARFLAGS $TARGET $SOURCES'])

cFlags = os.path.join(flagsPath, "c_flags.txt")
tklFlags = [
    '@' + cFlags,
    '-DLFS_CONFIG=lfs_config.h',
    '-iprefix', vendorPath,
    '@' + vendorIncludeFlags,
    '-iprefix', openSdkPath,
    '@' + openSdkIncludeFlags,
    '-iprefix', tklPath,
    '@' + tklIncludeFlags,
]
envTKL = env.Clone(CFLAGS=tklFlags)

allSource = []

# tklFiles = envTKL.Glob(os.path.join(objectOutputDir, 'src', '*.c'))
# allSource.extend(tklFiles)

directories = [
    os.path.join('src', '*.c'),
    os.path.join('include', 'init', 'src', '*.c'),
    os.path.join('include', 'utilities', 'src', '*.c')
]
absolutePath = [os.path.join(objectOutputDir, src) for src in directories]
for directory in absolutePath:
    files = envTKL.Glob(directory)
    allSource.extend(files)

objectsTKL = envTKL.Object(allSource)

Default(objectsTKL)

# 添加后置动作
envTKL.AddPostAction(objectsTKL, [envTKL['AR'] + ' rcs ' + os.path.join(libOutputDir, outputLibName) + ' $TARGET'])

