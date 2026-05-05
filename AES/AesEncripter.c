#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>

#include "AesEncripter.h"

#define AES_GCM_IV_SIZE 12
#define AES_GCM_TAG_SIZE 16
#define AES_GCM_HEADER_SIZE (AES_GCM_IV_SIZE + AES_GCM_TAG_SIZE)

typedef struct {
    Encripter base;
    unsigned char key[AES_256_KEY_SIZE];
} AesEncripter;

static unsigned char *aes_encrypt(
    Encripter *self,
    const unsigned char *input,
    size_t input_size,
    size_t *output_size
) {
    AesEncripter *aes = (AesEncripter *)self;
    EVP_CIPHER_CTX *ctx = NULL;

    unsigned char iv[AES_GCM_IV_SIZE];
    unsigned char tag[AES_GCM_TAG_SIZE];
    unsigned char *output = NULL;

    int len = 0;
    int ciphertext_len = 0;

    if (input == NULL || output_size == NULL) {
        return NULL;
    }

    if (input_size > INT_MAX) {
        return NULL;
    }

    if (RAND_bytes(iv, AES_GCM_IV_SIZE) != 1) {
        return NULL;
    }

    output = malloc(AES_GCM_HEADER_SIZE + input_size);
    if (output == NULL) {
        return NULL;
    }

    ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        free(output);
        return NULL;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(output);
        return NULL;
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, AES_GCM_IV_SIZE, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(output);
        return NULL;
    }

    if (EVP_EncryptInit_ex(ctx, NULL, NULL, aes->key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(output);
        return NULL;
    }

    if (EVP_EncryptUpdate(
            ctx,
            output + AES_GCM_HEADER_SIZE,
            &len,
            input,
            (int)input_size
        ) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(output);
        return NULL;
    }

    ciphertext_len = len;

    if (EVP_EncryptFinal_ex(
            ctx,
            output + AES_GCM_HEADER_SIZE + ciphertext_len,
            &len
        ) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(output);
        return NULL;
    }

    ciphertext_len += len;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, AES_GCM_TAG_SIZE, tag) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(output);
        return NULL;
    }

    memcpy(output, iv, AES_GCM_IV_SIZE);
    memcpy(output + AES_GCM_IV_SIZE, tag, AES_GCM_TAG_SIZE);

    *output_size = AES_GCM_HEADER_SIZE + ciphertext_len;

    EVP_CIPHER_CTX_free(ctx);
    return output;
}

static unsigned char *aes_decrypt(
    Encripter *self,
    const unsigned char *input,
    size_t input_size,
    size_t *output_size
) {
    AesEncripter *aes = (AesEncripter *)self;
    EVP_CIPHER_CTX *ctx = NULL;

    const unsigned char *iv = NULL;
    const unsigned char *tag = NULL;
    const unsigned char *ciphertext = NULL;

    unsigned char *output = NULL;

    size_t ciphertext_size;
    int len = 0;
    int plaintext_len = 0;
    int final_result;

    if (input == NULL || output_size == NULL) {
        return NULL;
    }

    if (input_size < AES_GCM_HEADER_SIZE) {
        return NULL;
    }

    ciphertext_size = input_size - AES_GCM_HEADER_SIZE;

    if (ciphertext_size > INT_MAX) {
        return NULL;
    }

    iv = input;
    tag = input + AES_GCM_IV_SIZE;
    ciphertext = input + AES_GCM_HEADER_SIZE;

    output = malloc(ciphertext_size > 0 ? ciphertext_size : 1);
    if (output == NULL) {
        return NULL;
    }

    ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        free(output);
        return NULL;
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(output);
        return NULL;
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, AES_GCM_IV_SIZE, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(output);
        return NULL;
    }

    if (EVP_DecryptInit_ex(ctx, NULL, NULL, aes->key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(output);
        return NULL;
    }

    if (EVP_DecryptUpdate(
            ctx,
            output,
            &len,
            ciphertext,
            (int)ciphertext_size
        ) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(output);
        return NULL;
    }

    plaintext_len = len;

    if (EVP_CIPHER_CTX_ctrl(
            ctx,
            EVP_CTRL_GCM_SET_TAG,
            AES_GCM_TAG_SIZE,
            (void *)tag
        ) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(output);
        return NULL;
    }

    final_result = EVP_DecryptFinal_ex(
        ctx,
        output + plaintext_len,
        &len
    );

    if (final_result <= 0) {
        EVP_CIPHER_CTX_free(ctx);
        free(output);
        return NULL;
    }

    plaintext_len += len;
    *output_size = (size_t)plaintext_len;

    EVP_CIPHER_CTX_free(ctx);
    return output;
}

static void aes_destroy(Encripter *self) {
    AesEncripter *aes = (AesEncripter *)self;

    if (aes != NULL) {
        OPENSSL_cleanse(aes->key, AES_256_KEY_SIZE);
        free(aes);
    }
}

Encripter *create_aes_256_gcm_encripter(
    const unsigned char key[AES_256_KEY_SIZE]
) {
    AesEncripter *instance;

    if (key == NULL) {
        return NULL;
    }

    instance = malloc(sizeof(AesEncripter));
    if (instance == NULL) {
        return NULL;
    }

    memcpy(instance->key, key, AES_256_KEY_SIZE);

    instance->base.encrypt = aes_encrypt;
    instance->base.decrypt = aes_decrypt;
    instance->base.destroy = aes_destroy;

    return (Encripter *)instance;
}