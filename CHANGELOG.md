# Changelog - Bible World Updates

## Latest Changes (2025-11-06 - Update 5)

### Centered Text Layout at Info Area

**Major Change:** Info area text is now **centered at world origin**, not starting at origin.

**Text Positioning:**
- **Before:** Text started at X=0, extended to X=656
- **After:** Text starts at X=-328, extends to X=328 (centered at X=0)

**Calculation:**
- Longest line: 41 characters ("W/A/S/D - Move (horizontal in fly mode)")
- Character width: 8 pixels × scale 2 = 16 blocks/char
- Total width: 41 × 16 = 656 blocks
- Center offset: 656 ÷ 2 = 328 blocks
- Text starts at: 0 - 328 = **-328**

**Platform Changes:**
- ❌ **Removed:** Ground platform at Y=74 (not needed)
- ✅ **Kept:** Glass viewing platform at Y=177, centered at X=0

**Teleport Position:**
```bash
> /bgoto
Position: (0, 177, 0)
```

Now you teleport to the **center** of the info text (X=0), not the left edge!

**Benefits:**
- World origin (0, 0, 0) is meaningful - it's the center of info area
- Teleport lands you in the middle of the text
- More balanced visual layout
- Glass platform under your feet, text visible through it

**Console Output:**
```
Step 1: Generating info area at world origin (0, 75, 0)...
  Creating viewing platform...
    Viewing platform: 225 blocks at Y=177, centered at X=0
  Rendering help message...
  Help message complete!
```

## Latest Changes (2025-11-06 - Update 4)

### Increased Viewing Altitude

**Change:** All `/bgoto` teleports now place you **102 blocks above** the text (instead of 2 blocks).

**Before:**
- Teleported to Y = text_y + 2 (e.g., Y=77 when text at Y=75)
- Very close to text
- Limited overview

**After:**
- Teleported to Y = text_y + 102 (e.g., Y=177 when text at Y=75)
- Bird's eye view of text
- Can see entire layout
- Better for navigation

**Coordinate Examples:**
```bash
/bgoto
→ Position: (0, 177, 0)

/bgoto Genesis
→ Position: (1000, 177, 0)

/bgoto John 3:16
→ Position: (26200, 177, 990)
```

**Camera Angle:**
- Changed from -20° to **-45°** (looking more steeply down)
- Perfect for viewing text 102 blocks below

**Spawn Updates:**
- Spawn platform: Y=177 (was Y=75)
- Player spawn: Y=187 (was Y=85)
- Spawns at viewing altitude now

**Benefits:**
- See more text at once
- Easier to understand layout
- Better sense of scale
- Flying navigation more natural at this height

## Latest Changes (2025-11-06 - Update 3)

### Chapter & Verse Validation

**New Feature:** `/bgoto` now validates chapter and verse references!

**Invalid chapters:**
```bash
> /bgoto 2 John 3
ERROR: 2 John has only 1 chapter (you requested chapter 3)

> /bgoto Genesis 51
ERROR: Genesis has only 50 chapters (you requested chapter 51)
```

**Invalid verses:**
```bash
> /bgoto 2 John 1:14
ERROR: 2 John 1 has only 13 verses (you requested verse 14)

> /bgoto John 3:37
ERROR: John 3 has only 36 verses (you requested verse 37)
```

**Features:**
- Clear error messages explaining the problem
- Shows exact valid range (e.g., "has only 13 verses")
- Proper grammar (singular: "1 chapter", plural: "5 chapters")
- Prevents invalid teleports
- Instant validation before teleporting

**New Functions Added:**
- `bible_get_chapter_count(book)` - Returns number of chapters in a book
- `bible_get_verse_count(book, chapter)` - Returns number of verses in a chapter

