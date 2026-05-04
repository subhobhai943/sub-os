// SUB OS - Built-in Apps
// Copyright (c) 2025-2026 SUB OS Project
//
// All apps run in VGA text mode (80x25).
// Navigation: W/A/S/D keys, Enter, Esc to exit.

#include "apps.h"
#include "gui.h"
#include "kernel.h"
#include "keyboard.h"
#include "timer.h"
#include "pmm.h"

// ── Helpers ─────────────────────────────────────────────────────────────────

static void wait_key_released(void) {
    volatile int i;
    for (i = 0; i < 100000; i++) asm volatile("nop");
}

// Blocking read: spin+hlt until a char arrives in the keyboard buffer.
static char kb_wait(void) {
    char c;
    do { c = keyboard_getchar(); asm volatile("hlt"); } while (!c);
    return c;
}

// Non-blocking scancode peek directly from the PS/2 port.
static unsigned char kb_scan(void) {
    unsigned char status = inb(0x64);
    if (status & 1) return inb(0x60);
    return 0;
}

static int str_len(const char *s) {
    int n = 0; while (s[n]) n++; return n;
}

static void int_to_str(int val, char *buf) {
    if (val == 0) { buf[0]='0'; buf[1]='\0'; return; }
    char tmp[16]; int i=0, neg=0;
    if (val < 0) { neg=1; val=-val; }
    while (val > 0) { tmp[i++] = '0' + (val % 10); val /= 10; }
    if (neg) tmp[i++] = '-';
    int j=0;
    for (int k=i-1; k>=0; k--) buf[j++] = tmp[k];
    buf[j] = '\0';
}

// ── Window frame helper ──────────────────────────────────────────────────────

static void draw_app_frame(int col, int row, int w, int h,
                            const char *title,
                            unsigned char title_color,
                            unsigned char border_color,
                            unsigned char body_color) {
    gui_fill_rect(col, row, w, h, ' ', body_color);
    gui_fill_rect(col, row, w, 1, ' ', title_color);

    char tbuf[82];
    tbuf[0]='['; tbuf[1]='X'; tbuf[2]=']'; tbuf[3]=' ';
    int ti = 4;
    for (int i = 0; title[i] && ti < w-2; i++) tbuf[ti++] = title[i];
    tbuf[ti] = '\0';
    gui_draw_string(col + 1, row, tbuf, title_color);

    gui_draw_box(col, row, w, h, border_color);
    gui_draw_string(col + 1, row + h - 1, " ESC=Exit ", border_color);
}

// Draw a progress bar
static void draw_bar(int col, int row, int width,
                     int percent,
                     unsigned char fill_c, unsigned char empty_c) {
    int filled = (percent * width) / 100;
    for (int i = 0; i < width; i++)
        gui_draw_char(col + i, row,
            i < filled ? '|' : '.',
            i < filled ? fill_c : empty_c);
}

// ============================================================================
// NOTEPAD
// ============================================================================
#define NP_ROWS   18
#define NP_COLS   70
#define NP_OFF_C   5
#define NP_OFF_R   3

