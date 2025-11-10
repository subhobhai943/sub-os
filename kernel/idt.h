// SUB OS - IDT Header
// Copyright (c) 2025 SUB OS Project

#ifndef IDT_H
#define IDT_H

// Initialize IDT
void idt_init();

// Handler functions
void isr_handler(unsigned int int_no, unsigned int err_code);
void irq_handler(unsigned int irq_no, unsigned int err_code);

#endif
