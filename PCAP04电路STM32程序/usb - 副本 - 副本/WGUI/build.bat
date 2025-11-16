@echo off
echo Building WPF USB Matrix Display...
dotnet build -c Release
if %ERRORLEVEL% EQU 0 (
    echo Build successful!
) else (
    echo Build failed!
    exit /b 1
)

