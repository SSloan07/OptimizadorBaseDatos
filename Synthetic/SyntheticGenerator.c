#include <stdlib.h>
#include <string.h>

#include "SyntheticGenerator.h"

static const char *nombres[] = {
    "Ana", "Luis", "Carlos", "Maria", "Sofia",
    "Juan", "Valentina", "Andres", "Camila", "Mateo"
};

static const char *escolaridades[] = {
    "primaria", "secundaria", "media", "tecnica", "profesional"
};

static const char *comunas[] = {
    "popular", "santa cruz", "manrique", "aranjuez",
    "castilla", "doce de octubre", "robledo", "villa hermosa",
    "buenos aires", "la candelaria", "laureles",
    "la america", "san javier", "el poblado", "guayabal", "belen"
};

Record generar_registro_sintetico(void) {
    Record r;

    r.id = -1;

    strcpy(r.nombre, nombres[rand() % 10]);
    r.edad = 10 + rand() % 15;
    strcpy(r.escolaridad, escolaridades[rand() % 5]);
    strcpy(r.comuna, comunas[rand() % 16]);

    return r;
}