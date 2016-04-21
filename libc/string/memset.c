#include <string.h>

void* memset(void* bufptr, int value, size_t size)
{
	unsigned char* buf = (unsigned char*) bufptr;
	size_t i = 0;
	while ( i < size )
	{
		//printf(" %s ", itoa(i, 0, 10));
		//printf(": %s\n ", itoa(size, 0, 10));
		//if(i >= 5000) { i = 0; size -= 5000; }
		buf[i] = (unsigned char) value;
		i++;
	}
	return bufptr;
}

char* toupper(char* string)
{
    char* sv = string;
	while(*sv != '\0')
	{
	    if( *sv >= 'a' && *sv <= 'z')
	        *sv = *sv - ('a' - 'A');
	    sv++;
	}
    return string;
}

char toupper_single(char c)
{
    char sv = c;
	if( sv >= 'a' && sv <= 'z')
		sv = sv - ('a' - 'A');
	return sv;
}