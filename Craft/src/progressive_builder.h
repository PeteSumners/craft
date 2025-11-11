#ifndef _progressive_builder_h_
#define _progressive_builder_h_

// Progressive Block Placement System
// Allows blocks to be placed gradually over time with visual feedback
// Useful for large constructions like daily Bible reading generation

// Initialize the progressive builder system
void progressive_builder_init(void);

// Cleanup and free resources
void progressive_builder_cleanup(void);

// Queue a block to be placed progressively
// x, y, z: block coordinates
// w: block type
void progressive_builder_queue_block(int x, int y, int z, int w);

// Set the placement rate (blocks per frame)
// Default is 50 blocks per frame
void progressive_builder_set_rate(int blocks_per_frame);

// Update the builder (call this every frame from main loop)
// Places queued blocks at the configured rate
// Returns number of blocks placed this frame
int progressive_builder_update(void);

// Check if there are blocks still queued
int progressive_builder_is_active(void);

// Get number of blocks remaining in queue
int progressive_builder_get_queue_size(void);

// Clear all queued blocks (useful for canceling operations)
void progressive_builder_clear_queue(void);

// Start a new progressive build session with a callback
// This sets the actual block placement function
void progressive_builder_start_session(void (*block_func)(int x, int y, int z, int w));

// End the current session
void progressive_builder_end_session(void);

// Set a completion callback that will be called when all blocks are placed
void progressive_builder_set_completion_callback(void (*completion_func)(void));

#endif
