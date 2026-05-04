// SUB OS - Built-in Apps
// Copyright (c) 2025-2026 SUB OS Project
//
// All apps run in VGA text mode (80x25).
// Navigation: arrow keys (scan codes), Enter, Esc to exit.

#include "apps.h"
#include "gui.h"
#include "kernel.h"
#include "keyboard.h"
#include "timer.h"
#include "pmm.h"

// ── helpers ────────────────────────────────────────────────────────────────

static void wait_key_released(void) {
    // small drain to avoid stuck keys
    volatile int i;
    for (i = 0; i < 100000; i++) asm volatile("nop");
}

static char kb_wait(void) {
    char c;
    do { c = keyboard_getchar(); asm volatile("hlt"); } while (!c);
    return c;
}

// raw scancode peek (non-blocking)
static unsigned char kb_scan(void) {
    extern unsigned char inb(unsigned short);
    unsigned char status = inb(0x64);
    if (status & 1) return inb(0x60);
    return 0;
}

static int int_abs(int x) { return x < 0 ? -x : x; }

static void str_n_copy(char *dst, const char *src, int n) {
    int i = 0;
    while (i < n - 1 && src[i]) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
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
    for (int k=i-1;k>=0;k--) buf[j++]=tmp[k];
    buf[j]='\0';
}

// ── Window frame helper ─────────────────────────────────────────────────────

static void draw_app_frame(int col, int row, int w, int h,
                            const char *title,
                            unsigned char title_color,
                            unsigned char border_color,
                            unsigned char body_color) {
    // fill body
    gui_fill_rect(col, row, w, h, ' ', body_color);
    // title bar
    gui_fill_rect(col, row, w, 1, ' ', title_color);
    // write title centered-ish
    char tbuf[82];
    tbuf[0] = '['; tbuf[1] = 'X'; tbuf[2] = ']'; tbuf[3] = ' ';
    int ti = 4;
    for (int i = 0; title[i] && ti < w-2; i++) tbuf[ti++] = title[i];
    tbuf[ti] = '\0';
    gui_draw_string(col + 1, row, tbuf, title_color);
    // border
    gui_draw_box(col, row, w, h, border_color);
    // hint at bottom
    gui_draw_string(col + 1, row + h - 1,
        " ESC=Exit ", border_color);
}

// ══════════════════════════════════════════════════════════════════════════════
// NOTEPAD
// ══════════════════════════════════════════════════════════════════════════════
#define NP_ROWS   18
#define NP_COLS   70
#define NP_OFF_C   5
#define NP_OFF_R   3

void app_notepad(void) {
    // text buffer: NP_ROWS lines of NP_COLS chars
    static char buf[NP_ROWS][NP_COLS];
    for (int r = 0; r < NP_ROWS; r++)
        for (int c = 0; c < NP_COLS; c++) buf[r][c] = '\0';

    int cur_row = 0, cur_col = 0;

    // draw frame
    unsigned char tc = VGA_COLOR(VGA_WHITE, VGA_BLUE);
    unsigned char bc = VGA_COLOR(VGA_LIGHT_CYAN, VGA_BLACK);
    unsigned char bg = VGA_COLOR(VGA_BLACK, VGA_BLACK);

    // full screen backdrop
    gui_fill_rect(0, 0, 80, 25, ' ', VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
    draw_app_frame(NP_OFF_C - 1, NP_OFF_R - 1, NP_COLS + 2, NP_ROWS + 3,
                   "Notepad - SUB OS  (type to edit)", tc, bc, bg);

    // status bar
    gui_draw_string(NP_OFF_C, NP_OFF_R + NP_ROWS + 1,
        "Ln:01 Col:01 | Ctrl+C=clear | ESC=exit",
        VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY));

    // render all lines
    #define NP_RENDER() do { \
        for (int r=0;r<NP_ROWS;r++) { \
            gui_fill_rect(NP_OFF_C, NP_OFF_R+r, NP_COLS, 1, ' ', bg); \
            gui_draw_string(NP_OFF_C, NP_OFF_R+r, buf[r], VGA_COLOR(VGA_WHITE,VGA_BLACK)); \
        } \
    } while(0)

    #define NP_STATUS() do { \
        char sb[42]; \
        sb[0]='L';sb[1]='n';sb[2]=':'; \
        sb[3]='0'+(cur_row/10);sb[4]='0'+(cur_row%10); \
        sb[5]=' ';sb[6]='C';sb[7]='o';sb[8]='l';sb[9]=':'; \
        sb[10]='0'+(cur_col/10);sb[11]='0'+(cur_col%10); \
        sb[12]=' ';sb[13]='|';sb[14]=' ';sb[15]='C'; \
        sb[16]='t';sb[17]='r';sb[18]='l';sb[19]='+'; \
        sb[20]='C';sb[21]='=';sb[22]='c';sb[23]='l'; \
        sb[24]='e';sb[25]='a';sb[26]='r';sb[27]=' '; \
        sb[28]='|';sb[29]=' ';sb[30]='E';sb[31]='S'; \
        sb[32]='C';sb[33]='=';sb[34]='e';sb[35]='x'; \
        sb[36]='i';sb[37]='t';sb[38]='\0'; \
        gui_fill_rect(NP_OFF_C, NP_OFF_R+NP_ROWS+1, NP_COLS, 1, ' ', \
            VGA_COLOR(VGA_BLACK,VGA_LIGHT_GREY)); \
        gui_draw_string(NP_OFF_C, NP_OFF_R+NP_ROWS+1, sb, \
            VGA_COLOR(VGA_BLACK,VGA_LIGHT_GREY)); \
    } while(0)

    NP_RENDER();

    while (1) {
        // draw cursor
        gui_draw_char(NP_OFF_C + cur_col, NP_OFF_R + cur_row,
            buf[cur_row][cur_col] ? buf[cur_row][cur_col] : ' ',
            VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREEN));

        char c = kb_wait();

        // restore cursor cell
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
        } else if (c == 3) { // Ctrl+C = clear all
            for (int r=0;r<NP_ROWS;r++) buf[r][0]='\0';
            cur_row=0; cur_col=0;
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
}

