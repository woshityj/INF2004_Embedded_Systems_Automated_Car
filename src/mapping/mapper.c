#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include "../infrared/infrared.h"

#include "mapper.h"
//#include "set.h"

Set *allVectorSets;
Head startOfMaze;
Head endOfMaze;
Cell *currentCell;
Cell *previousCell;
Coordinates currentVector;

volatile int currentlyFacing = NORTH; // always assumes it starts off facing "north" which is calibrated to the magnetometer of relative 'north'

int main(void)
{
    printf("Hello world\n");
    Directions* dir = (Directions*)malloc(sizeof(Directions));
    // assignWalls(dir, north, south, east, west)
    assignWalls(dir, false, SOUTH_WALL, false, WEST_WALL);
    currentCell = initMaze(dir);

    currentlyFacing = EAST; // turn 90 degrees to the East
    assignWalls(dir, NORTH_WALL, SOUTH_WALL, false, false);
    movedForward(currentlyFacing, dir);

    currentlyFacing = WEST;
    movedForward(currentlyFacing, dir);

    assignWalls(dir, false, false, EAST_WALL, WEST_WALL);
    currentlyFacing = NORTH;
    movedForward(currentlyFacing, dir);

    currentlyFacing = SOUTH;
    movedForward(currentlyFacing, dir);

    Coordinates *rowsAndCols = getColsAndRows(allVectorSets);
    Coordinates dimensions = rowsAndCols[0];
    Coordinates lowestValues = rowsAndCols[1];
    int rows = dimensions.y;
    int cols = dimensions.x;
    int lowestX = lowestValues.x;
    int lowestY = lowestValues.y;
    printf("Rows: %d, Cols: %d\n", rows, cols);
    printf("Lowest x value: %d, Lowest y value: %d\n", lowestX, lowestY);
    Cell **maze = getMap(allVectorSets);
    Cell mazeCell;
    // This for loop represents the map from top left to bottom right as a 2D grid
    for(int i = rows - 1; i >= 0; i--)
    {
      for(int j = 0; j < cols; j++)
      {
        mazeCell = maze[i][j];
        if(mazeCell.origin == true)
        {
          printf("Hello I am the entrance\n");
        }
        else if(mazeCell.origin == false && mazeCell.vector.cellAddress != NULL)
        {
          printf("I am just a random cell\n");
        }
      }
    }
    print_set(allVectorSets);
}
// The only 2 functions that the movement logic needs to call everytime it does something
void movedForward(int currentlyFacing, Directions* neighbors)
{
  if(currentlyFacing == NORTH)
  {
      movedNorth(currentlyFacing, neighbors);
  }
  else if(currentlyFacing == SOUTH)
  {
      movedSouth(currentlyFacing, neighbors);    
  }
  else if(currentlyFacing == EAST)
  {
      movedEast(currentlyFacing, neighbors);
  }
  else
  {
      movedWest(currentlyFacing, neighbors);
  }
  printf("moved forward, facing %d\n", currentlyFacing);
  printf("Now at coord x:%d, y:%d\n", currentVector.x, currentVector.y);
}

// Only works if the maze is already mapped, otherwise will break
void movedBackwards(int forwardFacing, Directions* neighbors)
{
    switch(currentlyFacing)
    {
        case NORTH:
            movedSouth(forwardFacing, neighbors);
            // Set the currentlyFacing back to where it was actually at since it's moving backwards
            currentlyFacing = NORTH;
        case SOUTH:
            movedNorth(forwardFacing, neighbors);
            currentlyFacing = SOUTH;
        case EAST:
            movedWest(forwardFacing, neighbors);
            currentlyFacing = EAST;
        case WEST:
            movedEast(forwardFacing, neighbors);
            currentlyFacing = WEST;
    }
}

// API ends

void assignWalls(Directions *dir, bool northWall, bool southWall, bool eastWall, bool westWall)
{
    dir->north = northWall;
    dir->south = southWall;
    dir->east = eastWall;
    dir->west = westWall;
}

