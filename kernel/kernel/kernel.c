#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/multiboot.h>

struct multiboot* multiboot_info;

extern uint32_t kernstart;
extern uint32_t kernend;

//! different memory regions (in memory_region.type)
char* strMemoryTypes[] = {
	{"Available"},			//memory_region.type==0
	{"Reserved"},			//memory_region.type==1
	{"ACPI Reclaim"},		//memory_region.type==2
	{"ACPI NVS Memory"}		//memory_region.type==3
};

void kernel_early(struct multiboot* mbd, unsigned int magic)
{
	multiboot_info = mbd;

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
	// printf("Installing paging...\n");
	// paging_install();
	
	printf("\nReading multiboot info...\n");
	
	if(!(mbd->flags & 1))
	{
		printf("\tBit 0 of flags isn't set, invalid data... Aborting boot!\n");
		abort();
	}


	printf("Kernel start: 0x%s\n", itoa_nbuf(&kernstart, 16));
	printf("Kernel end: 0x%s\n", itoa_nbuf(&kernend, 16));
	printf("Kernel size: %s\n", itoa_nbuf((&kernend - &kernstart) / 1024, 10));

	struct multiboot_memory_map* region = (struct multiboot_memory_map*) mbd->mmap_addr;
	uint32_t i = 0;
	
	while(region < mbd->mmap_addr + mbd->mmap_length) {
		region = (unsigned int)region + region->size + sizeof(unsigned int);

		if (i > 0 && region->base_addr_low == 0)
			break;

		if (region->type > 4)
			region->type = 1;

		printf("region: %s ", itoa_nbuf(i, 10));
		printf("start: 0x%s", itoa_nbuf(region->base_addr_high, 16));
		printf("%s", itoa_nbuf(region->base_addr_low, 16));
		printf(" length (bytes): 0x%s", itoa_nbuf(region->length_high, 10));
		printf("%s", itoa_nbuf(region->length_low, 16));
		printf(" type: %s (", itoa_nbuf(region->type, 10));
		printf("%s)\n", strMemoryTypes[region->type - 1]);
	
		i++;
	}

	uint32_t kernel_size = &kernend - &kernstart;
	uint32_t mem_size = mbd->mem_upper; // in KB
	printf("Installing pmm...\n");
	pmm_install(mbd, mem_size);
	
	abort();

	printf("Installing keyboard driver...\n");
	keyboard_install();
	printf("Installing timer driver...\n");
	timer_install();

	__asm__ __volatile__ ("sti");

	printf("Initializing stdio...\n");
	init_stdio();

	/*if(mbd->flags & (1 << 11)) 
	{
		graphics_install(mbd);
		put_pixel(10, 100, 0x406001);
		put_pixel(10, 110, 0xFFFF00);
	} 
	else 
	{*/
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
		shell();
	for(;;);
}
