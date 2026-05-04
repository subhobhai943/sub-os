// SUB OS - GUI / TUI + Window Manager
// Copyright (c) 2025-2026 SUB OS Project

#include "gui.h"
#include "kernel.h"
#include "timer.h"
#include "apps.h"

#define VGA_BASE 0xB8000
#define COLS 80
#define ROWS 25

// ── primitives ──────────────────────────────────────────────────────────────

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
    gui_draw_char(col,       row,       '+', color);
    gui_draw_char(col+w-1,   row,       '+', color);
    gui_draw_char(col,       row+h-1,   '+', color);
    gui_draw_char(col+w-1,   row+h-1,   '+', color);
    for (int i = 1; i < w-1; i++) {
        gui_draw_char(col+i, row,     '-', color);
        gui_draw_char(col+i, row+h-1, '-', color);
    }
    for (int r = 1; r < h-1; r++) {
        gui_draw_char(col,     row+r, '|', color);
        gui_draw_char(col+w-1, row+r, '|', color);
    }
}

// ── boot banner ─────────────────────────────────────────────────────────────

void gui_draw_banner(void) {
    unsigned short *vga = (unsigned short *)VGA_BASE;
    unsigned char bg = VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK);
    for (int i = 0; i < ROWS * COLS; i++)
        vga[i] = (unsigned short)((bg << 8) | ' ');

    // title bar
    unsigned char tc = VGA_COLOR(VGA_WHITE, VGA_BLUE);
    gui_fill_rect(0, 0, COLS, 1, ' ', tc);
    gui_draw_string(1, 0,
        "SUB OS v0.11.0  |  x86 32-bit Kernel  |  (c) 2026 SUB OS Project", tc);

    // welcome box
    unsigned char bc = VGA_COLOR(VGA_LIGHT_CYAN, VGA_BLACK);
    gui_draw_box(10, 2, 60, 9, bc);
    gui_draw_string(12, 3, "Welcome to SUB OS!",
        VGA_COLOR(VGA_WHITE, VGA_BLACK));
    gui_draw_string(12, 4, "A lightweight 32-bit x86 operating system.",
        VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
    gui_draw_string(12, 5, "Type 'help'    for a list of commands.",
        VGA_COLOR(VGA_LIGHT_GREEN, VGA_BLACK));
    gui_draw_string(12, 6, "Type 'desktop' to open the full desktop.",
        VGA_COLOR(VGA_YELLOW, VGA_BLACK));
    gui_draw_string(12, 7, "Type 'notepad' to open the text editor.",
        VGA_COLOR(VGA_LIGHT_CYAN, VGA_BLACK));
    gui_draw_string(12, 8, "Type 'calc'    to open the calculator.",
        VGA_COLOR(VGA_LIGHT_MAGENTA, VGA_BLACK));
    gui_draw_string(12, 9, "Type 'halt'    to stop the system.",
        VGA_COLOR(VGA_LIGHT_RED, VGA_BLACK));

    // status bar
    unsigned char sc = VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY);
    gui_fill_rect(0, 24, COLS, 1, ' ', sc);
    gui_draw_string(1, 24,
        "[ help ] [ notepad ] [ calc ] [ files ] [ sysmon ] [ desktop ] [ halt ]", sc);

    extern int cursor_row;
    extern int cursor_col;
    cursor_row = 12;
    cursor_col = 0;
}

// ── desktop icon list ────────────────────────────────────────────────────────

typedef struct {
    int col, row;
    const char *icon;   // 3-char icon
    const char *label;
    unsigned char icon_c;
} desktop_icon_t;

static desktop_icon_t icons[] = {
    {  5, 6, "[N]", "Notepad",      VGA_COLOR(VGA_WHITE,  VGA_BLUE)   },
    { 20, 6, "[C]", "Calculator",   VGA_COLOR(VGA_WHITE,  VGA_MAGENTA)},
    { 35, 6, "[F]", "File Manager", VGA_COLOR(VGA_WHITE,  VGA_CYAN)   },
    { 50, 6, "[M]", "Sys Monitor",  VGA_COLOR(VGA_WHITE,  VGA_GREEN)  },
    { 65, 6, "[T]", "Terminal",     VGA_COLOR(VGA_WHITE,  VGA_RED)    },
};
#define ICON_COUNT 5

