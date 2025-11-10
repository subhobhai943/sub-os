// SUB OS - Task State Segment Header
// Copyright (c) 2025 SUB OS Project

#ifndef TSS_H
#define TSS_H

// Task State Segment structure
typedef struct {
    unsigned int prev_tss;   // Previous TSS
    unsigned int esp0;       // Kernel stack pointer
    unsigned int ss0;        // Kernel stack segment
    unsigned int esp1;
    unsigned int ss1;
    unsigned int esp2;
    unsigned int ss2;
    unsigned int cr3;        // Page directory
    unsigned int eip;
    unsigned int eflags;
    unsigned int eax;
    unsigned int ecx;
    unsigned int edx;
    unsigned int ebx;
    unsigned int esp;        // User stack
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;
    unsigned int es;
    unsigned int cs;
    unsigned int ss;
    unsigned int ds;
    unsigned int fs;
    unsigned int gs;
    unsigned int ldt;
    unsigned short trap;
    unsigned short iomap;
} __attribute__((packed)) tss_t;

void tss_init();
void tss_set_kernel_stack(unsigned int stack);
tss_t* tss_get();

#endif
