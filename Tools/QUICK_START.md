# Quick Start Guide - Development Tools

## For Windows Users (MinGW)

### 1. Install Tools

**cppcheck (Static Analysis):**
```bash
choco install cppcheck
# Or download from: https://cppcheck.sourceforge.io/
```

**Dr. Memory (Memory Leak Detection):**
```bash
choco install drmemory
# Or download from: https://drmemory.org/
```

### 2. Run Static Analysis

```bash
cd Tools
run_cppcheck.bat
```

This will analyze all C source files and generate a report.

### 3. Build with AddressSanitizer

```bash
cd Tools
build_with_asan.bat
```

Then run the game from `Craft/build_asan/craft.exe`. Any memory errors will be reported in the console.

### 4. Check for Memory Leaks

```bash
cd Tools
run_drmemory.bat
```

This will launch the game with Dr. Memory monitoring. Play for a few seconds, then close it. Check the report for memory leaks.

## For Linux Users

### Install Tools

```bash
sudo apt-get install cppcheck clang-tidy valgrind
```

### Run Checks

```bash
cd Tools
make cppcheck    # Static analysis
make asan        # Build with AddressSanitizer
```

### Run Valgrind

```bash
cd Craft
valgrind --leak-check=full --show-leak-kinds=all ./craft
```

## Common Issues Found by These Tools

### Memory Leaks
- Forgot to free malloc'd memory
- Missing cleanup functions

### Buffer Overflows
- String operations without bounds checking
- Array index out of bounds

### Use After Free
- Using pointers after free()
- Dangling pointers

### Undefined Behavior
- Uninitialized variables
- Integer overflow
- Null pointer dereference

## VSCode Integration

1. Copy `Tools/vscode-settings.json` to `Craft/.vscode/settings.json`
2. Install C/C++ extension
3. Reload window
4. Static analysis will run automatically!

## Interpreting Results

### cppcheck Severity Levels
- **error**: Real bugs that need fixing
- **warning**: Potential issues
- **style**: Code style suggestions
- **performance**: Optimization opportunities
- **portability**: Platform-specific issues

### ASAN Output
```
ERROR: AddressSanitizer: heap-buffer-overflow
```
This means you accessed memory outside an allocated buffer. Look at the stack trace to find the location.

### Dr. Memory Output
```
Error #1: UNINITIALIZED READ
```
This means you read from uninitialized memory. Initialize your variables!

## Best Practices

1. **Run cppcheck** before every commit
2. **Test with ASAN** during development
3. **Use Dr. Memory** for thorough memory leak testing
4. **Fix warnings** - they often indicate real bugs
5. **Add unit tests** for complex functions

## Questions?

See the full README.md for more details.
