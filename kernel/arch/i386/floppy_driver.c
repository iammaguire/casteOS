#include <stdint.h>
#include <stdio.h>
#include "kernel/isr.h"
#include "kernel/floppy_driver.h"

static volatile uint8_t floppy_irq_fired = 0;

void flpydsk_write_dor(uint8_t val)
{
	outportb(FLPYDSK_DOR, val);
}

uint8_t flpydsk_read_status()
{
	return inportb(FLPYDSK_MSR);
}

void flpydsk_send_command(uint8_t cmd)
{
	for(int i = 0; i < 500; i++)
		if(flpydsk_read_status() & FLPYDSK_MSR_MASK_DATAREG)
			return outportb(FLPYDSK_FIFO, cmd);
}

uint8_t flpydsk_read_data()
{
	for(int i = 0; i < 500; i++)
		if(flpydsk_read_status() & FLPYDSK_MSR_MASK_DATAREG)
			return inportb(FLPYDSK_FIFO);	
}

void flpydsk_write_ccr(uint8_t val)
{
	outportb(FLPYDSK_CTRL, val);
}

void flpydsk_initialize_dma()
{
	outportb(0x0a, 0x06);
	outportb(0xd8, 0xff);
	outportb(0x04, 0);
	outportb(0x05, 0x10);
	outportb(0xd8, 0xff);
	outportb(0x05, 0xff);
	outportb(0x05, 0x23);
	outportb(0x80, 0);
	outportb(0x0a, 0x02);
}

void flpydsk_dma_read()
{
	outportb(0x0a, 0x06);
	outportb(0x0b, 0x56);
	outportb(0x0a, 0x02);
}

void flpydsk_dma_write()
{
	outportb(0x0a, 0x06);
	outportb(0x0b, 0x5a);
	outportb(0x0a, 0x02);
}

void floppy_handler()
{
	floppy_irq_fired = 1;
}

void wait_flpydsk_irq()
{
	while(floppy_irq_fired == 0)
	;
	floppy_irq_fired = 0;
}

void floppy_install()
{
	irq_install_handler(6, floppy_handler);
}
