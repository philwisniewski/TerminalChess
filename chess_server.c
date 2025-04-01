#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include "chess_logic.h"

#define SOCKET_PATH "/tmp/chess_socket"
#define MAX_PLAYERS 2

typedef struct {
  int socket;
  struct sockaddr_un address;
} Player;

Player lobby[MAX_PLAYERS];
int lobby_count = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *handle_game(void *arg) {
  Player *players = (Player *) arg;
  int p1 = players[0].socket;
  int p2 = players[1].socket;
  char buffer[1024];

  printf("Starting game\n");

  send(p1, "Game start! You are playing as White.\n", 38, 0);
  send(p2, "Game start! You are playing as Black.\n", 38, 0);

  // 0 = white, 1 = black
  int turn = 0;

  while(1) {
    int current = turn == 0 ? p1 : p2;
    int opponent = turn == 0 ? p2 : p1;


    printf("sending your move to %d\n", current);
    send(current, "Your move: \n", 12, 0);
    // must be 4 chars (ex: e2e4)
    memset(buffer, 0, sizeof(buffer));
    recv(current, buffer, sizeof(buffer), 0);
    printf("Buffer: %s\nBuffer size: %ld\n", buffer, strlen(buffer));
    if (strlen(buffer) != 4) {
      send(current, "Invalid move format. Please try again.\n", 38, 0);
      continue;
    }

    char from[2] = { buffer[0], buffer[1] };
    char to[2] = { buffer[2], buffer[3] };

    if (move_piece(from, to, turn == 0 ? 'w' : 'b')) {
      send(opponent, "Opponent's move: ", 17, 0);
      send(opponent, buffer, 4, 0);
      send(opponent, "\n", 1, 0);
      printf("turn = %d\n", turn);
      turn = 1 - turn;
      printf("turn = %d\n", turn);


    }
    else {
      send(current, "Invalid move. Try again.\n", 24, 0);
    }
  }

  close(p1);
  close(p2);
  free(players);
}

void *lobby_manager(void * arg) {
  int server_fd = *((int *) arg);
  struct sockaddr_un address;
  socklen_t addr_len = sizeof(address);

  while (1) {
    int client_socket = accept(server_fd, (struct sockaddr *) &address, &addr_len);
    if (client_socket < 0) {
      perror("Accept failed");
      continue;
    }

    pthread_mutex_lock(&lock);
    lobby[lobby_count].socket = client_socket;
    lobby[lobby_count].address = address;
    lobby_count++;

    printf("Lobby is now at %d / %d\n", lobby_count, MAX_PLAYERS);

    if (lobby_count >= 2) {
      Player *players = malloc(2 * sizeof(Player));
      players[0] = lobby[0];
      players[1] = lobby[1];

      lobby_count -= 2;
      memmove(lobby, lobby + 2, lobby_count * sizeof(Player));

      pthread_t game_thread;
      pthread_create(&game_thread, NULL, handle_game, (void *) players);
    }
    pthread_mutex_unlock(&lock);
  }
  return NULL;
}

int main() {
  int server_fd;
  struct sockaddr_un address;

  server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("Socket creation failed");
    exit(1);
  }
  address.sun_family = AF_LOCAL;
  strcpy(address.sun_path, SOCKET_PATH);

  unlink(SOCKET_PATH);
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Bind failed");
    exit(1);
  }

  listen(server_fd, MAX_PLAYERS);

  printf("Lobby server started. Waiting for players...\n");

  pthread_t lobby_thread;
  pthread_create(&lobby_thread, NULL, lobby_manager, (void *) &server_fd);
  pthread_join(lobby_thread, NULL);

  close(server_fd);
  return 0;
}


