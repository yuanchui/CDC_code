@echo off
chcp 65001 >nul
REM 更新配置文件脚本
REM 1. 从 1.txt 解析寄存器详细说明
REM 2. 运行 verify_config.exe 并解析输出

cd /d %~dp0

echo 正在从 1.txt 解析寄存器详细说明...python parse_register_docs.py

echo.
echo 正在编译 verify_config.exe...
gcc -o verify_config.exe verify_config.c ..\Core\Src\pcap04_reg.c -I ..\Core\Inc -D__GNUC__ 2>nul
if %ERRORLEVEL% NEQ 0 (
    gcc -o verify_config.exe verify_config.c ..\Core\Src\pcap04_reg.c -I ..\Core\Inc -D__GNUC__
)

if %ERRORLEVEL% EQU 0 (
    echo.
    echo 正在运行 verify_config.exe 并解析输出...python parse_verify_output.py
    echo.
    echo 配置更新完成！
) else (
    echo.
    echo 编译失败，请检查编译环境
)

pause

