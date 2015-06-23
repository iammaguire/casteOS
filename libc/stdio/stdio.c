#include <stdio.h>

uint8_t *inbuffer;

void init_stdio()
{
	stdin = (uint8_t*) inbuffer;

	for(int i = 0; i < STDIO_SIZE; i++)
	{
		inbuffer[i] = 0;
	}

	putchar('\0');
	putchar('\0');
}
