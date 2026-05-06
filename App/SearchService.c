#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SearchService.h"
#include "BenchmarkUtils.h"

#include "../Storage/FileIndex.h"
#include "../Storage/CommuneBlockManager.h"
#include "../IO/FileReader.h"
#include "../Compression/CompressionAdapter.h"
#include "../Data/BlockLoader.h"
#include "../Data/IntRecordHashTable.h"

#include "../Encryption/Encripter.h"
#include "../AES/AesEncripter.h"

#define RECORD_TABLE_SIZE 1009

// Clave (Mala practica) pero pues como versión inicial la pusimos así. 
static const unsigned char AES_PROJECT_KEY[] = "CSF123CSF123CSF123CSF123CS12CSF1";

_Static_assert(sizeof(AES_PROJECT_KEY) - 1 == 32, "La clave AES-256 debe tener exactamente 32 caracteres");

/* Línea de fase con tiempo en ms (búsquedas puntuales sub-segundo). */
static void print_phase(const char *label, double seconds, double total, int width) {
    double pct = (total > 0.0) ? (seconds / total) * 100.0 : 0.0;
    int filled = (total > 0.0) ? (int)((seconds / total) * width + 0.5) : 0;
    if (filled > width) filled = width;
    if (filled < 0) filled = 0;

    printf("  %s%-22s%s %s%8.4f ms%s  %s%5.1f%%%s  %s",
           C_BOLD, label, C_RESET,
           C_CYAN, seconds * 1000.0, C_RESET,
           C_YELLOW, pct, C_RESET,
           C_GREEN);
    for (int i = 0; i < filled; i++) putchar('#');
    printf("%s", C_DIM);
    for (int i = filled; i < width; i++) putchar('.');
    printf("%s\n", C_RESET);
}

