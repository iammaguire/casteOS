#include <stdint.h>
#include <stdio.h>
#include "kernel/isr.h"
#include "kernel/kb.h"

uint16_t kb_flags = 0;
unsigned int kb_buf[KB_BUF_SIZE];
int last = 0;

void read_kb_buf(uint8_t *buf, uint16_t size)
{
	((uint8_t*) stdin)[in_size] = kb_buf[size - 1];
}

void keyboard_handler(registers_t *regs)
{
	unsigned char scancode = inportb(0x60);

	if(scancode & 0x80)
	{
		if(kbdus[scancode & ~0x80] == 0xCA)
		{
			if(kb_flags & CAPS_LOCK)
				kb_flags &= ~CAPS_LOCK;
			else
				kb_flags |= CAPS_LOCK;
		}

		if(kbdus[scancode & ~0x80] == 0xC0)
			kb_flags &= ~CTRL;
		if(kbdus[scancode & ~0x80] == 0x15)
			kb_flags &= ~LSHIFT;
		if(kbdus[scancode & ~0x80] == 0x25)
			kb_flags &= ~RSHIFT;
		if(kbdus[scancode & ~0x80] == 0xAA)
			kb_flags &= ~LALT;
	}
	else
	{
		switch(kbdus[scancode])
		{
			case 0xC0:
				kb_flags |= CTRL;
				return;
			case 0x15:
				kb_flags |= LSHIFT;
				return;
			case 0x25:
				kb_flags |= RSHIFT;
				return;
			case 0xAA:
				kb_flags |= LALT;
				return;
		}

		if(!(kb_flags & CTRL))
		{
			if(kb_flags & (CAPS_LOCK | LSHIFT | RSHIFT))
				kb_buf[last++] = kbdusShift[scancode];
			else
				kb_buf[last++] = kbdus[scancode];
			
			read_kb_buf(kb_buf, last);
		}

		if(last == KB_BUF_SIZE)
			last = 0;
	}
}

void install_keyboard()
{
	in_size = 0;

	irq_install_handler(1, keyboard_handler);
}
