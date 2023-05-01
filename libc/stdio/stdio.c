#include <stdio.h>

FILE _iob[OPEN_MAX] = {
    {0, NULL, NULL, _READ, 0},
    {0, NULL, NULL, _WRITE, 1},
    {0, NULL, NULL, _WRITE | _UNBUF, 2},
};