Cell *initMaze(Directions* neighbors)
{

    allVectorSets = init();

    startOfMaze.genesisCell = malloc(sizeof(Cell)); // returns the address of the starting cell

    currentVector.x = 0;
    currentVector.y = 0;
    currentVector.cellAddress = startOfMaze.genesisCell;
    // Puts x,y = 0 into the set to mark as a visited cell
    insert(allVectorSets, currentVector);
    // Generate the genesis cell
    previousCell = NULL;
    currentCell = startOfMaze.genesisCell;

    generateNeighborCellsForNorth(currentCell);

    currentlyFacing = NORTH;
    // Api to the infrared sensors
    // Directions *neighbors = get_directions(currentlyFacing);
    currentCell->origin = true;
    currentCell->ending = false;
    // Find out whether or not there are walls at these 4 directions
    currentCell->northWall = neighbors->north;
    currentCell->southWall = neighbors->south;
    currentCell->eastWall = neighbors->east;
    currentCell->westWall = neighbors->west;
    
    currentCell->vector.x = currentVector.x;
    currentCell->vector.y = currentVector.y;

    // For each of the directions, if there is a wall, just reclaim the memory there
    if(currentCell->northWall)
    {
        free(currentCell->northNeighbor);
        currentCell->northNeighbor = NULL;
    }
    if(currentCell->eastWall) // theres an east wall
    {
        free(currentCell->eastNeighbor);
        currentCell->eastNeighbor = NULL;
    }
    if(currentCell->westWall) // theres an west wall
    {
        free(currentCell->westNeighbor); // reclaim memory 
        currentCell->westNeighbor = NULL;
    }
    previousCell = currentCell;

    return currentCell; // returns the cell's address to the main callee
}

void movedNorth(int currentlyFacing, Directions* neighbors)
{   
    if(previousCell->northWall == true)
    {
        printf("Cannot go here!\n");
        return;
    }
    currentCell = previousCell->northNeighbor;
    currentlyFacing = NORTH;
    currentVector.x = previousCell->vector.x;
    currentVector.y = previousCell->vector.y + 1;
    currentVector.cellAddress = currentCell;

    if(!is_member(allVectorSets, currentVector)) // iff the currentVector position is not in the set
    {
        insert(allVectorSets, currentVector);

        generateNeighborCellsForNorth(currentCell);

        currentCell->origin = false;
        currentCell->ending = false;
        // Give this cell the coordinates associated with it
        currentCell->vector.x = currentVector.x;
        currentCell->vector.y = currentVector.y;
        currentCell->vector.cellAddress = currentCell;

        // The neighbors are all assumed to be moveable until proven otherwise

        // Query about its neighboring cells wall status
        // Directions *neighbors = get_directions(currentlyFacing);

        currentCell->northWall = neighbors->north;
        currentCell->southWall = neighbors->south;
        currentCell->eastWall = neighbors->east;
        currentCell->westWall = neighbors->west;

        // IFF all these conditions return true, then it is considered the end of the maze
        if(currentCell->northWall == false && currentCell->eastWall == false && currentCell->westWall == false)
        {
            currentCell->ending = true;
            currentCell->northWall = true;
            currentCell->eastWall = true;
            currentCell->westWall = true;

            free(currentCell->northNeighbor);
            currentCell->northNeighbor = NULL;
            free(currentCell->eastNeighbor);
            currentCell->eastNeighbor = NULL;
            free(currentCell->westNeighbor);
            currentCell->westNeighbor = NULL;

            endOfMaze.genesisCell = currentCell;
        }

        // For each of the directions, if there is a wall, just reclaim the memory there
        if(currentCell->eastWall)
        {
            free(currentCell->eastNeighbor);
            currentCell->eastNeighbor = NULL;
        }
        if(currentCell->westWall)
        {
            free(currentCell->westNeighbor);
            currentCell->westNeighbor = NULL;
        }
        if(currentCell->northWall)
        {
            free(currentCell->northNeighbor);
            currentCell->northNeighbor = NULL;
        }
    }
    previousCell = currentCell; // update the previous cell
}

