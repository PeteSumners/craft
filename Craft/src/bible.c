#include "bible.h"
#include "voxel_text.h"
#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

static char *bible_file_path = NULL;
static int bible_initialized = 0;

// Forward declaration for landing platform generation (internal use)
static void generate_landing_platform_internal(
    int center_x, int center_z, int platform_y,
    void (*block_func)(int x, int y, int z, int w));

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

                    // Save position before rendering (for teleportation)
                    db_insert_bible_position(book, chapter, verse_num, x, y, current_z);

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
// start_chapter: which chapter to start from (1-based, use 1 for beginning)
static int render_book_scroll(
    const char *book_name, int num_chapters,
    int x, int y, int z,
    int block_type,
    int width,
    int start_chapter,
    void (*block_func)(int x, int y, int z, int w))
{
    int current_z = z;
    int line_spacing = 5; // Space between verses

    if (start_chapter > 1) {
        printf("Resuming %s from chapter %d of %d...\n", book_name, start_chapter, num_chapters);
    } else {
        printf("Rendering %s (%d chapters)...\n", book_name, num_chapters);
    }

    // Render each chapter sequentially (starting from start_chapter)
    for (int chapter = start_chapter; chapter <= num_chapters; chapter++) {
        // Save chapter start position (verse 0 = chapter start)
        db_insert_bible_position(book_name, chapter, 0, x, y, current_z);
        printf("  Saved position: %s %d:0 at (%d, %d, %d)\n", book_name, chapter, x, y, current_z);

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

        // Save progress after each chapter (so we can resume mid-book)
        // Note: This is done in the calling function bible_generate_world
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

    int book_spacing = 600; // Space between books in X
    int text_width = 40;    // Character width for text
    int num_books = BIBLE_BOOK_COUNT;  // All 66 books!

    // Check if we're resuming from a previous run
    char progress_str[32] = {0};
    int start_book_index = 0;
    int resuming = 0;

    if (db_get_metadata("bible_generation_progress", progress_str, sizeof(progress_str))) {
        if (strcmp(progress_str, "complete") == 0) {
            printf("Bible generation already complete!\n");
            return (num_books * book_spacing); // Return full extent
        } else {
            // Parse book index (book-level tracking only for simplicity)
            start_book_index = atoi(progress_str);

            if (start_book_index >= 0 && start_book_index < num_books) {
                resuming = 1;
                printf("\n");
                printf("========================================\n");
                printf("  RESUMING BIBLE GENERATION\n");
                printf("========================================\n");
                printf("  Starting from book %d of %d: %s\n",
                       start_book_index + 1, num_books, bible_books[start_book_index].name);
                printf("  (Will re-render current book from beginning)\n");
                printf("========================================\n\n");
            }
        }
    }

    if (!resuming) {
        printf("\n");
        printf("========================================\n");
        printf("  GENERATING BIBLE IN WORLD\n");
        printf("========================================\n");
        printf("  Rendering all 66 books of the Bible\n");
        printf("  Layout: Books as scrolls along Z-axis\n");
        printf("  Width: 40 characters\n");
        printf("  Y-level: %d\n", start_y);
        printf("  This will take several minutes...\n");
        printf("  TIP: You can stop and resume anytime!\n");
        printf("========================================\n\n");

        // FIRST: Generate info area at world origin (0, Y, 0)
        printf("Step 1: Generating info area at world origin (0, 75, 0)...\n");

        // Info area coordinates - center at world origin
        // Text will be centered at X=0 (not starting at X=0)
        int info_center_x = 0;       // World origin X (center of text)
        int info_y = start_y;        // Same Y level as Bible text (75)
        int info_z = 0;              // World origin Z

        // Calculate text centering offset
        // Longest line is 41 characters: "W/A/S/D - Move (horizontal in fly mode)"
        // With scale=2 and 8-pixel chars: 41 * 16 = 656 blocks wide
        // To center at X=0: start at -328, end at +328
        int longest_line_chars = 41;
        int blocks_per_char = 16;  // 8 pixels * scale 2
        int text_width = longest_line_chars * blocks_per_char;  // 656 blocks
        int text_start_x = info_center_x - (text_width / 2);    // -328

        // Generate viewing platform at spawn/teleport altitude (centered at origin)
        printf("  Creating viewing platform...\n");
        int viewing_altitude = info_y + 102; // Y=177 (102 blocks above text)
        for (int x = info_center_x - 7; x <= info_center_x + 7; x++) {
            for (int z = info_z - 7; z <= info_z + 7; z++) {
                block_func(x, viewing_altitude, z, 10); // GLASS platform at Y=177
            }
        }
        printf("    Viewing platform: 225 blocks at Y=%d, centered at X=%d\n",
               viewing_altitude, info_center_x);

        // SECOND: Render help/navigation text (centered at origin)
        printf("  Rendering help message...\n");

        // Help text coordinates - offset to center the text
        int help_x = text_start_x;   // Start at -328 to center text at X=0
        int help_y = info_y;
        int help_z = info_z;

        // Render help text using voxel_text_render_flat
        int current_help_z = help_z;
        int line_spacing = 18;

        // Welcome header
        voxel_text_render_flat("=== BIBLE WORLD NAVIGATION ===",
                               help_x, help_y, current_help_z, block_type, 60, 2, block_func);
        current_help_z += 35;

        voxel_text_render_flat("Commands:",
                               help_x, help_y, current_help_z, block_type, 60, 2, block_func);
        current_help_z += 24;

        voxel_text_render_flat("/bgoto - Return to this info area",
                               help_x, help_y, current_help_z, block_type, 60, 2, block_func);
        current_help_z += line_spacing;

        voxel_text_render_flat("/bgoto Genesis - Go to a book",
                               help_x, help_y, current_help_z, block_type, 60, 2, block_func);
        current_help_z += line_spacing;

        voxel_text_render_flat("/bgoto John 3 - Go to a chapter",
                               help_x, help_y, current_help_z, block_type, 60, 2, block_func);
        current_help_z += line_spacing;

        voxel_text_render_flat("/bgoto John 3:16 - Go to a verse",
                               help_x, help_y, current_help_z, block_type, 60, 2, block_func);
        current_help_z += 30;

        voxel_text_render_flat("Controls:",
                               help_x, help_y, current_help_z, block_type, 60, 2, block_func);
        current_help_z += 24;

        voxel_text_render_flat("Tab - Toggle flying mode",
                               help_x, help_y, current_help_z, block_type, 60, 2, block_func);
        current_help_z += line_spacing;

        voxel_text_render_flat("Space - Move up while flying",
                               help_x, help_y, current_help_z, block_type, 60, 2, block_func);
        current_help_z += line_spacing;

        voxel_text_render_flat("W/A/S/D - Move (horizontal in fly mode)",
                               help_x, help_y, current_help_z, block_type, 60, 2, block_func);
        current_help_z += 30;

        voxel_text_render_flat("- Genesis is 1000 blocks to the east ->",
                               help_x, help_y, current_help_z, block_type, 60, 2, block_func);
        current_help_z += 24;

        voxel_text_render_flat("You are at world origin: (0, 75, 0)",
                               help_x, help_y, current_help_z, block_type, 60, 2, block_func);

        // Save info area CENTER position to database (chapter -1, verse 0 = info area marker)
        // This is where we teleport to - the center of the text, not the start
        db_insert_bible_position("INFO", -1, 0, info_center_x, info_y, info_z);

        printf("  Help message complete!\n\n");
        printf("Step 2: Generating Bible books...\n");
    }

    int current_x = start_x + (start_book_index * book_spacing);

    // Render each book (starting from where we left off)
    for (int i = start_book_index; i < num_books; i++) {
        printf("[%d/%d] Generating %s scroll at X=%d...\n",
               i + 1, BIBLE_BOOK_COUNT, bible_books[i].name, current_x);

        // Save book start position (chapter 0, verse 0 = book start)
        db_insert_bible_position(bible_books[i].name, 0, 0, current_x, start_y, start_z);
        printf("  Saved book start: %s at (%d, %d, %d)\n", bible_books[i].name, current_x, start_y, start_z);

        // Render the entire book from chapter 1
        int z_extent = render_book_scroll(
            bible_books[i].name,
            bible_books[i].chapters,
            current_x,
            start_y,
            start_z,
            block_type,
            text_width,
            1, // Always start from chapter 1
            block_func
        );

        printf("    %s: %d blocks in Z-axis\n", bible_books[i].name, z_extent);

        // Save progress after completing the entire book
        char progress_buf[16];
        snprintf(progress_buf, sizeof(progress_buf), "%d", i + 1);
        db_set_metadata("bible_generation_progress", progress_buf);
        db_commit_sync();  // Force immediate synchronous commit

        printf("  Progress saved: %d/%d books complete\n\n", i + 1, num_books);

        // Move to next book position
        current_x += book_spacing;
    }

    // Mark generation as complete
    db_set_metadata("bible_generation_progress", "complete");
    db_commit_sync();  // Force immediate commit

    printf("\n");
    printf("========================================\n");
    printf("  BIBLE WORLD GENERATION COMPLETE!\n");
    printf("========================================\n");
    printf("  Generated all %d books!\n", num_books);
    printf("  X-axis range: %d to %d\n", start_x, start_x + (num_books * book_spacing));
    printf("  Genesis at X=%d, Revelation at X=%d\n", start_x, start_x + ((num_books - 1) * book_spacing));
    printf("\n");
    printf("  Navigation:\n");
    printf("    /bgoto               - Return to info area\n");
    printf("    /bgoto Genesis       - Go to book\n");
    printf("    /bgoto John 3        - Go to chapter\n");
    printf("    /bgoto John 3:16     - Go to exact verse\n");
    printf("\n");
    printf("  Flight controls:\n");
    printf("    Tab   - Toggle flying\n");
    printf("    Space - Move up\n");
    printf("    W/S   - Forward/back (horizontal only in fly mode)\n");
    printf("========================================\n\n");

    // Return the total X extent (how many books × spacing)
    return num_books * book_spacing;
}

// Internal function - Generate a small glass landing platform at a specific position
// Creates a 9×9 platform centered at (x, z) at the given Y level
static void generate_landing_platform_internal(
    int center_x, int center_z, int platform_y,
    void (*block_func)(int x, int y, int z, int w))
{
    int glass_block_type = 10; // GLASS
    int platform_size = 9; // 9×9 platform
    int half_size = platform_size / 2;

    for (int dx = -half_size; dx <= half_size; dx++) {
        for (int dz = -half_size; dz <= half_size; dz++) {
            block_func(center_x + dx, platform_y, center_z + dz, glass_block_type);
        }
    }
}

// Public wrapper - Generate a landing platform (called by bgoto)
void bible_generate_landing_platform(
    int center_x, int center_z, int platform_y,
    void (*block_func)(int x, int y, int z, int w))
{
    generate_landing_platform_internal(center_x, center_z, platform_y, block_func);
}

// Generate single spawn platform only
void bible_generate_glass_platforms(
    int start_x, int start_y, int start_z,
    int platform_y,
    void (*block_func)(int x, int y, int z, int w))
{
    printf("\n");
    printf("========================================\n");
    printf("  GENERATING SPAWN PLATFORM\n");
    printf("========================================\n");
    printf("  Position: X=%d, Y=%d, Z=%d\n", start_x, platform_y, start_z);
    printf("  Size: 11x11 blocks (X: %d to %d, Z: %d to %d)\n",
           start_x - 5, start_x + 5, start_z - 5, start_z + 5);
    printf("  Block type: 3 (STONE - visible for testing)\n");
    printf("========================================\n\n");

    // Generate small 11×11 spawn platform
    // Using STONE (3) temporarily for visibility - change to GLASS (10) later
    int platform_block_type = 3; // STONE - visible for testing
    int blocks_placed = 0;

    printf("Placing blocks...\n");
    for (int x = start_x - 5; x <= start_x + 5; x++) {
        for (int z = start_z - 5; z <= start_z + 5; z++) {
            // Place the block using provided function
            block_func(x, platform_y, z, platform_block_type);
            blocks_placed++;

            // Debug: print first few blocks
            if (blocks_placed <= 3) {
                printf("  Placed block at (%d, %d, %d)\n", x, platform_y, z);
            }
        }
    }

    printf("Spawn platform complete! Placed %d blocks.\n", blocks_placed);
    printf("Note: Platform will be visible after restart/reload.\n");
    printf("========================================\n\n");
}

// Render help message at spawn explaining controls
void bible_render_help_message(
    int x, int y, int z,
    int block_type,
    void (*block_func)(int x, int y, int z, int w))
{
    printf("\n");
    printf("========================================\n");
    printf("  RENDERING HELP MESSAGE AT SPAWN\n");
    printf("========================================\n");
    printf("  Position: (%d, %d, %d)\n", x, y, z);
    printf("========================================\n\n");

    // Render help text using voxel_text_render_flat (horizontal layout)
    int current_z = z;
    int line_spacing = 22;

    // Welcome header
    voxel_text_render_flat("=== WELCOME TO THE BIBLE WORLD ===",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += 40;

    // Navigation section
    voxel_text_render_flat("NAVIGATION:",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += line_spacing;

    voxel_text_render_flat("/bgoto - Return to this info area",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += line_spacing;

    voxel_text_render_flat("/bgoto Genesis - Go to a book",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += line_spacing;

    voxel_text_render_flat("/bgoto John 3 - Go to a chapter",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += line_spacing;

    voxel_text_render_flat("/bgoto John 3:16 - Go to a verse",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += 30;

    // Controls section
    voxel_text_render_flat("CONTROLS:",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += line_spacing;

    voxel_text_render_flat("Tab - Toggle flying mode",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += line_spacing;

    voxel_text_render_flat("W/A/S/D - Move horizontally (flying)",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += line_spacing;

    voxel_text_render_flat("Space - Move up",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += line_spacing;

    voxel_text_render_flat("Mouse - Look around",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += 30;

    // Tips section
    voxel_text_render_flat("TIPS:",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += line_spacing;

    voxel_text_render_flat("- You are on a glass platform above the Bible",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += line_spacing;

    voxel_text_render_flat("- Look down to read the text below",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += line_spacing;

    voxel_text_render_flat("- Genesis starts at X=1000",
                           x, y, current_z, block_type, 60, 2, block_func);
    current_z += line_spacing;

    voxel_text_render_flat("- Use /bgoto to quickly navigate",
                           x, y, current_z, block_type, 60, 2, block_func);

    printf("Help message rendered successfully!\n\n");
}

// Get the number of chapters in a book
// Returns chapter count, or 0 if book not found
int bible_get_chapter_count(const char *book) {
    if (!book) return 0;

    // Search through the bible_books array
    for (int i = 0; i < BIBLE_BOOK_COUNT; i++) {
#ifdef _WIN32
        if (_stricmp(book, bible_books[i].name) == 0) {
#else
        if (strcasecmp(book, bible_books[i].name) == 0) {
#endif
            return bible_books[i].chapters;
        }
    }

    // Book not found
    return 0;
}

// Get the number of verses in a specific chapter
// Returns verse count, or 0 if book/chapter not found
int bible_get_verse_count(const char *book, int chapter) {
    if (!bible_initialized || !book || chapter < 1) {
        return 0;
    }

    // Validate chapter exists
    int chapter_count = bible_get_chapter_count(book);
    if (chapter > chapter_count) {
        return 0;
    }

    // Open Bible file and count verses in this chapter
    FILE *f = fopen(bible_file_path, "r");
    if (!f) {
        return 0;
    }

    char line[4096];
    char search_prefix[256];
    snprintf(search_prefix, sizeof(search_prefix), "%s %d:", book, chapter);

    int verse_count = 0;
    int max_verse = 0;

    // Skip first 2 header lines
    fgets(line, sizeof(line), f);
    fgets(line, sizeof(line), f);

    // Count verses matching "Book Chapter:*"
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, search_prefix, strlen(search_prefix)) == 0) {
            // Found a verse in this chapter
            // Extract verse number
            const char *verse_str = line + strlen(search_prefix);
            int verse_num = atoi(verse_str);
            if (verse_num > max_verse) {
                max_verse = verse_num;
            }
            verse_count++;
        }
    }

    fclose(f);
    return max_verse; // Return highest verse number found
}

// ============================================================================
// DAILY READING PLAN IMPLEMENTATION
// ============================================================================

// Configuration for daily reading area
#define DAILY_READING_X -1200       // Negative X, well separated from preface area at origin
#define DAILY_READING_Y 75          // Same Y level as main Bible
#define DAILY_READING_Z 0           // Starting Z
#define DAILY_READING_WIDTH 50      // Text width
#define DAILY_READING_BLOCK_TYPE 3  // Stone blocks

// Get current date in ISO format (YYYY-MM-DD)
// For 2026 daily reading plan
static void get_current_date_iso(char *buffer, size_t buffer_size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(buffer, buffer_size, "%04d-%02d-%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
}

// Get day of year (1-365) for 2026
// This function works for any year, but the daily reading plan is specifically for 2026
static int get_day_of_year() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    return t->tm_yday + 1; // tm_yday is 0-based, ranges from 1-365 (or 366 for leap years)
}

// Check if current year is 2026
static int is_year_2026() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    return (t->tm_year + 1900) == 2026;
}

// Map chapter index (0-1188) to book and chapter
// Returns 1 on success, 0 on failure
static int chapter_index_to_book_chapter(int chapter_idx, char *out_book, int *out_chapter) {
    // This is a cumulative index across all 66 books
    int cumulative = 0;

    for (int i = 0; i < BIBLE_BOOK_COUNT; i++) {
        int book_chapters = bible_books[i].chapters;
        if (chapter_idx < cumulative + book_chapters) {
            // Found the book!
            strcpy(out_book, bible_books[i].name);
            *out_chapter = (chapter_idx - cumulative) + 1; // 1-based chapter
            return 1;
        }
        cumulative += book_chapters;
    }
    return 0;
}

// Array to store Z coordinates for each day (for teleportation)
static int daily_reading_z_offsets[365] = {0};

// Precalculated Bible reading plan for 2026 (365 days)
// Auto-generated by generate_2026_plan.py
// Total chapters: 1,189 distributed across 365 days
// The daily_plan_2026.h file contains all type definitions and data
#include "../daily_plan_2026.h"

// Get which chapters to read for a given day (1-365)
// Returns number of chapters
// Now uses precalculated data for 2026 with parallel OT and NT plans
int bible_get_daily_chapters(int day_of_year, char out_books[][64], int out_chapters[], int max_entries) {
    if (day_of_year < 1 || day_of_year > 365) {
        fprintf(stderr, "Invalid day_of_year: %d (must be 1-365)\n", day_of_year);
        return 0;
    }

    // Use precalculated plan for 2026 (array is 0-indexed, days are 1-indexed)
    const DayPlan *plan = &daily_plan_2026[day_of_year - 1];

    if (plan->count > max_entries) {
        fprintf(stderr, "Not enough space in output arrays (need %d, have %d)\n",
                plan->count, max_entries);
        return 0;
    }

    // Copy the readings from precalculated plan
    for (int i = 0; i < plan->count; i++) {
        strncpy(out_books[i], plan->readings[i].book, 63);
        out_books[i][63] = '\0';
        out_chapters[i] = plan->readings[i].chapter;
    }

    return plan->count;
}

// Get Z coordinate for a specific day (1-365)
// Returns the Z coordinate where that day's reading starts
int bible_get_daily_z_offset(int day_of_year) {
    if (day_of_year < 1 || day_of_year > 365) {
        return DAILY_READING_Z;
    }
    return daily_reading_z_offsets[day_of_year - 1];
}

// Generate all 365 days of daily readings (2026 plan)
// Renders the entire year in one permanent table
int bible_generate_daily_reading(void (*block_func)(int x, int y, int z, int w)) {
    if (!bible_initialized || !bible_file_path) {
        fprintf(stderr, "Bible system not initialized\n");
        return 0;
    }

    printf("\n");
    printf("========================================\n");
    printf("  GENERATING DAILY READING TABLE\n");
    printf("  2026 Plan - All 365 Days\n");
    printf("========================================\n");

    // Generate main viewing platform
    printf("Creating viewing platform at (%d, %d, %d)...\n",
           DAILY_READING_X, DAILY_READING_Y + 102, DAILY_READING_Z);
    int viewing_altitude = DAILY_READING_Y + 102;
    for (int x = DAILY_READING_X - 7; x <= DAILY_READING_X + 7; x++) {
        for (int z = DAILY_READING_Z - 7; z <= DAILY_READING_Z + 200; z++) {
            block_func(x, viewing_altitude, z, 10); // GLASS
        }
    }

    // Render title header
    printf("Rendering header...\n");
    int current_z = DAILY_READING_Z;
    char title[256];
    snprintf(title, sizeof(title), "=== 2026 DAILY BIBLE READING PLAN ===");
    int title_lines = voxel_text_render_flat(
        title,
        DAILY_READING_X - 200,
        DAILY_READING_Y,
        current_z,
        DAILY_READING_BLOCK_TYPE,
        DAILY_READING_WIDTH,
        2,  // Scale
        block_func
    );
    current_z += (title_lines * 18) + 50;

    // Render instructions
    char instructions[] = "Use /daily [day] to teleport (e.g., /daily 3 for Jan 3)";
    int inst_lines = voxel_text_render_flat(
        instructions,
        DAILY_READING_X - 150,
        DAILY_READING_Y,
        current_z,
        DAILY_READING_BLOCK_TYPE,
        DAILY_READING_WIDTH + 10,
        1,  // Scale
        block_func
    );
    current_z += (inst_lines * 18) + 100;

    // Render all 365 days
    for (int day = 1; day <= 365; day++) {
        // Store Z offset for this day (for teleportation)
        daily_reading_z_offsets[day - 1] = current_z;

        // Get plan for this day
        const DayPlan *plan = &daily_plan_2026[day - 1];

        // Render day header
        char day_header[256];
        snprintf(day_header, sizeof(day_header), "DAY %d - ", day);

        // Add date (simple calculation for 2026)
        // Jan 1, 2026 is day 1
        static const int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        static const char *month_names[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        int day_of_month = day;
        int month = 0;
        while (day_of_month > month_days[month]) {
            day_of_month -= month_days[month];
            month++;
        }
        char date_str[64];
        snprintf(date_str, sizeof(date_str), "%s %d", month_names[month], day_of_month);
        strncat(day_header, date_str, sizeof(day_header) - strlen(day_header) - 1);

        int header_lines = voxel_text_render_flat(
            day_header,
            DAILY_READING_X,
            DAILY_READING_Y,
            current_z,
            DAILY_READING_BLOCK_TYPE,
            DAILY_READING_WIDTH,
            2,
            block_func
        );
        current_z += (header_lines * 18) + 20;

        // Render OT readings
        int has_ot = 0;
        for (int i = 0; i < plan->count; i++) {
            if (strcmp(plan->readings[i].testament, "OT") == 0) {
                has_ot = 1;
                break;
            }
        }

        if (has_ot) {
            char ot_label[] = "  OLD TESTAMENT:";
            int ot_label_lines = voxel_text_render_flat(
                ot_label,
                DAILY_READING_X,
                DAILY_READING_Y,
                current_z,
                DAILY_READING_BLOCK_TYPE,
                DAILY_READING_WIDTH,
                1,
                block_func
            );
            current_z += (ot_label_lines * 18) + 10;

            for (int i = 0; i < plan->count; i++) {
                if (strcmp(plan->readings[i].testament, "OT") == 0) {
                    char chapter_line[128];
                    snprintf(chapter_line, sizeof(chapter_line), "    %s %d",
                             plan->readings[i].book, plan->readings[i].chapter);
                    int line_height = voxel_text_render_flat(
                        chapter_line,
                        DAILY_READING_X,
                        DAILY_READING_Y,
                        current_z,
                        DAILY_READING_BLOCK_TYPE,
                        DAILY_READING_WIDTH,
                        1,
                        block_func
                    );
                    current_z += (line_height * 18) + 10;
                }
            }
        }

        // Render NT readings
        int has_nt = 0;
        for (int i = 0; i < plan->count; i++) {
            if (strcmp(plan->readings[i].testament, "NT") == 0) {
                has_nt = 1;
                break;
            }
        }

        if (has_nt) {
            char nt_label[] = "  NEW TESTAMENT:";
            int nt_label_lines = voxel_text_render_flat(
                nt_label,
                DAILY_READING_X,
                DAILY_READING_Y,
                current_z,
                DAILY_READING_BLOCK_TYPE,
                DAILY_READING_WIDTH,
                1,
                block_func
            );
            current_z += (nt_label_lines * 18) + 10;

            for (int i = 0; i < plan->count; i++) {
                if (strcmp(plan->readings[i].testament, "NT") == 0) {
                    char chapter_line[128];
                    snprintf(chapter_line, sizeof(chapter_line), "    %s %d",
                             plan->readings[i].book, plan->readings[i].chapter);
                    int line_height = voxel_text_render_flat(
                        chapter_line,
                        DAILY_READING_X,
                        DAILY_READING_Y,
                        current_z,
                        DAILY_READING_BLOCK_TYPE,
                        DAILY_READING_WIDTH,
                        1,
                        block_func
                    );
                    current_z += (line_height * 18) + 10;
                }
            }
        }

        // Add spacing between days
        current_z += 60;

        // Progress indicator
        if (day % 10 == 0) {
            printf("  Generated day %d / 365\n", day);
        }
    }

    printf("\n");
    printf("========================================\n");
    printf("  DAILY READING TABLE COMPLETE\n");
    printf("  Total days: 365\n");
    printf("  Total Z extent: %d blocks\n", current_z - DAILY_READING_Z);
    printf("  Use /daily to teleport to today!\n");
    printf("========================================\n\n");

    return 1;
}

