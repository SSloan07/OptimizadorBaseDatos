#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BlockWriter.h"

char *serializar_hash_table_a_bloque(IntRecordHashTable *table) {
    int i;
    size_t capacidad = 1024;
    size_t usado = 0;
    char linea[512];
    char *buffer;
    IntRecordNode *current;

    if (table == NULL) {
        return NULL;
    }

    buffer = (char *)malloc(capacidad);
    if (buffer == NULL) {
        return NULL;
    }

    buffer[0] = '\0';

    for (i = 0; i < table->size; i++) {
        current = table->buckets[i];

        while (current != NULL) {
            int escritos = snprintf(
                linea,
                sizeof(linea),
                "registro_%d,%s,%s\n",
                current->value.id,
                current->value.nombre,
                current->value.ciudad
            );

            if (escritos < 0) {
                free(buffer);
                return NULL;
            }

            while (usado + (size_t)escritos + 1 > capacidad) {
                char *nuevo_buffer = (char *)realloc(buffer, capacidad * 2);
                if (nuevo_buffer == NULL) {
                    free(buffer);
                    return NULL;
                }
                buffer = nuevo_buffer;
                capacidad *= 2;
            }

            memcpy(buffer + usado, linea, (size_t)escritos);
            usado += (size_t)escritos;
            buffer[usado] = '\0';

            current = current->next;
        }
    }

    return buffer;
}