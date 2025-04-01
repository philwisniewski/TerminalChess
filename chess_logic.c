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

void initialize_board() {
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; i < BOARD_SIZE; j++) {
      // already set, may implement later
    }
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

int is_valid_move(char from[2], char to[2], char player) {
  int from_row = from[1] - '1';
  int from_col = from[0] - 'a';
  int to_row = to[1] - '1';
  int to_col = to[0] - 'a';
  
  if (from_row == to_row && from_col == to_col) {
    return 0;
  }

  if (from_row < 0 || from_row >= BOARD_SIZE || from_col < 0 || from_col >= BOARD_SIZE ||
      to_row < 0 || to_row >= BOARD_SIZE || to_col < 0 || to_col >= BOARD_SIZE) {
    return 0;
  }

  char piece = board[from_row][from_col];
  char target = board[to_row][to_col];

  switch(tolower(piece)) {
    case 'r':
      // rook
      if (from_row != to_row && from_col != to_col) {
        return 0;
      }
      break;
    case 'k':
      // knight
      int row_dist = abs(from_row - to_row);
      int col_dist = abs(from_col - to_col);
      if (row_dist == 0 || col_dist == 0 || row_dist + col_dist != 3) {
        return 1;
      }
      break;
    case 'b':
      // bishop
      if (abs(to_row - from_row) != abs(to_col - from_col)) {
        return 1;
      }
      break;
    case 'q':
      // queen
      if ((from_row != to_row && from_col != to_col) && (abs(to_row - from_row) != abs(to_col - from_col))) {
        return 1;
      }
      break;
    case 'k':
      // king
      break;
    case 'p':
      // pawn
      break;
    default:
      // shouldn't happen
      return 0
  }

  if (piece == ' ' || (player == 'w' && islower(piece)) || (player == 'b' && isupper(piece))) {
    return 1;
  }

  // TODO: add checks for other pieces

  return 0;
}

int move_piece(char from[2], char to[2], char player) {
  int from_row = from[1] - '1';
  int from_col = from[0] - 'a';
  int to_row = to[1] - '1';
  int to_col = to[0] - 'a';

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

    return 1;  // Successful move
  }

  return 0;  // Invalid move
}


