#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "set.h"

// initilizes a new set to add coordinates into
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
    set->members[set->length] = inputVector;

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

/* How to use */
//int main()
//{
//  // create a set
//  Set *setA = init();
//  
//  // the set should initially be empty
//  //if (is_empty(setA)) printf("Set A is empty.\n");
//  //else printf("Set A is not empty.\n");
//  
//  // insert several members into the set, including a 'repeat/duplicate' (70)
//  Coordinates firstCell;
//  firstCell.x = 0;
//  firstCell.y = 0;
//  insert(setA, firstCell);
//
//  Coordinates secondCell;
//  secondCell.x = 1;
//  secondCell.y = 0;
//  insert(setA, secondCell);
//  
//  Coordinates thirdCell;
//  thirdCell.x = 2;
//  thirdCell.y = 0;
//  insert(setA, thirdCell);
//
//  Coordinates fourthCell;
//  fourthCell.x = 2;
//  fourthCell.y = 1;
//  insert(setA, fourthCell);
//
//  Coordinates fifthCell;
//  fifthCell.x = 2;
//  fifthCell.y = 2;
//  insert(setA, fifthCell);
//  
//  // when we print out set A, we should only see 7 once
//  printf("Set A: \n");
//  print_set(setA);
//
//  if(is_member(setA, firstCell))
//  {
//    printf("In-set check: true\n");
//  }
//  else
//  {
//    printf("In-set check: false\n");
//  }
//  return 0;
//}
