#include <stdio.h>
#include <stdlib.h>
#include "FileReader.h"

unsigned char *leer_archivo_binario(const char *ruta, size_t *size) {
    FILE *archivo;
    unsigned char *buffer;
    long tamano;

    if (ruta == NULL || size == NULL) {
        return NULL;
    }

    archivo = fopen(ruta, "rb");
    if (archivo == NULL) {
        return NULL;
    }

    if (fseek(archivo, 0, SEEK_END) != 0) {
        fclose(archivo);
        return NULL;
    }

    tamano = ftell(archivo);
    if (tamano < 0) {
        fclose(archivo);
        return NULL;
    }

    if (fseek(archivo, 0, SEEK_SET) != 0) {
        fclose(archivo);
        return NULL;
    }

    buffer = (unsigned char *)malloc((size_t)tamano);
    if (buffer == NULL) {
        fclose(archivo);
        return NULL;
    }

    if (fread(buffer, 1, (size_t)tamano, archivo) != (size_t)tamano) {
        free(buffer);
        fclose(archivo);
        return NULL;
    }

    fclose(archivo);
    *size = (size_t)tamano;
    return buffer;
}