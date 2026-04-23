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

/* Comprime el contenido de 'input' y escribe el resultado en 'output'.
   Ambos archivos deben estar ya abiertos (input en lectura, output en
   escritura binaria).

   Formato de salida: secuencia de uint16_t en little-endian (el orden
   nativo de x86/ARM). Cada código ocupa exactamente 2 bytes. */
CompressStats lzw_compress(FILE *input, FILE *output);

#endif /* COMPRESS_H */