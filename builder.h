/* builder.h - Builder module interface */
#ifndef BUILDER_H
#define BUILDER_H

#define MAX_PATH_LEN 256

typedef struct {
    char elf_path[256];
    int tech_id;
    int tech_param;
    int exec_mode;
    
    // Key Derivation Configuration
    int key_derivation_mode; // 1: Static Hash, 2: Proof-of-Work
    char key_input[64];      // Stores Passphrase or Seed
    int pow_difficulty;      // PoW Difficulty Level (1-3)

    // --- Dynamic Network Parameters ---
    char net_ip[64];
    char net_host[128];
    int net_port;
} StageConfig;

/**
 * Main function of the Builder module.
 * Receives an array of stage configurations and the total number of stages selected in main.
 *
 * @param stages Array containing configuration parameters for each stage.
 * @param total_stages Total number of stages to generate.
 * @return 0 on success, non-zero error code on failure.
 */
int run_builder(const StageConfig *stages, int total_stages);

#endif // BUILDER_H
