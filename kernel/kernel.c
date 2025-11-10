// SUB OS Kernel - Main Entry Point
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

void print_char(char c, int col, int row, char attr) {
    unsigned char *vidmem = (unsigned char*)VIDEO_MEMORY;
    int offset = 2 * (row * MAX_COLS + col);
    vidmem[offset] = c;
    vidmem[offset + 1] = attr;
}

void clear_screen() {
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            print_char(' ', col, row, WHITE_ON_BLACK);
        }
    }
    cursor_row = 0;
    cursor_col = 0;
}

void print_string(const char *str) {
    int i = 0;
    while (str[i] != 0) {
        if (str[i] == '\n') {
            cursor_row++;
            cursor_col = 0;
        } else if (str[i] == '\b') {
            if (cursor_col > 0) {
                cursor_col--;
                print_char(' ', cursor_col, cursor_row, WHITE_ON_BLACK);
            }
        } else if (str[i] == '\t') {
            cursor_col = (cursor_col + 4) & ~3;
        } else {
            print_char(str[i], cursor_col, cursor_row, WHITE_ON_BLACK);
            cursor_col++;
            if (cursor_col >= MAX_COLS) {
                cursor_col = 0;
                cursor_row++;
            }
        }
        if (cursor_row >= MAX_ROWS) {
            for (int row = 0; row < MAX_ROWS - 1; row++) {
                for (int col = 0; col < MAX_COLS; col++) {
                    unsigned char *vidmem = (unsigned char*)VIDEO_MEMORY;
                    int src_offset = 2 * ((row + 1) * MAX_COLS + col);
                    int dst_offset = 2 * (row * MAX_COLS + col);
                    vidmem[dst_offset] = vidmem[src_offset];
                    vidmem[dst_offset + 1] = vidmem[src_offset + 1];
                }
            }
            for (int col = 0; col < MAX_COLS; col++) {
                print_char(' ', col, MAX_ROWS - 1, WHITE_ON_BLACK);
            }
            cursor_row = MAX_ROWS - 1;
        }
        i++;
    }
}

void print_hex(unsigned int num) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[11] = "0x00000000";
    for (int i = 9; i >= 2; i--) {
        buffer[i] = hex_chars[num & 0xF];
        num >>= 4;
    }
    print_string(buffer);
}

void print_dec(unsigned int num) {
    if (num == 0) {
        print_string("0");
        return;
    }
    char buffer[12];
    int i = 0;
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    for (int j = i - 1; j >= 0; j--) {
        char str[2] = {buffer[j], 0};
        print_string(str);
    }
}

void irq_handler(unsigned int irq_no, unsigned int err_code) {
    if (irq_no == 32) {
        timer_handler();
        // Schedule every 50ms (5 ticks at 100Hz)
        if (timer_get_ticks() % 5 == 0) {
            schedule();
        }
    }
    else if (irq_no == 33) {
        keyboard_handler();
    }
    if (irq_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}

// Test process functions
void test_process_1() {
    while(1) {
        // Process 1 work
        asm volatile("hlt");
    }
}

void test_process_2() {
    while(1) {
        // Process 2 work
        asm volatile("hlt");
    }
}

void main() {
    clear_screen();
    print_string("========================================\n");
    print_string("     SUB OS - Alpha v0.6.0              \n");
    print_string("     Built from Scratch                 \n");
    print_string("========================================\n\n");
    print_string("[OK] Bootloader initialized\n");
    print_string("[OK] Protected mode enabled\n");
    print_string("[OK] GDT loaded\n");
    print_string("[OK] Kernel loaded\n");
    idt_init();
    print_string("[OK] IDT loaded\n");
    timer_init();
    print_string("\n");
    memory_init();
    print_string("\n");
    pmm_init();
    print_string("\n");
    paging_init();
    print_string("\n");
    heap_init();
    print_string("\n");
    process_init();
    print_string("\n");
    scheduler_init();
    print_string("\n");
    keyboard_init();
    
    // Create test processes
    print_string("\nCreating test processes...\n");
    process_t* proc1 = process_create("test1", test_process_1);
    if (proc1) {
        print_string("  Created process: test1 (PID ");
        print_dec(proc1->pid);
        print_string(")\n");
    }
    
    process_t* proc2 = process_create("test2", test_process_2);
    if (proc2) {
        print_string("  Created process: test2 (PID ");
        print_dec(proc2->pid);
        print_string(")\n");
    }
    
    print_string("\n");
    print_string("SUB OS Kernel Features:\n");
    print_string("  * 32-bit Protected Mode\n");
    print_string("  * Interrupt Handling (IDT)\n");
    print_string("  * PS/2 Keyboard Driver\n");
    print_string("  * PIT Timer Driver (100 Hz)\n");
    print_string("  * Memory Detection (E820)\n");
    print_string("  * Physical Memory Manager\n");
    print_string("  * Virtual Memory (Paging)\n");
    print_string("  * Heap Allocator (kmalloc/kfree)\n");
    print_string("  * Process Management (PCB)\n");
    print_string("  * Round-Robin Scheduler\n\n");
    
    print_string("System Status: RUNNING\n");
    print_string("Architecture: x86\n");
    print_string("Build Date: November 11, 2025\n");
    print_string("Uptime: 0 seconds\n");
    print_string("Active Processes: 3\n");
    
    print_string("\nMemory Statistics:\n");
    print_string("  Free pages: ");
    print_dec(pmm_get_free_pages());
    print_string(" (");
    print_dec(pmm_get_free_memory() / 1024);
    print_string(" KB)\n");
    
    asm volatile("sti");
    
    print_string("\nMultitasking enabled! Scheduler active.\n");
    print_string("Type something...\n");
    print_string("> ");
    
    unsigned long last_second = 0;
    
    while(1) {
        unsigned long uptime = get_uptime();
        if (uptime != last_second) {
            last_second = uptime;
            int saved_row = cursor_row;
            int saved_col = cursor_col;
            cursor_row = 17;
            cursor_col = 8;
            print_string("  ");
            cursor_col = 8;
            print_dec(uptime);
            print_string(" seconds  ");
            cursor_row = saved_row;
            cursor_col = saved_col;
        }
        asm volatile("hlt");
    }
}
