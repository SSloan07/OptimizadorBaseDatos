#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "BigHashTable/MedellinCommunes.h"
#include "Storage/PartitionManager.h"
#include "Storage/FileIndex.h"
#include "IO/FileReader.h"

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
    char *contenido;

    inicializar_comunas();

    printf("Ingrese el nombre de la comuna: ");
    if (fgets(comuna, sizeof(comuna), stdin) == NULL) {
        fprintf(stderr, "Error al leer la comuna.\n");
        return 1;
    }

    comuna[strcspn(comuna, "\n")] = '\0';
    convertir_a_minusculas(comuna);

    id_comuna = obtener_id_comuna(comuna);

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

    construir_ruta_bloque(id_comuna, id_bloque, ruta, sizeof(ruta));

    contenido = leer_archivo_completo(ruta);

    printf("\n--- RESULTADO ---\n");
    printf("Comuna: %s\n", comuna);
    printf("ID comuna: %d\n", id_comuna);
    printf("ID registro: %d\n", id_registro);
    printf("Bloque: %d\n", id_bloque);
    printf("Ruta archivo: %s\n", ruta);

    if (contenido == NULL) {
        printf("No se pudo leer el archivo.\n");
        return 1;
    }

    printf("\n--- CONTENIDO DEL ARCHIVO ---\n");
    printf("%s\n", contenido);

    free(contenido);
    return 0;
}