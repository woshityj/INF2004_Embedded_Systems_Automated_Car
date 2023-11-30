#ifndef mapper
#define mapper

#define NORTH 0x1
#define SOUTH 0x2
#define EAST 0x3
#define WEST 0x4
#define DIFFERENCE(a, b) (a<b ? (b-a) : (a-b))
#define BUFFER_SIZE 32768
#define NORTH_WALL true
#define SOUTH_WALL true
#define EAST_WALL true
#define WEST_WALL true

typedef struct Coordinates{
    int x;      // vector x-axis
    int y;      // vector y-axis
    struct Cell *cellAddress;
} Coordinates;

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

    struct Coordinates vector;

    int score;
} Cell;

typedef struct Head{
    struct Cell *genesisCell;
} Head;

typedef struct node
{
    struct Cell *cellAddress;
    struct node *infront;
    struct node *behind;
    int scoreToSet;
} node;

typedef struct qHead
{
    struct node *end;
    struct node *front;
} qHead;

typedef struct {
    int currentlyFacing;
    bool north;
    bool south;
    bool east;
    bool west;
} Directions;


/*************Sets*************/

typedef struct Set{
  Coordinates *members;    // pointer to dynamically allocated array of the set members
  int length;      // the number of members of the set
} Set;

Set* init();
bool is_empty(Set *set);
bool is_member(Set *set, Coordinates inputVector);
void insert(Set *set, Coordinates inputVector);
void print_set(Set *set);
Coordinates* getColsAndRows(Set *set);
void destroySet(Set *set);

/**********Mapper**********/
void printMap();
void smallMaze(Directions *dir);
void finalMaze(Directions *dir);
void assignWalls(Directions *dir, bool northWall, bool southWall, bool eastWall, bool westWall);
void movedForward(int currentlyFacing, Directions* neighbors);
void movedBackwards(int forwardFacing, Directions* neighbors);
Cell *initMaze(Directions* neighbors);
void movedNorth(int currentlyFacing, Directions* neighbors);
void movedSouth(int currentlyFacing, Directions* neighbors);
void movedEast(int currentlyFacing, Directions* neighbors);
void movedWest(int currentlyFacing, Directions* neighbors);
void generateNeighborCellsForNorth(Cell *currentCell);
void generateNeighborCellsForSouth(Cell *currentCell);
void generateNeighborCellsForEast(Cell *currentCell);
void generateNeighborCellsForWest(Cell *currentCell);
Cell** getMap(struct Set *set);
void destroyMaze(Cell *cellToDestroy);


void floodfill(Cell *endSource, Set *visited, int score);
void set_score(Cell *cell, Set *set, int score);
void shortestPath(Cell *startSource, Cell *endSource);
Cell* lookAtScores(Cell *currentSource);

#endif