# Terminal Chess

This project implements a simple multiplayer chess game using a server-client architecture. The server handles game logic and player interactions, while clients connect to play the game.

## Compilation Instructions

Ensure you have `gcc` installed on your system. To compile the project, use the following command:

```sh
make
```

This will generate two executables:
- `chess_server`: The server that manages the game.
- `chess_client`: The client program that players use to connect to the server.

## Running the Server

Start the chess server by running:

```sh
./chess_server
```

The server should be started first to accept client connections.

## Running the Client

Once the server is running, you can start a client by executing:

```sh
./chess_client
```

You can start multiple clients in different terminals to simulate a multiplayer environment.

## Cleaning Up

To remove compiled files, use:

```sh
make clean
```

This will delete the compiled binaries and object files.

## Dependencies

- `gcc` (GNU Compiler Collection)
- `pthread` (POSIX Threads Library for multithreading support)

## Notes

- The game logic is implemented in `chess_logic.c`.
- Ensure the server is running before attempting to connect clients.

