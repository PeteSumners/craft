#include "bible.h"
#include "voxel_text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char *bible_file_path = NULL;
static int bible_initialized = 0;

// Helper function to trim whitespace
static void trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    // All spaces?
    if(*str == 0) return;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    end[1] = '\0';
}

// Helper function to word wrap text
// Returns number of lines created
static int word_wrap(const char *text, int max_width, char lines[][256], int max_lines) {
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
        trim_whitespace(lines[line_count]);

        // Move to next line
        p += chars_to_copy;
        while (*p == ' ') p++; // Skip spaces

        line_count++;
    }

    return line_count;
}

int bible_init(const char *bible_path) {
    if (bible_initialized) {
        bible_cleanup();
    }

    // Test if file exists
    FILE *f = fopen(bible_path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open Bible file: %s\n", bible_path);
        return 0;
    }
    fclose(f);

    // Store path
    bible_file_path = malloc(strlen(bible_path) + 1);
    if (!bible_file_path) {
        return 0;
    }
    strcpy(bible_file_path, bible_path);

    bible_initialized = 1;
    printf("Bible system initialized with file: %s\n", bible_path);
    return 1;
}

void bible_cleanup(void) {
    if (bible_file_path) {
        free(bible_file_path);
        bible_file_path = NULL;
    }
    bible_initialized = 0;
}

int bible_get_verse_text(
    const char *book, int chapter, int verse,
    char *text_buffer, int buffer_size)
{
    if (!bible_initialized || !bible_file_path) {
        fprintf(stderr, "Bible system not initialized\n");
        return 0;
    }

    FILE *f = fopen(bible_file_path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open Bible file\n");
        return 0;
    }

    char line[1024];
    char search_pattern[256];

    // Build search pattern: "Book Chapter:Verse\t"
    snprintf(search_pattern, sizeof(search_pattern), "%s %d:%d\t", book, chapter, verse);

    int found = 0;

    // Skip first 2 header lines
    fgets(line, sizeof(line), f);
    fgets(line, sizeof(line), f);

    // Search for the verse
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, search_pattern, strlen(search_pattern)) == 0) {
            // Found it! Extract the text after the tab
            const char *text = strchr(line, '\t');
            if (text) {
                text++; // Skip the tab
                strncpy(text_buffer, text, buffer_size - 1);
                text_buffer[buffer_size - 1] = '\0';

                // Remove trailing newline
                size_t len = strlen(text_buffer);
                if (len > 0 && text_buffer[len - 1] == '\n') {
                    text_buffer[len - 1] = '\0';
                }
                if (len > 1 && text_buffer[len - 2] == '\r') {
                    text_buffer[len - 2] = '\0';
                }

                found = 1;
                break;
            }
        }
    }

    fclose(f);

    if (!found) {
        fprintf(stderr, "Verse not found: %s %d:%d\n", book, chapter, verse);
    }

    return found;
}

int bible_render_verse(
    const char *book, int chapter, int verse,
    int x, int y, int z,
    int block_type,
    int max_width,
    void (*block_func)(int x, int y, int z, int w))
{
    char verse_text[1024];

    if (!bible_get_verse_text(book, chapter, verse, verse_text, sizeof(verse_text))) {
        return 0;
    }

    // Add verse reference at the beginning
    char full_text[1200];
    snprintf(full_text, sizeof(full_text), "%s %d:%d - %s", book, chapter, verse, verse_text);

    // Handle word wrapping
    if (max_width > 0) {
        char lines[20][256];
        int line_count = word_wrap(full_text, max_width, lines, 20);

        int current_y = y;
        for (int i = 0; i < line_count; i++) {
            voxel_text_render(lines[i], x, current_y, z, block_type, 1, block_func);
            current_y -= 20; // Move down (each character is 16 pixels + 4 spacing)
        }

        return line_count;
    } else {
        // No wrapping
        voxel_text_render(full_text, x, y, z, block_type, 1, block_func);
        return 1;
    }
}

