#ifndef COMPRESSION_ADAPTER_H
#define COMPRESSION_ADAPTER_H

#include <stddef.h>

unsigned char *compress_buffer(const unsigned char *input, size_t input_size, size_t *output_size);
unsigned char *decompress_buffer(const unsigned char *input, size_t input_size, size_t *output_size);

#endif