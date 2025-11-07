# Bible World Layout

## Overview

The Bible world is laid out along the X-axis (East-West), with all text at Y=75 (ground level).

## Coordinate System

```
         North (-Z)
              ↑
              |
West (-X) ←---+---→ East (+X)
              |
              ↓
         South (+Z)
```

## World Layout (Top-Down View)

```
                 Y = 75 (All text at this height)

West ←――――――――――――――――――――――――――――――――――――――――――――――――→ East

      INFO AREA              GENESIS         EXODUS          MALACHI
      (0, 75, 0)             (1000, 75, 0)   (1600, 75, 0)   (24400, 75, 0)
         │                       │               │               │
         │                       │               │               │
     [Platform]             [Book 1]         [Book 2]        [Book 39]
     Stone 15×15            Scroll           Scroll          Scroll

                            NEW TESTAMENT
                            MATTHEW         ...             REVELATION
                            (24400, 75, 0)                  (64600, 75, 0)
                                │                               │
                            [Book 40]                       [Book 66]
                            Scroll                          Scroll
```

## Spawn Point

**Coordinates:** (0, 85, -20)

- **X = 0:** World origin (at info area)
- **Y = 85:** 10 blocks above ground (you fall to Y=75)
- **Z = -20:** 20 blocks north of info area (in front of help text)

## Navigation Coordinates

### Info Area
- **Position:** (0, 75, 0)
- **Platform:** 15×15 stone at Y=74
- **Command:** `/bgoto` (no arguments)
- **Features:**
  - Help text and navigation instructions
  - Flight control guide
  - Pointer to Genesis

### Bible Books

All books are spaced 600 blocks apart along the X-axis:

| Book | X Coordinate | Distance from Origin |
|------|--------------|---------------------|
| **INFO** | 0 | 0 blocks |
| Genesis | 1000 | 1000 blocks east |
| Exodus | 1600 | 1600 blocks east |
| Leviticus | 2200 | 2200 blocks east |
| ... | ... | ... |
| Malachi | 24400 | 24.4 km east |
| Matthew | 24400 | 24.4 km east |
| ... | ... | ... |
| Revelation | 64600 | 64.6 km east |

**Formula:** `book_x = 1000 + (book_index × 600)`

### Chapter/Verse Offsets

Within each book:
- **Z offset per chapter:** ~360 blocks
- **Z offset per verse:** ~18 blocks

**Example: John 3:16**
- Book: John (book #43)
- X = 1000 + (42 × 600) = 26,200
- Z = 0 + (2 × 360) + (15 × 18) = 990

## Teleportation Guide

### Basic Navigation

```bash
/bgoto                     # Return to info area (0, 77, -20)
/bgoto Genesis             # Go to Genesis (1000, 77, -50)
/bgoto Exodus              # Go to Exodus (1600, 77, -50)
```

### Books with Spaces/Numbers

```bash
/bgoto 1 Samuel            # First Samuel
/bgoto 2 Kings             # Second Kings
/bgoto Song of Solomon     # Full name with spaces
/bgoto 1 Thessalonians 5   # Chapter in numbered book
/bgoto 2 Corinthians 3:16  # Verse in numbered book
```

### Chapter Navigation

```bash
/bgoto Genesis 1           # Genesis chapter 1
/bgoto Psalm 23            # Psalm 23
/bgoto John 3              # John chapter 3
```

### Verse Navigation

```bash
/bgoto Genesis 1:1         # First verse of Bible
/bgoto John 3:16           # Most famous verse
/bgoto Revelation 22:21    # Last verse of Bible
```

## Distance Reference

From info area (origin):

| Destination | Distance | Travel Time (Walking) | Travel Time (Flying) |
|-------------|----------|----------------------|---------------------|
| Genesis | 1 km | ~4 minutes | ~30 seconds |
| Malachi | 24.4 km | ~1.5 hours | ~8 minutes |
| Revelation | 64.6 km | ~4 hours | ~22 minutes |

**Walking speed:** ~4.3 blocks/sec
**Flying speed:** ~50 blocks/sec (spectator mode)

## Tips

1. **Getting Oriented:**
   - Type `/bgoto` to return to origin (0, 75, 0)
   - Press F3 to see coordinates (if debug enabled)
   - Look for stone platform = info area

2. **Fast Travel:**
   - Use `/bgoto [book] [chapter]` instead of walking
   - Enable flying (Tab) for better navigation
   - Fly up high to see layout from above

3. **Finding Your Way:**
   - X coordinate tells you which book (every 600 blocks = 1 book)
   - Z coordinate tells you which chapter/verse
   - Y should always be ~75 for text

4. **Coordinate Math:**
   - X < 0: You're west of info area
   - X = 0-1000: Between info and Genesis
   - X > 64600: Past Revelation (nothing there!)

## World Boundaries

- **West boundary:** Unlimited (but nothing there)
- **East boundary:** X = ~65,000 (end of Revelation)
- **North boundary:** Z can be negative (nothing there)
- **South boundary:** Each book extends ~20,000-30,000 blocks south
- **Height:** All text at Y = 75

## Technical Details

### Generation Order

1. Info area at (0, 75, 0)
2. Stone platform at (0, 74, 0)
3. Help text from Z=0 to Z=~200
4. Genesis at (1000, 75, 0)
5. Books 2-66 every 600 blocks east

### Database Tracking

Each book/chapter/verse position is saved:
- `("Genesis", 0, 0, 1000, 75, 0)` - Genesis start
- `("Genesis", 1, 0, 1000, 75, 0)` - Chapter 1 start
- `("Genesis", 1, 1, 1000, 75, 0)` - Verse 1
- `("INFO", -1, 0, 0, 75, 0)` - Info area marker

### Viewing Altitude

When teleporting:
- You spawn at **Y = text_y + 2**
- Example: text at Y=75 → you at Y=77
- Look down (-20° pitch) to read text below

## Comparison to Old System

| Feature | Old System | New System |
|---------|-----------|------------|
| Info location | (800, 375, -100) | (0, 75, 0) |
| Info height | 300 blocks above text | Same level as text |
| Genesis location | (1000, 75, 0) | (1000, 75, 0) [same] |
| Spawn height | Y=385 (very high) | Y=85 (near ground) |
| Origin marker | None | Info area at (0,0,0) |
| Coordinates shown | No | Yes, always |

## Quick Reference Card

```
╔═══════════════════════════════════════╗
║     BIBLE WORLD QUICK REFERENCE       ║
╠═══════════════════════════════════════╣
║ Origin (0,0,0): Info Area             ║
║ Genesis: 1000 blocks east             ║
║ All text: Y = 75 (ground level)       ║
║                                       ║
║ /bgoto             → Info area        ║
║ /bgoto Genesis     → First book       ║
║ /bgoto John 3:16   → Specific verse   ║
║                                       ║
║ Tab: Toggle flying                    ║
║ Space: Move up (while flying)         ║
║ W/A/S/D: Move horizontally            ║
╚═══════════════════════════════════════╝
```
