// SUB OS Kernel - Main Entry Point
// Copyright (c) 2025 SUB OS Project

// VGA text mode definitions
#define VIDEO_MEMORY 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f

// I/O port operations
static inline void outb(unsigned short port, unsigned char val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Cursor position
int cursor_row = 0;
int cursor_col = 0;

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
        } else {
            print_char(str[i], cursor_col, cursor_row, WHITE_ON_BLACK);
            cursor_col++;
            if (cursor_col >= MAX_COLS) {
                cursor_col = 0;
                cursor_row++;
            }
        }
        if (cursor_row >= MAX_ROWS) {
            cursor_row = 0; // Simple wrap for now
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

// Kernel main function
void main() {
    clear_screen();
    
    print_string("========================================\n");
    print_string("     SUB OS - Alpha v0.1.0              \n");
    print_string("     Built from Scratch                 \n");
    print_string("========================================\n\n");
    
    print_string("[OK] Bootloader initialized\n");
    print_string("[OK] Protected mode enabled\n");
    print_string("[OK] GDT loaded\n");
    print_string("[OK] Kernel loaded at ");
    print_hex((unsigned int)&main);
    print_string("\n\n");
    
    print_string("SUB OS Kernel Features:\n");
    print_string("  * 32-bit Protected Mode\n");
    print_string("  * Custom Bootloader\n");
    print_string("  * VGA Text Mode Driver\n");
    print_string("  * Basic Memory Operations\n\n");
    
    print_string("System Status: RUNNING\n");
    print_string("Architecture: x86\n");
    print_string("Build Date: November 11, 2025\n\n");
    
    print_string("Next Steps: Memory Management, IDT, Keyboard\n\n");
    
    print_string("SUB OS is ready! Kernel halted.\n");
    
    // Halt the CPU
    while(1) {
        asm volatile("hlt");
    }
}
