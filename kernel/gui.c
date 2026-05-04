// SUB OS - GUI / TUI Implementation (VGA Text Mode)
// Copyright (c) 2025-2026 SUB OS Project

#include "gui.h"
#include "kernel.h"
#include "timer.h"

#define VGA_BASE 0xB8000
#define COLS 80
#define ROWS 25

void gui_draw_char(int col, int row, char c, unsigned char color) {
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return;
    unsigned short *vga = (unsigned short *)VGA_BASE;
    vga[row * COLS + col] = (unsigned short)((color << 8) | (unsigned char)c);
}

void gui_draw_string(int col, int row, const char *s, unsigned char color) {
    for (int i = 0; s[i] && (col + i) < COLS; i++)
        gui_draw_char(col + i, row, s[i], color);
}

void gui_fill_rect(int col, int row, int w, int h, char c, unsigned char color) {
    for (int r = row; r < row + h && r < ROWS; r++)
        for (int ci = col; ci < col + w && ci < COLS; ci++)
            gui_draw_char(ci, r, c, color);
}

void gui_draw_box(int col, int row, int w, int h, unsigned char color) {
    gui_draw_char(col, row, '+', color);
    for (int i = 1; i < w - 1; i++) gui_draw_char(col + i, row, '-', color);
    gui_draw_char(col + w - 1, row, '+', color);
    gui_draw_char(col, row + h - 1, '+', color);
    for (int i = 1; i < w - 1; i++) gui_draw_char(col + i, row + h - 1, '-', color);
    gui_draw_char(col + w - 1, row + h - 1, '+', color);
    for (int r = 1; r < h - 1; r++) {
        gui_draw_char(col, row + r, '|', color);
        gui_draw_char(col + w - 1, row + r, '|', color);
    }
}

void gui_draw_banner() {
    unsigned short *vga = (unsigned short *)VGA_BASE;
    unsigned char bg = VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK);
    for (int i = 0; i < ROWS * COLS; i++)
        vga[i] = (unsigned short)((bg << 8) | ' ');

    unsigned char title_color = VGA_COLOR(VGA_WHITE, VGA_BLUE);
    gui_fill_rect(0, 0, COLS, 1, ' ', title_color);
    gui_draw_string(1, 0,
        "SUB OS v0.11.0  |  x86 32-bit Kernel  |  (c) 2026 SUB OS Project",
        title_color);

    unsigned char status_color = VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY);
    gui_fill_rect(0, 24, COLS, 1, ' ', status_color);
    gui_draw_string(1, 24, "[ help ] [ uptime ] [ meminfo ] [ gui ] [ halt ]", status_color);

    gui_fill_rect(0, 1, COLS, 23, ' ', bg);

    unsigned char box_color = VGA_COLOR(VGA_LIGHT_CYAN, VGA_BLACK);
    gui_draw_box(10, 2, 60, 8, box_color);
    gui_draw_string(12, 3, "Welcome to SUB OS!", VGA_COLOR(VGA_WHITE, VGA_BLACK));
    gui_draw_string(12, 4, "A lightweight 32-bit x86 operating system.", VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
    gui_draw_string(12, 5, "Type 'help' for available commands.", VGA_COLOR(VGA_LIGHT_GREEN, VGA_BLACK));
    gui_draw_string(12, 6, "Type 'gui'  to see the desktop demo.", VGA_COLOR(VGA_YELLOW, VGA_BLACK));
    gui_draw_string(12, 7, "Type 'halt' to stop the system.", VGA_COLOR(VGA_LIGHT_RED, VGA_BLACK));
    gui_draw_string(12, 8, "Test with: qemu-system-x86_64 -drive format=raw,file=build/sub_os.bin",
        VGA_COLOR(VGA_LIGHT_MAGENTA, VGA_BLACK));

    extern int cursor_row;
    extern int cursor_col;
    cursor_row = 11;
    cursor_col = 0;
}