**Books with 1 Chapter:**
Remember these only have 1 chapter (don't use chapter 2+):
- Obadiah, Philemon, 2 John, 3 John, Jude

**Common Mistakes Caught:**
- `/bgoto 2 John 2` → ❌ Only 1 chapter
- `/bgoto Philemon 2` → ❌ Only 1 chapter
- `/bgoto Psalm 151` → ❌ Only 150 chapters
- `/bgoto Revelation 23` → ❌ Only 22 chapters

## Latest Changes (2025-11-06 - Update 2)

### Info Area Moved to World Origin (0, 75, 0)

**Previously:** Info area was at X=800 (200 blocks left of Genesis)
**Now:** Info area is at exact world origin: **(0, 75, 0)**

**Benefits:**
- Easy to remember coordinates
- Clear reference point for navigation
- "Where am I?" → Just look at coordinates!

**Updated spawn:**
- Spawn position: (0, 85, -20)
- You spawn 20 blocks in front of info area
- 10 blocks above ground (gravity brings you down)

### Coordinate Display in /bgoto

All `/bgoto` commands now show **exact coordinates**:

**Examples:**
```
/bgoto
→ "Teleported to info area: (0, 77, -20)"
→ "Enable flying (Tab) to navigate"

/bgoto Genesis
→ "Teleported to Genesis"
→ "Position: (1000, 77, -50)"
→ "Enable flying (Tab) to navigate"

/bgoto 1 Thessalonians 5:2
→ "Teleported to 1 Thessalonians 5:2"
→ "Position: (48600, 77, 212)"
→ "Enable flying (Tab) to navigate"
```

**Features:**
- Shows your X, Y, Z position after teleporting
- Includes `[estimated]` tag if Bible not yet generated
- Same Y level for all positions (77 = ground + 2 blocks)

### Info Area Updates

**New text added:**
- "You are at world origin: (0, 75, 0)"
- Updated distance: "Genesis is 1000 blocks to the east →"

## Recent Changes (2025-11-06 - Update 1)

### Text Input System Enhancements

**Full cursor support added to in-game text input:**
- ←/→ arrow keys to move cursor
- Home/End to jump to start/end
- Backspace deletes before cursor
- Delete key deletes at cursor
- Character insertion at cursor position
- Ctrl+V pastes at cursor position
- Visual cursor indicator (|) shows current position

**Buffer size increase:**
- MAX_MESSAGES increased from 4 to 20
- Full `/bgoto` help text now displays in-game

### Bible World Generation Overhaul

**Info area repositioned to ground level:**
- Previously at Y=375 (300 blocks above text)
- Now at Y=75 (same height as Bible text)
- Located 200 blocks to the LEFT of Genesis (X=800)
- Includes 15×15 stone platform at Y=74
- All navigation text rendered at ground level

**Updated spawn point:**
- Now spawns at info area (X=800, Y=85, Z=-20)
- Players spawn at ground level, not 300 blocks up
- Unified experience: all text at same height

**Info area content:**
- "=== BIBLE WORLD NAVIGATION ===" header
- Command examples with spaces in book names
- Flight controls
- Pointer to Genesis ("Genesis is 200 blocks to the east →")

### /bgoto Command Improvements

**Space handling in book names:**
- Now correctly parses books with spaces and numbers
- Examples that now work:
  - `/bgoto 1 Thessalonians 5:2`
  - `/bgoto 2 Corinthians 3`
  - `/bgoto Song of Solomon`
  - `/bgoto 1 Samuel 17:47`

**Improved parsing algorithm:**
- Matches against full book list first
- Checks for exact prefix match
- Validates word boundaries
- Extracts chapter:verse from remainder

**No-argument behavior:**
- `/bgoto` (no args) now teleports to INFO area
- Uses database position if available
- Falls back to calculated position if not generated yet
- Places player 2 blocks above text, 20 blocks in front

### Database Changes

**New position tracking:**
- INFO area saved as: `("INFO", -1, 0, X, Y, Z)`
- Chapter -1, verse 0 used as marker for info area
- Allows precise teleportation to help text

## Technical Details

### Modified Files

1. **config.h**
   - MAX_MESSAGES: 4 → 20
   - BIBLE_SPAWN_PLATFORM_Y: 375 → 75
   - BIBLE_SPAWN_Y: 385 → 85
   - BIBLE_SPAWN_X: 1000 → 800
   - BIBLE_SPAWN_Z: -100 → -20

2. **main.c**
   - Added `typing_cursor` field to Model struct
   - Implemented cursor movement (arrows, Home, End)
   - Updated backspace/delete to work at cursor
   - Modified character insertion for cursor position
   - Rewrote /bgoto parsing to handle spaces
   - Updated /bgoto no-args to use INFO position

3. **bible.c**
   - Moved info area generation to X=800, Y=75, Z=0
   - Changed from high platform to ground-level stone base
   - Updated help text content
   - Added "Genesis is 200 blocks east" pointer
   - Saves INFO position to database

## Testing

### Text Input
1. Press `/` to open command prompt
2. Type some text
3. Use ← and → to move cursor
4. Try inserting characters in the middle
5. Test backspace and delete at different positions

### Bible Navigation
1. Start game (fresh world recommended)
2. Wait for Bible generation to complete
3. Type `/bgoto` - should teleport to info area at ground level
4. Look around - you should see the help text on stone platform
5. Walk east (hold W) to reach Genesis (~200 blocks)
6. Test commands:
   - `/bgoto 1 Thessalonians 5:2`
   - `/bgoto Song of Solomon`
   - `/bgoto 2 Peter 1`

### Visual Check
- Info area should be at same Y level as Bible books
- Stone platform visible under info text
- No floating platforms 300 blocks up
- All text readable from ground level

## Known Issues

None currently. If you find any, please report!

## Next Steps

- Consider adding abbreviations (e.g., "1 Thess" → "1 Thessalonians")
- Add autocomplete for book names
- Create waypoint markers between books
- Add compass pointing to nearest book
