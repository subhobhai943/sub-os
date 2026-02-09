// SUB OS - Keyboard Driver
// Copyright (c) 2025 SUB OS Project

#include "keyboard.h"
#include "kernel.h"

#if defined(__aarch64__) || defined(__arm__)
#include "uart.h"

void keyboard_init() {
    print_string("[OK] Keyboard driver (UART) initialized\n");
}

char keyboard_getchar() {
    return uart_getc();
}

void keyboard_handler() {
    // Not used in polling mode for UART
}

#else

// Keyboard port addresses
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
// ... rest of x86 code
// US QWERTY keyboard layout
unsigned char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, /* Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, /* Right shift */
    '*',
    0,  /* Alt */
    ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* F1 */
    0, 0, 0, 0, 0, 0, 0, 0, 0,  /* F2 - F10 */
    0,  /* Num lock */
    0,  /* Scroll lock */
    0,  /* Home key */
    0,  /* Up arrow */
    0,  /* Page up */
    '-',
    0,  /* Left arrow */
    0,
    0,  /* Right arrow */
    '+',
    0,  /* End key */
    0,  /* Down arrow */
    0,  /* Page down */
    0,  /* Insert */
    0,  /* Delete */
    0, 0, 0,
    0,  /* F11 */
    0,  /* F12 */
    0,  /* All other keys are undefined */
};

// Keyboard buffer
#define KEYBOARD_BUFFER_SIZE 256
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int buffer_start = 0;
static int buffer_end = 0;

// Add character to buffer
void keyboard_buffer_add(char c) {
    int next = (buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != buffer_start) {
        keyboard_buffer[buffer_end] = c;
        buffer_end = next;
    }
}

// Get character from buffer
char keyboard_getchar() {
    if (buffer_start == buffer_end) {
        return 0;  // Buffer empty
    }
    char c = keyboard_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

// Check if buffer has data
int keyboard_buffer_has_data() {
    return buffer_start != buffer_end;
}

// Keyboard interrupt handler
void keyboard_handler() {
    unsigned char scancode;
    
    // Read scancode from keyboard
    scancode = inb(KEYBOARD_DATA_PORT);
    
    // Check if key released (bit 7 set)
    if (scancode & 0x80) {
        // Key released - we ignore this for now
    } else {
        // Key pressed
        char c = keyboard_map[scancode];
        
        if (c) {
            // Add to buffer
            keyboard_buffer_add(c);
            
            // Echo character to screen
            char str[2];
            str[0] = c;
            str[1] = 0;
            print_string(str);
        }
    }
}

// Initialize keyboard
void keyboard_init() {
    // Keyboard initialization is done by BIOS
    // We just need to enable IRQ1
    print_string("[OK] Keyboard driver initialized\n");
}
#endif
