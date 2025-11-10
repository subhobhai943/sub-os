// SUB OS - Physical Memory Manager Header
// Copyright (c) 2025 SUB OS Project

#ifndef PMM_H
#define PMM_H

// Initialize physical memory manager
void pmm_init();

// Page allocation
unsigned int pmm_alloc_page();
void pmm_free_page(unsigned int address);

// Multiple page allocation
unsigned int pmm_alloc_pages(unsigned int count);
void pmm_free_pages(unsigned int address, unsigned int count);

// Page status
void pmm_set_page_used(unsigned int address);
void pmm_set_page_free(unsigned int address);

// Statistics
unsigned int pmm_get_total_pages();
unsigned int pmm_get_used_pages();
unsigned int pmm_get_free_pages();
unsigned int pmm_get_total_memory();
unsigned int pmm_get_used_memory();
unsigned int pmm_get_free_memory();

#endif
