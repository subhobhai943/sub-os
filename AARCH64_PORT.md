# SUB OS — AArch64 Port

This document describes the `port/aarch64` branch which ports **SUB OS** from its original
`x86` (real-mode + protected-mode) design to the **AArch64 (ARM 64-bit)** architecture,
targeting the QEMU `virt` machine with a GIC-v2 interrupt controller.

---

## Directory Structure

```
arch/
└── aarch64/
    ├── boot/
    │   ├── start.S          # EL2→EL1 drop, stack setup, BSS clear, call main()
    │   └── vectors.S        # AArch64 exception vector table (EL1)
    ├── drivers/
    │   ├── uart.c           # PL011 UART @ 0x09000000 (replaces VGA/keyboard)
    │   ├── gic.c            # GIC-400 interrupt controller (C)
    │   ├── gic.S            # GIC low-level enable/disable/EOI (ASM)
    │   ├── timer.c          # ARM Generic Timer (100 Hz, PPI #30)
    │   └── mmu.c            # Stage-1 EL1 MMU, identity-map 0–1 GB
    ├── include/
    │   ├── uart.h
    │   ├── gic.h
    │   ├── timer.h
    │   └── mmu.h
    ├── kernel/
    │   ├── main.c           # C kernel entry point
    │   └── exception.c      # Sync / IRQ / FIQ handlers
    └── linker_aarch64.ld    # Linker script (load @ 0x40080000)
Makefile.aarch64             # AArch64 build rules
AARCH64_PORT.md              # This file
```

---

## Key Architectural Differences from x86

| Aspect | x86 (original) | AArch64 (this port) |
|---|---|---|
| Boot | BIOS MBR → real mode → PM | QEMU loads ELF directly at EL2/EL1 |
| Bootloader | Custom NASM boot.asm | start.S (EL2→EL1 drop) |
| Interrupts | 8259A PIC + IDT | GIC-400 (GICD + GICC) |
| Timer | i8254 PIT via port I/O | ARM Generic Timer (system register) |
| Console | VGA text mode (0xB8000) | PL011 UART (MMIO @ 0x09000000) |
| MMU | x86 paging (CR3, PDE/PTE) | AArch64 VMSAv8-64 (TTBR0_EL1) |
| Page size | 4 KB | 4 KB (2 MB blocks in L2) |
| Exception model | x86 IDT (256 vectors) | AArch64 vector table (VBAR_EL1) |
| Calling convention | cdecl (32-bit) | AAPCS64 |
| Instruction set | NASM x86 | GAS ARM64 |

---

## Building

### Prerequisites

```bash
# Debian / Ubuntu
sudo apt install gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu qemu-system-arm
```

### Build

```bash
make -f Makefile.aarch64
```

This produces:
- `build/aarch64/sub_os_aarch64.elf`  — ELF with debug symbols
- `build/aarch64/sub_os_aarch64.bin`  — raw binary for QEMU

### Run in QEMU

```bash
make -f Makefile.aarch64 run
```

Expected output on the QEMU serial console:

```
SUB OS - AArch64 port starting...
[OK] Exception vectors installed
[OK] GIC-400 initialised
[OK] MMU enabled (identity map 0-1 GB)
[OK] Generic timer initialised at 100 Hz
SUB OS AArch64 boot complete. Entering idle loop.
```

Press `Ctrl-A X` to quit QEMU.

---

## What Is NOT Yet Ported

The following x86-specific subsystems still need work:

- **Process / scheduler** — task switching uses x86 TSS; needs AArch64 context save/restore
- **System calls** — x86 `int 0x80`; needs AArch64 `svc` handler
- **ATA / filesystem** — x86 PIO ATA; QEMU virtio-blk or MMCI for AArch64
- **Heap / memory manager** — logic is portable; just needs re-linking
- **Paging (higher-half)** — only identity map is set up; kernel virtual memory TBD

---

## Testing

```bash
# Build and run unit-style test (boots and prints init messages)
make -f Makefile.aarch64 run

# Inspect ELF sections
aarch64-linux-gnu-objdump -h build/aarch64/sub_os_aarch64.elf

# Disassemble
aarch64-linux-gnu-objdump -d build/aarch64/sub_os_aarch64.elf | less
```

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md). When submitting patches for this port,
prefix commit messages with `feat(aarch64):` or `fix(aarch64):`.
