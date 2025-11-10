// SUB OS - Paging Header
// Copyright (c) 2025 SUB OS Project

#ifndef PAGING_H
#define PAGING_H

void paging_init();
void page_fault(unsigned int error_code, unsigned int faulting_address);
void map_page(unsigned int virtual_addr, unsigned int physical_addr, int is_kernel, int is_writeable);
void unmap_page(unsigned int virtual_addr);

#endif
