# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Chunky Craft** is a Minecraft-inspired voxel game written in C with OpenGL, featuring procedural terrain generation, multiplayer support, and a recently-added voxel text rendering system using GNU Unifont. The project is a fork/extension of Michael Fogleman's original Craft game with custom enhancements including multi-language text rendering capabilities.

## Build System & Commands

### Building the Project

The project uses CMake as its build system. All commands should be run from the `Craft/` directory:

```bash
cd Craft
cmake .
make
```

**Windows (MinGW):**
```bash
cd Craft
cmake -G "MinGW Makefiles"
mingw32-make
```

### Running the Game

```bash
# From Craft/ directory
./craft              # Offline mode
./craft HOST [PORT]  # Connect to multiplayer server
```

### Building the Multiplayer Server

The server requires compiling a shared library for terrain generation:

```bash
# From Craft/ directory
gcc -std=c99 -O3 -fPIC -shared -o world -I src -I deps/noise deps/noise/noise.c src/world.c
python server.py [HOST [PORT]]
```

### Important Build Notes

- The project uses C99 standard with `-O3` optimization
- SQLite database file (`craft.db`) stores world modifications
- Shader files in `Craft/shaders/` are loaded at runtime - do not move them
- Font files in `../Fonts/` directory must be accessible at runtime for voxel text

## Code Architecture

### Core System Structure

The game follows a traditional game loop architecture in `main.c`:

1. **Initialization** - OpenGL context, shader compilation, database, font loading
2. **Game Loop** - Input handling, world generation, rendering, networking
3. **Worker Threads** - Asynchronous chunk generation (4 workers by default)

### Key Subsystems

#### 1. World & Chunk Management

- World divided into 32×32 block chunks (XZ plane, Y is vertical)
- Chunks stored in array: `Chunk chunks[MAX_CHUNKS]` (MAX_CHUNKS = 8192)
- Each chunk contains:
  - `Map map` - block data (hash map: (x,y,z) → block_type)
  - `Map lights` - light data
  - `SignList signs` - text signs placed on blocks
  - OpenGL buffers for rendering

**Important functions:**
- `create_world(p, q, func, arg)` - Procedural terrain generation using Simplex noise
- Chunk loading/unloading based on player position and radius constants in `config.h`

#### 2. Rendering Pipeline

Modern OpenGL with shaders (no fixed-function pipeline):

- **Shaders** (in `Craft/shaders/`):
  - `block_vertex.glsl` / `block_fragment.glsl` - Main world rendering
  - `text_vertex.glsl` / `text_fragment.glsl` - 2D text overlay (HUD)
  - `sky_vertex.glsl` / `sky_fragment.glsl` - Sky dome
  - `line_vertex.glsl` / `line_fragment.glsl` - Wireframe/crosshairs

- **Optimization**: Only exposed faces rendered (not internal block faces)
- **Culling**: Frustum culling for chunks outside view
- **Transparency**: Glass/plants use fragment shader discard (magenta = transparent)
- **Ambient Occlusion**: Implemented per-vertex for realistic lighting

#### 3. Database (SQLite)

File: `src/db.c` / `src/db.h`

- Stores delta changes only (modifications from procedurally-generated world)
- Main table: `block(p, q, x, y, z, w)` where:
  - `(p, q)` = chunk coordinates
  - `(x, y, z)` = block position within world
  - `w` = block type (see `src/item.h` for all block types)
- Background thread handles writes to avoid blocking rendering
- Transaction commits every 5 seconds (`COMMIT_INTERVAL`)

#### 4. Voxel Text Rendering System

**NEW FEATURE** - Added support for rendering text as voxel blocks using GNU Unifont.

Files: `src/voxel_text.c` / `src/voxel_text.h`

Key API:
```c
int voxel_text_init(const char *font_path);  // Initialize with ../Fonts/unifont-17.0.03.hex
int voxel_text_render(const char *text, int x, int y, int z,
                      int block_type, int scale,
                      void (*block_func)(int x, int y, int z, int w));
void voxel_text_cleanup(void);
```

**How it works:**
- Loads GNU Unifont HEX file (58,000+ glyphs covering Unicode BMP)
- Parses bitmap font data: 8×16 pixels (Latin/Cyrillic) or 16×16 pixels (CJK)
- Converts each pixel to a voxel block in the world
- Text advances along X-axis, with Y as vertical

**In-game commands:**
- `/vtext X Y Z BLOCK_TYPE MESSAGE` - Render English text
- `/vtext-jp X Y Z BLOCK_TYPE` - Japanese demo
- `/vtext-cn X Y Z BLOCK_TYPE` - Chinese demo
- `/vtext-kr X Y Z BLOCK_TYPE` - Korean demo
- `/vtext-ru X Y Z BLOCK_TYPE` - Russian demo

#### 5. Multiplayer System

Files: `src/client.c` / `src/client.h` / `server.py`

