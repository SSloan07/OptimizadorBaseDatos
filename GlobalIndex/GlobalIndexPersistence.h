#ifndef GLOBAL_INDEX_PERSISTENCE_H
#define GLOBAL_INDEX_PERSISTENCE_H

#include "GlobalRecordIndex.h"

int guardar_indice_global_en_archivo(GlobalRecordIndex *index, const char *ruta);
int cargar_indice_global_desde_archivo(GlobalRecordIndex *index, const char *ruta);

#endif