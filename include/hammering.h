/* include/hammering.h - Enhanced System Call and Memory Allocation Stress Routines */
#ifndef HAMMERING_H
#define HAMMERING_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>
#include <pwd.h>
#include <string.h>

/**
 * Executes high-frequency benign system calls using an internal multiplier loop
 * to saturate API monitoring buffers and generate sustained analysis overhead.
 *
 * @param count Base iteration count multiplied internally (1000x) for extended runtime.
 */
static inline void execute_api_hammering(unsigned long long count) {
    struct utsname name;
    uid_t uid;
    volatile time_t now;

    // Internal nested multiplier loop ensures long-term execution and prevents premature termination
    for (unsigned long long i = 0; i < count; i++) {
        for (int j = 0; j < 1000; j++) {
            // Query host architecture and OS properties
            uname(&name);

            // Fetch process user identity
            uid = getuid();

            // Sample system clock timestamp
            now = time(NULL);

            // Prevent compiler dead-code elimination (-O2/-O3)
            (void)uid;
            (void)now;
        }
    }
}

/**
 * Performs continuous heap allocation across varying block sizes and writes across
 * 4KB memory page boundaries. This forces OS-level physical page faulting and 
 * completely bypasses fast heap caching mechanisms (tcache/fastbins).
 *
 * @param count Number of allocation and page-dirtying cycles to perform.
 */
static inline void memory_hammering(unsigned long long count) {
    volatile unsigned char dummy = 0;

    for (unsigned long long i = 0; i < count; i++) {
        // Allocate larger blocks (64 KB to 2 MB) to bypass standard heap caching thresholds
        size_t alloc_size = ((i % 32) + 1) * 65536; 
        char *p = (char*)malloc(alloc_size);

        if (p) {
            // Touch memory at 4096-byte (4KB) intervals to force true physical page mapping (page faults)
            for (size_t offset = 0; offset < alloc_size; offset += 4096) {
                p[offset] = (char)((i + offset) & 0xFF);
                dummy ^= p[offset]; // Prevent dead-code elimination via state accumulation
            }
            free(p);
        }
    }
    (void)dummy;
}

#endif // HAMMERING_H
