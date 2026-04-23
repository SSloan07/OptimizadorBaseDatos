#ifndef INT_RECORD_HASH_TABLE_H
#define INT_RECORD_HASH_TABLE_H

#include "Record.h"

typedef struct IntRecordNode {
    int key;
    Record value;
    struct IntRecordNode *next;
} IntRecordNode;

typedef struct {
    IntRecordNode **buckets;
    int size;
} IntRecordHashTable;

IntRecordHashTable *create_int_record_hash_table(int size);
void insert_int_record(IntRecordHashTable *table, int key, Record value);
Record *search_int_record(IntRecordHashTable *table, int key);
void free_int_record_hash_table(IntRecordHashTable *table);

#endif