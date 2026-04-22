#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "CommuneBlockManager.h"

static int extraer_numero_bloque(const char *nombre_archivo) {
    int numero;

    if (sscanf(nombre_archivo, "bloque_%d.txt", &numero) == 1) {
        return numero;
    }

    return -1;
}

static int contar_registros_en_archivo(const char *ruta) {
    FILE *archivo;
    int count = 0;
    char linea[512];

    archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        return -1;
    }

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        if (linea[0] != '\n' && linea[0] != '\0') {
            count++;
        }
    }

    fclose(archivo);
    return count;
}

int obtener_bloque_activo_por_comuna(int id_comuna) {
    char ruta_directorio[128];
    char ruta_archivo[256];
    DIR *dir;
    struct dirent *entry;
    int ultimo_bloque = -1;
    int registros_en_ultimo;

    if (id_comuna < 1) {
        return -1;
    }

    snprintf(ruta_directorio, sizeof(ruta_directorio), "data/comuna_%d", id_comuna);

    dir = opendir(ruta_directorio);
    if (dir == NULL) {
        /* Si la carpeta no existe, empezamos desde bloque 0 */
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) {
        int bloque = extraer_numero_bloque(entry->d_name);
        if (bloque > ultimo_bloque) {
            ultimo_bloque = bloque;
        }
    }

    closedir(dir);

    if (ultimo_bloque == -1) {
        return 0;
    }

    snprintf(
        ruta_archivo,
        sizeof(ruta_archivo),
        "data/comuna_%d/bloque_%d.txt",
        id_comuna,
        ultimo_bloque
    );

    registros_en_ultimo = contar_registros_en_archivo(ruta_archivo);

    if (registros_en_ultimo == -1) {
        return -1;
    }

    if (registros_en_ultimo >= MAX_REGISTROS_POR_BLOQUE) {
        return ultimo_bloque + 1;
    }

    return ultimo_bloque;
}