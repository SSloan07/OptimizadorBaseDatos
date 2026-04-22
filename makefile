CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = programa

SRC = main.c BigHashTable/MedellinCommunes.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c BigHashTable/MedellinCommunes.h
	$(CC) $(CFLAGS) -c main.c

BigHashTable/MedellinCommunes.o: BigHashTable/MedellinCommunes.c BigHashTable/MedellinCommunes.h
	$(CC) $(CFLAGS) -c BigHashTable/MedellinCommunes.c -o BigHashTable/MedellinCommunes.o

clean:
	rm -f $(OBJ) $(TARGET)
