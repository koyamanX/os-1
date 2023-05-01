#include <lib.h>

char *strtok(char *str, const char *delim) {
    static char *saveptr = NULL;
    char *bp;
    char *ret;

    if (str) {
        saveptr = str;
    }
    if (!saveptr || !*saveptr) {
        return NULL;
    }

    bp = saveptr;
    for (const char *c = delim; *c; c++) {
        if (*bp == *c) {
            bp++;
            saveptr = bp;
            break;
        }
    }

    while (*bp) {
        for (const char *c = delim; *c; c++) {
            if (*bp == *c) {
                *bp = '\0';
                ret = saveptr;
                saveptr = bp + 1;
                return ret;
            }
        }
        bp++;
    }
    ret = saveptr;
    saveptr = NULL;
    return ret;
}
