CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDLIBS = -lcrypto
TARGET = programa

SRC = main.c \
      App/AppContext.c \
      App/Menu.c \
      App/SearchService.c \
      App/InsertService.c \
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
      GlobalIndex/GlobalIndexPersistence.c \
      GlobalIndex/IdGenerator.c \
      Compression/CompressionAdapter.c \
      Encryption/Encripter.c \
      AES/AesEncripter.c \
      Synthetic/SyntheticGenerator.c \
      Synthetic/SyntheticBatchInsert.c \
      Synthetic/BatchGroup.c \
      Synthetic/BatchProcessor.c \
      LZW/compress.c \
      LZW/decompress.c \
      LZW/dictionary.c \
      App/BenchmarkUtils.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDLIBS)

main.o: main.c App/Menu.h App/SearchService.h App/InsertService.h App/AppContext.h
	$(CC) $(CFLAGS) -c main.c

App/AppContext.o: App/AppContext.c App/AppContext.h \
                  GlobalIndex/GlobalRecordIndex.h \
                  GlobalIndex/GlobalIndexPersistence.h
	$(CC) $(CFLAGS) -c App/AppContext.c -o App/AppContext.o

App/Menu.o: App/Menu.c App/Menu.h
	$(CC) $(CFLAGS) -c App/Menu.c -o App/Menu.o

App/SearchService.o: App/SearchService.c App/SearchService.h \
                     App/AppContext.h \
                     GlobalIndex/GlobalRecordIndex.h \
                     Storage/FileIndex.h \
                     IO/FileReader.h \
                     Compression/CompressionAdapter.h \
                     Data/BlockLoader.h \
                     Data/IntRecordHashTable.h \
                     Data/Record.h \
                     Encryption/Encripter.h \
                     AES/AesEncripter.h
	$(CC) $(CFLAGS) -c App/SearchService.c -o App/SearchService.o

App/InsertService.o: App/InsertService.c App/InsertService.h \
                     App/AppContext.h \
                     Synthetic/SyntheticBatchInsert.h \
                     GlobalIndex/GlobalIndexPersistence.h \
                     GlobalIndex/IdGenerator.h \
                     Storage/FileIndex.h \
                     Storage/CommuneBlockManager.h \
                     BigHashTable/MedellinCommunes.h \
                     IO/FileReader.h \
                     IO/FileWriter.h \
                     IO/DirectoryManager.h \
                     Compression/CompressionAdapter.h \
                     Data/BlockLoader.h \
                     Data/IntRecordHashTable.h \
                     Data/BlockWriter.h \
                     Data/Record.h \
                     Encryption/Encripter.h \
                     AES/AesEncripter.h
	$(CC) $(CFLAGS) -c App/InsertService.c -o App/InsertService.o

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

Encryption/Encripter.o: Encryption/Encripter.c Encryption/Encripter.h
	$(CC) $(CFLAGS) -c Encryption/Encripter.c -o Encryption/Encripter.o

AES/AesEncripter.o: AES/AesEncripter.c AES/AesEncripter.h Encryption/Encripter.h
	$(CC) $(CFLAGS) -c AES/AesEncripter.c -o AES/AesEncripter.o

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

GlobalIndex/GlobalIndexPersistence.o: GlobalIndex/GlobalIndexPersistence.c \
                                      GlobalIndex/GlobalIndexPersistence.h \
                                      GlobalIndex/GlobalRecordIndex.h
	$(CC) $(CFLAGS) -c GlobalIndex/GlobalIndexPersistence.c -o GlobalIndex/GlobalIndexPersistence.o

GlobalIndex/IdGenerator.o: GlobalIndex/IdGenerator.c \
                           GlobalIndex/IdGenerator.h \
                           GlobalIndex/GlobalRecordIndex.h
	$(CC) $(CFLAGS) -c GlobalIndex/IdGenerator.c -o GlobalIndex/IdGenerator.o

Synthetic/SyntheticGenerator.o: Synthetic/SyntheticGenerator.c \
                                Synthetic/SyntheticGenerator.h \
                                Data/Record.h
	$(CC) $(CFLAGS) -c Synthetic/SyntheticGenerator.c -o Synthetic/SyntheticGenerator.o

Synthetic/BatchGroup.o: Synthetic/BatchGroup.c \
                        Synthetic/BatchGroup.h \
                        Data/Record.h
	$(CC) $(CFLAGS) -c Synthetic/BatchGroup.c -o Synthetic/BatchGroup.o

Synthetic/BatchProcessor.o: Synthetic/BatchProcessor.c \
                            Synthetic/BatchProcessor.h \
                            Synthetic/BatchGroup.h \
                            Storage/FileIndex.h \
                            IO/FileReader.h \
                            IO/FileWriter.h \
                            IO/DirectoryManager.h \
                            Compression/CompressionAdapter.h \
                            Data/BlockLoader.h \
                            Data/IntRecordHashTable.h \
                            Data/BlockWriter.h \
                            Data/Record.h
	$(CC) $(CFLAGS) -c Synthetic/BatchProcessor.c -o Synthetic/BatchProcessor.o

Synthetic/SyntheticBatchInsert.o: Synthetic/SyntheticBatchInsert.c \
                                  Synthetic/SyntheticBatchInsert.h \
                                  Synthetic/SyntheticGenerator.h \
                                  Synthetic/BatchGroup.h \
                                  Synthetic/BatchProcessor.h \
                                  App/AppContext.h \
                                  GlobalIndex/GlobalRecordIndex.h \
                                  GlobalIndex/GlobalIndexPersistence.h \
                                  GlobalIndex/IdGenerator.h \
                                  Storage/CommuneBlockManager.h \
                                  BigHashTable/MedellinCommunes.h \
                                  Data/Record.h
	$(CC) $(CFLAGS) -c Synthetic/SyntheticBatchInsert.c -o Synthetic/SyntheticBatchInsert.o

LZW/compress.o: LZW/compress.c LZW/compress.h LZW/dictionary.h
	$(CC) $(CFLAGS) -c LZW/compress.c -o LZW/compress.o

LZW/decompress.o: LZW/decompress.c LZW/decompress.h LZW/dictionary.h
	$(CC) $(CFLAGS) -c LZW/decompress.c -o LZW/decompress.o

LZW/dictionary.o: LZW/dictionary.c LZW/dictionary.h
	$(CC) $(CFLAGS) -c LZW/dictionary.c -o LZW/dictionary.o

clean:
	rm -f $(OBJ) $(TARGET)