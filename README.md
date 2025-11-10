# SUB OS

**The World's Smoothest Operating System - Built from Scratch**

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Status](https://img.shields.io/badge/status-alpha-orange.svg)
![Architecture](https://img.shields.io/badge/arch-x86-green.svg)
![Version](https://img.shields.io/badge/version-0.8.0-blue.svg)

## 🚀 Overview

SUB OS is a custom operating system built entirely from scratch in just 8 days! Features user mode execution, multitasking, virtual memory, and system calls.

**Current Version:** Alpha v0.8.0  
**Development Started:** November 11, 2025  
**Current Day:** 8 of 365  
**Progress:** ~20% complete  
**Status:** All core systems working! ✅

## ✨ Features

### Fully Implemented (Day 1-8)

**Boot & Protection:**
- ✅ Custom bootloader (Assembly)
- ✅ Protected Mode (32-bit)
- ✅ GDT with Ring 0/3 segments
- ✅ IDT (256 interrupt gates)

**Drivers:**
- ✅ VGA Text Mode (80x25)
- ✅ PS/2 Keyboard (buffered)
- ✅ PIT Timer (100 Hz)
- ✅ Interrupt handling (48 handlers)

**Memory Management:**
- ✅ E820 memory detection
- ✅ Physical Memory Manager (bitmap)
- ✅ Virtual Memory (paging, 4KB pages)
- ✅ Heap allocator (kmalloc/kfree)

**Process Management:**
- ✅ Process Control Blocks (PCB)
- ✅ Round-robin scheduler
- ✅ Task switching
- ✅ Kernel & user mode processes

**System Interface:**
- ✅ System calls (INT 0x80)
- ✅ 9 syscalls implemented
- ✅ User mode (Ring 3)
- ✅ Task State Segment (TSS)
- ✅ Privilege transitions

## 📚 Architecture

### Memory Layout
```
0x00000000 - 0x000003FF : BIOS IVT
0x00000400 - 0x000004FF : BIOS Data Area
0x00000500 - 0x00007BFF : Free
0x00007C00 - 0x00007DFF : Bootloader
0x00007E00 - 0x00009FFF : Stack
0x0000A000 - 0x0000FFFF : Free
0x00010000 - 0x???????? : Bitmap (PMM)
0x???????? - 0x00100000 : Kernel
0x00100000+ : Free RAM (managed)
```

### Privilege Levels
- **Ring 0**: Kernel mode - full access
- **Ring 3**: User mode - restricted

### System Calls
```c
// getpid() - Get process ID
int pid;
asm("mov $6, %%eax; int $0x80" : "=a"(pid));

// write() - Write to stdout
const char* msg = "Hello!";
asm("mov $3, %%eax; mov $1, %%ebx; mov %0, %%ecx; mov $6, %%edx; int $0x80" 
    :: "r"(msg));

// yield() - Yield CPU
asm("mov $8, %%eax; int $0x80");
```

## 🛠️ Building

### Requirements
- **NASM** (assembler)
- **GCC** (cross-compiler: i686-elf-gcc or -m32)
- **LD** (linker)
- **QEMU** (x86 emulator)
- **Make**

### Ubuntu/Debian
```bash
sudo apt install build-essential nasm qemu-system-x86
```

### Arch Linux
```bash
sudo pacman -S base-devel nasm qemu
```

### macOS
```bash
brew install nasm qemu
brew install i686-elf-gcc  # Cross-compiler
```

### Build & Run
```bash
git clone https://github.com/subhobhai943/sub-os.git
cd sub-os

# Build
make

# Run in QEMU (128MB RAM)
make run

# Debug mode
make debug

# Clean
make clean
```

## 📁 Project Structure

```
sub-os/
├── boot/
│   ├── boot.asm              # Bootloader
│   ├── gdt.asm               # GDT setup
│   ├── disk_load.asm         # Disk I/O
│   ├── memory_detect.asm     # E820
│   └── ...
├── kernel/
│   ├── kernel_entry.asm      # Entry
│   ├── kernel.c              # Main
│   ├── idt.asm/c             # Interrupts
│   ├── isr.asm               # ISR stubs
│   ├── keyboard.c            # Keyboard
│   ├── timer.c               # Timer
│   ├── memory.c              # E820
│   ├── pmm.c                 # Physical mem
│   ├── paging.c              # Virtual mem
│   ├── heap.c                # Heap
│   ├── process.c             # Processes
│   ├── scheduler.c           # Scheduler
│   ├── syscall.c             # Syscalls
│   ├── task_switch.asm       # Context switch
│   ├── syscall_entry.asm     # INT 0x80
│   ├── tss.c/asm             # TSS
│   ├── usermode.asm          # Ring 3 entry
│   └── ...
├── Makefile                   # Build system
├── linker.ld                  # Linker script
└── README.md                  # This file
```

## 📊 Statistics

- **Development Time**: 8 days
- **Total Files**: 50+
- **Lines of Code**: ~7,000
- **Commits**: 38
- **Major Systems**: 26
- **Phase 1 Progress**: 50%

## 🏆 Milestones

| Day | Feature | Status |
|-----|---------|--------|
| 1-2 | Bootloader, Protected Mode | ✅ |
| 3 | Interrupts, Drivers | ✅ |
| 4 | Memory Detection, PMM, Heap | ✅ |
| 5 | Virtual Memory (Paging) | ✅ |
| 6 | Processes, Scheduler | ✅ |
| 7 | System Calls | ✅ |
| 8 | User Mode, TSS | ✅ |

## 🎯 What's Next

**Phase 2 (Days 9-30):**
- ELF Loader
- Fork/Exec
- ATA/IDE Disk Driver
- File System (FAT12 or custom)
- Shell (command-line)
- Standard Library basics

**Phase 3 (Days 31-60):**
- VFS (Virtual File System)
- Pipes & Redirection
- Multi-core support
- Network stack basics

**Phase 4 (Days 61-365):**
- GUI framework
- User applications
- Package manager
- Optimization & polish

## 🧪 Testing

After building, you should see:

========================================
     SUB OS - Alpha v0.8.0
     Built from Scratch
========================================
[OK] Bootloader initialized
[OK] Protected mode enabled
[OK] GDT loaded
...
[OK] TSS initialized
Creating processes...
  Created kernel process (PID 1)
  Created USER MODE process (PID 2)
SUB OS Kernel Features:
  * 32-bit Protected Mode
  * Interrupt Handling (IDT)
  * PS/2 Keyboard Driver
  * PIT Timer Driver
  * Memory Detection (E820)
  * Physical Memory Manager
  * Virtual Memory (Paging)
  * Heap Allocator
  * Process Management (PCB)
  * Round-Robin Scheduler
  * System Calls (INT 0x80)
  * User Mode (Ring 3)
  * Task State Segment (TSS)
System Status: RUNNING
Active Processes: 3 (2 kernel + 1 user)
User mode enabled! Ring 3 execution active.

## 🤝 Contributing

Solo 365-day challenge project. Bug reports and suggestions welcome!

## 📝 License

MIT License

## 👨‍💻 Author

**Subhobhai**   
PCMB student from West Bengal

---

**Status**: Day 8 Complete - User Mode Working! 🔐⚙️🚀

Built in 8 days what takes most projects 6+ months!
