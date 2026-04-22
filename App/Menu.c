#include <stdio.h>
#include "Menu.h"

int mostrar_menu_principal(void) {
    int opcion;

    printf("\n===== MENU PRINCIPAL =====\n");
    printf("1. Buscar un registro puntual\n");
    printf("2. Insertar registros masivamente\n");
    printf("3. Salir\n");
    printf("Seleccione una opcion: ");

    if (scanf("%d", &opcion) != 1) {
        return -1;
    }

    return opcion;
}