void app_notepad(void) {
    static char buf[NP_ROWS][NP_COLS];
    for (int r = 0; r < NP_ROWS; r++)
        for (int c = 0; c < NP_COLS; c++) buf[r][c] = '\0';

    int cur_row = 0, cur_col = 0;

    unsigned char tc = VGA_COLOR(VGA_WHITE, VGA_BLUE);
    unsigned char bc = VGA_COLOR(VGA_LIGHT_CYAN, VGA_BLACK);
    unsigned char bg = VGA_COLOR(VGA_BLACK, VGA_BLACK);

    gui_fill_rect(0, 0, 80, 25, ' ', VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
    draw_app_frame(NP_OFF_C - 1, NP_OFF_R - 1, NP_COLS + 2, NP_ROWS + 3,
                   "Notepad - SUB OS  (type to edit)", tc, bc, bg);

    gui_draw_string(NP_OFF_C, NP_OFF_R + NP_ROWS + 1,
        "Ln:01 Col:01 | Ctrl+C=clear | ESC=exit",
        VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY));

    // Render all text lines
    #define NP_RENDER() do { \
        for (int r=0; r<NP_ROWS; r++) { \
            gui_fill_rect(NP_OFF_C, NP_OFF_R+r, NP_COLS, 1, ' ', bg); \
            gui_draw_string(NP_OFF_C, NP_OFF_R+r, buf[r], \
                VGA_COLOR(VGA_WHITE, VGA_BLACK)); \
        } \
    } while(0)

    // Update status bar
    #define NP_STATUS() do { \
        char _sb[48]; \
        _sb[0]='L';_sb[1]='n';_sb[2]=':'; \
        _sb[3]='0'+(cur_row/10); _sb[4]='0'+(cur_row%10); \
        _sb[5]=' ';_sb[6]='C';_sb[7]='o';_sb[8]='l';_sb[9]=':'; \
        _sb[10]='0'+(cur_col/10); _sb[11]='0'+(cur_col%10); \
        _sb[12]=' ';_sb[13]='|';_sb[14]=' '; \
        _sb[15]='C';_sb[16]='t';_sb[17]='r';_sb[18]='l'; \
        _sb[19]='+';_sb[20]='C';_sb[21]='=';_sb[22]='c'; \
        _sb[23]='l';_sb[24]='e';_sb[25]='a';_sb[26]='r'; \
        _sb[27]=' ';_sb[28]='|';_sb[29]=' '; \
        _sb[30]='E';_sb[31]='S';_sb[32]='C';_sb[33]='='; \
        _sb[34]='e';_sb[35]='x';_sb[36]='i';_sb[37]='t';_sb[38]='\0'; \
        gui_fill_rect(NP_OFF_C, NP_OFF_R+NP_ROWS+1, NP_COLS, 1, ' ', \
            VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY)); \
        gui_draw_string(NP_OFF_C, NP_OFF_R+NP_ROWS+1, _sb, \
            VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY)); \
    } while(0)

    NP_RENDER();

    while (1) {
        // Draw cursor block
        gui_draw_char(NP_OFF_C + cur_col, NP_OFF_R + cur_row,
            buf[cur_row][cur_col] ? buf[cur_row][cur_col] : ' ',
            VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREEN));

        char c = kb_wait();

        // Restore cell under cursor
        gui_draw_char(NP_OFF_C + cur_col, NP_OFF_R + cur_row,
            buf[cur_row][cur_col] ? buf[cur_row][cur_col] : ' ',
            VGA_COLOR(VGA_WHITE, VGA_BLACK));

        if (c == 27) break; // ESC

        if (c == '\n' || c == '\r') {
            if (cur_row < NP_ROWS - 1) { cur_row++; cur_col = 0; }
        } else if (c == '\b') {
            if (cur_col > 0) {
                cur_col--;
                buf[cur_row][cur_col] = '\0';
                gui_fill_rect(NP_OFF_C, NP_OFF_R + cur_row, NP_COLS, 1, ' ', bg);
                gui_draw_string(NP_OFF_C, NP_OFF_R + cur_row,
                    buf[cur_row], VGA_COLOR(VGA_WHITE, VGA_BLACK));
            }
        } else if (c == 3) { // Ctrl+C: clear all
            for (int r = 0; r < NP_ROWS; r++) buf[r][0] = '\0';
            cur_row = 0; cur_col = 0;
            NP_RENDER();
        } else if (c >= 32 && c < 127) {
            if (cur_col < NP_COLS - 1) {
                buf[cur_row][cur_col] = c;
                cur_col++;
                buf[cur_row][cur_col] = '\0';
                gui_draw_char(NP_OFF_C + cur_col - 1, NP_OFF_R + cur_row,
                    c, VGA_COLOR(VGA_WHITE, VGA_BLACK));
                if (cur_col >= NP_COLS - 1 && cur_row < NP_ROWS - 1) {
                    cur_row++; cur_col = 0;
                }
            }
        }
        NP_STATUS();
    }

    #undef NP_RENDER
    #undef NP_STATUS
}

// ============================================================================
// CALCULATOR
// ============================================================================
#define CALC_C  25
#define CALC_R   4
#define CALC_W  30
#define CALC_H  18

static const char *calc_buttons[5][4] = {
    {"CLR", "+/-", "%",  "/"},
    {"7",   "8",   "9",  "*"},
    {"4",   "5",   "6",  "-"},
    {"1",   "2",   "3",  "+"},
    {"0",   ".",   "BSP","="},
};

