#include <stdint.h>
#include <stdio.h>

void reboot()
{
	uint8_t good = 0x02;
	while(good & 0x02)
		good = inportb(0x64);
	outportb(0x64, 0xFE);
	asm volatile("hlt");
}