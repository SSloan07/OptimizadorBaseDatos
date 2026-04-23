#include "compress.h"
#include "dictionary.h"
#include <stdint.h>
#include <stdlib.h>

/* Escribe un código de 16 bits al archivo en little-endian.
   Retorna 0 en éxito, !=0 si falla la escritura. */
static int emit_code(uint32_t code, FILE *output) {
    uint8_t bytes[2];
    bytes[0] = (uint8_t)(code & 0xFF);          /* byte bajo */
    bytes[1] = (uint8_t)((code >> 8) & 0xFF);   /* byte alto */
    return fwrite(bytes, 1, 2, output) == 2 ? 0 : 1;
}

CompressStats lzw_compress(FILE *input, FILE *output) {
    CompressStats stats = {0, 0, 0};

    Dictionary *dict = dict_create();
    if (!dict) {
        stats.error = 1;
        return stats;
    }

    // 'w' representa el código de la cadena actual acumulada. INVALID_CODE significa "todavía no tenemos prefijo".
    uint32_t w = INVALID_CODE;
    int c;  // fgetc retorna int para poder distinguir EOF (-1)

    while ((c = fgetc(input)) != EOF) {
        stats.bytes_read++;
        uint8_t byte = (uint8_t)c;

        if (w == INVALID_CODE) {
            // Primer byte del archivo: se convierte en prefijo sin emitir
            w = byte;
            continue;
        }

        /* ¿Existe la cadena (w + byte) en el diccionario? */
        uint32_t combined = dict_lookup(dict, w, byte);

        if (combined != INVALID_CODE) {
            // Sí existe: extendemos el prefijo
            w = combined;
        } else {
            // No existe: emitimos w, insertamos (w+byte), y reiniciamos el prefijo con el byte actual
            if (emit_code(w, output) != 0) {
                stats.error = 2;
                dict_destroy(dict);
                return stats;
            }
            stats.codes_written++;

            /* dict_insert puede retornar INVALID_CODE si está lleno.
               En esa situación simplemente no agregamos más entradas,
               pero seguimos comprimiendo con el diccionario existente. */
            dict_insert(dict, w, byte);

            w = byte;
        }
    }

    /* Al terminar, queda un último prefijo por emitir (si leímos algo) */
    if (w != INVALID_CODE) {
        if (emit_code(w, output) != 0) {
            stats.error = 2;
        } else {
            stats.codes_written++;
        }
    }

    dict_destroy(dict);
    return stats;
}