void movedSouth(int currentlyFacing, Directions* neighbors)
{
    if(previousCell->southWall == true)
    {
        printf("Cannot go here!\n");
        return;
    }
    currentCell = previousCell->southNeighbor;
    currentlyFacing = SOUTH;
    currentVector.x = previousCell->vector.x;
    currentVector.y = previousCell->vector.y - 1;
    currentVector.cellAddress = currentCell;

    if(!is_member(allVectorSets, currentVector)) // iff the currentVector position is not in the set
    {
        insert(allVectorSets, currentVector);

        generateNeighborCellsForSouth(currentCell);

        currentCell->origin = false;
        currentCell->ending = false;
        // Give this cell the coordinates associated with it
        currentCell->vector.x = currentVector.x;
        currentCell->vector.y = currentVector.y;
        currentCell->vector.cellAddress = currentCell;

        // The neighbors are all assumed to be moveable until proven otherwise

        // Query about its neighboring cells wall status
        // Directions *neighbors = get_directions(currentlyFacing);

        currentCell->northWall = neighbors->north;
        currentCell->southWall = neighbors->south;
        currentCell->eastWall = neighbors->east;
        currentCell->westWall = neighbors->west;

        // IFF all these conditions return false, then it is considered the end of the maze
        if(currentCell->southWall == false && currentCell->eastWall == false && currentCell->westWall == false)
        {
            currentCell->ending = true;
            currentCell->southWall = true;
            currentCell->eastWall = true;
            currentCell->westWall = true;

            free(currentCell->southNeighbor);
            currentCell->southNeighbor = NULL;
            free(currentCell->eastNeighbor);
            currentCell->eastNeighbor = NULL;
            free(currentCell->westNeighbor);
            currentCell->westNeighbor = NULL;

            endOfMaze.genesisCell = currentCell;
        }

        // For each of the directions, if there is a wall, just reclaim the memory there
        if(currentCell->southWall)
        {
            free(currentCell->southNeighbor);
            currentCell->southNeighbor = NULL;
        }
        if(currentCell->eastWall)
        {
            free(currentCell->eastNeighbor);
            currentCell->eastNeighbor = NULL;
        }
        if(currentCell->westWall)
        {
            free(currentCell->westNeighbor);
            currentCell->westNeighbor = NULL;
        }
    }

    previousCell = currentCell; // update the previous cell
}

void movedEast(int currentlyFacing, Directions* neighbors)
{

    if(previousCell->eastWall == true)
    {
        printf("Cannot go here!\n");
        return;
    }
    currentCell = previousCell->eastNeighbor;
    //currentCell = malloc(sizeof(Cell));
    currentlyFacing = EAST;
    currentVector.x = previousCell->vector.x + 1;
    currentVector.y = previousCell->vector.y;
    currentVector.cellAddress = currentCell;

    if(!is_member(allVectorSets, currentVector)) // iff the currentVector position is not in the set
    {

        insert(allVectorSets, currentVector);

        generateNeighborCellsForEast(currentCell);

        currentCell->origin = false;
        currentCell->ending = false;
        // Give this cell the coordinates associated with it
        currentCell->vector.x = currentVector.x;
        currentCell->vector.y = currentVector.y;
        currentCell->vector.cellAddress = currentCell;

        // The neighbors are all assumed to be moveable until proven otherwise

        // Query about its neighboring cells wall status
        // Directions *neighbors = get_directions(currentlyFacing);

        currentCell->northWall = neighbors->north;
        currentCell->southWall = neighbors->south;
        currentCell->eastWall = neighbors->east;
        currentCell->westWall = neighbors->west;

        // IFF all these conditions return false, then it is considered the end of the maze
        if(currentCell->northWall == false && currentCell->southWall == false && currentCell->eastWall == false)
        {
            currentCell->ending = true;
            currentCell->northWall = true;
            currentCell->southWall = true;
            currentCell->eastWall = true;
            free(currentCell->northNeighbor);
            currentCell->northNeighbor = NULL;
            free(currentCell->southNeighbor);
            currentCell->southNeighbor = NULL;
            free(currentCell->eastNeighbor);
            currentCell->eastNeighbor = NULL;
            endOfMaze.genesisCell = currentCell;
        }

        // For each of the directions, if there is a wall, just reclaim the memory there
        if(currentCell->northWall)
        {
            free(currentCell->northNeighbor);
            currentCell->northNeighbor = NULL;
        }
        if(currentCell->southWall)
        {
            free(currentCell->southNeighbor);
            currentCell->southNeighbor = NULL;
        }
        if(currentCell->eastWall)
        {
            free(currentCell->eastNeighbor);
            currentCell->eastNeighbor = NULL;
        }
    }

    previousCell = currentCell; // update the previous cell
}

