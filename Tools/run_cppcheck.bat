@echo off
REM Static analysis with cppcheck
echo Running cppcheck static analysis...
echo.

cd ..
cppcheck --enable=all --inconclusive --std=c99 ^
  --suppress=missingIncludeSystem ^
  --suppress=unusedFunction ^
  -I Craft/src -I Craft/deps ^
  Craft/src/*.c ^
  2> Tools/cppcheck-report.txt

echo.
echo Analysis complete! Report saved to Tools/cppcheck-report.txt
type Tools\cppcheck-report.txt
