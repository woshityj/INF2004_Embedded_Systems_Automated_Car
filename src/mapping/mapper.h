#ifndef mapper
#define mapper

#define NORTH 0x1
#define SOUTH 0x2
#define EAST 0x3
#define WEST 0x4

#define EVENT_NORTH 0x1
#define EVENT_SOUTH 0x2
#define EVENT_EAST 0x3
#define EVENT_WEST 0x4

#include "set.h"

typedef struct Cell{
    bool origin;
    bool ending;

    bool northWall;
    bool southWall;
    bool eastWall;
    bool westWall;

    // basic struct, for diagonals, must store 4 more for diagonal neighbors
    struct Cell *north;
    struct Cell *south;
    struct Cell *east;
    struct Cell *west;

    Coordinates vector;

} Cell;

typedef struct Head{
    struct Cell *genesisCell;
} Head;

Cell* generateGenesis();

#endif