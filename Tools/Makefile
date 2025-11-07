# Makefile for Craft development tools
# Usage: make [target]

.PHONY: all cppcheck asan clean help

all: cppcheck

help:
	@echo "Craft Development Tools"
	@echo "======================="
	@echo ""
	@echo "Available targets:"
	@echo "  make cppcheck   - Run static analysis with cppcheck"
	@echo "  make asan       - Build with AddressSanitizer"
	@echo "  make clean      - Clean analysis reports"
	@echo "  make help       - Show this help"

cppcheck:
	@echo "Running cppcheck static analysis..."
	cppcheck --enable=all --inconclusive --std=c99 \
	  --suppress=missingIncludeSystem \
	  --suppress=unusedFunction \
	  -I ../Craft/src -I ../Craft/deps \
	  ../Craft/src/*.c \
	  2> cppcheck-report.txt
	@echo "Analysis complete! Report: cppcheck-report.txt"
	@cat cppcheck-report.txt

asan:
	@echo "Building with AddressSanitizer..."
	cd ../Craft && \
	  mkdir -p build_asan && \
	  cd build_asan && \
	  cmake -DCMAKE_C_FLAGS="-fsanitize=address -fno-omit-frame-pointer -g" \
	        -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address" .. && \
	  make
	@echo "Build complete! Run: Craft/build_asan/craft"

clean:
	rm -f cppcheck-report.txt
	rm -f drmemory-*.txt
	rm -rf ../Craft/build_asan
