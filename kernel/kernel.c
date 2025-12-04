// SUB OS Kernel v0.10.0
// Copyright (c) 2025 SUB OS Project

#include "kernel.h"
#include "idt.h"
#include "keyboard.h"
#include "timer.h"
#include "memory.h"
#include "pmm.h"
#include "heap.h"
#include "paging.h"
#include "process.h"
#include "syscall.h"
#include "tss.h"
#include "ata.h"
#include "fs.h"

#define VIDEO_MEMORY 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f

int cursor_row = 0;
int cursor_col = 0;

void outb(unsigned short port, unsigned char val) { 
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port)); 
}

unsigned char inb(unsigned short port) { 
    unsigned char ret; 
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port)); 
    return ret; 
}

unsigned short inw(unsigned short port) { 
    unsigned short ret; 
    asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port)); 
    return ret; 
}

void outw(unsigned short port, unsigned short val) { 
    asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port)); 
}

void clear_screen() {
    char *video = (char *)VIDEO_MEMORY;
    for (int i = 0; i < MAX_ROWS * MAX_COLS; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = WHITE_ON_BLACK;
    }
    cursor_row = 0;
    cursor_col = 0;
}

void print_char(char c, int col, int row, char attr) {
    char *video = (char *)VIDEO_MEMORY;
    if (!attr) attr = WHITE_ON_BLACK;
    
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else if (c == '\b') {
        if (cursor_col > 0) {
            cursor_col--;
            video[(cursor_row * MAX_COLS + cursor_col) * 2] = ' ';
        }
    } else {
        if (col >= 0 && row >= 0) {
            video[(row * MAX_COLS + col) * 2] = c;
            video[(row * MAX_COLS + col) * 2 + 1] = attr;
        } else {
            video[(cursor_row * MAX_COLS + cursor_col) * 2] = c;
            video[(cursor_row * MAX_COLS + cursor_col) * 2 + 1] = attr;
            cursor_col++;
        }
    }
    
    if (cursor_col >= MAX_COLS) {
        cursor_col = 0;
        cursor_row++;
    }
    
    if (cursor_row >= MAX_ROWS) {
        for (int i = 0; i < (MAX_ROWS - 1) * MAX_COLS; i++) {
            video[i * 2] = video[(i + MAX_COLS) * 2];
            video[i * 2 + 1] = video[(i + MAX_COLS) * 2 + 1];
        }
        for (int i = (MAX_ROWS - 1) * MAX_COLS; i < MAX_ROWS * MAX_COLS; i++) {
            video[i * 2] = ' ';
            video[i * 2 + 1] = WHITE_ON_BLACK;
        }
        cursor_row = MAX_ROWS - 1;
    }
}

void print_string(const char *str) {
    for (int i = 0; str[i] != 0; i++) {
        print_char(str[i], -1, -1, WHITE_ON_BLACK);
    }
}

void print_hex(unsigned int num) {
    char hex_string[11];
    hex_string[0] = '0';
    hex_string[1] = 'x';
    
    for (int i = 0; i < 8; i++) {
        unsigned char nibble = (num >> (28 - i * 4)) & 0xF;
        hex_string[i + 2] = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
    }
    hex_string[10] = 0;
    
    print_string(hex_string);
}

void print_dec(unsigned int num) {
    if (num == 0) {
        print_char('0', -1, -1, WHITE_ON_BLACK);
        return;
    }
    
    char buffer[12];
    int i = 0;
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    for (int j = i - 1; j >= 0; j--) {
        print_char(buffer[j], -1, -1, WHITE_ON_BLACK);
    }
}

void kernel_main() {
    clear_screen();
    print_string("===================================\n");
    print_string("     SUB OS v0.10.0 Booting...    \n");
    print_string("===================================\n\n");
    
    print_string("[OK] Initializing IDT...\n");
    idt_init();
    
    print_string("[OK] Initializing Timer...\n");
    timer_init();
    
    print_string("[OK] Initializing Keyboard...\n");
    keyboard_init();
    
    print_string("[OK] Detecting Memory...\n");
    memory_init();
    
    print_string("[OK] Initializing PMM...\n");
    pmm_init();
    
    print_string("[OK] Initializing Heap...\n");
    heap_init();
    
    print_string("[OK] Initializing Paging...\n");
    paging_init();
    
    print_string("[OK] Initializing TSS...\n");
    tss_init();
    
    print_string("[OK] Initializing Syscalls...\n");
    syscall_init();
    
    print_string("[OK] Initializing Process Manager...\n");
    process_init();
    
    ata_init();
    fs_init();
    
    print_string("\n[OK] Mounting filesystem...\n");
    if (fs_mount() == 0) {
        print_string("[OK] Filesystem mounted\n");
    } else {
        print_string("[WARN] Filesystem mount failed\n");
    }
    
    print_string("\n===================================\n");
    print_string("   SUB OS v0.10.0 Ready!          \n");
    print_string("===================================\n\n");
    print_string("Welcome to SUB OS!\n");
    print_string("Type commands...\n\n");
    
    while (1) {
        asm volatile("hlt");
    }
}
