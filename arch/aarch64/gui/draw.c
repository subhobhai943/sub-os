/* SUB OS - Drawing Primitives */

#include "../include/draw.h"
#include "../include/fb.h"
#include "../include/types.h"

extern const u8 font8x8[128][8];

void draw_pixel(int x, int y, u32 color) {
    if ((u32)x >= fb_width || (u32)y >= fb_height) return;
    fb_pixels[y * fb_width + x] = color;
}

void draw_hline(int x, int y, int w, u32 color) {
    int i;
    for (i = 0; i < w; i++) draw_pixel(x+i, y, color);
}

void draw_vline(int x, int y, int h, u32 color) {
    int i;
    for (i = 0; i < h; i++) draw_pixel(x, y+i, color);
}

void draw_rect(int x, int y, int w, int h, u32 color) {
    draw_hline(x, y, w, color);
    draw_hline(x, y+h-1, w, color);
    draw_vline(x, y, h, color);
    draw_vline(x+w-1, y, h, color);
}

void draw_filled_rect(int x, int y, int w, int h, u32 color) {
    int row;
    for (row = 0; row < h; row++) draw_hline(x, y+row, w, color);
}

void draw_gradient_rect(int x, int y, int w, int h, u32 c1, u32 c2) {
    int row;
    for (row = 0; row < h; row++) {
        u32 r1=(c1>>16)&0xff, g1=(c1>>8)&0xff, b1=c1&0xff;
        u32 r2=(c2>>16)&0xff, g2=(c2>>8)&0xff, b2=c2&0xff;
        u32 r = r1 + (u32)((int)(r2-r1) * row / h);
        u32 g = g1 + (u32)((int)(g2-g1) * row / h);
        u32 b = b1 + (u32)((int)(b2-b1) * row / h);
        draw_hline(x, y+row, w, RGB(r,g,b));
    }
}

void draw_char(int x, int y, char c, u32 fg, u32 bg, int scale) {
    int row, col, sy, sx;
    u8 idx = (u8)c;
    if (idx >= 128) idx = '?';
    for (row = 0; row < 8; row++) {
        u8 bits = font8x8[idx][row];
        for (col = 0; col < 8; col++) {
            u32 color = (bits & (0x80 >> col)) ? fg : bg;
            for (sy = 0; sy < scale; sy++)
                for (sx = 0; sx < scale; sx++)
                    draw_pixel(x + col*scale + sx, y + row*scale + sy, color);
        }
    }
}

void draw_string(int x, int y, const char *s, u32 fg, u32 bg, int scale) {
    int cx = x;
    while (*s) {
        if (*s == '\n') { y += 8*scale; cx = x; }
        else { draw_char(cx, y, *s, fg, bg, scale); cx += 8*scale; }
        s++;
    }
}
