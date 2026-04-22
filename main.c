#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "BigHashTable/MedellinCommunes.h"
#include "Storage/PartitionManager.h"
#include "Storage/FileIndex.h"

#define TAM_BLOQUE 100000

static void convertir_a_minusculas(char *texto) {
    for (int i = 0; texto[i] != '\0'; i++) {
        texto[i] = (char)tolower((unsigned char)texto[i]);
    }
}

int main(void) {
    char comuna[100];
    int id_comuna;
    int id_registro;
    int id_bloque;
    char ruta[256];

    inicializar_comunas();

    printf("Ingrese el nombre de la comuna: ");
    fgets(comuna, sizeof(comuna), stdin);

    comuna[strcspn(comuna, "\n")] = '\0';
    convertir_a_minusculas(comuna);

    id_comuna = obtener_id_comuna(comuna);

    if (id_comuna == -1) {
        printf("Comuna no encontrada\n");
        return 1;
    }

    printf("Ingrese el id del registro: ");
    scanf("%d", &id_registro);

    id_bloque = calcular_bloque(id_registro, TAM_BLOQUE);

    if (id_bloque == -1) {
        printf("ID de registro invalido\n");
        return 1;
    }

    construir_ruta_bloque(id_comuna, id_bloque, ruta, sizeof(ruta));

    printf("\n--- RESULTADO ---\n");
    printf("Comuna: %s\n", comuna);
    printf("ID comuna: %d\n", id_comuna);
    printf("ID registro: %d\n", id_registro);
    printf("Bloque: %d\n", id_bloque);
    printf("Ruta archivo: %s\n", ruta);

    return 0;
}