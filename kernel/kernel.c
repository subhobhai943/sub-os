// SUB OS Kernel v0.10.0
// Copyright (c) 2025 SUB OS Project

#include "kernel.h"

#if defined(__aarch64__) || defined(__arm__)
#include "uart.h"
#include "memory.h"
#include "pmm.h"
#include "heap.h"
#include "keyboard.h"
#include "ata.h"
#include "fs.h"
#include "shell.h"
#else
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
#include "shell.h"
#include "graphics.h"
#include "mouse.h"
#endif

#define VIDEO_MEMORY 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 40
#define WHITE_ON_BLACK 0x0f

int cursor_row = 0;
int cursor_col = 0;

#if !defined(__aarch64__) && !defined(__arm__)
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
#endif

void clear_screen() {
#if defined(__aarch64__) || defined(__arm__)
    uart_puts("\033[2J\033[H"); // ANSI clear screen
#else
    graphics_clear(COLOR_BLACK);
    graphics_present();
    cursor_row = 0;
    cursor_col = 0;
#endif
}

void print_char(char c, int col, int row, char attr) {
#if defined(__aarch64__) || defined(__arm__)
    uart_putc(c);
    (void)col; (void)row; (void)attr;
#else
    if (!attr) attr = COLOR_WHITE;
    
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else if (c == '\b') {
        if (cursor_col > 0) {
            cursor_col--;
            // Clear char (draw black space)
            draw_rect(cursor_col * 8, cursor_row * 8, 8, 8, COLOR_BLACK);
        }
    } else {
        if (col >= 0 && row >= 0) {
            draw_char(c, col * 8, row * 8, attr);
        } else {
            draw_char(c, cursor_col * 8, cursor_row * 8, attr);
            cursor_col++;
        }
    }
    
    if (cursor_col >= MAX_COLS) {
        cursor_col = 0;
        cursor_row++;
    }
    
    if (cursor_row >= MAX_ROWS) {
        // Scroll up by 8 pixels
        graphics_scroll_up(8, COLOR_BLACK);
        graphics_present();
        cursor_row = MAX_ROWS - 1;
    }
#endif
}

void print_string(const char *str) {
#if defined(__aarch64__) || defined(__arm__)
    uart_puts(str);
#else
    for (int i = 0; str[i] != 0; i++) {
        print_char(str[i], -1, -1, WHITE_ON_BLACK);
    }
    graphics_present();
#endif
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
#if !defined(__aarch64__) && !defined(__arm__)
    graphics_init();
    
    // Draw Desktop Background
    draw_rect(0, 0, 320, 200, COLOR_BLUE);

    // Draw Taskbar
    draw_rect(0, 185, 320, 15, COLOR_LIGHT_GRAY);
    draw_line(0, 185, 320, 185, COLOR_WHITE);

    // Draw Start Button
    draw_rect(2, 187, 40, 11, COLOR_GREEN);
    draw_string("Start", 4, 189, COLOR_BLACK);
    graphics_present();

    // Reset cursor for console output
    cursor_row = 1;
    cursor_col = 1;
#else
    clear_screen();
#endif

    print_string("===================================\n");
    print_string("   SUB OS (Yeagerist Edition)     \n");
    print_string("===================================\n\n");
    
#if !defined(__aarch64__) && !defined(__arm__)
    print_string("[OK] Initializing IDT...\n");
    idt_init();
    
    print_string("[OK] Initializing Timer...\n");
    timer_init();
    
    print_string("[OK] Initializing UART...\n");
    uart_init();
    
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

    print_string("[OK] Initializing Mouse...\n");
    mouse_init();
    
    ata_init();
    fs_init();
    
    print_string("\n[OK] Mounting filesystem...\n");
    if (fs_mount() == 0) {
        print_string("[OK] Filesystem mounted\n");
    } else {
        print_string("[WARN] Filesystem mount failed\n");
    }
#else
    print_string("[OK] ARM Architecture Detected.\n");
    print_string("[OK] UART Initialized (PL011).\n");
    print_string("[INFO] Skipped x86-specific initialization.\n");
    
    memory_init();
    pmm_init();
    heap_init();
    
    void* ptr = kmalloc(128);
    if(ptr) {
        print_string("[OK] Heap Allocation Test Passed: ");
        print_hex((unsigned int)ptr);
        print_string("\n");
        kfree(ptr);
    } else {
        print_string("[FAIL] Heap Allocation Test Failed\n");
    }
    
    keyboard_init();
    ata_init();
    fs_init();
    // fs_mount() will fail on ARM currently as ATA is dummy, but that's fine.
    if (fs_mount() == 0) {
        print_string("[OK] Filesystem mounted\n");
    } else {
        print_string("[WARN] Filesystem mount failed (expected on ARM)\n");
    }

#endif
    
    print_string("\n===================================\n");
    print_string("   SUB OS v0.10.0 Ready!          \n");
    print_string("===================================\n\n");
    
    shell_init();
    shell_run();
    
    while (1) {
#if defined(__aarch64__) || defined(__arm__)
        asm volatile("wfi");
#else
        asm volatile("hlt");
#endif
    }
}
