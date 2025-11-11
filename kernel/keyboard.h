// SUB OS - Keyboard Header
// Copyright (c) 2025 SUB OS Project

#ifndef KEYBOARD_H
#define KEYBOARD_H

// Initialize keyboard driver
void keyboard_init();

// Keyboard IRQ handler
void keyboard_handler();

// Read character from keyboard buffer
char keyboard_getchar();

#endif
