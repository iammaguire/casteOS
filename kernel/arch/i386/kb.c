#include <stdint.h>
#include <stdio.h>
#include "kernel/isr.h"
#include "kernel/kb.h"

uint16_t kb_flags = 0;
unsigned int kb_buf[KB_BUF_SIZE];
kb_hook_t kb_hooks[MAX_HOOKS];
int num_of_hooks = 0;
int last = 0;

void scan_key_hooks(int key)
{
	for(int i = 0; i < num_of_hooks; i++)
	{
		if(key == kb_hooks[i].key)
		{
			void (*hook_func)(void);
			hook_func = kb_hooks[i].func;
			(*hook_func)();
		}
	}
}

void add_key_hook(int key, void* func)
{
	if(num_of_hooks < MAX_HOOKS)
	{
		kb_hooks[num_of_hooks].key = key;
		kb_hooks[num_of_hooks].func = func;
		num_of_hooks++;
	}
	else
	{
		puts("Too many keyboard hooks assigned...");
	}
}

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
			case 0xA1:
				scan_key_hooks(0xA1);
				return;
			case 0xA2:
				scan_key_hooks(0xA2);
				return;
			case 0xA3:
				scan_key_hooks(0xA3);
				return;
			case 0xA4:
				scan_key_hooks(0xA4);
				return;
		}

		if(!(kb_flags & CTRL))
		{
			if(kb_flags & (CAPS_LOCK | LSHIFT | RSHIFT))
				kb_buf[last++] = kbdusShift[scancode];
			else
				kb_buf[last++] = kbdus[scancode];

			scan_key_hooks(kb_buf[last - 1]);
			
			read_kb_buf(kb_buf, last);
		}

		if(last == KB_BUF_SIZE)
			last = 0;
	}
}

void keyboard_install()
{
	in_size = 0;

	irq_install_handler(1, keyboard_handler);
}
