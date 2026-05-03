/* SUB OS - Desktop GUI
 * Pixel-based desktop drawn to VirtIO GPU framebuffer.
 * Layout:
 *   - Gradient background
 *   - Bottom taskbar (32px)
 *   - Centered welcome window
 *   - System info window
 *   - Terminal window with live UART input
 */

#include "../include/draw.h"
#include "../include/fb.h"
#include "../include/uart.h"
#include "../include/timer.h"
#include "../include/desktop.h"
#include "../include/types.h"

/* ---- Tiny helpers (no libc) ---- */
static int str_len(const char *s) { int n=0; while(*s++) n++; return n; }
static void uint_to_str(u64 v, char *buf) {
    char tmp[21]; int i=0;
    if (!v) { buf[0]='0'; buf[1]=0; return; }
    while (v) { tmp[i++]='0'+(v%10); v/=10; }
    int j=0; while(i--) buf[j++]=tmp[i]; buf[j]=0;
}
static void hex_to_str(u64 v, char *buf) {
    char tmp[17]; int i=0;
    if (!v) { buf[0]='0'; buf[1]=0; return; }
    while (v) { int d=v&0xf; tmp[i++]=d<10?'0'+d:'a'+d-10; v>>=4; }
    int j=0; while(i--) buf[j++]=tmp[i]; buf[j]=0;
}
static void str_cpy(char *dst, const char *src) { while((*dst++=*src++)); }
static void str_cat(char *dst, const char *src) {
    while (*dst) dst++;
    while ((*dst++=*src++));
}

/* ---- Window drawing ---- */
#define WIN_TITLEBAR_H  20
#define WIN_BORDER      2

typedef struct {
    int x, y, w, h;
    const char *title;
} Window;

static void draw_window(Window *win) {
    /* Shadow */
    draw_filled_rect(win->x+4, win->y+4, win->w, win->h, RGB(0,0,0));
    /* Background */
    draw_filled_rect(win->x, win->y, win->w, win->h, COL_WINBG);
    /* Title bar gradient */
    draw_gradient_rect(win->x, win->y, win->w, WIN_TITLEBAR_H,
                       RGB(40,80,200), RGB(20,40,120));
    /* Title text */
    draw_string(win->x + 6, win->y + 6, win->title,
                COL_WHITE, RGB(0,0,0), 1);
    /* Close button */
    draw_filled_rect(win->x + win->w - 18, win->y + 3, 14, 14, RGB(180,40,40));
    draw_string(win->x + win->w - 15, win->y + 6, "X", COL_WHITE, RGB(180,40,40), 1);
    /* Minimise button */
    draw_filled_rect(win->x + win->w - 36, win->y + 3, 14, 14, RGB(200,160,0));
    draw_string(win->x + win->w - 33, win->y + 6, "-", COL_WHITE, RGB(200,160,0), 1);
    /* Border */
    draw_rect(win->x, win->y, win->w, win->h, RGB(60,90,200));
    /* Content separator */
    draw_hline(win->x, win->y + WIN_TITLEBAR_H, win->w, RGB(60,90,200));
}

/* ---- Taskbar ---- */
#define TASKBAR_Y    (600 - 32)
#define TASKBAR_H    32

static void draw_taskbar(u64 ticks) {
    char tbuf[32];
    u64 secs = ticks / 100;
    u64 mm   = (secs / 60) % 60;
    u64 ss   = secs % 60;
    char ts[16] = "00:00";

    /* Build time string manually */
    ts[0] = '0' + (mm/10); ts[1] = '0' + (mm%10);
    ts[3] = '0' + (ss/10); ts[4] = '0' + (ss%10);

    draw_gradient_rect(0, TASKBAR_Y, 800, TASKBAR_H,
                       RGB(15,15,35), RGB(25,25,60));
    draw_hline(0, TASKBAR_Y, 800, RGB(60,80,180));

    /* Start button */
    draw_gradient_rect(4, TASKBAR_Y+4, 80, 24,
                       RGB(30,100,200), RGB(10,60,140));
    draw_rect(4, TASKBAR_Y+4, 80, 24, RGB(80,140,255));
    draw_string(14, TASKBAR_Y+10, "SUB OS", COL_WHITE, RGB(0,0,0), 1);

    /* Clock (right side) */
    draw_string(800 - 60, TASKBAR_Y + 10, ts, COL_CYAN, COL_TASKBAR, 1);

    /* Uptime label */
    str_cpy(tbuf, "Up:");
    uint_to_str(secs, tbuf+3);
    str_cat(tbuf, "s");
    draw_string(800 - 130, TASKBAR_Y + 10, tbuf, COL_GRAY, COL_TASKBAR, 1);

    (void)hex_to_str; (void)str_len;
}

