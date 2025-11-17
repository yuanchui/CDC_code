@echo off
setlocal

REM Change to script directory
cd /d "%~dp0"

REM Ensure venv python exists
if not exist "venv\Scripts\python.exe" (
  echo [ERROR] venv\Scripts\python.exe not found. Please create venv first.
  exit /b 1
)

REM Install Nuitka (and dependencies) into local venv if missing
"venv\Scripts\python.exe" -m pip show nuitka >nul 2>&1
if errorlevel 1 (
  echo Installing Nuitka...
  "venv\Scripts\python.exe" -m pip install --quiet nuitka zstandard ordered-set
)

set APPNAME=PCap04_GUI
set ENTRY=main.py
set ICON=app.ico

REM Detect CPU cores for parallel build
set JOBS=%NUMBER_OF_PROCESSORS%
if "%JOBS%"=="" set JOBS=4

REM Cleanup previous output
if exist %APPNAME%.dist rmdir /s /q %APPNAME%.dist
if exist %APPNAME%.build rmdir /s /q %APPNAME%.build
if exist %APPNAME%.exe del /f /q %APPNAME%.exe

REM Build standalone (distributed folder) with PyQt5 plugin and console window
"venv\Scripts\python.exe" -m nuitka ^
  --standalone ^
  --enable-plugin=pyqt5 ^
  --windows-console-mode=force ^
  --nofollow-import-to=tkinter ^
  --nofollow-import-to=PyQt6 ^
  --nofollow-import-to=pyqt6 ^
  --nofollow-import-to=pyqt6_qt6 ^
  --include-data-files=command_config.json=command_config.json ^
  --include-data-files=app.ico=app.ico ^
  --include-data-files=pcap04_data.db=pcap04_data.db ^
  --windows-icon-from-ico=%ICON% ^
  --include-qt-plugins=platforms,styles,imageformats ^
  --noinclude-qt-translations ^
  --lto=no ^
  --jobs=%JOBS% ^
  --output-filename=%APPNAME%.exe ^
  %ENTRY%

if errorlevel 1 (
  echo [ERROR] Nuitka build failed.
  echo Hint: If still slow, try uninstalling PyQt6 from venv: venv\Scripts\pip uninstall -y PyQt6 pyqt6-qt6 pyqt6-sip
  exit /b 1
)

if exist "%APPNAME%.dist\%APPNAME%.exe" (
  echo Build succeeded: %APPNAME%.dist\%APPNAME%.exe
  echo This is a distributed folder build with console. Run the EXE directly.
) else (
  echo [ERROR] Build output not found.
  exit /b 1
)

endlocal
