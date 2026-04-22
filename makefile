CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = programa

SRC = main.c \
      BigHashTable/MedellinCommunes.c \
      DataStructures/StringIntHashTable.c \
      Storage/PartitionManager.c \
      Storage/FileIndex.c \
      IO/FileReader.c \
      Data/IntRecordHashTable.c \
      Data/BlockLoader.c \
      GlobalIndex/GlobalRecordIndex.c \
      GlobalIndex/GlobalRecordIndexLoader.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c \
        BigHashTable/MedellinCommunes.h \
        DataStructures/StringIntHashTable.h \
        Storage/PartitionManager.h \
        Storage/FileIndex.h \
        IO/FileReader.h \
        Data/Record.h \
        Data/IntRecordHashTable.h \
        Data/BlockLoader.h \
        GlobalIndex/GlobalRecordIndex.h \
        GlobalIndex/GlobalRecordIndexLoader.h
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

Data/IntRecordHashTable.o: Data/IntRecordHashTable.c Data/IntRecordHashTable.h Data/Record.h
	$(CC) $(CFLAGS) -c Data/IntRecordHashTable.c -o Data/IntRecordHashTable.o

Data/BlockLoader.o: Data/BlockLoader.c Data/BlockLoader.h Data/IntRecordHashTable.h Data/Record.h
	$(CC) $(CFLAGS) -c Data/BlockLoader.c -o Data/BlockLoader.o

GlobalIndex/GlobalRecordIndex.o: GlobalIndex/GlobalRecordIndex.c GlobalIndex/GlobalRecordIndex.h
	$(CC) $(CFLAGS) -c GlobalIndex/GlobalRecordIndex.c -o GlobalIndex/GlobalRecordIndex.o

GlobalIndex/GlobalRecordIndexLoader.o: GlobalIndex/GlobalRecordIndexLoader.c \
                                       GlobalIndex/GlobalRecordIndexLoader.h \
                                       GlobalIndex/GlobalRecordIndex.h
	$(CC) $(CFLAGS) -c GlobalIndex/GlobalRecordIndexLoader.c -o GlobalIndex/GlobalRecordIndexLoader.o

clean:
	rm -f $(OBJ) $(TARGET)