@echo off
echo Building USB Matrix Display Application...
echo.

cd /d %~dp0

dotnet build -c Release

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful!
    echo Executable location: bin\Release\net6.0-windows\USBMatrixDisplay.exe
) else (
    echo.
    echo Build failed!
    pause
    exit /b 1
)

pause

