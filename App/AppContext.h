#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include "../GlobalIndex/GlobalRecordIndex.h"

typedef struct {
    GlobalRecordIndex *global_index;
} AppContext;

AppContext *crear_contexto_app(void);
void destruir_contexto_app(AppContext *ctx);
int recargar_indice_global(AppContext *ctx);

#endif