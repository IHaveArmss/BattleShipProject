# Makefile pentru Termux
# Compileaza serverul de Battleship folosind clang

CC = clang
CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE

server: server.c
	$(CC) $(CFLAGS) -o server server.c

clean:
	rm -f server

run: server
	./server
