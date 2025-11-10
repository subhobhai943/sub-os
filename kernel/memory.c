// SUB OS - Memory Manager
// Copyright (c) 2025 SUB OS Project

#include "memory.h"
#include "kernel.h"

// Memory map from bootloader (at 0x5000)
#define MEMORY_MAP_ADDR 0x5000

// Memory map entry structure
typedef struct {
    unsigned long long base;
    unsigned long long length;
    unsigned int type;
    unsigned int acpi;
} __attribute__((packed)) memory_map_entry_t;

// Memory statistics
static unsigned long total_memory = 0;
static unsigned long usable_memory = 0;
static unsigned long reserved_memory = 0;

// Memory map entry count
static unsigned int memory_map_entries = 0;

// Memory types
const char* memory_type_names[] = {
    "Unknown",
    "Usable",
    "Reserved",
    "ACPI Reclaimable",
    "ACPI NVS",
    "Bad Memory"
};

// Get memory type name
const char* get_memory_type_name(unsigned int type) {
    if (type == 0 || type > 5) return memory_type_names[0];
    return memory_type_names[type];
}

// Initialize memory manager
void memory_init() {
    print_string("[OK] Detecting memory...\n");
    
    // Get entry count (stored at end of memory map)
    memory_map_entry_t* entries = (memory_map_entry_t*)MEMORY_MAP_ADDR;
    memory_map_entries = *(unsigned short*)(MEMORY_MAP_ADDR + 24 * 100);
    
    if (memory_map_entries == 0) {
        print_string("[WARN] No memory map, assuming 32MB\n");
        total_memory = 32 * 1024 * 1024;
        usable_memory = 32 * 1024 * 1024;
        return;
    }
    
    print_string("Memory Map (");
    print_hex(memory_map_entries);
    print_string(" entries):\n");
    
    // Parse memory map
    for (unsigned int i = 0; i < memory_map_entries && i < 100; i++) {
        memory_map_entry_t* entry = &entries[i];
        
        // Print entry
        print_string("  ");
        print_hex((unsigned int)(entry->base >> 32));
        print_hex((unsigned int)(entry->base & 0xFFFFFFFF));
        print_string(" - ");
        print_hex((unsigned int)((entry->base + entry->length) >> 32));
        print_hex((unsigned int)((entry->base + entry->length) & 0xFFFFFFFF));
        print_string(" : ");
        print_string(get_memory_type_name(entry->type));
        print_string("\n");
        
        // Update statistics
        unsigned long length = (unsigned long)(entry->length & 0xFFFFFFFF);
        total_memory += length;
        
        if (entry->type == 1) {  // Usable
            usable_memory += length;
        } else {
            reserved_memory += length;
        }
    }
    
    // Print summary
    print_string("\nMemory Summary:\n");
    print_string("  Total: ");
    print_hex(total_memory / 1024);
    print_string(" KB (");
    print_hex(total_memory / (1024 * 1024));
    print_string(" MB)\n");
    
    print_string("  Usable: ");
    print_hex(usable_memory / 1024);
    print_string(" KB (");
    print_hex(usable_memory / (1024 * 1024));
    print_string(" MB)\n");
    
    print_string("  Reserved: ");
    print_hex(reserved_memory / 1024);
    print_string(" KB\n");
    
    print_string("[OK] Memory detection complete\n");
}

// Get total memory
unsigned long get_total_memory() {
    return total_memory;
}

// Get usable memory
unsigned long get_usable_memory() {
    return usable_memory;
}
