@echo off
setlocal

@REM use utf-8
chcp 65001

REM %1 packtools path
REM %2 build temp path
REM %3 build project name

@REM echo %1 %2 %3

@REM 路径中的 \ 转换为 \\，否则会被当成转义字符
set tmp_path=%1
set pack_tools_path=%tmp_path:\=\\%
set tmp_path=%2
set build_temp_path=%tmp_path:\=\\%
set project_name=%3

@REM echo %pack_tools_path%
@REM echo %build_temp_path%
@REM echo %project_name%

@REM 判断路径是否存在
if not exist "%pack_tools_path%" (
	echo "%pack_tools_path% not exist"
	exit /b 1
)

if not exist "%build_temp_path%" (
	echo "%build_temp_path% not exist"
	exit /b 1
)

@REM 切换到 Arduino 草图临时目录
cd /d "%build_temp_path%"

@REM 删除旧文件
IF EXIST "all_1.00.bin" (
	DEL "all_1.00.bin"
	@REM echo "delete all_1.00.bin"
)
IF EXIST "config.json" (
	DEL "config.json"
	@REM echo "delete config.json"
)
IF EXIST "%project_name%.cpr" (
	DEL "%project_name%.cpr"
	@REM echo "delete %project_name%.cpr"
)
IF EXIST "%project_name%.out" (
	DEL "%project_name%.out"
	@REM echo "delete %project_name%.out"
)
IF EXIST "%project_name%_enc.bin" (
	DEL "%project_name%_enc.bin"
	@REM echo "delete %project_name%_enc.bin"
)
IF EXIST "%project_name%_enc_uart_1.00.bin" (
	DEL "%project_name%_enc_uart_1.00.bin"
	@REM echo "delete %project_name%_enc_uart_1.00.bin"
)
IF EXIST "%project_name%_UA.bin" (
	DEL "%project_name%_UA.bin"
	@REM echo "delete %project_name%_UA.bin"
)

echo "encrypt bin"
echo %pack_tools_path%\\encrypt.exe %build_temp_path%\\%project_name%.bin
"%pack_tools_path%\\encrypt.exe" "%build_temp_path%\\%project_name%.bin" 510fb093 a3cbeadc 5993a17e c7adeb03 10000

COPY "%pack_tools_path%\\pack_config_def.json" "%build_temp_path%\\config.json"

@REM 替换 config.json 中的 bootloader 和 app 路径
set t2_bootloader_path="%pack_tools_path%\\t2_bootloader_enc.bin"
set build_app_path="%build_temp_path%\\%project_name%_enc.bin"
powershell -Command "(Get-Content '%build_temp_path%\\config.json') -replace 't2_bootloader_enc.bin', '%t2_bootloader_path%' | Set-Content '%build_temp_path%\\config.json'"
powershell -Command "(Get-Content '%build_temp_path%\\config.json') -replace 'bk7231_common_1.0.1_enc.bin', '%build_app_path%' | Set-Content '%build_temp_path%\\config.json'"

@REM 打包
"%pack_tools_path%\\beken_packager.exe" %build_temp_path%\\config.json

@REM 重命名QIO固件
rename "all_1.00.bin" "%project_name%_QIO.bin"

@REM 重命名应用固件
rename "%project_name%_enc_uart_1.00.bin" "%project_name%_UA.bin"

endlocal
