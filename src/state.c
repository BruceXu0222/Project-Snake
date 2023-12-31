#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  game_state_t *game_state = (game_state_t*)malloc(sizeof(game_state_t));
  game_state->num_rows = 18;
  game_state->num_snakes = 1;

  game_state->board = (char**)malloc(sizeof(char*) * game_state->num_rows);
  for (int i = 0; i < game_state->num_rows; i++) {
    game_state->board[i] = (char*)malloc(sizeof(char)*21);
    game_state->board[i][20] = '\0';
  }
  char *a = "####################";
  char *b = "#                  #";
  strcpy(game_state->board[0], a);
  strcpy(game_state->board[17], a);
  for (int i = 1; i < 17; i++) {
    strcpy(game_state->board[i], b);
  }
  game_state->board[2][2] = 'd';
  game_state->board[2][3] = '>';
  game_state->board[2][4] = 'D';
  game_state->board[2][9] = '*';

  game_state->snakes = (snake_t*)malloc(sizeof(snake_t));
  game_state->snakes->tail_row = 2;
  game_state->snakes->tail_col = 2;
  game_state->snakes->head_row = 2;
  game_state->snakes->head_col = 4;
  game_state->snakes->live = true;

  return game_state;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i++) {
    fprintf(fp, "%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  return c == 'w' || c == 'a' || c == 's' || c == 'd';
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  return c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x';
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  return is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' || c == '>';
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  if (c == '^') {
    return 'w';
  }
  else if (c == '<') {
    return 'a';
  }
  else if (c == 'v') {
    return 's';
  }
  return 'd';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if (c == 'W') {
    return '^';
  }
  else if (c == 'A') {
    return '<';
  }
  else if (c == 'S') {
    return 'v';
  }
  return '>';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') {
    return cur_row + 1;
  }
  else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  }
  else if (c == '<' || c == 'a' || c == 'A') {
    return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int head_row = state->snakes[snum].head_row;
  unsigned int head_col = state->snakes[snum].head_col;
  char head = get_board_at(state, head_row, head_col);

  unsigned int next_row = get_next_row(head_row, head);
  unsigned int next_col = get_next_col(head_col, head);

  return get_board_at(state, next_row, next_col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int head_row = state->snakes[snum].head_row;
  unsigned int head_col = state->snakes[snum].head_col;
  char head = get_board_at(state, head_row, head_col);
  unsigned int next_row = get_next_row(head_row, head);
  unsigned int next_col = get_next_col(head_col, head);

  state->snakes[snum].head_row = next_row;
  state->snakes[snum].head_col = next_col;

  set_board_at(state, head_row, head_col, head_to_body(head));
  set_board_at(state, next_row, next_col, head);

  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int tail_row = state->snakes[snum].tail_row;
  unsigned int tail_col = state->snakes[snum].tail_col;
  char tail = get_board_at(state, tail_row, tail_col);
  unsigned int next_row = get_next_row(tail_row, tail);
  unsigned int next_col = get_next_col(tail_col, tail);
  char next_tail = get_board_at(state, next_row, next_col);

  state->snakes[snum].tail_row = next_row;
  state->snakes[snum].tail_col = next_col;

  set_board_at(state, tail_row, tail_col, ' ');
  set_board_at(state, next_row, next_col, body_to_tail(next_tail));

  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_snakes; i++) {
    if (state->snakes[i].live == false) {
      continue;
    }

    unsigned int head_row = state->snakes[i].head_row;
    unsigned int head_col = state->snakes[i].head_col;
    
    char next_sq = next_square(state, i);
    if (next_sq == '#' || is_snake(next_sq)) {
      state->snakes[i].live = false;
      set_board_at(state, head_row, head_col, 'x');
    }
    else if (next_sq == '*') {
      update_head(state, i);
      add_food(state);
    }
    else {
      update_head(state, i);
      update_tail(state, i);
    }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(FILE* fp) {
  // TODO: Implement this function.

  if (fp == NULL) {
    return NULL;
  }

  game_state_t* state = malloc(sizeof(game_state_t));
  state->board = malloc(sizeof(char*));

  char **buffer = malloc(sizeof(char*));
  unsigned int row = 0;
  unsigned int col = 0;
  char c;
  buffer[row] = malloc(sizeof(char));
  while ((c = fgetc(fp)) != EOF) {
    if (col >= (strlen(buffer[row]) - 1)) {
      buffer[row] = realloc(buffer[row], (sizeof(char) * (col+5)));
    }
    if (c == '\n') {
      buffer[row][col] = '\0';
      state->board[row] = malloc(sizeof(char) * (col+1));
      strcpy(state->board[row], buffer[row]);
      row++;
      col = 0;
      state->board = realloc(state->board, (sizeof(char*) * (row+1)));
      buffer[row] = malloc(sizeof(char));
      continue;
    }
    if (row >= ((sizeof(buffer) / sizeof(buffer[row])) - 1)) {
      buffer = realloc(buffer, sizeof(char*) * (row+5));
    }
    buffer[row][col] = c;
    col++;
  }
  state->num_rows = row;
  fclose(fp);
  state->num_snakes = 0;
  state->snakes = NULL;
  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  if (snum >= state->num_snakes) {
    return;
  }

  unsigned int row = state->snakes[snum].tail_row;
  unsigned int col = state->snakes[snum].tail_col;

  while (!is_head(get_board_at(state, row, col))) {
    row = get_next_row(row, get_board_at(state, row, col));
    if (is_head(get_board_at(state, row, col))) {
      break;
    }
    col = get_next_col(col, get_board_at(state, row, col));
  }

  state->snakes[snum].head_row = row;
  state->snakes[snum].head_col = col;

  return;
}


/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.

  unsigned int snake_num = 0;
  for (int i = 1; i < state->num_rows - 1; i++) {
    for (int j = 1; state->board[i][j] != '\0'; j++) {
      if (is_tail(state->board[i][j])) { 
        snake_num++;
      }
    }
  }
  state->num_snakes = snake_num;

  state->snakes = malloc(sizeof(snake_t) * snake_num);
  snake_num = 0;
  for (int i = 1; i < state->num_rows - 1; i++) {
    for (int j = 1; state->board[i][j] != '\0'; j++) {
      if (is_tail(state->board[i][j])) {
        state->snakes[snake_num].tail_row = i;
        state->snakes[snake_num].tail_col = j;
        state->snakes[snake_num].live = true;
        snake_num++;
      }
    }
  }

  for (int i = 0; i < snake_num; i++) {
    find_head(state, i);
  }
  
  return state;
}
