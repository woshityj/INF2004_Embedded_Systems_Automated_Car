#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../infrared/infrared.h"

#include "mapper.h"
#include "set.h"

Set *allVectorSets;
Head startOfMaze;
Head endOfMaze;
Cell *currentCell;
Cell *previousCell;
Coordinates currentVector;

volatile int currentlyFacing = NORTH; // always assumes it starts off facing "north" which is calibrated to the magnetometer of relative 'north'

// The only 2 functions that the movement logic needs to call everytime it does something
void movedForward(int currentlyFacing)
{
    switch(currentlyFacing)
    {
        case NORTH:
            movedNorth(currentlyFacing);
        case SOUTH:
            movedSouth(currentlyFacing);
        case EAST:
            movedEast(currentlyFacing);
        case WEST:
            movedWest(currentlyFacing);
    }
}

// Only works if the maze is already mapped, otherwise will break
void movedBackwards(int forwardFacing)
{
    switch(currentlyFacing)
    {
        case NORTH:
            movedSouth(forwardFacing);
            // Set the currentlyFacing back to where it was actually at since it's moving backwards
            currentlyFacing = NORTH;
        case SOUTH:
            movedNorth(forwardFacing);
            currentlyFacing = SOUTH;
        case EAST:
            movedWest(forwardFacing);
            currentlyFacing = EAST;
        case WEST:
            movedEast(forwardFacing);
            currentlyFacing = WEST;
    }
}

// API ends


Cell *initMaze()
{
    allVectorSets = init();
    startOfMaze.genesisCell = generateGenesis(); // returns the address of the starting cell
    currentVector.x = 0;
    currentVector.y = 0;

    // Puts x,y = 0 into the set to mark as a visited cell
    insert(allVectorSets, currentVector);
    // Generate the genesis cell
    previousCell = NULL;
    currentCell = startOfMaze.genesisCell;

    // Api to the infrared sensors
    Directions *neighbors = get_directions(currentlyFacing);

    // Find out whether or not there are walls at these 4 directions
    currentCell->northWall = neighbors->north;
    currentCell->southWall = neighbors->south;
    currentCell->eastWall = neighbors->east;
    currentCell->westWall = negihbors->west;

    // For each of the directions, if there is a wall, just reclaim the memory there
    if(currentCell->northWall)
    {
        free(currentCell->north);
    }
    if(currentCell->eastWall) // theres an east wall
    {
        free(currentCell->east);
    }
    if(currentCell->westWall) // theres an west wall
    {
        free(currentCell->west); // reclaim memory
    }
    previousCell = currentCell;

    return currentCell; // returns the cell's address to the main callee
}

void movedNorth(int currentlyFacing)
{
    currentCell = previousCell.northNeighbor;
    currentlyFacing = NORTH;
    currentVector.x = previousCell->vector.x;
    currentVector.y = previousCell->vector.y + 1;

    if(!is_member(allVectorSets, currentVector)) // iff the currentVector position is not in the set
    {
        insert(allVectorSets, currentVector);

        currentCell.origin = false;
        currentCell.ending = false;
        // Give this cell the coordinates associated with it
        currentCell->vector.x = currentVector.x;
        currentCell->vector.y = currentVector.y;
        currentCell->vector.cellAddress = currentCell;

        // The neighbors are all assumed to be moveable until proven otherwise
        generateNeighborCellsForNorth(currentCell);

        // Query about its neighboring cells wall status
        Directions *neighbors = get_directions(currentlyFacing);

        currentCell->northWall = neighbors->north;
        currentCell->southWall = neighbors->south;
        currentCell->eastWall = neighbors->east;
        currentCell->westWall = negihbors->west;

        // IFF all these conditions return true, then it is considered the end of the maze
        if(!(currentCell->northWall & currentCell->eastWall & currentCell->westWall))
        {
            currentCell.ending = true;
            currentCell->northWall = true;
            currentCell->easthWall = true;
            currentCell->westWall = true;
            free(currentCell->northNeighbor);
            free(currentCell->eastNeighbor);
            free(currentCell->westNeighbor);
            endOfMaze.genesisCell = currentCell;
        }

        // For each of the directions, if there is a wall, just reclaim the memory there
        if(currentCell->eastWall)
        {
            free(currentCell->eastNeighbor);
        }
        if(currentCell->westWall)
        {
            free(currentCell->westNeighbor);
        }
        if(currentCell->northWall)
        {
            free(currentCell->northNeighbor);
        }
    }
    previousCell = currentCell; // update the previous cell
}

