# SUB OS

**The World's Smoothest Operating System - Built from Scratch**

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Status](https://img.shields.io/badge/status-alpha-orange.svg)
![Architecture](https://img.shields.io/badge/arch-x86-green.svg)

## 🚀 Overview

SUB OS is a custom operating system built entirely from scratch without using existing kernels (Linux, Windows, or macOS). The goal is to create the world's smoothest and lag-free operating system within 365 days.

**Current Version:** Alpha v0.1.0  
**Development Started:** November 11, 2025

## ✨ Features

### Currently Implemented (Day 1)
- ✅ Custom bootloader (written in x86 Assembly)
- ✅ 32-bit Protected Mode
- ✅ Global Descriptor Table (GDT)
- ✅ VGA Text Mode Driver
- ✅ Basic kernel in C
- ✅ Kernel prints system information

### Roadmap (365 Days)
- [ ] Interrupt Descriptor Table (IDT)
- [ ] Keyboard driver
- [ ] Memory management (paging, heap)
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
├── boot/               # Bootloader code
│   ├── boot.asm       # Main bootloader
│   ├── gdt.asm        # Global Descriptor Table
│   ├── disk_load.asm  # Disk reading functions
│   └── *.asm          # Other bootloader utilities
├── kernel/            # Kernel source code
│   ├── kernel_entry.asm  # Kernel entry point
│   └── kernel.c       # Main kernel code
├── build/             # Build output (generated)
├── Makefile           # Build system
├── linker.ld          # Linker script
└── README.md          # This file
```

## 🎯 Design Goals

1. **Performance First**: Minimize context switch overhead and optimize scheduling
2. **Lag-Free Experience**: Advanced caching and predictive algorithms
3. **Clean Architecture**: Well-documented, modular code
4. **Educational**: Serve as a learning resource for OS development

## 📚 Learning Resources

- [OSDev Wiki](https://wiki.osdev.org/) - Comprehensive OS development resource
- [The little book about OS development](https://littleosbook.github.io/)
- [os-tutorial by Carlos Fenollosa](https://github.com/cfenollosa/os-tutorial)

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

---

**Status**: Day 1 of 365 - The journey begins! 🚀
