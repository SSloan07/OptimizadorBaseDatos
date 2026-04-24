#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdio.h>
#include <stddef.h>

/* Resultado de una operación de compresión */
typedef struct {
    size_t bytes_read;      /* bytes leídos del archivo original */
    size_t codes_written;   /* códigos emitidos al archivo comprimido */
    int    error;           /* 0 = éxito, != 0 = error */
} CompressStats;

CompressStats lzw_compress(const unsigned char *input, size_t input_size, unsigned char *output, size_t output_cap, size_t *output_size);

#endif /* COMPRESS_H */