// SUB OS - Kernel Header
// Copyright (c) 2025 SUB OS Project

#ifndef KERNEL_H
#define KERNEL_H

// I/O operations
void outb(unsigned short port, unsigned char val);
unsigned char inb(unsigned short port);
unsigned short inw(unsigned short port);
void outw(unsigned short port, unsigned short val);

// Screen operations
void clear_screen();
void print_string(const char *str);
void print_char(char c, int col, int row, char attr);
void print_hex(unsigned int num);
void print_dec(unsigned int num);

// Timer
void timer_init();
void timer_handler();
unsigned long timer_get_ticks();
unsigned long get_uptime();
void sleep_ms(unsigned int ms);

// Keyboard
void keyboard_init();
void keyboard_handler();

// Memory
void memory_init();

// Scheduler
void schedule();

#endif
