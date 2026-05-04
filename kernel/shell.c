// SUB OS - Shell (SUBsh) Implementation
// Copyright (c) 2025-2026 SUB OS Project

#include "shell.h"
#include "kernel.h"
#include "keyboard.h"
#include "timer.h"
#include "pmm.h"
#include "gui.h"
#include "apps.h"

#define COLOR_DEFAULT   0x0F
#define COLOR_GREEN     0x0A
#define COLOR_YELLOW    0x0E
#define COLOR_CYAN      0x0B
#define COLOR_RED       0x0C
#define COLOR_BLUE      0x09
#define COLOR_MAGENTA   0x0D

static char cmd_buf[SHELL_MAX_CMD];
static int  cmd_len = 0;
static char history[SHELL_HISTORY_SIZE][SHELL_MAX_CMD];
static int  history_count = 0;

static void print_colored(const char *s, char attr) {
    for (int i = 0; s[i]; i++)
        print_char(s[i], -1, -1, attr);
}

static int str_eq(const char *a, const char *b) {
    while (*a && *b) { if (*a != *b) return 0; a++; b++; }
    return *a == *b;
}

static int str_starts(const char *s, const char *prefix) {
    while (*prefix) { if (*s != *prefix) return 0; s++; prefix++; }
    return 1;
}

static void str_copy(char *dst, const char *src) {
    while ((*dst++ = *src++));
}

static const char *skip_word_space(const char *s) {
    while (*s && *s != ' ') s++;
    while (*s == ' ') s++;
    return s;
}

static void cmd_help(void) {
    print_colored("\n SUB OS Shell Commands (SUBsh):\n", COLOR_CYAN);
    print_colored("-----------------------------------------\n", COLOR_YELLOW);
    print_colored("  help          ", COLOR_GREEN); print_colored("- Show this help\n", COLOR_DEFAULT);
    print_colored("  clear         ", COLOR_GREEN); print_colored("- Clear screen\n", COLOR_DEFAULT);
    print_colored("  echo [text]   ", COLOR_GREEN); print_colored("- Print text\n", COLOR_DEFAULT);
    print_colored("  version       ", COLOR_GREEN); print_colored("- Show OS version\n", COLOR_DEFAULT);
    print_colored("  uptime        ", COLOR_GREEN); print_colored("- Show system uptime\n", COLOR_DEFAULT);
    print_colored("  meminfo       ", COLOR_GREEN); print_colored("- Show memory info\n", COLOR_DEFAULT);
    print_colored("  ls            ", COLOR_GREEN); print_colored("- List files (VFS)\n", COLOR_DEFAULT);
    print_colored("  cat [file]    ", COLOR_GREEN); print_colored("- Read a file\n", COLOR_DEFAULT);
    print_colored("  desktop       ", COLOR_CYAN);  print_colored("- Open graphical desktop\n", COLOR_DEFAULT);
    print_colored("  notepad       ", COLOR_CYAN);  print_colored("- Open text editor\n", COLOR_DEFAULT);
    print_colored("  calc          ", COLOR_CYAN);  print_colored("- Open calculator\n", COLOR_DEFAULT);
    print_colored("  files         ", COLOR_CYAN);  print_colored("- Open file manager\n", COLOR_DEFAULT);
    print_colored("  sysmon        ", COLOR_CYAN);  print_colored("- Open system monitor\n", COLOR_DEFAULT);
    print_colored("  reboot        ", COLOR_GREEN); print_colored("- Reboot the system\n", COLOR_DEFAULT);
    print_colored("  halt          ", COLOR_GREEN); print_colored("- Halt the system\n", COLOR_DEFAULT);
    print_colored("-----------------------------------------\n", COLOR_YELLOW);
}

static void cmd_version(void) {
    print_colored("\n  SUB OS v0.11.0\n", COLOR_CYAN);
    print_colored("  Arch: x86 (32-bit Protected Mode)\n", COLOR_DEFAULT);
    print_colored("  Kernel: Monolithic  Shell: SUBsh v1.0\n", COLOR_DEFAULT);
    print_colored("  Apps: Notepad, Calculator, File Manager, Sys Monitor\n\n", COLOR_DEFAULT);
}

static void cmd_uptime(void) {
    unsigned long secs = get_uptime();
    print_colored("  Uptime: ", COLOR_GREEN);
    print_dec((unsigned int)(secs / 3600)); print_string("h ");
    print_dec((unsigned int)((secs % 3600) / 60)); print_string("m ");
    print_dec((unsigned int)(secs % 60)); print_string("s\n");
}

