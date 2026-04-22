#include "MedellinCommunes.h"

Comuna *diccionario = NULL;

static void insertar_comuna(const char *nombre, int id) {
    Comuna *c = malloc(sizeof(Comuna));
    if (c == NULL) {
        printf("Error al reservar memoria\n");
        exit(1);
    }

    strcpy(c->nombre, nombre);
    c->id = id;
    HASH_ADD_STR(diccionario, nombre, c);
}

void inicializar_comunas() {
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
    insertar_comuna("america", 12);
    insertar_comuna("san javier", 13);
    insertar_comuna("poblado", 14);
    insertar_comuna("guayabal", 15);
    insertar_comuna("belen", 16);
}

int obtener_id_comuna(const char *nombre) {
    Comuna *c;
    HASH_FIND_STR(diccionario, nombre, c);

    if (c != NULL) {
        return c->id;
    }

    return -1;
}