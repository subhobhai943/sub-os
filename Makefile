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

# Source files
... [cut for brevity - full sources included] ...
# Add FS_C_SRC and FS_OBJ as above
