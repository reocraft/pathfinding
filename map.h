#pragma once

#include <stdbool.h>
#include <stdlib.h>

// Use an enum to keep track of map tiles
typedef enum cell {
  CELL_BLANK,
  CELL_GRASS,
  CELL_WALL,
  CELL_LAKE
} cell_t;

// A coordinate is just an x and y value
typedef struct coord {
  int x;
  int y;
} coord_t;

// A map is a data array with width, heigh, and starting location information
typedef struct map {
  cell_t* data;
  size_t width;
  size_t height;
  coord_t start;
} map_t;

// Load a map from a file. Returns false if any error occurs during loading
bool load_map(const char* filename, map_t* map);

// Free all the memory used to store a map
void free_map(map_t* map);

// Display the map and player
void display_map(map_t map, coord_t player, coord_t* viewport);

// Check if a coordinate on a map is a valid place to move. Returns true if allowed.
bool can_move(map_t map, coord_t c);

