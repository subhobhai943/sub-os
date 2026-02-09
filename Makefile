# SUB OS Makefile

# SUB OS Makefile

ARCH := $(shell uname -m)

ifneq (,$(findstring arm,$(ARCH)))
    TARGET_ARCH = arm
else ifeq ($(ARCH),aarch64)
    TARGET_ARCH = arm
else
    TARGET_ARCH = x86
endif

ifeq ($(TARGET_ARCH),arm)
    ASM = as
    CC = gcc
    LD = ld
    
    # ARM Flags
    ASM_FLAGS = -g
    CC_FLAGS = -g -c -ffreestanding -O2 -Wall -Wextra
    LD_FLAGS = -T linker_arm.ld
    
    BOOT_DIR = boot
    KERNEL_DIR = kernel
    BUILD_DIR = build
    
    # ARM Sources
    BOOT_ASM = $(BOOT_DIR)/start.S
    
    # Only compile generic kernel C files
    KERNEL_C_SRC = $(KERNEL_DIR)/kernel.c \
                   $(KERNEL_DIR)/uart.c \
                   $(KERNEL_DIR)/memory.c \
                   $(KERNEL_DIR)/pmm.c \
                   $(KERNEL_DIR)/heap.c \
                   $(KERNEL_DIR)/keyboard.c \
                   $(KERNEL_DIR)/ata.c \
                   $(KERNEL_DIR)/fs.c \
                   $(KERNEL_DIR)/shell.c
    
    # ARM Objects
    KERNEL_OBJ = $(BUILD_DIR)/start.o \
                 $(patsubst $(KERNEL_DIR)/%.c, $(BUILD_DIR)/%.o, $(KERNEL_C_SRC))

    OS_IMAGE = $(BUILD_DIR)/sub_os.elf

else
    # x86 Configuration
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
    OS_FLOPPY = $(BUILD_DIR)/sub_os.img
    OS_ISO = $(BUILD_DIR)/sub_os.iso
    ISO_ROOT = $(BUILD_DIR)/iso_root

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
                   $(KERNEL_DIR)/fs.c \
                   $(KERNEL_DIR)/shell.c

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
endif

.PHONY: all clean run iso

all: $(OS_IMAGE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

ifeq ($(TARGET_ARCH),arm)
# ARM Build Rules
$(BUILD_DIR)/start.o: $(BOOT_ASM) | $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $< -o $@

$(OS_IMAGE): $(KERNEL_OBJ)
	$(LD) $(LD_FLAGS) $^ -o $@

else
# x86 Build Rules
$(BOOT_BIN): $(BOOT_ASM) | $(BUILD_DIR)
	$(ASM) -f bin $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.asm | $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(BOOT_DIR)/%.asm | $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $< -o $@

$(KERNEL_BIN): $(KERNEL_OBJ)
	$(LD) $(LD_FLAGS) $^ -o $@.elf
	objcopy -O binary $@.elf $@

$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $^ > $@

$(OS_FLOPPY): $(OS_IMAGE)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(OS_IMAGE) of=$@ conv=notrunc

$(OS_ISO): $(OS_FLOPPY)
	mkdir -p $(ISO_ROOT)
	cp $(OS_FLOPPY) $(ISO_ROOT)/sub_os.img
	xorriso -as mkisofs -R -J -o $@ -b sub_os.img -boot-load-size 2880 -boot-info-table $(ISO_ROOT)

iso: $(OS_ISO)
endif

clean:
	rm -rf $(BUILD_DIR)

run: $(OS_IMAGE)
ifeq ($(TARGET_ARCH),arm)
	qemu-system-arm -M virt -cpu cortex-a15 -nographic -kernel $(OS_IMAGE)
else
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE)
endif

