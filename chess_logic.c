#include "chess_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BOARD_SIZE 8

char board[BOARD_SIZE][BOARD_SIZE] = {
  {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
  {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
  {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
  {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
};

// Castling tracking variables
int king_moved[2] = {0, 0};  // 0 for white, 1 for black
int left_rook_moved[2] = {0, 0};  // 0 for white, 1 for black
int right_rook_moved[2] = {0, 0};  // 0 for white, 1 for black

void initialize_board() {
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; i < BOARD_SIZE; j++) {
      // already set, may implement later
    }
  }
}

void promote_pawn(int row, int col) {
  if (tolower(board[row][col]) != 'p') {
    return;
  }
  if (row == 0 || row == BOARD_SIZE - 1) {
    board[row][col] = (board[row][col] == 'p' ? 'q' : 'Q'); // Promote to queen
  }
}

void print_board(int socket, char player) {
  const char * letters = "abcdefghijklmnopqrstuvwxyz";
  char board_str[1024] = "";
  if (player == 'w') {
    // white
    for (int i = BOARD_SIZE - 1; i >= 0; i--) {
      // rank identifier
      snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str)
, "%d | ", i + 1);

      // populate row with pieces
      for (int j = 0; j < BOARD_SIZE; j++) {
        char piece = board[i][j];
        if (piece == ' ') {
          piece = '.';
        }
        snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str)
, "%c ", piece);
      }
      snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str)
, "\n");
    }

    // file identifier
    snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str), "  |-----------------\n    ");
    for (int j = 0; j < BOARD_SIZE; j++) {
      snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str), "%c ", letters[j]);
    }
    snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str), "\n");
  }
  else {
    // black
    for (int i = 0; i < BOARD_SIZE; i++) {
      // rank identifier
      snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str), "%d | ", i + 1);

      // place pieces in row
      for (int j = BOARD_SIZE - 1; j >= 0; j--) {
        char piece = board[i][j];
        if (piece == ' ') {
          piece = '.';
        }
        snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str), "%c ", piece);
      }
      snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str), "\n");
    }
    // letter positions
    snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str), "  |-----------------\n    ");

    // file identifier
    for (int j = BOARD_SIZE - 1; j >= 0; j--) {
      printf("%d\n", j);
      snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str), "%c ", letters[j]);
    }

    snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str), "\n");
  }
  printf("sending to socket\n");
  send(socket, board_str, strlen(board_str), 0);
  printf("sent to socket\n");
}

/*
 * is_valid_move
 * checks if move is legal
 */
int is_valid_move(char from[2], char to[2], char player) {
  int from_row = from[1] - '1';
  int from_col = from[0] - 'a';
  int to_row = to[1] - '1';
  int to_col = to[0] - 'a';

  // Basic bounds check
  if (from_row == to_row && from_col == to_col) {
    return 0;  // Can't move to the same position
  }
  if (from_row < 0 || from_row >= BOARD_SIZE || from_col < 0 || from_col >= BOARD_SIZE ||
      to_row < 0 || to_row >= BOARD_SIZE || to_col < 0 || to_col >= BOARD_SIZE) {
    return 0;  // Out of bounds
  }

  char piece = board[from_row][from_col];
  char target = board[to_row][to_col];

  // Check for valid piece
  if (piece == ' ' || (player == 'b' && islower(piece)) || (player == 'w' && isupper(piece))) {
    return 0;  // Empty square or wrong player's piece
  }

  if (target != ' ' && (player == 'w' && islower(target))) {
    return 0; // trying to capture own team
  }

  // Rook movement (horizontal/vertical)
  if (tolower(piece) == 'r') {
    if (from_row != to_row && from_col != to_col) {
      return 0;  // Must move straight
    }
    // Check for blocking pieces
    if (from_row == to_row) {  // Moving horizontally
      int step = (to_col > from_col) ? 1 : -1;
      for (int col = from_col + step; col != to_col; col += step) {
        if (board[from_row][col] != ' ') return 0;  // Blocked
      }
    } else {  // Moving vertically
      int step = (to_row > from_row) ? 1 : -1;
      for (int row = from_row + step; row != to_row; row += step) {
        if (board[row][from_col] != ' ') return 0;  // Blocked
      }
    }
  }
  // Bishop movement (diagonal)
  else if (tolower(piece) == 'b') {
    if (abs(to_row - from_row) != abs(to_col - from_col)) {
      return 0;  // Must move diagonally
    }
    int row_step = (to_row > from_row) ? 1 : -1;
    int col_step = (to_col > from_col) ? 1 : -1;
    int row = from_row + row_step, col = from_col + col_step;
    while (row != to_row && col != to_col) {
      if (board[row][col] != ' ') return 0;  // Blocked
      row += row_step;
      col += col_step;
    }
  }
  // Queen movement (rook or bishop logic)
  else if (tolower(piece) == 'q') {
    if (from_row == to_row || from_col == to_col) {
      // Rook-like movement
      if (from_row == to_row) {  // Moving horizontally
        int step = (to_col > from_col) ? 1 : -1;
        for (int col = from_col + step; col != to_col; col += step) {
          if (board[from_row][col] != ' ') return 0;  // Blocked
        }
      } else {  // Moving vertically
        int step = (to_row > from_row) ? 1 : -1;
        for (int row = from_row + step; row != to_row; row += step) {
          if (board[row][from_col] != ' ') return 0;  // Blocked
        }
      }
    } else if (abs(to_row - from_row) == abs(to_col - from_col)) {
      // Bishop-like movement
      int row_step = (to_row > from_row) ? 1 : -1;
      int col_step = (to_col > from_col) ? 1 : -1;
      int row = from_row + row_step, col = from_col + col_step;
      while (row != to_row && col != to_col) {
        if (board[row][col] != ' ') return 0;  // Blocked
        row += row_step;
        col += col_step;
      }
    } else {
      return 0;  // Invalid queen move
    }
  }
  // Knight movement (L-shape) - no need to check for obstacles
  else if (tolower(piece) == 'n') {
    int row_dist = abs(from_row - to_row);
    int col_dist = abs(from_col - to_col);
    if (!(row_dist == 2 && col_dist == 1) && !(row_dist == 1 && col_dist == 2)) {
      return 0;  // Invalid knight move
    }
  }
  // King movement (one step, except for castling)
  else if (tolower(piece) == 'k') {
    if (abs(to_row - from_row) > 1 || abs(to_col - from_col) > 1) {
      return 0;  // King moves only one square
    }
  }
  // Pawn movement
  else if (tolower(piece) == 'p') {
    int direction = (isupper(piece)) ? -1 : 1;  // White moves up, Black moves down
    if (from_col == to_col && target == ' ' && ((from_row + direction == to_row) || 
        (from_row == (direction == 1 ? 1 : 6) && from_row + 2 * direction == to_row))) {
      return 1;  // Regular move or initial 2-square move
    }
    // Pawn capture (diagonal)
    else if (abs(from_col - to_col) == 1 && to_row == from_row + direction &&
             (islower(target) && player == 'b' || isupper(target) && player == 'w')) {
      return 1;  // Capture opponent’s piece
    }
    return 0;  // Invalid pawn move
  }

  return 1;  // Valid move
}

