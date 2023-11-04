#ifndef mapper
#define mapper

#define NORTH 0x1
#define SOUTH 0x2
#define EAST 0x3
#define WEST 0x4

#include "set.h"

typedef struct Cell{
    bool origin;
    bool ending;

    bool northWall;
    bool southWall;
    bool eastWall;
    bool westWall;

    // basic struct, for diagonals, must store 4 more for diagonal neighbors
    struct Cell *northNeighbor;
    struct Cell *southNeighbor;
    struct Cell *eastNeighbor;
    struct Cell *westNeighbor;

    Coordinates vector;

} Cell;

typedef struct Head{
    struct Cell *genesisCell;
} Head;

Cell* generateGenesis();
void movedForward(int currentlyFacing);
void movedBackwards(int forwardFacing);
Cell *initMaze();
void movedNorth(int currentlyFacing);
void movedSouth(int currentlyFacing);
void movedEast(int currentlyFacing);
void movedWest(int currentlyFacing);
void generateNeighborCellsForNorth(Cell *currentCell);
void generateNeighborCellsForSouth(Cell *currentCell);
void generateNeighborCellsForEast(Cell *currentCell);
void generateNeighborCellsForWest(Cell *currentCell);
Cell** getMap(struct Set *set);

#endif