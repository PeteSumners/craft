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

#endif
