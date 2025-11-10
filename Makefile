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

BOOT_SRC = $(BOOT_DIR)/boot.asm
KERNEL_ENTRY_SRC = $(KERNEL_DIR)/kernel_entry.asm
KERNEL_C_SRC = $(KERNEL_DIR)/kernel.c
IDT_ASM_SRC = $(KERNEL_DIR)/idt.asm
ISR_ASM_SRC = $(KERNEL_DIR)/isr.asm
TASK_SWITCH_SRC = $(KERNEL_DIR)/task_switch.asm
SYSCALL_ENTRY_SRC = $(KERNEL_DIR)/syscall_entry.asm
TSS_ASM_SRC = $(KERNEL_DIR)/tss.asm
USERMODE_ASM_SRC = $(KERNEL_DIR)/usermode.asm
IDT_C_SRC = $(KERNEL_DIR)/idt.c
KEYBOARD_C_SRC = $(KERNEL_DIR)/keyboard.c
TIMER_C_SRC = $(KERNEL_DIR)/timer.c
MEMORY_C_SRC = $(KERNEL_DIR)/memory.c
PMM_C_SRC = $(KERNEL_DIR)/pmm.c
HEAP_C_SRC = $(KERNEL_DIR)/heap.c
PAGING_C_SRC = $(KERNEL_DIR)/paging.c
PROCESS_C_SRC = $(KERNEL_DIR)/process.c
SCHEDULER_C_SRC = $(KERNEL_DIR)/scheduler.c
SYSCALL_C_SRC = $(KERNEL_DIR)/syscall.c
TSS_C_SRC = $(KERNEL_DIR)/tss.c

KERNEL_ENTRY_OBJ = $(BUILD_DIR)/kernel_entry.o
KERNEL_OBJ = $(BUILD_DIR)/kernel.o
IDT_ASM_OBJ = $(BUILD_DIR)/idt.o
ISR_ASM_OBJ = $(BUILD_DIR)/isr.o
TASK_SWITCH_OBJ = $(BUILD_DIR)/task_switch.o
SYSCALL_ENTRY_OBJ = $(BUILD_DIR)/syscall_entry.o
TSS_ASM_OBJ = $(BUILD_DIR)/tss_asm.o
USERMODE_ASM_OBJ = $(BUILD_DIR)/usermode.o
IDT_OBJ = $(BUILD_DIR)/idt_c.o
KEYBOARD_OBJ = $(BUILD_DIR)/keyboard.o
TIMER_OBJ = $(BUILD_DIR)/timer.o
MEMORY_OBJ = $(BUILD_DIR)/memory.o
PMM_OBJ = $(BUILD_DIR)/pmm.o
HEAP_OBJ = $(BUILD_DIR)/heap.o
PAGING_OBJ = $(BUILD_DIR)/paging.o
PROCESS_OBJ = $(BUILD_DIR)/process.o
SCHEDULER_OBJ = $(BUILD_DIR)/scheduler.o
SYSCALL_OBJ = $(BUILD_DIR)/syscall.o
TSS_OBJ = $(BUILD_DIR)/tss.o

OBJS = $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ) $(IDT_ASM_OBJ) $(ISR_ASM_OBJ) \
       $(TASK_SWITCH_OBJ) $(SYSCALL_ENTRY_OBJ) $(TSS_ASM_OBJ) $(USERMODE_ASM_OBJ) \
       $(IDT_OBJ) $(KEYBOARD_OBJ) $(TIMER_OBJ) $(MEMORY_OBJ) $(PMM_OBJ) \
       $(HEAP_OBJ) $(PAGING_OBJ) $(PROCESS_OBJ) $(SCHEDULER_OBJ) \
       $(SYSCALL_OBJ) $(TSS_OBJ)

all: $(OS_IMAGE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BOOT_BIN): $(BOOT_SRC) | $(BUILD_DIR)
	@echo "Building bootloader..."
	$(ASM) -f bin $(BOOT_SRC) -o $(BOOT_BIN)

$(KERNEL_ENTRY_OBJ): $(KERNEL_ENTRY_SRC) | $(BUILD_DIR)
	@echo "Building kernel entry..."
	$(ASM) $(ASM_FLAGS) $(KERNEL_ENTRY_SRC) -o $(KERNEL_ENTRY_OBJ)

$(IDT_ASM_OBJ): $(IDT_ASM_SRC) | $(BUILD_DIR)
	@echo "Building IDT..."
	$(ASM) $(ASM_FLAGS) $(IDT_ASM_SRC) -o $(IDT_ASM_OBJ)

$(ISR_ASM_OBJ): $(ISR_ASM_SRC) | $(BUILD_DIR)
	@echo "Building ISRs..."
	$(ASM) $(ASM_FLAGS) $(ISR_ASM_SRC) -o $(ISR_ASM_OBJ)

$(TASK_SWITCH_OBJ): $(TASK_SWITCH_SRC) | $(BUILD_DIR)
	@echo "Building task switcher..."
	$(ASM) $(ASM_FLAGS) $(TASK_SWITCH_SRC) -o $(TASK_SWITCH_OBJ)

$(SYSCALL_ENTRY_OBJ): $(SYSCALL_ENTRY_SRC) | $(BUILD_DIR)
	@echo "Building syscall entry..."
	$(ASM) $(ASM_FLAGS) $(SYSCALL_ENTRY_SRC) -o $(SYSCALL_ENTRY_OBJ)

