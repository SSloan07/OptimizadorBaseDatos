#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "CompressionAdapter.h"
#include "../LZW/compress.h"
#include "../LZW/decompress.h"

unsigned char *compress_buffer(const unsigned char *input, size_t input_size, size_t *output_size) {
    if (input == NULL || output_size == NULL) return NULL;

    // Worst case LZW: cada byte se emite como código de 2 bytes
    size_t out_cap = input_size * 2 + 2;
    unsigned char *output = malloc(out_cap);
    if (!output) return NULL;

    CompressStats stats = lzw_compress(input, input_size, output, out_cap, output_size);
    if (stats.error) {
        free(output);
        return NULL;
    }

    return output;
}

unsigned char *decompress_buffer(const unsigned char *input, size_t input_size, size_t *output_size) {
    if (input == NULL || output_size == NULL) return NULL;

    // LZW puede expandir bastante — usar factor más generoso
    size_t out_cap = input_size * 64;
    unsigned char *output = malloc(out_cap);
    if (!output) return NULL;

    DecompressStats stats = lzw_decompress(input, input_size, output, out_cap, output_size);

    if (stats.error) {
        free(output);
        return NULL;
    }

    return output;
}