// SUB OS - Process Management
// Copyright (c) 2025 SUB OS Project

#include "process.h"
#include "heap.h"
#include "pmm.h"
#include "kernel.h"

extern void enter_usermode(unsigned int entry_point, unsigned int user_stack);

static process_t* process_list = 0;
static process_t* current_process = 0;
static unsigned int next_pid = 0;
static process_t* idle_process = 0;

void process_init() {
    print_string("[OK] Initializing Process Management...\n");
    idle_process = (process_t*)kmalloc(sizeof(process_t));
    idle_process->pid = next_pid++;
    const char* idle_name = "idle";
    int i;
    for (i = 0; idle_name[i] && i < 31; i++) {
        idle_process->name[i] = idle_name[i];
    }
    idle_process->name[i] = 0;
    idle_process->state = PROCESS_RUNNING;
    idle_process->privilege = PROCESS_KERNEL;
    idle_process->priority = 0;
    idle_process->quantum = 1;
    idle_process->cpu_time = 0;
    idle_process->next = idle_process;
    process_list = idle_process;
    current_process = idle_process;
    print_string("  Created idle process (PID 0)\n");
    print_string("[OK] Process Management initialized\n");
}

process_t* process_create(const char* name, void (*entry_point)()) {
    process_t* process = (process_t*)kmalloc(sizeof(process_t));
    if (!process) {
        print_string("[ERROR] Failed to allocate process!\n");
        return 0;
    }
    process->pid = next_pid++;
    int i;
    for (i = 0; name[i] && i < 31; i++) {
        process->name[i] = name[i];
    }
    process->name[i] = 0;
    process->state = PROCESS_READY;
    process->privilege = PROCESS_KERNEL;
    process->priority = 10;
    process->quantum = 5;
    process->cpu_time = 0;
    process->kernel_stack = pmm_alloc_page();
    if (process->kernel_stack == 0) {
        kfree(process);
        print_string("[ERROR] Failed to allocate stack!\n");
        return 0;
    }
    unsigned int* stack = (unsigned int*)(process->kernel_stack + 4096);
    stack--;
    *stack = 0x202;
    stack--;
    *stack = 0x08;
    stack--;
    *stack = (unsigned int)entry_point;
    process->registers.esp = (unsigned int)stack;
    process->registers.ebp = process->kernel_stack + 4096;
    scheduler_add(process);
    return process;
}

process_t* process_create_user(const char* name, void (*entry_point)()) {
    process_t* process = (process_t*)kmalloc(sizeof(process_t));
    if (!process) {
        print_string("[ERROR] Failed to allocate user process!\n");
        return 0;
    }
    process->pid = next_pid++;
    int i;
    for (i = 0; name[i] && i < 31; i++) {
        process->name[i] = name[i];
    }
    process->name[i] = 0;
    process->state = PROCESS_READY;
    process->privilege = PROCESS_USER;
    process->priority = 10;
    process->quantum = 5;
    process->cpu_time = 0;
    process->kernel_stack = pmm_alloc_page();
    if (process->kernel_stack == 0) {
        kfree(process);
        print_string("[ERROR] Failed to allocate kernel stack!\n");
        return 0;
    }
    process->user_stack = pmm_alloc_page();
    if (process->user_stack == 0) {
        pmm_free_page(process->kernel_stack);
        kfree(process);
        print_string("[ERROR] Failed to allocate user stack!\n");
        return 0;
    }
    unsigned int user_esp = process->user_stack + 4096;
    unsigned int* kstack = (unsigned int*)(process->kernel_stack + 4096);
    kstack--;
    *kstack = user_esp;
    kstack--;
    *kstack = (unsigned int)entry_point;
    kstack--;
    *kstack = (unsigned int)enter_usermode;
    process->registers.esp = (unsigned int)kstack;
    process->registers.ebp = process->kernel_stack + 4096;
    scheduler_add(process);
    return process;
}

process_t* process_get_current() { return current_process; }

void process_terminate(process_t* process) {
    if (!process) return;
    process->state = PROCESS_TERMINATED;
    if (process->kernel_stack) pmm_free_page(process->kernel_stack);
    if (process->user_stack) pmm_free_page(process->user_stack);
    scheduler_remove(process);
    kfree(process);
}

void process_switch(process_t* next) {
    if (!next || next == current_process) return;
    process_t* prev = current_process;
    current_process = next;
    prev->state = PROCESS_READY;
    next->state = PROCESS_RUNNING;
}
