# SUB OS Makefile

ASM = nasm
CC  = gcc
LD  = ld

ASM_FLAGS = -f elf32
CC_FLAGS  = -m32 -c -ffreestanding -fno-pie -O2 -Wall -Wextra \
            -Wno-unused-parameter -Wno-unused-function
LD_FLAGS  = -m elf_i386 -T linker.ld

BOOT_DIR   = boot
KERNEL_DIR = kernel
BUILD_DIR  = build

BOOT_BIN   = $(BUILD_DIR)/boot.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
OS_IMAGE   = $(BUILD_DIR)/sub_os.bin
OS_FLOPPY  = $(BUILD_DIR)/sub_os.img
OS_ISO     = $(BUILD_DIR)/sub_os.iso
ISO_ROOT   = $(BUILD_DIR)/iso_root

BOOT_ASM = $(BOOT_DIR)/boot.asm

KERNEL_ASM_SRC = $(KERNEL_DIR)/kernel_entry.asm \
                 $(KERNEL_DIR)/idt.asm \
                 $(KERNEL_DIR)/isr.asm \
                 $(KERNEL_DIR)/syscall_entry.asm \
                 $(KERNEL_DIR)/task_switch.asm \
                 $(KERNEL_DIR)/tss.asm \
                 $(KERNEL_DIR)/usermode.asm

KERNEL_C_SRC = $(KERNEL_DIR)/kernel.c \
               $(KERNEL_DIR)/gui.c \
               $(KERNEL_DIR)/shell.c \
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

PRINT_ASM_SRC = boot/print_string_pm.asm \
                boot/disk_load.asm \
                boot/gdt.asm \
                boot/memory_detect.asm \
                boot/switch_to_pm.asm

PRINT_ASM_OBJ = $(patsubst boot/%.asm, $(BUILD_DIR)/%.o, $(PRINT_ASM_SRC))
KERNEL_ASM_OBJ = $(patsubst $(KERNEL_DIR)/%.asm, $(BUILD_DIR)/%.o, $(KERNEL_ASM_SRC))
KERNEL_C_OBJ   = $(patsubst $(KERNEL_DIR)/%.c,   $(BUILD_DIR)/%.o, $(KERNEL_C_SRC))
KERNEL_OBJ = $(KERNEL_ASM_OBJ) $(KERNEL_C_OBJ) $(PRINT_ASM_OBJ)

.PHONY: all clean run run-iso iso

all: $(OS_IMAGE) $(OS_FLOPPY) $(OS_ISO)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BOOT_BIN): $(BOOT_ASM) | $(BUILD_DIR)
	$(ASM) -f bin $< -o $@

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

$(OS_FLOPPY): $(OS_IMAGE)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(OS_IMAGE) of=$@ conv=notrunc

$(OS_ISO): $(OS_FLOPPY)
	mkdir -p $(ISO_ROOT)
	cp $(OS_FLOPPY) $(ISO_ROOT)/sub_os.img
	xorriso -as mkisofs -R -J -o $@ -b sub_os.img \
	    -boot-load-size 2880 -boot-info-table $(ISO_ROOT)

iso: $(OS_ISO)

# Run with qemu-system-x86_64 (recommended)
run: $(OS_IMAGE)
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE) -m 32M \
	    -display curses 2>/dev/null || \
	qemu-system-x86_64 -drive format=raw,file=$(OS_IMAGE) -m 32M

# Run ISO with qemu-system-x86_64
run-iso: $(OS_ISO)
	qemu-system-x86_64 -cdrom $(OS_ISO) -m 32M \
	    -display curses 2>/dev/null || \
	qemu-system-x86_64 -cdrom $(OS_ISO) -m 32M

# Legacy: run with qemu-system-i386
run-i386: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE) -m 32M

clean:
	rm -rf $(BUILD_DIR)