void movedWest(int currentlyFacing, Directions* neighbors)
{
    if(previousCell->westWall == true)
    {
        printf("Cannot go here!\n");
        return;
    }
    currentCell = previousCell->westNeighbor;
    currentlyFacing = WEST;
    currentVector.x = previousCell->vector.x - 1;
    currentVector.y = previousCell->vector.y;
    currentVector.cellAddress = currentCell;

    if(!is_member(allVectorSets, currentVector)) // iff the currentVector position is not in the set
    {

        insert(allVectorSets, currentVector);

        generateNeighborCellsForWest(currentCell);

        currentCell->origin = false;
        currentCell->ending = false;
        // Give this cell the coordinates associated with it
        currentCell->vector.x = currentVector.x;
        currentCell->vector.y = currentVector.y;
        currentCell->vector.cellAddress = currentCell;

        // The neighbors are all assumed to be moveable until proven otherwise

        // Query about its neighboring cells wall status
        // Directions *neighbors = get_directions(currentlyFacing);

        currentCell->northWall = neighbors->north;
        currentCell->southWall = neighbors->south;
        currentCell->eastWall = neighbors->east;
        currentCell->westWall = neighbors->west;

        // IFF all these conditions return false, then it is considered the end of the maze
        if(!(currentCell->northWall & currentCell->southWall & currentCell->westWall))
        {
            currentCell->ending = true;
            currentCell->northWall = true;
            currentCell->southWall = true;
            currentCell->westWall = true;

            free(currentCell->northNeighbor);
            currentCell->northNeighbor = NULL;
            free(currentCell->southNeighbor);
            currentCell->southNeighbor = NULL;
            free(currentCell->westNeighbor);
            currentCell->westNeighbor = NULL;

            endOfMaze.genesisCell = currentCell;
        }

        // For each of the directions, if there is a wall, just reclaim the memory there
        if(currentCell->northWall)
        {
            free(currentCell->northNeighbor);
            currentCell->northNeighbor = NULL;
        }
        if(currentCell->southWall)
        {
            free(currentCell->southNeighbor);
            currentCell->southNeighbor = NULL;
        }
        if(currentCell->westWall)
        {
            free(currentCell->westNeighbor);
            currentCell->westNeighbor = NULL;
        }
    }

    previousCell = currentCell; // update the previous cell
}

void generateNeighborCellsForNorth(Cell *currentCell) // just generates and initializes all the information required
{   
    currentCell->southNeighbor = previousCell;
    Cell *newNorthNeighbor = malloc(sizeof(Cell));
    Cell *newEastNeighbor = malloc(sizeof(Cell));
    Cell *newWestNeighbor = malloc(sizeof(Cell));
    if(newNorthNeighbor == NULL || newEastNeighbor == NULL || newWestNeighbor == NULL)
    {
        exit(1);
    }
    currentCell->northNeighbor = newNorthNeighbor;
    currentCell->eastNeighbor = newEastNeighbor;
    currentCell->westNeighbor = newWestNeighbor;
}

void generateNeighborCellsForSouth(Cell *currentCell)
{
    currentCell->northNeighbor = previousCell;
    Cell *newSouthNeighbor = malloc(sizeof(Cell));
    Cell *newEastNeighbor = malloc(sizeof(Cell));
    Cell *newWestNeighbor = malloc(sizeof(Cell));
    if(newSouthNeighbor == NULL || newEastNeighbor == NULL || newWestNeighbor == NULL)
    {
        exit(1);
    }
    currentCell->southNeighbor = newSouthNeighbor;
    currentCell->eastNeighbor = newEastNeighbor;
    currentCell->westNeighbor = newWestNeighbor;
}

void generateNeighborCellsForEast(Cell *currentCell)
{
    currentCell->westNeighbor = previousCell;
    Cell *newNorthNeighbor = malloc(sizeof(Cell));
    Cell *newSouthNeighbor = malloc(sizeof(Cell));
    Cell *newEastNeighbor = malloc(sizeof(Cell));
    if(newSouthNeighbor == NULL || newEastNeighbor == NULL || newNorthNeighbor == NULL)
    {
        exit(1);
    }
    currentCell->northNeighbor = newNorthNeighbor;
    currentCell->southNeighbor = newSouthNeighbor;
    currentCell->eastNeighbor = newEastNeighbor;
}

void generateNeighborCellsForWest(Cell *currentCell)
{
    currentCell->eastNeighbor = previousCell;
    Cell *newNorthNeighbor = malloc(sizeof(Cell));
    Cell *newSouthNeighbor = malloc(sizeof(Cell));
    Cell *newWestNeighbor = malloc(sizeof(Cell));
    if(newSouthNeighbor == NULL || newWestNeighbor == NULL || newNorthNeighbor == NULL)
    {
        exit(1);
    }
    currentCell->northNeighbor = newNorthNeighbor;
    currentCell->southNeighbor = newSouthNeighbor;
    currentCell->westNeighbor = newWestNeighbor;
}