static void cmd_meminfo(void) {
    unsigned int total    = pmm_get_total_memory() / 1024;
    unsigned int used     = pmm_get_used_memory()  / 1024;
    unsigned int free_mem = pmm_get_free_memory()  / 1024;
    print_colored("\n  Memory Information:\n", COLOR_CYAN);
    print_colored("  Total: ", COLOR_GREEN);  print_dec(total);    print_string(" KB\n");
    print_colored("  Used:  ", COLOR_YELLOW); print_dec(used);     print_string(" KB\n");
    print_colored("  Free:  ", COLOR_GREEN);  print_dec(free_mem); print_string(" KB\n\n");
}

static void cmd_ls(void) {
    print_colored("\n  VFS Root (/):\n", COLOR_CYAN);
    print_colored("  [filesystem empty - no files yet]\n\n", COLOR_DEFAULT);
}

static void cmd_cat(const char *arg) {
    if (!arg || !*arg) {
        print_colored("  Usage: cat <filename>\n", COLOR_RED);
        return;
    }
    print_colored("  cat: ", COLOR_RED);
    print_string(arg);
    print_string(": No such file or directory\n");
}

void shell_execute(const char *cmd) {
    while (*cmd == ' ') cmd++;
    if (!*cmd) return;

    if (history_count < SHELL_HISTORY_SIZE)
        str_copy(history[history_count++], cmd);

    if      (str_eq(cmd, "help"))     cmd_help();
    else if (str_eq(cmd, "clear"))    { clear_screen(); return; }
    else if (str_eq(cmd, "version"))  cmd_version();
    else if (str_eq(cmd, "uptime"))   cmd_uptime();
    else if (str_eq(cmd, "meminfo"))  cmd_meminfo();
    else if (str_eq(cmd, "ls"))       cmd_ls();
    else if (str_eq(cmd, "desktop"))  gui_draw_desktop();
    else if (str_eq(cmd, "notepad"))  { app_notepad();     gui_draw_banner(); }
    else if (str_eq(cmd, "calc"))     { app_calculator();  gui_draw_banner(); }
    else if (str_eq(cmd, "files"))    { app_filemanager(); gui_draw_banner(); }
    else if (str_eq(cmd, "sysmon"))   { app_sysmon();      gui_draw_banner(); }
    else if (str_eq(cmd, "reboot")) {
        print_colored("\n  Rebooting...\n", COLOR_YELLOW);
        outb(0x64, 0xFE);
        asm volatile("hlt");
    }
    else if (str_eq(cmd, "halt")) {
        print_colored("\n  System halted. Power off safely.\n", COLOR_YELLOW);
        asm volatile("cli; hlt");
    }
    else if (str_starts(cmd, "echo ")) {
        const char *arg = skip_word_space(cmd);
        print_string("  "); print_string(arg); print_string("\n");
    }
    else if (str_starts(cmd, "cat ")) {
        cmd_cat(skip_word_space(cmd));
    }
    else {
        print_colored("  ", COLOR_DEFAULT);
        print_string(cmd);
        print_colored(": command not found. Type 'help'\n", COLOR_RED);
    }
}

static void print_prompt(void) {
    print_colored("\nsubos", COLOR_GREEN);
    print_colored("@", COLOR_DEFAULT);
    print_colored("kernel", COLOR_CYAN);
    print_colored(":/ $ ", COLOR_YELLOW);
}

void shell_process_char(char c) {
    if (c == '\n' || c == '\r') {
        print_string("\n");
        cmd_buf[cmd_len] = '\0';
        if (cmd_len > 0)
            shell_execute(cmd_buf);
        cmd_len = 0;
        cmd_buf[0] = '\0';
        print_prompt();
    } else if (c == '\b') {
        if (cmd_len > 0) {
            cmd_len--;
            cmd_buf[cmd_len] = '\0';
        }
    } else if (c >= 32 && c < 127) {
        if (cmd_len < SHELL_MAX_CMD - 1) {
            cmd_buf[cmd_len++] = c;
            cmd_buf[cmd_len]   = '\0';
        }
    }
}

void shell_init(void) {
    cmd_len = 0;
    cmd_buf[0] = '\0';
    history_count = 0;
}

void shell_run(void) {
    shell_init();
    gui_draw_banner();
    print_colored("\n  Welcome to SUB OS! Type 'help' for commands.\n", COLOR_CYAN);
    print_prompt();

    while (1) {
        char c = keyboard_getchar();
        if (c)
            shell_process_char(c);
        asm volatile("hlt");
    }
}
