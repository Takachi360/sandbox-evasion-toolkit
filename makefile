# Compiler and Flag Configurations
CC = gcc
CFLAGS = -Wall -Wextra -w -Iinclude
LIBS = -lcrypto -lssl -lpthread -lm -lz

# Targets & Output Directories
TARGET = generator
BUILD_DIR = dist

.PHONY: all clean deps install

# Default build target
all: $(TARGET)

# Compile main binary
$(TARGET): main.c builder.c
	@echo "[+] Building $(TARGET)..."
	$(CC) $(CFLAGS) main.c builder.c -o $(TARGET) $(LIBS)
	@echo "[✔] Build completed successfully."

# Install required system dependencies (Debian/Ubuntu/Kali)
deps:
	@echo "[+] Installing required library dependencies..."
	sudo apt update
	sudo apt install -y build-essential libssl-dev zlib1g-dev

# Clean compiled binaries and temporary editor files
clean:
	@echo "[+] Cleaning binaries and temporary files..."
	rm -f $(TARGET) Cifrando Derivando loader loader_test main
	rm -f payloads/*.elf dist/*.elf
	rm -f include/*.swp *.swp
	@echo "[✔] Cleanup finished."
