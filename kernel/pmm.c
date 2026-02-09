// SUB OS - Physical Memory Manager
// Copyright (c) 2025 SUB OS Project

#include "pmm.h"
#include "kernel.h"
#include "memory.h"

// Page size (4KB)
#define PAGE_SIZE 4096
#define PAGES_PER_BYTE 8

// Bitmap for tracking page allocation
// Each bit represents one 4KB page
#ifdef __arm__
static unsigned char* page_bitmap; 
#else
static unsigned char* page_bitmap = (unsigned char*)0x10000; // Bitmap at 1MB
#endif
static unsigned int total_pages = 0;
static unsigned int used_pages = 0;
static unsigned int bitmap_size = 0;

// Kernel end marker (defined in linker script)
extern unsigned int kernel_end;

// Initialize physical memory manager
void pmm_init() {
    print_string("[OK] Initializing Physical Memory Manager...\n");
    
#ifdef __arm__
    // Place bitmap after kernel
    page_bitmap = (unsigned char*)((unsigned int)&kernel_end + 0x1000);
#endif
    
    // Get usable memory from memory detection
    unsigned long usable_mem = get_usable_memory();
    
    if (usable_mem == 0) {
        print_string("[ERROR] No usable memory detected!\n");
        return;
    }
    
    // Calculate total number of pages
    total_pages = usable_mem / PAGE_SIZE;
    bitmap_size = total_pages / PAGES_PER_BYTE;
    if (total_pages % PAGES_PER_BYTE) bitmap_size++;
    
    print_string("  Total pages: ");
    print_dec(total_pages);
    print_string(" (");
    print_dec(total_pages * 4);
    print_string(" KB)\n");
    
    print_string("  Bitmap size: ");
    print_dec(bitmap_size);
    print_string(" bytes\n");
    
    // Initialize bitmap - mark all as used
    for (unsigned int i = 0; i < bitmap_size; i++) {
        page_bitmap[i] = 0xFF;
    }
    
    // Mark first 1MB as used (BIOS, video memory, etc.)
    unsigned int first_mb_pages = (1024 * 1024) / PAGE_SIZE;
    
    // Mark usable memory as free (starting from 1MB)
    for (unsigned int i = first_mb_pages; i < total_pages; i++) {
        pmm_free_page(i * PAGE_SIZE);
    }
    
    // Mark kernel memory as used
    unsigned int kernel_pages = ((unsigned int)&kernel_end) / PAGE_SIZE + 1;
    for (unsigned int i = 0; i < kernel_pages; i++) {
        pmm_set_page_used(i * PAGE_SIZE);
    }
    
    print_string("  Reserved: First ");
    print_dec(kernel_pages);
    print_string(" pages for kernel\n");
    
    print_string("  Available pages: ");
    print_dec(total_pages - used_pages);
    print_string(" (");
    print_dec((total_pages - used_pages) * 4);
    print_string(" KB)\n");
    
    print_string("[OK] Physical Memory Manager initialized\n");
}

// Set page as used
void pmm_set_page_used(unsigned int address) {
    unsigned int page = address / PAGE_SIZE;
    unsigned int byte = page / PAGES_PER_BYTE;
    unsigned int bit = page % PAGES_PER_BYTE;
    
    if (byte >= bitmap_size) return;
    
    unsigned char was_free = !(page_bitmap[byte] & (1 << bit));
    page_bitmap[byte] |= (1 << bit);
    
    if (was_free) used_pages++;
}

// Set page as free
void pmm_set_page_free(unsigned int address) {
    unsigned int page = address / PAGE_SIZE;
    unsigned int byte = page / PAGES_PER_BYTE;
    unsigned int bit = page % PAGES_PER_BYTE;
    
    if (byte >= bitmap_size) return;
    
    unsigned char was_used = page_bitmap[byte] & (1 << bit);
    page_bitmap[byte] &= ~(1 << bit);
    
    if (was_used) used_pages--;
}

// Allocate a single page
unsigned int pmm_alloc_page() {
    for (unsigned int i = 0; i < bitmap_size; i++) {
        if (page_bitmap[i] != 0xFF) {
            // Found a byte with at least one free page
            for (unsigned int bit = 0; bit < PAGES_PER_BYTE; bit++) {
                if (!(page_bitmap[i] & (1 << bit))) {
                    // Found free page
                    unsigned int page = i * PAGES_PER_BYTE + bit;
                    unsigned int address = page * PAGE_SIZE;
                    pmm_set_page_used(address);
                    return address;
                }
            }
        }
    }
    
    // No free pages
    return 0;
}

// Free a single page
void pmm_free_page(unsigned int address) {
    pmm_set_page_free(address);
}

// Allocate multiple contiguous pages
unsigned int pmm_alloc_pages(unsigned int count) {
    if (count == 0) return 0;
    if (count == 1) return pmm_alloc_page();
    
    unsigned int start_page = 0;
    unsigned int found_count = 0;
    
    for (unsigned int page = 0; page < total_pages; page++) {
        unsigned int byte = page / PAGES_PER_BYTE;
        unsigned int bit = page % PAGES_PER_BYTE;
        
        if (byte >= bitmap_size) break;
        
        if (!(page_bitmap[byte] & (1 << bit))) {
            // Free page
            if (found_count == 0) {
                start_page = page;
            }
            found_count++;
            
            if (found_count == count) {
                // Found enough contiguous pages
                for (unsigned int i = 0; i < count; i++) {
                    pmm_set_page_used((start_page + i) * PAGE_SIZE);
                }
                return start_page * PAGE_SIZE;
            }
        } else {
            // Used page, reset counter
            found_count = 0;
        }
    }
    
    // Not enough contiguous pages
    return 0;
}

// Free multiple contiguous pages
void pmm_free_pages(unsigned int address, unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        pmm_free_page(address + (i * PAGE_SIZE));
    }
}

// Get total pages
unsigned int pmm_get_total_pages() {
    return total_pages;
}

// Get used pages
unsigned int pmm_get_used_pages() {
    return used_pages;
}

// Get free pages
unsigned int pmm_get_free_pages() {
    return total_pages - used_pages;
}

// Get total memory
unsigned int pmm_get_total_memory() {
    return total_pages * PAGE_SIZE;
}

// Get used memory
unsigned int pmm_get_used_memory() {
    return used_pages * PAGE_SIZE;
}

// Get free memory
unsigned int pmm_get_free_memory() {
    return (total_pages - used_pages) * PAGE_SIZE;
}
