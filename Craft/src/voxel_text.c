#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "voxel_text.h"

static char font_path[1024] = {0};

int voxel_text_init(const char *path) {
    if (!path) {
        return 0;
    }
    strncpy(font_path, path, sizeof(font_path) - 1);

    // Test if file exists
    FILE *f = fopen(font_path, "r");
    if (!f) {
        fprintf(stderr, "Error: Could not open font file: %s\n", font_path);
        return 0;
    }
    fclose(f);
    return 1;
}

void voxel_text_cleanup(void) {
    font_path[0] = '\0';
}

// Convert UTF-8 character to Unicode codepoint
// Returns the codepoint and advances the string pointer
static uint32_t utf8_to_unicode(const char **text) {
    const unsigned char *bytes = (const unsigned char *)*text;
    uint32_t codepoint = 0;
    int len = 0;

    if (bytes[0] < 0x80) {
        // Single byte (ASCII)
        codepoint = bytes[0];
        len = 1;
    } else if ((bytes[0] & 0xE0) == 0xC0) {
        // Two bytes
        codepoint = ((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F);
        len = 2;
    } else if ((bytes[0] & 0xF0) == 0xE0) {
        // Three bytes
        codepoint = ((bytes[0] & 0x0F) << 12) |
                    ((bytes[1] & 0x3F) << 6) |
                    (bytes[2] & 0x3F);
        len = 3;
    } else if ((bytes[0] & 0xF8) == 0xF0) {
        // Four bytes
        codepoint = ((bytes[0] & 0x07) << 18) |
                    ((bytes[1] & 0x3F) << 12) |
                    ((bytes[2] & 0x3F) << 6) |
                    (bytes[3] & 0x3F);
        len = 4;
    } else {
        // Invalid UTF-8, return replacement character
        codepoint = 0xFFFD;
        len = 1;
    }

    *text += len;
    return codepoint;
}

// Find a glyph in the hex file by codepoint
// Returns 1 on success, 0 on failure
// Fills hex_data with the hex string (without codepoint prefix)
static int find_glyph(uint32_t codepoint, char *hex_data, int max_len) {
    FILE *f = fopen(font_path, "r");
    if (!f) {
        return 0;
    }

    char line[256];
    char search[16];
    snprintf(search, sizeof(search), "%04X:", (unsigned int)codepoint);

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, search, 5) == 0) {
            // Found it! Extract hex data
            char *hex_start = line + 5;
            int len = strlen(hex_start);
            // Remove newline
            if (len > 0 && hex_start[len-1] == '\n') {
                hex_start[len-1] = '\0';
                len--;
            }
            if (len > 0 && hex_start[len-1] == '\r') {
                hex_start[len-1] = '\0';
                len--;
            }
            strncpy(hex_data, hex_start, max_len - 1);
            hex_data[max_len - 1] = '\0';
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

// Convert a hex character to integer
static int hex_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

// Render a single glyph at position (x, y, z)
// Returns the width of the glyph in pixels
static int render_glyph(
    uint32_t codepoint,
    int x, int y, int z,
    int block_type,
    int scale,
    void (*set_block_func)(int x, int y, int z, int w)
) {
    char hex_data[256];
    if (!find_glyph(codepoint, hex_data, sizeof(hex_data))) {
        // Glyph not found, render a box for missing glyph
        for (int sy = 0; sy < 16 * scale; sy++) {
            for (int sx = 0; sx < 8 * scale; sx++) {
                if (sy == 0 || sy == (16*scale-1) || sx == 0 || sx == (8*scale-1)) {
                    set_block_func(x + sx, y + sy, z, block_type);
                }
            }
        }
        return 8 * scale;
    }

    int hex_len = strlen(hex_data);
    int width = (hex_len == 32) ? 8 : 16;
    int height = 16;

    // Parse hex data and render voxels
    for (int row = 0; row < height; row++) {
        int hex_per_row = width / 4;
        for (int hex_idx = 0; hex_idx < hex_per_row; hex_idx++) {
            int pos = row * hex_per_row + hex_idx;
            if (pos >= hex_len) break;

            int nibble = hex_to_int(hex_data[pos]);

            // Each hex digit represents 4 pixels
            for (int bit = 0; bit < 4; bit++) {
                if (nibble & (1 << (3 - bit))) {
                    // Pixel is set, place voxel(s)
                    int px = hex_idx * 4 + bit;
                    int py = (height - 1) - row; // Flip vertically - row 0 is top of glyph

                    // Apply scale
                    for (int sy = 0; sy < scale; sy++) {
                        for (int sx = 0; sx < scale; sx++) {
                            set_block_func(
                                x + px * scale + sx,
                                y + py * scale + sy,
                                z,
                                block_type
                            );
                        }
                    }
                }
            }
        }
    }

    return width * scale;
}

int voxel_text_render(
    const char *text,
    int x, int y, int z,
    int block_type,
    int scale,
    void (*set_block_func)(int x, int y, int z, int w)
) {
    if (!text || !set_block_func) {
        return 0;
    }

    if (font_path[0] == '\0') {
        fprintf(stderr, "Error: Voxel text not initialized. Call voxel_text_init() first.\n");
        return 0;
    }

    int cursor_x = x;
    const char *p = text;

    while (*p) {
        uint32_t codepoint = utf8_to_unicode(&p);
        if (codepoint == 0) break;

        // Render the glyph
        int glyph_width = render_glyph(
            codepoint,
            cursor_x, y, z,
            block_type,
            scale,
            set_block_func
        );

        // Advance cursor along X axis
        cursor_x += glyph_width + scale; // Add spacing between characters
    }

    return cursor_x - x;
}
