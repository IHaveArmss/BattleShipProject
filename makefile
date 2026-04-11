
CC = gcc
CFLAGS = -Wall -std=c99 -Wno-missing-braces

LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = battleship
SRC = main.c scripts/widget.c scripts/animations.c scripts/gameLogic.c scripts/socketLogic/socketBA.c

SERVER_TARGET = server
SERVER_SRC = scripts/socketLogic/server.c
CC_SERVER = gcc

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

$(SERVER_TARGET): $(SERVER_SRC)
	$(CC_SERVER) $(CFLAGS) -o $(SERVER_TARGET) $(SERVER_SRC)

all: $(TARGET) $(SERVER_TARGET)

clean:
	rm -f $(TARGET) $(SERVER_TARGET)

run: $(TARGET)
	./$(TARGET)

run-server: $(SERVER_TARGET)
	./$(SERVER_TARGET)