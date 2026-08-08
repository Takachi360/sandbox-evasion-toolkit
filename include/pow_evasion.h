#ifndef POW_EVASION_H
#define POW_EVASION_H

#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

/**
 * Proof of Work: Searches for a nonce that produces a hash with the specified 
 * number of leading zero bits.
 *
 * @param seed Base seed string (e.g., machine ID or random string).
 * @param difficulty Number of leading bits that must be 0.
 * @return The solved nonce that satisfies the puzzle criteria.
 */
unsigned long solve_pow_challenge(const char* seed, int difficulty) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned char input[256];
    unsigned long nonce = 0;
    int solved = 0;

    printf("[*] Starting Proof of Work (Difficulty: %d bits)...\n", difficulty);

    while (!solved) {

        /* Construct payload string: "seed + nonce" */
        sprintf((char*)input, "%s%lu", seed, nonce);

        /* Compute SHA-256 hash */
        SHA256(input, strlen((char*)input), hash);

        /* Verify bit difficulty requirement */
        int bits_ok = 1;

        int full_bytes = difficulty / 8;
        int remaining_bits = difficulty % 8;

        /* All full bytes must be 0x00 */
        for (int i = 0; i < full_bytes; i++) {
            if (hash[i] != 0x00) {
                bits_ok = 0;
                break;
            }
        }

        /* Check remaining leading bits in the subsequent byte */
        if (bits_ok && remaining_bits > 0) {
            unsigned char mask = (unsigned char)(0xFF << (8 - remaining_bits));

            if ((hash[full_bytes] & mask) != 0)
                bits_ok = 0;
        }

        if (bits_ok) {
            solved = 1;

            printf("[+] Puzzle solved! Nonce: %lu\n", nonce);
            printf("[+] Resulting hash: ");

            for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
                printf("%02x", hash[i]);

            printf("\n");
        } else {
            nonce++;
        }

        /* Optional: Periodically execute auxiliary anti-analysis checks */
    }

    return nonce;
}

#endif