void gui_draw_desktop() {
    unsigned short *vga = (unsigned short *)VGA_BASE;
    unsigned char desktop_bg = VGA_COLOR(VGA_LIGHT_GREY, VGA_BLUE);
    for (int i = 0; i < ROWS * COLS; i++)
        vga[i] = (unsigned short)((desktop_bg << 8) | ' ');

    unsigned char title = VGA_COLOR(VGA_WHITE, VGA_CYAN);
    gui_fill_rect(0, 0, COLS, 1, ' ', title);
    gui_draw_string(1, 0, "SUB OS Desktop  |  Taskbar", title);
    gui_draw_string(70, 0, "[ v0.11 ]", title);

    unsigned char w1 = VGA_COLOR(VGA_WHITE, VGA_BLACK);
    unsigned char w1t = VGA_COLOR(VGA_BLACK, VGA_LIGHT_BLUE);
    gui_fill_rect(2, 2, 36, 12, ' ', w1);
    gui_draw_box(2, 2, 36, 12, VGA_COLOR(VGA_LIGHT_CYAN, VGA_BLACK));
    gui_fill_rect(3, 2, 34, 1, ' ', w1t);
    gui_draw_string(3, 2, "[x]  File Manager", w1t);
    gui_draw_string(4, 3, "Name              Size    Type", VGA_COLOR(VGA_YELLOW, VGA_BLACK));
    gui_draw_string(4, 4, "----------------------------", VGA_COLOR(VGA_DARK_GREY, VGA_BLACK));
    gui_draw_string(4, 5, "/                   DIR", w1);
    gui_draw_string(4, 6, "kernel.bin    124 KB   BIN", w1);
    gui_draw_string(4, 7, "boot.bin        512 B   BIN", w1);
    gui_draw_string(4, 8, "README.txt       2 KB   TXT", w1);
    gui_draw_string(4, 10, "(VFS - empty)", VGA_COLOR(VGA_DARK_GREY, VGA_BLACK));

    unsigned char w2 = VGA_COLOR(VGA_WHITE, VGA_BLACK);
    unsigned char w2t = VGA_COLOR(VGA_BLACK, VGA_GREEN);
    gui_fill_rect(42, 2, 36, 10, ' ', w2);
    gui_draw_box(42, 2, 36, 10, VGA_COLOR(VGA_LIGHT_GREEN, VGA_BLACK));
    gui_fill_rect(43, 2, 34, 1, ' ', w2t);
    gui_draw_string(43, 2, "[x]  System Info", w2t);
    gui_draw_string(44, 3, "OS:     SUB OS v0.11.0", w2);
    gui_draw_string(44, 4, "Arch:   x86 32-bit", w2);
    gui_draw_string(44, 5, "Memory: 16 MB (detected)", w2);
    gui_draw_string(44, 6, "CPU:    QEMU/KVM x86", w2);
    gui_draw_string(44, 7, "Shell:  SUBsh v1.0", w2);
    gui_draw_string(44, 8, "FS:     SubFS (VFS)", w2);
    gui_draw_string(44, 9, "Boot:   BIOS/MBR", w2);

    unsigned char w3t = VGA_COLOR(VGA_BLACK, VGA_MAGENTA);
    gui_fill_rect(42, 14, 36, 8, ' ', VGA_COLOR(VGA_WHITE, VGA_BLACK));
    gui_draw_box(42, 14, 36, 8, VGA_COLOR(VGA_LIGHT_MAGENTA, VGA_BLACK));
    gui_fill_rect(43, 14, 34, 1, ' ', w3t);
    gui_draw_string(43, 14, "[x]  Terminal", w3t);
    gui_draw_string(44, 15, "subos@kernel:/ $ help", VGA_COLOR(VGA_LIGHT_GREEN, VGA_BLACK));
    gui_draw_string(44, 16, "  help, clear, echo,", VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
    gui_draw_string(44, 17, "  version, uptime,", VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
    gui_draw_string(44, 18, "  meminfo, ls, cat,", VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
    gui_draw_string(44, 19, "  gui, reboot, halt", VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));

    unsigned char tb = VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY);
    gui_fill_rect(0, 23, COLS, 1, ' ', tb);
    gui_draw_string(1, 23, "[ Start ]  [File Manager]  [Sys Info]  [Terminal]", tb);
    gui_draw_string(60, 23, "SUB OS 0.11", tb);

    gui_draw_string(0, 24, "  Press Enter or type any command to return to shell  ",
        VGA_COLOR(VGA_YELLOW, VGA_BLACK));

    extern int cursor_row;
    extern int cursor_col;
    cursor_row = 24;
    cursor_col = 0;
}
