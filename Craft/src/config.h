#ifndef _config_h_
#define _config_h_

// app parameters
#define DEBUG 0
#define FULLSCREEN 0
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define VSYNC 1
#define SCROLL_THRESHOLD 0.1
#define MAX_MESSAGES 20
#define DB_PATH "craft.db"
#define USE_CACHE 1
#define DAY_LENGTH 600
#define INVERT_MOUSE 0

// rendering options
#define SHOW_LIGHTS 1
#define SHOW_PLANTS 1
#define SHOW_CLOUDS 0               // Disabled for Bible viewing (clearer sky)
#define SHOW_TREES 1
#define SHOW_ITEM 1
#define SHOW_CROSSHAIRS 1
#define SHOW_WIREFRAME 1
#define SHOW_INFO_TEXT 1
#define SHOW_CHAT_TEXT 1
#define SHOW_PLAYER_NAMES 1
#define SHOW_FOG 0              // Set to 0 for Bible viewing (clearer text at distance)

// world generation options
#define FLATLANDS 1             // 1 = flat world for Bible viewing, 0 = normal terrain
#define FLATLANDS_HEIGHT 32     // Height of flat ground

// Bible world generation options
#define GENERATE_BIBLE 1                 // 1 = Generate Bible scrolls at world start, 0 = disabled
#define BIBLE_START_X 1000               // Starting X coordinate for first book (Genesis)
#define BIBLE_START_Y 75                 // Y level for Bible text - just above clouds
#define BIBLE_START_Z 0                  // Starting Z coordinate for Bible scrolls
#define BIBLE_BLOCK_TYPE 12              // Block type for Bible text (12 = LIGHT_STONE - bright white)
#define BIBLE_SPAWN_PLATFORM_Y 177       // Y level for spawn platform (102 blocks above text at Y=75)
#define BIBLE_SPAWN_Y 187                // Player spawn Y (10 blocks above platform, will drop down)
#define BIBLE_SPAWN_X 0                  // Spawn X coordinate (at world origin, info area)
#define BIBLE_SPAWN_Z 0                  // Spawn Z coordinate (at info area origin)

// key bindings
#define CRAFT_KEY_FORWARD 'W'
#define CRAFT_KEY_BACKWARD 'S'
#define CRAFT_KEY_LEFT 'A'
#define CRAFT_KEY_RIGHT 'D'
#define CRAFT_KEY_JUMP GLFW_KEY_SPACE
#define CRAFT_KEY_FLY GLFW_KEY_TAB
#define CRAFT_KEY_OBSERVE 'O'
#define CRAFT_KEY_OBSERVE_INSET 'P'
#define CRAFT_KEY_ITEM_NEXT 'E'
#define CRAFT_KEY_ITEM_PREV 'R'
#define CRAFT_KEY_ZOOM GLFW_KEY_LEFT_SHIFT
#define CRAFT_KEY_ORTHO 'F'
#define CRAFT_KEY_CHAT 't'
#define CRAFT_KEY_COMMAND '/'
#define CRAFT_KEY_SIGN '`'

// advanced parameters
#define CREATE_CHUNK_RADIUS 10   // Reduced for Bible viewing (320 block radius)
#define RENDER_CHUNK_RADIUS 8    // Reduced for Bible viewing (256 block radius)
#define RENDER_SIGN_RADIUS 4
#define DELETE_CHUNK_RADIUS 14   // Reduced for Bible viewing (448 block radius)
#define CHUNK_SIZE 32
#define COMMIT_INTERVAL 5

#endif
