#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "InsertService.h"

#include "../GlobalIndex/GlobalRecordIndex.h"
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

#define GLOBAL_INDEX_SIZE 1009
#define RECORD_TABLE_SIZE 1009
#define GLOBAL_INDEX_FILE "data/global_index.txt"

static void limpiar_buffer_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static int insertar_registro_puntual(void) {
    GlobalRecordIndex *global_index = NULL;
    IntRecordHashTable *record_table_destino = NULL;

    unsigned char *contenido_comprimido = NULL;
    unsigned char *contenido_descomprimido = NULL;
    unsigned char *contenido_recomprimido = NULL;

    size_t compressed_size = 0;
    size_t decompressed_size = 0;
    size_t recompressed_size = 0;

    char ruta_destino[256];
    char ruta_salida[256];
    char *bloque_serializado = NULL;

    Record nuevo;
    int id_comuna_destino;
    int id_bloque_destino;
    int write_result;

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

    inicializar_comunas();

    printf("\n--- INSERCION DE REGISTRO PUNTUAL ---\n");

    nuevo.id = obtener_siguiente_id_global(global_index);
    if (nuevo.id == -1) {
        fprintf(stderr, "No se pudo generar un nuevo ID global.\n");
        liberar_comunas();
        free_global_record_index(global_index);
        return 1;
    }

    limpiar_buffer_entrada();

    printf("ID generado automaticamente: %d\n", nuevo.id);

    printf("Ingrese el nombre: ");
    if (fgets(nuevo.nombre, sizeof(nuevo.nombre), stdin) == NULL) {
        fprintf(stderr, "Error al leer el nombre.\n");
        liberar_comunas();
        free_global_record_index(global_index);
        return 1;
    }
    nuevo.nombre[strcspn(nuevo.nombre, "\n")] = '\0';

    printf("Ingrese la edad: ");
    if (scanf("%d", &nuevo.edad) != 1) {
        fprintf(stderr, "Error al leer la edad.\n");
        liberar_comunas();
        free_global_record_index(global_index);
        return 1;
    }

    limpiar_buffer_entrada();

    printf("Ingrese la escolaridad: ");
    if (fgets(nuevo.escolaridad, sizeof(nuevo.escolaridad), stdin) == NULL) {
        fprintf(stderr, "Error al leer la escolaridad.\n");
        liberar_comunas();
        free_global_record_index(global_index);
        return 1;
    }
    nuevo.escolaridad[strcspn(nuevo.escolaridad, "\n")] = '\0';

    printf("Ingrese la comuna: ");
    if (fgets(nuevo.comuna, sizeof(nuevo.comuna), stdin) == NULL) {
        fprintf(stderr, "Error al leer la comuna.\n");
        liberar_comunas();
        free_global_record_index(global_index);
        return 1;
    }
    nuevo.comuna[strcspn(nuevo.comuna, "\n")] = '\0';

    id_comuna_destino = obtener_id_comuna(nuevo.comuna);
    if (id_comuna_destino == -1) {
        fprintf(stderr, "La comuna ingresada no es valida.\n");
        liberar_comunas();
        free_global_record_index(global_index);
        return 1;
    }

    id_bloque_destino = obtener_bloque_activo_por_comuna(id_comuna_destino);
    if (id_bloque_destino == -1) {
        fprintf(stderr, "No se pudo obtener el bloque activo para la comuna.\n");
        liberar_comunas();
        free_global_record_index(global_index);
        return 1;
    }

    construir_ruta_bloque(id_comuna_destino, id_bloque_destino, ruta_destino, sizeof(ruta_destino));

    printf("\n--- DESTINO DEL NUEVO REGISTRO ---\n");
    printf("ID comuna destino: %d\n", id_comuna_destino);
    printf("Bloque destino: %d\n", id_bloque_destino);
    printf("Ruta destino: %s\n", ruta_destino);

    contenido_comprimido = leer_archivo_binario(ruta_destino, &compressed_size);

    if (contenido_comprimido != NULL) {
        contenido_descomprimido = decompress_buffer(
            contenido_comprimido,
            compressed_size,
            &decompressed_size
        );

        if (contenido_descomprimido == NULL) {
            printf("No se pudo descomprimir el archivo destino.\n");
            free(contenido_comprimido);
            liberar_comunas();
            free_global_record_index(global_index);
            return 1;
        }

        record_table_destino = cargar_bloque_en_hash_table(
            (const char *)contenido_descomprimido,
            RECORD_TABLE_SIZE
        );

        if (record_table_destino == NULL) {
            printf("No se pudo cargar el bloque destino en la hash table.\n");
            free(contenido_descomprimido);
            free(contenido_comprimido);
            liberar_comunas();
            free_global_record_index(global_index);
            return 1;
        }
    } else {
        record_table_destino = create_int_record_hash_table(RECORD_TABLE_SIZE);
        if (record_table_destino == NULL) {
            printf("No se pudo crear una hash table vacia para el bloque destino.\n");
            liberar_comunas();
            free_global_record_index(global_index);
            return 1;
        }
        printf("El bloque destino no existia. Se creara uno nuevo.\n");
    }

    insert_int_record(record_table_destino, nuevo.id, nuevo);

    {
        RecordLocation nueva_ubicacion;
        nueva_ubicacion.id_registro = nuevo.id;
        nueva_ubicacion.id_comuna = id_comuna_destino;
        nueva_ubicacion.id_bloque = id_bloque_destino;
        insert_global_record_index(global_index, nuevo.id, nueva_ubicacion);
    }

    if (guardar_indice_global_en_archivo(global_index, GLOBAL_INDEX_FILE) != 0) {
        printf("No se pudo guardar el indice global actualizado.\n");
        free_int_record_hash_table(record_table_destino);
        free(contenido_descomprimido);
        free(contenido_comprimido);
        liberar_comunas();
        free_global_record_index(global_index);
        return 1;
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
        free_int_record_hash_table(record_table_destino);
        free(contenido_descomprimido);
        free(contenido_comprimido);
        liberar_comunas();
        free_global_record_index(global_index);
        return 1;
    }

    contenido_recomprimido = compress_buffer(
        (const unsigned char *)bloque_serializado,
        strlen(bloque_serializado),
        &recompressed_size
    );

    if (contenido_recomprimido == NULL) {
        printf("No se pudo recomprimir el bloque destino.\n");
        free(bloque_serializado);
        free_int_record_hash_table(record_table_destino);
        free(contenido_descomprimido);
        free(contenido_comprimido);
        liberar_comunas();
        free_global_record_index(global_index);
        return 1;
    }

    if (asegurar_directorio_comuna(id_comuna_destino) != 0) {
        printf("No se pudo asegurar el directorio de la comuna destino.\n");
        free(contenido_recomprimido);
        free(bloque_serializado);
        free_int_record_hash_table(record_table_destino);
        free(contenido_descomprimido);
        free(contenido_comprimido);
        liberar_comunas();
        free_global_record_index(global_index);
        return 1;
    }

    strncpy(ruta_salida, ruta_destino, sizeof(ruta_salida) - 1);
    ruta_salida[sizeof(ruta_salida) - 1] = '\0';

    write_result = escribir_archivo_binario(
        ruta_salida,
        contenido_recomprimido,
        recompressed_size
    );

    if (write_result != 0) {
        printf("No se pudo escribir el archivo destino.\n");
        free(contenido_recomprimido);
        free(bloque_serializado);
        free_int_record_hash_table(record_table_destino);
        free(contenido_descomprimido);
        free(contenido_comprimido);
        liberar_comunas();
        free_global_record_index(global_index);
        return 1;
    }

    printf("\n--- ARCHIVO SOBRESCRITO / CREADO ---\n");
    printf("Ruta archivo destino: %s\n", ruta_salida);
    printf("Tamano recomprimido: %zu bytes\n", recompressed_size);

    free(contenido_recomprimido);
    free(bloque_serializado);
    free_int_record_hash_table(record_table_destino);
    free(contenido_descomprimido);
    free(contenido_comprimido);
    liberar_comunas();
    free_global_record_index(global_index);

    return 0;
}

int ejecutar_menu_insercion(void) {
    int opcion;

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
                insertar_registro_puntual();
                break;
            case 2:
                printf("\n--- INSERCION MASIVA ---\n");
                printf("Esta opcion sera implementada en la siguiente capa.\n");
                break;
            case 3:
                return 0;
            default:
                printf("Opcion invalida.\n");
                break;
        }
    }
}