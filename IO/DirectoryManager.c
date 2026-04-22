#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "DirectoryManager.h"

int asegurar_directorio_comuna(int id_comuna) {
    char ruta[128];

    if (mkdir("data", 0777) == -1 && errno != EEXIST) {
        return -1;
    }

    snprintf(ruta, sizeof(ruta), "data/comuna_%d", id_comuna);

    if (mkdir(ruta, 0777) == -1 && errno != EEXIST) {
        return -1;
    }

    return 0;
}