int is_legal_castle(char from[2], char to[2], char player) {
  int from_row = from[1] - '1';
  int from_col = from[0] - 'a';
  int to_row = to[1] - '1';
  int to_col = to[0] - 'a';

  if (abs(from_col - to_col) != 2 || from_row != to_row) {
    return 0;
  }
  if (player == 'w') {
    // white
    char piece = board[from_row][from_col];
    if (piece == 'k' && king_moved[0] == 0) {
      if (to_col == 6 && right_rook_moved[0] == 0 && board[0][5] == ' ' && board[0][6] == ' ') {
        return 1;
      }
      else if (to_col == 2 && left_rook_moved[0] == 0 && board[0][1] == ' ' && board[0][2] == ' ' && board[0][3] == ' ') {
        return 1;
      }
    }
  }
  else if (player == 'b') {
    // black
    char piece = board[from_row][from_col];
    if (piece == 'K' && king_moved[1] == 0) {
      if (to_col == 6 && right_rook_moved[1] == 0 && board[7][5] == ' ' && board[7][6] == ' ') {
        return 1;
      }
      else if (to_col == 2 && left_rook_moved[1] == 0 && board[7][1] == ' ' && board[7][2] == ' ' && board[7][3] == ' ') {
        return 1;
      }
    }
  }
  // illegal
  return 0;
}

int king_gone() {
  int white_king_found = 1;
  int black_king_found = 1;
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      if (board[i][j] == 'k') {
        white_king_found = 0;
      }
      else if (board[i][j] == 'K') {
        black_king_found = 0;
      }
    }
  }
  return white_king_found | black_king_found;
}

int move_piece(char from[2], char to[2], char player) {
  int from_row = from[1] - '1';
  int from_col = from[0] - 'a';
  int to_row = to[1] - '1';
  int to_col = to[0] - 'a';

  if (is_legal_castle(from, to, player)) {
    // successful castle
    if (player == 'w') {
      // player is white
      if (to_col == 6) {
        board[0][6] = 'k';
        board[0][5] = 'r';
        board[0][7] = ' ';
        board[0][4] = ' ';
        king_moved[0] = 1;
      }
      else if (to_col == 2) {
        board[0][2] = 'k';
        board[0][3] = 'r';
        board[0][0] = ' ';
        board[0][4] = ' ';
        king_moved[0] = 1;
      }
    }
    else if (player == 'b') {
      // player is black
      if (to_col == 6) { 
        board[7][6] = 'k';
        board[7][5] = 'r';
        board[7][4] = ' ';
        board[7][7] = ' ';
      } else if (to_col == 2) { // Black queenside castling (O-O-O)
        board[7][2] = 'k';
        board[7][3] = 'r';
        board[7][4] = ' ';
        board[7][0] = ' ';
        king_moved[1] = 1;
      }
    }
    return 1;
  }
  if (is_valid_move(from, to, player)) {
    board[to_row][to_col] = board[from_row][from_col];  // Move piece
    board[from_row][from_col] = ' ';  // Clear the original position
    promote_pawn(to_row, to_col);  // Promote pawn if it reaches the opposite end

    // Update movement flags for king and rook (castling)
    if (board[to_row][to_col] == 'k' || board[to_row][to_col] == 'K') {
      king_moved[player == 'w' ? 0 : 1] = 1;  // King has moved
    }
    if (board[to_row][to_col] == 'r' || board[to_row][to_col] == 'R') {
      if (from_col == 0) {
        left_rook_moved[player == 'w' ? 0 : 1] = 1;  // Left rook has moved
      } else if (from_col == 7) {
        right_rook_moved[player == 'w' ? 0 : 1] = 1;  // Right rook has moved
      }
    }

    if (king_gone()) {
      printf("king taken\n");
      return 2; // end of game
    }

    return 1;  // Successful move
  }

  return 0;  // Invalid move
}


