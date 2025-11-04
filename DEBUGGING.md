# Bible Rendering - Debugging Guide

## Issues Fixed

### Issue 1: Command Parsing Bug
**Problem**: The parser was stopping at the first digit (e.g., the "1" in "Genesis 1:1") instead of parsing the full book/chapter/verse before finding coordinates.

**Fix**: Changed parsing logic to find the colon (`:`) first, then skip past the verse numbers to find the X Y Z coordinates.

### Issue 2: Missing Include
**Problem**: `main.c` used `isdigit()` and `isspace()` without including `<ctype.h>`.

**Fix**: Added `#include <ctype.h>` to main.c.

## Testing the Bible Commands

### Quick Test Commands

Open the game and try these commands in order:

#### 1. Test Single Verse (Genesis 1:1)
```
/bible Genesis 1:1 0 100 0 3 60
```

**What should happen:**
- Message: "Rendering Genesis 1:1 at (0,100,0)"
- Text should appear in the world at Y=100
- Message: "Rendered X lines"

**If it doesn't work:**
- Check console output for error messages
- Verify you're in offline mode (run without server)

#### 2. Test Verse Range (John 3:16-17)
```
/bible John 3:16-17 0 120 0 4 50
```

**What should happen:**
- Renders two verses in brick blocks
- Message shows "Rendering John 3:16-17"

#### 3. Test Chapter (Psalm 23)
```
/bible Psalm 23 0 150 0 3 55
```

**What should happen:**
- Warning: "Full chapters can be very large!"
- Renders all 6 verses of Psalm 23
- May take 5-10 seconds

#### 4. Test Multi-Word Book Name
```
/bible 1 Kings 8:27 0 100 0 3 60
```

## Checking Initialization

When you start the game, look for these messages in the console:

```
Voxel text system initialized with font: ../Fonts/unifont-17.0.03.hex
Bible system initialized with file: ../Bible/kjv.txt
```

**If you see:**
```
Warning: Could not initialize voxel text system
Warning: Could not initialize Bible system
```

Then the files aren't being found. Check:
1. Run from `Craft/` directory: `cd Craft && ./craft.exe`
2. Verify files exist:
   - `../Fonts/unifont-17.0.03.hex`
   - `../Bible/kjv.txt`

## File Paths

The game expects this structure:
```
craft/
├── Bible/
│   └── kjv.txt
├── Craft/              <- Run from HERE
│   ├── craft.exe
│   └── ...
└── Fonts/
    └── unifont-17.0.03.hex
```

## Common Issues

### "Verse not found"
**Cause**: Book name spelling or chapter/verse doesn't exist

**Fix**: Check exact book name (case-sensitive):
- "Genesis" not "genesis"
- "Psalm" not "Psalms"
- "1 Kings" with space, not "1Kings"

### "Bible system not initialized"
**Cause**: kjv.txt not found

**Fix**:
```bash
cd Craft
ls ../Bible/kjv.txt    # Should show the file
./craft.exe            # Run from Craft directory
```

### No text appears in world
**Possible causes:**
1. Text rendered far away (wrong Y coordinate)
2. Text rendered below ground (Y too low)
3. Wrong camera position

**Fix**:
- Use Y=100-200 for visibility
- Use `/goto` to teleport if needed
- Try flying with Tab key

### Text appears but is garbled
**Cause**: Wrong file encoding

**Fix**: Ensure kjv.txt is UTF-8 encoded with tabs (not spaces)

## Manual Testing

### Test 1: Check if voxel text works
```
/vtext 0 100 0 3 TEST
```

If this works, voxel text is fine.

### Test 2: Check if Bible file loads
Look for this message at startup:
```
Bible system initialized with file: ../Bible/kjv.txt
```

### Test 3: Try simplest Bible command
```
/bible John 1:1 0 100 0 3 40
```

## Debug Mode

To enable debug output:

1. Edit `Craft/src/config.h`:
```c
#define DEBUG 1
```

2. Rebuild:
```bash
cd Craft
cmake .
mingw32-make
```

3. Run and check console for detailed messages

## Verbose Testing

Add debug messages to bible.c:

```c
// In bible_render_verse():
printf("DEBUG: Rendering %s %d:%d\n", book, chapter, verse);
printf("DEBUG: Position (%d, %d, %d), block=%d\n", x, y, z, block_type);
```

Rebuild and test.

## Success Indicators

When working correctly, you should see:

1. **At startup**: Initialization messages
2. **After command**: "Rendering..." messages in chat
3. **In world**: Voxel text appearing at coordinates
4. **After rendering**: "Rendered X lines" message

## If Still Not Working

1. **Verify build is current**:
```bash
cd Craft
mingw32-make clean
cmake .
mingw32-make
ls -lh craft.exe  # Should show recent timestamp
```

2. **Check file locations**:
```bash
pwd  # Should be in Craft/
ls ../Bible/kjv.txt
ls ../Fonts/unifont-17.0.03.hex
```

3. **Run with output redirect**:
```bash
./craft.exe 2>&1 | tee output.log
# Try commands, then Ctrl+C
# Check output.log for errors
```

4. **Test basic features**:
   - Can you place blocks? (right-click)
   - Can you type in chat? (T key)
   - Do other commands work? (/goto, /list)

## Getting Help

If the issue persists, provide:
1. Console output at startup
2. Error messages when running /bible command
3. Output of: `ls -la ../Bible/kjv.txt`
4. Output of: `ls -la ../Fonts/unifont-17.0.03.hex`
5. Which directory you're running from
