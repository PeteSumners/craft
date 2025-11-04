# Chunky Craft Documentation

This directory contains documentation for the enhanced features of Chunky Craft, a Minecraft-inspired voxel game with advanced text rendering capabilities.

## Documentation Index

### Feature Documentation

1. **[Voxel Text System](VoxelTextSystem.md)**
   - Multi-language voxel text rendering
   - GNU Unifont integration (~58,000 characters)
   - UTF-8 support for all major languages
   - API reference and examples

2. **[Bible Rendering System](BibleRenderingSystem.md)**
   - Complete KJV Bible (31,102 verses)
   - Verse, passage, and chapter rendering
   - Word wrapping and layout controls
   - In-game command reference

### System Requirements

- **Font File**: GNU Unifont 17.0.03 HEX format
  - Location: `Fonts/unifont-17.0.03.hex`
  - Size: 3.6 MB
  - Coverage: Unicode BMP (~58,000 glyphs)

- **Bible File**: KJV text file
  - Location: `Bible/kjv.txt`
  - Size: ~5 MB
  - Format: Tab-delimited plain text

### Quick Links

- **Main README**: `../Craft/README.md` - Build instructions and game info
- **Font Documentation**: `../Fonts/README.md` - GNU Unifont details
- **Project TODO**: `../TODO.md` - Development roadmap

## Feature Overview

### Voxel Text Rendering

Transform any text into 3D voxel blocks in the game world:

```
/vtext 0 100 0 3 Hello World
```

- **Languages Supported**: English, Chinese, Japanese, Korean, Russian, and more
- **Scalable**: 1x to 10x size
- **Block Types**: Any in-game block (stone, brick, wood, etc.)

### Bible Rendering

Render Scripture passages as monumental voxel text:

```
/bible Genesis 1:1 0 100 0 3 60
/bible John 3:16-17 0 100 0 4 50
/bible Psalm 23 0 200 0 3 60
```

- **Complete KJV**: All 66 books, 1,189 chapters, 31,102 verses
- **Word Wrapping**: Automatic line breaks
- **Flexible**: Single verses, ranges, or entire chapters

## In-Game Commands

### Voxel Text Commands

| Command | Description | Example |
|---------|-------------|---------|
| `/vtext X Y Z BLOCK MESSAGE` | Render English text | `/vtext 0 100 0 3 Hello` |
| `/vtext-jp X Y Z BLOCK` | Render Japanese demo | `/vtext-jp 0 100 0 3` |
| `/vtext-cn X Y Z BLOCK` | Render Chinese demo | `/vtext-cn 0 100 0 3` |
| `/vtext-kr X Y Z BLOCK` | Render Korean demo | `/vtext-kr 0 100 0 3` |
| `/vtext-ru X Y Z BLOCK` | Render Russian demo | `/vtext-ru 0 100 0 3` |

### Bible Commands

| Command | Description | Example |
|---------|-------------|---------|
| `/bible BOOK CH:V X Y Z BLOCK WIDTH` | Single verse | `/bible Genesis 1:1 0 100 0 3 60` |
| `/bible BOOK CH:V-V X Y Z BLOCK WIDTH` | Verse range | `/bible John 3:16-17 0 100 0 3 50` |
| `/bible BOOK CH X Y Z BLOCK WIDTH` | Full chapter | `/bible Psalm 23 0 100 0 3 60` |

**Parameters:**
- `X Y Z` - World coordinates
- `BLOCK` - Block type (3=Stone, 4=Brick, etc.)
- `WIDTH` - Max line width in characters (optional, default 60)

## Getting Started

### 1. Initialize Systems

The voxel text and Bible systems are automatically initialized when the game starts. Verify the font and Bible files exist:

```
Fonts/unifont-17.0.03.hex    (~3.6 MB)
Bible/kjv.txt                (~5 MB)
```

### 2. Try a Simple Command

Start with a basic text rendering:

```
/vtext 0 100 0 3 Hello World
```

This renders "Hello World" in stone blocks at coordinates (0, 100, 0).

### 3. Render a Bible Verse

Try rendering a famous verse:

```
/bible John 3:16 0 150 0 3 50
```

### 4. Experiment

