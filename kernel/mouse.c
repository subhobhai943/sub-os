#include "mouse.h"
#include "kernel.h"
#include "graphics.h"

#define MOUSE_PORT_DATA    0x60
#define MOUSE_PORT_STATUS  0x64
#define MOUSE_PORT_CMD     0x64

static unsigned char mouse_cycle = 0;
static char mouse_byte[3];
static int mouse_x = 0;
static int mouse_y = 0;
static int mouse_buttons = 0;

void mouse_wait(unsigned char type) {
    unsigned int time_out = 100000;
    if (type == 0) {
        while (time_out--) {
            if ((inb(MOUSE_PORT_STATUS) & 1) == 1) {
                return;
            }
        }
        return;
    } else {
        while (time_out--) {
            if ((inb(MOUSE_PORT_STATUS) & 2) == 0) {
                return;
            }
        }
        return;
    }
}

void mouse_write(unsigned char write) {
    mouse_wait(1);
    outb(MOUSE_PORT_CMD, 0xD4);
    mouse_wait(1);
    outb(MOUSE_PORT_DATA, write);
}

unsigned char mouse_read() {
    mouse_wait(0);
    return inb(MOUSE_PORT_DATA);
}

void mouse_handler() {
    unsigned char status = inb(MOUSE_PORT_STATUS);
    // Check if buffer is full and if it's from auxiliary device (bit 5)
    if ((status & 0x01) && (status & 0x20)) {
        mouse_byte[mouse_cycle++] = inb(MOUSE_PORT_DATA);

        if (mouse_cycle == 3) {
            mouse_cycle = 0;

            unsigned char flags = mouse_byte[0];
            int dx = (char)mouse_byte[1];
            int dy = (char)mouse_byte[2];

            // Handle overflows (optional, basic driver might skip)
            if (flags & 0xC0) return;

            mouse_x += dx;
            mouse_y -= dy; // PS/2 Y is inverted (up is positive)

            // Clamp to screen
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x >= VGA_WIDTH) mouse_x = VGA_WIDTH - 1;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y >= VGA_HEIGHT) mouse_y = VGA_HEIGHT - 1;

            mouse_buttons = flags & 0x07;

            // Draw cursor
            graphics_update_cursor(mouse_x, mouse_y);
        }
    } else { 
        // Not mouse data, might be keyboard? 
        // Just read port 0x60 to clear it if it was signalled to us?
        // But interrupt 12 should only fire for mouse.
        // We read anyway to clear the buffer.
        // (inb(MOUSE_PORT_DATA));
    }
}

void mouse_init() {
    unsigned char status;

    print_string("[OK] Initializing PS/2 Mouse...\n");

    // Enable Auxiliary Device
    mouse_wait(1);
    outb(MOUSE_PORT_CMD, 0xA8);

    // Get Compaq Status Byte
    mouse_wait(1);
    outb(MOUSE_PORT_CMD, 0x20);
    mouse_wait(0);
    status = (inb(MOUSE_PORT_DATA) | 2); // Enable IRQ12
    mouse_wait(1);
    outb(MOUSE_PORT_CMD, 0x60);
    mouse_wait(1);
    outb(MOUSE_PORT_DATA, status);

    // Set defaults
    mouse_write(0xF6);
    mouse_read(); // Ack

    // Enable Streaming
    mouse_write(0xF4);
    mouse_read(); // Ack
    
    // Set initial position center
    mouse_x = VGA_WIDTH / 2;
    mouse_y = VGA_HEIGHT / 2;
    graphics_update_cursor(mouse_x, mouse_y);

    print_string("[OK] Mouse Driver initialized\n");
}

int mouse_get_x() { return mouse_x; }
int mouse_get_y() { return mouse_y; }
int mouse_get_buttons() { return mouse_buttons; }
