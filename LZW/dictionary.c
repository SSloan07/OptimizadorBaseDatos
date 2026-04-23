#include "dictionary.h"
#include <stdlib.h>
#include <string.h>

/* Tamaño de la tabla hash. Usamos un primo mayor que MAX_DICT_SIZE
   para reducir colisiones. 75%-80% de factor de carga es razonable. */
#define HASH_TABLE_SIZE 98317  /* primo cercano a 65536 * 1.5 */

/* Cada entrada del diccionario es un par (prefix_code, byte) que
   representa una cadena. El 'code' es el índice que le asignamos. */
typedef struct {
    uint32_t prefix_code;
    uint8_t  byte;
    uint32_t code;       /* el código asignado a esta cadena */
    int      occupied;   /* 0 = slot vacío, 1 = ocupado */
} HashEntry;

/* Entrada compacta para búsqueda inversa (por código). */
typedef struct {
    uint32_t prefix_code;
    uint8_t  byte;
} Entry;

struct Dictionary {
    HashEntry *table;    /* tabla hash: búsqueda por (prefix, byte) → code */
    Entry     *entries;  /* array: búsqueda por code → (prefix, byte).
                            Índice code-256, porque 0-255 son literales. */
    size_t     count;    /* cuántas entradas hay (incluyendo las 256 iniciales) */
};

/* Función hash: combina prefix_code y byte en un índice de tabla.
   Usamos una mezcla simple pero efectiva (inspirada en FNV-1a). */
static size_t hash_key(uint32_t prefix_code, uint8_t byte) {
    uint64_t h = 2166136261u;
    h = (h ^ prefix_code) * 16777619u;
    h = (h ^ byte) * 16777619u;
    return (size_t)(h % HASH_TABLE_SIZE);
}

Dictionary *dict_create(void) {
    Dictionary *dict = malloc(sizeof(Dictionary));
    if (!dict) return NULL;

    /* calloc pone occupied=0 en todas las entradas automáticamente */
    dict->table = calloc(HASH_TABLE_SIZE, sizeof(HashEntry));
    if (!dict->table) {
        free(dict);
        return NULL;
    }

    /* Array para búsqueda inversa. Tamaño: MAX_DICT_SIZE - 256, porque
       los primeros 256 códigos son literales y no necesitan almacenarse. */
    dict->entries = calloc(MAX_DICT_SIZE - 256, sizeof(Entry));
    if (!dict->entries) {
        free(dict->table);
        free(dict);
        return NULL;
    }

    /* Los primeros 256 códigos (0-255) son reservados para bytes ASCII.
       No los insertamos en la tabla hash: como son cadenas de un solo
       byte, el "código" ES el byte. Los tratamos como caso especial. */
    dict->count = 256;
    return dict;
}

void dict_destroy(Dictionary *dict) {
    if (!dict) return;
    free(dict->table);
    free(dict->entries);
    free(dict);
}

uint32_t dict_lookup(const Dictionary *dict,
                     uint32_t prefix_code,
                     uint8_t  next_byte) {
    /* Sondeo lineal (linear probing) para resolver colisiones */
    size_t idx = hash_key(prefix_code, next_byte);

    for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
        size_t probe = (idx + i) % HASH_TABLE_SIZE;
        HashEntry *entry = &dict->table[probe];

        if (!entry->occupied) {
            return INVALID_CODE;  /* slot vacío = no existe */
        }
        if (entry->prefix_code == prefix_code && entry->byte == next_byte) {
            return entry->code;   /* encontrado */
        }
        /* si no, sigue sondeando */
    }
    return INVALID_CODE;  /* tabla llena y no se encontró */
}

uint32_t dict_insert(Dictionary *dict,
                     uint32_t prefix_code,
                     uint8_t  next_byte) {
    if (dict->count >= MAX_DICT_SIZE) {
        return INVALID_CODE;  /* diccionario lleno */
    }

    size_t idx = hash_key(prefix_code, next_byte);

    for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
        size_t probe = (idx + i) % HASH_TABLE_SIZE;
        HashEntry *entry = &dict->table[probe];

        if (!entry->occupied) {
            entry->prefix_code = prefix_code;
            entry->byte        = next_byte;
            entry->code        = (uint32_t)dict->count;
            entry->occupied    = 1;

            /* También lo guardamos en el array de búsqueda inversa.
               El índice es code - 256 porque los primeros 256 son literales. */
            size_t idx_entries = dict->count - 256;
            dict->entries[idx_entries].prefix_code = prefix_code;
            dict->entries[idx_entries].byte        = next_byte;

            dict->count++;
            return entry->code;
        }
        /* Nota: no chequeamos duplicados aquí porque el algoritmo LZW
           siempre hace lookup antes de insert. Si quisieras un diccionario
           más defensivo, agregarías esa verificación. */
    }
    return INVALID_CODE;  /* imposible si HASH_TABLE_SIZE > MAX_DICT_SIZE */
}

size_t dict_size(const Dictionary *dict) {
    return dict->count;
}

int dict_get_entry(const Dictionary *dict,
                   uint32_t code,
                   uint32_t *out_prefix,
                   uint8_t  *out_byte) {
    if (code < 256) {
        /* Literal: no tiene prefijo, el byte es el código mismo */
        *out_prefix = INVALID_CODE;
        *out_byte   = (uint8_t)code;
        return 0;
    }
    if ((size_t)code >= dict->count) {
        return -1;  /* código fuera de rango */
    }
    size_t idx = code - 256;
    *out_prefix = dict->entries[idx].prefix_code;
    *out_byte   = dict->entries[idx].byte;
    return 0;
}