# SUB OS

**The World's Smoothest Operating System - Built from Scratch**

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Status](https://img.shields.io/badge/status-alpha-orange.svg)
![Architecture](https://img.shields.io/badge/arch-x86-green.svg)
![Version](https://img.shields.io/badge/version-0.3.0-blue.svg)

## 🚀 Overview

SUB OS is a custom operating system built entirely from scratch without using existing kernels (Linux, Windows, or macOS). The goal is to create the world's smoothest and lag-free operating system within 365 days.

**Current Version:** Alpha v0.3.0  
**Development Started:** November 11, 2025  
**Current Day:** 3 of 365  
**Progress:** ~7% complete

## ✨ Features

### Currently Implemented (Day 3)
- ✅ Custom bootloader (written in x86 Assembly)
- ✅ 32-bit Protected Mode
- ✅ Global Descriptor Table (GDT)
- ✅ VGA Text Mode Driver with scrolling
- ✅ Interrupt Descriptor Table (IDT)
- ✅ Exception handling (32 CPU exceptions)
- ✅ Hardware interrupt handling (16 IRQs)
- ✅ Programmable Interrupt Controller (PIC) setup
- ✅ PS/2 Keyboard driver with buffer
- ✅ **PIT Timer driver (100 Hz)**
- ✅ **System uptime tracking**
- ✅ **Memory detection (BIOS E820)**
- ✅ **Memory map parsing and display**
- ✅ Modular kernel architecture

### Roadmap (365 Days)
- [x] Bootloader ✅
- [x] Protected Mode ✅
- [x] Interrupt Descriptor Table (IDT) ✅
- [x] Keyboard driver ✅
- [x] Timer driver ✅
- [x] Memory detection ✅
- [ ] Physical memory manager
- [ ] Virtual memory (paging)
- [ ] Heap allocator
- [ ] Process management
- [ ] Scheduler
- [ ] File system
- [ ] Device drivers (disk, display)
- [ ] System calls
- [ ] User mode
- [ ] Command shell
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
│   ├── disk_load.asm         # Disk reading functions
│   ├── memory_detect.asm     # BIOS E820 memory detection
│   ├── print_string.asm      # Real mode printing
│   ├── print_string_pm.asm   # Protected mode printing
│   └── switch_to_pm.asm      # Mode switching
├── kernel/                    # Kernel source code
│   ├── kernel_entry.asm      # Kernel entry point
│   ├── kernel.c              # Main kernel code
│   ├── kernel.h              # Kernel header
│   ├── idt.asm               # IDT assembly code
│   ├── isr.asm               # Interrupt service routines
│   ├── idt.c                 # IDT C implementation
│   ├── idt.h                 # IDT header
│   ├── keyboard.c            # Keyboard driver
│   ├── keyboard.h            # Keyboard header
│   ├── timer.c               # Timer driver
│   ├── timer.h               # Timer header
│   ├── memory.c              # Memory manager
│   └── memory.h              # Memory header
├── build/                     # Build output (generated)
├── Makefile                   # Build system
├── linker.ld                  # Linker script
└── README.md                  # This file
```

## 🎯 Design Goals

1. **Performance First**: Minimize context switch overhead and optimize scheduling
2. **Lag-Free Experience**: Advanced caching and predictive algorithms
3. **Clean Architecture**: Well-documented, modular code
4. **Educational**: Serve as a learning resource for OS development

## 🎮 Try It Out!

When you run SUB OS, you'll see:

1. **Boot Sequence**: Bootloader initializes and loads kernel
2. **Memory Detection**: BIOS E820 memory map displayed
3. **System Information**: Hardware details and available memory
4. **Live Uptime Counter**: Updates every second
5. **Interactive Shell**: Type and see your input echoed

### What You Can Do:
- **Type** and see characters appear in real-time
- **Watch** the uptime counter increment
- **See** the memory map of your system
- **Experience** a real OS running bare metal!

## 📚 Technical Details

### Boot Process
1. BIOS loads bootloader into memory at 0x7c00
2. Bootloader detects memory using INT 0x15, EAX=0xE820
3. Bootloader loads kernel from disk sectors 2-21
4. Bootloader sets up GDT and switches to Protected Mode
5. Bootloader jumps to kernel at 0x1000

### Memory Detection
- Uses BIOS E820 function to query memory map
- Stores map at physical address 0x5000
- Identifies usable, reserved, and ACPI memory regions
- Displays detailed memory map on boot

### Timer System
- PIT (Programmable Interval Timer) configured to 100 Hz
- IRQ0 generates timer interrupts 100 times per second
- System tracks ticks for uptime and timing functions
- Supports sleep_ms() for delays

### Interrupt Handling
- **IDT**: 256-entry Interrupt Descriptor Table
- **Exceptions**: 32 CPU exception handlers
- **IRQs**: 16 hardware interrupt handlers
  - IRQ0: Timer (PIT)
  - IRQ1: Keyboard
  - IRQ2-15: Reserved for future devices
- **PIC**: 8259 PIC remapped to avoid conflicts

### Keyboard Driver
- **PS/2 Protocol**: Reads from port 0x60
- **Input Buffer**: 256-byte circular buffer
- **Scancode Translation**: US QWERTY layout
- **Special Keys**: Backspace, tab, enter, space

## 📊 System Requirements

- **CPU**: x86 (32-bit) or x86-64
- **RAM**: Minimum 1MB (detected automatically)
- **Disk**: Floppy or hard drive with at least 10KB
- **Display**: VGA-compatible text mode (80x25)

## 📚 Learning Resources

- [OSDev Wiki](https://wiki.osdev.org/) - Comprehensive OS development resource
- [The little book about OS development](https://littleosbook.github.io/)
- [os-tutorial by Carlos Fenollosa](https://github.com/cfenollosa/os-tutorial)
- [Intel x86 Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [BIOS E820 Specification](https://wiki.osdev.org/Detecting_Memory_(x86))

## 🤝 Contributing

This is currently a solo project for the 365-day challenge, but suggestions and feedback are welcome! Feel free to open issues or discussions.

## 📝 License

MIT License - See LICENSE file for details

## 👨‍💻 Author

**Subhobhai**  
PCMB student from West Bengal  
Building web apps, experimenting with AI, and exploring game development

## 📊 Development Log

| Date | Milestone | Status |
|------|-----------|--------|
| Nov 11, 2025 | Project initialization, bootloader, basic kernel | ✅ Complete |
| Nov 11, 2025 | IDT, exception handling, keyboard driver | ✅ Complete |
| Nov 11, 2025 | Timer driver, memory detection, uptime tracking | ✅ Complete |

## 🎯 Next Steps (Day 4)

- Physical memory manager with bitmap allocator
- Page frame allocation (4KB pages)
- Memory allocation/deallocation functions
- Preparation for virtual memory (paging)

## 📊 Development Statistics

- **Days Elapsed**: 3 of 365
- **Progress**: ~7%
- **Total Files**: 28
- **Lines of Code**: ~2,100
- **Commits**: 10+
- **Features Implemented**: 14

---

**Status**: Day 3 of 365 - Timer and memory systems online! ⏰💾🚀
