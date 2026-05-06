#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SyntheticBatchInsert.h"
#include "SyntheticGenerator.h"
#include "BatchGroup.h"
#include "BatchProcessor.h"
#include "../App/BenchmarkUtils.h"

#include "../GlobalIndex/GlobalRecordIndex.h"
#include "../GlobalIndex/GlobalIndexPersistence.h"
#include "../GlobalIndex/IdGenerator.h"
#include "../Storage/CommuneBlockManager.h"
#include "../BigHashTable/MedellinCommunes.h"

#define GLOBAL_INDEX_FILE "data/global_index.txt"
#define TOTAL_COMUNAS 16
#define CHUNK_SIZE 100000

/* Requiere las macros de color y la función now_sec() definidas en
 * SearchService.c. Si están en otro archivo, mover a un header común
 * (p.ej. App/BenchmarkUtils.h). */

/* Estadísticas acumuladas de chunks. Se actualizan en cada iteración
 * del while principal y se reportan al final. */
typedef struct {
    int    n_chunks;
    double t_total_chunks;
    double t_min_chunk;
    double t_max_chunk;

    double t_generate;       /* generar_registro_sintetico */
    double t_route;          /* obtener_id_comuna + lógica de bloque */
    double t_index_insert;   /* insert_global_record_index */
    double t_group_add;      /* agregar_record_a_grupo */
    double t_batch;          /* procesar_grupos_batch (compresion + cifrado + IO) */
} ChunkStats;

/* Cuenta cuántos bloques nuevos se crearon por comuna durante esta
 * inserción. Útil para ver el balanceo del generador sintético. */
typedef struct {
    int registros[TOTAL_COMUNAS + 1];
    int bloques_nuevos[TOTAL_COMUNAS + 1];
} ComunaStats;

/* Imprime una barra de progreso en una sola línea (se sobrescribe con \r).
 * porcentaje en [0, 1]. */
static void print_progress_bar(double porcentaje, int width, int procesados, int total, double rate) {
    int filled = (int)(porcentaje * width + 0.5);
    if (filled > width) filled = width;
    if (filled < 0) filled = 0;

    printf("\r  %s[%s", C_BOLD, C_GREEN);
    for (int i = 0; i < filled; i++) putchar('#');
    printf("%s", C_DIM);
    for (int i = filled; i < width; i++) putchar('.');
    printf("%s%s] %s%5.1f%%%s  %s%d/%d%s  %s%.0f reg/s%s     ",
           C_RESET, C_BOLD,
           C_YELLOW, porcentaje * 100.0, C_RESET,
           C_CYAN, procesados, total, C_RESET,
           C_MAGENTA, rate, C_RESET);
    fflush(stdout);
}

