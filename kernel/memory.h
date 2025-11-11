// SUB OS - Memory Detection Header
// Copyright (c) 2025 SUB OS Project

#ifndef MEMORY_H
#define MEMORY_H

// Memory map entry structure
typedef struct {
    unsigned int base_low;
    unsigned int base_high;
    unsigned int length_low;
    unsigned int length_high;
    unsigned int type;
} __attribute__((packed)) memory_map_entry_t;

// Initialize memory detection
void memory_init();

// Get total memory
unsigned long get_total_memory();
unsigned long get_usable_memory();

#endif
