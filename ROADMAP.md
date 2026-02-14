# SUB OS Development Roadmap

**Goal**: Build a complete operating system from scratch in 365 days

## Phase 1: Foundation and Bootloader (Days 1-45) ⏳

### Week 1-2: Environment and Theory ✅
- [x] Set up development environment
- [x] Create GitHub repository
- [x] Study x86 architecture basics

### Week 3-4: First Bootloader ✅ 
- [x] Write 512-byte bootloader
- [x] Print "SUB OS" message
- [x] Test in QEMU

### Week 5-6: Advanced Bootloader ✅
- [x] Implement disk reading
- [x] Create two-stage bootloader
- [x] Switch to Protected Mode
- [x] Set up GDT
- [x] Enable A20 line

### Week 7: Transition to C ✅
- [x] Create kernel entry in assembly
- [x] Write first C kernel function
- [x] Set up linker script
- [x] Implement VGA text driver (Later replaced by Graphics Mode)

## Phase 2: Core Kernel Development (Days 46-120)

### Week 8-10: Interrupts ✅
- [x] Implement IDT
- [x] Handle CPU exceptions
- [x] Set up PIC
- [x] Keyboard driver (IRQ1)

### Week 11-13: Physical Memory ✅
- [x] Detect memory (E820)
- [x] Physical memory manager
- [x] Page frame allocator

### Week 14-16: Virtual Memory ✅
- [x] Enable paging
- [x] Virtual memory manager
- [x] Heap allocator (malloc/free)
- [ ] Higher-half kernel

### Week 17: Memory Optimization
- [ ] Slab allocator
- [ ] Memory pools

## Phase 3: Process Management (Days 121-180)

### Week 18-19: Processes ✅
- [x] Process Control Block design
- [x] Process creation/termination
- [x] Process states

### Week 20-21: Context Switching ✅
- [x] Register save/restore
- [x] Context switch mechanism
- [x] Timer interrupt
- [x] Round Robin scheduler

### Week 22-23: Advanced Scheduling
- [ ] Priority scheduling
- [ ] Multi-level queues
- [ ] Semaphores and mutexes

### Week 24-26: IPC
- [ ] Message passing
- [ ] Shared memory
- [ ] Pipes
- [ ] Signals

## Phase 4: File System (Days 181-240)

### Week 27-29: Design ✅
- [x] VFS layer (Simple File System)
- [x] Inode structure (SFS entries)
- [x] Directory design

### Week 30-32: Basic Operations ✅
- [x] Create/delete files
- [x] Read/write files
- [ ] Buffer cache

### Week 33-35: Advanced Features
- [ ] Subdirectories
- [ ] Permissions
- [ ] File system checks

## Phase 5: Device Drivers (Days 241-285)

### Week 36-38: Storage ✅
- [x] ATA/IDE driver
- [ ] Block device layer
- [ ] Disk caching

### Week 39-40: Display (In Progress) 🏃
- [x] VGA Mode 13h Driver
- [x] Basic Drawing Primitives
- [x] Double Buffering
- [ ] Window Compositor

### Week 41: Additional Devices
- [ ] Serial port driver (Done for ARM)
- [ ] RTC driver
- [ ] Timer driver (PIT Done)
- [ ] PS/2 Mouse Driver

## Phase 6: System Services (Days 286-330)

### Week 42-44: System Calls ✅
- [x] System call interface
- [x] Core syscalls (read, write, fork, exec - partial)
- [x] User-kernel boundary

### Week 45-46: User Mode ✅
- [x] Ring 0 to Ring 3 transition
- [x] User process memory layout
- [ ] ELF loader

### Week 47: Standard Library
- [ ] Minimal libc
- [ ] String operations (Partial)
- [ ] stdio functions

## Phase 7: Shell and UI (Days 331-365)

### Week 48-49: Command Shell ✅
- [x] Shell design
- [x] Command parser
- [x] Built-in commands (Yeagerist Theme)

### Week 50-51: Utilities
- [ ] File utilities
- [ ] Text editor
- [ ] System tools

### Week 52: Final Polish
- [ ] Comprehensive testing
- [ ] Documentation
- [ ] Demo preparation

## Progress Tracking

- **Current Day**: 11 of 365
- **Current Phase**: Phase 5 - Device Drivers / GUI
- **Completion**: ~46%
- **Status**: Ahead of Schedule (Rapid Prototyping) 🚀

---

*Last Updated: February 14, 2026*
