#ifndef GRAPHICS_H
#define GRAPHICS_H

// VGA Mode 13h (320x200, 256 colors)
#define VGA_WIDTH 320
#define VGA_HEIGHT 200
#define VGA_MEMORY 0xA0000

// Colors
#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GRAY 7
#define COLOR_DARK_GRAY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_YELLOW 14
#define COLOR_WHITE 15

// Function prototypes
void graphics_init();
void put_pixel(int x, int y, unsigned char color);
void draw_rect(int x, int y, int w, int h, unsigned char color);
void draw_line(int x1, int y1, int x2, int y2, unsigned char color);
void draw_char(char c, int x, int y, unsigned char color);
void draw_string(const char* str, int x, int y, unsigned char color);
void graphics_clear(unsigned char color);
void graphics_present();
void graphics_scroll_up(int pixels, unsigned char clear_color);
void graphics_update_cursor(int x, int y);
void graphics_draw_cursor(int x, int y);

#endif
