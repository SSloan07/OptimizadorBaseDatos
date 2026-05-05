#ifndef AES_ENCRIPTER_H
#define AES_ENCRIPTER_H

#include "../Encryption/Encripter.h"

#define AES_256_KEY_SIZE 32

Encripter *create_aes_256_gcm_encripter(
    const unsigned char key[AES_256_KEY_SIZE]
);

#endif