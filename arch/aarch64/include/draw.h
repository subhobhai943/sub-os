#ifndef AARCH64_DRAW_H
#define AARCH64_DRAW_H

#include "types.h"

/* Color helpers: ARGB32 */
#define RGB(r,g,b)   (0xFF000000u | ((u32)(r)<<16) | ((u32)(g)<<8) | (u32)(b))
#define RGBA(r,g,b,a) (((u32)(a)<<24) | ((u32)(r)<<16) | ((u32)(g)<<8) | (u32)(b))

/* Common colors */
#define COL_BLACK    RGB(0,0,0)
#define COL_WHITE    RGB(255,255,255)
#define COL_RED      RGB(220,50,50)
#define COL_GREEN    RGB(50,200,80)
#define COL_BLUE     RGB(30,100,220)
#define COL_DARKBLUE RGB(10,30,80)
#define COL_CYAN     RGB(0,200,220)
#define COL_YELLOW   RGB(240,200,0)
#define COL_GRAY     RGB(160,160,160)
#define COL_DARKGRAY RGB(40,40,50)
#define COL_TASKBAR  RGB(18,18,30)
#define COL_WINBAR   RGB(30,60,140)
#define COL_WINBG    RGB(20,20,32)
#define COL_DESKTOP1 RGB(10,15,40)
#define COL_DESKTOP2 RGB(5,30,80)

void draw_pixel(int x, int y, u32 color);
void draw_hline(int x, int y, int w, u32 color);
void draw_vline(int x, int y, int h, u32 color);
void draw_rect(int x, int y, int w, int h, u32 color);
void draw_filled_rect(int x, int y, int w, int h, u32 color);
void draw_gradient_rect(int x, int y, int w, int h, u32 c1, u32 c2);
void draw_char(int x, int y, char c, u32 fg, u32 bg, int scale);
void draw_string(int x, int y, const char *s, u32 fg, u32 bg, int scale);

#endif
