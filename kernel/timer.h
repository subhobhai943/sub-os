// SUB OS - Timer Header
// Copyright (c) 2025 SUB OS Project

#ifndef TIMER_H
#define TIMER_H

// Initialize PIT timer
void timer_init();

// Timer IRQ handler
void timer_handler();

// Get timer ticks
unsigned long timer_get_ticks();

// Get uptime in seconds
unsigned long get_uptime();

// Sleep for milliseconds
void sleep_ms(unsigned long ms);

#endif
