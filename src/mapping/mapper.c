#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "mapper.h"
#include "set.h"

Set *allVectorSets;
Head startOfMaze;
Head endOfMaze;
Cell *currentCell;
Cell *previousCell;
Coordinates currentVector;

int currentlyFacing = NORTH; // always assumes it starts off facing "north" which is calibrated to the magnetometer of relative 'north'
bool isMapping = 1;

int main(void) //rename to mazeInit() as a task
{
    allVectorSets = init();
    startOfMaze.genesisCell = generateGenesis(); // returns the address of the starting cell
    currentVector.x = 0;
    currentVector.y = 0;

    // generate the genesis cell
    previousCell = NULL;
    currentCell = startOfMaze.genesisCell;

    //loop unroll for the genesis cell
    currentCell->northWall = getWall("north", currentlyFacing);
    if(currentCell->northWall) // theres an north wall
    {
        free(currentCell->north); // reclaim memory
    }
    currentCell->eastWall = getWall("east", currentlyFacing);
    if(currentCell->eastWall) // theres an east wall
    {
        free(currentCell->east);
    }

    currentCell->westWall = getWall("west", currentlyFacing);
    if(currentCell->westWall) // theres an west wall
    {
        free(currentCell->west); // reclaim memory
    }

    previousCell = currentCell;

    //move this outside this mazeInit() and becomes a interrupt callback

    // for every cell moment made (decided by wheel encoder and others if possible)
    // add a new Cell and update currentCell to be that Cell

    gpio_set_irq_enabled_with_callback(MOVEMENT_PULSE,/* message buffer message type telling what direction it moved */, true, &movementCallBack);
}

//movementCallbackFunction interrupts the task and changes previousCell before making a new currentCell

void movementCallBack(uint gpio, uint32_t events)
{
    if(movedNorth)
    {
        currentCell = previousCell.north;
        currentlyFacing = NORTH;
        currentVector.x = previousCell->vector.x;
        currentVector.y = previousCell->vector.y + 1;

        if(!is_member(allVectorSets, currentVector)) // iff the currentVector position is not in the set
        {
            insert(allVectorSets, currentVector);
            currentCell = previousCell.north;
            currentCell = generateCellsForNorth(currentCell); // generate a 3 new cells and put the previous cell's address into south and add the vectors

            currentCell->southWall = false; // logically this means that there is no wall in the south cell

            currentCell->eastWall = getWall("east", currentlyFacing);
            if(currentCell->eastWall) // theres an east wall
            {
                free(currentCell->east);
            }

            currentCell->westWall = getWall("west", currentlyFacing);
            if(currentCell->westWall) // theres an west wall
            {
                free(currentCell->west); // reclaim memory
            }

            currentCell->northWall = getWall("north", currentlyFacing);
            if(currentCell->northWall) // theres an north wall
            {
                free(currentCell->north); // reclaim memory
            }

        }

        previousCell = currentCell; // update the previous cell

    }

    else if(movedSouth)
    {

    }


    else if(movedEast)
    {

    }


    else if(movedWest)

    if(is_End()) // reached end doesn't mean mapped whole maze
    {
        currentCell.ending = true;
        endOfMaze.genesisCell = currentCell;
    }
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

    genesisCell->southWall = true; // assumes to have a wall behind
    
    // assumes every other direction is clear for now
    genesisCell->northWall = false;
    genesisCell->eastWall = false;
    genesisCell->westWall = false; // change these to the function call for the eyes IR

    // starts at origin
    genesisCell->vector.x = 0; // puts the value 0 into x
    genesisCell->vector.y = 0; // puts the value 0 into y

    // assumes it all directions neighbors
    genesisCell->north = malloc(sizeof(Cell));
    genesisCell->south = NULL; // genesis always has a NULL south
    genesisCell->east = malloc(sizeof(Cell));;
    genesisCell->west = malloc(sizeof(Cell));

    return genesisCell;
}

Cell* generateCellsForNorth(Cell *currentCell) // just generates and initializes all the information required
{
    currentCell->south = previousCell;

    Cell *newCell = malloc(sizeof(Cell));

    if(newCell == NULL)
    {
        exit(1);
    }

    newCell->origin = false;
    newCell->ending = false;

    newCell->southWall = false; // will always be true

    newCell->northWall = false;
    newCell->eastWall = false;
    newCell->westWall = false;

    newCell.vector.x = currentVector.x;
    newCell.vector.y = currentVector.y;

    newCell->north = malloc(sizeof(Cell));
    newCell->south = currentCell;
    newCell->east = malloc(sizeof(Cell));
    newCell->west = malloc(sizeof(Cell));

    return newCell;
}

bool isNull(Cell *cellToCheck)
{
    if(cellToCheck == NULL)
    {
        return true;
    }
}
bool isEnd()
{
    // bunch of sensor checks
    // return true if confident it's the end
    // else return false
}