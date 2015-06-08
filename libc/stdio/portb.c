unsigned char inportb(unsigned short _port)
{
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

void outportb(unsigned short _port, unsigned char _data)
{
	unsigned char rv;
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}