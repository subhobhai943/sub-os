// SUB OS - Process Management Header
// Copyright (c) 2025 SUB OS Project

#ifndef PROCESS_H
#define PROCESS_H

typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

typedef enum {
    PROCESS_KERNEL = 0,
    PROCESS_USER = 3
} process_privilege_t;

typedef struct {
    unsigned int eax, ebx, ecx, edx;
    unsigned int esi, edi;
    unsigned int esp, ebp;
    unsigned int eip;
    unsigned int eflags;
    unsigned int cr3;
} registers_t;

typedef struct process {
    unsigned int pid;
    char name[32];
    process_state_t state;
    process_privilege_t privilege;
    registers_t registers;
    unsigned int kernel_stack;
    unsigned int user_stack;
    unsigned int page_directory;
    unsigned long priority;
    unsigned long quantum;
    unsigned long cpu_time;
    struct process* next;
} process_t;

void process_init();
process_t* process_create(const char* name, void (*entry_point)());
process_t* process_create_user(const char* name, void (*entry_point)());
void process_terminate(process_t* process);
process_t* process_get_current();
void process_switch(process_t* next);

void scheduler_init();
void scheduler_add(process_t* process);
void scheduler_remove(process_t* process);
process_t* scheduler_next();
void schedule();

#endif
