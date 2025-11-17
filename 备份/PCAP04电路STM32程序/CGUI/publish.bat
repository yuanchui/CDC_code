@echo off
echo Publishing USB Matrix Display Application as single-file executable...
echo.

cd /d %~dp0

echo Building optimized release version...
dotnet clean -c Release
echo.

dotnet publish -c Release -r win-x64 ^
    --self-contained true ^
    /p:PublishSingleFile=true ^
    /p:IncludeNativeLibrariesForSelfExtract=true ^
    /p:EnableCompressionInSingleFile=true ^
    /p:PublishReadyToRun=false ^
    /p:DebugType=none ^
    /p:DebugSymbols=false ^
    /p:Optimize=true

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Publish successful!
    echo.
    echo Executable location: bin\Release\net6.0-windows\win-x64\publish\USBMatrixDisplay.exe
    echo.
    echo Copying exe to current directory...
    copy /Y "bin\Release\net6.0-windows\win-x64\publish\USBMatrixDisplay.exe" "USBMatrixDisplay.exe" >nul
    if %ERRORLEVEL% EQU 0 (
        echo.
        echo Standalone exe copied to: %CD%\USBMatrixDisplay.exe
        echo.
        echo This is a standalone executable that requires no additional files.
        echo You can move this exe to any Windows x64 machine and run it directly.
    )
) else (
    echo.
    echo Publish failed!
    pause
    exit /b 1
)

pause

