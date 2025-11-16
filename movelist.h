#pragma once

#include <stdlib.h>

// Use enum for movement direction
typedef enum dir {
  DIR_N, DIR_E, DIR_S, DIR_W,
  EMPTY
} dir_t;

// Used to store a list of movement directions
typedef struct movelist {
  int* input;
} movelist_t;

// Initialize a movelist. The m parameter points to space that holds a movelist
void movelist_init(movelist_t* m);

// Clean up allocated memory in a movelist.
void movelist_destroy(movelist_t* m);

// Peek at the value at the front of movelist m. Returns EMPTY if the list is empty
dir_t movelist_peek_front(const movelist_t* m);

// Take the value at the front of movelist m. Returns EMPTY if the list is empty
dir_t movelist_take_front(movelist_t* m);

// Peek at the value at the back of movelist m. Returns EMPTY if the list is empty
dir_t movelist_peek_back(const movelist_t* m);

// Take the value at the back of movelist m. Returns EMPTY if the list is empty
dir_t movelist_take_back(movelist_t* m);

// Add a move to the front of movelist m
void movelist_add_front(movelist_t* m, dir_t d);

// Add a move to the back of movelist m
void movelist_add_back(movelist_t* m, dir_t d);

// Empty the movelist m
void movelist_clear(movelist_t* m);
