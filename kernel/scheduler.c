// SUB OS - Scheduler (Round-Robin)
// Copyright (c) 2025 SUB OS Project

#include "process.h"
#include "kernel.h"

// Ready queue
static process_t* ready_queue_head = 0;
static process_t* ready_queue_tail = 0;

// Scheduler statistics
static unsigned long context_switches = 0;

// Initialize scheduler
void scheduler_init() {
    print_string("[OK] Initializing Scheduler...\n");
    ready_queue_head = 0;
    ready_queue_tail = 0;
    context_switches = 0;
    print_string("  Algorithm: Round-Robin\n");
    print_string("  Time quantum: 50ms (5 ticks)\n");
    print_string("[OK] Scheduler initialized\n");
}

// Add process to ready queue
void scheduler_add(process_t* process) {
    if (!process) return;
    
    process->next = 0;
    process->state = PROCESS_READY;
    
    if (ready_queue_tail) {
        ready_queue_tail->next = process;
        ready_queue_tail = process;
    } else {
        ready_queue_head = process;
        ready_queue_tail = process;
    }
}

// Remove process from ready queue
void scheduler_remove(process_t* process) {
    if (!process) return;
    
    if (ready_queue_head == process) {
        ready_queue_head = process->next;
        if (ready_queue_tail == process) {
            ready_queue_tail = 0;
        }
        return;
    }
    
    process_t* current = ready_queue_head;
    while (current && current->next != process) {
        current = current->next;
    }
    
    if (current) {
        current->next = process->next;
        if (ready_queue_tail == process) {
            ready_queue_tail = current;
        }
    }
}

// Get next process (round-robin)
process_t* scheduler_next() {
    if (!ready_queue_head) {
        return 0;  // No processes ready
    }
    
    // Remove from front
    process_t* next = ready_queue_head;
    ready_queue_head = next->next;
    
    if (!ready_queue_head) {
        ready_queue_tail = 0;
    }
    
    // Add back to end if not terminated
    if (next->state != PROCESS_TERMINATED) {
        scheduler_add(next);
    }
    
    return next;
}

// Schedule next process
void schedule() {
    process_t* current = process_get_current();
    process_t* next = scheduler_next();
    
    if (!next) return;  // No process to switch to
    
    if (next != current) {
        context_switches++;
        process_switch(next);
    }
}

// Get context switch count
unsigned long scheduler_get_switches() {
    return context_switches;
}
