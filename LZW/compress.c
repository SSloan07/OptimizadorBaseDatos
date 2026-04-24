#include "compress.h"
#include "dictionary.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

CompressStats lzw_compress(const unsigned char *input, size_t input_size,
                           unsigned char *output, size_t output_cap,
                           size_t *output_size) {
    CompressStats stats = {0, 0, 0};

    if (!input || !output || !output_size) {
        stats.error = 1;
        return stats;
    }

    Dictionary *dict = dict_create();
    if (!dict) {
        stats.error = 1;
        return stats;
    }

    size_t out_pos = 0;
    uint32_t w = INVALID_CODE;

    for (size_t i = 0; i < input_size; i++) {
        stats.bytes_read++;
        uint8_t byte = input[i];

        if (w == INVALID_CODE) {
            w = byte;
            continue;
        }

        uint32_t combined = dict_lookup(dict, w, byte);

        if (combined != INVALID_CODE) {
            w = combined;
        } else {
            /* Emitir w en little-endian */
            if (out_pos + 2 > output_cap) {
                stats.error = 2;
                dict_destroy(dict);
                return stats;
            }
            output[out_pos++] = (uint8_t)(w & 0xFF);
            output[out_pos++] = (uint8_t)((w >> 8) & 0xFF);
            stats.codes_written++;

            dict_insert(dict, w, byte);
            w = byte;
        }
    }

    /* Último prefijo pendiente */
    if (w != INVALID_CODE) {
        if (out_pos + 2 > output_cap) {
            stats.error = 2;
        } else {
            output[out_pos++] = (uint8_t)(w & 0xFF);
            output[out_pos++] = (uint8_t)((w >> 8) & 0xFF);
            stats.codes_written++;
        }
    }

    *output_size = out_pos;

    // printf("Original:   %zu bytes\n", input_size);
    // printf("Comprimido: %zu bytes\n", out_pos);
    if (input_size > 0)
        // printf("Tasa:       %.2f%%\n", 100.0 * out_pos / input_size);

    dict_destroy(dict);
    return stats;
}