#ifndef STRING_INT_HASH_TABLE_H
#define STRING_INT_HASH_TABLE_H

typedef struct HashNode {
    char key[50];
    int value;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode **buckets;
    int size;
} StringIntHashTable;

StringIntHashTable *create_string_int_hash_table(int size);
void insert_string_int(StringIntHashTable *table, const char *key, int value);
int search_string_int(StringIntHashTable *table, const char *key);
void free_string_int_hash_table(StringIntHashTable *table);

#endif