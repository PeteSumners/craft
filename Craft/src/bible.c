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
        return 0;
    }

    // Return Z extent (how much we advanced in Z), not line count
    return current_z - z;
}

// WORLD GENERATION - Render entire Bible as scrolls

// Bible book information: name and chapter count
typedef struct {
    const char *name;
    int chapters;
} BibleBook;

// All 66 books of the Bible in order
static const BibleBook bible_books[] = {
    // Old Testament (39 books)
    {"Genesis", 50}, {"Exodus", 40}, {"Leviticus", 27}, {"Numbers", 36}, {"Deuteronomy", 34},
    {"Joshua", 24}, {"Judges", 21}, {"Ruth", 4}, {"1 Samuel", 31}, {"2 Samuel", 24},
    {"1 Kings", 22}, {"2 Kings", 25}, {"1 Chronicles", 29}, {"2 Chronicles", 36}, {"Ezra", 10},
    {"Nehemiah", 13}, {"Esther", 10}, {"Job", 42}, {"Psalm", 150}, {"Proverbs", 31},
    {"Ecclesiastes", 12}, {"Song of Solomon", 8}, {"Isaiah", 66}, {"Jeremiah", 52}, {"Lamentations", 5},
    {"Ezekiel", 48}, {"Daniel", 12}, {"Hosea", 14}, {"Joel", 3}, {"Amos", 9},
    {"Obadiah", 1}, {"Jonah", 4}, {"Micah", 7}, {"Nahum", 3}, {"Habakkuk", 3},
    {"Zephaniah", 3}, {"Haggai", 2}, {"Zechariah", 14}, {"Malachi", 4},
    // New Testament (27 books)
    {"Matthew", 28}, {"Mark", 16}, {"Luke", 24}, {"John", 21}, {"Acts", 28},
    {"Romans", 16}, {"1 Corinthians", 16}, {"2 Corinthians", 13}, {"Galatians", 6}, {"Ephesians", 6},
    {"Philippians", 4}, {"Colossians", 4}, {"1 Thessalonians", 5}, {"2 Thessalonians", 3}, {"1 Timothy", 6},
    {"2 Timothy", 4}, {"Titus", 3}, {"Philemon", 1}, {"Hebrews", 13}, {"James", 5},
    {"1 Peter", 5}, {"2 Peter", 3}, {"1 John", 5}, {"2 John", 1}, {"3 John", 1},
    {"Jude", 1}, {"Revelation", 22}
};

#define BIBLE_BOOK_COUNT 66

// Render entire book as continuous scroll along Z-axis
// Returns the Z extent (how far in Z the book extends)
static int render_book_scroll(
    const char *book_name, int num_chapters,
    int x, int y, int z,
    int block_type,
    int width,
    void (*block_func)(int x, int y, int z, int w))
{
    int current_z = z;
    int line_spacing = 5; // Space between verses

    printf("Rendering %s (%d chapters)...\n", book_name, num_chapters);

    // Render each chapter sequentially
    for (int chapter = 1; chapter <= num_chapters; chapter++) {
        // Add chapter header
        char chapter_header[100];
        snprintf(chapter_header, sizeof(chapter_header), "=== %s %d ===", book_name, chapter);

        int header_lines = voxel_text_render_flat(
            chapter_header,
            x, y, current_z,
            block_type,
            width,
            2,
            block_func
        );

        current_z += (header_lines * 18) + 30; // Extra space after header

        // Render the chapter (returns Z extent used)
        int z_extent = bible_render_chapter_flat(
            book_name, chapter,
            x, y, current_z,
            block_type,
            width,
            line_spacing,
            block_func
        );

        if (z_extent > 0) {
            printf("  Chapter %d: %d blocks in Z\n", chapter, z_extent);
            current_z += z_extent + 40; // Add chapter extent plus extra space between chapters
        }
    }

    // Return total Z extent used
    return current_z - z;
}

int bible_generate_world(
    int start_x, int start_y, int start_z,
    int block_type,
    void (*block_func)(int x, int y, int z, int w))
{
    if (!bible_initialized || !bible_file_path) {
        fprintf(stderr, "Bible system not initialized\n");
        return 0;
    }

    printf("\n");
    printf("========================================\n");
    printf("  GENERATING BIBLE IN WORLD\n");
    printf("========================================\n");
    printf("  Testing with Genesis and Exodus\n");
    printf("  Layout: Books as scrolls along Z-axis\n");
    printf("  Width: 40 characters\n");
    printf("  Y-level: %d\n", start_y);
    printf("========================================\n\n");

    int current_x = start_x;
    int book_spacing = 600; // Space between books in X
    int text_width = 40;    // Character width for text
    int num_books = 2;      // Just Genesis and Exodus for now

    // Render each book
    for (int i = 0; i < num_books; i++) {
        printf("[%d/%d] Generating %s scroll at X=%d...\n",
               i + 1, BIBLE_BOOK_COUNT, bible_books[i].name, current_x);

        int z_extent = render_book_scroll(
            bible_books[i].name,
            bible_books[i].chapters,
            current_x,
            start_y,
            start_z,
            block_type,
            text_width,
            block_func
        );

        printf("    %s: %d blocks in Z-axis\n\n", bible_books[i].name, z_extent);

        // Move to next book position
        current_x += book_spacing;
    }

    printf("\n");
    printf("========================================\n");
    printf("  BIBLE WORLD GENERATION COMPLETE!\n");
    printf("========================================\n");
    printf("  Generated %d books at X=%d to X=%d\n", num_books, start_x, current_x - book_spacing);
    printf("  Genesis at X=%d, Exodus at X=%d\n", start_x, start_x + book_spacing);
    printf("  Navigate:\n");
    printf("    - Fly along X-axis to switch books\n");
    printf("    - Fly along Z-axis to read through book\n");
    printf("    - Commands: Tab (fly), Space (up), W/S (forward/back)\n");
    printf("========================================\n\n");

    return 1;
}