static void desktop_draw(int sel) {
    // wallpaper
    unsigned char wall = VGA_COLOR(VGA_LIGHT_GREY, VGA_BLUE);
    gui_fill_rect(0, 1, COLS, 22, ' ', wall);

    // SUB OS logo in center
    unsigned char logo_c = VGA_COLOR(VGA_WHITE, VGA_BLUE);
    gui_draw_string(28,  9, "  ___  _   _ ____    ___  ____  ", logo_c);
    gui_draw_string(28, 10, " / __|| | | | __ )  / _ \\/ ___| ", logo_c);
    gui_draw_string(28, 11, " \\__ \\| | | |  _ \ | | | \\___ \\ ", logo_c);
    gui_draw_string(28, 12, "  ___) | |_| | |_) || |_| |___) |", logo_c);
    gui_draw_string(28, 13, " |____/ \\___/|____/  \\___/|____/ ", logo_c);
    gui_draw_string(32, 15, "v0.11.0  |  x86 32-bit", VGA_COLOR(VGA_LIGHT_CYAN, VGA_BLUE));

    // Icons row
    for (int i = 0; i < ICON_COUNT; i++) {
        unsigned char ic = (i == sel) ?
            VGA_COLOR(VGA_BLACK, VGA_YELLOW) : icons[i].icon_c;
        gui_fill_rect(icons[i].col, icons[i].row, 12, 3, ' ',
            (i == sel) ? VGA_COLOR(VGA_BLACK, VGA_YELLOW) : wall);
        gui_draw_box(icons[i].col, icons[i].row, 12, 3,
            (i == sel) ? VGA_COLOR(VGA_BLACK, VGA_YELLOW) : icons[i].icon_c);
        gui_draw_string(icons[i].col + 4, icons[i].row + 1,
            icons[i].icon, ic);
        gui_draw_string(icons[i].col + 1, icons[i].row + 3,
            icons[i].label,
            (i == sel) ? VGA_COLOR(VGA_YELLOW, VGA_BLUE)
                       : VGA_COLOR(VGA_WHITE, VGA_BLUE));
    }
}

static void taskbar_draw(int sel) {
    unsigned char tc = VGA_COLOR(VGA_WHITE, VGA_CYAN);
    gui_fill_rect(0, 0, COLS, 1, ' ', tc);
    gui_draw_string(1, 0, "SUB OS v0.11.0", tc);

    // uptime
    unsigned long up = get_uptime();
    char ubuf[20];
    // simple format: "Up: Xh Ym Zs"
    ubuf[0]='U';ubuf[1]='p';ubuf[2]=':';ubuf[3]=' ';
    int idx=4;
    unsigned int h=(unsigned int)(up/3600);
    unsigned int m=(unsigned int)((up%3600)/60);
    unsigned int s=(unsigned int)(up%60);
    if(h>9){ubuf[idx++]='0'+h/10;}ubuf[idx++]='0'+h%10;
    ubuf[idx++]='h';ubuf[idx++]=' ';
    if(m>9){ubuf[idx++]='0'+m/10;}ubuf[idx++]='0'+m%10;
    ubuf[idx++]='m';ubuf[idx++]=' ';
    if(s>9){ubuf[idx++]='0'+s/10;}ubuf[idx++]='0'+s%10;
    ubuf[idx++]='s';ubuf[idx]='\0';
    gui_draw_string(18, 0, ubuf, tc);

    gui_draw_string(40, 0,
        "A=Left D=Right Enter=Open ESC=Shell", tc);

    // bottom taskbar
    unsigned char bc = VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY);
    gui_fill_rect(0, 24, COLS, 1, ' ', bc);
    const char *app_names[] = {"Notepad","Calc","Files","SysMon","Term"};
    int bx = 1;
    for (int i = 0; i < ICON_COUNT; i++) {
        unsigned char bc2 = (i == sel) ?
            VGA_COLOR(VGA_BLACK, VGA_YELLOW) :
            VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY);
        gui_draw_char(bx, 24, '[', bc2);
        for (int j = 0; app_names[i][j]; j++)
            gui_draw_char(bx+1+j, 24, app_names[i][j], bc2);
        // find length
        int len=0; while(app_names[i][len]) len++;
        gui_draw_char(bx+1+len, 24, ']', bc2);
        bx += len + 3;
    }
}

void gui_draw_desktop(void) {
    int sel = 0;
    taskbar_draw(sel);
    desktop_draw(sel);

    while (1) {
        char c = keyboard_getchar();
        asm volatile("hlt");

        if (!c) continue;

        if (c == 27) break; // ESC → back to shell

        if (c == 'a' || c == 'A') {
            if (sel > 0) sel--;
            taskbar_draw(sel);
            desktop_draw(sel);
        }
        if (c == 'd' || c == 'D') {
            if (sel < ICON_COUNT - 1) sel++;
            taskbar_draw(sel);
            desktop_draw(sel);
        }
        if (c == '\n' || c == '\r' || c == ' ') {
            // launch selected app
            switch (sel) {
                case 0: app_notepad();     break;
                case 1: app_calculator();  break;
                case 2: app_filemanager(); break;
                case 3: app_sysmon();      break;
                case 4:
                    // Terminal: just return to shell
                    return;
            }
            // redraw desktop after app exits
            taskbar_draw(sel);
            desktop_draw(sel);
        }
    }
}

// Window manager entry (alias of desktop)
void gui_wm_run(void) {
    gui_draw_desktop();
}
