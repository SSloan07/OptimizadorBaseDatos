#include <stdio.h>
#include <stdlib.h>
#include "FileReader.h"

char *leer_archivo_completo(const char *ruta) {
    FILE *archivo = fopen(ruta, "rb");
    char *buffer;
    long tamano;

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

    buffer = (char *)malloc((size_t)tamano + 1);
    if (buffer == NULL) {
        fclose(archivo);
        return NULL;
    }

    if (fread(buffer, 1, (size_t)tamano, archivo) != (size_t)tamano) {
        free(buffer);
        fclose(archivo);
        return NULL;
    }

    buffer[tamano] = '\0';

    fclose(archivo);
    return buffer;
}