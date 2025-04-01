#include "chess_logic.h"
#include <stdio.h>
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
      snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str)
, "%d | ", i + 1);

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

    // letter positions
    snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str), "  |-----------------\n    ");
    for (int j = 0; j < BOARD_SIZE; j++) {
      snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str), "%c ", letters[j]);
    }
    snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str), "\n");
  }
  else {
    // black
    for (int i = 0; i < BOARD_SIZE; i++) {
      snprintf(board_str + strlen(board_str), sizeof(board_str) - strlen(board_str), "%d | ", i + 1);

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

  if (from_row < 0 || from_row >= BOARD_SIZE || from_col < 0 || from_col >= BOARD_SIZE ||
      to_row < 0 || to_row >= BOARD_SIZE || to_col < 0 || to_col >= BOARD_SIZE) {
    return 0;
  }

  char piece = board[from_row][from_col];
  char target = board[to_row][to_col];

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

  printf("from %d, %d to %d, %d\n", from_row, from_col, to_row, to_col);
  // Basic move validation
  if (is_valid_move(from, to, player)) {
    // Move the piece on the board
    board[to_row][to_col] = board[from_row][from_col];
    board[from_row][from_col] = ' ';
    return 1; // Successful move
  }

  return 0; // Invalid move
}


