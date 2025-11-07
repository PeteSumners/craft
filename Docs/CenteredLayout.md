# Centered Layout Design - Info Area

## Overview

The info area text is now **centered at world origin (0, 0, 0)**, providing a more balanced and intuitive layout.

## Layout Calculation

### Text Width Calculation

**Longest line in info area:**
```
"W/A/S/D - Move (horizontal in fly mode)"
```

**Character count:** 41 characters

**Voxel text rendering:**
- Each character: 8 pixels wide (Unifont)
- Scale factor: 2 (each pixel becomes 2 blocks)
- Blocks per character: 8 × 2 = **16 blocks**

**Total width:**
```
41 characters × 16 blocks/character = 656 blocks
```

### Centering Offset

To center 656-block-wide text at X=0:

```
Half width = 656 ÷ 2 = 328 blocks

Start position = 0 - 328 = -328
End position = -328 + 656 = 328

Center = 0 ✓
```

## Position Details

### Text Rendering

**Start position:** X=-328, Y=75, Z=0

**Text extent:**
- X: **-328 to +328** (656 blocks wide)
- Y: **75** (ground level)
- Z: **0 to ~220** (multiple lines)

**Center position:** X=0, Y=75, Z=~110

### Platform

**Glass viewing platform:**
- Position: X=0, Y=177, Z=0
- Size: 15×15 blocks (X: -7 to +7, Z: -7 to +7)
- Material: Glass (block type 10)
- Purpose: Landing spot for spawn and teleports

**No ground platform:**
- Previously had stone platform at Y=74
- Removed as unnecessary (text doesn't need physical support)

### Spawn & Teleport

**Spawn:**
- Position: (0, 187, 0)
- Falls to: (0, 177, 0) - lands on glass platform
- Looking: -45° down

**Teleport (/bgoto):**
- Position: (0, 177, 0)
- On platform: Yes (glass)
- Looking: -45° down

## Visual Layout

### Top-Down View (Y=177)

```
              West ←                    → East

        X=-328              X=0              X=328
          ↓                  ↓                 ↓

    Text: ████████████████████████████████████████
          [← 328 blocks →][← 328 blocks →]
                         ↑
                    Center/Origin

Platform:              ╔═══╗
                       ║ G ║  Glass (15×15)
                       ╚═══╝
```

### Side View (X=0, looking north)

```
Y=187     👤 Player spawns here
          ↓
Y=177     ▓▓▓ Glass platform (you stand here)


          (102 blocks of air - see through)


Y=75      ███ Help text (voxel blocks)
```

### 3D Perspective

```
                  North (-Z)
                      ↑
                      |

         -328 ←─────[0]─────→ +328
                      ↓
                  South (+Z)

           Text centered at origin
        Platform centered at origin
       Teleport position at origin
```

## Code Implementation

### Text Offset Calculation

```c
// Calculate centering
int longest_line_chars = 41;                        // Measured from content
int blocks_per_char = 16;                          // 8 pixels × scale 2
int text_width = longest_line_chars * blocks_per_char;  // 656 blocks
int text_start_x = 0 - (text_width / 2);          // -328

// Render text at offset position
voxel_text_render_flat("text", text_start_x, y, z, ...);
```

### Platform Generation

```c
// Platform centered at origin
int info_center_x = 0;
int viewing_altitude = 177;

for (int x = info_center_x - 7; x <= info_center_x + 7; x++) {
    for (int z = -7; z <= 7; z++) {
        block_func(x, viewing_altitude, z, 10);  // GLASS
    }
}
```

### Database Save

```c
// Save center position (not text start)
db_insert_bible_position("INFO", -1, 0,
                         0,     // X=0 (center)
                         75,    // Y=75 (text level)
                         0);    // Z=0
```

## Comparison

### Before (Left-Aligned)

```
Origin
  ↓
  0                             656
  █████████████████████████████
  Text starts at origin

  Teleport: X=0 (left edge)
```

### After (Centered)

```
        Origin
           ↓
  -328     0     328
  ██████████████████
    Text centered

  Teleport: X=0 (center)
```

## Benefits

### 1. Meaningful Origin

World origin (0, 0, 0) is now the **center** of the info area:
- Mathematically clean
- Intuitively balanced
- Easy to remember

### 2. Better Teleport Position

When you `/bgoto`, you land in the **middle** of the text:
- Not at the edge
- Can see text in all directions
- More balanced view

### 3. Simplified Platform

Only one platform needed:
- Glass at Y=177 (viewing altitude)
- No ground platform clutter
- Cleaner design

### 4. Visual Balance

Text extends equally in both directions:
- 328 blocks left of origin
- 328 blocks right of origin
- Symmetrical layout

## Coordinate Examples

### Info Area

```bash
> /bgoto
Teleported to info area: (0, 177, 0)
```

- **X=0:** Center of text (not left edge!)
- **Y=177:** Viewing altitude (102 above text)
- **Z=0:** Start of first line

### Walking Around

From teleport position (0, 177, 0):

- **Strafe left (A):** See left half of text (X: -328 to 0)
- **Strafe right (D):** See right half of text (X: 0 to 328)
- **Move forward (W):** Progress through lines (Z: 0 to 220)

## Implementation Notes

### Text Line Measurements

Actual line lengths (characters):
1. "=== BIBLE WORLD NAVIGATION ===" - 31 chars
2. "/bgoto - Return to this info area" - 34 chars
3. "W/A/S/D - Move (horizontal in fly mode)" - **41 chars** ← Used for centering
4. "- Genesis is 1000 blocks to the east ->" - **41 chars**

Using the longest line ensures all text fits within the centered boundaries.

### Block Calculation

```
Character pixels: 8
Scale factor: 2
Blocks per pixel: 1
Blocks per character: 8 × 2 × 1 = 16

Example:
"Hello" = 5 chars × 16 blocks = 80 blocks wide
```

### Platform Size

```
Platform: 15×15 blocks
Centered at (0, 177, 0)

X range: -7 to +7 (15 blocks)
Z range: -7 to +7 (15 blocks)
Total: 225 blocks
```

## Testing

### Visual Verification

After regenerating world (delete `craft.db`):

1. **Spawn** - Should land at (0, 187, 0)
2. **Fall** - Should land on glass at (0, 177, 0)
3. **Look down** - Should see text extending left and right equally
4. **Platform** - Should be centered under your feet
5. **Text** - Should be visible in both directions (±X)

### Coordinate Check

```bash
> /bgoto
Position: (0, 177, 0)
# You should be at origin, centered on text

# Strafe left
X decreases (seeing left text)

# Strafe right
X increases (seeing right text)
```

### Console Output

```
Step 1: Generating info area at world origin (0, 75, 0)...
  Creating viewing platform...
    Viewing platform: 225 blocks at Y=177, centered at X=0
  Rendering help message...
  Help message complete!
```

## Summary

The info area is now **perfectly centered at world origin**:

- **Text:** X=-328 to X=328 (center at X=0)
- **Platform:** Glass at (0, 177, 0)
- **Teleport:** Lands at (0, 177, 0) - center of everything
- **Origin:** Meaningful reference point

This creates a clean, balanced, and intuitive layout! 🎯
