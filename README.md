# SUB OS

**The World's Smoothest Operating System - Built from Scratch**

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Status](https://img.shields.io/badge/status-alpha-orange.svg)
![Architecture](https://img.shields.io/badge/arch-x86-green.svg)
![Version](https://img.shields.io/badge/version-0.6.0-blue.svg)

## 🚀 Overview

SUB OS is a custom operating system built entirely from scratch without using existing kernels (Linux, Windows, or macOS). The goal is to create the world's smoothest and lag-free operating system within 365 days.

**Current Version:** Alpha v0.6.0  
**Development Started:** November 11, 2025  
**Current Day:** 6 of 365  
**Progress:** ~15% complete

## ✨ Features

### Currently Implemented (Day 6)
- ✅ Custom bootloader (x86 Assembly)
- ✅ 32-bit Protected Mode
- ✅ Global Descriptor Table (GDT)
- ✅ VGA Text Mode Driver with scrolling
- ✅ Interrupt Descriptor Table (IDT)
- ✅ Exception handling (32 CPU exceptions)
- ✅ Hardware interrupt handling (16 IRQs)
- ✅ Programmable Interrupt Controller (PIC)
- ✅ PS/2 Keyboard driver with buffer
- ✅ PIT Timer driver (100 Hz)
- ✅ System uptime tracking
- ✅ Memory detection (BIOS E820)
- ✅ Physical Memory Manager (bitmap allocator)
- ✅ Virtual Memory (Paging)
- ✅ Page Directory & Page Tables
- ✅ Heap Allocator (kmalloc/kfree)
- ✅ **Process Management (PCB)** 🆕
- ✅ **Round-Robin Scheduler** 🆕
- ✅ **Task Switching** 🆕
- ✅ **Multitasking Support** 🆕

### Roadmap (365 Days)
- [x] Bootloader ✅
- [x] Protected Mode ✅
- [x] Interrupt Handling ✅
- [x] Keyboard driver ✅
- [x] Timer driver ✅
- [x] Memory detection ✅
- [x] Physical Memory Manager ✅
- [x] Virtual Memory (Paging) ✅
- [x] Heap allocator ✅
- [x] Process management ✅
- [x] Scheduler ✅
- [ ] System calls
- [ ] User mode
- [ ] Disk driver
- [ ] File system
- [ ] Shell
- [ ] Multi-core support
- [ ] GUI (stretch goal)

## 🛠️ Building SUB OS

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt install build-essential nasm qemu-system-x86

# Arch Linux
sudo pacman -S base-devel nasm qemu

# macOS
brew install nasm qemu i686-elf-gcc
```

### Build and Run

```bash
# Clone repository
git clone https://github.com/subhobhai943/sub-os.git
cd sub-os

# Build
make

# Run (128MB RAM)
make run

# Debug mode
make debug

# Clean
make clean
```

## 📁 Project Structure

```
sub-os/
├── boot/                      # Bootloader
│   ├── boot.asm              # Main bootloader
│   ├── gdt.asm               # GDT
│   ├── disk_load.asm         # Disk I/O
│   ├── memory_detect.asm     # E820 detection
│   ├── print_string.asm      # Real mode print
│   ├── print_string_pm.asm   # Protected mode print
│   └── switch_to_pm.asm      # Mode switch
├── kernel/                    # Kernel
│   ├── kernel_entry.asm      # Entry point
│   ├── kernel.c              # Main kernel
│   ├── kernel.h              # Header
│   ├── idt.asm               # IDT assembly
│   ├── isr.asm               # ISR handlers
│   ├── task_switch.asm       # Task switcher (NEW!)
│   ├── idt.c                 # IDT implementation
│   ├── idt.h                 # IDT header
│   ├── keyboard.c            # Keyboard driver
│   ├── keyboard.h            # Header
│   ├── timer.c               # Timer driver
│   ├── timer.h               # Header
│   ├── memory.c              # Memory detection
│   ├── memory.h              # Header
│   ├── pmm.c                 # Physical memory
│   ├── pmm.h                 # Header
│   ├── paging.c              # Virtual memory
│   ├── paging.h              # Header
│   ├── heap.c                # Heap allocator
│   ├── heap.h                # Header
│   ├── process.c             # Process mgmt (NEW!)
│   ├── process.h             # Header (NEW!)
│   ├── scheduler.c           # Scheduler (NEW!)
│   └── (scheduler in process.h)
├── Makefile                   # Build system
├── linker.ld                  # Linker script
└── README.md                  # Documentation
```

## 🎯 Design Goals

1. **Performance First**: Minimal context switch overhead
2. **Lag-Free Experience**: Optimized scheduling
3. **Clean Architecture**: Modular, documented code
4. **Educational**: Resource for OS development

## 🎮 Try It Out!

When you run SUB OS:

1. Boot sequence with all subsystems
2. Memory detection and configuration
3. Process creation (idle + 2 test processes)
4. Scheduler activation
5. Live uptime counter
6. Interactive keyboard input

### Multitasking in Action:
- **3 processes** running concurrently
- **Round-robin scheduling** (50ms quantum)
- **Context switches** every time slice
- **Isolated address spaces** (via paging)

## 📚 Technical Details

### Process Management
- **PCB Structure**: Contains PID, name, state, registers, stacks, page directory
- **Process Creation**: `process_create()` allocates PCB, stack, initializes context
- **Process States**: READY, RUNNING, BLOCKED, TERMINATED
- **Context Switching**: Save all registers, switch page directory, restore registers

### Scheduler
- **Algorithm**: Round-Robin (fair time-sharing)
- **Time Quantum**: 50ms (5 timer ticks at 100 Hz)
- **Ready Queue**: Linked list of runnable processes
- **Preemption**: Timer interrupt triggers scheduler
- **Idle Process**: PID 0, runs when no other process ready

### Task Switching
- **Assembly Implementation**: Fast register save/restore
- **Saved State**: EAX, EBX, ECX, EDX, ESI, EDI, ESP, EBP, EIP, EFLAGS, CR3
- **Page Directory Switch**: Enables per-process virtual memory
- **Return Path**: Jumps to saved EIP in new process

### Virtual Memory
- **Page Size**: 4KB
- **Address Space**: 4GB per process
- **Kernel Mapping**: Identity mapped (first 4MB)
- **Heap Space**: Dynamically mapped
- **Protection**: Page-level read/write/user permissions

### Memory Management
- **Physical**: Bitmap allocator, page-level tracking
- **Virtual**: Page directory + page tables
- **Heap**: kmalloc/kfree with coalescing
- **Statistics**: Real-time usage monitoring

## 📊 System Capabilities

✅ Boot from custom bootloader  
✅ Detect and manage system memory  
✅ Virtual memory with paging  
✅ Dynamic memory allocation  
✅ Hardware interrupt handling  
✅ Process creation and management  
✅ Round-robin scheduling  
✅ **Multitasking** - Multiple processes running  
✅ Keyboard input and echo  
✅ System uptime tracking  
✅ VGA text output with scrolling  

## 📊 System Requirements

- **CPU**: x86 (32-bit) or x86-64
- **RAM**: Minimum 1MB (128MB recommended)
- **Disk**: 10KB+ space
- **Display**: VGA text mode (80x25)

## 📚 Learning Resources

- [OSDev Wiki](https://wiki.osdev.org/)
- [Process Scheduling](https://wiki.osdev.org/Scheduling_Algorithms)
- [Context Switching](https://wiki.osdev.org/Context_Switching)
- [Intel x86 Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)

## 🤝 Contributing

Solo 365-day challenge project. Suggestions welcome via issues!

## 📝 License

MIT License

## 👨‍💻 Author

**Subhobhai**  
PCMB student from West Bengal

## 📊 Development Log

| Date | Milestone | Status |
|------|-----------|--------|
| Nov 11, 2025 | Bootloader, kernel | ✅ |
| Nov 11, 2025 | IDT, keyboard | ✅ |
| Nov 11, 2025 | Timer, memory detection | ✅ |
| Nov 11, 2025 | PMM, heap | ✅ |
| Nov 11, 2025 | Virtual memory | ✅ |
| Nov 11, 2025 | Process management, scheduler | ✅ |

## 🎯 Next Steps (Day 7)

- System call interface
- User mode support
- Fork/exec implementation
- Inter-process communication

## 📈 Statistics

- **Days**: 6 of 365 (~1.6%)
- **Files**: 38
- **Lines**: ~5,000
- **Commits**: 28+
- **Features**: 22 systems
- **Phase 1**: ~40% complete

---

**Status**: Day 6 of 365 - Multitasking enabled! ⚙️🔄🚀

SUB OS now supports multiple processes running concurrently - a major milestone in OS development!