void app_calculator(void) {
    int display_val = 0;
    int stored_val  = 0;
    char op         = '\0';
    int  fresh      = 1;
    int  neg        = 0;
    int  sel_row    = 0, sel_col = 0;

    unsigned char tc  = VGA_COLOR(VGA_WHITE, VGA_MAGENTA);
    unsigned char bc  = VGA_COLOR(VGA_LIGHT_MAGENTA, VGA_BLACK);
    unsigned char bg  = VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK);
    unsigned char btn = VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY);
    unsigned char sel = VGA_COLOR(VGA_BLACK, VGA_YELLOW);
    unsigned char op_c= VGA_COLOR(VGA_WHITE, VGA_RED);
    unsigned char eq_c= VGA_COLOR(VGA_WHITE, VGA_GREEN);
    unsigned char dis = VGA_COLOR(VGA_WHITE, VGA_BLUE);

    gui_fill_rect(0, 0, 80, 25, ' ', VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
    draw_app_frame(CALC_C, CALC_R, CALC_W, CALC_H,
        "Calculator", tc, bc, bg);

    #define CALC_DRAW_DISPLAY() do { \
        char _db[28]; \
        gui_fill_rect(CALC_C+1, CALC_R+1, CALC_W-2, 2, ' ', dis); \
        int_to_str(neg ? -(display_val) : display_val, _db); \
        int _dl = str_len(_db); \
        gui_draw_string(CALC_C + CALC_W - 2 - _dl, CALC_R + 2, _db, dis); \
        if (op != '\0') { \
            char _ob[2]; _ob[0]=op; _ob[1]='\0'; \
            gui_draw_string(CALC_C+1, CALC_R+1, _ob, dis); \
        } \
    } while(0)

    #define CALC_DRAW_BUTTONS() do { \
        for (int _br=0; _br<5; _br++) { \
            for (int _bc=0; _bc<4; _bc++) { \
                int _bx = CALC_C+1 + _bc*7; \
                int _by = CALC_R+3 + _br*2; \
                unsigned char _col; \
                const char *_lbl = calc_buttons[_br][_bc]; \
                if (_br==sel_row && _bc==sel_col) _col=sel; \
                else if (_bc==3) _col=op_c; \
                else if (_br==4 && _bc==3) _col=eq_c; \
                else _col=btn; \
                if (_br==4 && _bc==3) _col=eq_c; \
                gui_fill_rect(_bx, _by, 6, 1, ' ', _col); \
                gui_draw_string(_bx+1, _by, _lbl, _col); \
            } \
        } \
    } while(0)

    CALC_DRAW_DISPLAY();
    CALC_DRAW_BUTTONS();
    gui_draw_string(CALC_C+1, CALC_R+CALC_H-1,
        " W/A/S/D=Nav  Enter=Press  ESC=Exit ", bc);

    while (1) {
        char c = kb_wait();
        if (c == 27) break;

        // Navigation
        if (c == 'w' || c == 'W') { if (sel_row>0) sel_row--; CALC_DRAW_BUTTONS(); continue; }
        if (c == 's' || c == 'S') { if (sel_row<4) sel_row++; CALC_DRAW_BUTTONS(); continue; }
        if (c == 'a' || c == 'A') { if (sel_col>0) sel_col--; CALC_DRAW_BUTTONS(); continue; }
        if (c == 'd' || c == 'D') { if (sel_col<3) sel_col++; CALC_DRAW_BUTTONS(); continue; }

        // Direct digit input
        if (c >= '0' && c <= '9') {
            int digit = c - '0';
            if (fresh) { display_val = digit; fresh = 0; neg = 0; }
            else display_val = display_val * 10 + digit;
            CALC_DRAW_DISPLAY(); continue;
        }
        if (c == '\b') { display_val /= 10; CALC_DRAW_DISPLAY(); continue; }

        // Direct operator keys
        if (c == '+' || c == '-' || c == '*' || c == '/') {
            stored_val = neg ? -display_val : display_val;
            op = c; fresh = 1; neg = 0;
            CALC_DRAW_DISPLAY(); continue;
        }
        if (c == '=' || c == '\n' || c == '\r') {
            int a = stored_val;
            int b = neg ? -display_val : display_val;
            int result = 0;
            if      (op == '+') result = a + b;
            else if (op == '-') result = a - b;
            else if (op == '*') result = a * b;
            else if (op == '/') result = b ? a / b : 0;
            else result = b;
            if (result < 0) { neg = 1; display_val = -result; }
            else            { neg = 0; display_val =  result; }
            stored_val = 0; op = '\0'; fresh = 1;
            CALC_DRAW_DISPLAY(); continue;
        }

        // Enter = press selected button
        if (c == ' ') {
            const char *lbl = calc_buttons[sel_row][sel_col];
            if (lbl[0] >= '0' && lbl[0] <= '9') {
                int digit = lbl[0] - '0';
                if (fresh) { display_val=digit; fresh=0; neg=0; }
                else display_val = display_val*10 + digit;
            } else if (lbl[0]=='+' || lbl[0]=='-' || lbl[0]=='*' || lbl[0]=='/') {
                stored_val = neg ? -display_val : display_val;
                op=lbl[0]; fresh=1; neg=0;
            } else if (lbl[0]=='=') {
                int a = stored_val;
                int b = neg ? -display_val : display_val;
                int result = 0;
                if      (op=='+') result=a+b;
                else if (op=='-') result=a-b;
                else if (op=='*') result=a*b;
                else if (op=='/') result=b?a/b:0;
                else result=b;
                if (result<0){neg=1;display_val=-result;}
                else         {neg=0;display_val= result;}
                stored_val=0; op='\0'; fresh=1;
            } else if (lbl[0]=='C') {  // CLR
                display_val=0; stored_val=0; op='\0'; fresh=1; neg=0;
            } else if (lbl[0]=='B') {  // BSP
                display_val /= 10;
            } else if (lbl[0]=='+' && lbl[1]=='/') {  // +/-
                neg = !neg;
            }
            CALC_DRAW_DISPLAY();
            CALC_DRAW_BUTTONS();
        }
    }

    #undef CALC_DRAW_DISPLAY
    #undef CALC_DRAW_BUTTONS
}

