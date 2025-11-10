// SUB OS - Paging (Virtual Memory)
// Copyright (c) 2025 SUB OS Project

#include "paging.h"
#include "pmm.h"
#include "kernel.h"

typedef struct {
    unsigned int present    : 1;
    unsigned int rw         : 1;
    unsigned int user       : 1;
    unsigned int accessed   : 1;
    unsigned int dirty      : 1;
    unsigned int unused     : 7;
    unsigned int frame      : 20;
} __attribute__((packed)) page_t;

typedef struct {
    page_t pages[1024];
} page_table_t;

typedef struct {
    unsigned int tables_physical[1024];
    page_table_t* tables[1024];
} page_directory_t;

static page_directory_t* kernel_directory = 0;
static page_directory_t* current_directory = 0;

extern void page_fault_handler(unsigned int error_code);

static page_t* get_page(unsigned int address, int make, page_directory_t* dir) {
    address /= 0x1000;
    unsigned int table_idx = address / 1024;
    if (dir->tables[table_idx]) {
        return &dir->tables[table_idx]->pages[address % 1024];
    } else if (make) {
        unsigned int tmp;
        dir->tables[table_idx] = (page_table_t*)pmm_alloc_page();
        for (int i = 0; i < 1024; i++) {
            dir->tables[table_idx]->pages[i].present = 0;
            dir->tables[table_idx]->pages[i].rw = 1;
            dir->tables[table_idx]->pages[i].user = 0;
            dir->tables[table_idx]->pages[i].frame = 0;
        }
        tmp = (unsigned int)dir->tables[table_idx];
        dir->tables_physical[table_idx] = tmp | 0x7;
        return &dir->tables[table_idx]->pages[address % 1024];
    } else {
        return 0;
    }
}

void alloc_frame(page_t* page, int is_kernel, int is_writeable) {
    if (page->frame != 0) return;
    unsigned int idx = pmm_alloc_page() / 0x1000;
    page->present = 1;
    page->rw = (is_writeable) ? 1 : 0;
    page->user = (is_kernel) ? 0 : 1;
    page->frame = idx;
}

void free_frame(page_t* page) {
    unsigned int frame;
    if (!(frame = page->frame)) return;
    pmm_free_page(frame * 0x1000);
    page->frame = 0;
}

void switch_page_directory(page_directory_t* dir) {
    current_directory = dir;
    asm volatile("mov %0, %%cr3" :: "r"(&dir->tables_physical));
    unsigned int cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

void paging_init() {
    print_string("[OK] Initializing Paging...\n");
    kernel_directory = (page_directory_t*)pmm_alloc_page();
    for (int i = 0; i < 1024; i++) {
        kernel_directory->tables_physical[i] = 0;
        kernel_directory->tables[i] = 0;
    }
    unsigned int i = 0;
    while (i < 0x00400000) {
        alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
        i += 0x1000;
    }
    print_string("  Identity mapped: 0x00000000 - 0x00400000 (4MB)\n");
    unsigned int heap_start = 0x00400000;
    unsigned int heap_pages = 256;
    for (i = 0; i < heap_pages; i++) {
        alloc_frame(get_page(heap_start + i * 0x1000, 1, kernel_directory), 0, 1);
    }
    print_string("  Heap mapped: 0x00400000 - 0x00500000 (1MB)\n");
    switch_page_directory(kernel_directory);
    print_string("[OK] Paging enabled\n");
}

void page_fault(unsigned int error_code, unsigned int faulting_address) {
    int present = !(error_code & 0x1);
    int rw = error_code & 0x2;
    int user = error_code & 0x4;
    int reserved = error_code & 0x8;
    print_string("\n[PAGE FAULT] ");
    if (present) print_string("Present ");
    if (rw) print_string("Read-only ");
    if (user) print_string("User-mode ");
    if (reserved) print_string("Reserved ");
    print_string("at ");
    print_hex(faulting_address);
    print_string("\n");
    print_string("Error code: ");
    print_hex(error_code);
    print_string("\n");
    print_string("System halted.\n");
    for(;;);
}

page_directory_t* get_current_directory() {
    return current_directory;
}

void map_page(unsigned int virtual_addr, unsigned int physical_addr, int is_kernel, int is_writeable) {
    page_t* page = get_page(virtual_addr, 1, current_directory);
    page->present = 1;
    page->rw = (is_writeable) ? 1 : 0;
    page->user = (is_kernel) ? 0 : 1;
    page->frame = physical_addr / 0x1000;
}

void unmap_page(unsigned int virtual_addr) {
    page_t* page = get_page(virtual_addr, 0, current_directory);
    if (page) {
        page->present = 0;
        asm volatile("invlpg (%0)" :: "r"(virtual_addr) : "memory");
    }
}
