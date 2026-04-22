#include "PartitionManager.h"

int calcular_bloque(int id_registro, int tam_bloque) {
    if (id_registro < 0 || tam_bloque <= 0) {
        return -1;
    }

    return id_registro / tam_bloque;
}