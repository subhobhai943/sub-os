// SUB OS - Process Management
// Copyright (c) 2025 SUB OS Project

#include "process.h"
#include "heap.h"
#include "pmm.h"
#include "kernel.h"

// Process list
static process_t* process_list = 0;
static process_t* current_process = 0;
static unsigned int next_pid = 0;

// Idle process
static process_t* idle_process = 0;

// Initialize process management
void process_init() {
    print_string("[OK] Initializing Process Management...\n");
    
    // Create idle process (PID 0)
    idle_process = (process_t*)kmalloc(sizeof(process_t));
    idle_process->pid = next_pid++;
    
    // Copy name
    const char* idle_name = "idle";
    int i;
    for (i = 0; idle_name[i] && i < 31; i++) {
        idle_process->name[i] = idle_name[i];
    }
    idle_process->name[i] = 0;
    
    idle_process->state = PROCESS_RUNNING;
    idle_process->priority = 0;
    idle_process->quantum = 1;
    idle_process->cpu_time = 0;
    idle_process->next = idle_process;  // Points to itself
    
    process_list = idle_process;
    current_process = idle_process;
    
    print_string("  Created idle process (PID 0)\n");
    print_string("[OK] Process Management initialized\n");
}

// Create new process
process_t* process_create(const char* name, void (*entry_point)()) {
    process_t* process = (process_t*)kmalloc(sizeof(process_t));
    
    if (!process) {
        print_string("[ERROR] Failed to allocate process!\n");
        return 0;
    }
    
    // Set process ID
    process->pid = next_pid++;
    
    // Copy name
    int i;
    for (i = 0; name[i] && i < 31; i++) {
        process->name[i] = name[i];
    }
    process->name[i] = 0;
    
    // Set initial state
    process->state = PROCESS_READY;
    process->priority = 10;
    process->quantum = 5;
    process->cpu_time = 0;
    
    // Allocate kernel stack (4KB)
    process->kernel_stack = pmm_alloc_page();
    if (process->kernel_stack == 0) {
        kfree(process);
        print_string("[ERROR] Failed to allocate stack!\n");
        return 0;
    }
    
    // Set up initial stack frame
    unsigned int* stack = (unsigned int*)(process->kernel_stack + 4096);
    stack--;
    *stack = 0x202;  // EFLAGS (interrupts enabled)
    stack--;
    *stack = 0x08;   // CS
    stack--;
    *stack = (unsigned int)entry_point;  // EIP
    
    // Set ESP
    process->registers.esp = (unsigned int)stack;
    process->registers.ebp = process->kernel_stack + 4096;
    
    // Add to process list
    scheduler_add(process);
    
    return process;
}

// Get current process
process_t* process_get_current() {
    return current_process;
}

// Terminate process
void process_terminate(process_t* process) {
    if (!process) return;
    
    process->state = PROCESS_TERMINATED;
    
    // Free kernel stack
    if (process->kernel_stack) {
        pmm_free_page(process->kernel_stack);
    }
    
    // Remove from scheduler
    scheduler_remove(process);
    
    // Free PCB
    kfree(process);
}

// Switch to next process
void process_switch(process_t* next) {
    if (!next || next == current_process) return;
    
    process_t* prev = current_process;
    current_process = next;
    
    // In a real implementation, we'd save/restore registers here
    // For now, this is a placeholder
    
    prev->state = PROCESS_READY;
    next->state = PROCESS_RUNNING;
}
