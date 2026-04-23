#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdint.h>
#include <stddef.h>

/* Código máximo: 16 bits = 65536 entradas (0 a 65535) */
#define MAX_DICT_SIZE 65536
#define INVALID_CODE  0xFFFFFFFF  /* Sentinela: "no encontrado" */

/* Tipo opaco: el usuario no ve los detalles internos */
typedef struct Dictionary Dictionary;

/* Crea un diccionario vacío, pre-cargado con los 256 códigos ASCII.
   Retorna NULL si falla la asignación de memoria. */
Dictionary *dict_create(void);

/* Libera toda la memoria del diccionario. */
void dict_destroy(Dictionary *dict);

/* Busca la cadena (prefix_code + next_byte) en el diccionario.
   - prefix_code: código de una cadena ya existente en el diccionario
   - next_byte:   el siguiente byte que se concatena
   
   Retorna el código de la cadena concatenada si existe,
   o INVALID_CODE si no existe. */
uint32_t dict_lookup(const Dictionary *dict,
                     uint32_t prefix_code,
                     uint8_t  next_byte);

/* Inserta (prefix_code + next_byte) en el diccionario.
   Retorna el nuevo código asignado, o INVALID_CODE si el diccionario
   está lleno (alcanzó MAX_DICT_SIZE). */
uint32_t dict_insert(Dictionary *dict,
                     uint32_t prefix_code,
                     uint8_t  next_byte);

/* Retorna el número actual de entradas en el diccionario. */
size_t dict_size(const Dictionary *dict);

/* Búsqueda inversa: dado un código >= 256, obtiene su (prefix, byte).
   Para códigos < 256, el prefix se retorna como INVALID_CODE y byte = code
   (son bytes literales).

   Retorna 0 en éxito, -1 si el código no existe en el diccionario. */
int dict_get_entry(const Dictionary *dict,
                   uint32_t code,
                   uint32_t *out_prefix,
                   uint8_t  *out_byte);

#endif /* DICTIONARY_H */