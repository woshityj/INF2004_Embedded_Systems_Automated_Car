#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
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
    Directions* dir = (Directions*)malloc(sizeof(Directions));

		// Smaller maze start

    //smallMaze(dir);
    
		//Smaller maze end

		// Final maze start 
    finalMaze(dir);
    // Final maze end

    // Had to hard code change this portion because the logic in the maze formation compared to IR data is different
		Cell *endingCell = endOfMaze.genesisCell;
		endingCell->northWall = false;
    endingCell->ending = true;
    // print_set(allVectorSets);
		printMap();

    // For floodfill score assignment
    Set *visitedSet = init();
    Cell *endSource = endingCell;

    floodfill(endSource, visitedSet, 0);
    printf("\n");
    shortestPath(startOfMaze.genesisCell, endOfMaze.genesisCell);
    free(dir);
    free(visitedSet);
    free(allVectorSets);
    destroyMaze(startOfMaze.genesisCell);

}

void smallMaze(Directions *dir)
{
    assignWalls(dir, NORTH_WALL, false, false, WEST_WALL);
    currentCell = initMaze(dir);

    currentlyFacing = EAST;
    assignWalls(dir, false, SOUTH_WALL, EAST_WALL, false);
    movedForward(currentlyFacing, dir);

    currentlyFacing = NORTH;
    assignWalls(dir, NORTH_WALL, false, EAST_WALL, false);
    movedForward(currentlyFacing, dir);

    currentlyFacing = WEST;
    assignWalls(dir, NORTH_WALL, SOUTH_WALL, false, false);
    movedForward(currentlyFacing, dir);

    assignWalls(dir, false, false, false, WEST_WALL);
    movedForward(currentlyFacing, dir);

    currentlyFacing = SOUTH;
    assignWalls(dir, false, SOUTH_WALL, EAST_WALL, WEST_WALL);
    movedForward(currentlyFacing, dir);

    currentlyFacing = NORTH;
    movedForward(currentlyFacing, dir);

    assignWalls(dir, NORTH_WALL, false, false, WEST_WALL);
    movedForward(currentlyFacing, dir);

    currentlyFacing = EAST;
    assignWalls(dir, NORTH_WALL, SOUTH_WALL, false, false);
    movedForward(currentlyFacing, dir);
    endOfMaze.genesisCell = currentCell;

    assignWalls(dir, NORTH_WALL, SOUTH_WALL, WEST_WALL, false);
    movedForward(currentlyFacing, dir);

    currentlyFacing = WEST;
    movedForward(currentlyFacing, dir);
}

