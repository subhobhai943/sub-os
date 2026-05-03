/* SUB OS - AArch64 Interactive UART Shell
 * Provides a minimal command-line interface over the PL011 UART.
 */

#include "../include/uart.h"
#include "../include/timer.h"
#include "../include/shell.h"

/* ------------------------------------------------------------------ */
/* Tiny string helpers (no libc)                                        */
/* ------------------------------------------------------------------ */
static int sh_strlen(const char *s) {
    int n = 0; while (*s++) n++; return n;
}
static int sh_strcmp(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}
static int sh_strncmp(const char *a, const char *b, int n) {
    while (n-- && *a && *a == *b) { a++; b++; }
    return n < 0 ? 0 : (unsigned char)*a - (unsigned char)*b;
}
static void sh_itoa(unsigned long v, char *buf, int base) {
    char tmp[32]; int i = 0;
    if (v == 0) { buf[0]='0'; buf[1]=0; return; }
    while (v) {
        int d = v % base;
        tmp[i++] = d < 10 ? '0'+d : 'a'+(d-10);
        v /= base;
    }
    int j = 0;
    while (i--) buf[j++] = tmp[i];
    buf[j] = 0;
}

/* ------------------------------------------------------------------ */
/* ASCII art banner                                                     */
/* ------------------------------------------------------------------ */
static void print_banner(void) {
    uart_puts("\r\n");
    uart_puts("  ███████╗██╗   ██╗██████╗      ██████╗ ███████╗\r\n");
    uart_puts("  ██╔════╝██║   ██║██╔══██╗    ██╔═══██╗██╔════╝\r\n");
    uart_puts("  ███████╗██║   ██║██████╔╝    ██║   ██║███████╗\r\n");
    uart_puts("  ╚════██║██║   ██║██╔══██╗    ██║   ██║╚════██║\r\n");
    uart_puts("  ███████║╚██████╔╝██████╔╝    ╚██████╔╝███████║\r\n");
    uart_puts("  ╚══════╝ ╚═════╝ ╚═════╝      ╚═════╝ ╚══════╝\r\n");
    uart_puts("\r\n");
    uart_puts("  ┌─────────────────────────────────────────────┐\r\n");
    uart_puts("  │  SUB OS  v0.1-aarch64   (QEMU virt / A53)  │\r\n");
    uart_puts("  │  Kernel by Subhadip Sarkar                  │\r\n");
    uart_puts("  │  Type 'help' for available commands         │\r\n");
    uart_puts("  └─────────────────────────────────────────────┘\r\n");
    uart_puts("\r\n");
}

/* ------------------------------------------------------------------ */
/* Command implementations                                              */
/* ------------------------------------------------------------------ */
static void cmd_help(void) {
    uart_puts("\r\n");
    uart_puts("  ┌──────────────────────────────────────────────────┐\r\n");
    uart_puts("  │               Available Commands                 │\r\n");
    uart_puts("  ├──────────────┬───────────────────────────────────┤\r\n");
    uart_puts("  │ help         │ Show this help message             │\r\n");
    uart_puts("  │ ver          │ Show OS version                    │\r\n");
    uart_puts("  │ uname        │ System information                 │\r\n");
    uart_puts("  │ uptime       │ Time since boot (ticks)            │\r\n");
    uart_puts("  │ meminfo      │ Basic memory layout                │\r\n");
    uart_puts("  │ echo <text>  │ Print text to screen               │\r\n");
    uart_puts("  │ clear        │ Clear the terminal screen          │\r\n");
    uart_puts("  │ reboot       │ Reboot the system                  │\r\n");
    uart_puts("  └──────────────┴───────────────────────────────────┘\r\n");
    uart_puts("\r\n");
}

static void cmd_ver(void) {
    uart_puts("\r\n  SUB OS v0.1-aarch64\r\n");
    uart_puts("  Architecture : AArch64 (ARMv8-A)\r\n");
    uart_puts("  Target       : QEMU virt (GIC-v2)\r\n");
    uart_puts("  Author       : Subhadip Sarkar\r\n\r\n");
}

static void cmd_uname(void) {
    uart_puts("\r\n  SUB-OS 0.1 aarch64 QEMU-virt\r\n\r\n");
}

