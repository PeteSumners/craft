# Bible Rendering System

## Overview

The Bible Rendering System allows you to render Bible verses, passages, and entire chapters as voxel text in the Craft game world. It uses the King James Version (KJV) text and integrates with the Voxel Text Rendering System to display Scripture in 3D blocks.

## Features

- **Complete KJV Bible**: All 66 books, 31,102 verses
- **Flexible Rendering**: Single verses, verse ranges, or entire chapters
- **Word Wrapping**: Automatic text wrapping for long verses
- **Configurable Layout**: Adjustable width, line spacing, and block types
- **All Book Names**: Support for single and multi-word book names (e.g., "Genesis", "1 Kings", "Song of Solomon")

## Quick Start

### In-Game Commands

#### Render a Single Verse
```
/bible Genesis 1:1 0 100 0 3 60
```
- Book: Genesis
- Chapter: 1
- Verse: 1
- Position: (0, 100, 0)
- Block Type: 3 (Stone)
- Max Width: 60 characters

#### Render a Verse Range
```
/bible John 3:16-17 0 100 0 4 50
```
Renders John 3:16 and 3:17 with brick blocks, 50 character width.

#### Render an Entire Chapter
```
/bible Psalm 23 0 100 0 3 60
```
**Warning**: Full chapters can be very large!

#### Multi-Word Book Names
```
/bible 1 Kings 8:27 0 100 0 3 60
/bible Song of Solomon 2:1 0 100 0 3 60
```

## API Reference

### Initialization

```c
int bible_init(const char *bible_path);
```

Initializes the Bible system with the KJV text file.

**Parameters:**
- `bible_path`: Path to the KJV text file (e.g., `"../Bible/kjv.txt"`)

**Returns:**
- `1` on success
- `0` on failure

**Example:**
```c
if (!bible_init("../Bible/kjv.txt")) {
    fprintf(stderr, "Failed to initialize Bible system\n");
}
```

### Render a Single Verse

```c
int bible_render_verse(
    const char *book,
    int chapter,
    int verse,
    int x, int y, int z,
    int block_type,
    int max_width,
    void (*block_func)(int x, int y, int z, int w)
);
```

**Parameters:**
- `book`: Book name (e.g., "Genesis", "1 Kings")
- `chapter`: Chapter number (1-based)
- `verse`: Verse number (1-based)
- `x, y, z`: Starting position
- `block_type`: Block type for rendering
- `max_width`: Maximum line width in characters (0 = no wrapping)
- `block_func`: Block placement function (use `builder_block`)

**Returns:**
- Number of lines rendered
- `0` on error

**Example:**
```c
int lines = bible_render_verse(
    "Genesis", 1, 1,
    0, 100, 0,
    3,              // Stone
    60,             // 60 char width
    builder_block
);
printf("Rendered %d lines\n", lines);
```

### Render a Verse Range

```c
int bible_render_verse_range(
    const char *book,
    int chapter,
    int verse_start,
    int verse_end,
    int x, int y, int z,
    int block_type,
    int max_width,
    int line_spacing,
    void (*block_func)(int x, int y, int z, int w)
);
```

**Parameters:**
- `verse_start`, `verse_end`: Range of verses to render
- `line_spacing`: Extra space between verses (in voxels)
- Other parameters same as `bible_render_verse()`

**Returns:**
- Total number of lines rendered

**Example:**
```c
// Render Genesis 1:1-3
int lines = bible_render_verse_range(
    "Genesis", 1, 1, 3,
    0, 100, 0,
    3,              // Stone
    60,             // 60 char width
    5,              // 5 voxel spacing between verses
    builder_block
);
```

### Render an Entire Chapter

```c
int bible_render_chapter(
    const char *book,
    int chapter,
    int x, int y, int z,
    int block_type,
    int max_width,
    int line_spacing,
    void (*block_func)(int x, int y, int z, int w)
);
```

Renders all verses in a chapter. **Warning**: Some chapters are very long!

**Example:**
```c
// Render Psalm 23 (6 verses)
int lines = bible_render_chapter(
    "Psalm", 23,
    0, 100, 0,
    3, 60, 5,
    builder_block
);
```

### Get Verse Text (Debug/Testing)

```c
int bible_get_verse_text(
    const char *book,
    int chapter,
    int verse,
    char *text_buffer,
    int buffer_size
);
```

Retrieves verse text without rendering.

**Returns:**
- `1` if verse found
- `0` if not found