$(TSS_ASM_OBJ): $(TSS_ASM_SRC) | $(BUILD_DIR)
	@echo "Building TSS assembly..."
	$(ASM) $(ASM_FLAGS) $(TSS_ASM_SRC) -o $(TSS_ASM_OBJ)

$(USERMODE_ASM_OBJ): $(USERMODE_ASM_SRC) | $(BUILD_DIR)
	@echo "Building usermode entry..."
	$(ASM) $(ASM_FLAGS) $(USERMODE_ASM_SRC) -o $(USERMODE_ASM_OBJ)

$(KERNEL_OBJ): $(KERNEL_C_SRC) | $(BUILD_DIR)
	@echo "Building kernel..."
	$(CC) $(CC_FLAGS) $(KERNEL_C_SRC) -o $(KERNEL_OBJ)

$(IDT_OBJ): $(IDT_C_SRC) | $(BUILD_DIR)
	@echo "Building IDT C code..."
	$(CC) $(CC_FLAGS) $(IDT_C_SRC) -o $(IDT_OBJ)

$(KEYBOARD_OBJ): $(KEYBOARD_C_SRC) | $(BUILD_DIR)
	@echo "Building keyboard driver..."
	$(CC) $(CC_FLAGS) $(KEYBOARD_C_SRC) -o $(KEYBOARD_OBJ)

$(TIMER_OBJ): $(TIMER_C_SRC) | $(BUILD_DIR)
	@echo "Building timer driver..."
	$(CC) $(CC_FLAGS) $(TIMER_C_SRC) -o $(TIMER_OBJ)

$(MEMORY_OBJ): $(MEMORY_C_SRC) | $(BUILD_DIR)
	@echo "Building memory manager..."
	$(CC) $(CC_FLAGS) $(MEMORY_C_SRC) -o $(MEMORY_OBJ)

$(PMM_OBJ): $(PMM_C_SRC) | $(BUILD_DIR)
	@echo "Building physical memory manager..."
	$(CC) $(CC_FLAGS) $(PMM_C_SRC) -o $(PMM_OBJ)

$(HEAP_OBJ): $(HEAP_C_SRC) | $(BUILD_DIR)
	@echo "Building heap allocator..."
	$(CC) $(CC_FLAGS) $(HEAP_C_SRC) -o $(HEAP_OBJ)

$(PAGING_OBJ): $(PAGING_C_SRC) | $(BUILD_DIR)
	@echo "Building paging system..."
	$(CC) $(CC_FLAGS) $(PAGING_C_SRC) -o $(PAGING_OBJ)

$(PROCESS_OBJ): $(PROCESS_C_SRC) | $(BUILD_DIR)
	@echo "Building process management..."
	$(CC) $(CC_FLAGS) $(PROCESS_C_SRC) -o $(PROCESS_OBJ)

$(SCHEDULER_OBJ): $(SCHEDULER_C_SRC) | $(BUILD_DIR)
	@echo "Building scheduler..."
	$(CC) $(CC_FLAGS) $(SCHEDULER_C_SRC) -o $(SCHEDULER_OBJ)

$(SYSCALL_OBJ): $(SYSCALL_C_SRC) | $(BUILD_DIR)
	@echo "Building system calls..."
	$(CC) $(CC_FLAGS) $(SYSCALL_C_SRC) -o $(SYSCALL_OBJ)

$(TSS_OBJ): $(TSS_C_SRC) | $(BUILD_DIR)
	@echo "Building TSS..."
	$(CC) $(CC_FLAGS) $(TSS_C_SRC) -o $(TSS_OBJ)

$(KERNEL_BIN): $(OBJS) | $(BUILD_DIR)
	@echo "Linking kernel..."
	$(LD) $(LD_FLAGS) -o $(KERNEL_BIN) $(OBJS)

$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	@echo "Creating OS image..."
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(OS_IMAGE)
	@echo ""
	@echo "==========================================="
	@echo "  SUB OS v0.8.0 Build Complete!"
	@echo "==========================================="
	@echo "Image: $(OS_IMAGE)"
	@echo "Size: $$(stat -f%z $(OS_IMAGE) 2>/dev/null || stat -c%s $(OS_IMAGE)) bytes"
	@echo ""
	@echo "Day 8 Features:"
	@echo "  - User Mode (Ring 3)"
	@echo "  - Task State Segment (TSS)"
	@echo "  - Privilege Transitions"
	@echo "  - User Mode Syscalls"
	@echo ""
	@echo "Total: 26 major systems"
	@echo "Run with: make run"
	@echo "==========================================="

run: $(OS_IMAGE)
	@echo ""
	@echo "Starting SUB OS v0.8.0..."
	@echo "User mode enabled!"
	@echo ""
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE) -m 128M

debug: $(OS_IMAGE)
	@echo ""
	@echo "Starting SUB OS v0.8.0 (Debug Mode)..."
	@echo ""
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE) -m 128M -serial stdio

clean:
	rm -rf $(BUILD_DIR)
	@echo "Build directory cleaned"

help:
	@echo "SUB OS Build System - v0.8.0"
	@echo ""
	@echo "Targets:"
	@echo "  make         - Build SUB OS"
	@echo "  make run     - Build and run in QEMU (128MB RAM)"
	@echo "  make debug   - Build and run with debug output"
	@echo "  make clean   - Remove build files"
	@echo "  make help    - Show this help"

.PHONY: all run debug clean help
