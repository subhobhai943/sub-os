/* SUB OS - AArch64 MMU Driver (fixed)
 * Stage-1 EL1 translation, TTBR0_EL1.
 * Maps:
 *   0x00000000 - 0x40000000 (1 GB) : Normal WB (RAM)
 *   0x08000000 - 0x09010000        : Device nGnRnE (GIC + UART MMIO)
 *                                    overlaid inside the 1 GB Normal map
 *                                    via a separate L2 Device block
 */

#include "../include/mmu.h"

#define PAGE_SIZE     4096UL
#define TABLE_SIZE    (512 * 8)   /* 512 x 8-byte entries = 4 KB */

/* ---- Descriptor bit definitions ---- */
/* Bits [1:0] */
#define DESC_INVALID  0UL
#define DESC_BLOCK    1UL   /* L1/L2 block: bits[1:0] = 0b01 */
#define DESC_TABLE    3UL   /* L0/L1/L2 table: bits[1:0] = 0b11 */
#define DESC_PAGE     3UL   /* L3 page: bits[1:0] = 0b11 */

/* Upper / lower attributes */
#define AF            (1UL << 10)           /* Access Flag - must be set */
#define SH_INNER      (3UL << 8)            /* Inner Shareable */
#define SH_OUTER      (2UL << 8)            /* Outer Shareable */
#define AP_RW_EL1     (0UL << 6)            /* EL1 R/W, EL0 no access */
#define ATTRINDX(n)   ((unsigned long)(n) << 2)

/* MAIR indices */
#define MAIR_IDX_DEVICE  0   /* Device nGnRnE = 0x00 */
#define MAIR_IDX_NORMAL  1   /* Normal Inner/Outer WB RA WA = 0xFF */

/* ---- Page tables (must be 4 KB aligned) ---- */
__attribute__((aligned(PAGE_SIZE))) static unsigned long l0[512];
__attribute__((aligned(PAGE_SIZE))) static unsigned long l1[512];
__attribute__((aligned(PAGE_SIZE))) static unsigned long l2_ram[512];    /* 0x00000000 - 0x40000000 */

void mmu_init(void) {
    unsigned long i;
    unsigned long mair, tcr, sctlr;

    /* --------------------------------------------------
     * 1. Program MAIR_EL1
     *    Index 0 = Device nGnRnE  (0x00)
     *    Index 1 = Normal WB/WA   (0xFF)
     * -------------------------------------------------- */
    mair = (0x00UL << (MAIR_IDX_DEVICE * 8))
         | (0xFFUL << (MAIR_IDX_NORMAL  * 8));
    __asm__ volatile("msr mair_el1, %0\n\t" :: "r"(mair));
    __asm__ volatile("isb" ::: "memory");

    /* --------------------------------------------------
     * 2. Build translation tables
     *    L0[0] -> L1  (covers VA 0x0000_0000_0000_0000 ..)
     *    L1[0] -> L2_RAM (covers 0x00000000 - 0x40000000)
     *    L2_RAM[i] -> 2 MB Normal blocks for all 512 slots
     *    Override slots covering GIC/UART as Device blocks
     * -------------------------------------------------- */

    /* Zero all tables first */
    for (i = 0; i < 512; i++) { l0[i] = 0; l1[i] = 0; l2_ram[i] = 0; }

    /* L0[0] -> L1 table */
    l0[0] = ((unsigned long)l1) | DESC_TABLE;

    /* L1[0] -> L2_RAM table (0 - 1 GB) */
    l1[0] = ((unsigned long)l2_ram) | DESC_TABLE;

    /* L2_RAM: 512 x 2 MB Normal WB blocks */
    for (i = 0; i < 512; i++) {
        unsigned long pa = i << 21;   /* 2 MB per slot */
        l2_ram[i] = pa
            | DESC_BLOCK
            | AF
            | SH_INNER
            | AP_RW_EL1
            | ATTRINDX(MAIR_IDX_NORMAL);
    }

    /* Override slots 0x08000000-0x09FFFFFF (slots 4..4) as Device.
     * Slot index = PA >> 21.  GIC=0x08000000 (slot 64), UART=0x09000000 (slot 72) */
    for (i = 64; i <= 76; i++) {
        unsigned long pa = i << 21;
        l2_ram[i] = pa
            | DESC_BLOCK
            | AF
            | SH_OUTER
            | AP_RW_EL1
            | ATTRINDX(MAIR_IDX_DEVICE);
    }

    /* DSB to ensure table writes are visible before loading TTBR */
    __asm__ volatile("dsb sy" ::: "memory");

    /* --------------------------------------------------
     * 3. TCR_EL1
     *    T0SZ=32 (32-bit VA space, covers 4 GB with 3-level walk)
     *    IRGN0=01 (Normal WB RA WA Inner)
     *    ORGN0=01 (Normal WB RA WA Outer)
     *    SH0=11   (Inner Shareable)
     *    TG0=00   (4 KB granule)
     *    T1SZ=32, TG1=10 (4 KB), SH1=11
     *    IPS=001  (36-bit PA)
     * -------------------------------------------------- */
    tcr =  (32UL << 0)     /* T0SZ  [5:0]  */
         | (1UL  << 8)     /* IRGN0 [9:8]  = 0b01 WB RA WA */
         | (1UL  << 10)    /* ORGN0 [11:10]= 0b01 WB RA WA */
         | (3UL  << 12)    /* SH0   [13:12]= 0b11 Inner */
         | (0UL  << 14)    /* TG0   [15:14]= 0b00 4 KB */
         | (32UL << 16)    /* T1SZ  [21:16]*/
         | (3UL  << 28)    /* SH1   [29:28]= 0b11 Inner */
         | (2UL  << 30)    /* TG1   [31:30]= 0b10 4 KB */
         | (1UL  << 32);   /* IPS   [34:32]= 0b001 36-bit PA */
    __asm__ volatile("msr tcr_el1, %0\n\t" :: "r"(tcr));
    __asm__ volatile("isb" ::: "memory");

    /* --------------------------------------------------
     * 4. Load TTBR0_EL1 (ASID=0, table base)
     * -------------------------------------------------- */
    __asm__ volatile("msr ttbr0_el1, %0\n\t" :: "r"((unsigned long)l0));
    __asm__ volatile("isb" ::: "memory");

    /* Invalidate all TLBs before enabling MMU */
    __asm__ volatile("tlbi vmalle1\n\t"
                     "dsb  sy\n\t"
                     "isb" ::: "memory");

    /* --------------------------------------------------
     * 5. Enable MMU + D-cache + I-cache in SCTLR_EL1
     * -------------------------------------------------- */
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    sctlr |= (1UL << 0);   /* M: MMU */
    sctlr |= (1UL << 2);   /* C: D-cache */
    sctlr |= (1UL << 12);  /* I: I-cache */
    __asm__ volatile("msr sctlr_el1, %0\n\t"
                     "isb" :: "r"(sctlr) : "memory");
}
