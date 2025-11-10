// SUB OS - System Call Interface
// Copyright (c) 2025 SUB OS Project

#ifndef SYSCALL_H
#define SYSCALL_H

// System call numbers
#define SYS_EXIT    0
#define SYS_FORK    1
#define SYS_READ    2
#define SYS_WRITE   3
#define SYS_OPEN    4
#define SYS_CLOSE   5
#define SYS_GETPID  6
#define SYS_SLEEP   7
#define SYS_YIELD   8

// System call return values
#define SYSCALL_SUCCESS  0
#define SYSCALL_ERROR   -1

// Initialize system calls
void syscall_init();

// System call handlers
int sys_exit(int status);
int sys_fork();
int sys_read(int fd, void* buf, unsigned int count);
int sys_write(int fd, const void* buf, unsigned int count);
int sys_getpid();
int sys_sleep(unsigned int ms);
int sys_yield();

// System call dispatcher
int syscall_handler(int syscall_num, int arg1, int arg2, int arg3);

#endif
