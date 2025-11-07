# Craft Development Tools

This directory contains tools and scripts for code quality checking, memory leak detection, and static analysis.

## Available Tools

### 1. Static Analysis (cppcheck)

Cppcheck is a static analysis tool for C/C++ code.

**Installation (Windows):**
```bash
# Download from https://cppcheck.sourceforge.io/
# Or via chocolatey:
choco install cppcheck
```

**Usage:**
```bash
cd Tools
./run_cppcheck.bat
```

### 2. Memory Leak Detection (Dr. Memory)

Dr. Memory is a memory debugging tool for Windows (similar to Valgrind on Linux).

**Installation (Windows):**
```bash
# Download from https://drmemory.org/
# Or via chocolatey:
choco install drmemory
```

**Usage:**
```bash
cd Tools
./run_drmemory.bat
```

### 3. Address Sanitizer (Built into GCC/Clang)

AddressSanitizer detects memory errors at runtime.

**Usage:**
```bash
cd Tools
./build_with_asan.bat
# Then run: ../Craft/craft.exe
```

### 4. Clang-Tidy

Modern C++ linter and static analyzer.

**Installation:**
```bash
# Install LLVM/Clang from https://releases.llvm.org/
```

**Usage:**
```bash
cd Tools
./run_clang_tidy.bat
```

## Running All Checks

To run all available tools:
```bash
cd Tools
./run_all_checks.bat
```

## IDE Integration

### Visual Studio Code

Add these extensions:
- C/C++ (Microsoft)
- C/C++ Themes
- clangd (optional, better than MS C++ IntelliSense)

Copy `vscode-settings.json` to `.vscode/settings.json` in the Craft directory.

### CLion

CLion has built-in support for:
- Clang-Tidy
- Cppcheck
- Valgrind (on Linux)

Enable them in: Settings → Editor → Inspections → C/C++

## Continuous Integration

See `ci-checks.yml` for GitHub Actions integration.
