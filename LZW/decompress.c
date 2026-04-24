#include "decompress.h"
#include "dictionary.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Reconstruye la cadena de 'code' en buffer, de atrás hacia adelante. */
static int decode_string(const Dictionary *dict, uint32_t code,
                         uint8_t *buffer, size_t buffer_capacity,
                         uint8_t **out_start, size_t *out_len) {
    uint8_t *ptr = buffer + buffer_capacity;
    size_t   len = 0;
    uint32_t current = code;

    while (current != INVALID_CODE) {
        uint32_t prefix;
        uint8_t  byte;

        if (dict_get_entry(dict, current, &prefix, &byte) != 0)
            return -1;
        if (len >= buffer_capacity)
            return -1;

        ptr--;
        *ptr = byte;
        len++;
        current = prefix;
    }

    *out_start = ptr;
    *out_len   = len;
    return 0;
}

DecompressStats lzw_decompress(const unsigned char *input, size_t input_size,
                               unsigned char *output, size_t output_cap,
                               size_t *output_size) {
    DecompressStats stats = {0, 0, 0};

    if (!input || !output || !output_size) {
        stats.error = 1;
        return stats;
    }

    Dictionary *dict = dict_create();
    if (!dict) {
        stats.error = 1;
        return stats;
    }

    size_t buf_cap = MAX_DICT_SIZE + 1;
    uint8_t *buffer = malloc(buf_cap);
    if (!buffer) {
        dict_destroy(dict);
        stats.error = 1;
        return stats;
    }

    size_t in_pos  = 0;
    size_t out_pos = 0;

    /* --- Primer código --- */
    if (in_pos + 2 > input_size) goto cleanup;

    uint32_t prev_code = (uint32_t)input[in_pos] | ((uint32_t)input[in_pos+1] << 8);
    in_pos += 2;
    stats.codes_read++;

    uint8_t *str_start;
    size_t   str_len;

    if (decode_string(dict, prev_code, buffer, buf_cap, &str_start, &str_len) != 0) {
        stats.error = 2;
        goto cleanup;
    }
    if (out_pos + str_len > output_cap) { stats.error = 2; goto cleanup; }
    memcpy(output + out_pos, str_start, str_len);
    out_pos += str_len;
    stats.bytes_written += str_len;

    /* --- Bucle principal --- */
    while (in_pos + 2 <= input_size) {
        uint32_t curr_code = (uint32_t)input[in_pos] | ((uint32_t)input[in_pos+1] << 8);
        in_pos += 2;
        stats.codes_read++;

        uint8_t first_byte;

        if ((size_t)curr_code < dict_size(dict)) {
            if (decode_string(dict, curr_code, buffer, buf_cap, &str_start, &str_len) != 0) {
                stats.error = 2;
                goto cleanup;
            }
            first_byte = str_start[0];
        } else if ((size_t)curr_code == dict_size(dict)) {
            /* Caso especial: el código es el que estamos por insertar */
            if (decode_string(dict, prev_code, buffer, buf_cap, &str_start, &str_len) != 0) {
                stats.error = 2;
                goto cleanup;
            }
            str_start[str_len] = str_start[0];
            str_len++;
            first_byte = str_start[0];
        } else {
            stats.error = 3;  /* archivo corrupto */
            goto cleanup;
        }

        if (out_pos + str_len > output_cap) { stats.error = 2; goto cleanup; }
        memcpy(output + out_pos, str_start, str_len);
        out_pos += str_len;
        stats.bytes_written += str_len;

        if (dict_size(dict) < MAX_DICT_SIZE)
            dict_insert(dict, prev_code, first_byte);

        prev_code = curr_code;
    }

cleanup:
    *output_size = out_pos;

    // printf("Comprimido:    %zu bytes\n", input_size);
    // printf("Descomprimido: %zu bytes\n", out_pos);
    if (out_pos > 0)
        // printf("Tasa:          %.2f%%\n", 100.0 * input_size / out_pos);

    free(buffer);
    dict_destroy(dict);
    return stats;
}