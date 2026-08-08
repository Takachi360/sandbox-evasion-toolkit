/* include/storage.h - Storage and File System I/O Stress Routines */
#ifndef STORAGE_H
#define STORAGE_H

#define _XOPEN_SOURCE 700
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

/**
 * Internal recursive helper function for directory tree traversal.
 *
 * @param dir_path Target directory path to traverse.
 * @param limit Maximum number of entry metadata evaluations.
 * @param counter Pointer to tracking variable for total entries processed.
 */
static inline void internal_disk_walk_recursive(const char *dir_path, long long limit, long long *counter) {
    if (*counter >= limit) return;

    DIR *dir = opendir(dir_path);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && *counter < limit) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        (*counter)++;

        char full_path[2048];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        // Force an inode metadata lookup per discovered directory entry
        struct stat st;
        stat(full_path, &st);

        // Descend recursively if entry is a directory
        if (S_ISDIR(st.st_mode)) {
            internal_disk_walk_recursive(full_path, limit, counter);
        }
    }
    closedir(dir);
}

/**
 * Recursively traverses directory structures to induce filesystem I/O load.
 *
 * @param base_path Root directory path to initiate traversal.
 * @param limit Maximum total filesystem entry queries to execute.
 */
static inline void delay_disk_walk(const char *base_path, long long limit) {
    long long counter = 0;
    while (counter < limit) {
        long long prev = counter;
        internal_disk_walk_recursive(base_path, limit, &counter);
        // Break if no further entries can be read (end of directory tree)
        if (counter == prev) break;
    }
}

/**
 * Performs heavy disk write and read operations using a temporary file with forced page synchronization.
 *
 * @param size_mb File size volume in megabytes to write and read.
 */
static inline void delay_heavy_io_blips(int size_mb) {
    char tmp_file[] = "/tmp/io_XXXXXX";
    int fd = mkstemp(tmp_file);
    if (fd == -1) return;

    unsigned char *data = (unsigned char *)malloc(1024 * 1024);
    if (!data) { 
        close(fd); 
        unlink(tmp_file); 
        return; 
    }

    int urandom = open("/dev/urandom", O_RDONLY);
    if (urandom >= 0) {
        if (read(urandom, data, 1024 * 1024) < 0) {
            memset(data, 0xAA, 1024 * 1024);
        }
        close(urandom);
    } else {
        memset(data, 0xAA, 1024 * 1024);
    }

    // Write with physical synchronization to force storage flush
    for (int i = 0; i < size_mb; i++) {
        if (write(fd, data, 1024 * 1024) < 0) break;
        fsync(fd);
    }

    // Complete sequential read-back
    lseek(fd, 0, SEEK_SET);
    while (read(fd, data, 1024 * 1024) > 0);

    close(fd);
    free(data);
    unlink(tmp_file);
}

/**
 * Executes high-frequency 1-byte read system calls against kernel entropy devices.
 *
 * @param bytes_to_read Total quantity of byte read syscalls to perform.
 */
static inline void delay_entropy_source(long long bytes_to_read) {
    int fd = open("/dev/random", O_RDONLY);
    if (fd < 0) return;

    char byte;
    for (long long i = 0; i < bytes_to_read; i++) {
        if (read(fd, &byte, 1) <= 0) break;
    }

    close(fd);
}

/**
 * Creates a sparse temporary file and performs sequential reads across virtual allocations.
 *
 * @param size_gb Virtual target allocation size in gigabytes.
 */
static inline void delay_sparse_file_bomb(long long size_gb) {
    char tmp_file[] = "/tmp/sparse_XXXXXX";
    int fd = mkstemp(tmp_file);
    if (fd == -1) return;

    if (lseek(fd, (size_gb * 1024 * 1024 * 1024) - 1, SEEK_SET) != -1) {
        if (write(fd, "\0", 1) > 0) {
            lseek(fd, 0, SEEK_SET);
            char buf[1024 * 1024];
            while (read(fd, buf, sizeof(buf)) > 0);
        }
    }

    close(fd);
    unlink(tmp_file);
}

/**
 * Rapidly creates and deletes directories to trigger inode management overhead.
 *
 * @param iterations Total directory creation and deletion cycles.
 */
static inline void delay_mkdir_spam(int iterations) {
    char base_dir[] = "/tmp/dirs_XXXXXX";
    if (!mkdtemp(base_dir)) return;
    char sub_dir[512];

    for (int i = 0; i < iterations; i++) {
        snprintf(sub_dir, sizeof(sub_dir), "%s/dir_%d", base_dir, i);
        if (mkdir(sub_dir, 0700) == 0) {
            rmdir(sub_dir);
        }
    }
    rmdir(base_dir);
}

/**
 * Allocates and writes to large memory blocks to generate system RAM pressure.
 *
 * @param chunks Quantity of individual memory buffers to allocate.
 * @param chunk_size_mb Size in megabytes per allocation chunk.
 */
static inline void delay_memory_pressure(int chunks, int chunk_size_mb) {
    size_t size = (size_t)chunk_size_mb * 1024 * 1024;
    void **hold_memory = (void **)malloc(chunks * sizeof(void *));
    if (!hold_memory) return;

    for (int i = 0; i < chunks; i++) {
        hold_memory[i] = malloc(size);
        if (hold_memory[i]) {
            memset(hold_memory[i], 0xAA, size);
        }
    }

    for (int i = 0; i < chunks; i++) {
        if (hold_memory[i]) free(hold_memory[i]);
    }
    free(hold_memory);
}

/**
 * Issues repeated status checks on non-existent paths to generate VFS cache misses.
 *
 * @param iterations Total status query attempts to perform.
 */
static inline void delay_metadata_stat(long long iterations) {
    struct stat st;
    char dummy_path[256];
    for (long long i = 0; i < iterations; i++) {
        snprintf(dummy_path, sizeof(dummy_path), "/etc/passwd_%lld", i);
        stat(dummy_path, &st);
    }
}

#endif // STORAGE_H
