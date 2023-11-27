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
    dir->north = false; // no wall
    dir->south = true; // wall
    dir->east = false; // no wall
    dir->west = true; // wall
    currentCell = initMaze(dir);
    currentlyFacing = EAST; // turn 90 degrees to the East
    dir->north = true; // wall
    dir->south = true; // wall
    dir->east = false; // free space
    dir->west = false; // no wall if you came from here
    movedForward(currentlyFacing, dir);
    currentlyFacing = WEST;
    movedForward(currentlyFacing, dir);
    currentlyFacing = NORTH;
    movedForward(currentlyFacing, dir);
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
  print_set(allVectorSets);
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


Cell *initMaze(Directions* neighbors)
{

    allVectorSets = init();

    startOfMaze.genesisCell = malloc(sizeof(Cell)); // returns the address of the starting cell

    currentVector.x = 0;
    currentVector.y = 0;

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
    printf("Generated Maze\n");
    return currentCell; // returns the cell's address to the main callee
}

void movedNorth(int currentlyFacing, Directions* neighbors)
{
    currentCell = previousCell->northNeighbor;
    currentlyFacing = NORTH;
    currentVector.x = previousCell->vector.x;
    currentVector.y = previousCell->vector.y + 1;

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
    currentCell = previousCell->southNeighbor;
    currentlyFacing = SOUTH;
    currentVector.x = previousCell->vector.x;
    currentVector.y = previousCell->vector.y - 1;

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
    printf("Attempting to move east\n");
    currentCell = previousCell->eastNeighbor;
    //currentCell = malloc(sizeof(Cell));
    currentlyFacing = EAST;
    currentVector.x = previousCell->vector.x + 1;
    currentVector.y = previousCell->vector.y;
    printf("no errors yet\n");
    if(!is_member(allVectorSets, currentVector)) // iff the currentVector position is not in the set
    {
        printf("new member\n");
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
    printf("Moved east successfully\n");
    previousCell = currentCell; // update the previous cell
}

void movedWest(int currentlyFacing, Directions* neighbors)
{
    currentCell = previousCell->westNeighbor;
    currentlyFacing = WEST;
    currentVector.x = previousCell->vector.x - 1;
    currentVector.y = previousCell->vector.y;

    if(!is_member(allVectorSets, currentVector)) // iff the currentVector position is not in the set
    {
        printf("new member\n");
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

Cell* generateGenesis()
{

    // allocate memory for one Cell
    Cell *genesisCell = malloc(sizeof(Cell));

    if(genesisCell == NULL)
    {
        exit(1);
    }

    genesisCell->origin = true;
    genesisCell->ending = false;

    genesisCell->southWall = false; // Assumes false so the map can have a hole in it to signify it's the start
    
    // assumes every other direction is clear for now
    genesisCell->northWall = false;
    genesisCell->eastWall = false;
    genesisCell->westWall = false; // change these to the function call for the eyes IR

    // starts at origin
    genesisCell->vector.x = 0; // puts the value 0 into x
    genesisCell->vector.y = 0; // puts the value 0 into y
    currentCell->vector.cellAddress = genesisCell;

    Cell *newNorthNeighbor = malloc(sizeof(Cell));
    Cell *newEastNeighbor = malloc(sizeof(Cell));
    Cell *newWestNeighbor = malloc(sizeof(Cell));

    if(newNorthNeighbor == NULL || newEastNeighbor == NULL || newWestNeighbor == NULL)
    {
        exit(1);
    }
    // Assumes it all directions neighbors
    genesisCell->northNeighbor = newNorthNeighbor;
    genesisCell->southNeighbor = NULL; // genesis always has a NULL south
    genesisCell->eastNeighbor = newEastNeighbor;
    genesisCell->westNeighbor = newWestNeighbor;

    return genesisCell;
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
  // allocate space for the set
  Set *new_set = malloc(sizeof(Set));
  
  // initially the set will be empty with no members, so set length to 0
  new_set->length = 0;

  // allocate enough space to store 1 member, we'll expand this as needed
  new_set->members = malloc(sizeof(Coordinates));

  // return the new Set (or more specifically, a pointer to it)
  return new_set;
}

// returns true if the Set is empty, and false otherwise
bool is_empty(Set *set)
{
  return (set->length == 0);
}

// returns true if value is a member of set, and false otherwise
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

// inserts the member value into the set (if it is not already in the set!)
void insert(Set *set, Coordinates inputVector)
{
    // allocate space to store the *new* amount of members in the set 
    set->members = realloc(set->members, sizeof(Coordinates) * (set->length + 1));
    
    // put the member into the set at the next available index
    set->members[set->length].x = inputVector.x;
    set->members[set->length].y = inputVector.y;
    // increment the set length to acknowledge the new length of the set
    set->length = set->length + 1;
    
}

// for debugging purposes only
void print_set(Set *set)
{
  // loop through the array of set values, print each of them out separated by 
  // a comma, except the last element - instead output a newline afterwards
  for (int i = 0; i < set->length; i++)
  {
      if (i == (set->length - 1))
        printf("x:%d  y: %d\n", set->members[i].x, set->members[i].y);
      else
        printf("x:%d  y:%d\n,", set->members[i].x, set->members[i].y);
  }
}

Coordinates* getColsAndRows(Set *set)
{
    unsigned short size = set->length;

    Coordinates *colsAndRowsVector = (Coordinates *)(2 * sizeof(Coordinates));
    
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

    short rows = DIFFERENCE(lowestY, highestY);
    short columns = DIFFERENCE(lowestX, highestX);

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