# /bgoto Validation and Error Messages

## Overview

The `/bgoto` command now validates chapter and verse references before teleporting. If you request an invalid chapter or verse, you'll get a helpful error message explaining the valid range.

## Error Types

### 1. Invalid Chapter

**Scenario:** You request a chapter that doesn't exist in the book.

**Examples:**

```bash
> /bgoto 2 John 3
ERROR: 2 John has only 1 chapter (you requested chapter 3)

> /bgoto Philemon 2
ERROR: Philemon has only 1 chapter (you requested chapter 2)

> /bgoto Genesis 51
ERROR: Genesis has only 50 chapters (you requested chapter 51)

> /bgoto Psalm 151
ERROR: Psalm has only 150 chapters (you requested chapter 151)
```

### 2. Invalid Verse

**Scenario:** You request a verse that doesn't exist in the chapter.

**Examples:**

```bash
> /bgoto 2 John 1:14
ERROR: 2 John 1 has only 13 verses (you requested verse 14)

> /bgoto 3 John 1:15
ERROR: 3 John 1 has only 14 verses (you requested verse 15)

> /bgoto Genesis 1:32
ERROR: Genesis 1 has only 31 verses (you requested verse 32)

> /bgoto John 3:37
ERROR: John 3 has only 36 verses (you requested verse 37)
```

### 3. Book Not Found

**Scenario:** You misspell a book name or use an invalid name.

**Example:**

```bash
> /bgoto Genisis 1
Book not found! Use /bgoto to see all books.

> /bgoto Revelations 22
Book not found! Use /bgoto to see all books.
# (Note: It's "Revelation" not "Revelations")
```

## How Validation Works

### Chapter Validation

1. Parse the book name from your command
2. Look up chapter count in the book database
3. Compare requested chapter against actual count
4. Show error if chapter > chapter_count

**Implementation:**
```c
int chapter_count = bible_get_chapter_count("2 John");  // Returns 1
if (chapter > chapter_count) {
    // Show error message
}
```

### Verse Validation

1. Parse book and chapter from your command
2. Scan Bible text file to count verses in that chapter
3. Compare requested verse against actual count
4. Show error if verse > verse_count

**Implementation:**
```c
int verse_count = bible_get_verse_count("2 John", 1);  // Returns 13
if (verse > verse_count) {
    // Show error message
}
```

## Book Chapter Counts

Quick reference for books with only 1 chapter:

| Book | Chapters | Common Mistakes |
|------|----------|----------------|
| Obadiah | 1 | Don't use "Obadiah 2" |
| Philemon | 1 | Don't use "Philemon 2" |
| 2 John | 1 | Don't use "2 John 2" or "2 John 3" |
| 3 John | 1 | Don't use "3 John 2" |
| Jude | 1 | Don't use "Jude 2" |

## Verse Count Examples

Some chapters with limited verses:

| Reference | Verses | Notes |
|-----------|--------|-------|
| 2 John 1 | 13 | Single chapter with 13 verses |
| 3 John 1 | 14 | Single chapter with 14 verses |
| Jude 1 | 25 | Single chapter with 25 verses |
| Psalm 117 | 2 | Shortest chapter in Bible |
| Psalm 119 | 176 | Longest chapter in Bible |

## Error Message Format

### Chapter Errors

**Singular (1 chapter):**
```
ERROR: [Book] has only 1 chapter (you requested chapter [N])
```

**Plural (multiple chapters):**
```
ERROR: [Book] has only [N] chapters (you requested chapter [M])
```

### Verse Errors

**Singular (1 verse):**
```
ERROR: [Book] [Chapter] has only 1 verse (you requested verse [N])
```

**Plural (multiple verses):**
```
ERROR: [Book] [Chapter] has only [N] verses (you requested verse [M])
```

## Testing Examples

### Valid Commands (Should Work)

```bash
/bgoto 2 John          # Valid - goes to the one chapter
/bgoto 2 John 1        # Valid - explicit chapter 1
/bgoto 2 John 1:1      # Valid - first verse
/bgoto 2 John 1:13     # Valid - last verse
/bgoto Psalm 119       # Valid - longest chapter
/bgoto Genesis 50      # Valid - last chapter
```

### Invalid Commands (Should Error)

