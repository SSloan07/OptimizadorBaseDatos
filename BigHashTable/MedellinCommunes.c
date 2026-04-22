#include "MedellinCommunes.h"

Comuna *diccionario_comunas = NULL;

static void insertar_comuna(const char *nombre, int id) {
    Comuna *comuna = malloc(sizeof(Comuna));
    if (comuna == NULL) {
        fprintf(stderr, "Error: no se pudo reservar memoria para una comuna.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(comuna->nombre, nombre);
    comuna->id = id;

    HASH_ADD_STR(diccionario_comunas, nombre, comuna);
}

void inicializar_comunas(void) {
    insertar_comuna("popular", 1);
    insertar_comuna("santa cruz", 2);
    insertar_comuna("manrique", 3);
    insertar_comuna("aranjuez", 4);
    insertar_comuna("castilla", 5);
    insertar_comuna("doce de octubre", 6);
    insertar_comuna("robledo", 7);
    insertar_comuna("villa hermosa", 8);
    insertar_comuna("buenos aires", 9);
    insertar_comuna("la candelaria", 10);
    insertar_comuna("laureles", 11);
    insertar_comuna("la america", 12);
    insertar_comuna("san javier", 13);
    insertar_comuna("poblado", 14);
    insertar_comuna("guayabal", 15);
    insertar_comuna("belen", 16);
}

int obtener_id_comuna(const char *nombre) {
    Comuna *comuna_encontrada = NULL;
    HASH_FIND_STR(diccionario_comunas, nombre, comuna_encontrada);

    if (comuna_encontrada == NULL) {
        return -1;
    }

    return comuna_encontrada->id;
}