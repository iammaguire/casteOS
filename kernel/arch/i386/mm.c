#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "kernel/multiboot.h"
#include "kernel/mm.h"
#include "kernel/page_util.h"

int* volatile page_directory	= (int*) PAGE_DIRECTORY;
int* volatile page_table		  = (int*) PAGE_TABLE;
char* volatile pmm_bitmap		  = (char*)PMM_BITMAP;

void mm_init(struct multiboot* mbd)
{
	printf("\tDetecting BIOS memory...\n");
	if(mbd->flags & 0x40 == 0)
  {
		printf("\tMemory map not present! Aborting...");
    abort();
  }

	printf("\tBIOS-provided physical memory map is at 0x");
	printf(itoa_nbuf(mbd->mmap_addr, 16));
	printf("\n");

	mm_parse_map(mbd);
  mm_init_paging();
  //mm_init_pmm();		// initialize the physical memory manager
  abort();
}

void mm_print_mem_type(int type)
{
	switch(type)
	{
		case MEMORY_AVAILABLE:
			printf("AVAILABLE\n");
			break;
		case MEMORY_RESERVED:
			printf("RESERVED\n");
			break;
		case MEMORY_ACPI_TABLES:
			printf("ACPI TABLES\n");
			break;
		case MEMORY_ACPI_NVS:
			printf("ACPI NVS\n");
			break;
		case MEMORY_BAD:
			printf("BAD MEMORY\n");
			break;
		default:
			printf("UNKNOWN MEMORY TYPE\n");
			break;
	}
}

void mm_parse_map(struct multiboot* mbd)
{
  mmap_t* mmap;

	total_memory = 0;
	usable_memory = 0;

	mmap = (mmap_t*) mbd->mmap_addr;

	printf("STARTING ADDRESS - ENDING ADDRESS   - TYPE\n");

  int count = 0;

  while (mmap < mbd->mmap_addr + mbd->mmap_length)
  {
    printf("\tregion: %s ", itoa_nbuf(count, 10));
		printf("start: 0x%s", itoa_nbuf((uint32_t) mmap->base >> 32, 16));
		printf("%s", itoa_nbuf((uint32_t) mmap->base & 0xFFFFFFFF, 16));
		printf(" length (bytes): 0x%s", itoa_nbuf((uint32_t) mmap->size >> 32, 10));
		printf("%s", itoa_nbuf((uint32_t) mmap->size & 0xFFFFFFFF, 16));
		printf(" type: %s ", itoa_nbuf(mmap->type, 10));
    mm_print_mem_type(mmap->type);

		if (mmap->type == 1)
			usable_memory += mmap->size;

    total_memory += mmap->size;

  	mmap = (mmap_t*) ((unsigned int) mmap + mmap->entry_size + sizeof(unsigned int));
		count++;
  }

	total_memory_mb = total_memory / 1048576;
	usable_memory_mb = usable_memory / 1048576;

	total_memory_pages = total_memory / 4096;
	usable_memory_pages = usable_memory / 4096;

	reserved_memory_mb = total_memory_mb - usable_memory_mb;
	reserved_memory_pages = total_memory_pages - usable_memory_pages;

	printf("\tTotal memory is %s", itoa_nbuf(total_memory_mb, 10));
	printf(" MB, of which %s MB is usable.\n", itoa_nbuf(usable_memory_mb, 10));
  printf("\tTotal pages is %s", itoa_nbuf(total_memory_pages, 10));
  printf(", of which %s are usable.\n", itoa_nbuf(usable_memory_pages, 10));

	if(usable_memory_mb < REQUIRED_MEMORY)
  {
		printf("casteOS requires at least 64 MB of RAM! Aborting...");
    abort();
  }
}

void mm_init_paging()
{
  printf("\tInitializing paging subsystem...\n");

	int i = 0;

	while(i < 1024)
	{
		page_directory[i] = (unsigned int) page_table + (i * 4096) | PAGE_PRESENT | PAGE_RW | PAGE_USER | PAGE_WRITE_THROUGH;
		i++;
	}

  stosb(page_table, 0, 1024 * 1024 * 4);

	i = 0;
  int memory = 0x00000000;

  while(i < 8192) // identity map lowest 32 MB of RAM
	{
		page_table[i] = (unsigned int) memory + (i * 4096) | PAGE_PRESENT | PAGE_USER | PAGE_WRITE_THROUGH;
		i++;
	}

  // enable paging!
	write_cr4((int) read_cr4() & (~0x20));
	write_cr3((int) page_directory | PAGE_WRITE_THROUGH);
	write_cr0((int) read_cr0() | 0x80000000);
}

void mm_init_pmm()
{
	stosb(pmm_bitmap, 0, 0x100000);

	used_memory_pages = reserved_memory_pages;
	free_memory_pages = total_memory_pages - used_memory_pages;

	pmm_mark_used(0x00000000, 4096);
}

void pmm_mark_page_used(void* page)
{
	pmm_bitmap[(unsigned int)page>>12] = 1;
	used_memory_pages++;
	free_memory_pages--;
}

void pmm_mark_used(void* base, int count)
{
	int c = count;
	int b = base;

	while(c != 0)
	{
		pmm_mark_page_used(b);
		b = b+4096;
		c--;
	}
}