int ejecutar_insercion_masiva_sintetica(AppContext *ctx, int cantidad) {
    int siguiente_id;
    int procesados = 0;
    int bloque_actual_por_comuna[TOTAL_COMUNAS + 1];
    int cantidad_en_bloque_actual[TOTAL_COMUNAS + 1];
    int i;

    ChunkStats cstats = {0};
    cstats.t_min_chunk = 1e18;
    cstats.t_max_chunk = 0.0;

    ComunaStats comuna_stats = {0};

    if (ctx == NULL || ctx->global_index == NULL) {
        fprintf(stderr, "%sContexto de aplicacion invalido.%s\n", C_RED, C_RESET);
        return 1;
    }

    if (cantidad <= 0) {
        printf("%sLa cantidad debe ser mayor que 0.%s\n", C_RED, C_RESET);
        return 1;
    }

    /* Banner inicial */
    printf("\n%s%s╔══════════════════════════════════════════════════════════════╗%s\n", C_BOLD, C_BLUE, C_RESET);
    printf("%s%s║              INSERCION MASIVA SINTETICA                      ║%s\n", C_BOLD, C_BLUE, C_RESET);
    printf("%s%s╚══════════════════════════════════════════════════════════════╝%s\n", C_BOLD, C_BLUE, C_RESET);
    printf("  %sObjetivo:%s %s%d%s registros\n", C_BOLD, C_RESET, C_CYAN, cantidad, C_RESET);
    printf("  %sChunk size:%s %s%d%s\n\n", C_BOLD, C_RESET, C_CYAN, CHUNK_SIZE, C_RESET);

    double t_inicio = now_sec();

    for (i = 0; i <= TOTAL_COMUNAS; i++) {
        bloque_actual_por_comuna[i] = -1;
        cantidad_en_bloque_actual[i] = 0;
    }

    double t_setup_0 = now_sec();
    inicializar_comunas();
    double t_setup_comunas = now_sec() - t_setup_0;

    siguiente_id = obtener_siguiente_id_global(ctx->global_index);
    if (siguiente_id == -1) {
        liberar_comunas();
        return 1;
    }

    while (procesados < cantidad) {
        int este_chunk = CHUNK_SIZE;
        GroupNode *groups = NULL;
        double tc0 = now_sec();

        if (cantidad - procesados < CHUNK_SIZE) {
            este_chunk = cantidad - procesados;
        }

        /* Acumuladores por chunk para sumar al final */
        double dt_gen = 0.0, dt_route = 0.0, dt_idx = 0.0, dt_grp = 0.0;

        for (i = 0; i < este_chunk; i++) {
            Record r;
            int id_comuna;
            int id_bloque;

            double tg0 = now_sec();
            r = generar_registro_sintetico();
            r.id = siguiente_id++;
            dt_gen += now_sec() - tg0;

            double tr0 = now_sec();
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
                comuna_stats.bloques_nuevos[id_comuna]++;
            }
            dt_route += now_sec() - tr0;

            double tag0 = now_sec();
            if (agregar_record_a_grupo(&groups, r, id_comuna, id_bloque) != 0) {
                liberar_grupos(groups);
                liberar_comunas();
                return 1;
            }
            cantidad_en_bloque_actual[id_comuna]++;
            dt_grp += now_sec() - tag0;

            double ti0 = now_sec();
            {
                RecordLocation loc;
                loc.id_registro = r.id;
                loc.id_comuna = id_comuna;
                loc.id_bloque = id_bloque;
                insert_global_record_index(ctx->global_index, r.id, loc);
            }
            dt_idx += now_sec() - ti0;

            comuna_stats.registros[id_comuna]++;
        }

        double tb0 = now_sec();
        if (procesar_grupos_batch(groups) != 0) {
            liberar_grupos(groups);
            liberar_comunas();
            return 1;
        }
        double dt_batch = now_sec() - tb0;

        liberar_grupos(groups);

        double tc_total = now_sec() - tc0;

        /* Acumular en stats globales */
        cstats.n_chunks++;
        cstats.t_total_chunks += tc_total;
        if (tc_total < cstats.t_min_chunk) cstats.t_min_chunk = tc_total;
        if (tc_total > cstats.t_max_chunk) cstats.t_max_chunk = tc_total;
        cstats.t_generate     += dt_gen;
        cstats.t_route        += dt_route;
        cstats.t_index_insert += dt_idx;
        cstats.t_group_add    += dt_grp;
        cstats.t_batch        += dt_batch;

        procesados += este_chunk;

        /* Barra de progreso (se actualiza cada chunk) */
        double elapsed = now_sec() - t_inicio;
        double rate = (elapsed > 0.0) ? procesados / elapsed : 0.0;
        print_progress_bar((double)procesados / cantidad, 30, procesados, cantidad, rate);
    }
    printf("\n\n");  /* Cerrar línea de progreso */

    double t_save0 = now_sec();
    if (guardar_indice_global_en_archivo(ctx->global_index, GLOBAL_INDEX_FILE) != 0) {
        liberar_comunas();
        return 1;
    }
    double t_save = now_sec() - t_save0;

    double tiempo_total = now_sec() - t_inicio;

    /* Tamaño del índice en disco — útil para saber si el formato es compacto */
    long index_size_bytes = 0;
    {
        FILE *f = fopen(GLOBAL_INDEX_FILE, "rb");
        if (f) {
            fseek(f, 0, SEEK_END);
            index_size_bytes = ftell(f);
            fclose(f);
        }
    }

    /* ---------- Reporte final ---------- */
    printf("%s%s╔══════════════════════════════════════════════════════════════╗%s\n", C_BOLD, C_GREEN, C_RESET);
    printf("%s%s║                  RESULTADO                                   ║%s\n", C_BOLD, C_GREEN, C_RESET);
    printf("%s%s╚══════════════════════════════════════════════════════════════╝%s\n", C_BOLD, C_GREEN, C_RESET);
    printf("  %sInsertados%s            %s%d%s registros\n", C_BOLD, C_RESET, C_CYAN, cantidad, C_RESET);
    printf("  %sTiempo total%s          %s%.3f s%s\n", C_BOLD, C_RESET, C_CYAN, tiempo_total, C_RESET);
    printf("  %sThroughput%s            %s%.0f reg/s%s   %s(%.2f µs por registro)%s\n",
           C_BOLD, C_RESET, C_GREEN, cantidad / tiempo_total, C_RESET,
           C_DIM, (tiempo_total / cantidad) * 1e6, C_RESET);

    printf("\n%s%s╔══════════════════════════════════════════════════════════════╗%s\n", C_BOLD, C_MAGENTA, C_RESET);
    printf("%s%s║                  DESGLOSE POR FASE                           ║%s\n", C_BOLD, C_MAGENTA, C_RESET);
    printf("%s%s╚══════════════════════════════════════════════════════════════╝%s\n", C_BOLD, C_MAGENTA, C_RESET);
    printf("  %s%-22s %9s    %5s   %s\n", C_DIM, "FASE", "TIEMPO", "%", "BARRA"); printf("%s", C_RESET);
    print_phase_line("Setup comunas",       t_setup_comunas,       tiempo_total, 25);
    print_phase_line("Generar registros",   cstats.t_generate,     tiempo_total, 25);
    print_phase_line("Rutear comuna/bloque", cstats.t_route,        tiempo_total, 25);
    print_phase_line("Agregar a grupo",     cstats.t_group_add,    tiempo_total, 25);
    print_phase_line("Insertar en indice",  cstats.t_index_insert, tiempo_total, 25);
    print_phase_line("Batch (zip+aes+io)",  cstats.t_batch,        tiempo_total, 25);
    print_phase_line("Guardar indice",      t_save,                tiempo_total, 25);

    /* Suma cubierta vs total — el residuo es overhead no medido (logging,
     * la propia instrumentación, etc.) */
    double covered = t_setup_comunas + cstats.t_generate + cstats.t_route +
                     cstats.t_group_add + cstats.t_index_insert + cstats.t_batch + t_save;
    double overhead = tiempo_total - covered;
    printf("  %s%-22s%s %s%9.3f s%s  %s%5.1f%%%s   %s(no instrumentado)%s\n",
           C_DIM, "Otros", C_RESET,
           C_DIM, overhead, C_RESET,
           C_DIM, (overhead / tiempo_total) * 100.0, C_RESET,
           C_DIM, C_RESET);

    printf("\n%s%s╔══════════════════════════════════════════════════════════════╗%s\n", C_BOLD, C_CYAN, C_RESET);
    printf("%s%s║                  ESTADISTICAS DE CHUNKS                      ║%s\n", C_BOLD, C_CYAN, C_RESET);
    printf("%s%s╚══════════════════════════════════════════════════════════════╝%s\n", C_BOLD, C_CYAN, C_RESET);
    double avg_chunk = (cstats.n_chunks > 0) ? cstats.t_total_chunks / cstats.n_chunks : 0.0;
    printf("  %sChunks procesados%s     %s%d%s\n", C_BOLD, C_RESET, C_CYAN, cstats.n_chunks, C_RESET);
    printf("  %sTiempo por chunk%s\n", C_BOLD, C_RESET);
    printf("    Minimo               %s%.4f s%s\n", C_GREEN,  cstats.t_min_chunk, C_RESET);
    printf("    Promedio             %s%.4f s%s\n", C_CYAN,   avg_chunk,           C_RESET);
    printf("    Maximo               %s%.4f s%s\n", C_YELLOW, cstats.t_max_chunk, C_RESET);
    /* Si max/min > 3, hay alta varianza — probable jitter de IO o GC del SO */
    double ratio = (cstats.t_min_chunk > 0.0) ? cstats.t_max_chunk / cstats.t_min_chunk : 0.0;
    printf("    Ratio max/min        %s%.2fx%s   %s%s%s\n",
           (ratio > 3.0) ? C_YELLOW : C_GREEN, ratio, C_RESET,
           C_DIM,
           (ratio > 3.0) ? "(alta varianza, probable jitter de IO)" : "(estable)",
           C_RESET);

    printf("\n%s%s╔══════════════════════════════════════════════════════════════╗%s\n", C_BOLD, C_BLUE, C_RESET);
    printf("%s%s║                  DISTRIBUCION POR COMUNA                     ║%s\n", C_BOLD, C_BLUE, C_RESET);
    printf("%s%s╚══════════════════════════════════════════════════════════════╝%s\n", C_BOLD, C_BLUE, C_RESET);
    printf("  %s%-8s %12s %12s %14s%s\n", C_DIM, "COMUNA", "REGISTROS", "BLOQUES+", "% DEL TOTAL", C_RESET);
    int total_bloques_nuevos = 0;
    int max_reg = 0, min_reg = 1 << 30;
    for (i = 1; i <= TOTAL_COMUNAS; i++) {
        if (comuna_stats.registros[i] > max_reg) max_reg = comuna_stats.registros[i];
        if (comuna_stats.registros[i] < min_reg) min_reg = comuna_stats.registros[i];
        total_bloques_nuevos += comuna_stats.bloques_nuevos[i];
    }
    for (i = 1; i <= TOTAL_COMUNAS; i++) {
        double pct = (cantidad > 0) ? (comuna_stats.registros[i] * 100.0) / cantidad : 0.0;
        printf("  %s%-8d%s %12d %12d %13.2f%%\n",
               C_CYAN, i, C_RESET,
               comuna_stats.registros[i],
               comuna_stats.bloques_nuevos[i],
               pct);
    }
    printf("  %s%-8s %12s %12d%s\n", C_BOLD, "TOTAL", "", total_bloques_nuevos, C_RESET);

    /* Detección de desbalance en el generador sintético */
    if (min_reg > 0) {
        double balance_ratio = (double)max_reg / (double)min_reg;
        printf("  %sBalance%s max/min      %s%.2fx%s   %s%s%s\n",
               C_BOLD, C_RESET,
               (balance_ratio > 2.0) ? C_YELLOW : C_GREEN, balance_ratio, C_RESET,
               C_DIM,
               (balance_ratio > 2.0) ? "(generador desbalanceado)" : "(distribucion uniforme)",
               C_RESET);
    }

    printf("\n%s%s╔══════════════════════════════════════════════════════════════╗%s\n", C_BOLD, C_MAGENTA, C_RESET);
    printf("%s%s║                  ALMACENAMIENTO                              ║%s\n", C_BOLD, C_MAGENTA, C_RESET);
    printf("%s%s╚══════════════════════════════════════════════════════════════╝%s\n", C_BOLD, C_MAGENTA, C_RESET);
    if (index_size_bytes > 0) {
        char buf[32];
        if (index_size_bytes < 1024) snprintf(buf, sizeof(buf), "%ld B", index_size_bytes);
        else if (index_size_bytes < 1024L*1024) snprintf(buf, sizeof(buf), "%.2f KB", index_size_bytes / 1024.0);
        else snprintf(buf, sizeof(buf), "%.2f MB", index_size_bytes / (1024.0 * 1024.0));
        printf("  %sIndice global%s         %s%s%s\n", C_BOLD, C_RESET, C_CYAN, buf, C_RESET);
        printf("  %sBytes por registro%s    %s%.1f B%s   %s(en indice)%s\n",
               C_BOLD, C_RESET, C_CYAN, (double)index_size_bytes / cantidad, C_RESET, C_DIM, C_RESET);
    }
    printf("  %sBloques nuevos creados%s %s%d%s   %s(~%.0f registros/bloque)%s\n",
           C_BOLD, C_RESET, C_CYAN, total_bloques_nuevos, C_RESET,
           C_DIM,
           (total_bloques_nuevos > 0) ? (double)cantidad / total_bloques_nuevos : 0.0,
           C_RESET);
    printf("\n");

    liberar_comunas();
    return 0;
}