int ejecutar_busqueda_puntual(AppContext *ctx) {
    int id_registro;
    char ruta[256];

    RecordLocation *location = NULL;
    Encripter *encripter = NULL;

    unsigned char *contenido_archivo = NULL;
    unsigned char *contenido_descifrado = NULL;
    unsigned char *contenido_descomprimido = NULL;

    size_t file_size = 0;
    size_t decrypted_size = 0;
    size_t decompressed_size = 0;

    IntRecordHashTable *record_table = NULL;
    Record *record = NULL;

    double t0, t1, t2, t3, t4, t5, t6;
    double dt_index, dt_read, dt_decrypt, dt_decompress, dt_load, dt_search, dt_total;

    int status = 1;

    if (ctx == NULL || ctx->global_index == NULL) {
        fprintf(stderr, "%sContexto de aplicacion invalido.%s\n", C_RED, C_RESET);
        return 1;
    }

    printf("%sIngrese el id global del registro a buscar:%s ", C_BOLD, C_RESET);
    if (scanf("%d", &id_registro) != 1) {
        fprintf(stderr, "%sError al leer el id.%s\n", C_RED, C_RESET);
        return 1;
    }

    /* ---------- Pipeline cronometrado ---------- */
    t0 = now_sec();
    location = search_global_record_index(ctx->global_index, id_registro);
    t1 = now_sec();

    if (location == NULL) {
        printf("%sRegistro no encontrado en el indice global.%s\n", C_YELLOW, C_RESET);
        return 1;
    }

    construir_ruta_bloque(location->id_comuna, location->id_bloque, ruta, sizeof(ruta));

    encripter = create_aes_256_gcm_encripter((const unsigned char *)AES_PROJECT_KEY);
    if (encripter == NULL) {
        fprintf(stderr, "%sNo se pudo crear el encripter AES.%s\n", C_RED, C_RESET);
        goto cleanup;
    }

    contenido_archivo = leer_archivo_binario(ruta, &file_size);
    t2 = now_sec();
    if (contenido_archivo == NULL) {
        printf("%sNo se pudo leer el archivo del bloque.%s\n", C_RED, C_RESET);
        goto cleanup;
    }

    contenido_descifrado = encripter_decrypt(
        encripter, contenido_archivo, file_size, &decrypted_size);
    t3 = now_sec();
    if (contenido_descifrado == NULL) {
        printf("%sNo se pudo descifrar el bloque.%s\n", C_RED, C_RESET);
        goto cleanup;
    }

    contenido_descomprimido = decompress_buffer(
        contenido_descifrado, decrypted_size, &decompressed_size);
    t4 = now_sec();
    if (contenido_descomprimido == NULL) {
        printf("%sNo se pudo descomprimir el bloque.%s\n", C_RED, C_RESET);
        goto cleanup;
    }

    record_table = cargar_bloque_en_hash_table(
        (const char *)contenido_descomprimido, RECORD_TABLE_SIZE);
    t5 = now_sec();
    if (record_table == NULL) {
        printf("%sNo se pudo cargar el bloque en memoria.%s\n", C_RED, C_RESET);
        goto cleanup;
    }

    record = search_int_record(record_table, id_registro);
    t6 = now_sec();

    /* ---------- Cálculos ---------- */
    dt_index      = t1 - t0;
    dt_read       = t2 - t1;
    dt_decrypt    = t3 - t2;
    dt_decompress = t4 - t3;
    dt_load       = t5 - t4;
    dt_search     = t6 - t5;
    dt_total      = t6 - t0;

    /* Throughputs en MB/s. Protegidos contra división por cero. */
    double mb_decrypt    = (dt_decrypt    > 0.0) ? (file_size       / (1024.0 * 1024.0)) / dt_decrypt    : 0.0;
    double mb_decompress = (dt_decompress > 0.0) ? (decrypted_size  / (1024.0 * 1024.0)) / dt_decompress : 0.0;
    double mb_read       = (dt_read       > 0.0) ? (file_size       / (1024.0 * 1024.0)) / dt_read       : 0.0;

    /* Ratio de compresión: cuántas veces creció el bloque al descomprimir.
     * Mayor = mejor compresión original. */
    double compression_ratio = (decrypted_size > 0)
        ? (double)decompressed_size / (double)decrypted_size : 0.0;

    /* Overhead del cifrado: AES-GCM añade tag + IV. Diferencia entre el
     * archivo en disco y el payload comprimido real. */
    long crypto_overhead = (long)file_size - (long)decrypted_size;

    /* Factor de carga de la hash table tras la inserción. Si está muy
     * por encima de 1.0, RECORD_TABLE_SIZE quedó corto. */
    double load_factor = (RECORD_TABLE_SIZE > 0)
        ? (double)record_table->size / (double)RECORD_TABLE_SIZE : 0.0;

    char buf_file[32], buf_decrypted[32], buf_decompressed[32];
    format_bytes(file_size,         buf_file,         sizeof(buf_file));
    format_bytes(decrypted_size,    buf_decrypted,    sizeof(buf_decrypted));
    format_bytes(decompressed_size, buf_decompressed, sizeof(buf_decompressed));

    /* ---------- Salida formateada ---------- */
    printf("\n");
    printf("%s%s╔══════════════════════════════════════════════════════════════╗%s\n", C_BOLD, C_BLUE, C_RESET);
    printf("%s%s║                  RESULTADO DE BUSQUEDA                       ║%s\n", C_BOLD, C_BLUE, C_RESET);
    printf("%s%s╚══════════════════════════════════════════════════════════════╝%s\n", C_BOLD, C_BLUE, C_RESET);

    printf("  %sUbicacion%s    Comuna %s%d%s, Bloque %s%d%s\n",
           C_BOLD, C_RESET, C_CYAN, location->id_comuna, C_RESET,
           C_CYAN, location->id_bloque, C_RESET);
    printf("  %sRuta%s         %s%s%s\n", C_BOLD, C_RESET, C_DIM, ruta, C_RESET);

    if (record == NULL) {
        printf("  %sEstado%s       %sRegistro no encontrado dentro del bloque%s\n",
               C_BOLD, C_RESET, C_YELLOW, C_RESET);
    } else {
        printf("  %sEstado%s       %s✓ Encontrado%s\n", C_BOLD, C_RESET, C_GREEN, C_RESET);
        printf("  %sID%s           %d\n", C_BOLD, C_RESET, record->id);
        printf("  %sNombre%s       %s\n", C_BOLD, C_RESET, record->nombre);
        printf("  %sEdad%s         %d\n", C_BOLD, C_RESET, record->edad);
        printf("  %sEscolaridad%s  %s\n", C_BOLD, C_RESET, record->escolaridad);
        printf("  %sComuna%s       %s\n", C_BOLD, C_RESET, record->comuna);
    }

    printf("\n%s%s╔══════════════════════════════════════════════════════════════╗%s\n", C_BOLD, C_MAGENTA, C_RESET);
    printf("%s%s║                  DESGLOSE POR FASE                           ║%s\n", C_BOLD, C_MAGENTA, C_RESET);
    printf("%s%s╚══════════════════════════════════════════════════════════════╝%s\n", C_BOLD, C_MAGENTA, C_RESET);

    print_phase("Indice global",    dt_index,      dt_total, 20);
    print_phase("Lectura de bloque", dt_read,       dt_total, 20);
    print_phase("Descifrado AES",    dt_decrypt,    dt_total, 20);
    print_phase("Descompresion",     dt_decompress, dt_total, 20);
    print_phase("Carga hash table",  dt_load,       dt_total, 20);
    print_phase("Busqueda final",    dt_search,     dt_total, 20);
    printf("  %s%-22s %s%8.4f ms%s\n",
           C_BOLD, "TOTAL", C_GREEN, dt_total * 1000.0, C_RESET);

    printf("\n%s%s╔══════════════════════════════════════════════════════════════╗%s\n", C_BOLD, C_CYAN, C_RESET);
    printf("%s%s║                  METRICAS DE PIPELINE                        ║%s\n", C_BOLD, C_CYAN, C_RESET);
    printf("%s%s╚══════════════════════════════════════════════════════════════╝%s\n", C_BOLD, C_CYAN, C_RESET);

    printf("  %sTamanos%s\n", C_BOLD, C_RESET);
    printf("    Disco (cifrado)        %s%s%s\n", C_CYAN, buf_file, C_RESET);
    printf("    Tras descifrar         %s%s%s\n", C_CYAN, buf_decrypted, C_RESET);
    printf("    Tras descomprimir      %s%s%s\n", C_CYAN, buf_decompressed, C_RESET);
    printf("    Overhead cripto        %s%+ld B%s   %s(IV + tag GCM)%s\n",
           C_YELLOW, crypto_overhead, C_RESET, C_DIM, C_RESET);
    printf("    Ratio de compresion    %s%.2fx%s   %s(descomprimido/comprimido)%s\n",
           C_GREEN, compression_ratio, C_RESET, C_DIM, C_RESET);

    printf("\n  %sThroughput%s\n", C_BOLD, C_RESET);
    printf("    Lectura disco          %s%7.2f MB/s%s\n", C_CYAN, mb_read, C_RESET);
    printf("    Descifrado AES-GCM     %s%7.2f MB/s%s\n", C_CYAN, mb_decrypt, C_RESET);
    printf("    Descompresion LZW      %s%7.2f MB/s%s\n", C_CYAN, mb_decompress, C_RESET);

    printf("\n  %sHash table%s\n", C_BOLD, C_RESET);
    printf("    Registros cargados     %s%d%s\n", C_CYAN, record_table->size, C_RESET);
    printf("    Capacidad (slots)      %s%d%s\n", C_CYAN, RECORD_TABLE_SIZE, C_RESET);
    printf("    Factor de carga        %s%.3f%s   %s%s%s\n",
           (load_factor > 0.75) ? C_YELLOW : C_GREEN, load_factor, C_RESET,
           C_DIM,
           (load_factor > 0.75) ? "(considerar aumentar tamano)" : "(saludable)",
           C_RESET);

    printf("\n");
    status = 0;

cleanup:
    if (record_table != NULL) {
        free_int_record_hash_table(record_table);
    }
    free(contenido_descomprimido);
    free(contenido_descifrado);
    free(contenido_archivo);
    encripter_destroy(encripter);

    return status;
}