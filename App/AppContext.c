#include <stdlib.h>
#include "AppContext.h"
#include "../GlobalIndex/GlobalIndexPersistence.h"

#define GLOBAL_INDEX_SIZE 1009
#define GLOBAL_INDEX_FILE "data/global_index.txt"

AppContext *crear_contexto_app(void) {
    AppContext *ctx;

    ctx = (AppContext *)malloc(sizeof(AppContext));
    if (ctx == NULL) {
        return NULL;
    }

    ctx->global_index = create_global_record_index(GLOBAL_INDEX_SIZE);
    if (ctx->global_index == NULL) {
        free(ctx);
        return NULL;
    }

    if (cargar_indice_global_desde_archivo(ctx->global_index, GLOBAL_INDEX_FILE) != 0) {
        free_global_record_index(ctx->global_index);
        free(ctx);
        return NULL;
    }

    return ctx;
}

void destruir_contexto_app(AppContext *ctx) {
    if (ctx == NULL) {
        return;
    }

    free_global_record_index(ctx->global_index);
    free(ctx);
}

int recargar_indice_global(AppContext *ctx) {
    if (ctx == NULL) {
        return -1;
    }

    free_global_record_index(ctx->global_index);

    ctx->global_index = create_global_record_index(GLOBAL_INDEX_SIZE);
    if (ctx->global_index == NULL) {
        return -1;
    }

    if (cargar_indice_global_desde_archivo(ctx->global_index, GLOBAL_INDEX_FILE) != 0) {
        free_global_record_index(ctx->global_index);
        ctx->global_index = NULL;
        return -1;
    }

    return 0;
}