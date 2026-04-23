#ifndef MEDELLIN_COMMUNES_H
#define MEDELLIN_COMMUNES_H

#include "../DataStructures/StringIntHashTable.h"

extern StringIntHashTable *diccionario_comunas;

void inicializar_comunas(void);
int obtener_id_comuna(const char *nombre);
void liberar_comunas(void);

#endif