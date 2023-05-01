#include <lib.h>
#include <libgen.h>

char *dirname(char *path) {
    int i;

    if ((path == NULL) || (*path == '\0')) {
        // NULL or ""
        return ".";
    }

    i = strlen(path) - 1;
    if ((path[i] == '/') && (i == 0)) {
        // "/"
        return "/";
    }
    i--;

    while (path[i] != '/') {
        if (i == 0) {
            // only filename
            return ".";
        }
        i--;
    }

    // filename, "/", NULL, "" are handled above,
    // so remaining char must be either
    // "/A/B/../"
    // or "//.."
    while (path[i] == '/') {
        // find '/'
        if (i == 0) {
            // if it is the last '/'
            return "/";
        }
        i--;
    }
    // last '/' found and also character remains
    path[i + 1] = '\0';

    return path;
}