- Try different block types (1-23)
- Adjust position (X, Y, Z)
- Change text width (30-80 characters)
- Test different languages

## Examples

### Multi-Language Display

Create an international welcome sign:

```
/vtext 0 120 0 3 Welcome
/vtext-jp 0 100 0 3
/vtext-cn 0 80 0 3
/vtext-kr 0 60 0 3
/vtext-ru 0 40 0 3
```

### Creation Story

Render the beginning of Genesis:

```
/bible Genesis 1:1-5 0 200 0 3 60
```

### Large Monument

Create a massive text monument:

```
/vtext 0 150 0 4 CRAFT
```

Using brick blocks for a classic look.

## Technical Information

### Architecture

```
main.c
  ├─ voxel_text.h/c     → Character rendering
  │   └─ GNU Unifont (HEX format)
  └─ bible.h/c          → Bible text parsing
      ├─ KJV text file (tab-delimited)
      └─ voxel_text      → Text rendering
```

### Coordinate System

- **X Axis**: Horizontal, text advances left to right (+X)
- **Y Axis**: Vertical, height (0-255)
- **Z Axis**: Depth, into screen (+Z)

Text is rendered "standing up" - readable from -Z direction.

### Block Placement

All rendering uses the `builder_block()` function which:
- Places blocks in the world
- Marks chunks as dirty (for re-rendering)
- Records changes to database
- Handles multiplayer synchronization

### Performance

**Font Loading:**
- HEX file read per character
- ~58,000 glyphs available
- No caching (reads on demand)

**Rendering Speed:**
- Short text (< 20 chars): Instant
- Bible verse: < 1 second
- Bible chapter: 10-30 seconds (depending on size)

**Memory Usage:**
- Static: < 3 KB
- Per render: < 5 KB temporary buffers

## Troubleshooting

### Common Issues

**"Voxel text not initialized"**
- Check: `Fonts/unifont-17.0.03.hex` exists
- Solution: Re-download font or fix path

**"Bible system not initialized"**
- Check: `Bible/kjv.txt` exists
- Solution: Re-download Bible text or fix path

**Text appears as boxes**
- Cause: Characters not in font
- Solution: Use different characters or update font

**Slow rendering**
- Cause: Large text or slow disk
- Solution: Break into smaller pieces, use SSD

**Text renders off-screen**
- Cause: Wrong coordinates
- Solution: Start at Y=100-150, use `/goto` to view

### Debug Mode

To enable verbose logging, compile with:

```c
#define DEBUG 1
```

in `Craft/src/config.h`.

## Development

### Adding New Commands

To add custom text/Bible commands:

1. Edit `Craft/src/main.c`
2. Find `parse_command()` function
3. Add new command handling:

```c
else if (strncmp(buffer, "/mycommand ", 11) == 0) {
    // Your code here
}
```

4. Rebuild: `cmake . && make`

### Extending the Systems

**Voxel Text:**
- Add font caching in `voxel_text.c`
- Implement text effects (shadows, outlines)
- Support rotation/orientation

**Bible Rendering:**
- Add verse indexing for speed
- Support other translations
- Implement search functionality

### Contributing

When contributing improvements:
1. Document all new features
2. Update relevant `.md` files
3. Add examples to documentation
4. Test on multiple platforms

## Support

For issues or questions:
1. Check documentation in this folder
2. Review TODO.md for known issues
3. Check Craft/README.md for build help
4. Examine source comments in `.c/.h` files

## License

- **Craft Game**: See `Craft/LICENSE.md`
- **GNU Unifont**: Dual-licensed (GPL 2+ OR SIL OFL 1.1)
- **KJV Bible**: Public Domain
- **Documentation**: Same as Craft project

## Version History

### v1.0 (Current)
- Initial voxel text rendering system
- GNU Unifont integration (58,000+ glyphs)
- Multi-language support (CJK, Cyrillic, Latin)
- Complete KJV Bible rendering
- Word wrapping and layout controls
- In-game commands (/vtext, /bible)

### Planned (v1.1)
- Glyph caching for performance
- Bible verse indexing
- Additional translations
- Text effects (shadows, gradients)
- Multi-column layouts

See `../TODO.md` for complete roadmap.
