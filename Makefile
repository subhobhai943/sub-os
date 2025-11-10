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
IDT_ASM_SRC = $(KERNEL_DIR)/idt.asm
ISR_ASM_SRC = $(KERNEL_DIR)/isr.asm
IDT_C_SRC = $(KERNEL_DIR)/idt.c
KEYBOARD_C_SRC = $(KERNEL_DIR)/keyboard.c
TIMER_C_SRC = $(KERNEL_DIR)/timer.c
MEMORY_C_SRC = $(KERNEL_DIR)/memory.c

# Object files
KERNEL_ENTRY_OBJ = $(BUILD_DIR)/kernel_entry.o
KERNEL_OBJ = $(BUILD_DIR)/kernel.o
IDT_ASM_OBJ = $(BUILD_DIR)/idt.o
ISR_ASM_OBJ = $(BUILD_DIR)/isr.o
IDT_OBJ = $(BUILD_DIR)/idt_c.o
KEYBOARD_OBJ = $(BUILD_DIR)/keyboard.o
TIMER_OBJ = $(BUILD_DIR)/timer.o
MEMORY_OBJ = $(BUILD_DIR)/memory.o

# All object files
OBJS = $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ) $(IDT_ASM_OBJ) $(ISR_ASM_OBJ) \
       $(IDT_OBJ) $(KEYBOARD_OBJ) $(TIMER_OBJ) $(MEMORY_OBJ)

# Default target
all: $(OS_IMAGE)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build bootloader
$(BOOT_BIN): $(BOOT_SRC) | $(BUILD_DIR)
	@echo "Building bootloader..."
	$(ASM) -f bin $(BOOT_SRC) -o $(BOOT_BIN)

# Build kernel entry
$(KERNEL_ENTRY_OBJ): $(KERNEL_ENTRY_SRC) | $(BUILD_DIR)
	@echo "Building kernel entry..."
	$(ASM) $(ASM_FLAGS) $(KERNEL_ENTRY_SRC) -o $(KERNEL_ENTRY_OBJ)

# Build IDT assembly
$(IDT_ASM_OBJ): $(IDT_ASM_SRC) | $(BUILD_DIR)
	@echo "Building IDT..."
	$(ASM) $(ASM_FLAGS) $(IDT_ASM_SRC) -o $(IDT_ASM_OBJ)

# Build ISR assembly
$(ISR_ASM_OBJ): $(ISR_ASM_SRC) | $(BUILD_DIR)
	@echo "Building ISRs..."
	$(ASM) $(ASM_FLAGS) $(ISR_ASM_SRC) -o $(ISR_ASM_OBJ)

# Build kernel C code
$(KERNEL_OBJ): $(KERNEL_C_SRC) | $(BUILD_DIR)
	@echo "Building kernel..."
	$(CC) $(CC_FLAGS) $(KERNEL_C_SRC) -o $(KERNEL_OBJ)

# Build IDT C code
$(IDT_OBJ): $(IDT_C_SRC) | $(BUILD_DIR)
	@echo "Building IDT C code..."
	$(CC) $(CC_FLAGS) $(IDT_C_SRC) -o $(IDT_OBJ)

# Build keyboard C code
$(KEYBOARD_OBJ): $(KEYBOARD_C_SRC) | $(BUILD_DIR)
	@echo "Building keyboard driver..."
	$(CC) $(CC_FLAGS) $(KEYBOARD_C_SRC) -o $(KEYBOARD_OBJ)

# Build timer C code
$(TIMER_OBJ): $(TIMER_C_SRC) | $(BUILD_DIR)
	@echo "Building timer driver..."
	$(CC) $(CC_FLAGS) $(TIMER_C_SRC) -o $(TIMER_OBJ)

# Build memory C code
$(MEMORY_OBJ): $(MEMORY_C_SRC) | $(BUILD_DIR)
	@echo "Building memory manager..."
	$(CC) $(CC_FLAGS) $(MEMORY_C_SRC) -o $(MEMORY_OBJ)

# Link kernel
$(KERNEL_BIN): $(OBJS) | $(BUILD_DIR)
	@echo "Linking kernel..."
	$(LD) $(LD_FLAGS) -o $(KERNEL_BIN) $(OBJS)

# Create OS image
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	@echo "Creating OS image..."
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(OS_IMAGE)
	@echo ""
	@echo "=========================================="
	@echo "  SUB OS v0.3.0 Build Complete!"
	@echo "=========================================="
	@echo "Image: $(OS_IMAGE)"
	@echo "Size: $$(stat -f%z $(OS_IMAGE) 2>/dev/null || stat -c%s $(OS_IMAGE)) bytes"
	@echo ""
	@echo "Run with: make run"
	@echo "=========================================="

# Run in QEMU
run: $(OS_IMAGE)
	@echo ""
	@echo "Starting SUB OS v0.3.0..."
	@echo ""
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE)

# Run with serial output for debugging
debug: $(OS_IMAGE)
	@echo ""
	@echo "Starting SUB OS v0.3.0 (Debug Mode)..."
	@echo ""
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE) -serial stdio

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
	@echo "Build directory cleaned"

# Display help
help:
	@echo "SUB OS Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make         - Build SUB OS"
	@echo "  make run     - Build and run in QEMU"
	@echo "  make debug   - Build and run with debug output"
	@echo "  make clean   - Remove build files"
	@echo "  make help    - Show this help"

.PHONY: all run debug clean help
