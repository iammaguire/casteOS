#include <string.h>
#include <stdint.h>

uint16_t inportw(uint16_t port)
{
	uint16_t ret;
	__asm volatile("inw %%dx, %%ax":"=a"(ret):"d"(port));
	return ret;
}

void insw(unsigned short port, void *addr, size_t cnt)
{
   __asm volatile("rep; insw"
       : "+D" (addr), "+c" (cnt)
       : "d" (port)
       : "memory");
}

void outportw(unsigned short port, unsigned short val)
{
	__asm__ __volatile__ ("outw %%ax, %%dx" : : "d" (port), "a" (val));
}