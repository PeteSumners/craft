#ifndef _bible_h_
#define _bible_h_

// Bible text rendering system for KJV text file

// Initialize the Bible system with path to KJV text file
int bible_init(const char *bible_path);

// Clean up Bible system
void bible_cleanup(void);

// Render a single verse at position (x, y, z)
// Returns the number of lines rendered (for vertical spacing)
// book: "Genesis", "Exodus", "1 Kings", etc.
// chapter: chapter number (1-based)
// verse: verse number (1-based)
// block_type: block type to use for rendering
// max_width: maximum width in characters before word wrap (0 = no wrap)
// block_func: function to place blocks
int bible_render_verse(
    const char *book, int chapter, int verse,
    int x, int y, int z,
    int block_type,
    int max_width,
    void (*block_func)(int x, int y, int z, int w)
);

// Render a range of verses (e.g., Genesis 1:1-3)
// Returns the total number of lines rendered
int bible_render_verse_range(
    const char *book, int chapter, int verse_start, int verse_end,
    int x, int y, int z,
    int block_type,
    int max_width,
    int line_spacing,
    void (*block_func)(int x, int y, int z, int w)
);

// Render an entire chapter
// Returns the total number of lines rendered
int bible_render_chapter(
    const char *book, int chapter,
    int x, int y, int z,
    int block_type,
    int max_width,
    int line_spacing,
    void (*block_func)(int x, int y, int z, int w)
);

// Search for a verse and get its text (for testing/debugging)
// Returns 1 if found, 0 if not found
// text_buffer: buffer to store verse text
// buffer_size: size of text_buffer
int bible_get_verse_text(
    const char *book, int chapter, int verse,
    char *text_buffer, int buffer_size
);

// FLAT RENDERING (readable from above, uses Z axis for lines)
// Perfect for large-scale Bible rendering in the sky

// Render a chapter flat (horizontal)
// All text at same Y level, lines advance along Z axis
// Readable from above - fly up and look down!
int bible_render_chapter_flat(
    const char *book, int chapter,
    int x, int y, int z,
    int block_type,
    int max_width,
    int line_spacing,
    void (*block_func)(int x, int y, int z, int w)
);

// Render a verse range flat
int bible_render_verse_range_flat(
    const char *book, int chapter, int verse_start, int verse_end,
    int x, int y, int z,
    int block_type,
    int max_width,
    int line_spacing,
    void (*block_func)(int x, int y, int z, int w)
);

// WORLD GENERATION
// Generate the entire Bible in the world as part of world generation
// This renders all 66 books, 1,189 chapters in a massive grid
// Uses flat rendering for space efficiency
// Returns the X extent (total width in blocks) on success, 0 on failure
int bible_generate_world(
    int start_x, int start_y, int start_z,
    int block_type,
    void (*block_func)(int x, int y, int z, int w)
);

// Generate glass platforms at teleport points
// Creates spawn platform and prepares for per-location landing pads
void bible_generate_glass_platforms(
    int start_x, int start_y, int start_z,
    int platform_y,
    void (*block_func)(int x, int y, int z, int w)
);

// Render help message at spawn explaining controls
void bible_render_help_message(
    int x, int y, int z,
    int block_type,
    void (*block_func)(int x, int y, int z, int w)
);

// Generate a small glass landing platform at specific coordinates
// Used by bgoto to ensure a platform exists at teleport destination
void bible_generate_landing_platform(
    int center_x, int center_z, int platform_y,
    void (*block_func)(int x, int y, int z, int w)
);

// DAILY READING PLAN FUNCTIONS (2026)
// Generate the entire 365-day reading plan table (2026)
// Renders all 365 days in one permanent structure
// Returns 1 on success, 0 on failure
// Renders at negative X coordinates (separate from main Bible)
int bible_generate_daily_reading(
    void (*block_func)(int x, int y, int z, int w)
);

// Get reading chapters for a specific day (1-365)
// Returns number of chapters to read
// Fills out_books and out_chapters arrays with the reading list
int bible_get_daily_chapters(
    int day_of_year,
    char out_books[][64],
    int out_chapters[],
    int max_entries
);

// Get Z coordinate for a specific day's reading (1-365)
// Returns the Z coordinate where that day's reading starts
// Used by /daily command to teleport to today's reading
int bible_get_daily_z_offset(int day_of_year);

// VALIDATION FUNCTIONS
// Get the number of chapters in a book
// Returns chapter count, or 0 if book not found
int bible_get_chapter_count(const char *book);

// Get the number of verses in a specific chapter
// Returns verse count, or 0 if book/chapter not found
int bible_get_verse_count(const char *book, int chapter);

#endif
