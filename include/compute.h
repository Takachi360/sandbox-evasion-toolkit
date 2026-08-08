/* include/compute.h - Computational Delay & CPU Stress Evasion Functions */
#ifndef COMPUTE_H
#define COMPUTE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <openssl/sha.h>
#include <zlib.h>

/**
 * Executes a repeated SHA-512 hashing chain to consume CPU cycles.
 * Multiplier: 10,000x input iterations.
 *
 * @param iterations Base iteration multiplier count.
 */
static inline void delay_hash_chain(unsigned long long iterations) {
    unsigned char data[SHA512_DIGEST_LENGTH];
    unsigned char hash[SHA512_DIGEST_LENGTH];

    memset(data, 0, SHA512_DIGEST_LENGTH);
    memcpy(data, "init_seed", 9);

    unsigned long long real_limit = iterations * 10000ULL;

    for (unsigned long long i = 0; i < real_limit; i++) {
        SHA512(data, SHA512_DIGEST_LENGTH, hash);
        memcpy(data, hash, SHA512_DIGEST_LENGTH);
    }

    // Volatile read prevention to prevent compiler dead-code elimination (-O2)
    if (data[0] == 0xFF && data[1] == 0x00) {
        (void)data[0];
    }
}

/**
 * Performs floating-point mathematical operations (square root and tangent) to stress the FPU.
 * Multiplier: 100,000x input iterations.
 *
 * @param iterations Base iteration multiplier count.
 * @return Computed floating-point result to ensure execution.
 */
static inline double delay_float_math(unsigned long long iterations) {
    volatile double val = 1.0;
    unsigned long long real_limit = iterations * 100000ULL;

    for (unsigned long long i = 1; i < real_limit; i++) {
        val += sqrt((double)i) * tan(1.0 / (double)i);
    }
    return val;
}

/**
 * Repeatedly compresses a memory buffer containing entropy using zlib.
 * Multiplier: 100x input iterations.
 *
 * @param iterations Base iteration multiplier count.
 */
static inline void delay_compression_spam(unsigned long long iterations) {
    uLong sourceLen = 1024 * 10; // 10 KB buffer
    Bytef *source = (Bytef*)malloc(sourceLen);
    if (!source) return;

    FILE *f = fopen("/dev/urandom", "rb");
    if (f) {
        if (fread(source, 1, sourceLen, f) != sourceLen) {
            memset(source, 0x41, sourceLen);
        }
        fclose(f);
    } else {
        memset(source, 0x41, sourceLen);
    }

    uLong destLen = compressBound(sourceLen);
    Bytef *dest = (Bytef*)malloc(destLen);
    if (!dest) {
        free(source);
        return;
    }

    unsigned long long real_limit = iterations * 100ULL;

    for (unsigned long long i = 0; i < real_limit; i++) {
        uLong tempDestLen = destLen;
        compress(dest, &tempDestLen, source, sourceLen);
    }

    free(source);
    free(dest);
}

/**
 * Evaluates prime numbers sequentially via trial division (O(N * sqrt(N))).
 * Multiplier: 500x input ceiling.
 *
 * @param limit Base ceiling multiplier limit.
 * @return Total quantity of prime numbers identified.
 */
static inline int delay_prime_calculation(unsigned long long limit) {
    unsigned long long real_limit = limit * 500ULL;
    volatile int count = 0;

    for (unsigned long long num = 2; num < real_limit; num++) {
        int is_prime = 1;
        for (unsigned long long i = 2; i * i <= num; i++) {
            if (num % i == 0) {
                is_prime = 0;
                break;
            }
        }
        if (is_prime) count++;
    }
    return count;
}

/**
 * Executes bitwise bit rotations and XOR mutations in a tight CPU loop.
 * Multiplier: 500,000x input iterations.
 *
 * @param iterations Base iteration multiplier count.
 * @return Mutated bitwise state accumulator.
 */
static inline uint32_t delay_bitwise_ops(unsigned long long iterations) {
    volatile uint32_t n = 0xABCDE;
    unsigned long long real_limit = iterations * 500000ULL;

    for (unsigned long long i = 0; i < real_limit; i++) {
        n = ((n << 1) | (n >> 31));
        n ^= (uint32_t)i;
    }
    return n;
}

/**
 * Computes Fibonacci sequence numbers recursively to generate stack frame overhead.
 *
 * @param x Input term index.
 * @return Calculated Fibonacci value at position x.
 */
static inline long long _fib(int x) {
    if (x <= 1) return x;
    return _fib(x - 1) + _fib(x - 2);
}

/**
 * Wrapper to trigger deep stack recursion via Fibonacci calculation.
 *
 * @param n Target Fibonacci sequence index.
 * @return Resulting Fibonacci value.
 */
static inline long long delay_recursive_fib(int n) {
    return _fib(n);
}

/**
 * Executes branch-heavy conditional logical loops to generate variable state paths.
 * Multiplier: 300,000x input iterations.
 *
 * @param iterations Base iteration multiplier count.
 * @return Final logical state accumulator.
 */
static inline long delay_junk_logic(unsigned long long iterations) {
    volatile long state = 0;
    unsigned long long real_limit = iterations * 300000ULL;

    for (unsigned long long i = 0; i < real_limit; i++) {
        if (i % 2 == 0) {
            state += (i * 2);
        } else {
            state -= i;
        }

        if (state > 1000000) {
            state /= 2;
        }
    }
    return state;
}

#endif // COMPUTE_H
