#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SyntheticBatchInsert.h"
#include "SyntheticGenerator.h"
#include "BatchGroup.h"
#include "BatchProcessor.h"

#include "../GlobalIndex/GlobalRecordIndex.h"
#include "../GlobalIndex/GlobalIndexPersistence.h"
#include "../GlobalIndex/IdGenerator.h"
#include "../Storage/CommuneBlockManager.h"
#include "../BigHashTable/MedellinCommunes.h"

#define GLOBAL_INDEX_FILE "data/global_index.txt"
#define TOTAL_COMUNAS 16
#define CHUNK_SIZE 100000

int ejecutar_insercion_masiva_sintetica(AppContext *ctx, int cantidad) {
    clock_t inicio, fin;
    double tiempo_total;

    int siguiente_id;
    int procesados = 0;
    int bloque_actual_por_comuna[TOTAL_COMUNAS + 1];
    int cantidad_en_bloque_actual[TOTAL_COMUNAS + 1];
    int i;

    if (ctx == NULL || ctx->global_index == NULL) {
        fprintf(stderr, "Contexto de aplicacion invalido.\n");
        return 1;
    }

    inicio = clock();

    if (cantidad <= 0) {
        printf("La cantidad debe ser mayor que 0.\n");
        return 1;
    }

    for (i = 0; i <= TOTAL_COMUNAS; i++) {
        bloque_actual_por_comuna[i] = -1;
        cantidad_en_bloque_actual[i] = 0;
    }

    inicializar_comunas();

    siguiente_id = obtener_siguiente_id_global(ctx->global_index);
    if (siguiente_id == -1) {
        liberar_comunas();
        return 1;
    }

    while (procesados < cantidad) {
        int este_chunk = CHUNK_SIZE;
        GroupNode *groups = NULL;

        if (cantidad - procesados < CHUNK_SIZE) {
            este_chunk = cantidad - procesados;
        }

        for (i = 0; i < este_chunk; i++) {
            Record r;
            int id_comuna;
            int id_bloque;

            r = generar_registro_sintetico();
            r.id = siguiente_id++;

            id_comuna = obtener_id_comuna(r.comuna);
            if (id_comuna == -1) {
                liberar_grupos(groups);
                liberar_comunas();
                return 1;
            }

            if (bloque_actual_por_comuna[id_comuna] == -1) {
                bloque_actual_por_comuna[id_comuna] = obtener_bloque_activo_por_comuna(id_comuna);
                if (bloque_actual_por_comuna[id_comuna] == -1) {
                    liberar_grupos(groups);
                    liberar_comunas();
                    return 1;
                }
                cantidad_en_bloque_actual[id_comuna] = 0;
            }

            id_bloque = bloque_actual_por_comuna[id_comuna];

            if (cantidad_en_bloque_actual[id_comuna] >= MAX_REGISTROS_POR_BLOQUE) {
                bloque_actual_por_comuna[id_comuna]++;
                cantidad_en_bloque_actual[id_comuna] = 0;
                id_bloque = bloque_actual_por_comuna[id_comuna];
            }

            if (agregar_record_a_grupo(&groups, r, id_comuna, id_bloque) != 0) {
                liberar_grupos(groups);
                liberar_comunas();
                return 1;
            }

            cantidad_en_bloque_actual[id_comuna]++;

            {
                RecordLocation loc;
                loc.id_registro = r.id;
                loc.id_comuna = id_comuna;
                loc.id_bloque = id_bloque;
                insert_global_record_index(ctx->global_index, r.id, loc);
            }
        }

        if (procesar_grupos_batch(groups) != 0) {
            liberar_grupos(groups);
            liberar_comunas();
            return 1;
        }

        liberar_grupos(groups);

        procesados += este_chunk;
        if (procesados % 100000 == 0 || procesados == cantidad) {
            printf("Procesados: %d / %d\n", procesados, cantidad);
        }
    }

    if (guardar_indice_global_en_archivo(ctx->global_index, GLOBAL_INDEX_FILE) != 0) {
        liberar_comunas();
        return 1;
    }

    fin = clock();
    tiempo_total = (double)(fin - inicio) / CLOCKS_PER_SEC;

    printf("\nInsercion masiva sintetica completada: %d registros.\n", cantidad);
    printf("\n--- METRICAS DE EJECUCION ---\n");
    printf("Registros insertados: %d\n", cantidad);
    printf("Tiempo total: %.4f segundos\n", tiempo_total);

    if (tiempo_total > 0.0) {
        printf("Throughput: %.2f registros/segundo\n", cantidad / tiempo_total);
    }

    liberar_comunas();
    return 0;
}