# Large-Scale Bible Rendering Guide

## Strategies for Rendering Large Portions

### Problem: Vertical Space

A single verse with wrapping can be 2-4 lines tall (~40-80 voxels).
A 31-verse chapter could require **1,000+ voxels of vertical space**!

### Solution Strategies

## Strategy 1: Multi-Column Layout (Recommended)

Render chapters side-by-side instead of stacking vertically.

### Example: Genesis 1 (31 verses) in 3 columns

```bash
# Column 1: Verses 1-10
/bible Genesis 1:1-10 0 200 0 3 45

# Column 2: Verses 11-20
/bible Genesis 1:11-20 400 200 0 3 45

# Column 3: Verses 21-31
/bible Genesis 1:21-31 800 200 0 3 45
```

**Result**: Spreads horizontally instead of vertically!

### Calculating Column Positions

- **Width per character**: ~8 voxels
- **Characters per line**: 45
- **Total width**: 45 × 8 = 360 voxels
- **Spacing between columns**: 40 voxels
- **Column positions**: 0, 400, 800, 1200...

## Strategy 2: Compact Width (Narrow Columns)

Use narrower text for more columns:

```bash
# Very narrow columns (30 chars)
/bible Genesis 1:1-5 0 200 0 3 30
/bible Genesis 1:6-10 250 200 0 3 30
/bible Genesis 1:11-15 500 200 0 3 30
/bible Genesis 1:16-20 750 200 0 3 30
```

**Width calculation**: 30 chars × 8 voxels = 240 voxels per column

## Strategy 3: Book-by-Book Layout

Create a "library" with organized structure:

### Genesis Layout
```bash
# Chapter 1
/bible Genesis 1:1-15 0 200 0 3 40
/bible Genesis 1:16-31 350 200 0 3 40

# Chapter 2
/bible Genesis 2:1-12 0 200 500 3 40
/bible Genesis 2:13-25 350 200 500 3 40

# Chapter 3
/bible Genesis 3:1-12 0 200 1000 3 40
/bible Genesis 3:13-24 350 200 1000 3 40
```

**Organization**: Each chapter gets its own Z-level!

## Strategy 4: Horizontal "Pages"

Render books like a physical book with "pages":

```bash
# Page 1 (left)
/bible Genesis 1:1-15 0 200 0 3 40

# Page 2 (right)
/bible Genesis 1:16-31 400 200 0 3 40

# Page 3 (left, next spread)
/bible Genesis 2:1-12 0 200 500 3 40

# Page 4 (right, next spread)
/bible Genesis 2:13-25 400 200 500 3 40
```

**Result**: Walk through Bible like turning pages!

## Optimal Parameters for Large Rendering

### For Maximum Readability
- **Width**: 40-50 characters
- **Line spacing**: 8-10 voxels
- **Block type**: Stone (3) or Brick (4)
- **Y position**: 150-200 (high visibility)

### For Compact/Space-Saving
- **Width**: 30-35 characters
- **Line spacing**: 5 voxels
- **Block type**: Any solid block
- **Y position**: 100-150

### For Monument/Display
- **Width**: 50-60 characters
- **Line spacing**: 10-15 voxels
- **Block type**: Brick (4) or Stone (3)
- **Y position**: 200-250

## Example: Entire Book Layouts

### Render All of Psalm 119 (176 verses!)

**Option A: Vertical Stacking (requires Y=500+)**
```bash
/bible Psalm 119:1-22 0 500 0 3 40
/bible Psalm 119:23-44 0 400 0 3 40
/bible Psalm 119:45-66 0 300 0 3 40
/bible Psalm 119:67-88 0 200 0 3 40
/bible Psalm 119:89-110 0 100 0 3 40
# ... continues
```

**Option B: Horizontal Grid (better!)**
```bash
# Row 1
/bible Psalm 119:1-20 0 200 0 3 35
/bible Psalm 119:21-40 300 200 0 3 35
/bible Psalm 119:41-60 600 200 0 3 35
/bible Psalm 119:61-80 900 200 0 3 35

# Row 2 (different Z)
/bible Psalm 119:81-100 0 200 400 3 35
/bible Psalm 119:101-120 300 200 400 3 35
/bible Psalm 119:121-140 600 200 400 3 35
/bible Psalm 119:141-160 900 200 400 3 35

# Row 3
/bible Psalm 119:161-176 0 200 800 3 35
```

### Render Entire Gospel of John (21 chapters)

**Grid Layout (4 chapters per row)**
```bash
# Row 1: Chapters 1-4
/bible John 1 0 200 0 3 40
/bible John 2 500 200 0 3 40
/bible John 3 1000 200 0 3 40
/bible John 4 1500 200 0 3 40

# Row 2: Chapters 5-8
/bible John 5 0 200 500 3 40
/bible John 6 500 200 500 3 40
/bible John 7 1000 200 500 3 40
/bible John 8 1500 200 500 3 40

# Row 3: Chapters 9-12
/bible John 9 0 200 1000 3 40
/bible John 10 500 200 1000 3 40
/bible John 11 1000 200 1000 3 40
/bible John 12 1500 200 1000 3 40

# Continue pattern...
```

## Time Estimates

Based on current performance:

| Content | Verses | Estimated Time |
|---------|--------|----------------|
| Single verse | 1 | < 1 second |
| Small passage (5 verses) | 5 | ~2 seconds |
| Medium chapter (20 verses) | 20 | ~10 seconds |
| Large chapter (50 verses) | 50 | ~30 seconds |
| Huge chapter (150 verses) | 150 | ~90 seconds |
| Entire book (e.g., Genesis) | 1,533 | ~25 minutes |

