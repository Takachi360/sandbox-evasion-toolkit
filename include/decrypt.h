/* include/decrypt.h - AES-256 Decryption and Base64 Utilities */
#ifndef DECRYPT_H
#define DECRYPT_H

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <string.h>
#include <stdlib.h>

/**
 * Decodes a Base64-encoded string into a raw binary buffer using OpenSSL BIO.
 *
 * @param input Null-terminated Base64 string to decode.
 * @param output Pointer to allocated memory containing decoded bytes.
 * @return Number of decoded bytes written to output, or 0 on failure.
 */
static inline int base64_decode(const char* input, unsigned char** output) {
    if (!input || !output) {
        return 0;
    }

    BIO *bio, *b64;
    int decodeLen = strlen(input);
    int len = 0;

    *output = (unsigned char*)malloc(decodeLen);
    if (!*output) {
        return 0;
    }

    bio = BIO_new_mem_buf(input, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    len = BIO_read(bio, *output, decodeLen);
    BIO_free_all(bio);

    return len;
}

/**
 * Decrypts a Base64-encoded AES-256-CBC payload using a key derived via SHA-256 passphrase hashing.
 *
 * @param ciphertext_b64 Base64-encoded string containing prepend IV (16 bytes) and ciphertext.
 * @param password Passphrase string used to derive the 256-bit AES key.
 * @param out_len Pointer to store the resulting decrypted plaintext byte length.
 * @return Dynamically allocated buffer containing decrypted binary data, or NULL on failure.
 */
static inline unsigned char* aes_decrypt(const char* ciphertext_b64, const char* password, size_t *out_len) {
    if (!ciphertext_b64 || !password) {
        return NULL;
    }

    // 1. Derive 256-bit AES key using SHA-256 hash of passphrase
    unsigned char key[32];
    SHA256((const unsigned char*)password, strlen(password), key);

    // 2. Decode Base64 input payload
    unsigned char* data = NULL;
    int data_len = base64_decode(ciphertext_b64, &data);

    if (data_len < 16) {
        if (data) free(data);
        return NULL;
    }

    // 3. Extract 16-byte IV prepended to ciphertext
    unsigned char iv[16];
    memcpy(iv, data, 16);

    unsigned char* actual_ciphertext = data + 16;
    int ciphertext_len = data_len - 16;

    // 4. Initialize OpenSSL AES-256-CBC decryption context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        free(data);
        return NULL;
    }

    unsigned char *plaintext = (unsigned char*)malloc(ciphertext_len + 32);
    if (!plaintext) {
        EVP_CIPHER_CTX_free(ctx);
        free(data);
        return NULL;
    }

    int len = 0, final_len = 0;

    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        EVP_CIPHER_CTX_free(ctx);
        free(data);
        free(plaintext);
        return NULL;
    }

    if (!EVP_DecryptUpdate(ctx, plaintext, &len, actual_ciphertext, ciphertext_len)) {
        EVP_CIPHER_CTX_free(ctx);
        free(data);
        free(plaintext);
        return NULL;
    }
    final_len = len;

    if (!EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        free(data);
        free(plaintext);
        return NULL;
    }
    final_len += len;

    // 5. Cleanup resources and set output parameters
    EVP_CIPHER_CTX_free(ctx);
    free(data);

    if (out_len) {
        *out_len = (size_t)final_len;
    }

    return plaintext;
}

/**
 * Decrypts a Base64-encoded AES-256-CBC payload using a direct raw 256-bit binary key.
 *
 * @param ciphertext_b64 Base64-encoded string containing prepend IV (16 bytes) and ciphertext.
 * @param key Pointer to 32-byte (256-bit) raw key array.
 * @param out_len Pointer to store the resulting decrypted plaintext byte length.
 * @return Dynamically allocated buffer containing decrypted binary data, or NULL on failure.
 */
static inline unsigned char* aes_decrypt_with_key(const char* ciphertext_b64, const unsigned char* key, size_t *out_len) {
    if (!ciphertext_b64 || !key) {
        return NULL;
    }

    // 1. Decode Base64 input payload
    unsigned char* data = NULL;
    int data_len = base64_decode(ciphertext_b64, &data);

    if (data_len < 16) {
        if (data) free(data);
        return NULL;
    }

    // 2. Extract 16-byte IV prepended to ciphertext
    unsigned char iv[16];
    memcpy(iv, data, 16);

    unsigned char* actual_ciphertext = data + 16;
    int ciphertext_len = data_len - 16;

    // 3. Initialize OpenSSL AES-256-CBC decryption context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        free(data);
        return NULL;
    }

    unsigned char *plaintext = (unsigned char*)malloc(ciphertext_len + 32);
    if (!plaintext) {
        EVP_CIPHER_CTX_free(ctx);
        free(data);
        return NULL;
    }

    int len = 0, final_len = 0;

    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        EVP_CIPHER_CTX_free(ctx);
        free(data);
        free(plaintext);
        return NULL;
    }

    if (!EVP_DecryptUpdate(ctx, plaintext, &len, actual_ciphertext, ciphertext_len)) {
        EVP_CIPHER_CTX_free(ctx);
        free(data);
        free(plaintext);
        return NULL;
    }
    final_len = len;

    if (!EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        free(data);
        free(plaintext);
        return NULL;
    }
    final_len += len;

    // 4. Cleanup resources and set output parameters
    EVP_CIPHER_CTX_free(ctx);
    free(data);

    if (out_len) {
        *out_len = (size_t)final_len;
    }

    return plaintext;
}

#endif // DECRYPT_H
