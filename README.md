# SUB OS

**The World's Smoothest Operating System - Built from Scratch**

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Status](https://img.shields.io/badge/status-alpha-orange.svg)
![Architecture](https://img.shields.io/badge/arch-x86-green.svg)
![Version](https://img.shields.io/badge/version-0.9.0-blue.svg)

## 🚀 Overview

SUB OS is a custom operating system built entirely from scratch in just 9 days! Now features disk I/O, user mode execution, multitasking, virtual memory, and system calls.

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

## 🛠️ Building

```bash
git clone https://github.com/subhobhai943/sub-os.git
cd sub-os
make
make run
```

## 📊 Statistics

- **Development Time**: 9 days
- **Total Files**: 52
- **Lines of Code**: ~7,500
- **Commits**: 40+
- **Major Systems**: 27
- **Phase 2 Progress**: 10%

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

## 🎯 What's Next (Day 10+)

- Simple File System (FAT12 or custom)
- VFS layer
- File operations (open, read, write, close)
- Directory management
- Simple shell

---

**Status**: Day 9 Complete - Disk I/O Working! 💾⚙️🚀

**Achievement**: Built a complete OS with disk I/O in 9 days!
