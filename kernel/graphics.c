// SUB OS - VGA Graphics Driver (Mode 13h)
// Copyright (c) 2026 SUB OS Project

#include "graphics.h"
#include "kernel.h"
#include "font.h"

// Pointer to video memory
static unsigned char* video_memory = (unsigned char*)VGA_MEMORY;
static unsigned char back_buffer[VGA_WIDTH * VGA_HEIGHT];

void graphics_init() {
    // Mode 13h is already set by bootloader
    // We just clear the screen
    graphics_clear(COLOR_BLACK);
    graphics_present();
}

void put_pixel(int x, int y, unsigned char color) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        back_buffer[y * VGA_WIDTH + x] = color;
    }
}

void draw_rect(int x, int y, int w, int h, unsigned char color) {
    for (int j = y; j < y + h; j++) {
        for (int i = x; i < x + w; i++) {
            put_pixel(i, j, color);
        }
    }
}

void draw_line(int x1, int y1, int x2, int y2, unsigned char color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        put_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void draw_char(char c, int x, int y, unsigned char color) {
    if (c < 32 || c > 126) c = '?';
    
    // Check bounds
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) return;
    
    const unsigned char* bitmap = font8x8_basic[c];
    
    for (int row = 0; row < 8; row++) {
        unsigned char line = bitmap[row];
        for (int col = 0; col < 8; col++) {
            if (line & (0x80 >> col)) {
                put_pixel(x + col, y + row, color);
            }
        }
    }
}

void draw_string(const char* str, int x, int y, unsigned char color) {
    int cur_x = x;
    int cur_y = y;
    
    while (*str) {
        if (*str == '\n') {
            cur_x = x;
            cur_y += 10;
        } else {
            draw_char(*str, cur_x, cur_y, color);
            cur_x += 8;
        }
        str++;
    }
}

void graphics_clear(unsigned char color) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        back_buffer[i] = color;
    }
}

void graphics_present() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video_memory[i] = back_buffer[i];
    }
}

void graphics_scroll_up(int pixels, unsigned char clear_color) {
    if (pixels <= 0) return;
    if (pixels >= VGA_HEIGHT) {
        graphics_clear(clear_color);
        return;
    }

    int row_stride = VGA_WIDTH;
    int shift = pixels * row_stride;
    int visible = (VGA_HEIGHT * row_stride) - shift;

    for (int i = 0; i < visible; i++) {
        back_buffer[i] = back_buffer[i + shift];
    }

    for (int i = visible; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        back_buffer[i] = clear_color;
    }
}

// Simple abs implementation for kernel space
int abs(int x) {
    return (x < 0) ? -x : x;
}

static unsigned char cursor_bg[64];
static int cursor_x = 0;
static int cursor_y = 0;
static int cursor_enabled = 0;

void graphics_draw_cursor(int x, int y) {
    // Restore old background if enabled
    if (cursor_enabled) {
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                if (cursor_x + c < VGA_WIDTH && cursor_y + r < VGA_HEIGHT) {
                    put_pixel(cursor_x + c, cursor_y + r, cursor_bg[r*8+c]);
                }
            }
        }
    }

    // Capture new background
    cursor_x = x;
    cursor_y = y;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (x + c < VGA_WIDTH && y + r < VGA_HEIGHT) {
                cursor_bg[r*8+c] = back_buffer[(y + r) * VGA_WIDTH + (x + c)];
            } else {
                cursor_bg[r*8+c] = 0;
            }
        }
    }

    // Draw new cursor (Simple pointer)
    // 1=White, 2=Black, 0=Transparent
    static const unsigned char mouse_pointer[8][8] = {
        {1,1,0,0,0,0,0,0},
        {1,2,1,0,0,0,0,0},
        {1,2,2,1,0,0,0,0},
        {1,2,2,2,1,0,0,0},
        {1,2,2,2,2,1,0,0},
        {1,2,2,1,1,0,0,0},
        {1,1,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0}
    };
    
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (x + c >= VGA_WIDTH || y + r >= VGA_HEIGHT) continue;
            
            unsigned char pixel = mouse_pointer[r][c];
            if (pixel == 1) put_pixel(x + c, y + r, COLOR_WHITE);
            else if (pixel == 2) put_pixel(x + c, y + r, COLOR_BLACK);
        }
    }
    
    cursor_enabled = 1;
}

void graphics_update_cursor(int x, int y) {
    graphics_draw_cursor(x, y);
    graphics_present();
}
