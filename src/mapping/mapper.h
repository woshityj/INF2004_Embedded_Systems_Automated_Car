#ifndef mapper
#define mapper

#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

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