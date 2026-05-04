// SUB OS Kernel v0.11.0
// Copyright (c) 2025-2026 SUB OS Project

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
#include "shell.h"
#include "gui.h"

#define VIDEO_MEMORY   0xB8000
#define MAX_ROWS       25
#define MAX_COLS       80
#define WHITE_ON_BLACK 0x0F

int cursor_row = 0;
int cursor_col = 0;

// ── I/O port helpers ──────────────────────────────────────────────────────
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

// ── Hardware cursor ──────────────────────────────────────────────────────
static void update_cursor(int row, int col) {
    unsigned short pos = (unsigned short)(row * MAX_COLS + col);
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

// ── Screen helpers ────────────────────────────────────────────────────────
void clear_screen(void) {
    volatile unsigned short *video = (volatile unsigned short *)VIDEO_MEMORY;
    for (int i = 0; i < MAX_ROWS * MAX_COLS; i++)
        video[i] = (unsigned short)((WHITE_ON_BLACK << 8) | ' ');
    cursor_row = 0;
    cursor_col = 0;
    update_cursor(0, 0);
}

void print_char(char c, int col, int row, char attr) {
    volatile unsigned short *video = (volatile unsigned short *)VIDEO_MEMORY;
    if (!attr) attr = (char)WHITE_ON_BLACK;

    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else if (c == '\b') {
        if (cursor_col > 0) {
            cursor_col--;
            video[cursor_row * MAX_COLS + cursor_col] =
                (unsigned short)((attr << 8) | ' ');
        }
    } else {
        if (col >= 0 && row >= 0) {
            video[row * MAX_COLS + col] = (unsigned short)((attr << 8) | (unsigned char)c);
        } else {
            video[cursor_row * MAX_COLS + cursor_col] =
                (unsigned short)((attr << 8) | (unsigned char)c);
            cursor_col++;
        }
    }

    if (cursor_col >= MAX_COLS) { cursor_col = 0; cursor_row++; }

    if (cursor_row >= MAX_ROWS) {
        // Scroll up one line
        for (int i = 0; i < (MAX_ROWS - 1) * MAX_COLS; i++)
            video[i] = video[i + MAX_COLS];
        for (int i = (MAX_ROWS - 1) * MAX_COLS; i < MAX_ROWS * MAX_COLS; i++)
            video[i] = (unsigned short)((WHITE_ON_BLACK << 8) | ' ');
        cursor_row = MAX_ROWS - 1;
    }
    update_cursor(cursor_row, cursor_col);
}

void print_string(const char *str) {
    for (int i = 0; str[i]; i++)
        print_char(str[i], -1, -1, (char)WHITE_ON_BLACK);
}

void print_hex(unsigned int num) {
    char buf[11];
    buf[0] = '0'; buf[1] = 'x';
    for (int i = 0; i < 8; i++) {
        unsigned char n = (unsigned char)((num >> (28 - i * 4)) & 0xF);
        buf[i + 2] = (n < 10) ? ('0' + n) : ('A' + n - 10);
    }
    buf[10] = '\0';
    print_string(buf);
}

void print_dec(unsigned int num) {
    if (num == 0) { print_char('0', -1, -1, (char)WHITE_ON_BLACK); return; }
    char buf[12]; int i = 0;
    while (num > 0) { buf[i++] = '0' + (int)(num % 10); num /= 10; }
    for (int j = i - 1; j >= 0; j--)
        print_char(buf[j], -1, -1, (char)WHITE_ON_BLACK);
}

// ── Kernel entry point ────────────────────────────────────────────────────
void kernel_main(void) {
    clear_screen();
    print_string("[BOOT] SUB OS v0.11.0 starting...\n");

    idt_init();
    timer_init();
    keyboard_init();
    memory_init();
    pmm_init();
    heap_init();
    paging_init();
    tss_init();
    syscall_init();
    process_init();
    scheduler_init();
    ata_init();
    fs_init();
    fs_mount();

    // Hand control to the interactive shell with GUI banner
    shell_run();

    // Should never reach here
    while (1) asm volatile("hlt");
}
