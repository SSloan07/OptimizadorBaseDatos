#ifndef BLOCK_LOADER_H
#define BLOCK_LOADER_H

#include "IntRecordHashTable.h"

IntRecordHashTable *cargar_bloque_en_hash_table(const char *contenido_bloque, int table_size);

#endif