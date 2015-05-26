#include <stdio.h>
#include <stdlib.h>
#include "kernel/isr.h"
#include "kernel/timer.h"

volatile int timer_ticks;

void timer_handler(registers_t *regs)
{
	timer_ticks++;
}

void timer_install()
{
	irq_install_handler(0, timer_handler);
}

void timer_wait(int ticks)
{
	unsigned long eticks;
	eticks = timer_ticks + ticks;
	while(timer_ticks < eticks);
}