void finalMaze(Directions *dir)
{
    assignWalls(dir, false, false, false, false);
    currentCell = initMaze(dir);

    currentlyFacing = WEST; // turn 90 degrees to the West
    assignWalls(dir, false, SOUTH_WALL, false, WEST_WALL);
    movedForward(currentlyFacing, dir);
    
    currentlyFacing = NORTH;
    assignWalls(dir, NORTH_WALL, false, EAST_WALL, false);
    movedForward(currentlyFacing, dir);

    currentlyFacing = WEST;
    assignWalls(dir, NORTH_WALL, false, false, WEST_WALL);
    movedForward(currentlyFacing, dir);
    
    currentlyFacing = SOUTH;
    assignWalls(dir, false, SOUTH_WALL, EAST_WALL, WEST_WALL);
    movedForward(currentlyFacing, dir);
    
    currentlyFacing = NORTH;
    movedForward(currentlyFacing, dir);

    currentlyFacing = EAST;
    movedForward(currentlyFacing, dir);

    currentlyFacing = SOUTH;
    movedForward(currentlyFacing, dir);

    currentlyFacing = EAST;
    movedForward(currentlyFacing, dir);

    currentlyFacing = NORTH;
    assignWalls(dir, NORTH_WALL, false, EAST_WALL, WEST_WALL);
    movedForward(currentlyFacing, dir);

    currentlyFacing = SOUTH;
    movedForward(currentlyFacing, dir);

    currentlyFacing = EAST;
    assignWalls(dir, false, SOUTH_WALL, EAST_WALL, false);
    movedForward(currentlyFacing, dir);

    currentlyFacing = NORTH;
    assignWalls(dir, false, false, EAST_WALL, WEST_WALL);
    movedForward(currentlyFacing, dir);

    assignWalls(dir, false, false, EAST_WALL, false);
    movedForward(currentlyFacing, dir);

    currentlyFacing = WEST;
    assignWalls(dir, NORTH_WALL, SOUTH_WALL, false, false);
    movedForward(currentlyFacing, dir);

    assignWalls(dir, NORTH_WALL, SOUTH_WALL, false, false);
    movedForward(currentlyFacing, dir);
    
    assignWalls(dir, NORTH_WALL, SOUTH_WALL, false, WEST_WALL);
    movedForward(currentlyFacing, dir);

    currentlyFacing = EAST;
    movedForward(currentlyFacing, dir);

    movedForward(currentlyFacing, dir);

    movedForward(currentlyFacing, dir);

    currentlyFacing = NORTH;
    assignWalls(dir, false, false, EAST_WALL, WEST_WALL);
    movedForward(currentlyFacing, dir);

    assignWalls(dir, NORTH_WALL, false, EAST_WALL, false);
    movedForward(currentlyFacing, dir);

    currentlyFacing = WEST;
    assignWalls(dir, false, false, false, WEST_WALL);
    movedForward(currentlyFacing, dir);

    currentlyFacing = SOUTH;
    assignWalls(dir, false, SOUTH_WALL, EAST_WALL, false);
    movedForward(currentlyFacing, dir);

    currentlyFacing = WEST;
    assignWalls(dir, NORTH_WALL, SOUTH_WALL, false, false);
    movedForward(currentlyFacing, dir);

    assignWalls(dir, false, SOUTH_WALL, false, WEST_WALL);
    movedForward(currentlyFacing, dir);

    currentlyFacing = NORTH;
    assignWalls(dir, NORTH_WALL, false, EAST_WALL, WEST_WALL);
    movedForward(currentlyFacing, dir);

    currentlyFacing = SOUTH;
    movedForward(currentlyFacing, dir);

    currentlyFacing = EAST;
    movedForward(currentlyFacing, dir);

    movedForward(currentlyFacing, dir);

    currentlyFacing = NORTH;
    movedForward(currentlyFacing, dir);

    assignWalls(dir, NORTH_WALL, false, false, false);
    movedForward(currentlyFacing, dir);

    currentlyFacing = EAST;
    assignWalls(dir, NORTH_WALL, SOUTH_WALL, EAST_WALL, false);
    movedForward(currentlyFacing, dir);

    currentlyFacing = WEST;
    movedForward(currentlyFacing, dir);

    assignWalls(dir, true, false, false, false);
    movedForward(currentlyFacing, dir);
    endOfMaze.genesisCell = currentCell;

    currentlyFacing = SOUTH;
    assignWalls(dir, false, SOUTH_WALL, EAST_WALL, WEST_WALL);
    movedForward(currentlyFacing, dir);

    currentlyFacing = NORTH;
    movedForward(currentlyFacing, dir);

    currentlyFacing = WEST;
    assignWalls(dir, NORTH_WALL, SOUTH_WALL, false, WEST_WALL);
    movedForward(currentlyFacing, dir);

    currentlyFacing = EAST;
    movedForward(currentlyFacing, dir);
}

