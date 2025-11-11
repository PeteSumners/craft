#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "progressive_builder.h"

// Block entry in the queue
typedef struct {
    int x, y, z;
    int w;  // block type
} QueuedBlock;

// Queue state
static struct {
    QueuedBlock *blocks;
    int capacity;
    int size;
    int read_index;  // Next block to place
    int blocks_per_frame;
    void (*block_func)(int x, int y, int z, int w);
    void (*completion_func)(void);  // Called when all blocks are placed
    int active;
} queue = {
    .blocks = NULL,
    .capacity = 0,
    .size = 0,
    .read_index = 0,
    .blocks_per_frame = 50,
    .block_func = NULL,
    .completion_func = NULL,
    .active = 0
};

void progressive_builder_init(void) {
    // Start with capacity for 100K blocks
    queue.capacity = 100000;
    queue.blocks = (QueuedBlock *)malloc(queue.capacity * sizeof(QueuedBlock));
    queue.size = 0;
    queue.read_index = 0;
    queue.blocks_per_frame = 50;
    queue.block_func = NULL;
    queue.active = 0;

    printf("Progressive builder initialized (capacity: %d blocks)\n", queue.capacity);
}

void progressive_builder_cleanup(void) {
    if (queue.blocks) {
        free(queue.blocks);
        queue.blocks = NULL;
    }
    queue.capacity = 0;
    queue.size = 0;
    queue.read_index = 0;
    queue.active = 0;
    queue.completion_func = NULL;
}

void progressive_builder_queue_block(int x, int y, int z, int w) {
    // Expand capacity if needed
    if (queue.size >= queue.capacity) {
        int new_capacity = queue.capacity * 2;
        QueuedBlock *new_blocks = (QueuedBlock *)realloc(queue.blocks, new_capacity * sizeof(QueuedBlock));
        if (!new_blocks) {
            fprintf(stderr, "ERROR: Failed to expand progressive builder queue!\n");
            return;
        }
        queue.blocks = new_blocks;
        queue.capacity = new_capacity;
        printf("Progressive builder queue expanded to %d blocks\n", new_capacity);
    }

    // Add block to queue
    queue.blocks[queue.size].x = x;
    queue.blocks[queue.size].y = y;
    queue.blocks[queue.size].z = z;
    queue.blocks[queue.size].w = w;
    queue.size++;
}

void progressive_builder_set_rate(int blocks_per_frame) {
    if (blocks_per_frame > 0) {
        queue.blocks_per_frame = blocks_per_frame;
    }
}

int progressive_builder_update(void) {
    if (!queue.active || !queue.block_func) {
        return 0;
    }

    int blocks_placed = 0;
    int blocks_to_place = queue.blocks_per_frame;

    while (blocks_to_place > 0 && queue.read_index < queue.size) {
        QueuedBlock *block = &queue.blocks[queue.read_index];
        queue.block_func(block->x, block->y, block->z, block->w);
        queue.read_index++;
        blocks_placed++;
        blocks_to_place--;
    }

    // Check if we're done
    if (queue.read_index >= queue.size) {
        printf("Progressive builder complete: %d blocks placed\n", queue.size);

        // Call completion callback if set
        if (queue.completion_func) {
            printf("Calling completion callback...\n");
            queue.completion_func();
            queue.completion_func = NULL;  // Clear after calling
        }

        queue.active = 0;
        queue.size = 0;
        queue.read_index = 0;
    }

    return blocks_placed;
}

int progressive_builder_is_active(void) {
    return queue.active && queue.read_index < queue.size;
}

int progressive_builder_get_queue_size(void) {
    return queue.size - queue.read_index;
}

void progressive_builder_clear_queue(void) {
    queue.size = 0;
    queue.read_index = 0;
    queue.active = 0;
    queue.completion_func = NULL;
}

void progressive_builder_start_session(void (*block_func)(int x, int y, int z, int w)) {
    queue.block_func = block_func;
    queue.size = 0;
    queue.read_index = 0;
    queue.active = 1;
    queue.completion_func = NULL;  // Clear any previous callback
    printf("Progressive builder session started\n");
}

void progressive_builder_end_session(void) {
    if (queue.active) {
        printf("Progressive builder session ending: %d blocks queued, starting placement\n", queue.size);
        // Keep active flag on so update() will process
    }
}

void progressive_builder_set_completion_callback(void (*completion_func)(void)) {
    queue.completion_func = completion_func;
    printf("Progressive builder completion callback set\n");
}
