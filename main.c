#include <stdio.h>
#include <stdlib.h>

#include "GlobalIndex/GlobalRecordIndex.h"
#include "GlobalIndex/GlobalRecordIndexLoader.h"
#include "Storage/FileIndex.h"
#include "IO/FileReader.h"
#include "Data/BlockLoader.h"
#include "Data/IntRecordHashTable.h"

#define GLOBAL_INDEX_SIZE 1009
#define RECORD_TABLE_SIZE 1009

int main(void) {
    int id_registro;
    char ruta[256];
    char *contenido;
    GlobalRecordIndex *global_index;
    RecordLocation *location;
    IntRecordHashTable *record_table;
    Record *record;

    global_index = create_global_record_index(GLOBAL_INDEX_SIZE);
    if (global_index == NULL) {
        fprintf(stderr, "No se pudo crear el indice global.\n");
        return 1;
    }

    inicializar_indice_global(global_index);

    printf("Ingrese el id global del registro: ");
    if (scanf("%d", &id_registro) != 1) {
        fprintf(stderr, "Error al leer el id del registro.\n");
        free_global_record_index(global_index);
        return 1;
    }

    location = search_global_record_index(global_index, id_registro);
    if (location == NULL) {
        printf("Registro no encontrado en el indice global.\n");
        free_global_record_index(global_index);
        return 1;
    }

    construir_ruta_bloque(location->id_comuna, location->id_bloque, ruta, sizeof(ruta));

    contenido = leer_archivo_completo(ruta);
    if (contenido == NULL) {
        printf("No se pudo leer el archivo.\n");
        free_global_record_index(global_index);
        return 1;
    }

    record_table = cargar_bloque_en_hash_table(contenido, RECORD_TABLE_SIZE);
    if (record_table == NULL) {
        printf("No se pudo cargar el bloque en la hash table.\n");
        free(contenido);
        free_global_record_index(global_index);
        return 1;
    }

    record = search_int_record(record_table, id_registro);

    printf("\n--- RESULTADO ---\n");
    printf("ID registro: %d\n", id_registro);
    printf("ID comuna: %d\n", location->id_comuna);
    printf("Bloque: %d\n", location->id_bloque);
    printf("Ruta archivo: %s\n", ruta);

    if (record == NULL) {
        printf("Registro no encontrado dentro del bloque.\n");
    } else {
        printf("\n--- REGISTRO ENCONTRADO ---\n");
        printf("ID: %d\n", record->id);
        printf("Nombre: %s\n", record->nombre);
        printf("Ciudad: %s\n", record->ciudad);
    }

    free_int_record_hash_table(record_table);
    free(contenido);
    free_global_record_index(global_index);

    return 0;
}