/* include/key_derivation.h - Cryptographic Key Derivation Routines */
#ifndef KEY_DERIVATION_H
#define KEY_DERIVATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

/**
 * Derives a 256-bit AES key using PBKDF2 (Password-Based Key Derivation Function 2)
 * with HMAC-SHA256 across a specified number of hashing iterations.
 *
 * @param seed Input seed/passphrase string.
 * @param salt Salt string to ensure unique key derivation.
 * @param iterations Hashing iteration count.
 * @return Dynamically allocated 32-byte (256-bit) raw key buffer, or NULL on failure.
 */
static inline unsigned char* derive_key_pbkdf2(const char* seed, const char* salt, int iterations) {
    if (!seed || !salt || iterations < 1) {
        return NULL;
    }

    unsigned char* key = (unsigned char*)malloc(32); // 256 bits for AES-256
    if (!key) {
        return NULL;
    }

    if (!PKCS5_PBKDF2_HMAC(seed, strlen(seed),
                           (const unsigned char*)salt, strlen(salt),
                           iterations, EVP_sha256(),
                           32, key)) {
        free(key);
        return NULL;
    }

    return key;
}

/**
 * Derives a 256-bit AES key via a Proof-of-Work challenge by searching for a nonce
 * that yields a SHA-256 hash starting with a specified number of zero bits.
 *
 * @param seed Input seed text used as the challenge prefix.
 * @param difficulty_bits Target number of leading zero bits required in the resulting hash.
 * @return Dynamically allocated 32-byte (256-bit) raw key buffer matching the target hash.
 */
static inline unsigned char* derive_key_pow(const char* seed, int difficulty_bits) {
    if (!seed || difficulty_bits < 0 || difficulty_bits > 256) {
        return NULL;
    }

    unsigned char* key = (unsigned char*)malloc(SHA256_DIGEST_LENGTH);
    if (!key) {
        return NULL;
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    char buffer[256];
    unsigned long long nonce = 0;

    int full_bytes = difficulty_bits / 8; // Full 0x00 bytes required
    int rem_bits   = difficulty_bits % 8; // Remaining leading zero bits in partial byte

    while (1) {
        snprintf(buffer, sizeof(buffer), "%s%llu", seed, nonce);
        SHA256((const unsigned char*)buffer, strlen(buffer), hash);

        int success = 1;

        // 1. Verify leading full 0x00 bytes
        for (int i = 0; i < full_bytes; i++) {
            if (hash[i] != 0x00) {
                success = 0;
                break;
            }
        }

        // 2. Verify remaining leading bits in the subsequent byte
        if (success && rem_bits > 0) {
            if ((hash[full_bytes] >> (8 - rem_bits)) != 0) {
                success = 0;
            }
        }

        // Return derived key when hash meets zero-bit condition
        if (success) {
            memcpy(key, hash, SHA256_DIGEST_LENGTH);
            return key;
        }

        nonce++;
    }
}

#endif // KEY_DERIVATION_H
