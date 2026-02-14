// SUB OS - Shell Implementation
// Copyright (c) 2025 SUB OS Project

#include "shell.h"
#include "kernel.h"
#include "keyboard.h"
#include "fs.h"
#include "pmm.h"
#include "heap.h"

#define CMD_BUFFER_SIZE 128

static char cmd_buffer[CMD_BUFFER_SIZE];

// String helper functions
static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++; s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static int strncmp(const char* s1, const char* s2, int n) {
    while (n > 0 && *s1 && (*s1 == *s2)) {
        s1++; s2++; n--;
    }
    if (n == 0) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

// Get argument from command string
static void get_arg(const char* cmd, char* arg) {
    int i = 0;
    // Skip command
    while (cmd[i] && cmd[i] != ' ') i++;
    // Skip spaces
    while (cmd[i] && cmd[i] == ' ') i++;
    // Copy argument
    int j = 0;
    while (cmd[i]) {
        arg[j++] = cmd[i++];
    }
    arg[j] = 0;
}

void shell_init() {
    print_string("[OK] Shell initialized\n");
}

void shell_prompt() {
    print_string("TATAKAE> ");
}

void shell_read_line() {
    int pos = 0;
    while (1) {
        char c = keyboard_getchar();
        if (c == 0) {
            // Wait for interrupt
#if defined(__aarch64__) || defined(__arm__)
            asm volatile("wfi");
#else
            asm volatile("hlt");
#endif
            continue;
        }

        if (c == '\n' || c == '\r') {
            cmd_buffer[pos] = 0;
            print_string("\n"); // Ensure newline is printed
            return;
        } else if (c == '\b' || c == 0x7F) { // Handle backspace and delete
            if (pos > 0) {
                pos--;
            }
        } else {
            if (pos < CMD_BUFFER_SIZE - 1) {
                cmd_buffer[pos++] = c;
            }
        }
    }
}

void shell_execute() {
    if (strlen(cmd_buffer) == 0) return;

    if (strcmp(cmd_buffer, "help") == 0) {
        print_string("\nAvailable commands:\n");
        print_string("  help       - Show this help\n");
        print_string("  clear      - Clear screen\n");
        print_string("  echo [txt] - Print text\n");
        print_string("  ls         - List files\n");
        print_string("  cat [file] - Read file content\n");
        print_string("  touch [f]  - Create empty file\n");
        print_string("  rm [file]  - Delete file\n");
        print_string("  mem        - Show memory usage\n");
        print_string("  sys        - Show system info\n");
        print_string("  eren       - Tatakae\n");
    } else if (strcmp(cmd_buffer, "clear") == 0) {
        clear_screen();
    } else if (strncmp(cmd_buffer, "echo ", 5) == 0) {
        print_string(cmd_buffer + 5);
        print_string("\n");
    } else if (strcmp(cmd_buffer, "ls") == 0) {
        fs_list("/");
    } else if (strncmp(cmd_buffer, "cat ", 4) == 0) {
        char filename[32];
        get_arg(cmd_buffer, filename);
        fs_file_t* file = fs_open(filename, "r");
        if (file) {
            char buf[512]; // Small buffer
            int bytes;
            print_string("\n");
            while ((bytes = fs_read(file, buf, 511)) > 0) {
                buf[bytes] = 0;
                print_string(buf);
            }
            print_string("\n");
            fs_close(file);
        } else {
            print_string("File not found\n");
        }
    } else if (strncmp(cmd_buffer, "touch ", 6) == 0) {
        char filename[32];
        get_arg(cmd_buffer, filename);
        if (fs_create(filename, FS_TYPE_FILE) == 0) {
            print_string("File created\n");
        }
    } else if (strncmp(cmd_buffer, "rm ", 3) == 0) {
        char filename[32];
        get_arg(cmd_buffer, filename);
        if (fs_delete(filename) == 0) {
            print_string("File deleted\n");
        }
    } else if (strcmp(cmd_buffer, "mem") == 0) {
        print_string("\nMemory Statistics:\n");
        print_string("  Total: ");
        print_dec(pmm_get_total_memory() / 1024);
        print_string(" KB\n");
        print_string("  Used:  ");
        print_dec(pmm_get_used_memory() / 1024);
        print_string(" KB\n");
        print_string("  Free:  ");
        print_dec(pmm_get_free_memory() / 1024);
        print_string(" KB\n");
        
        unsigned int h_total, h_used, h_free;
        heap_get_stats(&h_total, &h_used, &h_free);
        print_string("\nHeap Statistics:\n");
        print_string("  Total: ");
        print_dec(h_total);
        print_string(" bytes\n");
        print_string("  Used:  ");
        print_dec(h_used);
        print_string(" bytes\n");
        print_string("  Free:  ");
        print_dec(h_free);
        print_string(" bytes\n");

    } else if (strcmp(cmd_buffer, "sys") == 0) {
        print_string("\nSUB OS v0.10.0\n");
        print_string("Built on: " __DATE__ " " __TIME__ "\n");
        print_string("Author: Subhobhai\n");
    } else if (strcmp(cmd_buffer, "eren") == 0) {
        print_string("\n  If you don't fight, you can't win.\n");
        print_string("  Fight! Fight!\n");
        print_string("         - Eren Yeager\n");
    } else {
        print_string("Unknown command: ");
        print_string(cmd_buffer);
        print_string("\n");
    }
}

void shell_run() {
    print_string("Type 'help' for commands.\n");
    while (1) {
        shell_prompt();
        shell_read_line();
        shell_execute();
    }
}