```bash
/bgoto 2 John 2        # ERROR: only 1 chapter
/bgoto 2 John 1:14     # ERROR: only 13 verses
/bgoto Psalm 151       # ERROR: only 150 chapters
/bgoto Genesis 51      # ERROR: only 50 chapters
/bgoto John 3:37       # ERROR: John 3 has 36 verses
/bgoto Revelation 23   # ERROR: only 22 chapters
```

## Why This Matters

### Before Validation

Without validation, invalid references would either:
- Teleport you to wrong location
- Show confusing "not found" messages
- Not explain why the command failed

### After Validation

With validation, you get:
- **Clear error messages** explaining the problem
- **Exact counts** showing valid range
- **Immediate feedback** without teleporting
- **Educational info** about Bible structure

## Implementation Details

### bible_get_chapter_count()

**Location:** `Craft/src/bible.c`

**Purpose:** Returns number of chapters in a book

**How it works:**
- Searches through `bible_books[]` array
- Case-insensitive book name matching
- Returns chapter count or 0 if not found

**Performance:** O(1) - very fast (array lookup)

### bible_get_verse_count()

**Location:** `Craft/src/bible.c`

**Purpose:** Returns number of verses in a chapter

**How it works:**
- Opens Bible text file (`../Bible/kjv.txt`)
- Scans for lines matching "Book Chapter:*"
- Tracks highest verse number found
- Returns max verse number

**Performance:** O(n) where n = lines in Bible file
- First call per chapter: ~100-200ms (file scan)
- Could be cached for better performance

### Validation Flow

```
User: /bgoto 2 John 3
        ↓
Parse: book="2 John", chapter=3, verse=0
        ↓
Validate Chapter:
    chapter_count = bible_get_chapter_count("2 John")  // Returns 1
    if (3 > 1) → ERROR!
        ↓
Display: "ERROR: 2 John has only 1 chapter (you requested chapter 3)"
        ↓
Return (no teleport)
```

## Edge Cases Handled

### Single Chapter Books

Books with only 1 chapter use singular grammar:
- "has only 1 chapter" (not "1 chapters")
- Applies to: Obadiah, Philemon, 2 John, 3 John, Jude

### Single Verse Chapters

Chapters with only 1 verse use singular grammar:
- "has only 1 verse" (not "1 verses")
- Example: Psalm 117 (though rare)

### Book Only (No Chapter/Verse)

Valid commands like `/bgoto Genesis`:
- No validation needed
- Takes you to book start (Chapter 1:1)

### Chapter Only (No Verse)

Commands like `/bgoto Genesis 1`:
- Only validates chapter
- Takes you to chapter start (verse 1)

## Future Enhancements

Possible improvements:

1. **Verse count caching**
   - Cache verse counts in memory
   - Avoid re-scanning file for same chapter
   - Faster validation

2. **Helpful suggestions**
   - "Did you mean Genesis instead of Genisis?"
   - "Try John 3:16 instead of John 3:37"

3. **Range validation**
   - For verse ranges like "John 3:16-18"
   - Validate both start and end verses

4. **Book abbreviations**
   - Accept "Gen" for "Genesis"
   - Accept "1 Thess" for "1 Thessalonians"

## Troubleshooting

### "ERROR: has only X chapters"

**Problem:** You requested a chapter that doesn't exist

**Solution:** Use a lower chapter number (1 to X)

**Example:**
```bash
# Instead of:
/bgoto 2 John 3

# Use:
/bgoto 2 John 1
# or just:
/bgoto 2 John
```

### "ERROR: has only X verses"

**Problem:** You requested a verse that doesn't exist

**Solution:** Use a lower verse number (1 to X)

**Example:**
```bash
# Instead of:
/bgoto 2 John 1:14

# Use:
/bgoto 2 John 1:13
# (13 is the last verse)
```

### Validation Seems Wrong

**Problem:** Error message doesn't match your Bible

**Possible causes:**
- Using different translation (this is KJV)
- Verse numbering differs between translations
- Typo in book name

**Solution:**
- Verify you're using KJV numbering
- Check book spelling carefully
- Type `/bgoto` to see all book names

## Summary

The validation system:
- ✅ Prevents invalid teleports
- ✅ Shows helpful error messages
- ✅ Explains valid ranges
- ✅ Uses proper grammar (singular/plural)
- ✅ Validates both chapters and verses
- ✅ Works for all 66 Bible books

No more guessing if a reference is valid - you'll know immediately!
