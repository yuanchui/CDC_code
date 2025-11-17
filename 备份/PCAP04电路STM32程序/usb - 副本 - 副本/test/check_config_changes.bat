@echo off
chcp 65001 >nul
echo ========================================
echo PCAP04 Config Change Checker
echo ========================================

where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] GCC not found in PATH!
    pause
    exit /b
)

echo [INFO] GCC found, compiling...
gcc -o verify_config.exe verify_config.c ../Core/Src/pcap04_reg.c -I ../Core/Inc
set compile_ok=%errorlevel%

if %compile_ok% equ 0 (
    echo.
    echo =========== Running verify_config ===========
    verify_config.exe
) else (
    echo.
    echo [ERROR] Compilation failed, exe not generated!
)

pause