# SUB OS

**The World's Smoothest Operating System - Built from Scratch**

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Status](https://img.shields.io/badge/status-alpha-orange.svg)
![Architecture](https://img.shields.io/badge/arch-x86-green.svg)
![Version](https://img.shields.io/badge/version-0.2.0-blue.svg)

## 🚀 Overview

SUB OS is a custom operating system built entirely from scratch without using existing kernels (Linux, Windows, or macOS). The goal is to create the world's smoothest and lag-free operating system within 365 days.

**Current Version:** Alpha v0.2.0  
**Development Started:** November 11, 2025  
**Current Day:** 2 of 365

## ✨ Features

### Currently Implemented (Day 2)
- ✅ Custom bootloader (written in x86 Assembly)
- ✅ 32-bit Protected Mode
- ✅ Global Descriptor Table (GDT)
- ✅ VGA Text Mode Driver with scrolling
- ✅ Interrupt Descriptor Table (IDT)
- ✅ Exception handling (32 CPU exceptions)
- ✅ Hardware interrupt handling (16 IRQs)
- ✅ Programmable Interrupt Controller (PIC) setup
- ✅ PS/2 Keyboard driver
- ✅ Keyboard input buffer
- ✅ Basic kernel in C with modular structure

### Roadmap (365 Days)
- [x] Interrupt Descriptor Table (IDT) ✅
- [x] Keyboard driver ✅
- [ ] Memory management (paging, heap)
- [ ] Timer driver
- [ ] Process management and scheduling
- [ ] File system implementation
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
```

## 📁 Project Structure

```
sub-os/
├── boot/                  # Bootloader code
│   ├── boot.asm          # Main bootloader
│   ├── gdt.asm           # Global Descriptor Table
│   ├── disk_load.asm     # Disk reading functions
│   ├── print_string.asm  # Real mode printing
│   ├── print_string_pm.asm  # Protected mode printing
│   └── switch_to_pm.asm  # Mode switching
├── kernel/                # Kernel source code
│   ├── kernel_entry.asm  # Kernel entry point
│   ├── kernel.c          # Main kernel code
│   ├── kernel.h          # Kernel header
│   ├── idt.asm           # IDT assembly code
│   ├── isr.asm           # Interrupt service routines
│   ├── idt.c             # IDT C implementation
│   ├── idt.h             # IDT header
│   ├── keyboard.c        # Keyboard driver
│   └── keyboard.h        # Keyboard header
├── build/                 # Build output (generated)
├── Makefile               # Build system
├── linker.ld              # Linker script
└── README.md              # This file
```

## 🎯 Design Goals

1. **Performance First**: Minimize context switch overhead and optimize scheduling
2. **Lag-Free Experience**: Advanced caching and predictive algorithms
3. **Clean Architecture**: Well-documented, modular code
4. **Educational**: Serve as a learning resource for OS development

## 🎮 Try It Out!

When you run SUB OS, you'll see the boot sequence and then be able to type with the keyboard! The OS now:
- Handles hardware interrupts
- Responds to keyboard input
- Displays typed characters on screen
- Has proper exception handling

Try typing anything - the keyboard driver will echo your input!

## 📚 Technical Details

### Interrupt Handling
- **IDT**: 256-entry Interrupt Descriptor Table
- **Exceptions**: 32 CPU exception handlers (divide by zero, page fault, etc.)
- **IRQs**: 16 hardware interrupt handlers (timer, keyboard, etc.)
- **PIC**: Remapped 8259 PIC for proper IRQ handling

### Keyboard Driver
- **Input Buffer**: 256-byte circular buffer
- **Scancode Translation**: US QWERTY layout
- **Echo Support**: Characters are displayed as you type
- **Special Keys**: Backspace, tab, and enter supported

## 📚 Learning Resources

- [OSDev Wiki](https://wiki.osdev.org/) - Comprehensive OS development resource
- [The little book about OS development](https://littleosbook.github.io/)
- [os-tutorial by Carlos Fenollosa](https://github.com/cfenollosa/os-tutorial)
- [Intel x86 Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)

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

## 🎯 Next Steps (Day 3)

- Implement timer driver (IRQ0)
- Add memory detection using BIOS E820
- Begin physical memory manager implementation

---

**Status**: Day 2 of 365 - Interrupts and keyboard working! ⌨️🚀
