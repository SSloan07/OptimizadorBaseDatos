#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StringIntHashTable.h"

static unsigned int hash_function(const char *key, int table_size) {
    unsigned int hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + (unsigned int)c;
    }

    return hash % table_size;
}

StringIntHashTable *create_string_int_hash_table(int size) {
    StringIntHashTable *table = (StringIntHashTable *)malloc(sizeof(StringIntHashTable));
    if (table == NULL) {
        return NULL;
    }

    table->size = size;
    table->buckets = (HashNode **)calloc(size, sizeof(HashNode *));
    if (table->buckets == NULL) {
        free(table);
        return NULL;
    }

    return table;
}

void insert_string_int(StringIntHashTable *table, const char *key, int value) {
    unsigned int index;
    HashNode *current;
    HashNode *new_node;

    if (table == NULL || key == NULL) {
        return;
    }

    index = hash_function(key, table->size);
    current = table->buckets[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }

    new_node = (HashNode *)malloc(sizeof(HashNode));
    if (new_node == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria para HashNode.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(new_node->key, key);
    new_node->value = value;
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
}

int search_string_int(StringIntHashTable *table, const char *key) {
    unsigned int index;
    HashNode *current;

    if (table == NULL || key == NULL) {
        return -1;
    }

    index = hash_function(key, table->size);
    current = table->buckets[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }

    return -1;
}

void free_string_int_hash_table(StringIntHashTable *table) {
    int i;
    HashNode *current;
    HashNode *temp;

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