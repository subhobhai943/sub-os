// SUB OS - Kernel Header
// Copyright (c) 2025 SUB OS Project

#ifndef KERNEL_H
#define KERNEL_H

// I/O operations
void outb(unsigned short port, unsigned char val);
unsigned char inb(unsigned short port);

// Screen functions
void clear_screen();
void print_char(char c, int col, int row, char attr);
void print_string(const char *str);
void print_hex(unsigned int num);

#endif
