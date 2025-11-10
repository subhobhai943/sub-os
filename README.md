# SUB OS

**The World's Smoothest Operating System - Built from Scratch**

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Status](https://img.shields.io/badge/status-alpha-orange.svg)
![Architecture](https://img.shields.io/badge/arch-x86-green.svg)
![Version](https://img.shields.io/badge/version-0.5.0-blue.svg)

## 🚀 Overview

SUB OS is a custom operating system built entirely from scratch without using existing kernels (Linux, Windows, or macOS). The goal is to create the world's smoothest and lag-free operating system within 365 days.

**Current Version:** Alpha v0.5.0  
**Development Started:** November 11, 2025  
**Current Day:** 5 of 365  
**Progress:** ~10% complete

## ✨ Features

### Currently Implemented (Day 5)
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
- ✅ Memory map parsing and display
- ✅ Physical Memory Manager (bitmap allocator)
- ✅ **Virtual Memory (Paging)** 🆕
- ✅ **Page Directory & Page Tables** 🆕
- ✅ **Identity Mapping** 🆕
- ✅ Heap Allocator (kmalloc/kfree)
- ✅ Modular kernel architecture

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
- [ ] Process management
- [ ] Scheduler
- [ ] File system
- [ ] Disk driver
- [ ] System calls
- [ ] User mode
- [ ] Shell
- [ ] GUI (stretch goal)

## 🛠️ Building SUB OS

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt install build-essential nasm qemu-system-x86

# Arch Linux
sudo pacman -S base-devel nasm qemu

# macOS
brew install nasm qemu
brew install i686-elf-gcc  # Cross-compiler
```

### Build and Run

```bash
# Clone the repository
git clone https://github.com/subhobhai943/sub-os.git
cd sub-os

# Build the OS
make

# Run in QEMU emulator
make run

# Run with debugging output
make debug

# Clean build files
make clean

# Show help
make help
```

## 📁 Project Structure

```
sub-os/
├── boot/                      # Bootloader code
│   ├── boot.asm              # Main bootloader
│   ├── gdt.asm               # Global Descriptor Table
│   ├── disk_load.asm         # Disk reading
│   ├── memory_detect.asm     # E820 memory detection
│   ├── print_string.asm      # Real mode printing
│   ├── print_string_pm.asm   # Protected mode printing
│   └── switch_to_pm.asm      # Mode switching
├── kernel/                    # Kernel source code
│   ├── kernel_entry.asm      # Kernel entry point
│   ├── kernel.c              # Main kernel
│   ├── kernel.h              # Kernel header
│   ├── idt.asm               # IDT assembly
│   ├── isr.asm               # ISR handlers
│   ├── idt.c                 # IDT C implementation
│   ├── idt.h                 # IDT header
│   ├── keyboard.c            # Keyboard driver
│   ├── keyboard.h            # Keyboard header
│   ├── timer.c               # Timer driver
│   ├── timer.h               # Timer header
│   ├── memory.c              # Memory detection
│   ├── memory.h              # Memory header
│   ├── pmm.c                 # Physical Memory Manager
│   ├── pmm.h                 # PMM header
│   ├── paging.c              # Virtual Memory (NEW!)
│   ├── paging.h              # Paging header (NEW!)
│   ├── heap.c                # Heap allocator
│   └── heap.h                # Heap header
├── build/                     # Build output (generated)
├── Makefile                   # Build system
├── linker.ld                  # Linker script
└── README.md                  # This file
```

## 🎯 Design Goals

1. **Performance First**: Minimize overhead and optimize scheduling
2. **Lag-Free Experience**: Advanced caching and predictive algorithms
3. **Clean Architecture**: Well-documented, modular code
4. **Educational**: Serve as a learning resource for OS development

## 🎮 Try It Out!

When you run SUB OS, you'll see:

1. **Boot Sequence**: Bootloader initialization
2. **Memory Detection**: E820 memory map
3. **Paging Setup**: Virtual memory enabled
4. **System Information**: Hardware details
5. **Live Uptime**: Updates every second
6. **Interactive Input**: Type and see echo

### What You Can Do:
- **Type** and see real-time character echo
- **Watch** the uptime counter increment
- **See** detailed memory information
- **Experience** a fully functional paging system

## 📚 Technical Details

### Boot Process
1. BIOS loads bootloader at 0x7c00
2. Bootloader detects memory (E820)
3. Bootloader loads kernel from disk
4. GDT setup and Protected Mode switch
5. Jump to kernel at 0x1000

### Virtual Memory (Paging)
- **Page Size**: 4KB (4096 bytes)
- **Page Directory**: 1024 entries
- **Page Tables**: 1024 entries each
- **Identity Mapping**: First 4MB (0x0 - 0x400000)
- **Heap Mapping**: 1MB heap space (0x400000 - 0x500000)
- **Page Faults**: Handled with detailed error reporting

### Memory Management
**Physical Memory Manager**:
- Bitmap allocator (1 bit per page)
- Single page allocation
- Multi-page contiguous allocation
- Statistics tracking

**Virtual Memory**:
- Page directory at CR3
- Page tables on-demand
- Identity mapped kernel
- Separate address spaces possible

**Heap Allocator**:
- Dynamic memory allocation
- Block coalescing
- First-fit strategy
- 64KB initial size

### Interrupt Handling
- **IDT**: 256-entry table
- **Exceptions**: 32 CPU exceptions including page faults
- **IRQs**: 16 hardware interrupts
  - IRQ0: Timer (PIT)
  - IRQ1: Keyboard
  - IRQ14: Page Fault Handler
- **PIC**: 8259 remapped

### Keyboard Driver
- PS/2 protocol
- 256-byte circular buffer
- US QWERTY layout
- Special key support

### Timer System
- PIT configured to 100 Hz
- IRQ0 interrupt handler
- Uptime tracking
- Sleep/wait functions

## 📊 System Requirements

- **CPU**: x86 (32-bit) or x86-64
- **RAM**: Minimum 1MB (auto-detected)
- **Disk**: Floppy or HDD with 10KB+ space
- **Display**: VGA text mode (80x25)

## 📚 Learning Resources

- [OSDev Wiki](https://wiki.osdev.org/)
- [Intel x86 Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [Paging Tutorial](https://wiki.osdev.org/Paging)
- [The little book about OS development](https://littleosbook.github.io/)
- [os-tutorial by Carlos Fenollosa](https://github.com/cfenollosa/os-tutorial)

## 🤝 Contributing

This is a solo 365-day challenge project, but suggestions and feedback are welcome! Open issues or discussions.

## 📝 License

MIT License - See LICENSE file

## 👨‍💻 Author

**Subhobhai**  
PCMB student from West Bengal  
Building web apps, experimenting with AI, and exploring game development

## 📊 Development Log

| Date | Milestone | Status |
|------|-----------|--------|
| Nov 11, 2025 | Bootloader, basic kernel | ✅ Complete |
| Nov 11, 2025 | IDT, interrupts, keyboard | ✅ Complete |
| Nov 11, 2025 | Timer, memory detection | ✅ Complete |
| Nov 11, 2025 | PMM, heap allocator | ✅ Complete |
| Nov 11, 2025 | Virtual memory (paging) | ✅ Complete |

## 🎯 Next Steps (Day 6)

- Process Control Blocks (PCB)
- Task switching mechanism
- Simple round-robin scheduler
- Multi-tasking foundation

## 📈 Development Statistics

- **Days Elapsed**: 5 of 365
- **Progress**: ~10%
- **Total Files**: 34
- **Lines of Code**: ~4,000
- **Commits**: 24+
- **Features Implemented**: 19 major systems

---

**Status**: Day 5 of 365 - Virtual memory enabled! 🧠💾🚀

Virtual memory is one of the most complex parts of OS development - congratulations on reaching this milestone!
