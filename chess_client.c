#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/chess_socket"

int main() {
  int sock;
  struct sockaddr_un serv_addr;
  char buffer[1024] = "";

  sock = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("Socket creation failed");
    exit(1);
  }
  serv_addr.sun_family = AF_LOCAL;
  strcpy(serv_addr.sun_path, SOCKET_PATH);

  if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("Connection failed");
    return 1;
  }

  printf("Connected! Waiting for an opponent...\n");

  while (1) {
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_read = read(sock, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
      perror("Server disconnected or read failed");
      break;
    }
    buffer[bytes_read] = '\0';

    printf("Buff: %s", buffer);

  //#  printf("Read %ld but buffer is len %ld\n", bytes_read, strlen(buffer));

    if (strstr(buffer, "Your move")) {
      char move[5];
      // fflush(stdout);
      if (scanf("%4s", move) <= 0) {
        perror("Failed to read move");
        break;
      }
      move[strcspn(move, "\n")] = 0;
      printf("made move = %s\n", move);

      send(sock, move, strlen(move), 0);
    }
  }

  close(sock);

  return 0;

}
