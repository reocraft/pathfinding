#include <ncurses.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "map.h"
#include "movelist.h"
#include "util.h"

#define FRAME_RATE 30

// Set the player move speed in steps per second
#define MOVE_SPEED 10

// Find a path from the player position to dest and add the necessary moves to the movelist
/*
void plan_route(map_t map, coord_t player, coord_t dest, movelist_t* moves) {
  // Generate all moves required to get the player to the destination
  while (dest.x != player.x || dest.y != player.y) {
    // Does the player need to move on the x axis?
    if (dest.x != player.x) {
      // Yes. Add a left/right move
      if (dest.x > player.x) {
        movelist_add_front(moves, DIR_E);
        dest.x--;
      } else {
        movelist_add_front(moves, DIR_W);
        dest.x++;
      }
    }

    // Does the player need to move on the y axis?
    if (dest.y != player.y) {
      // Yes. Add an up/down move
      if (dest.y > player.y) {
        movelist_add_front(moves, DIR_S);
        dest.y--;
      } else {
        movelist_add_front(moves, DIR_N);
        dest.y++;
      }
    }
  }
} */

void plan_route(map_t map, coord_t player, coord_t dest, movelist_t* moves) {
  // Create 2D array of grid with distance values, where 65536 indicates an unexplored grid.
  int trail[map.height][map.width];
  for (int i = 0; i < map.height; i++) {
    for (int j = 0; j < map.width; j++) {
      trail[i][j] = 65536;
    }
  }
  bool notFound = true;
  // Sets starting point's distance to 0.
  trail[player.y][player.x] = 0;
  while (notFound) {
    // Goes through every grid and searches for a grid that has a distance (excluding 65536)
    // and checks the cardinally adjacent grids, and updates that with the distance
    // value of the adjacent block it referenced from and adds 1 to it.
    for (int i = 0; i < map.height && notFound; i++) {
      for (int j = 0; j < map.width; j++) {
        coord_t pos = {j, i};
        if (can_move(map, pos) && trail[i][j] < 65535) {
          // Updates grid above the selected grid.
          pos.x = j;
          pos.y = i-1;
          if (can_move(map, pos) && trail[pos.y][pos.x] > trail[i][j]) {
            trail[pos.y][pos.x] = trail[i][j]+1;
            if (pos.x == dest.x && pos.y == dest.y) {
              notFound = false;
              break;
            }
          }

          // Updates grid below the selected grid.
          pos.y = i+1;
          if (can_move(map, pos) && trail[pos.y][pos.x] > trail[i][j]) {
            trail[pos.y][pos.x] = trail[i][j]+1;
            if (pos.x == dest.x && pos.y == dest.y) {
              notFound = false;
              break;
            }
          }

          // Updates grid left of the selected grid.
          pos.x = j-1;
          pos.y = i;
          if (can_move(map, pos) && trail[pos.y][pos.x] > trail[i][j]) {
            trail[pos.y][pos.x] = trail[i][j]+1;
            if (pos.x == dest.x && pos.y == dest.y) {
              notFound = false;
              break;
            }
          }

          // Updates grid right of the selected grid.
          pos.x = j+1;
          if (can_move(map, pos) && trail[pos.y][pos.x] > trail[i][j]) {
            trail[pos.y][pos.x] = trail[i][j]+1;
            if (pos.x == dest.x && pos.y == dest.y) {
              notFound = false;
              break;
            }
          }
        }
      }
    }
  }


  while (trail[dest.y][dest.x] != 0) {
    // Currently found the location and we want to use the `distance` value to follow the trail where the value
    // gets smaller and go back to where it is 0. When we find the path to go to, add the move to queue.
    
    // When the grid on the left has a smaller value (indicating that distance is shorter)
    if ((dest.x)-1 >= 0 && trail[dest.y][dest.x] > trail[dest.y][(dest.x)-1]) {
      movelist_add_front(moves, DIR_E);
      dest.x--;
    }
    
    // When the grid on the right has a smaller value
    else if ((dest.x)+1 < map.width && trail[dest.y][dest.x] > trail[dest.y][(dest.x)+1]) {
      movelist_add_front(moves, DIR_W);
      dest.x++;
    }

    // When the grid above has a smaller value
    else if ((dest.y)-1 >= 0 && trail[dest.y][dest.x] > trail[(dest.y)-1][dest.x]) {
      movelist_add_front(moves, DIR_S);
      dest.y--;
    }

    // When the grid below has a smaller value
    else if ((dest.y)+1 < map.height && trail[dest.y][dest.x] > trail[(dest.y)+1][dest.x]) {
      movelist_add_front(moves, DIR_N);
      dest.y++;
    }
  }
} 

