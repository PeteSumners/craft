// Test utility to verify the 2026 daily reading plan
#include <stdio.h>
#include <string.h>

// Include the precalculated plan
#include "daily_plan_2026.h"

int main() {
    printf("===========================================\n");
    printf("2026 Daily Bible Reading Plan Test\n");
    printf("===========================================\n\n");

    // Test a few specific days
    int test_days[] = {1, 100, 200, 365};
    int num_tests = sizeof(test_days) / sizeof(test_days[0]);

    for (int i = 0; i < num_tests; i++) {
        int day = test_days[i];
        const DayPlan *plan = &daily_plan_2026[day - 1];

        printf("Day %d of 365:\n", day);
        for (int j = 0; j < plan->count; j++) {
            printf("  %d. %s %d\n", j + 1,
                   plan->readings[j].book,
                   plan->readings[j].chapter);
        }
        printf("\n");
    }

    // Verify totals
    int total_chapters = 0;
    for (int day = 0; day < 365; day++) {
        total_chapters += daily_plan_2026[day].count;
    }

    printf("===========================================\n");
    printf("Validation:\n");
    printf("  Total days: 365\n");
    printf("  Total chapters: %d (should be 1189)\n", total_chapters);
    printf("  First reading: %s %d\n",
           daily_plan_2026[0].readings[0].book,
           daily_plan_2026[0].readings[0].chapter);
    printf("  Last day count: %d\n", daily_plan_2026[364].count);
    printf("  Last reading: %s %d\n",
           daily_plan_2026[364].readings[daily_plan_2026[364].count - 1].book,
           daily_plan_2026[364].readings[daily_plan_2026[364].count - 1].chapter);
    printf("===========================================\n");

    if (total_chapters == 1189) {
        printf("SUCCESS: Plan covers all 1,189 chapters!\n");
        return 0;
    } else {
        printf("ERROR: Chapter count mismatch!\n");
        return 1;
    }
}