void printMap()
{
	Coordinates *rowsAndCols = getColsAndRows(allVectorSets);
    Coordinates dimensions = rowsAndCols[0];
    Coordinates lowestValues = rowsAndCols[1];
    int rows = dimensions.y;
    int cols = dimensions.x;
    int lowestX = lowestValues.x;
    int lowestY = lowestValues.y;
    // printf("Rows: %d, Cols: %d\n", rows, cols);
    // printf("Lowest x value: %d, Lowest y value: %d\n", lowestX, lowestY);
    Cell **maze = getMap(allVectorSets);
    Cell mazeCell;

		char line1[200];
		char line2[200];
		char final[2000];

		strcpy(line1, " _ ");
		strcpy(line2, "|_ ");
		strcpy(final, "");
		
    // This for loop represents the map from top left to bottom right as a 2D grid
    for(int i = rows - 1; i >= 0; i--)
    {
      for(int j = 0; j < cols; j++)
      {
        //mazeCell = maze[i][j];
        for(int k = 0; k < cols; k++)
        {
            if(k == 0 && i == rows -1) //most top left maze cell
            {
                
            }
        }
        mazeCell = maze[i][j];
        if(mazeCell.origin == true)
        {
          // printf("Hello I am the entrance\n");
					strcat(line2, "  ");
					// printf("coordinates of cell: %d,%d\n", mazeCell.vector.x, mazeCell.vector.y);
          // printf("North wall: %d\n", mazeCell.northWall);
          // printf("South wall: %d\n", mazeCell.southWall);
          // printf("East wall: %d\n", mazeCell.eastWall);
          // printf("West wall: %d\n", mazeCell.westWall);
					// printf("how i look like on the map:\n");
					// printf("%s\n", line2);
        }
        if(mazeCell.origin == false && mazeCell.vector.cellAddress != NULL)
        {	
          if (mazeCell.vector.y == (rows - 1) && mazeCell.vector.x == lowestX){
						if (mazeCell.northWall){
							strcpy(line1, " _ ");
						}
						else{
							strcpy(line1, "   ");
						}

						if (mazeCell.southWall && mazeCell.westWall && mazeCell.eastWall){
								strcpy(line2, "|_|");
							}
						else if (mazeCell.southWall && mazeCell.eastWall){
								strcpy(line2, " _|");
							}
						else if (mazeCell.southWall && mazeCell.westWall){
								strcpy(line2, "|_ ");
							}
						else if (mazeCell.westWall && mazeCell.eastWall){
								strcpy(line2, "| |");
							}
						else if (mazeCell.eastWall){
								strcpy(line2, "  |");
							}
						else if (mazeCell.westWall){
								strcpy(line2, "|  ");
							}
						else if (mazeCell.southWall){
								strcpy(line2, " _ ");
							}
							else{
								strcpy(line2, "   ");
							}
          }

					else if (mazeCell.vector.y == (rows - 1)){
						if (mazeCell.northWall){
              strcat(line1, "_ ");
            }
            else{
              strcat(line1, "  ");
            }

            if (mazeCell.southWall && mazeCell.eastWall){
              strcat(line2, "_|");
            }
            else if (mazeCell.southWall){
              strcat(line2, "_ ");
            }
            else if (mazeCell.eastWall){
              strcat(line2, " |");
            }
            else{
              strcat(line2, "  ");
            }
					}


					else if (mazeCell.vector.x == lowestX){
						if (mazeCell.southWall && mazeCell.eastWall && mazeCell.westWall){
              strcat(line2, "|_|");
            }
            else if (mazeCell.southWall && mazeCell.eastWall){
              strcat(line2, " _|");
            }
            else if (mazeCell.southWall && mazeCell.westWall){
              strcat(line2, "|_ ");
            }
						else if (mazeCell.eastWall && mazeCell.westWall){
							strcat(line2, "| |");
						}
            else if (mazeCell.southWall){
              strcat(line2, " _ ");
            }
            else if (mazeCell.eastWall){
              strcat(line2, "  |");
            }
            else if (mazeCell.westWall){
              strcat(line2, "|  ");
            }
            else{
              strcat(line2, "   ");
            }
					}

					else {
            if (mazeCell.southWall && mazeCell.eastWall){
              strcat(line2, "_|");
            }
            else if (mazeCell.southWall){
              strcat(line2, "_ ");
            }
            else if (mazeCell.eastWall){
              strcat(line2, " |");
            }
            else{
              strcat(line2, "  ");
            }
          }

          // printf("coordinates of cell: %d,%d\n", mazeCell.vector.x, mazeCell.vector.y);
          // printf("North wall: %d\n", mazeCell.northWall);
          // printf("South wall: %d\n", mazeCell.southWall);
          // printf("East wall: %d\n", mazeCell.eastWall);
          // printf("West wall: %d\n", mazeCell.westWall);
					// printf("how i look like on the map:\n");
					// printf("%s\n", line1);
					// printf("%s\n", line2);
          // printf("I am just a random cell\n");
        }
				
      }
			if (i == rows-1){
			strcat(final, line1);
      strcat(final, "\n");	
			}

			strcat(final, line2);
			strcat(final, "\n");

			strcpy(line1, "");
			strcpy(line2, "");

    }
		printf("%s", final);

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
  // printf("moved forward, facing %d\n", currentlyFacing);
  // printf("Now at coord x:%d, y:%d\n", currentVector.x, currentVector.y);

  //  endOfMaze.genesisCell = currentCell;
  //  currentCell->ending = true;
  //  free(currentCell->northNeighbor);
  //  currentCell->northNeighbor = NULL;
  //}
}

