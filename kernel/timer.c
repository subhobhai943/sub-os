// SUB OS - Timer Driver
// Copyright (c) 2025 SUB OS Project

#include "timer.h"
#include "kernel.h"

// Timer frequency (100 Hz = 100 ticks per second)
#define TIMER_FREQUENCY 100
#define PIT_CHANNEL_0 0x40
#define PIT_COMMAND 0x43

// Tick counter
static unsigned long timer_ticks = 0;

// Get current tick count
unsigned long timer_get_ticks() {
    return timer_ticks;
}

// Timer interrupt handler
void timer_handler() {
    timer_ticks++;
    
    // Display tick every second (100 ticks = 1 second at 100 Hz)
    if (timer_ticks % 100 == 0) {
        // Update time display (optional - can be removed for less output)
        // print_string(".");
    }
}

// Initialize timer
void timer_init() {
    // Calculate divisor for desired frequency
    // PIT base frequency is 1193182 Hz
    unsigned int divisor = 1193182 / TIMER_FREQUENCY;
    
    // Send command byte: Channel 0, lobyte/hibyte, rate generator
    outb(PIT_COMMAND, 0x36);
    
    // Send divisor
    outb(PIT_CHANNEL_0, divisor & 0xFF);        // Low byte
    outb(PIT_CHANNEL_0, (divisor >> 8) & 0xFF); // High byte
    
    print_string("[OK] Timer initialized (");
    print_hex(TIMER_FREQUENCY);
    print_string(" Hz)\n");
}

// Sleep for specified number of ticks
void timer_wait(unsigned long ticks) {
    unsigned long target = timer_ticks + ticks;
    while(timer_ticks < target) {
        asm volatile("hlt");
    }
}

// Sleep for milliseconds
void sleep_ms(unsigned long ms) {
    unsigned long ticks = (ms * TIMER_FREQUENCY) / 1000;
    timer_wait(ticks);
}

// Get uptime in seconds
unsigned long get_uptime() {
    return timer_ticks / TIMER_FREQUENCY;
}