Cell** getMap(struct Set *set)
{
    Coordinates *arrayOfCords = getColsAndRows(set);

    Coordinates dimensions = arrayOfCords[0];
    Coordinates lowestValues = arrayOfCords[1];

    short rows = dimensions.y;
    short cols = dimensions.x;
    short lowestX = lowestValues.x;
    short lowestY = lowestValues.y;

    Cell **mapGrid = (Cell **)calloc(rows, sizeof(Cell *));

    for(short i = 0; i < rows; i++)
    {
        mapGrid[i] = (Cell *)calloc(cols, sizeof(Cell)); 
    }

    short setSize = set->length;
    
    for(short i = 0; i < setSize; i++)
    {
        Cell *currentCell = set->members[i].cellAddress;
        short normalizedColumnPosition = set->members[i].x + lowestX;
        short normalizedRowPosition = set->members[i].y + lowestY;
        mapGrid[normalizedRowPosition][normalizedColumnPosition] = *currentCell;
    }

    free(arrayOfCords);
    
    return mapGrid;
    // get the extreme ends of -y, -x, y, x
    // form rows and columns based off the absolute diff btwn -y to y for rows and -x to x for columns
    // malloc(sizeof(Cell*rows*cols))
    // walk through the entire set and pluck their addresses one by one
}

void destroyMaze(Cell *cellToDestroy)
{
    if(cellToDestroy == NULL)
    {
        return;
    }
    destroyMaze(cellToDestroy->northNeighbor);
    destroyMaze(cellToDestroy->southNeighbor);
    destroyMaze(cellToDestroy->eastNeighbor);
    destroyMaze(cellToDestroy->westNeighbor);
    free(cellToDestroy);
    return;
}

Set* init()
{
  Set *new_set = malloc(sizeof(Set));
  
  new_set->length = 0;
  new_set->members = malloc(sizeof(Coordinates));
  return new_set;
}

bool is_empty(Set *set)
{
  return (set->length == 0);
}

bool is_member(Set *set, Coordinates inputVector)
{
  // if we can find the value in the set's members, it is in the set
  for (int i = 0; i < set->length; i++)
      if(  
          (set->members[i].x) == (inputVector.x) //check the x-axis
            &&
          (set->members[i].y) == (inputVector.y) //check the y-axis
        )
        {
          return true;
        }
  
  // if after checking all the set's members we can't find the value, it is 
  // not a member of the set
  return false;
}

void insert(Set *set, Coordinates inputVector)
{
    set->members = realloc(set->members, sizeof(Coordinates) * (set->length + 1));
    
    set->members[set->length].x = inputVector.x;
    set->members[set->length].y = inputVector.y;
    set->members[set->length].cellAddress = inputVector.cellAddress;

    set->length = set->length + 1;
    
}

void print_set(Set *set)
{
  for (int i = 0; i < set->length; i++)
  {
      if (i == (set->length - 1))
        printf("x:%d  y:%d\n", set->members[i].x, set->members[i].y);
      else
        printf("x:%d  y:%d\n", set->members[i].x, set->members[i].y);
  }
}

Coordinates* getColsAndRows(Set *set)
{
    unsigned short size = set->length;

    Coordinates *colsAndRowsVector = (Coordinates *)malloc((2 * sizeof(Coordinates)));
  
    if(colsAndRowsVector == NULL)
    {
        exit(1);
    }

    short lowestX = 0;
    short lowestY = 0;
    short highestX = 0;
    short highestY = 0;

    for(int i = 0; i < size; i++)
    {   
        Coordinates currentVector = set->members[i];
        short currentX = currentVector.x;
        short currentY = currentVector.y;
        // Check the lowest vectors and update
        if(lowestX > currentX)
        {
            lowestX = currentX;
        }
        if(lowestY > currentY)
        {
            lowestY = currentY;
        }
        if(highestX < currentX)
        {
            highestX = currentX;
        }
        if(highestY < currentY)
        {
            highestY = currentY;
        }
    }

    short rows = DIFFERENCE(lowestY, highestY) + 1;
    short columns = DIFFERENCE(lowestX, highestX) + 1;

    colsAndRowsVector[0].y = rows;
    colsAndRowsVector[0].x = columns;
    
    colsAndRowsVector[1].y = lowestY;
    colsAndRowsVector[1].x = lowestX;

    return colsAndRowsVector;

}

void destroySet(Set *set) 
{
    if (set != NULL) 
    {
        if (set->members != NULL) 
        {
            free(set->members);
        }
        free(set);
    }
}