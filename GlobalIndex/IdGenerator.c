#include <stddef.h>
#include "IdGenerator.h"

int obtener_siguiente_id_global(GlobalRecordIndex *index) {
    int i;
    int max_id = 0;
    GlobalIndexNode *current;

    if (index == NULL) {
        return -1;
    }

    for (i = 0; i < index->size; i++) {
        current = index->buckets[i];
        while (current != NULL) {
            if (current->key > max_id) {
                max_id = current->key;
            }
            current = current->next;
        }
    }

    return max_id + 1;
}