**Example:**
```c
char verse_text[1024];
if (bible_get_verse_text("John", 3, 16, verse_text, sizeof(verse_text))) {
    printf("John 3:16: %s\n", verse_text);
}
```

### Cleanup

```c
void bible_cleanup(void);
```

Cleans up the Bible system. Called automatically on game exit.

## Bible Text Format

### KJV Text File Format

The KJV text file (`Bible/kjv.txt`) has a simple format:

```
KJV
King James Bible: Pure Cambridge Edition - Text courtesy of www.BibleProtector.com
Genesis 1:1	In the beginning God created the heaven and the earth.
Genesis 1:2	And the earth was without form, and void; ...
```

**Format:**
- Line 1: "KJV" (with BOM)
- Line 2: Attribution
- Lines 3+: `Book Chapter:Verse<TAB>Text`

### Book Names

All 66 books of the Bible are supported:

**Old Testament (39 books):**
- Genesis, Exodus, Leviticus, Numbers, Deuteronomy
- Joshua, Judges, Ruth, 1 Samuel, 2 Samuel
- 1 Kings, 2 Kings, 1 Chronicles, 2 Chronicles
- Ezra, Nehemiah, Esther, Job, Psalm
- Proverbs, Ecclesiastes, Song of Solomon
- Isaiah, Jeremiah, Lamentations, Ezekiel, Daniel
- Hosea, Joel, Amos, Obadiah, Jonah
- Micah, Nahum, Habakkuk, Zephaniah, Haggai
- Zechariah, Malachi

**New Testament (27 books):**
- Matthew, Mark, Luke, John, Acts
- Romans, 1 Corinthians, 2 Corinthians, Galatians
- Ephesians, Philippians, Colossians
- 1 Thessalonians, 2 Thessalonians
- 1 Timothy, 2 Timothy, Titus, Philemon
- Hebrews, James, 1 Peter, 2 Peter
- 1 John, 2 John, 3 John, Jude, Revelation

**Note**: Book names are case-sensitive and must match exactly.

## Layout and Formatting

### Word Wrapping

When `max_width` is specified, long verses are automatically wrapped:

```c
bible_render_verse("Genesis", 1, 28, 0, 100, 0, 3, 40, builder_block);
```

The text will break at word boundaries when possible.

### Line Spacing

The `line_spacing` parameter adds extra voxels between lines:

- `0`: No extra spacing (lines touch)
- `5`: Small spacing (recommended for verses)
- `10`: Medium spacing
- `20`: Large spacing

### Vertical Layout

Text is rendered **downward** (decreasing Y):

```
Y = 100: "Genesis 1:1 - In the beginning..."
Y = 80:  "God created the heaven and"
Y = 60:  "the earth."
```

Each line of text is approximately 20 voxels tall (16 for characters + 4 spacing).

### Horizontal Layout

Text advances along the +X axis (left to right).

## Examples

### Creation Story (Genesis 1:1-5)

```c
bible_render_verse_range(
    "Genesis", 1, 1, 5,
    0, 200, 0,      // High up for visibility
    3,              // Stone
    60,             // 60 char width
    10,             // 10 voxel spacing
    builder_block
);
```

### John 3:16 (Famous Verse)

```c
bible_render_verse(
    "John", 3, 16,
    0, 150, 0,
    4,              // Brick
    50,             // 50 char width
    builder_block
);
```

### Psalm 23 (Complete Chapter)

```c
// The Lord is my shepherd...
bible_render_chapter(
    "Psalm", 23,
    0, 200, 0,
    3,              // Stone
    50,             // 50 char width
    8,              // 8 voxel spacing
    builder_block
);
```

### Ten Commandments (Exodus 20:3-17)

```c
bible_render_verse_range(
    "Exodus", 20, 3, 17,
    0, 250, 0,
    3,              // Stone (appropriate!)
    55,             // 55 char width
    10,             // 10 voxel spacing
    builder_block
);
```

## Technical Details

### Performance

**File Reading:**
- KJV text file: ~31,104 lines, ~5 MB
- Each verse requires one file scan
- No caching (reads on demand)

**Rendering Times:**
- Single verse: < 0.5 seconds
- Verse range (5 verses): ~2 seconds
- Small chapter (< 20 verses): ~10 seconds
- Large chapter (> 50 verses): ~30+ seconds

### Memory Usage

- Minimal static memory (< 2 KB)
- No verse caching
- Temporary buffers for parsing (< 2 KB per operation)

### Coordinate System

Same as Voxel Text System:
- **X**: Horizontal (text direction)
- **Y**: Vertical (height, decreasing for text lines)
- **Z**: Depth