// Only works if the maze is already mapped, otherwise will break
void movedBackwards(int forwardFacing, Directions* neighbors)
{
    if(currentlyFacing == NORTH)
    {
        movedSouth(forwardFacing, neighbors);
        // Set the currentlyFacing back to where it was actually at since it's moving backwards
        currentlyFacing = NORTH;
    }
    else if(currentlyFacing == SOUTH)
    {
        movedNorth(forwardFacing, neighbors);
        currentlyFacing = SOUTH;
    }
    else if(currentlyFacing == EAST)
    {
        movedWest(forwardFacing, neighbors);
        currentlyFacing = EAST;
    }
    else
    {
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

    currentCell->score = -1;

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
        currentCell->score = -1;
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
        currentCell->score = -1;
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
        currentCell->score = -1;
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
        currentCell->score = -1;
        // The neighbors are all assumed to be moveable until proven otherwise

        // Query about its neighboring cells wall status
        // Directions *neighbors = get_directions(currentlyFacing);

        currentCell->northWall = neighbors->north;
        currentCell->southWall = neighbors->south;
        currentCell->eastWall = neighbors->east;
        currentCell->westWall = neighbors->west;

        // IFF all these conditions return false, then it is considered the end of the maze
        if(currentCell->northWall == false && currentCell->southWall == false && currentCell->westWall == false)
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
    short lowestX = abs(lowestValues.x);
    short lowestY = abs(lowestValues.y);

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
    free(cellToDestroy);
    destroyMaze(cellToDestroy->northNeighbor);
    destroyMaze(cellToDestroy->southNeighbor);
    destroyMaze(cellToDestroy->eastNeighbor);
    destroyMaze(cellToDestroy->westNeighbor);
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

void floodfill(Cell *endSource, Set *visited, int score)
{
    if (endSource == NULL || is_member(visited, endSource->vector))
    {
        return;
    }

    set_score(endSource, visited, score);
    insert(visited, endSource->vector);

    floodfill(endSource->northNeighbor, visited, score + 1);
    floodfill(endSource->southNeighbor, visited, score + 1);
    floodfill(endSource->eastNeighbor, visited, score + 1);
    floodfill(endSource->westNeighbor, visited, score + 1);
}


void set_score(Cell *cell, Set *set, int score)
{
    if(cell == NULL)
    {
        return;
    }
    Coordinates thisVector = cell->vector;
    if(is_member(set, thisVector)) // if visited dont bother
    {
        return;
    }
    insert(set, thisVector); // mark as visited
    // printf("Inserted x:%d, y: %d into the queue, the score for this is at %d\n", cell->vector.x, cell->vector.y, score);

    cell->score = score;

}

Cell* lookAtScores(Cell *currentSource)
{
    int lowestScore = 1000000;
    Cell *lowestAddress;
    if( (currentSource->northNeighbor != NULL) && ( (currentSource->northNeighbor->score) < lowestScore) )
    {
        lowestScore = currentSource->northNeighbor->score;
        lowestAddress = currentSource->northNeighbor;
    }

    if( (currentSource->southNeighbor != NULL) && ( (currentSource->southNeighbor->score) < lowestScore) )
    {
        lowestScore = currentSource->southNeighbor->score;
        lowestAddress = currentSource->southNeighbor;
    }

    if( (currentSource->eastNeighbor != NULL) && ( (currentSource->eastNeighbor->score) < lowestScore) )
    {
        lowestScore = currentSource->eastNeighbor->score;
        lowestAddress = currentSource->eastNeighbor;
    }

    if( (currentSource->westNeighbor != NULL) && ( (currentSource->westNeighbor->score) < lowestScore) )
    {
        lowestScore = currentSource->westNeighbor->score;
        lowestAddress = currentSource->westNeighbor;
    }

    return lowestAddress;
}
void shortestPath(Cell *startSource, Cell *endSource)
{
    printf("Starting at (%d,%d)!\n", startSource->vector.x, startSource->vector.y);
    int i = 1;
    while(startSource != endSource)
    {
        Cell *moveTo = lookAtScores(startSource);
        printf("Step %d) Move to (%d,%d)\n", i, moveTo->vector.x, moveTo->vector.y);
        i++;
        startSource = moveTo;
    }
    printf("Shortest path found!\n");
}
