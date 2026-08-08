/* include/hammering.h - System Call and Memory Allocation Stress Routines */
#ifndef HAMMERING_H
#define HAMMERING_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>
#include <pwd.h>

/**
 * Executes repeated high-frequency benign system calls to flood API monitoring 
 * buffers and generate analysis overhead.
 *
 * @param count Number of iterations for each system call sequence.
 */
static inline void execute_api_hammering(int count) {
    struct utsname name;
    struct passwd *pw;
    uid_t uid;
    volatile time_t now;
    volatile pid_t pid;
    volatile pid_t ppid;
    volatile char *env_val;

    for (int i = 0; i < count; i++) {
        // 1. Host system information querying
        uname(&name);

        // 2. User account context retrieval
        uid = getuid();
        pw = getpwuid(uid);
        (void)pw;

        // 3. System time state sampling
        now = time(NULL);
        (void)now;

        // 4. Process identifier resolution
        pid = getpid();
        ppid = getppid();
        (void)pid;
        (void)ppid;

        // 5. Environment variable lookup
        env_val = getenv("NON_EXISTENT_VARIABLE_FOR_HAMMERING");
        (void)env_val;
    }
}

/**
 * Performs dynamic heap memory allocations and releases of varying block sizes, 
 * writing to memory pages to force actual OS memory page mapping.
 *
 * @param count Number of allocation and free iterations to execute.
 */
static inline void memory_hammering(long long count) {
    volatile unsigned char dummy = 0;

    for (long long i = 0; i < count; i++) {
        // Dynamic block size computation to bypass fast heap caching mechanisms
        size_t alloc_size = (size_t)((i % 2048) + 128);
        void *p = malloc(alloc_size);

        if (p) {
            ((char*)p)[0] = (char)(i & 0xFF);
            dummy ^= ((char*)p)[0]; // Read-write access to prevent compiler dead-code elimination
            free(p);
        }
    }
    (void)dummy;
}

#endif // HAMMERING_H
