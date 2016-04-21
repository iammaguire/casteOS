#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/multiboot.h>

struct multiboot* multiboot_info;

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

	uint32_t kernel_size = 0;
	
	printf("\nReading multiboot info...\n");
	if(!(mbd->flags & 1))
	{
		printf("\tBit 0 of flags isn't set, invalid data... Aborting boot!");
		abort();
	} 
	else
	{
		uint32_t mem_size = ((mbd->mem_lower + mbd->mem_upper) * 1024); //Bytes
		printf("Installing pmm...");
		printf("\tWe have %s MB of memory\n", itoa(mem_size / (1024 * 1024), 0, 10));
		//pmm_install(mbd, mem_size, 0x100000 + kernel_size * 512);
	}

	printf("Installing keyboard driver...\n");
	keyboard_install();
	printf("Installing timer driver...\n");
	timer_install();
	__asm__ __volatile__ ("sti");
	//printf("Installing floppy driver...\n");
	printf("Initializing stdio...\n");
	init_stdio();

	if(mbd->flags & (1 << 11)) 
	{
		graphics_install(mbd);
		put_pixel(10, 100, 0x406001);
		put_pixel(10, 110, 0xFFFF00);
	} 
	else 
	{
		printf("\n");
		floppy_install();
		printf("\nInstalling ATA driver...\n");
		ata_install();
		printf("\nInstalling FAT16 driver...\n");
		f16_install();

		printf("Doing other stuff...\n");

		printf("\nFinished initialization successfully, starting CLI...\n\n");

		init_shell();
	}
}

void kernel_main(void)
{
	if(!multiboot_info->flags & (1 << 11)) 
	{
		terminal_setcolor(make_color(COLOR_GREEN, COLOR_BLACK));
		printf("Hello kernel world!\n\n");
		terminal_setcolor(make_color(COLOR_LIGHT_GREY, COLOR_BLACK));

		for(;;)
			shell();
		for(;;);
	}
}
