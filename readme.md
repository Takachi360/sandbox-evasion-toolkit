# Payload & Evasion Generator

A modular C-based framework designed for research and testing of execution delay techniques, system call hammering, anti-analysis routines, remote C2 key retrieval, and cryptographic operations.

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
│   ├── key_derivation.h    # PBKDF2, PoW & C2 key derivation mechanisms
│   ├── network.h           # Network I/O, socket delays & remote C2 retrieval
│   ├── payload.h           # Generated header holding stage configuration/data
│   ├── pow_evasion.h       # Proof-of-Work CPU delay routines
│   ├── storage.h           # Storage and VFS metadata stress techniques
│   ├── synchronization.h   # Multi-threading & IPC synchronization delays
│   └── temporal.h          # Time-based and signal-based delay functions
├── dist/                   # Output directory for single/multi-stage final loaders
├── dist_batch/             # Output directory for automated batch binaries (36 loaders)
├── payloads/               # Target directory for input ELF payloads (e.g., stage1.elf)
├── builder.c               # Dynamic payload builder implementation
├── builder.h               # Builder structures and prototypes
├── loader.c                # Final loader template source
├── main.c                  # CLI entry point for generator (Single, Multi & Batch)
├── Makefile                # Automated build, installation, and clean rules
└── README.md


===============================================================================
3. COMPILATION & BUILDING
===============================================================================

### Using Makefile (Recommended):

* Build the framework generator:
    make

* Clean compiled binaries and temporary build files (`dist/`, `dist_batch/`, `payload.h`):
    make clean

* Install required system dependencies:
    make deps

### Manual Compilation:

If compiling manually using GCC, ensure you include `-Iinclude` for headers and link required libraries:

    gcc -Wall -Wextra -Wno-unused-variable -Iinclude main.c builder.c -o generator -lcrypto -lssl -lpthread -lm -lz


===============================================================================
4. USAGE
===============================================================================

1. Place your target raw ELF binary into the `payloads/` directory (e.g., `./payloads/stage1.elf`).
2. Run the generator executable:
    ./generator

3. Select your desired execution mode in the CLI wizard:

   * **1) Single Stage:** Encrypts 1 ELF binary paired with 1 evasion technique. Output: `./dist/loader_final.elf`.
   * **2) Multi-Stage:** Chains $N$ ELFs sequentially with derived keys per stage. Output: `./dist/loader_final.elf`.
   * **3) Batch Mode:** Automatically generates **36 individual binary loaders** (one per evasion technique) for automated sandbox evaluation and behavioral analysis. Output: `./dist_batch/loader_<technique_name>.elf`.


===============================================================================
5. SUPPORTED EVASION TECHNIQUES (36 TOTAL)
===============================================================================

* **Compute (101-108):** Hash chains, floating-point math, compression spam, prime calculations, bitwise operations, recursive Fibonacci, junk logic, and Proof-of-Work (PoW).
* **Network & C2 (201-208):** TCP/HTTP/UDP socket delays, DNS lookup flooding, NTP synchronization, network garbage injection, reverse DNS scanning, and Remote C2 Key Retrieval.
* **Storage (301-307):** Disk walking, heavy I/O blips, entropy generation, sparse file creation, directory spamming, memory pressure, and VFS metadata querying (`stat`).
* **Synchronization (401-407):** Thread joining, blocking queues, pipe waiting, semaphore racing, event waiting, barrier synchronization, and condition variables.
* **Temporal (501-507):** Standard sleep, `select()`, `nanosleep()`, signal handling, subprocess waiting, `ppoll()`, and interval timers (`itimer`).
* **Hammering (601-602):** WinAPI/Syscall hammering and memory allocation hammering.


===============================================================================
6. LICENSE & DISCLAIMER
===============================================================================

Disclaimer: This project is intended strictly for educational purposes, security research, and defensive testing in controlled environments. Unauthorized use against target systems without consent is prohibited.
