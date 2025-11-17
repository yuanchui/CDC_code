@echo off
echo Publishing WPF USB Matrix Display Application as single-file executable...
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
    echo Executable location: bin\Release\net8.0-windows\win-x64\publish\USBMatrixDisplay.exe
    echo.
    echo Copying exe to current directory...
    copy /Y "bin\Release\net8.0-windows\win-x64\publish\USBMatrixDisplay.exe" "USBMatrixDisplay.exe"
    set COPYERROR=%ERRORLEVEL%
    if %COPYERROR% EQU 0 (
        if exist "USBMatrixDisplay.exe" (
            echo.
            echo Standalone exe copied to: %CD%\USBMatrixDisplay.exe
            echo.
            echo This is a standalone executable that requires no additional files.
            echo You can move this exe to any Windows x64 machine and run it directly.
            echo.
            echo Optimization settings applied:
            echo   - Single file packaging
            echo   - Compression enabled
            echo   - Debug symbols removed
            echo   - Code optimized
            echo   - ReadyToRun disabled (smaller size)
        ) else (
            echo.
            echo Warning: Copy command completed but file not found!
        )
    ) else (
        echo.
        echo Warning: Failed to copy exe to current directory, but original file exists in publish folder.
    )
) else (
    echo.
    echo Publish failed!
    pause
    exit /b 1
)

pause

