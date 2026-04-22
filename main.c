#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "BigHashTable/MedellinCommunes.h"

void a_minusculas(char *texto) {
    for (int i = 0; texto[i] != '\0'; i++) {
        texto[i] = tolower((unsigned char)texto[i]);
    }
}

int main() {
    char comuna[100];
    int id;

    inicializar_comunas();

    printf("Ingrese el nombre de la comuna: ");
    fgets(comuna, sizeof(comuna), stdin);

    comuna[strcspn(comuna, "\n")] = '\0';

    a_minusculas(comuna);

    id = obtener_id_comuna(comuna);

    if (id != -1) {
        printf("El id de la comuna es: %d\n", id);
    } else {
        printf("Comuna no encontrada\n");
    }

    return 0;
}
