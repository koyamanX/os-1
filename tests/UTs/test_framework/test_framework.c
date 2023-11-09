#include <test_framework.h>
#include <string.h>

int num_tests = 0;
test *tests[MAX_TESTS];

int run_tests(void) {
    printf("Running %d tests\n", num_tests);
    printf("========================================\n");
    printf("\n");

    int num_failed = 0;

    for (int i = 0; i < num_tests; i++) {
        test *t = tests[i];

        if (t == NULL) {
            break;
        }
        printf("----------------------------------------\n");
        printf("Running test `%s`\n", t->name);
        int result = t->function();
        if (result != 0) {
            printf("Test `%s` failed with code %d\n", t->name, result);
            printf("----------------------------------------\n");
            printf("\n");
            num_failed++;
        }
    }

    printf("========================================\n");
    printf("Ran %d tests\n", num_tests);
    printf("%d tests failed\n", num_failed);

    return num_failed;
}

void register_test(test *test) {
    tests[num_tests] = test;
    num_tests++;
}