/* ---- Terminal window state ---- */
#define TERM_COLS  50
#define TERM_ROWS  14
#define TERM_CHAR_W 8
#define TERM_CHAR_H 9

typedef struct {
    char buf[TERM_ROWS][TERM_COLS+1];
    int  row, col;
} TermBuf;

static TermBuf term;

static void term_newline(void) {
    int r;
    if (term.row < TERM_ROWS - 1) {
        term.row++;
    } else {
        /* Scroll up */
        for (r = 0; r < TERM_ROWS-1; r++) {
            int c;
            for (c = 0; c <= TERM_COLS; c++)
                term.buf[r][c] = term.buf[r+1][c];
        }
        /* Clear last row */
        int c;
        for (c = 0; c <= TERM_COLS; c++)
            term.buf[TERM_ROWS-1][c] = 0;
    }
    term.col = 0;
}

static void term_putc(char c) {
    if (c == '\n' || c == '\r') { term_newline(); return; }
    if (c == '\b' || c == 127) {
        if (term.col > 0) { term.col--; term.buf[term.row][term.col] = 0; }
        return;
    }
    if (term.col >= TERM_COLS) term_newline();
    term.buf[term.row][term.col] = c;
    term.col++;
}

static void term_puts(const char *s) {
    while (*s) term_putc(*s++);
}

static void draw_terminal_content(int wx, int wy) {
    int r;
    int cx = wx + 4;
    int cy = wy + WIN_TITLEBAR_H + 4;
    for (r = 0; r < TERM_ROWS; r++) {
        draw_string(cx, cy + r*TERM_CHAR_H, term.buf[r],
                    COL_GREEN, COL_WINBG, 1);
    }
    /* Cursor */
    draw_filled_rect(cx + term.col*TERM_CHAR_W,
                     cy + term.row*TERM_CHAR_H + 7,
                     6, 2, COL_GREEN);
}

/* ---- Sysinfo window ---- */
static void draw_sysinfo_content(int wx, int wy, u64 ticks) {
    char buf[32];
    int cx = wx + 8;
    int cy = wy + WIN_TITLEBAR_H + 8;
    int ls = 12;  /* line spacing */
    draw_string(cx, cy,       "OS   : SUB OS v0.1",      COL_CYAN,   COL_WINBG, 1); cy+=ls;
    draw_string(cx, cy,       "ARCH : AArch64 ARMv8-A",  COL_CYAN,   COL_WINBG, 1); cy+=ls;
    draw_string(cx, cy,       "MACH : QEMU virt GIC-v2", COL_CYAN,   COL_WINBG, 1); cy+=ls;
    draw_string(cx, cy,       "CPU  : Cortex-A53 (max)", COL_CYAN,   COL_WINBG, 1); cy+=ls;
    draw_string(cx, cy,       "RAM  : 256 MB",            COL_CYAN,   COL_WINBG, 1); cy+=ls;
    draw_string(cx, cy,       "GPU  : VirtIO GPU",        COL_CYAN,   COL_WINBG, 1); cy+=ls;
    draw_string(cx, cy,       "RES  : 800x600 BGRX8888", COL_CYAN,   COL_WINBG, 1); cy+=ls;
    draw_string(cx, cy,       "MMU  : disabled",          COL_YELLOW, COL_WINBG, 1); cy+=ls;
    /* Uptime */
    str_cpy(buf, "UP   : ");
    uint_to_str(ticks/100, buf+7);
    str_cat(buf, "s");
    draw_string(cx, cy, buf, COL_GREEN, COL_WINBG, 1);
}

/* ---- Shell input in terminal window ---- */
#define SHELL_LINE_MAX 48

static char shell_line[SHELL_LINE_MAX+1];
static int  shell_len = 0;

static void shell_dispatch(const char *cmd);

static void shell_prompt(void) {
    term_puts("\n[SUB-OS]$ ");
}

static void gui_handle_key(char c) {
    /* Echo to UART too */
    uart_putc(c);

    if (c == '\r' || c == '\n') {
        shell_line[shell_len] = 0;
        term_putc('\n');
        if (shell_len > 0) shell_dispatch(shell_line);
        shell_len = 0;
        shell_prompt();
    } else if ((c == 127 || c == '\b') && shell_len > 0) {
        shell_len--;
        term_putc('\b');
    } else if (c >= 32 && shell_len < SHELL_LINE_MAX) {
        shell_line[shell_len++] = c;
        term_putc(c);
    }
}

/* Shell commands inside GUI */
static int gc_strcmp(const char *a, const char *b) {
    while (*a && *a==*b){a++;b++;}
    return (unsigned char)*a-(unsigned char)*b;
}
static int gc_strncmp(const char *a,const char *b,int n){
    while(n--&&*a&&*a==*b){a++;b++;}
    return n<0?0:(unsigned char)*a-(unsigned char)*b;
}

