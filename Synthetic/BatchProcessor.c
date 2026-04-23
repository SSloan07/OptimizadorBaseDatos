#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BatchProcessor.h"

#include "../Storage/FileIndex.h"
#include "../IO/FileReader.h"
#include "../IO/FileWriter.h"
#include "../IO/DirectoryManager.h"
#include "../Compression/CompressionAdapter.h"
#include "../Data/BlockLoader.h"
#include "../Data/IntRecordHashTable.h"
#include "../Data/BlockWriter.h"

#define RECORD_TABLE_SIZE 1009

int procesar_grupos_batch(GroupNode *groups) {
    GroupNode *grupo = groups;

    while (grupo != NULL) {
        char ruta_destino[256];
        unsigned char *contenido_comprimido = NULL;
        unsigned char *contenido_descomprimido = NULL;
        unsigned char *contenido_recomprimido = NULL;
        size_t compressed_size = 0;
        size_t decompressed_size = 0;
        size_t recompressed_size = 0;
        IntRecordHashTable *record_table_destino = NULL;
        char *bloque_serializado = NULL;

        construir_ruta_bloque(
            grupo->id_comuna,
            grupo->id_bloque,
            ruta_destino,
            sizeof(ruta_destino)
        );

        contenido_comprimido = leer_archivo_binario(ruta_destino, &compressed_size);

        if (contenido_comprimido != NULL) {
            contenido_descomprimido = decompress_buffer(
                contenido_comprimido,
                compressed_size,
                &decompressed_size
            );

            if (contenido_descomprimido == NULL) {
                free(contenido_comprimido);
                return -1;
            }

            record_table_destino = cargar_bloque_en_hash_table(
                (const char *)contenido_descomprimido,
                RECORD_TABLE_SIZE
            );

            if (record_table_destino == NULL) {
                free(contenido_descomprimido);
                free(contenido_comprimido);
                return -1;
            }
        } else {
            record_table_destino = create_int_record_hash_table(RECORD_TABLE_SIZE);
            if (record_table_destino == NULL) {
                return -1;
            }
        }

        {
            GeneratedNode *node = grupo->records;
            while (node != NULL) {
                insert_int_record(record_table_destino, node->record.id, node->record);
                node = node->next;
            }
        }

        bloque_serializado = serializar_hash_table_a_bloque(record_table_destino);
        if (bloque_serializado == NULL) {
            free_int_record_hash_table(record_table_destino);
            free(contenido_descomprimido);
            free(contenido_comprimido);
            return -1;
        }

        contenido_recomprimido = compress_buffer(
            (const unsigned char *)bloque_serializado,
            strlen(bloque_serializado),
            &recompressed_size
        );

        if (contenido_recomprimido == NULL) {
            free(bloque_serializado);
            free_int_record_hash_table(record_table_destino);
            free(contenido_descomprimido);
            free(contenido_comprimido);
            return -1;
        }

        if (asegurar_directorio_comuna(grupo->id_comuna) != 0) {
            free(contenido_recomprimido);
            free(bloque_serializado);
            free_int_record_hash_table(record_table_destino);
            free(contenido_descomprimido);
            free(contenido_comprimido);
            return -1;
        }

        if (escribir_archivo_binario(ruta_destino, contenido_recomprimido, recompressed_size) != 0) {
            free(contenido_recomprimido);
            free(bloque_serializado);
            free_int_record_hash_table(record_table_destino);
            free(contenido_descomprimido);
            free(contenido_comprimido);
            return -1;
        }

        free(contenido_recomprimido);
        free(bloque_serializado);
        free_int_record_hash_table(record_table_destino);
        free(contenido_descomprimido);
        free(contenido_comprimido);

        grupo = grupo->next;
    }

    return 0;
}