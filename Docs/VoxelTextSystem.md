# Voxel Text Rendering System

## Overview

The Voxel Text Rendering System allows you to render text as 3D voxel blocks in the Craft game world. It uses the GNU Unifont font to support over 58,000 characters across multiple languages including English, Chinese, Japanese, Korean, and Russian.

## Features

- **Multi-language Support**: Renders text in any language supported by GNU Unifont
- **UTF-8 Encoding**: Full Unicode support via UTF-8
- **Scalable**: Text can be rendered at different scales (1x, 2x, 3x, etc.)
- **Automatic Sizing**: Characters are automatically sized:
  - Latin/Cyrillic: 8×16 pixels
  - CJK (Chinese/Japanese/Korean): 16×16 pixels

## API Reference

### Initialization

```c
int voxel_text_init(const char *font_path);
```

Initializes the voxel text system with the specified font file.

**Parameters:**
- `font_path`: Path to the GNU Unifont HEX file (e.g., `"../Fonts/unifont-17.0.03.hex"`)

**Returns:**
- `1` on success
- `0` on failure

**Example:**
```c
if (!voxel_text_init("../Fonts/unifont-17.0.03.hex")) {
    fprintf(stderr, "Failed to initialize voxel text system\n");
}
```

### Rendering Text

```c
int voxel_text_render(
    const char *text,
    int x, int y, int z,
    int block_type,
    int scale,
    void (*block_func)(int x, int y, int z, int w)
);
```

Renders text as voxel blocks in the world.

**Parameters:**
- `text`: UTF-8 encoded string to render
- `x, y, z`: Starting position in world coordinates
- `block_type`: Type of block to use (see Block Types below)
- `scale`: Scale factor (1-10, where 1 = normal size)
- `block_func`: Function to call for placing each block (use `builder_block` in-game)

**Returns:**
- Total width of rendered text in voxels
- `0` on error

**Example:**
```c
int width = voxel_text_render(
    "Hello World",
    0, 100, 0,    // Position
    3,            // Stone blocks
    1,            // 1x scale
    builder_block // Block placement function
);
printf("Rendered text %d blocks wide\n", width);
```

### Cleanup

```c
void voxel_text_cleanup(void);
```

Cleans up the voxel text system. Called automatically on game exit.

## Block Types

Common block types you can use:

| Block Type | Name | ID |
|------------|------|-----|
| GRASS | Grass | 1 |
| SAND | Sand | 2 |
| STONE | Stone | 3 |
| BRICK | Brick | 4 |
| WOOD | Wood | 5 |
| CEMENT | Cement | 6 |
| DIRT | Dirt | 7 |
| PLANK | Planks | 8 |
| SNOW | Snow | 9 |
| GLASS | Glass | 10 |
| COBBLE | Cobblestone | 11 |

See `Craft/src/item.h` for the complete list.

## In-Game Commands

### Basic Text Rendering

```
/vtext X Y Z BLOCK_TYPE MESSAGE
```

Renders text at the specified coordinates.

**Examples:**
```
/vtext 0 100 0 3 Hello World
/vtext 10 120 5 4 Welcome to Craft!
```

### Language-Specific Demos

Pre-configured commands for testing different languages:

#### Japanese
```
/vtext-jp X Y Z BLOCK_TYPE
```
Renders: こんにちは世界 (Hello World)

#### Chinese
```
/vtext-cn X Y Z BLOCK_TYPE
```
Renders: 你好世界 (Hello World)

#### Korean
```
/vtext-kr X Y Z BLOCK_TYPE
```
Renders: 안녕하세요 세계 (Hello World)

#### Russian
```
/vtext-ru X Y Z BLOCK_TYPE
```
Renders: Привет мир (Hello World)

**Example:**
```
/vtext-jp 0 100 0 3
```

## Coordinate System

- **X axis**: Text advances from left to right along +X
- **Y axis**: Vertical (up is positive)
- **Z axis**: Depth (into the screen is positive)

Text is rendered "standing up" in the world, readable when viewed from the -Z direction.

