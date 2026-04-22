CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = programa

SRC = main.c \
      BigHashTable/MedellinCommunes.c \
      DataStructures/StringIntHashTable.c \
      Storage/PartitionManager.c \
      Storage/FileIndex.c \
      Storage/CommuneBlockManager.c \
      IO/FileReader.c \
      IO/FileWriter.c \
      IO/DirectoryManager.c \
      Data/IntRecordHashTable.c \
      Data/BlockLoader.c \
      Data/BlockWriter.c \
      GlobalIndex/GlobalRecordIndex.c \
      GlobalIndex/GlobalRecordIndexLoader.c \
      GlobalIndex/IdGenerator.c \
      Compression/CompressionAdapter.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c \
        IO/FileReader.h \
        IO/FileWriter.h \
        IO/DirectoryManager.h \
        Compression/CompressionAdapter.h \
        Data/Record.h \
        Data/IntRecordHashTable.h \
        Data/BlockLoader.h \
        Data/BlockWriter.h \
        GlobalIndex/GlobalRecordIndex.h \
        GlobalIndex/GlobalRecordIndexLoader.h \
        GlobalIndex/IdGenerator.h \
        Storage/FileIndex.h \
        Storage/CommuneBlockManager.h \
        BigHashTable/MedellinCommunes.h
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

Storage/CommuneBlockManager.o: Storage/CommuneBlockManager.c Storage/CommuneBlockManager.h
	$(CC) $(CFLAGS) -c Storage/CommuneBlockManager.c -o Storage/CommuneBlockManager.o

IO/FileReader.o: IO/FileReader.c IO/FileReader.h
	$(CC) $(CFLAGS) -c IO/FileReader.c -o IO/FileReader.o

IO/FileWriter.o: IO/FileWriter.c IO/FileWriter.h
	$(CC) $(CFLAGS) -c IO/FileWriter.c -o IO/FileWriter.o

IO/DirectoryManager.o: IO/DirectoryManager.c IO/DirectoryManager.h
	$(CC) $(CFLAGS) -c IO/DirectoryManager.c -o IO/DirectoryManager.o

Compression/CompressionAdapter.o: Compression/CompressionAdapter.c Compression/CompressionAdapter.h
	$(CC) $(CFLAGS) -c Compression/CompressionAdapter.c -o Compression/CompressionAdapter.o

Data/IntRecordHashTable.o: Data/IntRecordHashTable.c Data/IntRecordHashTable.h Data/Record.h
	$(CC) $(CFLAGS) -c Data/IntRecordHashTable.c -o Data/IntRecordHashTable.o

Data/BlockLoader.o: Data/BlockLoader.c Data/BlockLoader.h Data/IntRecordHashTable.h Data/Record.h
	$(CC) $(CFLAGS) -c Data/BlockLoader.c -o Data/BlockLoader.o

Data/BlockWriter.o: Data/BlockWriter.c Data/BlockWriter.h Data/IntRecordHashTable.h Data/Record.h
	$(CC) $(CFLAGS) -c Data/BlockWriter.c -o Data/BlockWriter.o

GlobalIndex/GlobalRecordIndex.o: GlobalIndex/GlobalRecordIndex.c GlobalIndex/GlobalRecordIndex.h
	$(CC) $(CFLAGS) -c GlobalIndex/GlobalRecordIndex.c -o GlobalIndex/GlobalRecordIndex.o

GlobalIndex/GlobalRecordIndexLoader.o: GlobalIndex/GlobalRecordIndexLoader.c \
                                       GlobalIndex/GlobalRecordIndexLoader.h \
                                       GlobalIndex/GlobalRecordIndex.h
	$(CC) $(CFLAGS) -c GlobalIndex/GlobalRecordIndexLoader.c -o GlobalIndex/GlobalRecordIndexLoader.o

GlobalIndex/IdGenerator.o: GlobalIndex/IdGenerator.c \
                           GlobalIndex/IdGenerator.h \
                           GlobalIndex/GlobalRecordIndex.h
	$(CC) $(CFLAGS) -c GlobalIndex/IdGenerator.c -o GlobalIndex/IdGenerator.o

clean:
	rm -f $(OBJ) $(TARGET)