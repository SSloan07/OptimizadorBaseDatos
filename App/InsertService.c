#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "InsertService.h"
#include "../Synthetic/SyntheticBatchInsert.h"

#include "../GlobalIndex/GlobalIndexPersistence.h"
#include "../GlobalIndex/IdGenerator.h"
#include "../Storage/FileIndex.h"
#include "../Storage/CommuneBlockManager.h"
#include "../BigHashTable/MedellinCommunes.h"
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
#define GLOBAL_INDEX_FILE "data/global_index.txt"

// Clave temporal . Se podría poner en un .env (Somos conscientes de que esta no es la mejor práctica)
static const unsigned char AES_PROJECT_KEY[] =
    "CSF123CSF123CSF123CSF123CS12CSF1";

_Static_assert(sizeof(AES_PROJECT_KEY) - 1 == 32, "La clave AES-256 debe tener exactamente 32 caracteres");

static void limpiar_buffer_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static int insertar_registro_puntual(AppContext *ctx) {
    IntRecordHashTable *record_table_destino = NULL;
    Encripter *encripter = NULL;

    unsigned char *contenido_archivo = NULL;
    unsigned char *contenido_descifrado = NULL;
    unsigned char *contenido_descomprimido = NULL;
    unsigned char *contenido_recomprimido = NULL;
    unsigned char *contenido_cifrado = NULL;

    size_t file_size = 0;
    size_t decrypted_size = 0;
    size_t decompressed_size = 0;
    size_t recompressed_size = 0;
    size_t encrypted_size = 0;

    char ruta_destino[256];
    char ruta_salida[256];
    char *bloque_serializado = NULL;

    Record nuevo;
    int id_comuna_destino;
    int id_bloque_destino;
    int write_result;
    int status = 1;

    if (ctx == NULL || ctx->global_index == NULL) {
        fprintf(stderr, "Contexto de aplicacion invalido.\n");
        return 1;
    }

    inicializar_comunas();

    printf("\n--- INSERCION DE REGISTRO PUNTUAL ---\n");

    nuevo.id = obtener_siguiente_id_global(ctx->global_index);
    if (nuevo.id == -1) {
        fprintf(stderr, "No se pudo generar un nuevo ID global.\n");
        goto cleanup;
    }

    limpiar_buffer_entrada();

    printf("ID generado automaticamente: %d\n", nuevo.id);

    printf("Ingrese el nombre: ");
    if (fgets(nuevo.nombre, sizeof(nuevo.nombre), stdin) == NULL) {
        fprintf(stderr, "Error al leer el nombre.\n");
        goto cleanup;
    }
    nuevo.nombre[strcspn(nuevo.nombre, "\n")] = '\0';

    printf("Ingrese la edad: ");
    if (scanf("%d", &nuevo.edad) != 1) {
        fprintf(stderr, "Error al leer la edad.\n");
        goto cleanup;
    }

    limpiar_buffer_entrada();

    printf("Ingrese la escolaridad: ");
    if (fgets(nuevo.escolaridad, sizeof(nuevo.escolaridad), stdin) == NULL) {
        fprintf(stderr, "Error al leer la escolaridad.\n");
        goto cleanup;
    }
    nuevo.escolaridad[strcspn(nuevo.escolaridad, "\n")] = '\0';

    printf("Ingrese la comuna: ");
    if (fgets(nuevo.comuna, sizeof(nuevo.comuna), stdin) == NULL) {
        fprintf(stderr, "Error al leer la comuna.\n");
        goto cleanup;
    }
    nuevo.comuna[strcspn(nuevo.comuna, "\n")] = '\0';

    id_comuna_destino = obtener_id_comuna(nuevo.comuna);
    if (id_comuna_destino == -1) {
        fprintf(stderr, "La comuna ingresada no es valida.\n");
        goto cleanup;
    }

    id_bloque_destino = obtener_bloque_activo_por_comuna(id_comuna_destino);
    if (id_bloque_destino == -1) {
        fprintf(stderr, "No se pudo obtener el bloque activo para la comuna.\n");
        goto cleanup;
    }

    construir_ruta_bloque(
        id_comuna_destino,
        id_bloque_destino,
        ruta_destino,
        sizeof(ruta_destino)
    );

    printf("\n--- DESTINO DEL NUEVO REGISTRO ---\n");
    printf("ID comuna destino: %d\n", id_comuna_destino);
    printf("Bloque destino: %d\n", id_bloque_destino);
    printf("Ruta destino: %s\n", ruta_destino);

    encripter = create_aes_256_gcm_encripter(AES_PROJECT_KEY);

    if (encripter == NULL) {
        fprintf(stderr, "No se pudo crear el encripter AES.\n");
        goto cleanup;
    }

    // archivo,  Descifro,  Descomprimo
    
    contenido_archivo = leer_archivo_binario(ruta_destino, &file_size);

    if (contenido_archivo != NULL) {
        contenido_descifrado = encripter_decrypt(
            encripter,
            contenido_archivo,
            file_size,
            &decrypted_size
        );

        if (contenido_descifrado == NULL) {
            printf("No se pudo descifrar el archivo destino.\n");
            goto cleanup;
        }

        contenido_descomprimido = decompress_buffer(
            contenido_descifrado,
            decrypted_size,
            &decompressed_size
        );

        if (contenido_descomprimido == NULL) {
            printf("No se pudo descomprimir el archivo destino.\n");
            goto cleanup;
        }

        record_table_destino = cargar_bloque_en_hash_table(
            (const char *)contenido_descomprimido,
            RECORD_TABLE_SIZE
        );

        if (record_table_destino == NULL) {
            printf("No se pudo cargar el bloque destino en la hash table.\n");
            goto cleanup;
        }
    } else {
        record_table_destino = create_int_record_hash_table(RECORD_TABLE_SIZE);
        if (record_table_destino == NULL) {
            printf("No se pudo crear una hash table vacia para el bloque destino.\n");
            goto cleanup;
        }

        printf("El bloque destino no existia. Se creara uno nuevo.\n");
    }

    insert_int_record(record_table_destino, nuevo.id, nuevo);

    {
        RecordLocation nueva_ubicacion;
        nueva_ubicacion.id_registro = nuevo.id;
        nueva_ubicacion.id_comuna = id_comuna_destino;
        nueva_ubicacion.id_bloque = id_bloque_destino;

        insert_global_record_index(
            ctx->global_index,
            nuevo.id,
            nueva_ubicacion
        );
    }

    if (guardar_indice_global_en_archivo(ctx->global_index, GLOBAL_INDEX_FILE) != 0) {
        printf("No se pudo guardar el indice global actualizado.\n");
        goto cleanup;
    }

    printf("\n--- NUEVO REGISTRO INSERTADO EN MEMORIA ---\n");
    printf("ID: %d\n", nuevo.id);
    printf("Nombre: %s\n", nuevo.nombre);
    printf("Edad: %d\n", nuevo.edad);
    printf("Escolaridad: %s\n", nuevo.escolaridad);
    printf("Comuna: %s\n", nuevo.comuna);

    bloque_serializado = serializar_hash_table_a_bloque(record_table_destino);
    if (bloque_serializado == NULL) {
        printf("No se pudo serializar la hash table destino.\n");
        goto cleanup;
    }

    contenido_recomprimido = compress_buffer(
        (const unsigned char *)bloque_serializado,
        strlen(bloque_serializado),
        &recompressed_size
    );

    if (contenido_recomprimido == NULL) {
        printf("No se pudo recomprimir el bloque destino.\n");
        goto cleanup;
    }

    // Comprimimos, luego ciframos y ahí sí pasamos a escribir en el lote correspondiente. Ojo, mano, si lo hacemos en el otro orden eso no funciona porque como AES produce secuencias casi aleatorias entonces si primero cifro y después comprimo, LZW queda pailas porque no encuentra patrones sobre los cuales trabajar. 
    contenido_cifrado = encripter_encrypt(
        encripter,
        contenido_recomprimido,
        recompressed_size,
        &encrypted_size
    );

    if (contenido_cifrado == NULL) {
        printf("No se pudo cifrar el bloque destino.\n");
        goto cleanup;
    }

    if (asegurar_directorio_comuna(id_comuna_destino) != 0) {
        printf("No se pudo asegurar el directorio de la comuna destino.\n");
        goto cleanup;
    }

    strncpy(ruta_salida, ruta_destino, sizeof(ruta_salida) - 1);
    ruta_salida[sizeof(ruta_salida) - 1] = '\0';

    write_result = escribir_archivo_binario(
        ruta_salida,
        contenido_cifrado,
        encrypted_size
    );

    if (write_result != 0) {
        printf("No se pudo escribir el archivo destino.\n");
        goto cleanup;
    }

    printf("\n--- ARCHIVO SOBRESCRITO / CREADO ---\n");
    printf("Ruta archivo destino: %s\n", ruta_salida);
    printf("Tamano recomprimido: %zu bytes\n", recompressed_size);
    printf("Tamano cifrado: %zu bytes\n", encrypted_size);

    status = 0;

cleanup:
    free(contenido_cifrado);
    free(contenido_recomprimido);
    free(bloque_serializado);

    if (record_table_destino != NULL) {
        free_int_record_hash_table(record_table_destino);
    }

    free(contenido_descomprimido);
    free(contenido_descifrado);
    free(contenido_archivo);

    encripter_destroy(encripter);
    liberar_comunas();

    return status;
}

