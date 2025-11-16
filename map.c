#include "map.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

// Load a map from a file. Returns false if any error occurs during loading
bool load_map(const char* filename, map_t* map) {
  // Set the map up for loading
  map->data = NULL;
  map->width = 0;
  map->height = 0;
  map->start.x = -1;
  map->start.y = -1;

  // Open the map file
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("Failed to open map file");
    return false;
  }

  // Read in the map file
  char* line = NULL;
  size_t linecap = 0;
  ssize_t length;
  while ((length = getline(&line, &linecap, fp)) > 0) {
    // If this is the first line, the map width will be zero. Calculate the real width
    if (map->width == 0) {
      // The length of the line is the map width plus one for the newline character
      map->width = length - 1;

    } else if (length < map->width) {
      // The line is too short to be a map row, so leave the reading loop. This might be an error, or could just be the last line
      break;
    }

    // Add space for a row of the map
    map->height++;
    map->data = realloc(map->data, sizeof(cell_t) * map->width * map->height);

    // Fill in the new row of the map
    int start_index = map->width * (map->height - 1);
    for (int i=0; i<map->width; i++) {
      if (line[i] == 'W') {
        map->data[start_index + i] = CELL_WALL;
      } else if (line[i] == '~') {
        map->data[start_index + i] = CELL_LAKE;
      } else if (line[i] == 'G') {
        map->data[start_index + i] = CELL_GRASS;
      } else if (line[i] == 'S') {
        // The start position must be a grass cell
        map->data[start_index + i] = CELL_GRASS;

        // Make sure we haven't already seen a start position
        if (map->start.x != -1 || map->start.y != -1) {
          fprintf(stderr, "Map specifies multiple start positions.\n");
          free(line);
          fclose(fp);
          return false;
        }

        // Save the start position
        map->start.x = i;
        map->start.y = map->height - 1;

      } else {
        fprintf(stderr, "Invalid character %c in map file\n", line[i]);
        free(line);
        fclose(fp);
        return false;
      }
    }
  }
  free(line);
  fclose(fp);
  
  // Make sure the map is at least 3x3
  if (map->width < 3 || map->height < 3) {
    fprintf(stderr, "Map must be at least 3x3.\n");
    return false;
  }

  // Make sure the map has a start position
  if (map->start.x == -1 || map->start.y == -1) {
    fprintf(stderr, "Map does not specify a start position.\n");
    return false;
  }

  return true;
}

// Free all the memory used to store a map
void free_map(map_t* map) {
  free(map->data);
  map->data = NULL;
}

// Display the map and player
void display_map(map_t map, coord_t player, coord_t* viewport) {
  // Get screen size
  coord_t screen_size;
  screen_size.x = getmaxx(stdscr);
  screen_size.y = getmaxy(stdscr);

  // Update the viewport to center the player
  viewport->x = player.x - screen_size.x / 4;
  viewport->y = player.y - screen_size.y / 2;

  // Avoid horizontal scrolling of the viewport when possible
  if (screen_size.x/2 >= map.width) {
    // The terminal is wide enough for the map, so center it
    viewport->x = -(screen_size.x/2 - map.width) / 2;

  } else if (viewport->x < 0) {
    // The player is near the left edge
    viewport->x = 0;
  
  } else if (viewport->x > map.width - screen_size.x/2) {
    // The player is near the right edge
    viewport->x = map.width - screen_size.x/2;
  }

  // Avoid vertical scrolling of the viewport when possible
  if (screen_size.y >= map.height) {
    // The terminal is tall enough for the map, so center it
    viewport->y = -(screen_size.y - map.height) / 2;

  } else if (viewport->y < 0) {
    // The player is near the top edge
    viewport->y = 0;

  } else if (viewport->y > map.height - screen_size.y + 1) {
    // The player is near the bottom edge
    viewport->y = map.height - screen_size.y + 1;
  }

  // Display the map
  for (int screen_y = 0; screen_y < screen_size.y-1; screen_y++) {
    for (int screen_x = 0; screen_x < screen_size.x-1; screen_x++) {
      int map_x = screen_x / 2 + viewport->x;
      int map_y = screen_y + viewport->y;
      
      int color = CELL_BLANK;
      if (map_x >= 0 && map_x < map.width && map_y >= 0 && map_y < map.height) {
        color = map.data[map_x + map_y * map.width];
      }

      attron(COLOR_PAIR(color));
      mvaddch(screen_y, screen_x, ' ');
      attroff(COLOR_PAIR(color));
    }
  }

  // Draw the player
  attron(COLOR_PAIR(map.data[player.x + player.y * map.width]));
  mvaddch(player.y - viewport->y, player.x*2 - viewport->x*2, '(');
  addch(')');
  attroff(COLOR_PAIR(map.data[player.x + player.y * map.width]));
}

// Check if a coordinate on a map is a valid place to move. Returns true if allowed.
bool can_move(map_t map, coord_t c) {
  // Is the x coordinate out of bounds?
  if (c.x < 0 || c.x >= map.width) {
    return false;
  }

  // Is the y coordinate out of bounds?
  if (c.y < 0 || c.y >= map.height) {
    return false;
  }

  // Is there a collision?
  if (map.data[c.x + c.y * map.width] != CELL_GRASS) {
    return false;
  }

  // Looks okay
  return true;
}
