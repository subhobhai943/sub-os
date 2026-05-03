/* SUB OS - AArch64 MMU Driver
 * Currently disabled for raw binary boot mode.
 * Will be re-enabled once an ELF/stub loader provides
 * a reliable physical address for page table arrays.
 *
 * To enable: compile with -DENABLE_MMU
 */

#ifdef ENABLE_MMU

#include "../include/mmu.h"

#define PAGE_SIZE   4096UL
#define DESC_BLOCK  1UL
#define DESC_TABLE  3UL
#define AF            (1UL << 10)
#define SH_INNER      (3UL << 8)
#define SH_OUTER      (2UL << 8)
#define AP_RW_EL1     (0UL << 6)
#define ATTRINDX(n)   ((unsigned long)(n) << 2)
#define MAIR_DEVICE   0
#define MAIR_NORMAL   1

__attribute__((aligned(PAGE_SIZE))) static unsigned long l0[512];
__attribute__((aligned(PAGE_SIZE))) static unsigned long l1[512];
__attribute__((aligned(PAGE_SIZE))) static unsigned long l2_low[512];
__attribute__((aligned(PAGE_SIZE))) static unsigned long l2_high[512];

static void fill_l2_normal(unsigned long *tbl, unsigned long base_pa) {
    unsigned long i;
    for (i = 0; i < 512; i++) {
        unsigned long pa = base_pa + (i << 21);
        tbl[i] = pa | DESC_BLOCK | AF | SH_INNER | AP_RW_EL1 | ATTRINDX(MAIR_NORMAL);
    }
}

void mmu_init(void) {
    unsigned long i, mair, tcr, sctlr;

    mair = (0x00UL << (MAIR_DEVICE * 8)) | (0xFFUL << (MAIR_NORMAL * 8));
    __asm__ volatile("msr mair_el1, %0" :: "r"(mair));
    __asm__ volatile("isb" ::: "memory");

    for (i = 0; i < 512; i++) l0[i] = l1[i] = l2_low[i] = l2_high[i] = 0;

    l0[0] = (unsigned long)l1      | DESC_TABLE;
    l1[0] = (unsigned long)l2_low  | DESC_TABLE;
    l1[1] = (unsigned long)l2_high | DESC_TABLE;

    fill_l2_normal(l2_low,  0x00000000UL);
    fill_l2_normal(l2_high, 0x40000000UL);

    /* Device: GIC 0x08000000 (slot 64) .. 0x09FFFFFF (slot 76) */
    for (i = 64; i <= 76; i++) {
        unsigned long pa = i << 21;
        l2_low[i] = pa | DESC_BLOCK | AF | SH_OUTER | AP_RW_EL1 | ATTRINDX(MAIR_DEVICE);
    }

    __asm__ volatile("dsb sy" ::: "memory");

    tcr =  (31UL << 0) | (1UL << 8) | (1UL << 10) | (3UL << 12) | (0UL << 14)
         | (31UL << 16) | (3UL << 28) | (2UL << 30) | (1UL << 32);
    __asm__ volatile("msr tcr_el1, %0" :: "r"(tcr));
    __asm__ volatile("isb" ::: "memory");

    __asm__ volatile("msr ttbr0_el1, %0" :: "r"((unsigned long)l0));
    __asm__ volatile("isb" ::: "memory");

    __asm__ volatile("tlbi vmalle1\n\tdsb sy\n\tisb" ::: "memory");

    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    sctlr |= (1UL << 0) | (1UL << 2) | (1UL << 12);
    __asm__ volatile("msr sctlr_el1, %0\n\tisb" :: "r"(sctlr) : "memory");
}

#endif /* ENABLE_MMU */
