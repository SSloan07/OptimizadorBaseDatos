#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <stdio.h>
#include <stddef.h>

typedef struct {
    size_t codes_read;      /* códigos leídos del archivo comprimido */
    size_t bytes_written;   /* bytes escritos al archivo de salida */
    int    error;           /* 0 = éxito, != 0 = error */
} DecompressStats;

/* Descomprime 'input' y escribe el resultado en 'output'.
   El formato esperado es el mismo que produce lzw_compress:
   secuencia de uint16_t en little-endian. */
DecompressStats lzw_decompress(const unsigned char *input, size_t input_size, unsigned char *output, size_t output_cap, size_t *output_size);
#endif /* DECOMPRESS_H */