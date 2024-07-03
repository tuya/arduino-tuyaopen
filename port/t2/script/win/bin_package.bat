@echo off
rem 关闭命令回显
setlocal

@REM 使用 UTF-8 编码
chcp 65001

rem 检查是否提供了参数
if "%~1"=="" (
    echo 错误: 请提供编译器的绝对路径作为第一个参数。
    exit /b 1
)

if "%~2"=="" (
    echo 错误: 请提供 .axf 文件的绝对路径作为第二个参数。
    exit /b 1
)

if "%~3"=="" (
    echo 错误: 请提供输出文件路径作为第三个参数。
    exit /b 1
)

rem 获取第一个参数（编译器的绝对路径）
set "COMPILER_PATH=%~1"

rem 获取第二个参数（.axf 文件的绝对路径）
set "AXF_PATH=%~2"

rem 获取第三个参数（输出文件路径）
set "OUTPUT_PATH=%~3"

rem 打印编译器的绝对路径
echo 编译器的绝对路径: %COMPILER_PATH%

rem 打印 .axf 文件的绝对路径
echo .axf 文件的绝对路径: %AXF_PATH%

rem 打印输出文件路径
echo 输出文件路径: %OUTPUT_PATH%

rem 从 AXF_PATH 中提取文件名
for %%i in ("%AXF_PATH%") do set "AXF_NAME=%%~ni"
echo AXF 文件名: %AXF_NAME%

rem .axf -> .bin
%COMPILER_PATH%arm-none-eabi-objcopy.exe -O binary %AXF_PATH% %OUTPUT_PATH%\%AXF_NAME%.bin

rem 执行 bin_pack.bat, 参数一当前路径，参数二输出文件名称，无后缀
%~dp0\bin_pack.bat %~dp0 %OUTPUT_PATH% %AXF_NAME%


rem 结束脚本
exit /b 0