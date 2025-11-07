@echo off
REM Run all available code quality checks
echo ======================================
echo Running all code quality checks
echo ======================================
echo.

REM Check if tools are available
where cppcheck >nul 2>&1
if %errorlevel% neq 0 (
    echo WARNING: cppcheck not found. Install from https://cppcheck.sourceforge.io/
    echo Skipping static analysis...
) else (
    call run_cppcheck.bat
)

echo.
echo ======================================
echo.

where drmemory >nul 2>&1
if %errorlevel% neq 0 (
    echo WARNING: Dr. Memory not found. Install from https://drmemory.org/
    echo Skipping memory leak detection...
) else (
    echo Dr. Memory check skipped (requires interactive run)
    echo Run manually: run_drmemory.bat
)

echo.
echo ======================================
echo All automated checks complete!
echo ======================================
