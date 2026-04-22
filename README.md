# OptimizadorBaseDatos

#ifndef DATO_H
#define DATO_H

typedef enum {

    Boston,

} Barrio;

typedef enum {

    Poblado,
    Candelaria,

} Comuna;

typedef struct {

    char* cedula;
    char* nombre;
    int grado;
    char* respuestaPregunta;
    Comuna comuna;
    Barrio barrio;

} Dato;

#endif