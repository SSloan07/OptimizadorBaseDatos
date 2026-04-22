CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = programa

SRC = main.c \
      BigHashTable/MedellinCommunes.c \
      DataStructures/StringIntHashTable.c \
      Storage/PartitionManager.c \
      Storage/FileIndex.c \
      IO/FileReader.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c \
        BigHashTable/MedellinCommunes.h \
        Storage/PartitionManager.h \
        Storage/FileIndex.h \
        IO/FileReader.h \
        DataStructures/StringIntHashTable.h
	$(CC) $(CFLAGS) -c main.c

BigHashTable/MedellinCommunes.o: BigHashTable/MedellinCommunes.c \
                                 BigHashTable/MedellinCommunes.h \
                                 DataStructures/StringIntHashTable.h
	$(CC) $(CFLAGS) -c BigHashTable/MedellinCommunes.c -o BigHashTable/MedellinCommunes.o

DataStructures/StringIntHashTable.o: DataStructures/StringIntHashTable.c \
                                     DataStructures/StringIntHashTable.h
	$(CC) $(CFLAGS) -c DataStructures/StringIntHashTable.c -o DataStructures/StringIntHashTable.o

Storage/PartitionManager.o: Storage/PartitionManager.c Storage/PartitionManager.h
	$(CC) $(CFLAGS) -c Storage/PartitionManager.c -o Storage/PartitionManager.o

Storage/FileIndex.o: Storage/FileIndex.c Storage/FileIndex.h
	$(CC) $(CFLAGS) -c Storage/FileIndex.c -o Storage/FileIndex.o

IO/FileReader.o: IO/FileReader.c IO/FileReader.h
	$(CC) $(CFLAGS) -c IO/FileReader.c -o IO/FileReader.o

clean:
	rm -f $(OBJ) $(TARGET)
