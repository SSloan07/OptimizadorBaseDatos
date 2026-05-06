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

#include "../Encryption/Encripter.h"
#include "../AES/AesEncripter.h"

#define RECORD_TABLE_SIZE 1009

static const unsigned char AES_PROJECT_KEY[] = "CSF123CSF123CSF123CSF123CS12CSF1";

_Static_assert(sizeof(AES_PROJECT_KEY) - 1 == 32, "La clave AES-256 debe tener exactamente 32 caracteres");


int procesar_grupos_batch(GroupNode *groups) {
    GroupNode *grupo = groups;
    Encripter *encripter = NULL;

    encripter = create_aes_256_gcm_encripter(AES_PROJECT_KEY);
    if (encripter == NULL) {
        fprintf(stderr, "No se pudo crear el encriptador AES-256-GCM.\n");
        return -1;
    }

    while (grupo != NULL) {
        char ruta_destino[256];

        unsigned char *contenido_cifrado = NULL;
        unsigned char *contenido_comprimido = NULL;
        unsigned char *contenido_descomprimido = NULL;
        unsigned char *contenido_recomprimido = NULL;
        unsigned char *contenido_recifrado = NULL;

        size_t encrypted_size = 0;
        size_t compressed_size = 0;
        size_t decompressed_size = 0;
        size_t recompressed_size = 0;
        size_t reencrypted_size = 0;

        IntRecordHashTable *record_table_destino = NULL;
        char *bloque_serializado = NULL;

        construir_ruta_bloque(
            grupo->id_comuna,
            grupo->id_bloque,
            ruta_destino,
            sizeof(ruta_destino)
        );

        
        contenido_cifrado = leer_archivo_binario(ruta_destino, &encrypted_size);

        if (contenido_cifrado != NULL) {
            contenido_comprimido = encripter->decrypt(
                encripter,
                contenido_cifrado,
                encrypted_size,
                &compressed_size
            );

            if (contenido_comprimido == NULL) {
                fprintf(stderr, "No se pudo descifrar el bloque destino: %s\n", ruta_destino);

                free(contenido_cifrado);
                return -1;
            }

            contenido_descomprimido = decompress_buffer(
                contenido_comprimido,
                compressed_size,
                &decompressed_size
            );

            if (contenido_descomprimido == NULL) {
                fprintf(stderr, "No se pudo descomprimir el bloque destino: %s\n", ruta_destino);

                free(contenido_comprimido);
                free(contenido_cifrado);
                return -1;
            }

            record_table_destino = cargar_bloque_en_hash_table(
                (const char *)contenido_descomprimido,
                RECORD_TABLE_SIZE
            );

            if (record_table_destino == NULL) {
                fprintf(stderr, "No se pudo cargar el bloque en hash table: %s\n", ruta_destino);

                free(contenido_descomprimido);
                free(contenido_comprimido);
                free(contenido_cifrado);
                return -1;
            }
        } else {
            record_table_destino = create_int_record_hash_table(RECORD_TABLE_SIZE);

            if (record_table_destino == NULL) {
                fprintf(stderr, "No se pudo crear la hash table destino.\n");
                return -1;
            }
        }

        {
            GeneratedNode *node = grupo->records;

            while (node != NULL) {
                insert_int_record(
                    record_table_destino,
                    node->record.id,
                    node->record
                );

                node = node->next;
            }
        }

        bloque_serializado = serializar_hash_table_a_bloque(record_table_destino);

        if (bloque_serializado == NULL) {
            fprintf(stderr, "No se pudo serializar el bloque.\n");

            free_int_record_hash_table(record_table_destino);
            free(contenido_descomprimido);
            free(contenido_comprimido);
            free(contenido_cifrado);
            return -1;
        }

        contenido_recomprimido = compress_buffer(
            (const unsigned char *)bloque_serializado,
            strlen(bloque_serializado),
            &recompressed_size
        );

        if (contenido_recomprimido == NULL) {
            fprintf(stderr, "No se pudo comprimir el bloque serializado.\n");

            free(bloque_serializado);
            free_int_record_hash_table(record_table_destino);
            free(contenido_descomprimido);
            free(contenido_comprimido);
            free(contenido_cifrado);
            return -1;
        }

        
        contenido_recifrado = encripter->encrypt(
            encripter,
            contenido_recomprimido,
            recompressed_size,
            &reencrypted_size
        );

        if (contenido_recifrado == NULL) {
            fprintf(stderr, "No se pudo cifrar el bloque destino: %s\n", ruta_destino);

            free(contenido_recomprimido);
            free(bloque_serializado);
            free_int_record_hash_table(record_table_destino);
            free(contenido_descomprimido);
            free(contenido_comprimido);
            free(contenido_cifrado);
            return -1;
        }

        if (asegurar_directorio_comuna(grupo->id_comuna) != 0) {
            fprintf(stderr, "No se pudo asegurar el directorio de la comuna %d.\n", grupo->id_comuna);

            free(contenido_recifrado);
            free(contenido_recomprimido);
            free(bloque_serializado);
            free_int_record_hash_table(record_table_destino);
            free(contenido_descomprimido);
            free(contenido_comprimido);
            free(contenido_cifrado);
            return -1;
        }

        if (escribir_archivo_binario(
                ruta_destino,
                contenido_recifrado,
                reencrypted_size
            ) != 0) {
            fprintf(stderr, "No se pudo escribir el bloque cifrado: %s\n", ruta_destino);

            free(contenido_recifrado);
            free(contenido_recomprimido);
            free(bloque_serializado);
            free_int_record_hash_table(record_table_destino);
            free(contenido_descomprimido);
            free(contenido_comprimido);
            free(contenido_cifrado);
            return -1;
        }

        free(contenido_recifrado);
        free(contenido_recomprimido);
        free(bloque_serializado);
        free_int_record_hash_table(record_table_destino);
        free(contenido_descomprimido);
        free(contenido_comprimido);
        free(contenido_cifrado);

        grupo = grupo->next;
    }

    

    return 0;
}