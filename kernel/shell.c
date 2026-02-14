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
static char cwd[FS_MAX_FILENAME] = "/";

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

static void strcpy(char* dest, const char* src) {
    while (*src) *dest++ = *src++;
    *dest = 0;
}

static void resolve_path(const char* input, char* out) {
    if (!input || !input[0]) {
        strcpy(out, cwd);
        return;
    }
    if (input[0] == '/') {
        strcpy(out, input);
    } else if (strcmp(cwd, "/") == 0) {
        out[0] = '/';
        int i = 0;
        while (input[i] && i < FS_MAX_FILENAME - 2) { out[i + 1] = input[i]; i++; }
        out[i + 1] = 0;
    } else {
        int i = 0;
        while (cwd[i] && i < FS_MAX_FILENAME - 1) { out[i] = cwd[i]; i++; }
        if (i < FS_MAX_FILENAME - 1) out[i++] = '/';
        int j = 0;
        while (input[j] && i < FS_MAX_FILENAME - 1) out[i++] = input[j++];
        out[i] = 0;
    }
    int len = strlen(out);
    while (len > 1 && out[len - 1] == '/') out[--len] = 0;
}

// Get argument from command string
static void get_arg(const char* cmd, char* arg) {
    int i = 0;
    while (cmd[i] && cmd[i] != ' ') i++;
    while (cmd[i] && cmd[i] == ' ') i++;
    int j = 0;
    while (cmd[i] && j < FS_MAX_FILENAME - 1) {
        arg[j++] = cmd[i++];
    }
    arg[j] = 0;
}

void shell_init() {
    print_string("[OK] Shell initialized\n");
}

void shell_prompt() {
    print_string(cwd);
    print_string(" > ");
}

void shell_read_line() {
    int pos = 0;
    while (1) {
        char c = keyboard_getchar();
        if (c == 0) {
#if defined(__aarch64__) || defined(__arm__)
            asm volatile("wfi");
#else
            asm volatile("hlt");
#endif
            continue;
        }

        if (c == '\n' || c == '\r') {
            cmd_buffer[pos] = 0;
            print_string("\n");
            return;
        } else if (c == '\b' || c == 0x7F) {
            if (pos > 0) pos--;
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
        print_string("  help         - Show this help\n");
        print_string("  clear        - Clear screen\n");
        print_string("  echo [txt]   - Print text\n");
        print_string("  pwd          - Print current directory\n");
        print_string("  cd [dir]     - Change directory\n");
        print_string("  ls [dir]     - List directory entries\n");
        print_string("  mkdir [dir]  - Create directory\n");
        print_string("  rmdir [dir]  - Remove empty directory\n");
        print_string("  touch [f]    - Create empty file\n");
        print_string("  cat [file]   - Read file content\n");
        print_string("  rm [file]    - Delete file\n");
        print_string("  mem          - Show memory usage\n");
        print_string("  sys          - Show system info\n");
        print_string("  eren         - Tatakae\n");
    } else if (strcmp(cmd_buffer, "clear") == 0) {
        clear_screen();
    } else if (strncmp(cmd_buffer, "echo ", 5) == 0) {
        print_string(cmd_buffer + 5);
        print_string("\n");
    } else if (strcmp(cmd_buffer, "pwd") == 0) {
        print_string(cwd);
        print_string("\n");
    } else if (strcmp(cmd_buffer, "ls") == 0 || strncmp(cmd_buffer, "ls ", 3) == 0) {
        char arg[FS_MAX_FILENAME], path[FS_MAX_FILENAME];
        get_arg(cmd_buffer, arg);
        resolve_path(arg, path);
        fs_list(path);
    } else if (strncmp(cmd_buffer, "cd ", 3) == 0) {
        char arg[FS_MAX_FILENAME], path[FS_MAX_FILENAME];
        get_arg(cmd_buffer, arg);
        resolve_path(arg, path);
        if (fs_is_dir(path)) strcpy(cwd, path);
        else print_string("Directory not found\n");
    } else if (strncmp(cmd_buffer, "mkdir ", 6) == 0) {
        char arg[FS_MAX_FILENAME], path[FS_MAX_FILENAME];
        get_arg(cmd_buffer, arg);
        resolve_path(arg, path);
        if (fs_create(path, FS_TYPE_DIR) == 0) print_string("Directory created\n");
    } else if (strncmp(cmd_buffer, "rmdir ", 6) == 0) {
        char arg[FS_MAX_FILENAME], path[FS_MAX_FILENAME];
        get_arg(cmd_buffer, arg);
        resolve_path(arg, path);
        if (fs_delete(path) == 0) print_string("Directory deleted\n");
    } else if (strncmp(cmd_buffer, "cat ", 4) == 0) {
        char filename[FS_MAX_FILENAME], path[FS_MAX_FILENAME];
        get_arg(cmd_buffer, filename);
        resolve_path(filename, path);
        fs_file_t* file = fs_open(path, "r");
        if (file) {
            char buf[512];
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
        char filename[FS_MAX_FILENAME], path[FS_MAX_FILENAME];
        get_arg(cmd_buffer, filename);
        resolve_path(filename, path);
        if (fs_create(path, FS_TYPE_FILE) == 0) {
            print_string("File created\n");
        }
    } else if (strncmp(cmd_buffer, "rm ", 3) == 0) {
        char filename[FS_MAX_FILENAME], path[FS_MAX_FILENAME];
        get_arg(cmd_buffer, filename);
        resolve_path(filename, path);
        if (fs_delete(path) == 0) {
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
