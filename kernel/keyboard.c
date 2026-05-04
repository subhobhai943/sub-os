// SUB OS - Keyboard Driver
// Copyright (c) 2025 SUB OS Project

#include "keyboard.h"
#include "kernel.h"

#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

// US QWERTY scancode -> ASCII map
unsigned char keyboard_map[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,          // Left Ctrl
    'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,          // Left Shift
    '\\','z','x','c','v','b','n','m',',','.','/',
    0,          // Right Shift
    '*',
    0,          // Left Alt
    ' ',        // Space
    0,          // Caps Lock
    0,0,0,0,0,0,0,0,0,0,  // F1-F10
    0,          // Num Lock
    0,          // Scroll Lock
    0,          // Home
    0,          // Up Arrow
    0,          // Page Up
    '-',
    0,          // Left Arrow
    0,
    0,          // Right Arrow
    '+',
    0,          // End
    0,          // Down Arrow
    0,          // Page Down
    0,          // Insert
    0,          // Delete
    0,0,0,
    0,          // F11
    0,          // F12
    0
};

// ── Circular keyboard ring buffer ──────────────────────────────────────────
#define KB_BUF_SIZE 256
static char  kb_buf[KB_BUF_SIZE];
static int   kb_head = 0;   // read  pointer
static int   kb_tail = 0;   // write pointer

static void kb_buf_push(char c) {
    int next = (kb_tail + 1) % KB_BUF_SIZE;
    if (next != kb_head) {          // drop if full
        kb_buf[kb_tail] = c;
        kb_tail = next;
    }
}

// Returns 0 if buffer is empty, otherwise pops and returns the char.
char keyboard_getchar(void) {
    if (kb_head == kb_tail) return 0;
    char c = kb_buf[kb_head];
    kb_head = (kb_head + 1) % KB_BUF_SIZE;
    return c;
}

// ── IRQ1 handler (called from IDT) ─────────────────────────────────────────
void keyboard_handler(void) {
    unsigned char sc = inb(KEYBOARD_DATA_PORT);
    if (sc & 0x80) return;   // key-release: ignore
    char c = keyboard_map[sc & 0x7F];
    if (c) kb_buf_push(c);
}

// ── Init ────────────────────────────────────────────────────────────────────
void keyboard_init(void) {
    kb_head = kb_tail = 0;
    print_string("[OK] Keyboard driver initialized\n");
}
