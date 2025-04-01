CC = gcc
CFLAGS = -Wall -pthread -g

# Server executable
SERVER = chess_server
LOGIC = chess_logic.o

# Client executable
CLIENT = chess_client

# Define source files
SERVER_SRC = chess_server.c
LOGIC_SRC = chess_logic.c
CLIENT_SRC = chess_client.c

# Object files
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

# Default target to build the server
all: $(SERVER) $(CLIENT)

# Link the server and chess logic together
$(SERVER): $(SERVER_OBJ) $(LOGIC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJ) $(LOGIC)

$(CLIENT): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJ)

# Compile the logic object file
$(LOGIC): $(LOGIC_SRC)
	$(CC) $(CFLAGS) -c $(LOGIC_SRC)

# Clean up compiled files
clean:
	rm -f $(SERVER) $(SERVER_OBJ) $(LOGIC)

# Run the server
start_server: $(SERVER)
	./$(SERVER)

# Run the client
start_client:
	# Replace with the appropriate client command (could be a different file if you have a separate client)
	# For now, we assume the client connects to the server via the Unix socket
	echo "Start your client separately in another terminal!"

