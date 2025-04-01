#ifndef CHESS_LOGIC_H
#define CHESS_LOGIC_H

#define BOARD_SIZE 8

extern char board[BOARD_SIZE][BOARD_SIZE];

// set up board
void initialize_board();

// print current board state to socket
void print_board(int socket, char player);

// validate move
int is_valid_move(char from[2], char to[2], char player);

int move_piece(char from[2], char to[2], char player);

#endif // CHESS_LOGIC_H
