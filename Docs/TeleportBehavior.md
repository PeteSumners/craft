# Teleport Behavior - /bgoto and /daily Commands

## Overview

Two teleportation commands provide instant navigation to Bible locations:

- **`/bgoto [BOOK [CHAPTER[:VERSE]]]`** - Navigate to any Bible book, chapter, or verse
- **`/daily [DAY]`** - Jump to daily reading for any day of the year (1-365)

Both commands use **pure database lookups** for precise positioning - no calculations, no guessing. You'll always land exactly at the requested location, looking down at it.

## Positioning Details

### Player Position After Teleport (/bgoto)

When you teleport to any Bible location, your position is set from the database:

- **X coordinate:** Exact X of the book/verse (from database)
- **Y coordinate:** Text Y + 104 blocks (viewing altitude, bird's eye view)
- **Z coordinate:** Exact Z where text starts (from database, no offset!)

### Player Position After Teleport (/daily)

When you teleport to a daily reading:

- **X coordinate:** -1200 (daily reading area is west of world origin)
- **Y coordinate:** 75 + 104 = 179 (viewing altitude)
- **Z coordinate:** Exact Z offset for that day (from database)

### Camera Orientation

After teleporting with either command:
- **rx (yaw):** 0° (looking south, along +Z axis)
- **ry (pitch):** -45° (looking down steeply at the text far below)

This means you're positioned directly over the text start, looking down and south.

## Coordinate Examples

### Info Area

```bash
> /bgoto

Position: (0, 177, 0)
```

- X=0: World origin
- Y=177: 102 blocks above text (text is at Y=75)
- Z=0: Right at the start of info text

### Genesis

```bash
> /bgoto Genesis

Position: (1000, 177, 0)
```

- X=1000: Genesis X position
- Y=177: 102 blocks above text
- Z=0: Right at Genesis 1:1 start

### Exodus

```bash
> /bgoto Exodus

Position: (1600, 177, 0)
```

- X=1600: Exodus X position (1000 + 600)
- Y=177: 102 blocks above text
- Z=0: Right at Exodus 1:1 start

### Chapter Navigation

```bash
> /bgoto Genesis 2

Position: (1000, 177, 360)
```

- X=1000: Still at Genesis X
- Y=177: 102 blocks above
- Z=360: Chapter 2 offset (~360 blocks per chapter)

### Verse Navigation

```bash
> /bgoto Genesis 1:3

Position: (1000, 177, 36)
```

- X=1000: Genesis X
- Y=177: 102 blocks above
- Z=36: Verse 3 offset (2 verses × 18 blocks/verse)

### Book with Spaces

```bash
> /bgoto 1 Thessalonians 5:2

Position: (48600, 177, 1458)
```

- X=48600: 1 Thessalonians X position
- Y=177: 102 blocks above
- Z=1458: Chapter 5, verse 2 offset

## Why These Coordinates?

### No Z Offset

**Previously:** We used Z offsets like `-20` or `-50` to place you "in front of" the text.

**Problem:** This caused issues:
- Genesis looked good but Exodus was off-center
- You had to fly to see the full book
- Inconsistent positioning between books

**Now:** We use exact Z coordinates where text starts:
- Z=0 for all book starts (Genesis, Exodus, etc.)
- Z=chapter_offset for chapters
- Z=chapter_offset+verse_offset for verses

**Result:** You're always perfectly centered over the text!

### Y Offset (+102)

We place you **102 blocks above** the text (Y=177 when text is Y=75):
- High viewing altitude for bird's eye perspective
- Can see entire layout and structure
- Perfect for understanding scale
- Consistent for all locations

### Camera Angle

Looking down at -45° pitch:
- Steep angle for viewing text far below
- Can see large sections of text at once
- Easy to understand spatial relationships
- Natural for navigation at this altitude

## Text Layout Understanding

Bible text renders **flat** along the ground:
- Starts at the saved Z position (usually 0)
- Extends **south** (positive Z direction)
- Each line wraps at configured width (usually 40 chars)
- Line spacing is consistent (~18 blocks per verse)

**Visualization (top-down view):**

```
        North (-Z)
             ↑
             |
    You spawn here at Z=0
             ↓
    ═══════════════════════
    Genesis 1:1 text starts
    ═══════════════════════
             ↓
    Genesis 1:2 (Z ≈ 18)
    ═══════════════════════
             ↓
    Genesis 1:3 (Z ≈ 36)
    ═══════════════════════
             ↓
        South (+Z)
```

When you teleport to Genesis, you appear at the top of this diagram, looking down and south.

## Spawn Behavior

### Initial Spawn

On first world load, you spawn at:
- **(0, 187, 0)** - Info area at viewing altitude
- Falls to Y≈177 due to gravity (lands on platform)
- Looking at info area text 102 blocks below

### Respawn After Death

Game respawns you at the same location:
- Back to (0, 187, 0)
- At the info area viewing altitude

## Navigation Tips

### Finding Your Position

After any `/bgoto`:
1. Look at chat - shows exact coordinates
2. Example: "Position: (1000, 77, 0)"
3. Enable F3 debug (if available) to see live coordinates

### Moving Around After Teleport

Once teleported:
- **W** - Move south (forward along text)
- **S** - Move north (backward)
- **A/D** - Strafe left/right
- **Space** - Fly up (if flying enabled)
- **Tab** - Toggle flying mode

### Viewing Text

Best viewing method:
1. Teleport to location
2. Enable flying (Tab)
3. Look down (you're already at -20° pitch)
4. Use W to move forward through the text
5. Adjust height with Space/Shift if needed

### Quick Navigation Workflow

### Using /bgoto

```bash
# Start at origin
/bgoto
Position: (0, 177, 0)

# Go to a book
/bgoto John
Position: (26200, 177, 0)

# Navigate to specific chapter
/bgoto John 3
Position: (26200, 177, 720)

# Jump to famous verse
/bgoto John 3:16
Position: (26200, 177, 990)

# Return to start
/bgoto
Position: (0, 177, 0)
```

### Using /daily

```bash
# Jump to today's reading (uses system date)
/daily
Teleported to Day 318 reading (Nov 14, 2026)!
Position: (-1200, 179, 75296)

# Jump to a specific day
/daily 1
Teleported to Day 1 reading (Jan 1, 2026)!
Position: (-1200, 179, 250)

# Jump to day 100
/daily 100
Teleported to Day 100 reading (Apr 10, 2026)!
Position: (-1200, 179, 23650)
```

## Database Tracking

### Bible Positions (`bible_position` table)

Each Bible location is saved during world generation:

| Book | Chapter | Verse | X | Y | Z |
|------|---------|-------|---|---|---|
| INFO | -1 | 0 | 0 | 75 | 0 |
| Genesis | 0 | 0 | 1000 | 75 | 0 |
| Genesis | 1 | 0 | 1000 | 75 | 0 |
| Genesis | 1 | 1 | 1000 | 75 | 0 |
| Exodus | 0 | 0 | 1600 | 75 | 0 |

**Special markers:**
- `chapter=0, verse=0` - Book start position
- `chapter=N, verse=0` - Chapter N start position
- `chapter=-1, verse=0` - Info area marker (book="INFO")

### Daily Reading Offsets (`daily_reading_z_offsets` table)

Each day's Z coordinate is saved during daily reading generation:

| Day | Z Offset |
|-----|----------|
| 1 | 250 |
| 2 | 580 |
| 3 | 920 |
| ... | ... |
| 365 | 88450 |

### How Teleportation Works

**For `/bgoto`:**
1. Parse book/chapter/verse from command
2. Query `bible_position` table
3. **If found:** Use exact X, Y, Z from database
4. **If NOT found:** Display error, teleport fails
5. Set player position to (X, Y+104, Z)
6. Set camera to look down and south

**For `/daily`:**
1. Parse day number (or use today's date)
2. Query `daily_reading_z_offsets` table
3. **If found (z_offset > 0):** Use exact Z offset from database
4. **If NOT found (z_offset == 0):** Display error, teleport fails
5. Set player position to (-1200, 179, z_offset)
6. Set camera to look down and south

**Important:** Both commands now use **PURE DATABASE LOOKUPS**. If the position isn't in the database, the teleport will fail with a clear error message. No math calculations, no guessing!

## Troubleshooting

### "ERROR: [Location] not found in database"

**Cause:** Bible/daily reading has not been generated yet, or generation was incomplete
**Fix:**
- Wait for world generation to complete (check console for "BIBLE WORLD GENERATION COMPLETE!")
- If generation failed, delete `craft.db` and restart to regenerate
- Check console output for generation errors

### "Text is off to the side"

**Cause:** Old database from previous version with different positioning
**Fix:** Delete `craft.db` and regenerate the world

### "I'm not directly over the text"

**Cause:** Possible database corruption or stale data from interrupted generation
**Fix:** Delete `craft.db` and regenerate from scratch

### "I see nothing below me"

**Cause:** Chunks not loaded yet, or text wasn't actually generated
**Fix:**
- Wait a few seconds for chunks to load
- Check console: Did generation complete successfully?
- If text doesn't exist, delete `craft.db` and regenerate
- Increase render distance in settings

### "/bgoto works but /daily doesn't"

**Cause:** Daily reading table not generated
**Fix:** Check console for "DAILY READING TABLE COMPLETE" message. If missing, daily reading wasn't generated. Delete `craft.db` and regenerate.

## Comparison to Old System

| Feature | Old System | New System |
|---------|-----------|------------|
| Lookup method | Database + Math fallback | **Pure database only** |
| Reliability | Sometimes wrong position | **Always accurate** |
| Error handling | Silent wrong teleport | **Clear error messages** |
| Z offset | -50 blocks before | 0 blocks (exact) |
| Info area Z | -20 offset | 0 (at origin) |
| Y offset | +2 blocks | +104 blocks |
| Positioning | In front of text | Directly over text |
| Consistency | Varied by book | Same for all books |
| Centering | Sometimes off | Always centered |
| View angle | -20° | -45° |

## Technical Implementation

### Code Flow (/bgoto)

```c
// Query database for position
int found = db_get_bible_position(book, chapter, verse, &pos_x, &pos_y, &pos_z);

if (found) {
    // Use exact database coordinates
    player->x = pos_x;          // Exact book X from database
    player->y = pos_y + 104;    // 104 blocks above text (viewing altitude)
    player->z = pos_z;          // Exact start Z from database (no offset!)

    // Set camera
    player->rx = 0;             // Look south
    player->ry = -45;           // Look down steeply

    printf("Teleported to %s %d:%d\n", book, chapter, verse);
} else {
    // Database lookup failed - show error
    printf("ERROR: %s %d:%d not found in database\n", book, chapter, verse);
    printf("The Bible has not been generated yet.\n");
    return;  // Teleport fails
}
```

### Code Flow (/daily)

```c
// Get Z offset from database
int z_offset = bible_get_daily_z_offset(day_of_year);

if (z_offset == 0) {
    // Database lookup failed - show error
    printf("ERROR: Day %d not found in database\n", day_of_year);
    printf("The daily reading table has not been generated yet.\n");
    return;  // Teleport fails
}

// Use exact database coordinates
player->x = DAILY_READING_X;  // -1200 (daily reading area)
player->y = DAILY_READING_Y + 104;  // 179 (viewing altitude)
player->z = z_offset;         // From database

// Set camera
player->rx = 0;   // Look south
player->ry = -45; // Look down steeply
```

**Key Change:** No fallback calculations! If the database doesn't have the position, the teleport fails with a clear error message. This ensures you never teleport to the wrong location.

## Summary

Both teleport commands (`/bgoto` and `/daily`) now use **pure database lookups**:

### Key Features
- ✅ **Exact positioning** - Retrieved directly from database, no calculations
- ✅ **Reliable** - Always teleports to the correct location
- ✅ **Clear errors** - Fails gracefully with helpful messages if data missing
- ✅ **Consistent** - Same positioning behavior for all locations

### Positioning
- **No Z offsets** - Exact Z coordinates from database (not -20 or -50)
- **Directly over text** - You hover 104 blocks above the text
- **Looking down** - Camera angled at -45° for optimal viewing
- **Facing south** - Aligned with text flow direction

### Requirements
- Bible and daily reading must be **fully generated** before teleporting
- Database must contain position data (check for completion messages)
- If teleport fails, regenerate world with fresh `craft.db`

**Remember:** You're hovering high above the text, looking down and south. Enable flying (Tab) to navigate!
