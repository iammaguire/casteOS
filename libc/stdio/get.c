#include <stdio.h>

int getchar()
{
	int c = getch();
	putchar(c);
	return c;
}

int getch()
{
	for(;;)
	{
		if(((uint8_t*)stdin)[in_size] != 0)
		{
			in_size++;
			break;
		}
	}

	return ((uint8_t*)stdin)[in_size-1];
}

char* getstr(char *str)
{
	int c = getch();
	int i = 0;

	while(c != '\n')
	{
		if(c != '\b')
		{
			str[i++] = c;
			putchar(str[i-1]);
		} 
		else if (c == '\b' && i > 0)
		{
			str[--i] = 0;
			putchar(c);
		}

		c = getch();
	}

	str[i] = '\0';
	putchar('\n');
	return str;
}