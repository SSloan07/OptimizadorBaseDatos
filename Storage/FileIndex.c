#include <stdio.h>
#include "FileIndex.h"

void construir_ruta_bloque(int id_comuna, int id_bloque, char *buffer, size_t size) {
    snprintf(buffer, size, "data/comuna_%d/bloque_%d.txt", id_comuna, id_bloque);
}