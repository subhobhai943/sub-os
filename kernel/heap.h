// SUB OS - Heap Allocator Header
// Copyright (c) 2025 SUB OS Project

#ifndef HEAP_H
#define HEAP_H

// Initialize heap
void heap_init();

// Memory allocation
void* kmalloc(unsigned int size);
void kfree(void* ptr);

// Statistics
void heap_get_stats(unsigned int* total, unsigned int* used, unsigned int* free);

#endif
