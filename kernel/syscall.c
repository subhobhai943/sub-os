// SUB OS - System Call Implementation
// Copyright (c) 2025 SUB OS Project

#include "syscall.h"
#include "process.h"
#include "timer.h"
#include "kernel.h"

// System call table
typedef int (*syscall_fn_t)(int, int, int);

static syscall_fn_t syscall_table[256];

// Initialize system call table
void syscall_init() {
    print_string("[OK] Initializing System Calls...\n");
    
    // Clear syscall table
    for (int i = 0; i < 256; i++) {
        syscall_table[i] = 0;
    }
    
    // Register system calls
    syscall_table[SYS_EXIT] = (syscall_fn_t)sys_exit;
    syscall_table[SYS_FORK] = (syscall_fn_t)sys_fork;
    syscall_table[SYS_READ] = (syscall_fn_t)sys_read;
    syscall_table[SYS_WRITE] = (syscall_fn_t)sys_write;
    syscall_table[SYS_GETPID] = (syscall_fn_t)sys_getpid;
    syscall_table[SYS_SLEEP] = (syscall_fn_t)sys_sleep;
    syscall_table[SYS_YIELD] = (syscall_fn_t)sys_yield;
    
    print_string("  Registered 9 system calls\n");
    print_string("  Interface: INT 0x80\n");
    print_string("[OK] System Calls initialized\n");
}

// System call dispatcher (called from interrupt handler)
int syscall_handler(int syscall_num, int arg1, int arg2, int arg3) {
    if (syscall_num < 0 || syscall_num >= 256) {
        return SYSCALL_ERROR;
    }
    
    syscall_fn_t handler = syscall_table[syscall_num];
    if (!handler) {
        print_string("[SYSCALL] Invalid system call: ");
        print_hex(syscall_num);
        print_string("\n");
        return SYSCALL_ERROR;
    }
    
    return handler(arg1, arg2, arg3);
}

// sys_exit - Terminate current process
int sys_exit(int status) {
    process_t* current = process_get_current();
    
    if (current && current->pid != 0) {  // Don't exit idle process
        print_string("[SYSCALL] Process ");
        print_dec(current->pid);
        print_string(" exiting with status ");
        print_dec(status);
        print_string("\n");
        
        process_terminate(current);
        schedule();  // Switch to next process
    }
    
    return SYSCALL_SUCCESS;
}

// sys_fork - Create child process (stub for now)
int sys_fork() {
    print_string("[SYSCALL] fork() not yet implemented\n");
    return SYSCALL_ERROR;
}

// sys_read - Read from file descriptor (stub)
int sys_read(int fd, void* buf, unsigned int count) {
    print_string("[SYSCALL] read() not yet implemented\n");
    return SYSCALL_ERROR;
}

// sys_write - Write to file descriptor
int sys_write(int fd, const void* buf, unsigned int count) {
    if (fd == 1) {  // stdout
        const char* str = (const char*)buf;
        for (unsigned int i = 0; i < count; i++) {
            char temp[2] = {str[i], 0};
            print_string(temp);
        }
        return count;
    }
    return SYSCALL_ERROR;
}

// sys_getpid - Get current process ID
int sys_getpid() {
    process_t* current = process_get_current();
    return current ? current->pid : 0;
}

// sys_sleep - Sleep for milliseconds
int sys_sleep(unsigned int ms) {
    sleep_ms(ms);
    return SYSCALL_SUCCESS;
}

// sys_yield - Yield CPU to another process
int sys_yield() {
    schedule();
    return SYSCALL_SUCCESS;
}
