# SUB OS

**The World's Smoothest Operating System - Built from Scratch**

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Status](https://img.shields.io/badge/status-alpha-orange.svg)
![Architecture](https://img.shields.io/badge/arch-x86-green.svg)
![Version](https://img.shields.io/badge/version-0.8.0-blue.svg)

## 🚀 Overview

SUB OS is a custom operating system built entirely from scratch without using existing kernels. Built in just **8 days**, it now supports user mode execution, multitasking, and system calls!

**Current Version:** Alpha v0.8.0  
**Development Started:** November 11, 2025  
**Current Day:** 8 of 365  
**Progress:** ~20% complete

## ✨ Features

### Currently Implemented (Day 8)
- ✅ Custom bootloader (x86 Assembly)
- ✅ 32-bit Protected Mode
- ✅ Global Descriptor Table (GDT) with Ring 0/3
- ✅ VGA Text Mode Driver
- ✅ Interrupt Descriptor Table (IDT)
- ✅ Exception handling (32 exceptions)
- ✅ Hardware interrupts (16 IRQs)
- ✅ PS/2 Keyboard driver
- ✅ PIT Timer driver (100 Hz)
- ✅ Memory detection (BIOS E820)
- ✅ Physical Memory Manager
- ✅ Virtual Memory (Paging)
- ✅ Heap Allocator (kmalloc/kfree)
- ✅ Process Management (PCB)
- ✅ Round-Robin Scheduler
- ✅ Task Switching
- ✅ System Calls (INT 0x80)
- ✅ **User Mode (Ring 3)** 🆕
- ✅ **Task State Segment (TSS)** 🆕
- ✅ **Privilege Transitions** 🆕

## 📚 Technical Architecture

### Privilege Levels
- **Ring 0 (Kernel Mode)**: Full hardware access, kernel code
- **Ring 3 (User Mode)**: Restricted access, application code
- **Syscalls**: Controlled transition Ring 3 → Ring 0

### System Calls
```c
// From user mode (Ring 3):
int pid;
asm("mov $6, %%eax; int $0x80" : "=a"(pid));  // getpid()

const char* msg = "Hello!";
asm("mov $3, %%eax; mov $1, %%ebx; int $0x80");  // write()
```

## 🛠️ Building SUB OS

```bash
git clone https://github.com/subhobhai943/sub-os.git
cd sub-os
make
make run
```

## 📊 Statistics

- **Days**: 8 of 365 (~2.2%)
- **Files**: 48
- **Lines**: ~6,600
- **Commits**: 35+
- **Features**: 26 systems
- **Phase 1**: ~50% complete

## 🏆 Major Milestones

✅ Bootloader (Day 1-2)  
✅ Interrupts & Drivers (Day 3)  
✅ Memory Management (Day 4-5)  
✅ Multitasking (Day 6)  
✅ System Calls (Day 7)  
✅ **User Mode** (Day 8) ← Current

## 🎯 Next Steps

- ELF Loader
- Fork/Exec
- Disk Driver (ATA/IDE)
- File System (FAT or custom)
- Simple Shell

---

**Status**: Day 8 of 365 - User mode working! 🔐⚙️🚀

Built in 8 days what takes most projects 6+ months!
