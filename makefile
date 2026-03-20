
CC = gcc
CFLAGS = -Wall -std=c99 -Wno-missing-braces


LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = battleship
SRC = main.c widget.c animations.c


$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)