// ══════════════════════════════════════════════════════════════════════════════
// CALCULATOR
// ══════════════════════════════════════════════════════════════════════════════

#define CALC_C  25
#define CALC_R   4
#define CALC_W  30
#define CALC_H  18

// Button layout: 4 cols x 5 rows of buttons + display
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
    int  fresh      = 1;   // next digit starts new number
    int  has_dot    = 0;   // simple: we'll treat as integer
    int  sel_row    = 0, sel_col = 0;
    int  neg        = 0;

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
        char dbuf[28]; \
        gui_fill_rect(CALC_C+1, CALC_R+1, CALC_W-2, 2, ' ', dis); \
        int_to_str(neg ? -(display_val) : display_val, dbuf); \
        int dlen = str_len(dbuf); \
        gui_draw_string(CALC_C + CALC_W - 2 - dlen, CALC_R + 2, dbuf, dis); \
        if (op != '\0') { \
            char ob[2]; ob[0]=op; ob[1]='\0'; \
            gui_draw_string(CALC_C+1, CALC_R+1, ob, dis); \
        } \
    } while(0)

    #define CALC_DRAW_BUTTONS() do { \
        for (int br=0; br<5; br++) { \
            for (int bc2=0; bc2<4; bc2++) { \
                int bx = CALC_C+1 + bc2*7; \
                int by = CALC_R+3 + br*2; \
                unsigned char col; \
                const char *label = calc_buttons[br][bc2]; \
                if (br==sel_row && bc2==sel_col) col=sel; \
                else if (bc2==3) col=op_c; \
                else if (br==4 && bc2==3) col=eq_c; \
                else col=btn; \
                if (br==4 && bc2==3) col=eq_c; \
                gui_fill_rect(bx, by, 6, 1, ' ', col); \
                gui_draw_string(bx+1, by, label, col); \
            } \
        } \
    } while(0)

    CALC_DRAW_DISPLAY();
    CALC_DRAW_BUTTONS();
    gui_draw_string(CALC_C+1, CALC_R+CALC_H-1,
        " Arrows=Nav  Enter=Press  ESC=Exit ", bc);

    while (1) {
        char c = kb_wait();
        if (c == 27) break;

        // arrow key handling via scan codes — keyboard_getchar returns
        // special codes for up=0x48, down=0x50, left=0x4B, right=0x4D
        // but our keyboard driver returns ASCII; use numeric chars for nav
        // We map: w=up, s=down, a=left, d=right for now
        int moved = 0;
        if (c == 'w' || c == 'W') { if(sel_row>0)sel_row--; moved=1; }
        if (c == 's' || c == 'S') { if(sel_row<4)sel_row++; moved=1; }
        if (c == 'a' || c == 'A') { if(sel_col>0)sel_col--; moved=1; }
        if (c == 'd' || c == 'D') { if(sel_col<3)sel_col++; moved=1; }

        // direct digit input
        if (c >= '0' && c <= '9') {
            int digit = c - '0';
            if (fresh) { display_val = digit; fresh = 0; neg = 0; }
            else display_val = display_val * 10 + digit;
            CALC_DRAW_DISPLAY(); continue;
        }
        if (c == '\b') {
            display_val /= 10;
            CALC_DRAW_DISPLAY(); continue;
        }

        // operator keys
        if (c == '+' || c == '-' || c == '*' || c == '/') {
            stored_val = display_val; op = c; fresh = 1;
            CALC_DRAW_DISPLAY(); continue;
        }
        if (c == '=' || c == '\n' || c == '\r') {
            int result = 0;
            if (op == '+') result = stored_val + display_val;
            else if (op == '-') result = stored_val - display_val;
            else if (op == '*') result = stored_val * display_val;
            else if (op == '/') result = display_val ? stored_val / display_val : 0;
            else result = display_val;
            display_val = result < 0 ? (neg=1, -result) : (neg=0, result);
            stored_val = 0; op = '\0'; fresh = 1;
            CALC_DRAW_DISPLAY(); continue;
        }

        if (moved) { CALC_DRAW_BUTTONS(); continue; }

        // Enter on selected button
        if (c == '\n' || c == '\r' || c == ' ') {
            const char *lbl = calc_buttons[sel_row][sel_col];
            if (lbl[0] >= '0' && lbl[0] <= '9') {
                int digit = lbl[0] - '0';
                if (fresh) { display_val=digit; fresh=0; neg=0; }
                else display_val = display_val*10 + digit;
            } else if (lbl[0]=='+' || lbl[0]=='-' || lbl[0]=='*' || lbl[0]=='/') {
                stored_val=display_val; op=lbl[0]; fresh=1;
            } else if (lbl[0]=='=') {
                int result=0;
                if (op=='+') result=stored_val+display_val;
                else if (op=='-') result=stored_val-display_val;
                else if (op=='*') result=stored_val*display_val;
                else if (op=='/') result=display_val?stored_val/display_val:0;
                else result=display_val;
                display_val=result<0?(neg=1,-result):(neg=0,result);
                stored_val=0;op='\0';fresh=1;
            } else if (lbl[0]=='C') { // CLR
                display_val=0;stored_val=0;op='\0';fresh=1;neg=0;
            } else if (lbl[0]=='B') { // BSP
                display_val/=10;
            } else if (lbl[0]=='+' && lbl[1]=='/') { // +/-
                neg = !neg;
            }
            CALC_DRAW_DISPLAY();
            CALC_DRAW_BUTTONS();
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// FILE MANAGER
// ══════════════════════════════════════════════════════════════════════════════

#define FM_C  1
#define FM_R  1
#define FM_W  78
#define FM_H  22
#define FM_BODY_ROWS 17

static const char *fm_files[] = {
    "kernel.bin      124 KB   [BIN]",
    "boot.bin          512 B   [BIN]",
    "README.txt          2 KB   [TXT]",
    "linker.ld           1 KB   [TEXT]",
    "Makefile             3 KB   [TEXT]",
    "sub_os.img        1.4 MB   [IMG]",
    "sub_os.iso        1.4 MB   [ISO]",
    0
};
static const int fm_file_count = 7;

void app_filemanager(void) {
    int sel = 0;
    unsigned char tc  = VGA_COLOR(VGA_WHITE, VGA_BLUE);
    unsigned char bc  = VGA_COLOR(VGA_LIGHT_CYAN, VGA_BLACK);
    unsigned char bg  = VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK);
    unsigned char row_c = VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY);
    unsigned char sel_c = VGA_COLOR(VGA_WHITE, VGA_BLUE);
    unsigned char hdr_c = VGA_COLOR(VGA_YELLOW, VGA_BLACK);

    gui_fill_rect(0, 0, 80, 25, ' ', bg);
    draw_app_frame(FM_C, FM_R, FM_W, FM_H,
        "File Manager - / (root)", tc, bc, bg);

    // column headers
    gui_fill_rect(FM_C+1, FM_R+1, FM_W-2, 1, ' ', hdr_c);
    gui_draw_string(FM_C+2, FM_R+1,
        "Name                    Size     Type", hdr_c);
    gui_draw_string(FM_C + FM_W - 20, FM_R+1,
        "Modified: 2026-05-04", hdr_c);

    // path bar
    gui_fill_rect(FM_C+1, FM_R+2, FM_W-2, 1, ' ',
        VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY));
    gui_draw_string(FM_C+2, FM_R+2,
        "Path: /  (VFS root)",
        VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREY));

    #define FM_RENDER() do { \
        for (int i=0; i<fm_file_count && i<FM_BODY_ROWS; i++) { \
            unsigned char rc = (i==sel) ? sel_c : row_c; \
            gui_fill_rect(FM_C+1, FM_R+3+i, FM_W-2, 1, ' ', rc); \
            if (i==sel) gui_draw_char(FM_C+1, FM_R+3+i, '>', rc); \
            gui_draw_string(FM_C+3, FM_R+3+i, fm_files[i], rc); \
        } \
    } while(0)

    FM_RENDER();

    gui_draw_string(FM_C+1, FM_R+FM_H-1,
        " W/S or UP/DN=Navigate  Enter=Open  ESC=Exit ", bc);

    while (1) {
        char c = kb_wait();
        if (c == 27) break;
        if ((c == 'w' || c == 'W') && sel > 0)             sel--;
        if ((c == 's' || c == 'S') && sel < fm_file_count-1) sel++;
        if (c == '\n' || c == '\r') {
            // show a popup about the file
            gui_draw_box(20, 8, 40, 8, VGA_COLOR(VGA_YELLOW, VGA_BLACK));
            gui_fill_rect(21, 9, 38, 6, ' ', VGA_COLOR(VGA_BLACK, VGA_BLACK));
            gui_draw_string(22, 9, "Selected:",
                VGA_COLOR(VGA_YELLOW, VGA_BLACK));
            gui_draw_string(22, 10, fm_files[sel],
                VGA_COLOR(VGA_WHITE, VGA_BLACK));
            gui_draw_string(22, 12,
                "[Cannot open - VFS is read-only]",
                VGA_COLOR(VGA_LIGHT_RED, VGA_BLACK));
            gui_draw_string(22, 14,
                "Press any key to close",
                VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
            kb_wait();
            FM_RENDER();
            continue;
        }
        FM_RENDER();
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// SYSTEM MONITOR
// ══════════════════════════════════════════════════════════════════════════════

#define SM_C  5
#define SM_R  2
#define SM_W  70
#define SM_H  21

static void draw_bar(int col, int row, int width,
                     int percent, unsigned char fill_c, unsigned char empty_c) {
    int filled = (percent * width) / 100;
    for (int i = 0; i < width; i++)
        gui_draw_char(col + i, row,
            i < filled ? '|' : '.',
            i < filled ? fill_c : empty_c);
}

void app_sysmon(void) {
    unsigned char tc  = VGA_COLOR(VGA_WHITE, VGA_GREEN);
    unsigned char bc  = VGA_COLOR(VGA_LIGHT_GREEN, VGA_BLACK);
    unsigned char bg  = VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK);
    unsigned char lbl = VGA_COLOR(VGA_YELLOW, VGA_BLACK);
    unsigned char val = VGA_COLOR(VGA_WHITE, VGA_BLACK);
    unsigned char bar_fill  = VGA_COLOR(VGA_BLACK, VGA_LIGHT_GREEN);
    unsigned char bar_empty = VGA_COLOR(VGA_DARK_GREY, VGA_BLACK);
    unsigned char bar_warn  = VGA_COLOR(VGA_BLACK, VGA_YELLOW);
    unsigned char bar_crit  = VGA_COLOR(VGA_BLACK, VGA_RED);

    gui_fill_rect(0, 0, 80, 25, ' ', bg);
    draw_app_frame(SM_C, SM_R, SM_W, SM_H,
        "System Monitor - Live Stats (ESC=Exit  R=Refresh)",
        tc, bc, bg);

    int refresh = 1;
    while (1) {
        if (refresh) {
            unsigned long uptime = get_uptime();
            unsigned int total_mem = pmm_get_total_memory() / 1024;
            unsigned int used_mem  = pmm_get_used_memory()  / 1024;
            unsigned int free_mem  = pmm_get_free_memory()  / 1024;
            int mem_pct = total_mem ? (int)((used_mem * 100) / total_mem) : 0;

            int base_row = SM_R + 2;

            // ── CPU ──────────────────────────────────────────────────────────
            gui_fill_rect(SM_C+1, base_row, SM_W-2, 4, ' ', bg);
            gui_draw_string(SM_C+2, base_row, "CPU Usage", lbl);
            // Simulated: ticks-based fake usage
            unsigned long ticks = timer_get_ticks();
            int cpu_pct = (int)((ticks * 7) % 85) + 5; // pseudo load
            char cpu_str[8];
            int_to_str(cpu_pct, cpu_str);
            gui_draw_string(SM_C+14, base_row, cpu_str, val);
            gui_draw_string(SM_C+14+str_len(cpu_str), base_row, "%", val);
            unsigned char cpu_bar_c = cpu_pct > 80 ? bar_crit :
                                      cpu_pct > 50 ? bar_warn : bar_fill;
            draw_bar(SM_C+2, base_row+1, SM_W-6, cpu_pct, cpu_bar_c, bar_empty);

            // ── Memory ───────────────────────────────────────────────────────
            base_row += 3;
            gui_fill_rect(SM_C+1, base_row, SM_W-2, 4, ' ', bg);
            gui_draw_string(SM_C+2, base_row, "Memory", lbl);
            char mu[10], mt[10], mf[10];
            int_to_str(used_mem, mu); int_to_str(total_mem, mt);
            int_to_str(free_mem, mf);
            gui_draw_string(SM_C+10, base_row, mu, val);
            gui_draw_string(SM_C+10+str_len(mu), base_row, " KB / ", val);
            gui_draw_string(SM_C+16+str_len(mu), base_row, mt, val);
            gui_draw_string(SM_C+16+str_len(mu)+str_len(mt), base_row, " KB", val);
            unsigned char mem_bar_c = mem_pct > 80 ? bar_crit :
                                      mem_pct > 50 ? bar_warn : bar_fill;
            draw_bar(SM_C+2, base_row+1, SM_W-6, mem_pct, mem_bar_c, bar_empty);

            // ── Uptime ───────────────────────────────────────────────────────
            base_row += 3;
            gui_fill_rect(SM_C+1, base_row, SM_W-2, 1, ' ', bg);
            gui_draw_string(SM_C+2, base_row, "Uptime:", lbl);
            char uph[4], upm[4], ups[4];
            int_to_str((int)(uptime/3600), uph);
            int_to_str((int)((uptime%3600)/60), upm);
            int_to_str((int)(uptime%60), ups);
            gui_draw_string(SM_C+10, base_row, uph, val);
            gui_draw_string(SM_C+10+str_len(uph), base_row, "h ", val);
            gui_draw_string(SM_C+12+str_len(uph), base_row, upm, val);
            gui_draw_string(SM_C+12+str_len(uph)+str_len(upm), base_row, "m ", val);
            gui_draw_string(SM_C+14+str_len(uph)+str_len(upm), base_row, ups, val);
            gui_draw_string(SM_C+14+str_len(uph)+str_len(upm)+str_len(ups), base_row, "s", val);

            // ── Processes ────────────────────────────────────────────────────
            base_row += 2;
            gui_fill_rect(SM_C+1, base_row, SM_W-2, 5, ' ', bg);
            gui_draw_string(SM_C+2, base_row, "Processes:", lbl);
            unsigned char ph = VGA_COLOR(VGA_YELLOW, VGA_BLACK);
            unsigned char pr = VGA_COLOR(VGA_WHITE,  VGA_BLACK);
            gui_draw_string(SM_C+2,  base_row+1, "PID  NAME             STATE        CPU%", ph);
            gui_draw_string(SM_C+2,  base_row+2, "  0  kernel           RUNNING       --", pr);
            gui_draw_string(SM_C+2,  base_row+3, "  1  shell            RUNNING        1", pr);
            gui_draw_string(SM_C+2,  base_row+4, "  2  sysmon           RUNNING        0", pr);

            // ── HW Info ──────────────────────────────────────────────────────
            base_row += 6;
            gui_fill_rect(SM_C+1, base_row, SM_W-2, 2, ' ', bg);
            gui_draw_string(SM_C+2,  base_row,   "Hardware:", lbl);
            gui_draw_string(SM_C+12, base_row,   "CPU: QEMU x86 32-bit  |  RAM: 32MB", val);
            gui_draw_string(SM_C+12, base_row+1, "VGA: Text 80x25  |  Disk: IDE/ATA", val);

            refresh = 0;
        }

        // non-blocking key check
        unsigned char sc = kb_scan();
        if (sc == 0x01) break;       // ESC scan code
        if (sc == 0x13 || sc == 0x13+0x80) { // R key
            refresh = 1;
            // small debounce drain
            wait_key_released();
        }
        // auto-refresh every few ticks
        if (timer_get_ticks() % 300 == 0) refresh = 1;

        asm volatile("hlt");
    }
}
