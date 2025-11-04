#ifndef _voxel_text_h_
#define _voxel_text_h_

// Voxel text rendering using GNU Unifont

// Initialize the voxel text system with path to unifont hex file
int voxel_text_init(const char *font_path);

// Clean up voxel text system
void voxel_text_cleanup(void);

// Render text as voxels starting at position (x, y, z)
// Returns the width of the rendered text in voxels
// block_type: the block type to use for rendering (1=grass, 3=stone, etc.)
// scale: scale factor (1=8x16 or 16x16, 2=16x32 or 32x32, etc.)
// block_func: function to place blocks (use builder_block for best results)
int voxel_text_render(
    const char *text,
    int x, int y, int z,
    int block_type,
    int scale,
    void (*block_func)(int x, int y, int z, int w)
);

// Render text flat/horizontal (readable from above)
// Text advances along X axis, lines advance along Z axis
// All text placed at same Y level (height)
// Perfect for rendering large amounts of text readable from above
// Returns the number of lines rendered
int voxel_text_render_flat(
    const char *text,
    int x, int y, int z,
    int block_type,
    int max_width,      // characters per line (0 = no wrap)
    int line_spacing,   // spacing between lines in voxels
    void (*block_func)(int x, int y, int z, int w)
);

#endif
