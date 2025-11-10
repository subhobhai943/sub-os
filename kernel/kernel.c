// SUB OS Kernel - Main Entry Point
// Copyright (c) 2025 SUB OS Project

#include "kernel.h"
#include "idt.h"
#include "keyboard.h"

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
            // Backspace
            if (cursor_col > 0) {
                cursor_col--;
                print_char(' ', cursor_col, cursor_row, WHITE_ON_BLACK);
            }
        } else if (str[i] == '\t') {
            // Tab - move to next multiple of 4
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
            // Simple scroll - move everything up one line
            for (int row = 0; row < MAX_ROWS - 1; row++) {
                for (int col = 0; col < MAX_COLS; col++) {
                    unsigned char *vidmem = (unsigned char*)VIDEO_MEMORY;
                    int src_offset = 2 * ((row + 1) * MAX_COLS + col);
                    int dst_offset = 2 * (row * MAX_COLS + col);
                    vidmem[dst_offset] = vidmem[src_offset];
                    vidmem[dst_offset + 1] = vidmem[src_offset + 1];
                }
            }
            // Clear last line
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

// Custom IRQ handler that calls keyboard handler
void irq_handler(unsigned int irq_no, unsigned int err_code) {
    // Handle keyboard interrupt (IRQ1)
    if (irq_no == 33) {
        keyboard_handler();
    }
    
    // Send EOI (End of Interrupt) to PICs
    if (irq_no >= 40) {
        outb(0xA0, 0x20);  // Send to slave
    }
    outb(0x20, 0x20);      // Send to master
}

// Kernel main function
void main() {
    clear_screen();
    
    print_string("========================================\n");
    print_string("     SUB OS - Alpha v0.2.0              \n");
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
    print_string("[OK] IDT loaded and interrupts enabled\n");
    
    // Initialize keyboard
    keyboard_init();
    
    print_string("\n");
    print_string("SUB OS Kernel Features:\n");
    print_string("  * 32-bit Protected Mode\n");
    print_string("  * Custom Bootloader\n");
    print_string("  * VGA Text Mode Driver\n");
    print_string("  * Interrupt Descriptor Table\n");
    print_string("  * Exception Handling\n");
    print_string("  * PS/2 Keyboard Driver\n");
    print_string("  * Hardware Interrupt Handling\n\n");
    
    print_string("System Status: RUNNING\n");
    print_string("Architecture: x86\n");
    print_string("Build Date: November 11, 2025\n\n");
    
    // Enable interrupts
    asm volatile("sti");
    
    print_string("Interrupts enabled! Try typing...\n");
    print_string("Type something: ");
    
    // Main kernel loop - now handles keyboard input
    while(1) {
        asm volatile("hlt");  // Halt until next interrupt
    }
}