### Error Handling

The system provides clear error messages:

```
Verse not found: InvalidBook 1:1
Failed to open Bible file
Bible system not initialized
```

Errors are logged to `stderr`.

## Command Reference

### Basic Syntax

```
/bible BOOK CHAPTER:VERSE X Y Z BLOCK_TYPE [WIDTH]
```

**Parameters:**
- `BOOK`: Book name (can contain spaces)
- `CHAPTER`: Chapter number
- `VERSE`: Verse number or range (e.g., `1` or `1-3`)
- `X Y Z`: World coordinates
- `BLOCK_TYPE`: Block type ID (0-255)
- `WIDTH`: Optional max width (default: 60)

### Examples

#### Single Verse
```
/bible Genesis 1:1 0 100 0 3
/bible John 3:16 10 120 5 4
/bible Psalm 119:105 0 100 0 3 70
```

#### Verse Range
```
/bible Matthew 5:3-10 0 150 0 3 50
/bible Romans 8:28-39 0 100 0 4 60
```

#### Entire Chapter
```
/bible Psalm 23 0 200 0 3 55
/bible John 1 0 300 0 3 50
```

#### Multi-Word Book Names
```
/bible 1 Corinthians 13:4-8 0 100 0 3 50
/bible Song of Solomon 2:1 0 100 0 4 60
/bible 1 John 4:8 0 100 0 3 50
```

## Tips and Best Practices

### 1. Start High

Place text high in the world (Y = 100-200) to avoid terrain:
```
/bible Genesis 1:1 0 150 0 3 60
```

### 2. Use Appropriate Widths

- Short verses: 40-50 characters
- Medium verses: 50-60 characters
- Long verses: 60-80 characters

### 3. Space Out Chapters

Large chapters need lots of vertical space:
```
Y_needed ≈ num_verses * 25 + line_spacing * num_verses
```

For a 31-verse chapter with 10px spacing:
```
Y_needed ≈ 31 * 25 + 10 * 31 = 1,085 voxels
```

### 4. Choose Readable Blocks

- **Stone (3)**: Classic, easy to read
- **Brick (4)**: Warm, traditional
- **Wood (5)**: Natural
- **Cobblestone (11)**: Rustic

Avoid:
- **Glass (10)**: Hard to see
- **Light Stone (12)**: May blend with sky

### 5. Test Position First

Use a single verse to test positioning:
```
/bible Genesis 1:1 0 100 0 3 60
```

Then adjust for your passage.

## Troubleshooting

### "Bible system not initialized" Error
**Cause**: `bible_init()` not called or failed
**Solution**: Verify `Bible/kjv.txt` exists

### "Verse not found" Error
**Cause**: Invalid book name, chapter, or verse
**Solution**:
- Check book name spelling (case-sensitive)
- Verify chapter/verse numbers exist
- Try `bible_get_verse_text()` for debugging

### Text Renders Off-Screen
**Cause**: Wrong coordinates or size too large
**Solution**:
- Start at Y=100-200 for visibility
- Calculate vertical space needed
- Use teleport (`/goto`) to view

### Slow Rendering
**Cause**: Large chapters or many verses
**Solution**:
- Break into smaller ranges
- Increase performance (close other programs)
- Be patient (it's reading a 5MB file repeatedly)

### Garbled Text
**Cause**: Wrong file encoding or corrupted file
**Solution**: Re-download `kjv.txt` (must be UTF-8 with tabs)

## Future Enhancements

Potential improvements:

1. **Verse Caching**: Cache parsed verses in memory
2. **Book Index**: Build index for faster lookup
3. **Formatted Output**: Support for verse numbers, chapter headings
4. **Multi-Column Layout**: Render text in columns
5. **Search Function**: Find verses by keyword
6. **Bookmarks**: Save favorite passages
7. **Dynamic Loading**: Stream large chapters
8. **Translations**: Support for other Bible versions

## Related Files

- **Implementation**: `Craft/src/bible.c`, `Craft/src/bible.h`
- **Bible Data**: `Bible/kjv.txt` (King James Version, ~31K verses)
- **Voxel Text**: Uses `voxel_text.h` for rendering
- **Usage Examples**: `Craft/src/main.c` (search for `/bible` command)

## Attribution

**King James Bible Text:**
- Version: Pure Cambridge Edition
- Courtesy of: www.BibleProtector.com
- License: Public Domain
- Format: Plain text with tab delimiters

The KJV Bible text is in the public domain and free to use.
