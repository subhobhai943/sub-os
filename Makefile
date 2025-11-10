# SUB OS Makefile
# Build system for SUB OS

# Compiler and assembler
ASM = nasm
CC = gcc
LD = ld

# Flags
ASM_FLAGS = -f elf32
CC_FLAGS = -m32 -c -ffreestanding -fno-pie -O2 -Wall -Wextra
LD_FLAGS = -m elf_i386 -T linker.ld

# Directories
BOOT_DIR = boot
KERNEL_DIR = kernel
BUILD_DIR = build

# Output files
BOOT_BIN = $(BUILD_DIR)/boot.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
OS_IMAGE = $(BUILD_DIR)/sub_os.bin

# Source files
BOOT_SRC = $(BOOT_DIR)/boot.asm
KERNEL_ENTRY_SRC = $(KERNEL_DIR)/kernel_entry.asm
KERNEL_C_SRC = $(KERNEL_DIR)/kernel.c

# Object files
KERNEL_ENTRY_OBJ = $(BUILD_DIR)/kernel_entry.o
KERNEL_OBJ = $(BUILD_DIR)/kernel.o

# Default target
all: $(OS_IMAGE)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build bootloader
$(BOOT_BIN): $(BOOT_SRC) | $(BUILD_DIR)
	$(ASM) -f bin $(BOOT_SRC) -o $(BOOT_BIN)

# Build kernel entry
$(KERNEL_ENTRY_OBJ): $(KERNEL_ENTRY_SRC) | $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) $(KERNEL_ENTRY_SRC) -o $(KERNEL_ENTRY_OBJ)

# Build kernel C code
$(KERNEL_OBJ): $(KERNEL_C_SRC) | $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $(KERNEL_C_SRC) -o $(KERNEL_OBJ)

# Link kernel
$(KERNEL_BIN): $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ) | $(BUILD_DIR)
	$(LD) $(LD_FLAGS) -o $(KERNEL_BIN) $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ)

# Create OS image
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(OS_IMAGE)
	@echo "SUB OS image created: $(OS_IMAGE)"

# Run in QEMU
run: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE)

# Run with serial output for debugging
debug: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE) -serial stdio

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run debug clean
