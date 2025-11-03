# GNU Unifont - Bitmap Font for Voxel Rendering

This folder contains GNU Unifont version 17.0.03, a comprehensive bitmap font covering virtually all Unicode characters including English, Cyrillic, Mandarin Chinese, Japanese, Korean, and Spanish.

## Font Files

- **unifont-17.0.03.hex** (3.6 MB) - HEX format, easiest to parse
- **unifont-17.0.03.bdf** (9.0 MB) - BDF (Bitmap Distribution Format)
- **LICENSE.txt** - Dual license: GPL 2+ with Font Embedding Exception OR SIL OFL 1.1

## Why GNU Unifont is Perfect for Voxel Rendering

1. **True Bitmap Font** - No antialiasing, pure pixel grid
2. **Low Resolution** - 8×16 or 16×16 pixels per character
3. **Complete Coverage** - Supports all required languages and scripts:
   - Latin (English, Spanish)
   - Cyrillic (Russian, etc.)
   - CJK (Chinese, Japanese, Korean)
4. **Simple Format** - Easy to convert to voxel blocks
5. **Open License** - Free for commercial use with embedding allowed

## Font Specifications

- **Character Dimensions:**
  - Standard characters (Latin, Cyrillic): 8 pixels wide × 16 pixels tall
  - CJK characters (Chinese, Japanese, Korean): 16 pixels wide × 16 pixels tall
- **Format:** Monochrome bitmap (1-bit per pixel)
- **Coverage:** ~58,000+ glyphs covering Unicode BMP (Basic Multilingual Plane)

## HEX Format Specification

The .hex format is a simple text format where each line represents one character:

```
CODEPOINT:HEXDATA
```

### Examples

**Latin 'A' (U+0041):**
```
0041:0000000018242442427E424242420000
```
- Codepoint: `0041` (Unicode for 'A')
- Data: 32 hex digits = 16 bytes = 8×16 bitmap
- Each pair of hex digits = one row of 8 pixels

**CJK '一' (U+4E00):**
```
4E00:0000000000000000000000000000FFFE00000000000000000000000000000000
```
- Codepoint: `4E00` (Unicode for Chinese "one")
- Data: 64 hex digits = 32 bytes = 16×16 bitmap
- Each pair of hex digits = half a row (16 pixels total per row)

### Parsing HEX Format

Each hex digit represents 4 pixels. For example:
- `F` = `1111` = ████ (4 white pixels)
- `0` = `0000` = □□□□ (4 black pixels)
- `8` = `1000` = █□□□
- `C` = `1100` = ██□□

**8-pixel wide characters:**
- 2 hex digits per row
- 16 rows total
- 32 hex digits per character

**16-pixel wide characters:**
- 4 hex digits per row
- 16 rows total
- 64 hex digits per character

## Converting to Voxels

### Algorithm

```
1. Parse line: split on ':'
2. Get codepoint (first part)
3. Get hex data (second part)
4. Determine width: 32 chars = 8px wide, 64 chars = 16px wide
5. For each row (16 total):
   - Get hex digits for that row
   - Convert each hex digit to 4 bits
   - For each bit: 1 = place voxel, 0 = empty
```

### Example Implementation (Pseudocode)

```c
// Read one character from HEX file
char* line = "0041:0000000018242442427E424242420000";

// Split on colon
char* codepoint_str = "0041";
char* hex_data = "0000000018242442427E424242420000";

int width = (strlen(hex_data) == 32) ? 8 : 16;
int height = 16;

// For 8-pixel wide character
for (int row = 0; row < 16; row++) {
    char byte_hex[3] = {hex_data[row*2], hex_data[row*2+1], '\0'};
    int byte = hex_to_int(byte_hex);

    for (int col = 0; col < 8; col++) {
        if (byte & (1 << (7-col))) {
            // Place voxel at (col, row)
            place_voxel(col, row);
        }
    }
}
```

## Language Support Details

### Latin Scripts
- Complete ASCII
- Extended Latin (diacritics for Spanish, French, German, etc.)
- **Coverage:** ~500+ characters

### Cyrillic
- Russian, Ukrainian, Bulgarian, Serbian, etc.
- **Coverage:** ~150+ characters

### CJK (Chinese, Japanese, Korean)
- **Chinese:** Simplified and Traditional
  - Common characters: ~20,902 (CJK Unified Ideographs)
  - Additional extensions available
- **Japanese:**
  - Hiragana: 93 characters
  - Katakana: 96 characters
  - Kanji: Same as Chinese CJK ideographs
- **Korean:**
  - Hangul syllables: 11,172 complete syllables
  - Hangul Jamo: 256 combining characters

### Other Scripts
Also includes: Greek, Hebrew, Arabic, Thai, mathematical symbols, arrows, box drawing, and more.

## Performance Considerations

### File Size
- HEX format: 3.6 MB (recommended - easiest to parse)
- BDF format: 9.0 MB (more metadata, harder to parse)

### Memory Usage
If loading all glyphs into memory:
- ~58,000 glyphs
- Average ~200 bytes per glyph (16×16 max)
- Total: ~11.6 MB uncompressed

**Recommendation:** Load glyphs on-demand or cache frequently used characters.

## Common Characters Quick Reference

```
U+0041: A (Latin)
U+0410: А (Cyrillic A)
U+4E00: 一 (Chinese "one")
U+3042: あ (Hiragana A)
U+30A2: ア (Katakana A)
U+AC00: 가 (Hangul GA)
```

## Tools and Utilities

To view characters:
```bash
# View a specific character (e.g., U+0041)
grep "^0041:" unifont-17.0.03.hex

# Count total glyphs
wc -l unifont-17.0.03.hex

# Find all Latin letters (U+0041 to U+005A)
grep "^00[4-5][0-9A-F]:" unifont-17.0.03.hex
```

## License Compatibility

The dual license allows you to choose:

1. **GPL 2+ with Font Embedding Exception**
   - Free to embed in any document/application (commercial or not)
   - Modified fonts must be GPL licensed

2. **SIL Open Font License 1.1**
   - Free for commercial use
   - Can bundle with software under any license
   - Modified fonts must use OFL

Both licenses are highly permissive for embedding in games and applications.

## Additional Resources

- Official website: https://unifoundry.com/unifont/
- HEX format specification: https://unifoundry.com/unifont/unifont-utilities.html
- Unicode charts: https://unicode.org/charts/

## Version Information

- **Version:** 17.0.03
- **Release Date:** November 1, 2025
- **Glyphs:** ~58,000+
- **Format:** HEX & BDF
