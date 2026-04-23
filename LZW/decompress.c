#include "decompress.h"
#include "dictionary.h"
#include <stdint.h>
#include <stdlib.h>

/* Lee un código de 16 bits desde el archivo (little-endian).
   Retorna el código (0-65535) o INVALID_CODE si se alcanzó EOF o error. */
static uint32_t read_code(FILE *input) {
    uint8_t bytes[2];
    if (fread(bytes, 1, 2, input) != 2) {
        return INVALID_CODE;  /* EOF o error */
    }
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8);
}

/* Reconstruye la cadena asociada a 'code' dentro de 'buffer' (de capacidad
   'buffer_capacity'). La cadena se reconstruye siguiendo la cadena de
   prefijos hacia atrás; por eso la escribimos de atrás hacia adelante.

   Parámetros de salida:
     *out_start = puntero al primer byte de la cadena dentro del buffer
     *out_len   = longitud de la cadena

   Retorna 0 en éxito, -1 si el código es inválido. */
static int decode_string(const Dictionary *dict,
                         uint32_t code,
                         uint8_t  *buffer,
                         size_t    buffer_capacity,
                         uint8_t **out_start,
                         size_t   *out_len) {
    /* Escribimos desde el final del buffer hacia atrás */
    uint8_t *ptr = buffer + buffer_capacity;
    size_t   len = 0;
    uint32_t current = code;

    while (current != INVALID_CODE) {
        uint32_t prefix;
        uint8_t  byte;

        if (dict_get_entry(dict, current, &prefix, &byte) != 0) {
            return -1;
        }
        if (len >= buffer_capacity) {
            return -1;  /* protección contra overflow */
        }

        ptr--;
        *ptr = byte;
        len++;
        current = prefix;
    }

    *out_start = ptr;
    *out_len   = len;
    return 0;
}

DecompressStats lzw_decompress(FILE *input, FILE *output) {
    DecompressStats stats = {0, 0, 0};

    Dictionary *dict = dict_create();
    if (!dict) {
        stats.error = 1;
        return stats;
    }

    /* La cadena más larga posible tiene a lo sumo MAX_DICT_SIZE bytes.
       Agregamos +1 de margen para el byte extra del caso especial. */
    size_t buf_cap = MAX_DICT_SIZE + 1;
    uint8_t *buffer = malloc(buf_cap);
    if (!buffer) {
        dict_destroy(dict);
        stats.error = 1;
        return stats;
    }

    /* --- Caso base: leer el primer código --- */
    uint32_t prev_code = read_code(input);
    if (prev_code == INVALID_CODE) {
        /* Archivo vacío: no es error, simplemente no hay nada que hacer */
        goto cleanup;
    }
    stats.codes_read++;

    uint8_t *str_start;
    size_t   str_len;

    if (decode_string(dict, prev_code, buffer, buf_cap,
                      &str_start, &str_len) != 0) {
        stats.error = 2;
        goto cleanup;
    }
    fwrite(str_start, 1, str_len, output);
    stats.bytes_written += str_len;

    /* --- Bucle principal --- */
    uint32_t curr_code;
    while ((curr_code = read_code(input)) != INVALID_CODE) {
        stats.codes_read++;

        uint8_t first_byte;

        if ((size_t)curr_code < dict_size(dict)) {
            /* Caso normal: el código ya existe */
            if (decode_string(dict, curr_code, buffer, buf_cap,
                              &str_start, &str_len) != 0) {
                stats.error = 2;
                goto cleanup;
            }
            first_byte = str_start[0];
        } else if ((size_t)curr_code == dict_size(dict)) {
            /* CASO ESPECIAL: el código es justo el que estamos por crear.
               La cadena resulta ser: prev_string + primer_byte(prev_string).
               Decodificamos prev_code y le añadimos su propio primer byte. */
            if (decode_string(dict, prev_code, buffer, buf_cap,
                              &str_start, &str_len) != 0) {
                stats.error = 2;
                goto cleanup;
            }
            /* Añadir el primer byte al final de la cadena */
            str_start[str_len] = str_start[0];
            str_len++;
            first_byte = str_start[0];
        } else {
            /* Código mayor que dict_size: archivo corrupto */
            stats.error = 3;
            goto cleanup;
        }

        fwrite(str_start, 1, str_len, output);
        stats.bytes_written += str_len;

        /* Agregar al diccionario: (prev_code, primer_byte_de_cadena_actual).
           Es el reflejo de lo que hacía el compresor. */
        if (dict_size(dict) < MAX_DICT_SIZE) {
            dict_insert(dict, prev_code, first_byte);
        }

        prev_code = curr_code;
    }

cleanup:
    free(buffer);
    dict_destroy(dict);
    return stats;
}