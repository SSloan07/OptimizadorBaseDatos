#ifndef GLOBAL_RECORD_INDEX_H
#define GLOBAL_RECORD_INDEX_H

typedef struct {
    int id_registro;
    int id_comuna;
    int id_bloque;
} RecordLocation;

typedef struct GlobalIndexNode {
    int key;
    RecordLocation value;
    struct GlobalIndexNode *next;
} GlobalIndexNode;

typedef struct {
    GlobalIndexNode **buckets;
    int size;
} GlobalRecordIndex;

GlobalRecordIndex *create_global_record_index(int size);
void insert_global_record_index(GlobalRecordIndex *index, int key, RecordLocation value);
RecordLocation *search_global_record_index(GlobalRecordIndex *index, int key);
void free_global_record_index(GlobalRecordIndex *index);

#endif