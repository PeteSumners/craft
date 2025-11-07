# Teleport Behavior - /bgoto Command

## Overview

The `/bgoto` command teleports you directly to Bible locations with precise positioning. You'll always land right at the start of the requested text, looking down at it.

## Positioning Details

### Player Position After Teleport

When you teleport to any location, your position is calculated as:

- **X coordinate:** Exact X of the book/verse
- **Y coordinate:** Text Y + 102 blocks (viewing altitude, bird's eye view)
- **Z coordinate:** Exact Z where text starts (no offset!)

### Camera Orientation

After teleporting:
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

## Database Tracking

Each teleport location is saved in the database:

| Book | Chapter | Verse | X | Y | Z |
|------|---------|-------|---|---|---|
| INFO | -1 | 0 | 0 | 75 | 0 |
| Genesis | 0 | 0 | 1000 | 75 | 0 |
| Genesis | 1 | 0 | 1000 | 75 | 0 |
| Genesis | 1 | 1 | 1000 | 75 | 0 |
| Exodus | 0 | 0 | 1600 | 75 | 0 |

When you teleport:
1. Query database for the location
2. Get exact X, Y, Z coordinates
3. Place you at (X, Y+2, Z)
4. Set camera to look down and south

## Troubleshooting

### "Text is off to the side"

**Cause:** Old database from previous version
**Fix:** Delete `craft.db` and regenerate the world

### "I'm not directly over the text"

**Cause:** Possible database corruption or old save
**Fix:** Try `/bgoto [book]` again, or regenerate

### "Estimated position used"

**Cause:** Bible not yet generated
**Fix:** Wait for generation to complete, coordinates will be exact

### "I see nothing below me"

**Cause:** Text rendering distance or chunks not loaded
**Fix:**
- Wait a few seconds for chunks to load
- Fly up and look around
- Increase render distance in settings

## Comparison to Old System

| Feature | Old System | New System |
|---------|-----------|------------|
| Z offset | -50 blocks before | 0 blocks (exact) |
| Info area Z | -20 offset | 0 (at origin) |
| Y offset | +2 blocks | +102 blocks |
| Positioning | In front of text | Directly over text |
| Consistency | Varied by book | Same for all books |
| Centering | Sometimes off | Always centered |
| View angle | -20° | -45° |

## Technical Implementation

### Code Flow

```c
// Get position from database
db_get_bible_position(book, chapter, verse, &pos_x, &pos_y, &pos_z);

// Place player
player->x = pos_x;          // Exact book X
player->y = pos_y + 102;    // 102 blocks above text (viewing altitude)
player->z = pos_z;          // Exact start Z (no offset!)

// Set camera
player->rx = 0;             // Look south
player->ry = -45;           // Look down steeply
```

### Offset Calculation (Estimated Mode)

If database position not found:

```c
// Book X position
x = BIBLE_START_X + (book_index * 600);

// Chapter/verse Z offset
z_offset = 0;
if (chapter > 0) {
    z_offset = (chapter - 1) * 360;  // ~360 blocks per chapter
    if (verse > 0) {
        z_offset += (verse - 1) * 18;  // ~18 blocks per verse
    }
}
z = BIBLE_START_Z + z_offset;  // No offset!
y = text_y + 102;              // Viewing altitude
```

## Summary

The new teleport system places you **exactly** at the text start coordinates:
- No Z offsets (was -20 or -50, now 0)
- Directly over the text
- Consistent positioning across all books
- Easy to navigate and read

Just remember: **You're hovering above the text, looking down!**