static void cmd_uptime(void) {
    unsigned long ticks = timer_get_ticks();
    unsigned long secs  = ticks / 100;
    unsigned long ms    = (ticks % 100) * 10;
    char buf[32];
    uart_puts("\r\n  Uptime: ");
    sh_itoa(secs, buf, 10);
    uart_puts(buf);
    uart_puts(".");
    if (ms < 100) uart_puts("0");
    if (ms < 10)  uart_puts("0");
    sh_itoa(ms, buf, 10);
    uart_puts(buf);
    uart_puts(" seconds\r\n\r\n");
}

static void cmd_meminfo(void) {
    /* Symbols from linker script */
    extern unsigned long kernel_end;
    unsigned long kend = (unsigned long)&kernel_end;
    char buf[32];
    uart_puts("\r\n");
    uart_puts("  ┌─────────────────────────────────────┐\r\n");
    uart_puts("  │           Memory Layout              │\r\n");
    uart_puts("  ├─────────────────────────────────────┤\r\n");
    uart_puts("  │ Kernel load : 0x40080000             │\r\n");
    uart_puts("  │ Kernel end  : 0x");
    sh_itoa(kend, buf, 16);
    uart_puts(buf);
    uart_puts("\r\n");
    uart_puts("  │ RAM total   : 256 MB (QEMU -m 256M)  │\r\n");
    uart_puts("  │ MMU         : disabled (raw mode)    │\r\n");
    uart_puts("  └─────────────────────────────────────┘\r\n\r\n");
}

static void cmd_clear(void) {
    /* ANSI escape: clear screen + move cursor home */
    uart_puts("\033[2J\033[H");
}

static void cmd_echo(const char *args) {
    uart_puts("\r\n  ");
    uart_puts(args);
    uart_puts("\r\n\r\n");
}

static void cmd_reboot(void) {
    uart_puts("\r\n  Rebooting...\r\n");
    /* QEMU virt: write 0x5555 to PSCI reset address via system reset */
    /* Simplest: write to QEMU sysreset MMIO (virt board 0x09030000) */
    volatile unsigned int *reset = (volatile unsigned int *)0x09030000;
    *reset = 0x5555;
    /* If that doesn't work, spin */
    for (;;) __asm__ volatile("wfi");
}

/* ------------------------------------------------------------------ */
/* Input line reader                                                    */
/* ------------------------------------------------------------------ */
#define LINE_MAX 128

static int read_line(char *buf) {
    int i = 0;
    char c;
    while (1) {
        c = uart_getc();
        if (c == '\r' || c == '\n') {
            buf[i] = 0;
            uart_puts("\r\n");
            return i;
        } else if (c == 127 || c == '\b') {  /* backspace */
            if (i > 0) {
                i--;
                uart_puts("\b \b");  /* erase character on terminal */
            }
        } else if (c >= 32 && i < LINE_MAX - 1) {
            buf[i++] = c;
            /* Echo the character */
            char echo[2] = {c, 0};
            uart_puts(echo);
        }
    }
}

/* ------------------------------------------------------------------ */
/* Shell dispatcher                                                     */
/* ------------------------------------------------------------------ */
static void dispatch(char *line) {
    /* Skip leading spaces */
    while (*line == ' ') line++;
    if (*line == 0) return;

    if (sh_strcmp(line, "help") == 0)        { cmd_help();  return; }
    if (sh_strcmp(line, "ver") == 0)         { cmd_ver();   return; }
    if (sh_strcmp(line, "uname") == 0)       { cmd_uname(); return; }
    if (sh_strcmp(line, "uptime") == 0)      { cmd_uptime(); return; }
    if (sh_strcmp(line, "meminfo") == 0)     { cmd_meminfo(); return; }
    if (sh_strcmp(line, "clear") == 0)       { cmd_clear(); return; }
    if (sh_strcmp(line, "reboot") == 0)      { cmd_reboot(); return; }
    if (sh_strncmp(line, "echo ", 5) == 0)   { cmd_echo(line + 5); return; }
    if (sh_strcmp(line, "echo") == 0)        { uart_puts("\r\n\r\n"); return; }

    uart_puts("\r\n  bash: ");
    uart_puts(line);
    uart_puts(": command not found\r\n\r\n");
}

/* ------------------------------------------------------------------ */
/* Shell main loop                                                      */
/* ------------------------------------------------------------------ */
void shell_run(void) {
    char line[LINE_MAX];

    cmd_clear();
    print_banner();

    for (;;) {
        uart_puts("  \033[1;32m[SUB-OS]\033[0m\033[1;34m$\033[0m ");
        read_line(line);
        dispatch(line);
    }
}
