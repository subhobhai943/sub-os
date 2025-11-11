// SUB OS Kernel v0.10.0
// Copyright (c) 2025 SUB OS Project

#include "kernel.h"
#include "idt.h"
#include "keyboard.h"
#include "timer.h"
#include "memory.h"
#include "pmm.h"
#include "heap.h"
#include "paging.h"
#include "process.h"
#include "syscall.h"
#include "tss.h"
#include "ata.h"
#include "fs.h"

#define VIDEO_MEMORY 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f

int cursor_row = 0;
int cursor_col = 0;

void outb(unsigned short port, unsigned char val) { asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port)); }
unsigned char inb(unsigned short port) { unsigned char ret; asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port)); return ret; }
unsigned short inw(unsigned short port) { unsigned short ret; asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port)); return ret; }
void outw(unsigned short port, unsigned short val) { asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port)); }
// ... rest unchanged ...
// Filesystem test (as above)
