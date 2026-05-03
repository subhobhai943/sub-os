/* SUB OS - AArch64 MMU Driver
 *
 * Maps 2 GB identity (VA == PA):
 *   0x00000000 - 0x40000000  1 GB  via l2_low  (Normal WB)
 *   0x40000000 - 0x80000000  1 GB  via l2_high (Normal WB)
 *     -> covers kernel load address 0x40080000
 *
 * Device override (GIC @ 0x08xxxxxx, UART @ 0x09xxxxxx):
 *   L2 slots 64-76 remapped as Device nGnRnE
 */

#include "../include/mmu.h"

#define PAGE_SIZE   4096UL

/* Descriptor bits */
#define DESC_BLOCK  1UL          /* L1/L2 block: bits[1:0] = 0b01 */
#define DESC_TABLE  3UL          /* Table pointer: bits[1:0] = 0b11 */

#define AF            (1UL << 10)
#define SH_INNER      (3UL << 8)
#define SH_OUTER      (2UL << 8)
#define AP_RW_EL1     (0UL << 6)
#define ATTRINDX(n)   ((unsigned long)(n) << 2)

#define MAIR_DEVICE   0
#define MAIR_NORMAL   1

/* 3 page tables, each 4 KB */
__attribute__((aligned(PAGE_SIZE))) static unsigned long l0[512];
__attribute__((aligned(PAGE_SIZE))) static unsigned long l1[512];
__attribute__((aligned(PAGE_SIZE))) static unsigned long l2_low[512];   /* 0x00000000 - 0x3FFFFFFF */
__attribute__((aligned(PAGE_SIZE))) static unsigned long l2_high[512];  /* 0x40000000 - 0x7FFFFFFF */

static void fill_l2_normal(unsigned long *tbl, unsigned long base_pa) {
    unsigned long i;
    for (i = 0; i < 512; i++) {
        unsigned long pa = base_pa + (i << 21);  /* 2 MB steps */
        tbl[i] = pa
            | DESC_BLOCK
            | AF
            | SH_INNER
            | AP_RW_EL1
            | ATTRINDX(MAIR_NORMAL);
    }
}

void mmu_init(void) {
    unsigned long i, mair, tcr, sctlr;

    /* 1. MAIR_EL1 */
    mair = (0x00UL << (MAIR_DEVICE * 8))
         | (0xFFUL << (MAIR_NORMAL  * 8));
    __asm__ volatile("msr mair_el1, %0" :: "r"(mair));
    __asm__ volatile("isb" ::: "memory");

    /* 2. Zero all tables */
    for (i = 0; i < 512; i++) l0[i] = l1[i] = l2_low[i] = l2_high[i] = 0;

    /* 3. Wire up table chain */
    l0[0] = (unsigned long)l1    | DESC_TABLE;
    l1[0] = (unsigned long)l2_low  | DESC_TABLE;  /* 0x00000000 - 0x3FFFFFFF */
    l1[1] = (unsigned long)l2_high | DESC_TABLE;  /* 0x40000000 - 0x7FFFFFFF */

    /* 4. Fill l2_low: Normal WB for 0x00000000 - 0x3FFFFFFF */
    fill_l2_normal(l2_low, 0x00000000UL);

    /* 5. Fill l2_high: Normal WB for 0x40000000 - 0x7FFFFFFF
     *    This covers the kernel at 0x40080000 */
    fill_l2_normal(l2_high, 0x40000000UL);

    /* 6. Override GIC/UART range in l2_low as Device nGnRnE
     *    0x08000000 = slot 64, 0x09FFFFFF ends at slot 76 */
    for (i = 64; i <= 76; i++) {
        unsigned long pa = i << 21;
        l2_low[i] = pa
            | DESC_BLOCK
            | AF
            | SH_OUTER
            | AP_RW_EL1
            | ATTRINDX(MAIR_DEVICE);
    }

    /* 7. Barrier before loading TTBR */
    __asm__ volatile("dsb sy" ::: "memory");

    /* 8. TCR_EL1
     *    T0SZ = 31  -> VA space = 2^(64-31) = 2 GB  (covers 0x00000000-0x7FFFFFFF)
     *    IRGN0/ORGN0 = 01 (Normal WB RA WA)
     *    SH0 = 11 (Inner Shareable)
     *    TG0 = 00 (4 KB)
     *    IPS = 001 (36-bit PA)
     */
    tcr =  (31UL << 0)     /* T0SZ        */
         | (1UL  << 8)     /* IRGN0 = 01  */
         | (1UL  << 10)    /* ORGN0 = 01  */
         | (3UL  << 12)    /* SH0   = 11  */
         | (0UL  << 14)    /* TG0   = 00  */
         | (31UL << 16)    /* T1SZ        */
         | (3UL  << 28)    /* SH1   = 11  */
         | (2UL  << 30)    /* TG1   = 10  */
         | (1UL  << 32);   /* IPS   = 001 */
    __asm__ volatile("msr tcr_el1, %0" :: "r"(tcr));
    __asm__ volatile("isb" ::: "memory");

    /* 9. Load TTBR0_EL1 */
    __asm__ volatile("msr ttbr0_el1, %0" :: "r"((unsigned long)l0));
    __asm__ volatile("isb" ::: "memory");

    /* 10. Invalidate TLBs */
    __asm__ volatile("tlbi vmalle1\n\t"
                     "dsb  sy\n\t"
                     "isb" ::: "memory");

    /* 11. Enable MMU + caches in SCTLR_EL1 */
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    sctlr |= (1UL << 0);   /* M */
    sctlr |= (1UL << 2);   /* C */
    sctlr |= (1UL << 12);  /* I */
    __asm__ volatile(
        "msr sctlr_el1, %0\n\t"
        "isb\n\t"
        :: "r"(sctlr) : "memory"
    );
    /* If we reach here the MMU is on and the kernel text is mapped */
}
