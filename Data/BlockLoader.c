#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BlockLoader.h"

IntRecordHashTable *cargar_bloque_en_hash_table(const char *contenido_bloque, int table_size) {
    IntRecordHashTable *table;
    char *copia;
    char *linea;

    if (contenido_bloque == NULL) {
        return NULL;
    }

    table = create_int_record_hash_table(table_size);
    if (table == NULL) {
        return NULL;
    }

    copia = (char *)malloc(strlen(contenido_bloque) + 1);
    if (copia == NULL) {
        free_int_record_hash_table(table);
        return NULL;
    }

    strcpy(copia, contenido_bloque);
    linea = strtok(copia, "\n");

    while (linea != NULL) {
        Record record;
        int id;

        if (sscanf(linea, "registro_%d,%99[^,],%99[^\n]", &id, record.nombre, record.ciudad) == 3) {
            record.id = id;
            insert_int_record(table, record.id, record);
        }

        linea = strtok(NULL, "\n");
    }

    free(copia);
    return table;
}