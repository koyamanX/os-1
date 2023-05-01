#include <lib.h>
#include <libgen.h>

char *basename(char *path) {
    int i;

    if ((path == NULL) || (*path == '\0')) {
        return ".";
    }
    if ((*path == '/') && (*(path + 1) == '\0')) {
        return "/";
    }
    if ((*path == '/') && (*(path + 1) == '/') && (*(path + 2) == '\0')) {
        return "/";
    }

    i = strlen(path) - 1;
    while (path[i] == '/' && i != 0) {
        path[i] = '\0';
        i--;
    }
    while (path[i] != '/' && i != 0) {
        i--;
    }
    if (path[i] == '/') {
        i++;
    }
    return &path[i];
}