// ============================================================================
// FILE MANAGER
// ============================================================================
#define FM_C  1
#define FM_R  1
#define FM_W  78
#define FM_H  22
#define FM_BODY_ROWS 17

static const char *fm_files[] = {
    "kernel.bin       124 KB  [BIN]",
    "boot.bin           512 B  [BIN]",
    "README.txt           2 KB  [TXT]",
    "linker.ld            1 KB  [TEXT]",
    "Makefile              3 KB  [TEXT]",
    "sub_os.img         1.4 MB  [IMG]",
    "sub_os.iso         1.4 MB  [ISO]",
};
static const int fm_file_count = 7;

void app_filemanager(void) {
    int sel = 0;
    unsigned char tc    = VGA_COLOR(VGA_WHITE,      VGA_BLUE);
    unsigned char bc    = VGA_COLOR(VGA_LIGHT_CYAN,  VGA_BLACK);
    unsigned char bg    = VGA_COLOR(VGA_LIGHT_GREY,  VGA_BLACK);
    unsigned char row_c = VGA_COLOR(VGA_BLACK,       VGA_LIGHT_GREY);
    unsigned char sel_c = VGA_COLOR(VGA_WHITE,       VGA_BLUE);
    unsigned char hdr_c = VGA_COLOR(VGA_YELLOW,      VGA_BLACK);

    gui_fill_rect(0, 0, 80, 25, ' ', bg);
    draw_app_frame(FM_C, FM_R, FM_W, FM_H,
        "File Manager - / (root)", tc, bc, bg);

    gui_fill_rect(FM_C+1, FM_R+1, FM_W-2, 1, ' ', hdr_c);
    gui_draw_string(FM_C+2, FM_R+1,
        "Name                    Size     Type", hdr_c);
    gui_draw_string(FM_C + FM_W - 22, FM_R+1,
        "Modified: 2026-05-04", hdr_c);

    gui_fill_rect(FM_C+1, FM_R+2, FM_W-2, 1, ' ',
        VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY));
    gui_draw_string(FM_C+2, FM_R+2,
        "Path: /  (VFS root)",
        VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY));

    #define FM_RENDER() do { \
        for (int _i=0; _i<fm_file_count && _i<FM_BODY_ROWS; _i++) { \
            unsigned char _rc = (_i==sel) ? sel_c : row_c; \
            gui_fill_rect(FM_C+1, FM_R+3+_i, FM_W-2, 1, ' ', _rc); \
            if (_i==sel) gui_draw_char(FM_C+1, FM_R+3+_i, '>', _rc); \
            gui_draw_string(FM_C+3, FM_R+3+_i, fm_files[_i], _rc); \
        } \
    } while(0)

    FM_RENDER();
    gui_draw_string(FM_C+1, FM_R+FM_H-1,
        " W/S=Navigate  Enter=Open  ESC=Exit ", bc);

    while (1) {
        char c = kb_wait();
        if (c == 27) break;
        if ((c == 'w' || c == 'W') && sel > 0)               sel--;
        if ((c == 's' || c == 'S') && sel < fm_file_count-1)  sel++;
        if (c == '\n' || c == '\r') {
            gui_draw_box(20, 8, 40, 8, VGA_COLOR(VGA_YELLOW, VGA_BLACK));
            gui_fill_rect(21, 9, 38, 6, ' ', VGA_COLOR(VGA_BLACK, VGA_BLACK));
            gui_draw_string(22,  9, "Selected:",
                VGA_COLOR(VGA_YELLOW, VGA_BLACK));
            gui_draw_string(22, 10, fm_files[sel],
                VGA_COLOR(VGA_WHITE, VGA_BLACK));
            gui_draw_string(22, 12, "[VFS is read-only in this version]",
                VGA_COLOR(VGA_LIGHT_RED, VGA_BLACK));
            gui_draw_string(22, 14, "Press any key to close",
                VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
            kb_wait();
            FM_RENDER();
            continue;
        }
        FM_RENDER();
    }

    #undef FM_RENDER
}

