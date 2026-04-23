#include <stdio.h>
#include <stdlib.h>
#include "GlobalRecordIndex.h"

static unsigned int hash_function(int key, int table_size) {
    if (key < 0) {
        key = -key;
    }
    return (unsigned int)key % (unsigned int)table_size;
}

GlobalRecordIndex *create_global_record_index(int size) {
    GlobalRecordIndex *index = (GlobalRecordIndex *)malloc(sizeof(GlobalRecordIndex));
    if (index == NULL) {
        return NULL;
    }

    index->size = size;
    index->buckets = (GlobalIndexNode **)calloc(size, sizeof(GlobalIndexNode *));
    if (index->buckets == NULL) {
        free(index);
        return NULL;
    }

    return index;
}

void insert_global_record_index(GlobalRecordIndex *index, int key, RecordLocation value) {
    unsigned int bucket_index;
    GlobalIndexNode *current;
    GlobalIndexNode *new_node;

    if (index == NULL) {
        return;
    }

    bucket_index = hash_function(key, index->size);
    current = index->buckets[bucket_index];

    while (current != NULL) {
        if (current->key == key) {
            current->value = value;
            return;
        }
        current = current->next;
    }

    new_node = (GlobalIndexNode *)malloc(sizeof(GlobalIndexNode));
    if (new_node == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria para GlobalIndexNode.\n");
        exit(EXIT_FAILURE);
    }

    new_node->key = key;
    new_node->value = value;
    new_node->next = index->buckets[bucket_index];
    index->buckets[bucket_index] = new_node;
}

RecordLocation *search_global_record_index(GlobalRecordIndex *index, int key) {
    unsigned int bucket_index;
    GlobalIndexNode *current;

    if (index == NULL) {
        return NULL;
    }

    bucket_index = hash_function(key, index->size);
    current = index->buckets[bucket_index];

    while (current != NULL) {
        if (current->key == key) {
            return &current->value;
        }
        current = current->next;
    }

    return NULL;
}

void free_global_record_index(GlobalRecordIndex *index) {
    int i;
    GlobalIndexNode *current;
    GlobalIndexNode *temp;

    if (index == NULL) {
        return;
    }

    for (i = 0; i < index->size; i++) {
        current = index->buckets[i];
        while (current != NULL) {
            temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(index->buckets);
    free(index);
}