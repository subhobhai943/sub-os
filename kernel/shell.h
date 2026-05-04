// SUB OS - Shell Header
// Copyright (c) 2025-2026 SUB OS Project

#ifndef SHELL_H
#define SHELL_H

#define SHELL_MAX_CMD    256
#define SHELL_HISTORY_SIZE 16

void shell_init(void);
void shell_run(void);
void shell_process_char(char c);
void shell_execute(const char *cmd);

#endif
