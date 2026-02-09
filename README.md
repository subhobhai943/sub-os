# SUB OS

**The World's Smoothest Operating System - Built from Scratch**

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Status](https://img.shields.io/badge/status-alpha-orange.svg)
![Architecture](https://img.shields.io/badge/arch-x86%20%7C%20arm-green.svg)
![Version](https://img.shields.io/badge/version-0.9.0-blue.svg)
![Build](https://github.com/subhobhai943/sub-os/actions/workflows/build.yml/badge.svg)
![Test](https://github.com/subhobhai943/sub-os/actions/workflows/test.yml/badge.svg)

## 🚀 Overview

SUB OS is a custom operating system built entirely from scratch in just 9 days! Now features disk I/O, user mode execution, multitasking, virtual memory, and system calls. **Now being ported to ARMv7 32-bit architecture!**

**Current Version:** Alpha v0.9.0  
**Development Started:** November 11, 2025  
**Current Day:** 9 of 365  
**Progress:** ~22% complete  
**Status:** All core systems + disk I/O working! ✅

## ✨ Features

### Fully Implemented (Day 1-9)

**Boot & Protection:**
- ✅ Custom bootloader
- ✅ Protected Mode (32-bit)
- ✅ GDT with Ring 0/3
- ✅ IDT (256 gates)

**Drivers:**
- ✅ VGA Text Mode (80x25)
- ✅ PS/2 Keyboard (buffered)
- ✅ PIT Timer (100 Hz)
- ✅ **ATA/IDE Disk Driver (PIO mode)** 🆕

**Memory Management:**
- ✅ E820 detection
- ✅ Physical Memory Manager
- ✅ Virtual Memory (paging)
- ✅ Heap allocator

**Process Management:**
- ✅ PCB
- ✅ Round-robin scheduler
- ✅ Task switching
- ✅ Kernel & user processes

**System Interface:**
- ✅ System calls (INT 0x80)
- ✅ User mode (Ring 3)
- ✅ TSS

**Storage:**
- ✅ ATA device detection
- ✅ Sector read/write
- ✅ LBA28 addressing (128GB)
- ✅ Up to 4 drives support

## 🔨 CI/CD Pipeline

SUB OS uses GitHub Actions for continuous integration:

- **Build Workflow**: Automatically compiles the OS on every push
- **Test Workflow**: Validates compilation, binary sizes, and boot tests
- **QEMU Testing**: Boots OS in emulator to verify functionality
- **Artifacts**: Successfully built OS images available for download

### Build Status

Click the badges above to see latest build results. The CI pipeline:
1. ✅ Compiles bootloader (NASM)
2. ✅ Compiles all kernel modules (GCC)
3. ✅ Links kernel binary
4. ✅ Creates final OS image
5. ✅ Validates bootloader size ≤ 512 bytes
6. ✅ Boot tests in QEMU
7. ✅ Uploads build artifacts

## 🛠️ Building

### Requirements
- **OS**: Ubuntu 20.04+ (or compatible Linux)
- **Compiler**: GCC with 32-bit support
- **Assembler**: NASM
- **Emulator**: QEMU (for testing)

### Build Commands

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential nasm qemu-system-x86 qemu-system-arm gcc-arm-none-eabi

# Clone and build (x86)
git clone https://github.com/subhobhai943/sub-os.git
cd sub-os
make

# Build for ARM
make TARGET_ARCH=arm CC=arm-none-eabi-gcc LD=arm-none-eabi-ld ASM=arm-none-eabi-as

# Run in QEMU (128MB RAM)
make run

# Run in QEMU (ARM)
make run TARGET_ARCH=arm

# Debug mode
make debug

# Clean build
make clean
```

## 📊 Statistics

- **Development Time**: 9 days
- **Total Files**: 54
- **Lines of Code**: ~7,500
- **Commits**: 42+
- **Major Systems**: 27
- **Phase 2 Progress**: 10%
- **CI/CD**: Automated testing enabled

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
| 9 | ATA/IDE Disk Driver | ✅ |
| - | CI/CD Pipeline | ✅ |

## 🎯 What's Next (Day 10+)

- Simple File System (FAT12 or custom)
- VFS layer
- File operations (open, read, write, close)
- Directory management
- Simple shell

## 📝 License

MIT License

## 👨‍💻 Author

**Subhobhai**  
PCMB student from West Bengal

---

**Status**: Day 9 Complete - Disk I/O + CI/CD Working! 💾⚙️🚀

**Achievement**: Built a complete OS with disk I/O and automated testing in 9 days!
