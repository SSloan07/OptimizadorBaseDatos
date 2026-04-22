#ifndef RECORD_H
#define RECORD_H

typedef struct {
    int id;
    char nombre[100];
    int edad;
    char grado[50];
    char escolaridad[50];
    char comuna[100];
} Record;

#endif