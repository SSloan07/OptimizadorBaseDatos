#include <stdio.h>
#include "App/Menu.h"
#include "App/SearchService.h"
#include "App/InsertService.h"

int main(void) {
    int opcion;
    int salir = 0;

    while (!salir) {
        opcion = mostrar_menu_principal();

        switch (opcion) {
            case 1:
                ejecutar_busqueda_puntual();
                break;
            case 2:
                ejecutar_menu_insercion();
                break;
            case 3:
                salir = 1;
                printf("Saliendo del programa.\n");
                break;
            default:
                printf("Opcion invalida.\n");
                break;
        }
    }

    return 0;
}