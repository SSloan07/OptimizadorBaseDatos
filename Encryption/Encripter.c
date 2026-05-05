#include <stdio.h>
#include "Encripter.h"

unsigned char *encripter_encrypt(
    Encripter *self,
    const unsigned char *input,
    size_t input_size,
    size_t *output_size
) {
    if (self == NULL || self->encrypt == NULL || input == NULL || output_size == NULL) {
        fprintf(stderr, "Encripter invalido en encrypt.\n");
        return NULL;
    }

    return self->encrypt(self, input, input_size, output_size);
}

unsigned char *encripter_decrypt(
    Encripter *self,
    const unsigned char *input,
    size_t input_size,
    size_t *output_size
) {
    if (self == NULL || self->decrypt == NULL || input == NULL || output_size == NULL) {
        fprintf(stderr, "Encripter invalido en decrypt.\n");
        return NULL;
    }

    return self->decrypt(self, input, input_size, output_size);
}

void encripter_destroy(Encripter *self) {
    if (self != NULL && self->destroy != NULL) {
        self->destroy(self);
    }
}