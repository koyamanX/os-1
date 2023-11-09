#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <string.h>

typedef int (*test_function)(void);

typedef struct {
    char *name;
    test_function function;
} test;

void register_test(test *test);
int run_tests(void);

#define MAX_TESTS 100
extern test *tests[];
extern int num_tests;

#define TEST(name) \
    int name(void); \
    test name##_test = {#name, name}; \
    void __attribute__((constructor)) register_##name(void) { \
        register_test(&name##_test); \
    } \
    int name(void)

#define TEST_PASSED() \
    return 0;

#define RUN_ALL_TESTS() \
    int num_failed = run_tests(); \
    return num_failed;

#define ASSERT_EQ(expected, actual) \
    if (expected != actual) { \
        printf("\tAssertion failed: %s != %s at %d\n", #expected, #actual, __LINE__); \
        printf("\tExpected: %x\n", expected); \
        printf("\tActual: %x\n", actual); \
        return 1; \
    }

#define ASSERT_NE(expected, actual) \
    if (expected == actual) { \
        printf("\tAssertion failed: %s == %s at %d\n", #expected, #actual, __LINE__); \
        printf("\tExpected: %x\n", expected); \
        printf("\tActual: %x\n", actual); \
        return 1; \
    }

#define ASSERT_TRUE(actual) \
    if (!actual) { \
        printf("\tAssertion failed: %s\n", #actual); \
        printf("\tExpected: true\n"); \
        printf("\tActual: false\n"); \
        return 1; \
    }

#define ASSERT_FALSE(actual) \
    if (actual) { \
        printf("\tAssertion failed: %s\n", #actual); \
        printf("\tExpected: false\n"); \
        printf("\tActual: true\n"); \
        return 1; \
    }

#define ASSERT_NULL(actual) \
    if (actual != NULL) { \
        printf("\tAssertion failed: %s\n", #actual); \
        printf("\tExpected: NULL\n"); \
        printf("\tActual: %p\n", actual); \
        return 1; \
    }

#define ASSERT_STR_EQ(expected, actual) \
    if (strcmp(expected, actual) != 0) { \
        printf("\tAssertion failed: %s != %s\n", #expected, #actual); \
        printf("\tExpected: %s\n", expected); \
        printf("\tActual: %s\n", actual); \
        return 1; \
    }

#define ASSERT_STR_NE(expected, actual) \
    if (strcmp(expected, actual) == 0) { \
        printf("\tAssertion failed: %s == %s\n", #expected, #actual); \
        printf("\tExpected: %s\n", expected); \
        printf("\tActual: %s\n", actual); \
        return 1; \
    }

#define ASSERT_STR_NULL(actual) \
    if (actual != NULL) { \
        printf("\tAssertion failed: %s\n", #actual); \
        printf("\tExpected: NULL\n"); \
        printf("\tActual: %p\n", actual); \
        return 1; \
    }

#define ASSERT_LE(expected, actual) \
    if (expected > actual) { \
        printf("\tAssertion failed: %s > %s\n", #expected, #actual); \
        printf("\tExpected: %d\n", expected); \
        printf("\tActual: %d\n", actual); \
        return 1; \
    }

#define ASSERT_LT(expected, actual) \
    if (expected >= actual) { \
        printf("\tAssertion failed: %s >= %s\n", #expected, #actual); \
        printf("\tExpected: %d\n", expected); \
        printf("\tActual: %d\n", actual); \
        return 1; \
    }

#define ASSERT_GE(expected, actual) \
    if (expected < actual) { \
        printf("\tAssertion failed: %s < %s\n", #expected, #actual); \
        printf("\tExpected: %d\n", expected); \
        printf("\tActual: %d\n", actual); \
        return 1; \
    }

#define ASSERT_GT(expected, actual) \
    if (expected <= actual) { \
        printf("\tAssertion failed: %s <= %s\n", #expected, #actual); \
        printf("\tExpected: %d\n", expected); \
        printf("\tActual: %d\n", actual); \
        return 1; \
    }
    
#endif