int bible_render_verse_range(
    const char *book, int chapter, int verse_start, int verse_end,
    int x, int y, int z,
    int block_type,
    int max_width,
    int line_spacing,
    void (*block_func)(int x, int y, int z, int w))
{
    int total_lines = 0;
    int current_y = y;

    for (int v = verse_start; v <= verse_end; v++) {
        int lines = bible_render_verse(
            book, chapter, v,
            x, current_y, z,
            block_type,
            max_width,
            block_func
        );

        if (lines > 0) {
            total_lines += lines;
            current_y -= (lines * 20) + line_spacing;
        }
    }

    return total_lines;
}

int bible_render_chapter(
    const char *book, int chapter,
    int x, int y, int z,
    int block_type,
    int max_width,
    int line_spacing,
    void (*block_func)(int x, int y, int z, int w))
{
    if (!bible_initialized || !bible_file_path) {
        fprintf(stderr, "Bible system not initialized\n");
        return 0;
    }

    FILE *f = fopen(bible_file_path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open Bible file\n");
        return 0;
    }

    char line[1024];
    char search_pattern[256];
    int total_lines = 0;
    int current_y = y;

    // Skip first 2 header lines
    fgets(line, sizeof(line), f);
    fgets(line, sizeof(line), f);

    // Build search pattern for the chapter: "Book Chapter:"
    snprintf(search_pattern, sizeof(search_pattern), "%s %d:", book, chapter);

    // Read through file and render all verses in this chapter
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, search_pattern, strlen(search_pattern)) == 0) {
            // Found a verse in this chapter
            // Parse verse number
            const char *colon = strchr(line, ':');
            if (colon) {
                int verse_num = atoi(colon + 1);

                // Extract verse text
                const char *tab = strchr(line, '\t');
                if (tab) {
                    tab++; // Skip tab
                    char verse_text[1024];
                    strncpy(verse_text, tab, sizeof(verse_text) - 1);
                    verse_text[sizeof(verse_text) - 1] = '\0';

                    // Remove trailing newline
                    size_t len = strlen(verse_text);
                    if (len > 0 && verse_text[len - 1] == '\n') {
                        verse_text[len - 1] = '\0';
                    }
                    if (len > 1 && verse_text[len - 2] == '\r') {
                        verse_text[len - 2] = '\0';
                    }

                    // Format with verse number
                    char full_text[1200];
                    snprintf(full_text, sizeof(full_text), "%d. %s", verse_num, verse_text);

                    // Handle word wrapping
                    if (max_width > 0) {
                        char lines[20][256];
                        int line_count = word_wrap(full_text, max_width, lines, 20);

                        for (int i = 0; i < line_count; i++) {
                            voxel_text_render(lines[i], x, current_y, z, block_type, 1, block_func);
                            current_y -= 20;
                        }

                        total_lines += line_count;
                        current_y -= line_spacing;
                    } else {
                        voxel_text_render(full_text, x, current_y, z, block_type, 1, block_func);
                        current_y -= 20 + line_spacing;
                        total_lines++;
                    }
                }
            }
        }
        // Stop if we've moved to the next chapter
        else if (total_lines > 0) {
            // We've already found verses, and this line doesn't match
            // Check if it's a new chapter/book
            char *space = strchr(line, ' ');
            if (space) {
                char line_book[256];
                strncpy(line_book, line, space - line);
                line_book[space - line] = '\0';

                // If book name matches, it might be next chapter
                // For simplicity, just stop here
                if (strncmp(line_book, book, strlen(book)) == 0) {
                    // Still same book, check chapter
                    int line_chapter = atoi(space + 1);
                    if (line_chapter != chapter) {
                        break; // Moved to different chapter
                    }
                }
            }
        }
    }

    fclose(f);

    if (total_lines == 0) {
        fprintf(stderr, "Chapter not found: %s %d\n", book, chapter);
    }

    return total_lines;
}