**Tip**: Render during breaks or while doing other tasks!

## Automated Batch Rendering

### Create a Batch Script

Create `render_genesis.txt` with commands:

```
/bible Genesis 1:1-15 0 200 0 3 40
/bible Genesis 1:16-31 400 200 0 3 40
/bible Genesis 2:1-12 0 200 500 3 40
/bible Genesis 2:13-25 400 200 500 3 40
... etc
```

Then in game, copy-paste commands one at a time.

**Note**: Current system requires manual command entry. Automation would require code changes.

## World Planning

### Calculate Space Requirements

**Vertical Formula**:
```
Height needed = (verses × 3 lines × 20 voxels) + (verses × spacing)
              ≈ verses × 70 voxels

Example: 30 verses = 30 × 70 = 2,100 voxels
```

**Horizontal Formula**:
```
Width needed = (chars per line × 8 voxels) + char_spacing
             ≈ chars × 9 voxels

Example: 40 chars = 40 × 9 = 360 voxels per column
```

### Grid Planning Example

To render Genesis (50 chapters):

```
Grid: 5 columns × 10 rows
Column width: 400 voxels each
Row depth: 500 voxels each

Total area: 2,000 × 5,000 voxels (X × Z)
Y position: 150-200
```

## Performance Tips

### 1. Render in Batches
Do 5-10 chapters at a time, take a break.

### 2. Use Teleport
```
/pq 0 0    # Teleport to starting chunk
/spawn     # Return to spawn
```

### 3. Fly Mode
Press Tab to toggle flying for easy positioning.

### 4. Mark Positions
Place colored blocks to mark where you'll render next.

### 5. Save Progress
The world auto-saves every 5 seconds. Your renders persist!

## Advanced Layouts

### "Scroll" Layout (Continuous Horizontal)

Render entire books in one long horizontal line:

```bash
# Walk along X-axis to read
/bible Genesis 1 0 200 0 3 40
/bible Genesis 2 600 200 0 3 40
/bible Genesis 3 1200 200 0 3 40
/bible Genesis 4 1800 200 0 3 40
# Continue for entire book...
```

### "Library" Layout (3D Grid)

Create a massive 3D structure:

```bash
# Floor 1 (Y=100): Old Testament
/bible Genesis 1 0 100 0 3 40
/bible Genesis 2 500 100 0 3 40
# ...

# Floor 2 (Y=200): Psalms & Wisdom
/bible Psalm 1 0 200 0 3 40
/bible Psalm 2 500 200 0 3 40
# ...

# Floor 3 (Y=300): New Testament
/bible Matthew 1 0 300 0 3 40
/bible Matthew 2 500 300 0 3 40
# ...
```

### "Cathedral" Layout (Monumental)

Create a cathedral with Bible verses as walls:

```bash
# North wall: Genesis 1
/bible Genesis 1:1-10 0 150 0 3 50
/bible Genesis 1:11-20 0 120 0 3 50
/bible Genesis 1:21-31 0 90 0 3 50

# East wall: John 1
/bible John 1:1-14 500 150 0 3 50
# ...

# South wall: Revelation 22
# West wall: Psalm 23
```

## Practical Example: The Four Gospels

### Compact 2×2 Grid

```bash
# Matthew (top-left)
/bible Matthew 1 0 200 0 3 35
/bible Matthew 2 0 180 0 3 35
# ... continue Matthew

# Mark (top-right)
/bible Mark 1 400 200 0 3 35
/bible Mark 2 400 180 0 3 35
# ... continue Mark

# Luke (bottom-left)
/bible Luke 1 0 200 500 3 35
/bible Luke 2 0 180 500 3 35
# ... continue Luke

# John (bottom-right)
/bible John 1 400 200 500 3 35
/bible John 2 400 180 500 3 35
# ... continue John
```

**Total verses**: ~4,700 verses
**Estimated time**: ~2 hours
**Result**: All four Gospels in one monument!

## Command Shortcuts

Save commonly used parameters:

**For quick passages**:
```bash
# Template: /bible BOOK CH:V-V 0 200 0 3 40
/bible Romans 8:28-39 0 200 0 3 40
```

**For chapters**:
```bash
# Template: /bible BOOK CH 0 200 0 3 40
/bible Psalm 23 0 200 0 3 40
```

## Future Enhancements

These would make large-scale rendering easier:

1. **Auto-layout**: `/bible Genesis --auto-columns 4`
2. **Batch rendering**: `/bible Genesis 1-5 --grid 0,200,0`
3. **Templates**: `/bible-layout gospel-grid Matthew Mark Luke John`
4. **Progress indicators**: Show % complete during rendering
5. **Resume support**: Continue interrupted large renders

## Recommended Reading Orders

### New to Bible Rendering?
Start small and work up:

1. **Psalm 23** (6 verses) - Classic, short
2. **John 3:16-21** (6 verses) - Famous passage
3. **Genesis 1** (31 verses) - Creation story
4. **Romans 8** (39 verses) - Paul's letter
5. **Gospel of Mark** (shortest gospel, ~670 verses)

### Building a Monument?

**Top Choices**:
- **Sermon on the Mount**: Matthew 5-7 (111 verses)
- **Ten Commandments**: Exodus 20:1-17 (17 verses)
- **Lord's Prayer**: Matthew 6:9-13 (5 verses)
- **Beatitudes**: Matthew 5:3-12 (10 verses)
- **Love Chapter**: 1 Corinthians 13 (13 verses)

## Questions?

See also:
- `BibleRenderingSystem.md` - Full Bible system docs
- `VoxelTextSystem.md` - Text rendering details
- `DEBUGGING.md` - Troubleshooting guide