int main(int argc, char** argv) {
  // Check for an argument
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <map file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Load a map
  map_t map;
  if (!load_map(argv[1], &map)) {
    fprintf(stderr, "Failed to load map %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  // Set up ncurses
  initscr();              // Initialize the standard window
  noecho();               // Don't display keys when they are typed
  nodelay(stdscr, true);  // Set up non-blocking input with getch()
  keypad(stdscr, true);   // Allow arrow keys
  curs_set(false);        // Hide the cursor

  // Accept mouse clicks
  mousemask(BUTTON1_CLICKED, NULL);

  // Set up colors for each cell type in the mapp
  start_color();
  init_pair(CELL_BLANK, COLOR_WHITE, COLOR_BLACK);
  init_pair(CELL_GRASS, COLOR_BLACK, COLOR_GREEN);
  init_pair(CELL_WALL, COLOR_BLACK, COLOR_WHITE);
  init_pair(CELL_LAKE, COLOR_BLACK, COLOR_BLUE);

  // Keep track of the player position
  coord_t player = map.start;

  // Keep track of where the current view is
  coord_t viewport = {0, 0};

  // Keep a list of moves
  movelist_t moves;
  movelist_init(&moves);

  // Keep track of how many frames the player must wait before moving again
  size_t frames_until_move = 0;



  // Start the game loop
  bool running = true;
  while (running) {
    // Check for user input
    int input;
    while ((input = getch()) != ERR) {
      if (input == 'q') {
        // Exit when the user types q
        running = false;
      
      } else if (input == KEY_UP) {
        // Clear the move queue and add one move north
        movelist_clear(&moves);
        movelist_add_front(&moves, DIR_N);
      
      } else if (input == KEY_DOWN) {
        // Clear the move queue and add one move south
        movelist_clear(&moves);
        movelist_add_front(&moves, DIR_S);
      
      } else if (input == KEY_LEFT) {
        // Clear the move queue and add one move west
        movelist_clear(&moves);
        movelist_add_front(&moves, DIR_W);

      } else if (input == KEY_RIGHT) {
        // Clear the move queue and add one move east
        movelist_clear(&moves);
        movelist_add_front(&moves, DIR_E);

      } else if (input == KEY_MOUSE) {
        // A mouse click was detected. Get the coordinates
        MEVENT m_event;
        if (getmouse(&m_event) == OK) {
          // Clear the movelist
          movelist_clear(&moves);

          // Calculate the destination the player should move to
          coord_t dest;
          dest.x = m_event.x / 2 + viewport.x;
          dest.y = m_event.y + viewport.y;

          // Plan a route to the destination
          plan_route(map, player, dest, &moves);
        }
      }
    }

    // Is it time to move?
    if (frames_until_move == 0) {
      // Yes. Grab a move if we have one
      dir_t move = movelist_take_front(&moves);
      if (move != EMPTY) {
        // Calculate the next player position
        coord_t next = player;
        if (move == DIR_N) {
          next.y--;
        } else if (move == DIR_E) {
          next.x++;
        } else if (move == DIR_S) {
          next.y++;
        } else if (move == DIR_W) {
          next.x--;
        }

        // Can the player move to the new position?
        if (can_move(map, next)) {
          // Yes. Update the player position.
          player = next;

          // Set a delay before the next move
          frames_until_move = FRAME_RATE / MOVE_SPEED;
        
        } else {
          // Clear the whole movelist when there's a collision
          movelist_clear(&moves);
        }
      }

    } else {
      // No. Decrement the frame count
      frames_until_move--;
    }

    // Display the map and player
    display_map(map, player, &viewport);

    // Display everything
    refresh();

    // Pause to limit frame rate
    sleep_ms(1000 / FRAME_RATE);
  }

  // Shut down
  endwin();

  // Clean up the movelist
  movelist_destroy(&moves);

  // Clean up the map
  free_map(&map);
}
