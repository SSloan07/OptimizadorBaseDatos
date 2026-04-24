#include <stdio.h>
#include "App/Menu.h"
#include "App/SearchService.h"
#include "App/InsertService.h"
#include "App/AppContext.h"

int main(void) {
    int opcion;
    int salir = 0;
    AppContext *ctx;

    ctx = crear_contexto_app();
    if (ctx == NULL) {
        fprintf(stderr, "No se pudo inicializar el contexto de la aplicacion.\n");
        return 1;
    }

    while (!salir) {
        opcion = mostrar_menu_principal();

        switch (opcion) {
            case 1:
                ejecutar_busqueda_puntual(ctx);
                break;
            case 2:
                ejecutar_menu_insercion(ctx);
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

    destruir_contexto_app(ctx);
    return 0;
}

