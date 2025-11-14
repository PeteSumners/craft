// Simple utility to extract today's daily Bible reading plan
// Outputs plaintext suitable for LinkedIn posting

#include <stdio.h>
#include <time.h>
#include <string.h>

typedef struct {
    const char *name;
    int chapters;
} BibleBook;

// All 66 books of the Bible in order (must match bible.c)
static const BibleBook bible_books[] = {
    // Old Testament (39 books)
    {"Genesis", 50}, {"Exodus", 40}, {"Leviticus", 27}, {"Numbers", 36}, {"Deuteronomy", 34},
    {"Joshua", 24}, {"Judges", 21}, {"Ruth", 4}, {"1 Samuel", 31}, {"2 Samuel", 24},
    {"1 Kings", 22}, {"2 Kings", 25}, {"1 Chronicles", 29}, {"2 Chronicles", 36}, {"Ezra", 10},
    {"Nehemiah", 13}, {"Esther", 10}, {"Job", 42}, {"Psalm", 150}, {"Proverbs", 31},
    {"Ecclesiastes", 12}, {"Song of Solomon", 8}, {"Isaiah", 66}, {"Jeremiah", 52}, {"Lamentations", 5},
    {"Ezekiel", 48}, {"Daniel", 12}, {"Hosea", 14}, {"Joel", 3}, {"Amos", 9},
    {"Obadiah", 1}, {"Jonah", 4}, {"Micah", 7}, {"Nahum", 3}, {"Habakkuk", 3},
    {"Zephaniah", 3}, {"Haggai", 2}, {"Zechariah", 14}, {"Malachi", 4},
    // New Testament (27 books)
    {"Matthew", 28}, {"Mark", 16}, {"Luke", 24}, {"John", 21}, {"Acts", 28},
    {"Romans", 16}, {"1 Corinthians", 16}, {"2 Corinthians", 13}, {"Galatians", 6}, {"Ephesians", 6},
    {"Philippians", 4}, {"Colossians", 4}, {"1 Thessalonians", 5}, {"2 Thessalonians", 3}, {"1 Timothy", 6},
    {"2 Timothy", 4}, {"Titus", 3}, {"Philemon", 1}, {"Hebrews", 13}, {"James", 5},
    {"1 Peter", 5}, {"2 Peter", 3}, {"1 John", 5}, {"2 John", 1}, {"3 John", 1},
    {"Jude", 1}, {"Revelation", 22}
};

#define BIBLE_BOOK_COUNT 66

static int get_day_of_year() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    return t->tm_yday + 1; // tm_yday is 0-based
}

static void get_current_date(char *buffer, size_t buffer_size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(buffer, buffer_size, "%04d-%02d-%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
}

static int chapter_index_to_book_chapter(int chapter_idx, char *out_book, int *out_chapter) {
    int cumulative = 0;

    for (int i = 0; i < BIBLE_BOOK_COUNT; i++) {
        int book_chapters = bible_books[i].chapters;
        if (chapter_idx < cumulative + book_chapters) {
            strcpy(out_book, bible_books[i].name);
            *out_chapter = (chapter_idx - cumulative) + 1;
            return 1;
        }
        cumulative += book_chapters;
    }
    return 0;
}

int main() {
    const int total_chapters = 1189;
    const int total_days = 365;

    int day_of_year = get_day_of_year();
    char today_date[32];
    get_current_date(today_date, sizeof(today_date));

    // Calculate which chapters to read today
    int start_chapter_idx = ((day_of_year - 1) * total_chapters) / total_days;
    int end_chapter_idx = (day_of_year * total_chapters) / total_days - 1;

    printf("===========================================\n");
    printf("Daily Bible Reading Plan\n");
    printf("===========================================\n");
    printf("Date: %s\n", today_date);
    printf("Day %d of 365\n", day_of_year);
    printf("\n");
    printf("Today's Reading:\n");
    printf("-------------------------------------------\n");

    // Convert to book/chapter pairs
    for (int idx = start_chapter_idx; idx <= end_chapter_idx; idx++) {
        char book[64];
        int chapter;

        if (chapter_index_to_book_chapter(idx, book, &chapter)) {
            printf("  %s %d\n", book, chapter);
        }
    }

    printf("===========================================\n");
    printf("\n");

    // LinkedIn-friendly format
    printf("LinkedIn Post Format:\n");
    printf("-------------------------------------------\n");
    printf("Daily Bible Reading - %s (Day %d/365)\n\n", today_date, day_of_year);

    for (int idx = start_chapter_idx; idx <= end_chapter_idx; idx++) {
        char book[64];
        int chapter;

        if (chapter_index_to_book_chapter(idx, book, &chapter)) {
            printf("%s %d\n", book, chapter);
        }
    }

    printf("\n#BibleReading #DailyReading #Scripture\n");
    printf("===========================================\n");

    return 0;
}
