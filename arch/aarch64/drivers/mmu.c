/* SUB OS - AArch64 MMU Driver
 * Configures a minimal 4-level (L0-L3) identity-map + kernel map
 * using 4KB pages and 512 GB L0 granularity.
 * Only maps the first 1 GB for simplicity (QEMU virt RAM base).
 */

#include "../include/mmu.h"

/* Page table sizes / counts */
#define PAGE_SIZE       4096UL
#define TABLE_ENTRIES   512

/* Descriptor attribute bits (Stage 1, EL1) */
#define PTE_VALID       (1UL << 0)
#define PTE_TABLE       (1UL << 1)      /* used for table descriptors */
#define PTE_PAGE        (1UL << 1)      /* used for level-3 page descriptors */
#define PTE_AF          (1UL << 10)     /* Access Flag */
#define PTE_SH_INNER    (3UL << 8)      /* Inner Shareable */
#define PTE_AP_RW_EL1   (0UL << 6)     /* EL1 RW, EL0 none */
#define PTE_ATTRINDX(n) ((unsigned long)(n) << 2)

/* MAIR indices */
#define MAIR_DEVICE_nGnRnE  0
#define MAIR_NORMAL_WBWA    1

/* Align page tables to 4 KB */
__attribute__((aligned(PAGE_SIZE)))
static unsigned long l0_table[TABLE_ENTRIES];

__attribute__((aligned(PAGE_SIZE)))
static unsigned long l1_table[TABLE_ENTRIES];

__attribute__((aligned(PAGE_SIZE)))
static unsigned long l2_table[TABLE_ENTRIES];

/* Map entire 1 GB (512 x 2MB blocks) as Normal WB */
/* We use L2 block descriptors (2 MB) for speed */
void mmu_init(void) {
    unsigned long i;
    unsigned long mair;
    unsigned long tcr;
    unsigned long sctlr;

    /* ---------------------------------------------------
     * 1. Set MAIR_EL1
     *    Index 0 = Device nGnRnE
     *    Index 1 = Normal Inner/Outer WB RA WA
     * --------------------------------------------------- */
    mair = (0x00UL << (MAIR_DEVICE_nGnRnE * 8)) |
           (0xFFUL << (MAIR_NORMAL_WBWA   * 8));
    asm volatile("msr mair_el1, %0" :: "r"(mair));

    /* ---------------------------------------------------
     * 2. Build page tables (identity map 0x00000000 - 0x40000000)
     * --------------------------------------------------- */
    /* L0[0] -> L1 table */
    l0_table[0] = ((unsigned long)l1_table) | PTE_VALID | PTE_TABLE;

    /* L1[0] -> L2 table (covers 0-1 GB) */
    l1_table[0] = ((unsigned long)l2_table) | PTE_VALID | PTE_TABLE;

    /* L2: 512 x 2 MB blocks = 1 GB, Normal WB */
    for (i = 0; i < 512; i++) {
        unsigned long pa = i << 21;  /* 2 MB steps */
        l2_table[i] = pa
            | PTE_VALID
            | (1UL)           /* block descriptor: bit[1]=1, bit[0]=1 -> 0b11 */
            | PTE_AF
            | PTE_SH_INNER
            | PTE_AP_RW_EL1
            | PTE_ATTRINDX(MAIR_NORMAL_WBWA);
    }

    /* ---------------------------------------------------
     * 3. Set TCR_EL1
     * --------------------------------------------------- */
    tcr = (16UL << 0)    /* T0SZ = 16 → 48-bit VA */
        | (0UL  << 6)    /* IRGN0 = Normal WB RA WA */
        | (1UL  << 8)    /* ORGN0 = Normal WB RA WA */
        | (3UL  << 10)   /* SH0 = Inner Shareable */
        | (0UL  << 14)   /* TG0 = 4 KB */
        | (16UL << 16)   /* T1SZ = 16 */
        | (3UL  << 28)   /* SH1 = Inner Shareable */
        | (1UL  << 37);  /* TBI0: top byte ignored */
    asm volatile("msr tcr_el1, %0" :: "r"(tcr));

    /* ---------------------------------------------------
     * 4. Load TTBR0_EL1
     * --------------------------------------------------- */
    asm volatile("msr ttbr0_el1, %0" :: "r"((unsigned long)l0_table));
    asm volatile("isb");

    /* ---------------------------------------------------
     * 5. Enable MMU in SCTLR_EL1
     * --------------------------------------------------- */
    asm volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    sctlr |= (1UL << 0);   /* M: MMU enable */
    sctlr |= (1UL << 2);   /* C: D-cache enable */
    sctlr |= (1UL << 12);  /* I: I-cache enable */
    asm volatile("msr sctlr_el1, %0; isb" :: "r"(sctlr));
}
