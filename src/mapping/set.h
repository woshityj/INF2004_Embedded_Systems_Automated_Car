#ifndef sets
#define sets

typedef struct Coordinates{
    int x;      // vector x-axis
    int y;      // vector y-axis
    Cell *cellAddress;
} Coordinates;

typedef struct Set{
  Coordinates *members;    // pointer to dynamically allocated array of the set members
  int length;      // the number of members of the set
} Set;

Set* init();
bool is_empty(Set *set);
bool is_member(Set *set, Coordinates inputVector);
void insert(Set *set, Coordinates inputVector);
void print_set(Set *set);

#endif