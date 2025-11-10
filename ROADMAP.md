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

### Week 5-6: Advanced Bootloader (Current)
- [x] Implement disk reading
- [x] Create two-stage bootloader
- [x] Switch to Protected Mode
- [x] Set up GDT
- [ ] Enable A20 line

### Week 7: Transition to C
- [x] Create kernel entry in assembly
- [x] Write first C kernel function
- [x] Set up linker script
- [x] Implement VGA text driver

## Phase 2: Core Kernel Development (Days 46-120)

### Week 8-10: Interrupts
- [ ] Implement IDT
- [ ] Handle CPU exceptions
- [ ] Set up PIC
- [ ] Keyboard driver (IRQ1)

### Week 11-13: Physical Memory
- [ ] Detect memory (E820)
- [ ] Physical memory manager
- [ ] Page frame allocator

### Week 14-16: Virtual Memory
- [ ] Enable paging
- [ ] Virtual memory manager
- [ ] Heap allocator (malloc/free)
- [ ] Higher-half kernel

### Week 17: Memory Optimization
- [ ] Slab allocator
- [ ] Memory pools

## Phase 3: Process Management (Days 121-180)

### Week 18-19: Processes
- [ ] Process Control Block design
- [ ] Process creation/termination
- [ ] Process states

### Week 20-21: Context Switching
- [ ] Register save/restore
- [ ] Context switch mechanism
- [ ] Timer interrupt
- [ ] Round Robin scheduler

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

### Week 27-29: Design
- [ ] VFS layer
- [ ] Inode structure
- [ ] Directory design

### Week 30-32: Basic Operations
- [ ] Create/delete files
- [ ] Read/write files
- [ ] Buffer cache

### Week 33-35: Advanced Features
- [ ] Subdirectories
- [ ] Permissions
- [ ] File system checks

## Phase 5: Device Drivers (Days 241-285)

### Week 36-38: Storage
- [ ] ATA/IDE driver
- [ ] Block device layer
- [ ] Disk caching

### Week 39-40: Display
- [ ] Enhanced VGA driver
- [ ] VESA graphics
- [ ] Framebuffer

### Week 41: Additional Devices
- [ ] Serial port driver
- [ ] RTC driver
- [ ] Timer driver

## Phase 6: System Services (Days 286-330)

### Week 42-44: System Calls
- [ ] System call interface
- [ ] Core syscalls (read, write, fork, exec)
- [ ] User-kernel boundary

### Week 45-46: User Mode
- [ ] Ring 0 to Ring 3 transition
- [ ] User process memory layout
- [ ] ELF loader

### Week 47: Standard Library
- [ ] Minimal libc
- [ ] String operations
- [ ] stdio functions

## Phase 7: Shell and UI (Days 331-365)

### Week 48-49: Command Shell
- [ ] Shell design
- [ ] Command parser
- [ ] Built-in commands

### Week 50-51: Utilities
- [ ] File utilities
- [ ] Text editor
- [ ] System tools

### Week 52: Final Polish
- [ ] Comprehensive testing
- [ ] Documentation
- [ ] Demo preparation

## Progress Tracking

- **Current Day**: 1 of 365
- **Current Phase**: Phase 1 - Foundation
- **Completion**: ~5%
- **Status**: On Track ✅

---

*Last Updated: November 11, 2025*