## Font Information

### GNU Unifont

- **Version**: 17.0.03
- **License**: Dual-licensed (GPL 2+ with Font Embedding Exception OR SIL OFL 1.1)
- **Coverage**: ~58,000 glyphs covering Unicode BMP
- **Format**: HEX format (human-readable bitmap data)

### Character Dimensions

- **Latin/Cyrillic**: 8 pixels wide × 16 pixels tall
- **CJK**: 16 pixels wide × 16 pixels tall

Each pixel becomes one voxel block (or more if scaled).

## Technical Details

### UTF-8 Support

The system automatically decodes UTF-8 encoded text:
- 1-byte sequences (ASCII): 0x00-0x7F
- 2-byte sequences: 0xC0-0xDF
- 3-byte sequences: 0xE0-0xEF
- 4-byte sequences: 0xF0-0xF7

### Missing Glyphs

If a character is not found in the font:
- A placeholder box outline is rendered
- Size: 8×16 pixels (scaled appropriately)

### Performance Considerations

**Font File Access:**
- The HEX file is read for each glyph
- File size: ~3.6 MB
- Recommendation: For frequently used characters, the system could benefit from a cache

**Rendering Speed:**
- Small texts (< 20 characters): Instant
- Medium texts (20-100 characters): < 1 second
- Large texts (100+ characters): May take several seconds

**Memory Usage:**
- Minimal static memory (< 1 KB)
- No glyph caching (reads on demand)

### Coordinate Limits

- Y-axis: 0-255 (enforced by game)
- X/Z-axis: Limited by floating-point precision
- Recommended stay within: ±1,000,000 blocks from origin

## Examples

### Simple English Text
```c
voxel_text_render("Hello", 0, 100, 0, 3, 1, builder_block);
```

### Large Scaled Text
```c
voxel_text_render("BIG", 0, 100, 0, 4, 3, builder_block);
// Renders at 3x scale (each pixel = 3×3 voxel blocks)
```

### Multi-language
```c
voxel_text_render("日本語", 0, 100, 0, 3, 1, builder_block);  // Japanese
voxel_text_render("中文", 100, 100, 0, 3, 1, builder_block);   // Chinese
voxel_text_render("한글", 200, 100, 0, 3, 1, builder_block);   // Korean
```

### Different Block Types
```c
voxel_text_render("STONE", 0, 100, 0, 3, 1, builder_block);   // Stone
voxel_text_render("BRICK", 0, 80, 0, 4, 1, builder_block);    // Brick
voxel_text_render("GLASS", 0, 60, 0, 10, 1, builder_block);   // Glass
```

## Troubleshooting

### "Voxel text not initialized" Error
**Cause**: `voxel_text_init()` not called or failed
**Solution**: Ensure font file exists at correct path

### Characters Appear as Boxes
**Cause**: Character not in GNU Unifont
**Solution**: Use different characters or update font

### Text Appears Upside Down
**Cause**: Coordinate system orientation
**Solution**: This is expected - text is rendered right-side-up in world space

### Text Too Small/Large
**Cause**: Wrong scale parameter
**Solution**: Adjust scale (1 = normal, 2 = double size, etc.)

### Performance Issues with Long Text
**Cause**: Each character requires file read
**Solution**: Consider breaking text into smaller chunks or implementing caching

## Future Enhancements

Potential improvements for future versions:

1. **Glyph Caching**: Cache frequently used characters in memory
2. **Text Rotation**: Support for different orientations
3. **Color Support**: Multi-colored text using different block types
4. **Text Effects**: Gradients, shadows, outlines
5. **Dynamic Font Loading**: Support for multiple fonts
6. **Batch Rendering**: Optimize multiple text rendering operations

## Related Files

- **Implementation**: `Craft/src/voxel_text.c`, `Craft/src/voxel_text.h`
- **Font Data**: `Fonts/unifont-17.0.03.hex`
- **Font Documentation**: `Fonts/README.md`
- **Usage Examples**: `Craft/src/main.c` (search for `/vtext` commands)
