// SUB OS - GUI / TUI Header
// Copyright (c) 2025-2026 SUB OS Project

#ifndef GUI_H
#define GUI_H

// VGA text-mode colors
#define VGA_BLACK         0
#define VGA_BLUE          1
#define VGA_GREEN         2
#define VGA_CYAN          3
#define VGA_RED           4
#define VGA_MAGENTA       5
#define VGA_BROWN         6
#define VGA_LIGHT_GREY    7
#define VGA_DARK_GREY     8
#define VGA_LIGHT_BLUE    9
#define VGA_LIGHT_GREEN   10
#define VGA_LIGHT_CYAN    11
#define VGA_LIGHT_RED     12
#define VGA_LIGHT_MAGENTA 13
#define VGA_LIGHT_BROWN   14
#define VGA_WHITE         15

#define VGA_COLOR(fg, bg) ((bg << 4) | fg)

void gui_draw_char(int col, int row, char c, unsigned char color);
void gui_draw_string(int col, int row, const char *s, unsigned char color);
void gui_fill_rect(int col, int row, int w, int h, char c, unsigned char color);
void gui_draw_box(int col, int row, int w, int h, unsigned char color);
void gui_draw_banner();
void gui_draw_desktop();

#endif
