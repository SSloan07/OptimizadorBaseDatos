#ifndef ENCRIPTER_H
#define ENCRIPTER_H

#include <stddef.h>

typedef struct Encripter Encripter;

struct Encripter {
    unsigned char *(*encrypt)(
        Encripter *self,
        const unsigned char *input,
        size_t input_size,
        size_t *output_size
    );

    unsigned char *(*decrypt)(
        Encripter *self,
        const unsigned char *input,
        size_t input_size,
        size_t *output_size
    );

    void (*destroy)(Encripter *self);
};

unsigned char *encripter_encrypt(
    Encripter *self,
    const unsigned char *input,
    size_t input_size,
    size_t *output_size
);

unsigned char *encripter_decrypt(
    Encripter *self,
    const unsigned char *input,
    size_t input_size,
    size_t *output_size
);

void encripter_destroy(Encripter *self);

#endif