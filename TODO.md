# Chunky Craft - Project TODO

## Current Status (v1.0 - COMPLETED!)
✅ Voxel text rendering system implemented
✅ Multi-language support (English, Chinese, Japanese, Korean, Russian)
✅ GNU Unifont integration (58,000+ characters)
✅ Commands: `/vtext` and language presets
✅ **Bible rendering system implemented**
✅ **Complete KJV Bible support (31,102 verses)**
✅ **Comprehensive documentation created**
✅ **Code cleanup and error handling**
✅ **README updates**

---

## Completed (Latest Session)

### 1. Bible Rendering System ✅
- ✅ Analyzed KJV text file format (tab-delimited)
- ✅ Designed Bible rendering architecture
- ✅ Implemented bible.c and bible.h
- ✅ Added word wrapping functionality
- ✅ Support for single verses, ranges, and chapters
- ✅ Integrated with voxel text system
- ✅ Added `/bible` command to game
- ✅ Tested with Genesis and other books

### 2. Documentation ✅
- ✅ Created `Docs/` folder with comprehensive guides
- ✅ VoxelTextSystem.md - Complete API reference
- ✅ BibleRenderingSystem.md - Full Bible rendering guide
- ✅ Docs/README.md - Documentation index and quick start
- ✅ Updated Craft/README.md with new features
- ✅ Added command reference and examples
- ✅ Created CLAUDE.md for AI assistants

### 3. Code Cleanup ✅
- ✅ Reviewed and cleaned up voxel_text.c
- ✅ Added comprehensive error handling
- ✅ Added input validation (scale, block types, null checks)
- ✅ Improved error messages
- ✅ Added detailed code comments
- ✅ Added initialization status tracking

### 4. Documentation Update ✅
- ✅ Updated main README with enhanced features
- ✅ Added complete command reference
- ✅ Organized commands into categories
- ✅ Added Bible rendering examples
- ✅ Added technical implementation details

---

## Ideas for Future (v1.1+)

### Performance Enhancements
- [ ] Implement glyph caching system (cache frequently used characters)
- [ ] Add Bible verse indexing for faster lookup
- [ ] Optimize font file reading (mmap or memory caching)
- [ ] Batch rendering for multiple text blocks

### Feature Additions
- [ ] Dynamic text scaling support (runtime adjustable)
- [ ] Text rotation/orientation options (vertical, rotated)
- [ ] Support for additional Bible translations (NIV, ESV, etc.)
- [ ] Bible search functionality (keyword search)
- [ ] Text color/block type variations (gradients, multi-color)
- [ ] In-game text editor UI
- [ ] Save/load text placements (presets)
- [ ] Sign integration for smaller text
- [ ] Text effects (shadows, outlines, glow)
- [ ] Multi-column Bible layout
- [ ] Chapter/book navigation system
- [ ] Bookmarks for favorite verses

### Quality of Life
- [ ] Auto-positioning helper (finds good Y coordinate)
- [ ] Preview mode (show text bounds before rendering)
- [ ] Undo/redo for text placement
- [ ] Copy/paste text blocks
- [ ] Text alignment options (left, center, right)

---

## Notes

- Current font: GNU Unifont 17.0.03 (dual-licensed GPL2+/OFL 1.1)
- Bible: KJV Pure Cambridge Edition (public domain)
- World gen: Normal terrain (flatlands disabled by default)
- Text advances along X axis (left to right)
- CJK characters: 16×16 pixels, Latin: 8×16 pixels
- Total codebase: ~6,588 lines of C code
- Documentation: 3 comprehensive guides in Docs/
- Build system: CMake with MinGW support

## File Structure

```
craft/
├── Bible/
│   └── kjv.txt (KJV Bible text, 31,102 verses)
├── Craft/
│   ├── src/
│   │   ├── bible.c/h (Bible rendering system)
│   │   ├── voxel_text.c/h (Voxel text rendering)
│   │   └── main.c (game logic, commands)
│   ├── CMakeLists.txt (build configuration)
│   └── craft.exe (compiled game)
├── Docs/
│   ├── README.md (documentation index)
│   ├── VoxelTextSystem.md (text rendering guide)
│   └── BibleRenderingSystem.md (Bible guide)
├── Fonts/
│   ├── unifont-17.0.03.hex (58,000+ glyphs)
│   └── README.md (font documentation)
├── CLAUDE.md (AI assistant guide)
└── TODO.md (this file)
```
