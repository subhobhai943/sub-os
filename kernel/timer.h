// SUB OS - Timer Driver Header
// Copyright (c) 2025 SUB OS Project

#ifndef TIMER_H
#define TIMER_H

// Initialize timer
void timer_init();

// Timer interrupt handler
void timer_handler();

// Get current tick count
unsigned long timer_get_ticks();

// Wait functions
void timer_wait(unsigned long ticks);
void sleep_ms(unsigned long ms);

// Get system uptime
unsigned long get_uptime();

#endif
