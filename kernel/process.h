// SUB OS - Process Management Header
// Copyright (c) 2025 SUB OS Project

#ifndef PROCESS_H
#define PROCESS_H

// Process states
typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

// CPU registers saved during context switch
typedef struct {
    unsigned int eax, ebx, ecx, edx;
    unsigned int esi, edi;
    unsigned int esp, ebp;
    unsigned int eip;
    unsigned int eflags;
    unsigned int cr3;  // Page directory
} registers_t;

// Process Control Block (PCB)
typedef struct process {
    unsigned int pid;              // Process ID
    char name[32];                 // Process name
    process_state_t state;         // Current state
    registers_t registers;         // Saved registers
    unsigned int kernel_stack;     // Kernel stack
    unsigned int user_stack;       // User stack
    unsigned int page_directory;   // Virtual memory space
    unsigned long priority;        // Scheduling priority
    unsigned long quantum;         // Time quantum (ticks)
    unsigned long cpu_time;        // Total CPU time used
    struct process* next;          // Next process in queue
} process_t;

// Process management functions
void process_init();
process_t* process_create(const char* name, void (*entry_point)());
void process_terminate(process_t* process);
process_t* process_get_current();
void process_switch(process_t* next);

// Scheduler functions
void scheduler_init();
void scheduler_add(process_t* process);
void scheduler_remove(process_t* process);
process_t* scheduler_next();
void schedule();

#endif
