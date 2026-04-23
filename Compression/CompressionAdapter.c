#include <stdlib.h>
#include <string.h>
#include "CompressionAdapter.h"

unsigned char *compress_buffer(const unsigned char *input, size_t input_size, size_t *output_size) {
    unsigned char *output;

    if (input == NULL || output_size == NULL) {
        return NULL;
    }

    output = (unsigned char *)malloc(input_size);
    if (output == NULL) {
        return NULL;
    }

    memcpy(output, input, input_size);
    *output_size = input_size;

    return output;
}

unsigned char *decompress_buffer(const unsigned char *input, size_t input_size, size_t *output_size) {
    unsigned char *output;

    if (input == NULL || output_size == NULL) {
        return NULL;
    }

    output = (unsigned char *)malloc(input_size + 1);
    if (output == NULL) {
        return NULL;
    }

    memcpy(output, input, input_size);
    output[input_size] = '\0';
    *output_size = input_size;

    return output;
}