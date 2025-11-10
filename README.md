# SUB OS

**The World's Smoothest Operating System - Built from Scratch**

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Status](https://img.shields.io/badge/status-alpha-orange.svg)
![Architecture](https://img.shields.io/badge/arch-x86-green.svg)
![Version](https://img.shields.io/badge/version-0.7.0-blue.svg)

## 🚀 Overview

SUB OS is a custom operating system built entirely from scratch without using existing kernels (Linux, Windows, or macOS). The goal is to create the world's smoothest and lag-free operating system within 365 days.

**Current Version:** Alpha v0.7.0  
**Development Started:** November 11, 2025  
**Current Day:** 7 of 365  
**Progress:** ~18% complete

## ✨ Features

### Currently Implemented (Day 7)
- ✅ Custom bootloader (x86 Assembly)
- ✅ 32-bit Protected Mode
- ✅ Global Descriptor Table (GDT)
- ✅ VGA Text Mode Driver with scrolling
- ✅ Interrupt Descriptor Table (IDT)
- ✅ Exception handling (32 CPU exceptions)
- ✅ Hardware interrupt handling (16 IRQs)
- ✅ PS/2 Keyboard driver
- ✅ PIT Timer driver (100 Hz)
- ✅ System uptime tracking
- ✅ Memory detection (BIOS E820)
- ✅ Physical Memory Manager
- ✅ Virtual Memory (Paging)
- ✅ Heap Allocator (kmalloc/kfree)
- ✅ Process Management (PCB)
- ✅ Round-Robin Scheduler
- ✅ Task Switching
- ✅ **System Calls (INT 0x80)** 🆕
- ✅ **User/Kernel Interface** 🆕

### Implemented System Calls
- `sys_exit(status)` - Exit process
- `sys_fork()` - Create child process (stub)
- `sys_read(fd, buf, count)` - Read from file (stub)
- `sys_write(fd, buf, count)` - Write to file (stdout works!)
- `sys_getpid()` - Get process ID
- `sys_sleep(ms)` - Sleep for milliseconds
- `sys_yield()` - Yield CPU to another process

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
- [x] System calls ✅
- [ ] User mode
- [ ] Fork/exec
- [ ] Disk driver
- [ ] File system
- [ ] Shell
- [ ] Multi-core
- [ ] GUI

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
git clone https://github.com/subhobhai943/sub-os.git
cd sub-os
make
make run
```

## 📊 Statistics

- **Days**: 7 of 365 (~2%)
- **Files**: 42
- **Lines**: ~6,000
- **Commits**: 31+
- **Features**: 24 systems

---

**Status**: Day 7 of 365 - System calls working! 📡⚙️🚀

SUB OS now supports system calls - user programs can request kernel services!
