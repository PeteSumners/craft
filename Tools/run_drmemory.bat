@echo off
REM Memory leak detection with Dr. Memory
echo Running Dr. Memory for memory leak detection...
echo This will launch the game - play for a few seconds then close it.
echo.

cd ..\Craft

REM Make sure the executable exists
if not exist craft.exe (
    echo Error: craft.exe not found. Build the project first.
    exit /b 1
)

REM Run with Dr. Memory
drmemory -brief -batch -- craft.exe

echo.
echo Memory check complete! Check results.txt for details.
