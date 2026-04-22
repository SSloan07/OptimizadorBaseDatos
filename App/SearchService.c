#include <stdio.h>
#include <stdlib.h>

#include "../GlobalIndex/GlobalRecordIndex.h"
#include "../GlobalIndex/GlobalIndexPersistence.h"
#include "../Storage/FileIndex.h"
#include "../IO/FileReader.h"
#include "../Compression/CompressionAdapter.h"
#include "../Data/BlockLoader.h"
#include "../Data/IntRecordHashTable.h"

#define GLOBAL_INDEX_SIZE 1009
#define RECORD_TABLE_SIZE 1009
#define GLOBAL_INDEX_FILE "data/global_index.txt"

int ejecutar_busqueda_puntual(void) {
    int id_registro;
    char ruta[256];

    GlobalRecordIndex *global_index = NULL;
    RecordLocation *location = NULL;

    unsigned char *contenido_comprimido = NULL;
    unsigned char *contenido_descomprimido = NULL;
    size_t compressed_size = 0;
    size_t decompressed_size = 0;

    IntRecordHashTable *record_table = NULL;
    Record *record = NULL;

    global_index = create_global_record_index(GLOBAL_INDEX_SIZE);
    if (global_index == NULL) {
        fprintf(stderr, "No se pudo crear el indice global.\n");
        return 1;
    }

    if (cargar_indice_global_desde_archivo(global_index, GLOBAL_INDEX_FILE) != 0) {
        fprintf(stderr, "No se pudo cargar el indice global.\n");
        free_global_record_index(global_index);
        return 1;
    }

    printf("Ingrese el id global del registro a buscar: ");
    if (scanf("%d", &id_registro) != 1) {
        fprintf(stderr, "Error al leer el id.\n");
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
        printf("No se pudo leer el archivo del bloque.\n");
        free_global_record_index(global_index);
        return 1;
    }

    contenido_descomprimido = decompress_buffer(
        contenido_comprimido,
        compressed_size,
        &decompressed_size
    );

    if (contenido_descomprimido == NULL) {
        printf("No se pudo descomprimir el bloque.\n");
        free(contenido_comprimido);
        free_global_record_index(global_index);
        return 1;
    }

    record_table = cargar_bloque_en_hash_table(
        (const char *)contenido_descomprimido,
        RECORD_TABLE_SIZE
    );

    if (record_table == NULL) {
        printf("No se pudo cargar el bloque en memoria.\n");
        free(contenido_descomprimido);
        free(contenido_comprimido);
        free_global_record_index(global_index);
        return 1;
    }

    record = search_int_record(record_table, id_registro);

    printf("\n--- RESULTADO DE BUSQUEDA ---\n");
    printf("ID comuna: %d\n", location->id_comuna);
    printf("Bloque: %d\n", location->id_bloque);
    printf("Ruta: %s\n", ruta);

    if (record == NULL) {
        printf("Registro no encontrado dentro del bloque.\n");
    } else {
        printf("ID: %d\n", record->id);
        printf("Nombre: %s\n", record->nombre);
        printf("Edad: %d\n", record->edad);
        printf("Escolaridad: %s\n", record->escolaridad);
        printf("Comuna: %s\n", record->comuna);
    }

    free_int_record_hash_table(record_table);
    free(contenido_descomprimido);
    free(contenido_comprimido);
    free_global_record_index(global_index);

    return 0;
}