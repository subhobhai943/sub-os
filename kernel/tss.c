// SUB OS - Task State Segment Implementation
// Copyright (c) 2025 SUB OS Project

#include "tss.h"
#include "kernel.h"

static tss_t tss;

extern void gdt_set_tss(unsigned int base, unsigned int limit);
extern void tss_flush();

void tss_init() {
    print_string("[OK] Initializing Task State Segment...\n");
    
    unsigned char* ptr = (unsigned char*)&tss;
    for (unsigned int i = 0; i < sizeof(tss_t); i++) {
        ptr[i] = 0;
    }
    
    tss.ss0 = 0x10;
    tss.esp0 = 0;
    tss.cs = 0x0b;
    tss.ss = 0x13;
    tss.ds = 0x13;
    tss.es = 0x13;
    tss.fs = 0x13;
    tss.gs = 0x13;
    
    unsigned int base = (unsigned int)&tss;
    unsigned int limit = sizeof(tss_t) - 1;
    gdt_set_tss(base, limit);
    tss_flush();
    
    print_string("  TSS base: ");
    print_hex(base);
    print_string("\n");
    print_string("  TSS limit: ");
    print_hex(limit);
    print_string("\n");
    print_string("[OK] TSS initialized\n");
}

void tss_set_kernel_stack(unsigned int stack) {
    tss.esp0 = stack;
}

tss_t* tss_get() {
    return &tss;
}