int ejecutar_menu_insercion(AppContext *ctx) {
    int opcion;

    if (ctx == NULL || ctx->global_index == NULL) {
        fprintf(stderr, "Contexto de aplicacion invalido.\n");
        return 1;
    }

    while (1) {
        printf("\n===== MENU DE INSERCION =====\n");
        printf("1. Insertar un registro puntual\n");
        printf("2. Insertar registros masivamente\n");
        printf("3. Volver al menu principal\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            limpiar_buffer_entrada();
            printf("Entrada invalida.\n");
            continue;
        }

        switch (opcion) {
            case 1:
                insertar_registro_puntual(ctx);
                break;

            case 2: {
                int cantidad;

                printf("\n--- INSERCION MASIVA ---\n");
                printf("Cuantos registros desea generar? ");

                if (scanf("%d", &cantidad) != 1) {
                    limpiar_buffer_entrada();
                    printf("Entrada invalida.\n");
                    break;
                }

                ejecutar_insercion_masiva_sintetica(ctx, cantidad);

                if (recargar_indice_global(ctx) != 0) {
                    fprintf(stderr, "Advertencia: no se pudo recargar el indice global.\n");
                }

                break;
            }

            case 3:
                return 0;

            default:
                printf("Opcion invalida.\n");
                break;
        }
    }
}