// FLAT RENDERING FUNCTIONS

int bible_render_verse_range_flat(
    const char *book, int chapter, int verse_start, int verse_end,
    int x, int y, int z,
    int block_type,
    int max_width,
    int line_spacing,
    void (*block_func)(int x, int y, int z, int w))
{
    if (!bible_initialized || !bible_file_path) {
        fprintf(stderr, "Bible system not initialized\n");
        return 0;
    }

    int total_lines = 0;
    int current_z = z;

    for (int v = verse_start; v <= verse_end; v++) {
        char verse_text[1024];

        if (!bible_get_verse_text(book, chapter, v, verse_text, sizeof(verse_text))) {
            continue;
        }

        // Format with verse number
        char full_text[1200];
        snprintf(full_text, sizeof(full_text), "%d. %s", v, verse_text);

        // Render flat
        int lines = voxel_text_render_flat(
            full_text,
            x, y, current_z,
            block_type,
            max_width,
            2, // Internal line spacing within this verse
            block_func
        );

        total_lines += lines;

        // Move Z for next verse (verse spacing)
        current_z += (lines * 18) + line_spacing;
    }

    return total_lines;
}

int bible_render_chapter_flat(
    const char *book, int chapter,
    int x, int y, int z,
    int block_type,
    int max_width,
    int line_spacing,
    void (*block_func)(int x, int y, int z, int w))
{
    if (!bible_initialized || !bible_file_path) {
        fprintf(stderr, "Bible system not initialized\n");
        return 0;
    }

    FILE *f = fopen(bible_file_path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open Bible file\n");
        return 0;
    }

    char line[1024];
    char search_pattern[256];
    int total_lines = 0;
    int current_z = z;

    // Skip first 2 header lines
    fgets(line, sizeof(line), f);
    fgets(line, sizeof(line), f);

    // Build search pattern for the chapter: "Book Chapter:"
    snprintf(search_pattern, sizeof(search_pattern), "%s %d:", book, chapter);

    // Read through file and render all verses in this chapter
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, search_pattern, strlen(search_pattern)) == 0) {
            // Found a verse in this chapter
            // Parse verse number
            const char *colon = strchr(line, ':');
            if (colon) {
                int verse_num = atoi(colon + 1);

                // Extract verse text
                const char *tab = strchr(line, '\t');
                if (tab) {
                    tab++; // Skip tab
                    char verse_text[1024];
                    strncpy(verse_text, tab, sizeof(verse_text) - 1);
                    verse_text[sizeof(verse_text) - 1] = '\0';

                    // Remove trailing newline
                    size_t len = strlen(verse_text);
                    if (len > 0 && verse_text[len - 1] == '\n') {
                        verse_text[len - 1] = '\0';
                    }
                    if (len > 1 && verse_text[len - 2] == '\r') {
                        verse_text[len - 2] = '\0';
                    }

                    // Format with verse number
                    char full_text[1200];
                    snprintf(full_text, sizeof(full_text), "%d. %s", verse_num, verse_text);

                    // Render flat
                    int lines = voxel_text_render_flat(
                        full_text,
                        x, y, current_z,
                        block_type,
                        max_width,
                        2, // Internal line spacing
                        block_func
                    );

                    total_lines += lines;

                    // Move Z for next verse
                    current_z += (lines * 18) + line_spacing;
                }
            }
        }
        // Stop if we've moved to the next chapter
        else if (total_lines > 0) {
            char *space = strchr(line, ' ');
            if (space) {
                char line_book[256];
                strncpy(line_book, line, space - line);
                line_book[space - line] = '\0';

                if (strncmp(line_book, book, strlen(book)) == 0) {
                    int line_chapter = atoi(space + 1);
                    if (line_chapter != chapter) {
                        break;
                    }
                }
            }
        }
    }

    fclose(f);

    if (total_lines == 0) {
        fprintf(stderr, "Chapter not found: %s %d\n", book, chapter);
    }

    return total_lines;
}
