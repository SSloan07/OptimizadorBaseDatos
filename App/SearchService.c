#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SearchService.h"

#include "../Storage/FileIndex.h"
#include "../IO/FileReader.h"
#include "../Compression/CompressionAdapter.h"
#include "../Data/BlockLoader.h"
#include "../Data/IntRecordHashTable.h"

#define RECORD_TABLE_SIZE 1009

int ejecutar_busqueda_puntual(AppContext *ctx) {
    int id_registro;
    char ruta[256];

    RecordLocation *location = NULL;

    unsigned char *contenido_comprimido = NULL;
    unsigned char *contenido_descomprimido = NULL;
    size_t compressed_size = 0;
    size_t decompressed_size = 0;

    IntRecordHashTable *record_table = NULL;
    Record *record = NULL;

    clock_t t0, t1, t2, t3, t4, t5;
    double dt_index, dt_read, dt_decompress, dt_load, dt_search, dt_total;

    if (ctx == NULL || ctx->global_index == NULL) {
        fprintf(stderr, "Contexto de aplicacion invalido.\n");
        return 1;
    }

    printf("Ingrese el id global del registro a buscar: ");
    if (scanf("%d", &id_registro) != 1) {
        fprintf(stderr, "Error al leer el id.\n");
        return 1;
    }

    t0 = clock();

    location = search_global_record_index(ctx->global_index, id_registro);

    t1 = clock();

    if (location == NULL) {
        printf("Registro no encontrado en el indice global.\n");
        return 1;
    }

    construir_ruta_bloque(location->id_comuna, location->id_bloque, ruta, sizeof(ruta));

    contenido_comprimido = leer_archivo_binario(ruta, &compressed_size);

    t2 = clock();

    if (contenido_comprimido == NULL) {
        printf("No se pudo leer el archivo del bloque.\n");
        return 1;
    }

    contenido_descomprimido = decompress_buffer(
        contenido_comprimido,
        compressed_size,
        &decompressed_size
    );

    t3 = clock();

    if (contenido_descomprimido == NULL) {
        printf("No se pudo descomprimir el bloque.\n");
        free(contenido_comprimido);
        return 1;
    }

    record_table = cargar_bloque_en_hash_table(
        (const char *)contenido_descomprimido,
        RECORD_TABLE_SIZE
    );

    t4 = clock();

    if (record_table == NULL) {
        printf("No se pudo cargar el bloque en memoria.\n");
        free(contenido_descomprimido);
        free(contenido_comprimido);
        return 1;
    }

    record = search_int_record(record_table, id_registro);

    t5 = clock();

    dt_index = (double)(t1 - t0) / CLOCKS_PER_SEC;
    dt_read = (double)(t2 - t1) / CLOCKS_PER_SEC;
    dt_decompress = (double)(t3 - t2) / CLOCKS_PER_SEC;
    dt_load = (double)(t4 - t3) / CLOCKS_PER_SEC;
    dt_search = (double)(t5 - t4) / CLOCKS_PER_SEC;
    dt_total = (double)(t5 - t0) / CLOCKS_PER_SEC;

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

    printf("\n--- METRICAS DE BUSQUEDA ---\n");
    printf("Indice global: %.6f s\n", dt_index);
    printf("Lectura de bloque: %.6f s\n", dt_read);
    printf("Descompresion: %.6f s\n", dt_decompress);
    printf("Carga a hash table: %.6f s\n", dt_load);
    printf("Busqueda en hash table: %.6f s\n", dt_search);
    printf("Tiempo total: %.6f s\n", dt_total);

    free_int_record_hash_table(record_table);
    free(contenido_descomprimido);
    free(contenido_comprimido);

    return 0;
}