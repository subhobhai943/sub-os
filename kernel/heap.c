// SUB OS - Heap Allocator
// Copyright (c) 2025 SUB OS Project

#include "heap.h"
#include "pmm.h"
#include "kernel.h"

// Heap block header
typedef struct heap_block {
    unsigned int size;
    unsigned int is_free;
    struct heap_block* next;
} heap_block_t;

// Heap start and end
static heap_block_t* heap_start = 0;
static unsigned int heap_size = 0;

// Initialize heap
void heap_init() {
    print_string("[OK] Initializing Heap Allocator...\n");
    
    // Allocate 16 pages (64KB) for heap
    unsigned int heap_pages = 16;
    unsigned int heap_addr = pmm_alloc_pages(heap_pages);
    
    if (heap_addr == 0) {
        print_string("[ERROR] Failed to allocate heap memory!\n");
        return;
    }
    
    heap_start = (heap_block_t*)heap_addr;
    heap_size = heap_pages * 4096;
    
    // Initialize first block
    heap_start->size = heap_size - sizeof(heap_block_t);
    heap_start->is_free = 1;
    heap_start->next = 0;
    
    print_string("  Heap size: ");
    print_dec(heap_size / 1024);
    print_string(" KB\n");
    print_string("  Heap address: ");
    print_hex(heap_addr);
    print_string("\n");
    print_string("[OK] Heap Allocator initialized\n");
}

// Allocate memory from heap
void* kmalloc(unsigned int size) {
    if (size == 0) return 0;
    
    // Align size to 4 bytes
    size = (size + 3) & ~3;
    
    heap_block_t* current = heap_start;
    
    while (current) {
        if (current->is_free && current->size >= size) {
            // Found suitable block
            
            // Split block if there's enough space
            if (current->size >= size + sizeof(heap_block_t) + 4) {
                heap_block_t* new_block = (heap_block_t*)((char*)current + sizeof(heap_block_t) + size);
                new_block->size = current->size - size - sizeof(heap_block_t);
                new_block->is_free = 1;
                new_block->next = current->next;
                
                current->size = size;
                current->next = new_block;
            }
            
            current->is_free = 0;
            return (void*)((char*)current + sizeof(heap_block_t));
        }
        
        current = current->next;
    }
    
    // No suitable block found
    return 0;
}

// Free memory
void kfree(void* ptr) {
    if (!ptr) return;
    
    heap_block_t* block = (heap_block_t*)((char*)ptr - sizeof(heap_block_t));
    block->is_free = 1;
    
    // Merge with next block if it's free
    if (block->next && block->next->is_free) {
        block->size += sizeof(heap_block_t) + block->next->size;
        block->next = block->next->next;
    }
    
    // Merge with previous block if it's free
    heap_block_t* current = heap_start;
    while (current && current->next != block) {
        current = current->next;
    }
    
    if (current && current->is_free) {
        current->size += sizeof(heap_block_t) + block->size;
        current->next = block->next;
    }
}

// Get heap statistics
void heap_get_stats(unsigned int* total, unsigned int* used, unsigned int* free) {
    *total = heap_size;
    *used = 0;
    *free = 0;
    
    heap_block_t* current = heap_start;
    while (current) {
        if (current->is_free) {
            *free += current->size;
        } else {
            *used += current->size;
        }
        current = current->next;
    }
}
