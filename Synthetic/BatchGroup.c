#include <stdlib.h>
#include "BatchGroup.h"

GroupNode *buscar_grupo(GroupNode *head, int id_comuna, int id_bloque) {
    while (head != NULL) {
        if (head->id_comuna == id_comuna && head->id_bloque == id_bloque) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

int agregar_record_a_grupo(GroupNode **head, Record record, int id_comuna, int id_bloque) {
    GroupNode *grupo;
    GeneratedNode *nuevo;

    grupo = buscar_grupo(*head, id_comuna, id_bloque);
    if (grupo == NULL) {
        grupo = (GroupNode *)malloc(sizeof(GroupNode));
        if (grupo == NULL) {
            return -1;
        }

        grupo->id_comuna = id_comuna;
        grupo->id_bloque = id_bloque;
        grupo->records = NULL;
        grupo->next = *head;
        *head = grupo;
    }

    nuevo = (GeneratedNode *)malloc(sizeof(GeneratedNode));
    if (nuevo == NULL) {
        return -1;
    }

    nuevo->record = record;
    nuevo->next = grupo->records;
    grupo->records = nuevo;

    return 0;
}

void liberar_grupos(GroupNode *head) {
    while (head != NULL) {
        GroupNode *tmp_group = head;
        GeneratedNode *node = head->records;

        while (node != NULL) {
            GeneratedNode *tmp_node = node;
            node = node->next;
            free(tmp_node);
        }

        head = head->next;
        free(tmp_group);
    }
}