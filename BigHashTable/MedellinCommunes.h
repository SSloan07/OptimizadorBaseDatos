#ifndef MEDELLINCOMMUNES_H
#define MEDELLINCOMMUNES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uthash.h"

typedef struct {
    char nombre[50];
    int id;
    UT_hash_handle hh;
} Comuna;

extern Comuna *diccionario;

void inicializar_comunas();
int obtener_id_comuna(const char *nombre);

#endif