@echo off
REM Build with AddressSanitizer for memory error detection
echo Building with AddressSanitizer...
echo.

cd ..\Craft

REM Create a separate build directory for ASAN builds
if not exist build_asan mkdir build_asan
cd build_asan

REM Configure with ASAN flags
cmake -G "MinGW Makefiles" ^
  -DCMAKE_C_FLAGS="-fsanitize=address -fno-omit-frame-pointer -g" ^
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address" ^
  ..

REM Build
mingw32-make

echo.
echo Build complete! Run craft.exe from build_asan directory.
echo ASAN will report memory errors at runtime.
echo.
echo Usage:
echo   cd Craft/build_asan
echo   ./craft.exe
