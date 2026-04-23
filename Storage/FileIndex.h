#ifndef FILE_INDEX_H
#define FILE_INDEX_H

#include <stddef.h>

/* Construye la ruta del archivo de un bloque dado */
void construir_ruta_bloque(int id_comuna, int id_bloque, char *buffer, size_t size);

#endif