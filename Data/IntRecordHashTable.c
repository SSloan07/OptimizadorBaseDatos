#include <stdio.h>
#include <stdlib.h>
#include "IntRecordHashTable.h"

static unsigned int hash_function(int key, int table_size) {
    if (key < 0) {
        key = -key;
    }
    return (unsigned int)key % (unsigned int)table_size;
}

IntRecordHashTable *create_int_record_hash_table(int size) {
    IntRecordHashTable *table = (IntRecordHashTable *)malloc(sizeof(IntRecordHashTable));
    if (table == NULL) {
        return NULL;
    }

    table->size = size;
    table->buckets = (IntRecordNode **)calloc(size, sizeof(IntRecordNode *));
    if (table->buckets == NULL) {
        free(table);
        return NULL;
    }

    return table;
}

void insert_int_record(IntRecordHashTable *table, int key, Record value) {
    unsigned int index;
    IntRecordNode *current;
    IntRecordNode *new_node;

    if (table == NULL) {
        return;
    }

    index = hash_function(key, table->size);
    current = table->buckets[index];

    while (current != NULL) {
        if (current->key == key) {
            current->value = value;
            return;
        }
        current = current->next;
    }

    new_node = (IntRecordNode *)malloc(sizeof(IntRecordNode));
    if (new_node == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria para IntRecordNode.\n");
        exit(EXIT_FAILURE);
    }

    new_node->key = key;
    new_node->value = value;
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
}

Record *search_int_record(IntRecordHashTable *table, int key) {
    unsigned int index;
    IntRecordNode *current;

    if (table == NULL) {
        return NULL;
    }

    index = hash_function(key, table->size);
    current = table->buckets[index];

    while (current != NULL) {
        if (current->key == key) {
            return &current->value;
        }
        current = current->next;
    }

    return NULL;
}

void free_int_record_hash_table(IntRecordHashTable *table) {
    int i;
    IntRecordNode *current;
    IntRecordNode *temp;

    if (table == NULL) {
        return;
    }

    for (i = 0; i < table->size; i++) {
        current = table->buckets[i];
        while (current != NULL) {
            temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(table->buckets);
    free(table);
}