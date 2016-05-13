#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kernel/tty.h"
#include "kernel/vga.h"
#include "kernel/vmm.h"
#include "kernel/multiboot.h"
#include "kernel/descriptor_tables.h"

#define VIDMEM_PHYSICAL_ADDRESS		0xB8000
#define PAGEDIR_PHYSICAL_ADDRESS		0x00080000

char* strMemoryTypes[] = {
	{"Available"},			//memory_region.type==0
	{"Reserved"},			//memory_region.type==1
	{"ACPI Reclaim"},		//memory_region.type==2
	{"ACPI NVS Memory"}		//memory_region.type==3
};

struct multiboot* multiboot_info;

extern uint32_t _begin;
extern uint32_t _end;

uint32_t kernel_start = &_begin;
uint32_t kernel_end = &_end;

void kernel_early(struct multiboot* mbd, unsigned int magic)
{
	terminal_initialize();

	terminal_setcolor(make_color(COLOR_GREEN, COLOR_BLACK));
	printf("               _        ____   _____\n");
    	printf("              | |      / __ \\ / ____|\n");
   	printf("  ___ __ _ ___| |_ ___| |  | | (___  \n");
  	printf(" / __/ _` / __| __/ _ \\ |  | |\\___ \\ \n");
 	printf("| (_| (_| \\__ \\ ||  __/ |__| |____) |\n");
  	printf(" \\___\\__,_|___/\\__\\___|\\____/|_____/ \n\n");
	terminal_setcolor(make_color(COLOR_LIGHT_GREY, COLOR_BLACK));

	printf("Welcome to casteOS!\n\nInitializing GDT and IDT...\n"); 
	init_descriptor_tables();
	printf("Installing IRQ...\n");
	irq_install();
		
	//break_everything_completely(false);

	if(!(mbd->flags & 1))
	{
		printf("\tBit 0 of flags isn't set, invalid data... Aborting boot!\n");
		abort();
	}

	printf("Initializing memory...\n");
	init_memory(mbd);

	printf("\nInstalling keyboard driver...\n");
	keyboard_install();
	printf("\nInstalling PS/2 mouse driver...\n");
	mouse_install();
	printf("\nInstalling timer driver...\n");
	timer_install();

	__asm__ __volatile__ ("sti");

	printf("Initializing stdio...\n");
	init_stdio();

	//if(mbd->flags & (1 << 11)) 
	//{
		graphics_install(mbd);
	//} 
	//else 
	//{
		//printf("\n");
		//floppy_install();
		printf("\nInstalling ATA driver...\n");
		ata_install();
		printf("\nInstalling FAT16 driver...\n");
		f16_install();

		printf("\nFinished initialization successfully, starting CLI...\n\n");

		init_shell();
		
	//}
}

void kernel_main(void)
{
	terminal_setcolor(make_color(COLOR_GREEN, COLOR_BLACK));
	printf("Hello kernel world!\n\n");
	terminal_setcolor(make_color(COLOR_LIGHT_GREY, COLOR_BLACK));

	for(;;)
	{
		update_graphics();
		//shell();
		//timer_wait(10);
	}
	
	for(;;);
}

void init_memory(struct multiboot* mbd)
{
	uint32_t mem_size = 0; 
	uint32_t i = 0;

	if (mbd->flags & 0x001)
	{
		printf("Total memory: %s MiB\n", itoa_nbuf((mbd->mem_upper + mbd->mem_lower + 1024) / 1024, 10));
	}

	if (mbd->flags & 0x040)
	{
		struct multiboot_memory_map * mmap = 0;
		struct multiboot_memory_map * mmap_end = 0;
		
		mmap_end = (struct multiboot_memory_map *) (mbd->mmap_addr + mbd->mmap_length);
		
		printf("Parsing GRUB memory map...\n");
		mmap = (struct multiboot_memory_map*) mbd->mmap_addr;
	
		int count = 0;

		while (mmap < mmap_end)
		{

			printf("region: %s ", itoa_nbuf(count, 10));
			printf("start: 0x%s", itoa_nbuf(mmap->base_addr_high, 16));
			printf("%s", itoa_nbuf(mmap->base_addr_low, 16));
			printf(" length (bytes): 0x%s", itoa_nbuf(mmap->length_high, 10));
			printf("%s", itoa_nbuf(mmap->length_low, 16));
			printf(" type: %s (", itoa_nbuf(mmap->type, 10));
			printf("%s)\n", strMemoryTypes[mmap->type - 1]);

			if (mmap->type == 1)
			{
				mem_size += mmap->length_low;
			}

			mmap = (struct multiboot_memory_map*) ((unsigned int) mmap + mmap->size + sizeof(unsigned int));
		
			count++;
		}

		pmm_install(mem_size, (uint32_t *) kernel_end);

		printf("Initialised physical memory manager to handle %s MiB\n", itoa_nbuf((mem_size >> 10) / 1024, 10));	

		printf("Parsing GRUB regions...\n");
		mmap = (struct multiboot_memory_map *) mbd->mmap_addr;

		while (mmap < mmap_end)
		{
			if (mmap->type == 1)
			{
				pmm_init_region(mmap->base_addr_low, mmap->length_low);
			}

			mmap = (struct multiboot_memory_map*) ((unsigned int) mmap + mmap->size + sizeof(unsigned int));
		}

		uint32_t usable_memory = pmm_get_free_block_count() * 4096 >> 10;

		pmm_deinit_region(kernel_end, get_mmap_end() - kernel_end); // PMM bitmap
		pmm_deinit_region(kernel_start, kernel_end - kernel_start); // kernel

		//vmm_initialize();

		//printf("0x%s\n", itoa_nbuf(vmm_alloc_page(0x010), 16));		
		//printf("0x%s\n", itoa_nbuf(vmm_alloc_page(0x0200), 16));		
		//printf("0x%s\n", itoa_nbuf(vmm_alloc_page(0x0C00000), 16));		
		//printf("0x%s\n", itoa_nbuf(vmm_alloc_page(0x010), 16));		
	}
}

/*void break_everything_completely(bool burn_it_all)
{
	printf("Poof! Your code is no more.");

	uint32_t* kernel_code = &virt;
 	
 	for(uint16_t i = 0; i < (kernel_end - kernel_start); i++)
 	{
 		kernel_code[i] = burn_it_all ? 0xDEADBEEF : 0xE920E81000;
 	}

 	printf("I could literally say anything and do anything because I will never ever be executed.");
}*/