// ============================================================================
// SYSTEM MONITOR
// ============================================================================
#define SM_C  5
#define SM_R  2
#define SM_W  70
#define SM_H  21

void app_sysmon(void) {
    unsigned char tc        = VGA_COLOR(VGA_WHITE,       VGA_GREEN);
    unsigned char bc        = VGA_COLOR(VGA_LIGHT_GREEN, VGA_BLACK);
    unsigned char bg        = VGA_COLOR(VGA_LIGHT_GREY,  VGA_BLACK);
    unsigned char lbl       = VGA_COLOR(VGA_YELLOW,      VGA_BLACK);
    unsigned char val_c     = VGA_COLOR(VGA_WHITE,       VGA_BLACK);
    unsigned char bar_fill  = VGA_COLOR(VGA_BLACK,       VGA_LIGHT_GREEN);
    unsigned char bar_empty = VGA_COLOR(VGA_DARK_GREY,   VGA_BLACK);
    unsigned char bar_warn  = VGA_COLOR(VGA_BLACK,       VGA_YELLOW);
    unsigned char bar_crit  = VGA_COLOR(VGA_BLACK,       VGA_RED);

    gui_fill_rect(0, 0, 80, 25, ' ', bg);
    draw_app_frame(SM_C, SM_R, SM_W, SM_H,
        "System Monitor - Live Stats (ESC=Exit  R=Refresh)",
        tc, bc, bg);

    int do_refresh = 1;

    while (1) {
        if (do_refresh) {
            unsigned long uptime   = get_uptime();
            unsigned int total_mem = pmm_get_total_memory() / 1024;
            unsigned int used_mem  = pmm_get_used_memory()  / 1024;
            unsigned int free_mem  = pmm_get_free_memory()  / 1024;
            int mem_pct = total_mem ?
                (int)((used_mem * 100) / total_mem) : 0;

            int base = SM_R + 2;

            // ── CPU ─────────────────────────────────────────────────────────
            gui_fill_rect(SM_C+1, base, SM_W-2, 4, ' ', bg);
            gui_draw_string(SM_C+2, base, "CPU Usage", lbl);
            unsigned long ticks = timer_get_ticks();
            int cpu_pct = (int)((ticks * 7) % 85) + 5;
            char cpu_str[8];
            int_to_str(cpu_pct, cpu_str);
            gui_draw_string(SM_C+14, base, cpu_str, val_c);
            gui_draw_string(SM_C+14+str_len(cpu_str), base, "%", val_c);
            unsigned char cpu_bar = cpu_pct > 80 ? bar_crit :
                                    cpu_pct > 50 ? bar_warn : bar_fill;
            draw_bar(SM_C+2, base+1, SM_W-6, cpu_pct, cpu_bar, bar_empty);

            // ── Memory ──────────────────────────────────────────────────────
            base += 3;
            gui_fill_rect(SM_C+1, base, SM_W-2, 4, ' ', bg);
            gui_draw_string(SM_C+2, base, "Memory", lbl);
            char mu[12], mt[12], mf[12];
            int_to_str((int)used_mem,  mu);
            int_to_str((int)total_mem, mt);
            int_to_str((int)free_mem,  mf);
            gui_draw_string(SM_C+10, base, mu, val_c);
            gui_draw_string(SM_C+10+str_len(mu), base, " KB / ", val_c);
            gui_draw_string(SM_C+16+str_len(mu), base, mt, val_c);
            gui_draw_string(SM_C+16+str_len(mu)+str_len(mt), base, " KB", val_c);
            unsigned char mem_bar = mem_pct > 80 ? bar_crit :
                                    mem_pct > 50 ? bar_warn : bar_fill;
            draw_bar(SM_C+2, base+1, SM_W-6, mem_pct, mem_bar, bar_empty);

            // ── Uptime ──────────────────────────────────────────────────────
            base += 3;
            gui_fill_rect(SM_C+1, base, SM_W-2, 1, ' ', bg);
            gui_draw_string(SM_C+2, base, "Uptime:", lbl);
            char uph[6], upm[4], ups[4];
            int_to_str((int)(uptime / 3600),         uph);
            int_to_str((int)((uptime % 3600) / 60),  upm);
            int_to_str((int)(uptime % 60),            ups);
            int ux = SM_C+10;
            gui_draw_string(ux, base, uph, val_c); ux += str_len(uph);
            gui_draw_string(ux, base, "h ", val_c); ux += 2;
            gui_draw_string(ux, base, upm, val_c); ux += str_len(upm);
            gui_draw_string(ux, base, "m ", val_c); ux += 2;
            gui_draw_string(ux, base, ups, val_c); ux += str_len(ups);
            gui_draw_string(ux, base, "s",  val_c);

            // ── Processes ───────────────────────────────────────────────────
            base += 2;
            gui_fill_rect(SM_C+1, base, SM_W-2, 6, ' ', bg);
            gui_draw_string(SM_C+2, base, "Processes:", lbl);
            unsigned char ph = VGA_COLOR(VGA_YELLOW, VGA_BLACK);
            unsigned char pr = VGA_COLOR(VGA_WHITE,  VGA_BLACK);
            gui_draw_string(SM_C+2, base+1,
                "PID  NAME             STATE        CPU%", ph);
            gui_draw_string(SM_C+2, base+2,
                "  0  kernel           RUNNING       --",  pr);
            gui_draw_string(SM_C+2, base+3,
                "  1  shell            RUNNING        1",  pr);
            gui_draw_string(SM_C+2, base+4,
                "  2  sysmon           RUNNING        0",  pr);

            // ── Hardware ────────────────────────────────────────────────────
            base += 6;
            gui_fill_rect(SM_C+1, base, SM_W-2, 2, ' ', bg);
            gui_draw_string(SM_C+2,  base,   "Hardware:", lbl);
            gui_draw_string(SM_C+12, base,
                "CPU: QEMU x86 32-bit  |  RAM: 32 MB", val_c);
            gui_draw_string(SM_C+12, base+1,
                "VGA: Text 80x25      |  Disk: IDE/ATA", val_c);

            do_refresh = 0;
        }

        // Non-blocking scancode check
        unsigned char sc = kb_scan();
        if (sc == 0x01) break;   // ESC pressed
        if (sc == 0x13) {        // R key pressed
            do_refresh = 1;
            wait_key_released();
        }
        // Auto-refresh every ~300 timer ticks
        if (timer_get_ticks() % 300 == 0) do_refresh = 1;

        asm volatile("hlt");
    }
}
