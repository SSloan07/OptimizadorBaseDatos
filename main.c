#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GlobalIndex/GlobalRecordIndex.h"
#include "GlobalIndex/GlobalRecordIndexLoader.h"
#include "Storage/FileIndex.h"
#include "IO/FileReader.h"
#include "IO/FileWriter.h"
#include "Compression/CompressionAdapter.h"
#include "Data/BlockLoader.h"
#include "Data/IntRecordHashTable.h"
#include "Data/BlockWriter.h"

#define GLOBAL_INDEX_SIZE 1009
#define RECORD_TABLE_SIZE 1009

int main(void) {
    int id_registro;
    char ruta[256];
    char ruta_salida[300];

    GlobalRecordIndex *global_index = NULL;
    RecordLocation *location = NULL;

    unsigned char *contenido_comprimido = NULL;
    unsigned char *contenido_recomprimido = NULL;
    unsigned char *contenido_descomprimido = NULL;

    size_t compressed_size = 0;
    size_t decompressed_size = 0;
    size_t recompressed_size = 0;

    IntRecordHashTable *record_table = NULL;
    Record *record = NULL;

    char *bloque_serializado = NULL;
    int write_result;

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

    contenido_comprimido = leer_archivo_binario(ruta, &compressed_size);
    if (contenido_comprimido == NULL) {
        printf("No se pudo leer el archivo.\n");
        free_global_record_index(global_index);
        return 1;
    }

    contenido_descomprimido = decompress_buffer(
        contenido_comprimido,
        compressed_size,
        &decompressed_size
    );

    if (contenido_descomprimido == NULL) {
        printf("No se pudo descomprimir el archivo.\n");
        free(contenido_comprimido);
        free_global_record_index(global_index);
        return 1;
    }

    record_table = cargar_bloque_en_hash_table(
        (const char *)contenido_descomprimido,
        RECORD_TABLE_SIZE
    );

    if (record_table == NULL) {
        printf("No se pudo cargar el bloque en la hash table.\n");
        free(contenido_descomprimido);
        free(contenido_comprimido);
        free_global_record_index(global_index);
        return 1;
    }

    record = search_int_record(record_table, id_registro);

    printf("\n--- RESULTADO ---\n");
    printf("ID registro: %d\n", id_registro);
    printf("ID comuna: %d\n", location->id_comuna);
    printf("Bloque: %d\n", location->id_bloque);
    printf("Ruta archivo original: %s\n", ruta);

    if (record == NULL) {
        printf("Registro no encontrado dentro del bloque.\n");
    } else {
        printf("\n--- REGISTRO ENCONTRADO ---\n");
        printf("ID: %d\n", record->id);
        printf("Nombre: %s\n", record->nombre);
        printf("Ciudad: %s\n", record->ciudad);
    }

    bloque_serializado = serializar_hash_table_a_bloque(record_table);
    if (bloque_serializado == NULL) {
        printf("\nNo se pudo serializar la hash table.\n");
        free_int_record_hash_table(record_table);
        free(contenido_descomprimido);
        free(contenido_comprimido);
        free_global_record_index(global_index);
        return 1;
    }

    printf("\n--- BLOQUE SERIALIZADO ---\n");
    printf("%s\n", bloque_serializado);

    contenido_recomprimido = compress_buffer(
        (const unsigned char *)bloque_serializado,
        strlen(bloque_serializado),
        &recompressed_size
    );

    if (contenido_recomprimido == NULL) {
        printf("No se pudo recomprimir el bloque.\n");
        free(bloque_serializado);
        free_int_record_hash_table(record_table);
        free(contenido_descomprimido);
        free(contenido_comprimido);
        free_global_record_index(global_index);
        return 1;
    }

    snprintf(ruta_salida, sizeof(ruta_salida), "%s.out", ruta);

    write_result = escribir_archivo_binario(
        ruta_salida,
        contenido_recomprimido,
        recompressed_size
    );

    if (write_result != 0) {
        printf("No se pudo escribir el archivo de salida.\n");
        free(contenido_recomprimido);
        free(bloque_serializado);
        free_int_record_hash_table(record_table);
        free(contenido_descomprimido);
        free(contenido_comprimido);
        free_global_record_index(global_index);
        return 1;
    }

    printf("\n--- ARCHIVO GENERADO ---\n");
    printf("Ruta archivo salida: %s\n", ruta_salida);
    printf("Tamano comprimido original: %zu bytes\n", compressed_size);
    printf("Tamano descomprimido: %zu bytes\n", decompressed_size);
    printf("Tamano recomprimido: %zu bytes\n", recompressed_size);

    free(contenido_recomprimido);
    free(bloque_serializado);
    free_int_record_hash_table(record_table);
    free(contenido_descomprimido);
    free(contenido_comprimido);
    free_global_record_index(global_index);

    return 0;
}