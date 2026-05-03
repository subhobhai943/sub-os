/* SUB OS - AArch64 Generic Timer header */
#ifndef AARCH64_TIMER_H
#define AARCH64_TIMER_H

void          timer_init(void);
void          timer_handler(void);
unsigned long timer_get_ticks(void);
void          timer_delay_ms(unsigned int ms);

#endif /* AARCH64_TIMER_H */