void movedSouth(int currentlyFacing)
{
    currentCell = previousCell.southNeighbor;
    currentlyFacing = SOUTH;
    currentVector.x = previousCell->vector.x;
    currentVector.y = previousCell->vector.y - 1;

    if(!is_member(allVectorSets, currentVector)) // iff the currentVector position is not in the set
    {
        insert(allVectorSets, currentVector);

        currentCell.origin = false;
        currentCell.ending = false;
        // Give this cell the coordinates associated with it
        currentCell->vector.x = currentVector.x;
        currentCell->vector.y = currentVector.y;
        currentCell->vector.cellAddress = currentCell;

        // The neighbors are all assumed to be moveable until proven otherwise
        generateNeighborCellsForSouth(currentCell);

        // Query about its neighboring cells wall status
        Directions *neighbors = get_directions(currentlyFacing);

        currentCell->northWall = neighbors->north;
        currentCell->southWall = neighbors->south;
        currentCell->eastWall = neighbors->east;
        currentCell->westWall = negihbors->west;

        // IFF all these conditions return false, then it is considered the end of the maze
        if(!(currentCell->southWall & currentCell->eastWall & currentCell->westWall))
        {
            currentCell.ending = true;
            currentCell->southWall = true;
            currentCell->eastWall = true;
            currentCell->westWall = true;
            free(currentCell->southNeighbor);
            free(currentCell->eastNeighbor);
            free(currentCell->westNeighbor);
            endOfMaze.genesisCell = currentCell;
        }

        // For each of the directions, if there is a wall, just reclaim the memory there
        if(currentCell->southWall)
        {
            free(currentCell->southNeighbor);
        }
        if(currentCell->eastWall)
        {
            free(currentCell->eastNeighbor);
        }
        if(currentCell->westWall)
        {
            free(currentCell->westNeighbor);
        }
    }

    previousCell = currentCell; // update the previous cell
}

void movedEast(int currentlyFacing)
{
    currentCell = previousCell.eastNeighbor;
    currentlyFacing = EAST;
    currentVector.x = previousCell->vector.x + 1;
    currentVector.y = previousCell->vector.y;

    if(!is_member(allVectorSets, currentVector)) // iff the currentVector position is not in the set
    {
        insert(allVectorSets, currentVector);

        currentCell.origin = false;
        currentCell.ending = false;
        // Give this cell the coordinates associated with it
        currentCell->vector.x = currentVector.x;
        currentCell->vector.y = currentVector.y;
        currentCell->vector.cellAddress = currentCell;

        // The neighbors are all assumed to be moveable until proven otherwise
        generateNeighborCellsForEast(currentCell);

        // Query about its neighboring cells wall status
        Directions *neighbors = get_directions(currentlyFacing);

        currentCell->northWall = neighbors->north;
        currentCell->southWall = neighbors->south;
        currentCell->eastWall = neighbors->east;
        currentCell->westWall = negihbors->west;

        // IFF all these conditions return false, then it is considered the end of the maze
        if(!(currentCell->northWall & currentCell->southWall & currentCell->eastWall))
        {
            currentCell.ending = true;
            currentCell->northWall = true;
            currentCell->southWall = true;
            currentCell->eastWall = true;
            free(currentCell->northNeighbor);
            free(currentCell->southNeighbor);
            free(currentCell->eastNeighbor);
            endOfMaze.genesisCell = currentCell;
        }

        // For each of the directions, if there is a wall, just reclaim the memory there
        if(currentCell->northWall)
        {
            free(currentCell->northNeighbor);
        }
        if(currentCell->southWall)
        {
            free(currentCell->southNeighbor);
        }
        if(currentCell->eastWall)
        {
            free(currentCell->eastNeighbor);
        }
    }

    previousCell = currentCell; // update the previous cell
}

void movedWest(int currentlyFacing)
{
    currentCell = previousCell.westNeighbor;
    currentlyFacing = WEST;
    currentVector.x = previousCell->vector.x - 1;
    currentVector.y = previousCell->vector.y;

    if(!is_member(allVectorSets, currentVector)) // iff the currentVector position is not in the set
    {
        insert(allVectorSets, currentVector);

        currentCell.origin = false;
        currentCell.ending = false;
        // Give this cell the coordinates associated with it
        currentCell->vector.x = currentVector.x;
        currentCell->vector.y = currentVector.y;
        currentCell->vector.cellAddress = currentCell;

        // The neighbors are all assumed to be moveable until proven otherwise
        generateNeighborCellsForWest(currentCell);

        // Query about its neighboring cells wall status
        Directions *neighbors = get_directions(currentlyFacing);

        currentCell->northWall = neighbors->north;
        currentCell->southWall = neighbors->south;
        currentCell->eastWall = neighbors->east;
        currentCell->westWall = negihbors->west;

        // IFF all these conditions return false, then it is considered the end of the maze
        if(!(currentCell->northWall & currentCell->southWall & currentCell->westWall))
        {
            currentCell.ending = true;
            currentCell->northWall = true;
            currentCell->southWall = true;
            currentCell->westWall = true;
            free(currentCell->northNeighbor);
            free(currentCell->southNeighbor);
            free(currentCell->westNeighbor);
            endOfMaze.genesisCell = currentCell;
        }

        // For each of the directions, if there is a wall, just reclaim the memory there
        if(currentCell->northWall)
        {
            free(currentCell->northNeighbor);
        }
        if(currentCell->southWall)
        {
            free(currentCell->southNeighbor);
        }
        if(currentCell->westWall)
        {
            free(currentCell->westNeighbor);
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

Cell* getMap(Set *set)
{
    // get the extreme ends of -y, -x, y, x
    // form rows and columns based off the absolute diff btwn -y to y for rows and -x to x for columns
    // malloc(sizeof(Cell*rows*cols))
    // walk through the entire set and pluck their addresses one by one
}