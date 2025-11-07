# Platform System - Info Area

## Overview

The info area at world origin (0, 0) now has **TWO platforms** at different heights to support the dual-level design.

## Platform Locations

### 1. Ground Platform (Y=74)

**Purpose:** Foundation under the help text

**Details:**
- **Position:** (0, 74, 0)
- **Size:** 15×15 blocks (225 blocks total)
- **Material:** Stone (block type 3)
- **Function:** Supports the help text at Y=75

**What it supports:**
- Help text rendering (Y=75)
- Visual base for the navigation info
- Ground-level reference point

### 2. Viewing Platform (Y=177)

**Purpose:** Landing platform for spawn and teleports

**Details:**
- **Position:** (0, 177, 0)
- **Size:** 15×15 blocks (225 blocks total)
- **Material:** Glass (block type 10)
- **Function:** Where you spawn and land when using `/bgoto`

**What it supports:**
- Player spawn at Y=187 (falls to Y=177)
- `/bgoto` teleports to Y=177
- Viewing altitude for looking down at text

## Height Relationship

```
Y=187: Player spawn point (falls down)
       ↓
Y=177: GLASS VIEWING PLATFORM ← You land here
       |
       | 102 blocks of air
       |
       ↓
Y=75:  Help text (voxel blocks)
       ↓
Y=74:  STONE GROUND PLATFORM ← Text sits on this
```

## Why Two Platforms?

### Before (Single Platform)

- Only had ground platform at Y=74
- Spawned at Y=187 with no platform to land on
- Would fall through air to ground
- Confusing experience

### After (Dual Platforms)

- Ground platform at Y=74 supports text
- Viewing platform at Y=177 for spawn/teleport
- You land on glass platform with text visible below
- Clear spatial organization

## Visual Design

### From Above (Top-Down View)

```
        15 blocks
    ←─────────────→

    ╔═════════════╗  ↑
    ║   GLASS     ║  │
    ║  PLATFORM   ║  │ 15 blocks
    ║   Y=177     ║  │
    ╚═════════════╝  ↓

         102 blocks down
              ↓

    ╔═════════════╗  ↑
    ║   STONE     ║  │
    ║  PLATFORM   ║  │ 15 blocks
    ║   Y=74      ║  │
    ╚═════════════╝  ↓
```

### Side View (Cross-Section)

```
Y=187  👤 Player spawns
       ↓
Y=177  ▓▓▓▓▓▓▓▓▓  Glass platform (transparent)


       (102 blocks of air - you can see through glass to text below)


Y=75   ████████  Help text (voxel blocks)
Y=74   ▓▓▓▓▓▓▓▓  Stone platform (solid)
```

## Generation Details

### Code Implementation

```c
// Ground platform (15×15) - under the help text
for (int x = info_x - 7; x <= info_x + 7; x++) {
    for (int z = info_z - 7; z <= info_z + 7; z++) {
        block_func(x, info_y - 1, z, 3); // STONE at Y=74
    }
}

// Viewing platform (15×15) - at teleport/spawn altitude
int viewing_altitude = info_y + 102; // Y=177
for (int x = info_x - 7; x <= info_x + 7; x++) {
    for (int z = info_z - 7; z <= info_z + 7; z++) {
        block_func(x, viewing_altitude, z, 10); // GLASS at Y=177
    }
}
```

### Generation Messages

```
Step 1: Generating info area at world origin (0, 75, 0)...
  Creating info platforms...
    Ground platform: 225 blocks at Y=74
    Viewing platform: 225 blocks at Y=177
  Info platforms complete!
  Rendering help message...
  Help message complete!
```

## Material Choices

### Ground Platform - Stone

- **Visibility:** Opaque and solid
- **Contrast:** Dark color stands out
- **Purpose:** Clear visual base for text
- **Block type:** 3 (STONE)

### Viewing Platform - Glass

- **Visibility:** Transparent (see through to text)
- **Safety:** Still solid (you can stand on it)
- **Aesthetics:** Clean, modern look
- **Block type:** 10 (GLASS)

**Why glass for viewing platform?**
- See through to the text below
- Know you're standing on something
- Don't obstruct the view
- Feel like you're floating

## Coordinates Reference

| Feature | X | Y | Z | Notes |
|---------|---|---|---|-------|
| Ground platform center | 0 | 74 | 0 | Under text |
| Ground platform range | -7 to +7 | 74 | -7 to +7 | 15×15 blocks |
| Help text start | 0 | 75 | 0 | One block above ground |
| Viewing platform center | 0 | 177 | 0 | Spawn/teleport level |
| Viewing platform range | -7 to +7 | 177 | -7 to +7 | 15×15 blocks |
| Player spawn | 0 | 187 | 0 | 10 blocks above platform |
| Teleport position | 0 | 177 | 0 | Right on platform |

## Player Experience

### On First Spawn

1. **Spawn at:** (0, 187, 0)
2. **Fall to:** (0, ~177, 0) - lands on glass platform
3. **See:** Help text 102 blocks below through glass
4. **Stand on:** Transparent glass platform

### Using /bgoto

1. **Teleport to:** (0, 177, 0)
2. **Stand on:** Glass platform (already at correct height)
3. **Look down:** See help text at Y=75
4. **Camera:** -45° pitch angle

### Benefits

- **Safe landing:** Always have platform under feet
- **Clear view:** Glass doesn't obstruct text visibility
- **Intuitive:** Understand height relationship instantly
- **Consistent:** Same experience for spawn and teleport

## Comparison to Book Locations

### Info Area (Special)

- Has **two platforms** (ground + viewing)
- Ground supports text
- Viewing supports player

### Book Locations (Genesis, Exodus, etc.)

- Only have **text at ground level** (Y=75)
- No viewing platforms (just teleport to air)
- Could add viewing platforms in future

**Future enhancement:** Add glass platforms at Y=177 for all books?

## Technical Notes

### Block Placement

- Platforms generated during world creation
- Uses same `block_func` as text rendering
- Saved to database automatically
- Persistent across game sessions

### Platform Size

- 15×15 blocks = 225 total blocks per platform
- 7 blocks in each direction from center
- Total coverage: 450 blocks (both platforms)

### Memory Impact

- Minimal - just 450 blocks
- Compared to entire Bible: negligible
- Worth it for improved UX

## Testing

### Verification Steps

1. Delete `craft.db` for fresh world
2. Run game and wait for generation
3. Check console for platform messages
4. Spawn - should land on glass at Y=177
5. Look down - should see text 102 blocks below
6. Look at feet - should see glass blocks
7. Type `/bgoto` - should teleport to same platform

### Expected Console Output

```
Step 1: Generating info area at world origin (0, 75, 0)...
  Creating info platforms...
    Ground platform: 225 blocks at Y=74
    Viewing platform: 225 blocks at Y=177
  Info platforms complete!
  Rendering help message...
  Help message complete!
```

### Visual Checklist

- [ ] Glass platform visible at Y=177
- [ ] Can walk on glass without falling
- [ ] Text visible through glass below
- [ ] Stone platform visible at Y=75 (from distance)
- [ ] Both platforms centered at (0, 0)
- [ ] Both platforms are 15×15 blocks

## Summary

The dual-platform system at the info area provides:
- **Ground platform (Y=74):** Supports help text
- **Viewing platform (Y=177):** Supports player

This creates a clear vertical relationship and ensures you always have a solid surface to land on while maintaining visibility of the text below through transparent glass.
