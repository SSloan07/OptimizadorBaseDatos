#ifndef BATCH_GROUP_H
#define BATCH_GROUP_H

#include "../Data/Record.h"

typedef struct GeneratedNode {
    Record record;
    struct GeneratedNode *next;
} GeneratedNode;

typedef struct GroupNode {
    int id_comuna;
    int id_bloque;
    GeneratedNode *records;
    struct GroupNode *next;
} GroupNode;

GroupNode *buscar_grupo(GroupNode *head, int id_comuna, int id_bloque);
int agregar_record_a_grupo(GroupNode **head, Record record, int id_comuna, int id_bloque);
void liberar_grupos(GroupNode *head);

#endif