# Payload & Evasion Generator

A modular C-based framework designed for research and testing of execution delay techniques, system call hammering, anti-analysis routines, and cryptographic operations.

===============================================================================
1. REQUIREMENTS & DEPENDENCIES
===============================================================================

To build and run this project, you need the following pre-requisites installed on your Linux system:

* GCC Compiler (gcc)
* OpenSSL Library (libcrypto and libssl development headers)
* Zlib compression library (zlib1g-dev)
* POSIX Threads (pthread)
* Standard C Library (glibc)

Installing Dependencies (Ubuntu / Debian / Kali):
  sudo apt update
  sudo apt install build-essential libssl-dev zlib1g-dev

Alternatively, you can install dependencies using the provided Makefile:
  make deps


===============================================================================
2. DIRECTORY STRUCTURE
===============================================================================

.
├── include/
│   ├── compute.h           # CPU & memory intensive operation routines
│   ├── decrypt.h           # AES-256-CBC decryption & Base64 decoding
│   ├── hammering.h         # API flooding and memory stress routines
│   ├── key_derivation.h    # PBKDF2 & key derivation mechanisms
│   ├── network.h           # Network I/O and socket delay mechanisms
│   ├── payload.h           # Generated header holding stage configuration/data
│   ├── pow_evasion.h       # Proof-of-Work CPU delay routines
│   ├── storage.h           # Storage and VFS metadata stress techniques
│   ├── synchronization.h   # Multi-threading & IPC synchronization delays
│   └── temporal.h          # Time-based and signal-based delay functions
├── dist/                   # Output directory for compiled final binaries/loaders
├── payloads/               # Directory storing input ELF targets and payloads
├── builder.c               # Dynamic payload builder implementation
├── builder.h               # Builder structures and prototypes
├── loader.c                # Final loader template source
├── main.c                  # CLI entry point for generator
├── makefile                # Automated build, installation, and clean rules
└── README.md


===============================================================================
3. COMPILATION & BUILDING
===============================================================================

### Using Makefile (Recommended):

* Build the framework:
    make

* Clean compiled binaries and temporary build files:
    make clean

* Install required system dependencies:
    make deps

### Manual Compilation:

If compiling manually using GCC, ensure you include `-Iinclude` for headers and link required libraries:

    gcc -Wall -Wextra -Wno-unused-variable -Iinclude main.c builder.c -o generator -lcrypto -lssl -lpthread -lm -lz


===============================================================================
4. USAGE
===============================================================================

1. Place your target ELF binary into the `payloads/` folder (e.g., `./payloads/stage1.elf`).
2. Run the generator executable:
    ./generator

3. Follow the CLI wizard to select single/multi-stage options, evasion techniques, parameters, and execution mode.
4. The output binary will be compiled directly into `./dist/loader_final.elf`.


===============================================================================
5. LICENSE & DISCLAIMER
===============================================================================

Disclaimer: This project is intended strictly for educational purposes, security research, and defensive testing in controlled environments. Unauthorized use against target systems without consent is prohibited.
