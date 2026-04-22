CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = programa

SRC = main.c \
      BigHashTable/MedellinCommunes.c \
      Storage/PartitionManager.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c BigHashTable/MedellinCommunes.h Storage/PartitionManager.h
	$(CC) $(CFLAGS) -c main.c

BigHashTable/MedellinCommunes.o: BigHashTable/MedellinCommunes.c BigHashTable/MedellinCommunes.h
	$(CC) $(CFLAGS) -c BigHashTable/MedellinCommunes.c -o BigHashTable/MedellinCommunes.o

Storage/PartitionManager.o: Storage/PartitionManager.c Storage/PartitionManager.h
	$(CC) $(CFLAGS) -c Storage/PartitionManager.c -o Storage/PartitionManager.o

clean:
	rm -f $(OBJ) $(TARGET)
