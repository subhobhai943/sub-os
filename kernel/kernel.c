// SUB OS Kernel - Main Entry Point
// Copyright (c) 2025 SUB OS Project

#include "kernel.h"
#include "idt.h"
#include "keyboard.h"
#include "timer.h"
#include "memory.h"

// VGA text mode definitions
#define VIDEO_MEMORY 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f

// Cursor position
int cursor_row = 0;
int cursor_col = 0;

// I/O port operations
void outb(unsigned short port, unsigned char val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Print character at specific location
void print_char(char c, int col, int row, char attr) {
    unsigned char *vidmem = (unsigned char*)VIDEO_MEMORY;
    int offset = 2 * (row * MAX_COLS + col);
    vidmem[offset] = c;
    vidmem[offset + 1] = attr;
}

// Clear screen
void clear_screen() {
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            print_char(' ', col, row, WHITE_ON_BLACK);
        }
    }
    cursor_row = 0;
    cursor_col = 0;
}

// Print string
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

// Print hexadecimal number
void print_hex(unsigned int num) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[11] = "0x00000000";
    
    for (int i = 9; i >= 2; i--) {
        buffer[i] = hex_chars[num & 0xF];
        num >>= 4;
    }
    
    print_string(buffer);
}

// Print decimal number
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
    
    // Print in reverse
    for (int j = i - 1; j >= 0; j--) {
        char str[2] = {buffer[j], 0};
        print_string(str);
    }
}

// Custom IRQ handler that calls device handlers
void irq_handler(unsigned int irq_no, unsigned int err_code) {
    // Handle timer interrupt (IRQ0)
    if (irq_no == 32) {
        timer_handler();
    }
    // Handle keyboard interrupt (IRQ1)
    else if (irq_no == 33) {
        keyboard_handler();
    }
    
    // Send EOI (End of Interrupt) to PICs
    if (irq_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}

// Kernel main function
void main() {
    clear_screen();
    
    print_string("========================================\n");
    print_string("     SUB OS - Alpha v0.3.0              \n");
    print_string("     Built from Scratch                 \n");
    print_string("========================================\n\n");
    
    print_string("[OK] Bootloader initialized\n");
    print_string("[OK] Protected mode enabled\n");
    print_string("[OK] GDT loaded\n");
    print_string("[OK] Kernel loaded at ");
    print_hex((unsigned int)&main);
    print_string("\n");
    
    // Initialize IDT
    print_string("[OK] Initializing IDT...\n");
    idt_init();
    print_string("[OK] IDT loaded\n");
    
    // Initialize timer
    timer_init();
    
    // Initialize memory detection
    print_string("\n");
    memory_init();
    
    // Initialize keyboard
    print_string("\n");
    keyboard_init();
    
    print_string("\n");
    print_string("SUB OS Kernel Features:\n");
    print_string("  * 32-bit Protected Mode\n");
    print_string("  * Custom Bootloader\n");
    print_string("  * VGA Text Mode Driver with Scrolling\n");
    print_string("  * Interrupt Descriptor Table (IDT)\n");
    print_string("  * Exception Handling (32 exceptions)\n");
    print_string("  * Hardware IRQ Handling (16 IRQs)\n");
    print_string("  * PS/2 Keyboard Driver\n");
    print_string("  * PIT Timer Driver (100 Hz)\n");
    print_string("  * Memory Detection (E820)\n");
    print_string("  * Memory Map Parsing\n\n");
    
    print_string("System Status: RUNNING\n");
    print_string("Architecture: x86\n");
    print_string("Build Date: November 11, 2025\n");
    print_string("Uptime: 0 seconds\n\n");
    
    // Enable interrupts
    asm volatile("sti");
    
    print_string("System ready! Type something...\n");
    print_string("> ");
    
    unsigned long last_second = 0;
    
    // Main kernel loop
    while(1) {
        // Update uptime display every second
        unsigned long uptime = get_uptime();
        if (uptime != last_second) {
            last_second = uptime;
            
            // Save cursor position
            int saved_row = cursor_row;
            int saved_col = cursor_col;
            
            // Update uptime at line 17
            cursor_row = 17;
            cursor_col = 8;
            print_string("  ");
            cursor_col = 8;
            print_dec(uptime);
            print_string(" seconds  ");
            
            // Restore cursor position
            cursor_row = saved_row;
            cursor_col = saved_col;
        }
        
        asm volatile("hlt");
    }
}
