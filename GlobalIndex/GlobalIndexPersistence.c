#include <stdio.h>
#include <stdlib.h>
#include "GlobalIndexPersistence.h"

int guardar_indice_global_en_archivo(GlobalRecordIndex *index, const char *ruta) {
    FILE *archivo;
    int i;
    GlobalIndexNode *current;

    if (index == NULL || ruta == NULL) {
        return -1;
    }

    archivo = fopen(ruta, "w");
    if (archivo == NULL) {
        return -1;
    }

    for (i = 0; i < index->size; i++) {
        current = index->buckets[i];
        while (current != NULL) {
            fprintf(
                archivo,
                "%d,%d,%d\n",
                current->value.id_registro,
                current->value.id_comuna,
                current->value.id_bloque
            );
            current = current->next;
        }
    }

    fclose(archivo);
    return 0;
}

int cargar_indice_global_desde_archivo(GlobalRecordIndex *index, const char *ruta) {
    FILE *archivo;
    RecordLocation loc;

    if (index == NULL || ruta == NULL) {
        return -1;
    }

    archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        return -1;
    }

    while (fscanf(archivo, "%d,%d,%d\n",
                  &loc.id_registro,
                  &loc.id_comuna,
                  &loc.id_bloque) == 3) {
        insert_global_record_index(index, loc.id_registro, loc);
    }

    fclose(archivo);
    return 0;
}