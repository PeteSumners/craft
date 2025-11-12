# Craft Bible Rendering - Project TODO

## Current Status (v1.2 - World Generation + Navigation)
✅ Voxel text rendering system implemented
✅ Multi-language support (English, Chinese, Japanese, Korean, Russian)
✅ GNU Unifont integration (58,000+ characters)
✅ Bible rendering system (31,102 verses)
✅ **Bible world generation (all 66 books as persistent world)**
✅ **Database position tracking (precise verse coordinates)**
✅ **Teleportation system (`/bgoto` command)**
✅ **One-time generation (persists across sessions)**
✅ **Flat Bible rendering (horizontal layout for large texts)**
✅ **Enhanced flying controls (spectator-style horizontal movement)**

---

## Completed (Latest Session - v1.2)

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

### 5. Bible World Generation System ✅
- ✅ Implemented `bible_generate_world()` function
- ✅ All 66 books rendered as scrolls along Z-axis
- ✅ Books separated by 600 blocks on X-axis
- ✅ Glowing text using LIGHT_STONE blocks (brightness 15)
- ✅ Flatlands mode with optimized settings
- ✅ One-time generation with database flag

### 6. Database Position Tracking ✅
- ✅ Added `bible_position` table to database schema
- ✅ Track exact coordinates for every verse (31,102+ positions)
- ✅ Track chapter starts and book starts
- ✅ Functions: `db_insert_bible_position()`, `db_get_bible_position()`
- ✅ Thread-safe database operations

### 7. Bible Navigation System ✅
- ✅ Implemented `/bgoto` command (book/chapter/verse teleportation)
- ✅ Precise position lookup from database
- ✅ Fallback to estimation if database not generated
- ✅ Self-documenting help (lists all 66 books)
- ✅ Supports formats: `/bgoto Genesis`, `/bgoto Genesis 1`, `/bgoto John 3:16`

### 8. Enhanced Flying Controls ✅
- ✅ Spectator-style horizontal movement (50 blocks/sec)
- ✅ WASD moves in XZ plane only (ignores vertical look angle)
- ✅ Perfect for viewing flat Bible text from above
- ✅ Space bar for vertical movement

---

## Ideas for Future (v1.3+)

### High Priority
- [ ] **Resumable Bible generation** - Track which books are complete, resume from last incomplete book if generation interrupted
- [ ] Bible verse search functionality (keyword search with teleport)
- [ ] Waypoints/bookmarks system (save favorite locations)
- [ ] `/blist` command to show nearby verses/chapters

### Performance Enhancements
- [ ] Implement glyph caching system (cache frequently used characters)
- [ ] Optimize font file reading (mmap or memory caching)
- [ ] Batch rendering for multiple text blocks
- [ ] Async Bible generation (non-blocking startup)

### Feature Additions
- [ ] Dynamic text scaling support (runtime adjustable)
- [ ] Text rotation/orientation options (vertical, rotated)
- [ ] Support for additional Bible translations (NIV, ESV, etc.)
- [ ] Text color/block type variations (gradients, multi-color)
- [ ] Multiple Bible worlds (different translations as separate worlds)
- [ ] In-game text editor UI
- [ ] Save/load text placements (presets)
- [ ] Sign integration for smaller text
- [ ] Text effects (shadows, outlines, glow)
- [ ] Multi-column Bible layout
- [ ] Minimap showing current book/chapter location

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
- World gen: Flatlands mode enabled (Y=32 ground level)
- Bible layout: Books along X-axis (600 blocks apart), scrolls along Z-axis
- Bible position: X=1000 (Genesis) to X=40600 (Revelation)
- Text rendering: Flat horizontal layout, readable from above
- CJK characters: 16×16 pixels, Latin: 8×16 pixels
- Flying speed: 50 blocks/sec (spectator-style)
- Total verses tracked: 31,102 with precise coordinates
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
