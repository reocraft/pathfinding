#include "movelist.h"

#include <stdlib.h>
#include <string.h>

int arrSize = 0;

// Initialize a movelist. The m parameter points to space that holds a movelist
void movelist_init(movelist_t* m) {
  arrSize = 0;
  m->input = malloc(sizeof(m->input) * arrSize);
}

// Clean up allocated memory in a movelist.
void movelist_destroy(movelist_t* m) {
  free(m->input);
}

// Peek at the value at the front of movelist m. Returns EMPTY if the list is empty
dir_t movelist_peek_front(const movelist_t* m) {
  if (arrSize == 0) {
    return EMPTY;
  } else {
    return m->input[arrSize - 1];
  }
}

// Take the value at the front of movelist m. Returns EMPTY if the list is empty
dir_t movelist_take_front(movelist_t* m) {
  if (arrSize == 0) {
    return EMPTY;
  } else {
    int top = m->input[arrSize - 1];
    m->input = realloc(m->input, sizeof(m->input) * (arrSize - 1));
    arrSize--;
    return top;
  }
  
}

// Peek at the value at the back of movelist m. Returns EMPTY if the list is empty
dir_t movelist_peek_back(const movelist_t* m) {
  if (arrSize == 0) {
    return EMPTY;
  } else {
    return m->input[0];
  }

}


// Take the value at the back of movelist m. Returns EMPTY if the list is empty
dir_t movelist_take_back(movelist_t* m) {
  if (arrSize == 0) {
    return EMPTY;
  } else {
    int back = m->input[0];
    for (int i = 0; i < arrSize - 1; i++) {
      m->input[i] = m->input[i+1];
    }
    m->input = realloc(m->input, sizeof(m->input) * (arrSize - 1));
    arrSize--;
    return back;
  }

}

// Add a move to the front of movelist m
void movelist_add_front(movelist_t* m, dir_t d) {
  m->input = realloc(m->input, sizeof(m->input) * (arrSize + 1));
  arrSize++;
  m->input[arrSize - 1] = d;
}

// Add a move to the back of movelist m
void movelist_add_back(movelist_t* m, dir_t d) {
  m->input = realloc(m->input, sizeof(m->input) * (arrSize + 1));
  arrSize++;
  for (int i = arrSize - 1; i >= 1; i--) {
    m->input[i] = m->input[i-1];
  }
  m->input[0] = d;
}

// Empty the movelist m
void movelist_clear(movelist_t* m) {
  arrSize = 0;
  m->input = realloc(m->input, 0);
}
