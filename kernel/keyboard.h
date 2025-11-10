// SUB OS - Keyboard Driver Header
// Copyright (c) 2025 SUB OS Project

#ifndef KEYBOARD_H
#define KEYBOARD_H

// Initialize keyboard driver
void keyboard_init();

// Keyboard interrupt handler
void keyboard_handler();

// Buffer functions
void keyboard_buffer_add(char c);
char keyboard_buffer_get();
int keyboard_buffer_has_data();

#endif
