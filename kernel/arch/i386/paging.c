#include "kernel/paging.h"

unsigned long *page_dir = (unsigned long*) 0x9C000;
unsigned long *page_table = (unsigned long*) 0x9D000;

void paging_install()
{
	unsigned long addr = 0;
	unsigned int i;

	for(i = 0; i < 1024; i++)
	{
		page_table[i] = addr | 3; // 011 (supervisor lvl, r/w perms, present)
		addr += 4096;
	}

	page_directory[0] = page_table;
	page_directory[0] = page_directory[0] | 3;

	for(i = 1; i < 1024; i++)
	{
		page_directory[i] = 0 | 2; // 010 (supervisor lvl, r/w perms, NOT present)
	}

	write_cr3(page_directory);
	write_cr0(read_cro() | 0x80000000);
}
