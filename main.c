#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "BigHashTable/MedellinCommunes.h"
#include "Storage/PartitionManager.h"

#define TAM_BLOQUE 100000

static void convertir_a_minusculas(char *texto) {
    for (int i = 0; texto[i] != '\0'; i++) {
        texto[i] = (char)tolower((unsigned char)texto[i]);
    }
}

int main(void) {
    char nombre_comuna[100];
    int id_comuna;
    int id_registro;
    int id_bloque;

    inicializar_comunas();

    printf("Ingrese el nombre de la comuna: ");
    if (fgets(nombre_comuna, sizeof(nombre_comuna), stdin) == NULL) {
        fprintf(stderr, "Error al leer la comuna.\n");
        return 1;
    }

    nombre_comuna[strcspn(nombre_comuna, "\n")] = '\0';
    convertir_a_minusculas(nombre_comuna);

    id_comuna = obtener_id_comuna(nombre_comuna);

    if (id_comuna == -1) {
        printf("Comuna no encontrada.\n");
        return 1;
    }

    printf("Ingrese el id del registro: ");
    if (scanf("%d", &id_registro) != 1) {
        fprintf(stderr, "Error al leer el id del registro.\n");
        return 1;
    }

    id_bloque = calcular_bloque(id_registro, TAM_BLOQUE);

    if (id_bloque == -1) {
        printf("ID de registro invalido.\n");
        return 1;
    }

    printf("ID comuna: %d\n", id_comuna);
    printf("ID registro: %d\n", id_registro);
    printf("Bloque calculado: %d\n", id_bloque);

    return 0;
}