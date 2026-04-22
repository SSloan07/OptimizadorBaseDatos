#include <stdio.h>
#include <stdlib.h>
#include "MedellinCommunes.h"

StringIntHashTable *diccionario_comunas = NULL;

void inicializar_comunas(void) {
    diccionario_comunas = create_string_int_hash_table(31);
    if (diccionario_comunas == NULL) {
        fprintf(stderr, "Error: no se pudo crear la tabla hash de comunas.\n");
        exit(EXIT_FAILURE);
    }

    insert_string_int(diccionario_comunas, "popular", 1);
    insert_string_int(diccionario_comunas, "santa cruz", 2);
    insert_string_int(diccionario_comunas, "manrique", 3);
    insert_string_int(diccionario_comunas, "aranjuez", 4);
    insert_string_int(diccionario_comunas, "castilla", 5);
    insert_string_int(diccionario_comunas, "doce de octubre", 6);
    insert_string_int(diccionario_comunas, "robledo", 7);
    insert_string_int(diccionario_comunas, "villa hermosa", 8);
    insert_string_int(diccionario_comunas, "buenos aires", 9);
    insert_string_int(diccionario_comunas, "la candelaria", 10);
    insert_string_int(diccionario_comunas, "laureles", 11);
    insert_string_int(diccionario_comunas, "la america", 12);
    insert_string_int(diccionario_comunas, "san javier", 13);
    insert_string_int(diccionario_comunas, "el poblado", 14);
    insert_string_int(diccionario_comunas, "guayabal", 15);
    insert_string_int(diccionario_comunas, "belen", 16);
}

int obtener_id_comuna(const char *nombre) {
    return search_string_int(diccionario_comunas, nombre);
}

void liberar_comunas(void) {
    free_string_int_hash_table(diccionario_comunas);
    diccionario_comunas = NULL;
}