- **Protocol**: Line-based ASCII over plain TCP sockets
- **Port**: 4080 (default)
- **Key commands**:
  - `C,p,q,key` - Request chunk data
  - `B,p,q,x,y,z,w` - Block update
  - `P,pid,x,y,z,rx,ry` - Player position
  - `K,p,q,key` - Cache key for chunk

Client interpolates player positions for smooth animation using `state1` and `state2` buffers.

#### 6. Input & Commands

In-game text commands parsed in `parse_command()` function in `main.c`:

- `/goto [NAME]` - Teleport to player
- `/list` - List players
- `/login NAME` / `/logout` - Authentication
- `/online HOST [PORT]` / `/offline [FILE]` - Mode switching
- `/pq P Q` - Teleport to chunk coordinates
- `/spawn` - Return to spawn
- `/vtext ...` - Voxel text rendering (see above)

### Key Data Structures

```c
// Chunk - represents 32×32×256 region
typedef struct {
    Map map;              // Block data (hash map)
    Map lights;           // Light values
    SignList signs;       // Text signs
    int p, q;             // Chunk coordinates
    GLuint buffer;        // OpenGL VBO
    int dirty;            // Needs regeneration
} Chunk;

// Player state
typedef struct {
    float x, y, z;        // Position
    float rx, ry;         // Rotation (yaw, pitch)
    float t;              // Timestamp for interpolation
} State;
```

### Important Constants (config.h)

Performance-critical values:
- `CREATE_CHUNK_RADIUS` (10) - Distance to generate chunks
- `RENDER_CHUNK_RADIUS` (10) - Distance to render chunks
- `DELETE_CHUNK_RADIUS` (14) - Distance before unloading chunks
- `CHUNK_SIZE` (32) - Blocks per chunk edge
- `MAX_CHUNKS` (8192) - Total chunk capacity
- `WORKERS` (4) - Background threads for chunk generation

Block types enumeration is in `src/item.h` (EMPTY=0, GRASS=1, STONE=3, etc.)

## Dependencies

All dependencies are vendored in `Craft/deps/`:

- **GLFW** (3.x) - Window/input management
- **GLEW** - OpenGL extension loading
- **SQLite3** - Database for world persistence
- **CURL** - HTTPS for authentication
- **lodepng** - PNG texture loading
- **noise** - Simplex/Perlin noise for terrain generation
- **tinycthread** - Cross-platform threading

External requirement: **CURL** must be installed system-wide (not vendored).

## Development Workflow

### Adding New Block Types

1. Add constant to `src/item.h` (follow existing numbering)
2. Add texture mapping in `src/item.c` in `blocks[]` array
3. Update `items[]` array if block should be selectable
4. Textures referenced by index in texture atlas (PNG files loaded in `main.c`)

### Modifying Shaders

Shaders are loaded from `Craft/shaders/` at runtime. After editing:
- No recompilation needed
- Restart the game to reload shaders
- Check for shader compilation errors in console output

### Working with the Database

When testing world changes:
- Database file: `Craft/craft.db`
- Delete file to reset world to procedural generation
- Use `/offline [FILE]` command to use different save files
- Schema is in `src/db.c` in `db_init()` function

### Testing Voxel Text

The voxel text system requires:
1. Font file accessible at `../Fonts/unifont-17.0.03.hex`
2. Initialized via `voxel_text_init()` in `main.c`
3. Use builder-style block placement function for proper chunk updates

Example usage in code:
```c
voxel_text_render("Hello", x, y, z, STONE, 1, builder_block);
```

The `builder_block` function (in `main.c`) ensures proper chunk invalidation and database updates.

## Common Gotchas

1. **Coordinate Systems**:
   - World: Y is up (0-255 valid range)
   - Chunks: (p, q) are chunk coordinates, not world coordinates
   - Conversion: chunk_p = floor(world_x / CHUNK_SIZE)

2. **Block Placement**:
   - Use `builder_block()` not direct map operations
   - This ensures chunk dirty flags are set and database is updated

3. **Shader Modifications**:
   - Attributes must match vertex buffer layout in `main.c`
   - Uniform names are hardcoded in C (search for `glGetUniformLocation`)

4. **Multithreading**:
   - Worker threads generate chunks asynchronously
   - Main thread handles all OpenGL calls
   - Database writes occur on separate thread (see `db_worker_run()`)

5. **Font Loading**:
   - HEX format: line format is `CODEPOINT:HEXDATA`
   - Width determined by data length: 32 chars = 8px, 64 chars = 16px
   - Handle UTF-8 encoding when parsing text input

## Future Development Notes

According to `TODO.md`, planned features include:
- Bible text rendering system (JSON input → voxel output)
- Enhanced documentation for voxel text API
- Performance optimizations for font loading
- Additional text layout options (scaling, rotation)

The project structure separates concerns well - keep game logic in `main.c`, rendering primitives in `cube.c`, world generation in `world.c`, and new features as separate modules like `voxel_text.c`.
