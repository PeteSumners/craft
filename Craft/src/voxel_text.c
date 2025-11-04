#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "voxel_text.h"

// Constants
#define MAX_SCALE 10
#define GLYPH_HEIGHT 16

static char font_path[1024] = {0};
static int initialized = 0;

int voxel_text_init(const char *path) {
    if (!path) {
        fprintf(stderr, "Error: voxel_text_init: NULL path provided\n");
        return 0;
    }

    strncpy(font_path, path, sizeof(font_path) - 1);
    font_path[sizeof(font_path) - 1] = '\0';

    // Test if file exists
    FILE *f = fopen(font_path, "r");
    if (!f) {
        fprintf(stderr, "Error: Could not open font file: %s\n", font_path);
        return 0;
    }
    fclose(f);

    initialized = 1;
    printf("Voxel text system initialized with font: %s\n", font_path);
    return 1;
}

void voxel_text_cleanup(void) {
    font_path[0] = '\0';
    initialized = 0;
    printf("Voxel text system cleaned up\n");
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

/**
 * Find a glyph in the hex file by codepoint
 * @param codepoint Unicode codepoint to search for
 * @param hex_data Buffer to store the hex data (output)
 * @param max_len Maximum length of hex_data buffer
 * @return 1 on success, 0 if glyph not found or error
 */
static int find_glyph(uint32_t codepoint, char *hex_data, int max_len) {
    if (!hex_data || max_len <= 0) {
        fprintf(stderr, "Error: find_glyph: Invalid buffer\n");
        return 0;
    }

    FILE *f = fopen(font_path, "r");
    if (!f) {
        fprintf(stderr, "Error: find_glyph: Could not open font file\n");
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
    // Input validation
    if (!text || !set_block_func) {
        fprintf(stderr, "Error: voxel_text_render: NULL parameter(s)\n");
        return 0;
    }

    if (!initialized || font_path[0] == '\0') {
        fprintf(stderr, "Error: Voxel text not initialized. Call voxel_text_init() first.\n");
        return 0;
    }

    // Validate scale
    if (scale < 1 || scale > MAX_SCALE) {
        fprintf(stderr, "Error: Invalid scale %d (must be 1-%d)\n", scale, MAX_SCALE);
        return 0;
    }

    // Validate block type (0-255 for standard Minecraft-like blocks)
    if (block_type < 0 || block_type > 255) {
        fprintf(stderr, "Warning: Block type %d out of range, using modulo 256\n", block_type);
        block_type = block_type & 0xFF;
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

// Helper function to word wrap text into lines
static int word_wrap_text(const char *text, int max_width, char lines[][256], int max_lines) {
    if (max_width <= 0 || strlen(text) <= (size_t)max_width) {
        // No wrapping needed
        strncpy(lines[0], text, 255);
        lines[0][255] = '\0';
        return 1;
    }

    int line_count = 0;
    const char *p = text;

    while (*p && line_count < max_lines) {
        int chars_to_copy = max_width;

        // If we're not at the end, try to break at a space
        if (strlen(p) > (size_t)max_width) {
            // Find last space within max_width
            const char *space = p + max_width;
            while (space > p && *space != ' ') {
                space--;
            }

            if (space > p) {
                // Found a space, break there
                chars_to_copy = space - p;
            }
            // Otherwise, hard break at max_width
        } else {
            // Last line
            chars_to_copy = strlen(p);
        }

        // Copy the line
        strncpy(lines[line_count], p, chars_to_copy);
        lines[line_count][chars_to_copy] = '\0';

        // Move to next line
        p += chars_to_copy;
        while (*p == ' ') p++; // Skip spaces

        line_count++;
    }

    return line_count;
}

int voxel_text_render_flat(
    const char *text,
    int x, int y, int z,
    int block_type,
    int max_width,
    int line_spacing,
    void (*set_block_func)(int x, int y, int z, int w)
) {
    // Input validation
    if (!text || !set_block_func) {
        fprintf(stderr, "Error: voxel_text_render_flat: NULL parameter(s)\n");
        return 0;
    }

    if (!initialized || font_path[0] == '\0') {
        fprintf(stderr, "Error: Voxel text not initialized. Call voxel_text_init() first.\n");
        return 0;
    }

    // Validate block type
    if (block_type < 0 || block_type > 255) {
        fprintf(stderr, "Warning: Block type %d out of range, using modulo 256\n", block_type);
        block_type = block_type & 0xFF;
    }

    // Split text into lines if needed
    char lines[100][256];
    int line_count;

    if (max_width > 0) {
        line_count = word_wrap_text(text, max_width, lines, 100);
    } else {
        // No wrapping, single line
        strncpy(lines[0], text, 255);
        lines[0][255] = '\0';
        line_count = 1;
    }

    // Render each line
    int current_z = z;
    for (int i = 0; i < line_count; i++) {
        const char *p = lines[i];
        int cursor_x = x;

        // Render each character in the line
        while (*p) {
            uint32_t codepoint = utf8_to_unicode(&p);
            if (codepoint == 0) break;

            // Get glyph data
            char hex_data[256];
            if (!find_glyph(codepoint, hex_data, sizeof(hex_data))) {
                // Missing glyph, render placeholder
                cursor_x += 8 + 1;
                continue;
            }

            int hex_len = strlen(hex_data);
            int width = (hex_len == 32) ? 8 : 16;
            int height = 16;

            // Render glyph flat (on XZ plane at height Y)
            // Parse hex data and place voxels
            for (int row = 0; row < height; row++) {
                int hex_per_row = width / 4;
                for (int hex_idx = 0; hex_idx < hex_per_row; hex_idx++) {
                    int pos = row * hex_per_row + hex_idx;
                    if (pos >= hex_len) break;

                    int nibble = hex_to_int(hex_data[pos]);

                    // Each hex digit represents 4 pixels
                    for (int bit = 0; bit < 4; bit++) {
                        if (nibble & (1 << (3 - bit))) {
                            // Pixel is set, place voxel
                            int px = hex_idx * 4 + bit;
                            int pz = row; // Row becomes Z offset

                            // Place block at (cursor_x + px, y, current_z + pz)
                            set_block_func(
                                cursor_x + px,
                                y,
                                current_z + pz,
                                block_type
                            );
                        }
                    }
                }
            }

            // Advance cursor
            cursor_x += width + 1; // Add spacing between characters
        }

        // Move to next line (advance Z)
        // Each character is 16 pixels tall
        current_z += 16 + line_spacing;
    }

    return line_count;
}
