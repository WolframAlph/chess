#ifndef CCHESS_SEARCH_H
#define CCHESS_SEARCH_H

#include "const.h"

#define MAX_SEARCH_DEPTH 64
#define DEFAULT_SEARCH_DEPTH 8

typedef struct {
    int move_count;
    move moves[MAX_SEARCH_DEPTH];
} PvLine;

move search(int);

#endif //CCHESS_SEARCH_H