static void shell_dispatch(const char *cmd) {
    while (*cmd==' ') cmd++;
    if (!*cmd) return;

    if (gc_strcmp(cmd,"help")==0) {
        term_puts("Commands: help ver uname uptime meminfo clear echo <t>");
        return;
    }
    if (gc_strcmp(cmd,"ver")==0) {
        term_puts("SUB OS v0.1-aarch64  by Subhadip Sarkar");
        return;
    }
    if (gc_strcmp(cmd,"uname")==0) {
        term_puts("SUB-OS 0.1 aarch64 QEMU-virt");
        return;
    }
    if (gc_strcmp(cmd,"uptime")==0) {
        char buf[24];
        uint_to_str(timer_get_ticks()/100, buf);
        term_puts("Uptime: "); term_puts(buf); term_puts("s");
        return;
    }
    if (gc_strcmp(cmd,"meminfo")==0) {
        term_puts("Kernel:0x40080000 RAM:256MB MMU:off");
        return;
    }
    if (gc_strcmp(cmd,"clear")==0) {
        int r,c;
        for(r=0;r<TERM_ROWS;r++) for(c=0;c<=TERM_COLS;c++) term.buf[r][c]=0;
        term.row=0; term.col=0;
        return;
    }
    if (gc_strncmp(cmd,"echo ",5)==0) { term_puts(cmd+5); return; }
    if (gc_strcmp(cmd,"echo")==0) return;
    term_puts("unknown: "); term_puts(cmd);
}

/* ---- Main GUI loop ---- */
void gui_main(void) {
    int ret;
    u64 last_tick = 0;
    char kc;

    /* Try framebuffer init */
    ret = fb_init();
    if (ret != 0) {
        /* Fall back to UART shell */
        uart_puts("[WARN] VirtIO GPU not found (err=");
        char eb[4]; eb[0]='0'+((-ret)%10); eb[1]=0;
        uart_puts(eb);
        uart_puts("). Falling back to UART shell.\r\n");
        /* Import shell_run from shell.c */
        extern void shell_run(void);
        shell_run();
        return;
    }

    uart_puts("[OK] VirtIO GPU framebuffer ready (800x600)\r\n");

    /* Initial desktop draw */
    draw_gradient_rect(0, 0, 800, TASKBAR_Y,
                       COL_DESKTOP1, COL_DESKTOP2);

    /* Stars / dots on desktop */
    {
        u32 i;
        u64 s = 0x12345;
        for (i = 0; i < 120; i++) {
            s = s * 6364136223846793005ULL + 1442695040888963407ULL;
            int sx = (int)((s >> 33) % 800);
            int sy = (int)((s >> 17) % (u64)TASKBAR_Y);
            draw_pixel(sx, sy, RGB(200+((s>>8)%55), 200+((s>>16)%55), 200+((s>>24)%55)));
        }
    }

    /* Draw SUB OS logo text large on desktop */
    draw_string(200, 30, "SUB", RGB(80,160,255), COL_DESKTOP1, 4);
    draw_string(392, 30, "OS",  RGB(0,220,120),  COL_DESKTOP1, 4);
    draw_string(248, 70, "v0.1-aarch64", COL_GRAY, COL_DESKTOP1, 1);

    /* Windows */
    Window win_term  = {10,  120, 440, 180, "Terminal"};
    Window win_info  = {460, 120, 330, 175, "System Info"};

    draw_window(&win_term);
    draw_window(&win_info);

    /* Terminal init */
    int r, c;
    for (r=0;r<TERM_ROWS;r++) for(c=0;c<=TERM_COLS;c++) term.buf[r][c]=0;
    term.row=0; term.col=0;
    term_puts("SUB OS GUI Shell ready.");
    shell_prompt();

    draw_terminal_content(win_term.x, win_term.y);
    draw_sysinfo_content(win_info.x, win_info.y, timer_get_ticks());
    draw_taskbar(timer_get_ticks());
    fb_flush();

    /* Main loop */
    for (;;) {
        u64 now = timer_get_ticks();
        int dirty = 0;

        /* Handle keyboard input (non-blocking) */
        if (uart_getc_nonblock(&kc)) {
            gui_handle_key(kc);
            dirty = 1;
        }

        /* Refresh every ~10 ticks (100ms) */
        if (now - last_tick >= 10) {
            last_tick = now;
            dirty = 1;
        }

        if (dirty) {
            /* Redraw desktop (partial - just windows + taskbar) */
            draw_window(&win_term);
            draw_terminal_content(win_term.x, win_term.y);
            draw_window(&win_info);
            draw_sysinfo_content(win_info.x, win_info.y, now);
            draw_taskbar(now);
            fb_flush();
        } else {
            __asm__ volatile("wfe");
        }
    }
}
