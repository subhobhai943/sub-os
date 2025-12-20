# SUB OS Makefile

ASM = nasm
CC = gcc
LD = ld

ASM_FLAGS = -f elf32
CC_FLAGS = -m32 -c -ffreestanding -fno-pie -O2 -Wall -Wextra
LD_FLAGS = -m elf_i386 -T linker.ld

BOOT_DIR = boot
KERNEL_DIR = kernel
BUILD_DIR = build

BOOT_BIN = $(BUILD_DIR)/boot.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
OS_IMAGE = $(BUILD_DIR)/sub_os.bin

# Boot assembly files
BOOT_ASM = $(BOOT_DIR)/boot.asm

# Kernel assembly files
KERNEL_ASM_SRC = $(KERNEL_DIR)/kernel_entry.asm \
                 $(KERNEL_DIR)/idt.asm \
                 $(KERNEL_DIR)/isr.asm \
                 $(KERNEL_DIR)/syscall_entry.asm \
                 $(KERNEL_DIR)/task_switch.asm \
                 $(KERNEL_DIR)/tss.asm \
                 $(KERNEL_DIR)/usermode.asm

# Kernel C source files
KERNEL_C_SRC = $(KERNEL_DIR)/kernel.c \
               $(KERNEL_DIR)/idt.c \
               $(KERNEL_DIR)/timer.c \
               $(KERNEL_DIR)/keyboard.c \
               $(KERNEL_DIR)/memory.c \
               $(KERNEL_DIR)/paging.c \
               $(KERNEL_DIR)/pmm.c \
               $(KERNEL_DIR)/heap.c \
               $(KERNEL_DIR)/process.c \
               $(KERNEL_DIR)/scheduler.c \
               $(KERNEL_DIR)/syscall.c \
               $(KERNEL_DIR)/tss.c \
               $(KERNEL_DIR)/ata.c \
               $(KERNEL_DIR)/fs.c

# Print routines and other boot assembly files
PRINT_ASM_SRC = boot/print_string_pm.asm \
                boot/disk_load.asm \
                boot/gdt.asm \
                boot/memory_detect.asm \
                boot/switch_to_pm.asm

PRINT_ASM_OBJ = $(patsubst boot/%.asm, $(BUILD_DIR)/%.o, $(PRINT_ASM_SRC))

# Object files
KERNEL_ASM_OBJ = $(patsubst $(KERNEL_DIR)/%.asm, $(BUILD_DIR)/%.o, $(KERNEL_ASM_SRC))
KERNEL_C_OBJ = $(patsubst $(KERNEL_DIR)/%.c, $(BUILD_DIR)/%.o, $(KERNEL_C_SRC))
KERNEL_OBJ = $(KERNEL_ASM_OBJ) $(KERNEL_C_OBJ) $(PRINT_ASM_OBJ)

.PHONY: all clean run

all: $(OS_IMAGE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Boot sector (raw binary, not ELF32!)
$(BOOT_BIN): $(BOOT_ASM) | $(BUILD_DIR)
	$(ASM) -f bin $< -o $@

# Kernel and print routines (ELF32 object)
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.asm | $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(BOOT_DIR)/%.asm | $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $< -o $@

$(KERNEL_BIN): $(KERNEL_OBJ)
	$(LD) $(LD_FLAGS) $^ -o $@

$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $^ > $@

clean:
	rm -rf $(BUILD_DIR)

run: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE)
