#include <stdio.h>
#include <stdlib.h>

#include "FileWriter.h"

int escribir_archivo_binario(const char *ruta, const unsigned char *buffer, size_t size) {
    FILE *archivo;

    if (ruta == NULL || buffer == NULL) {
        return -1;
    }

    archivo = fopen(ruta, "wb");
    if (archivo == NULL) {
        return -1;
    }

    if (fwrite(buffer, 1, size, archivo) != size) {
        fclose(archivo);
        return -1;
    }

    fclose(archivo);
    return 0;
}