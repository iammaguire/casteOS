#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/multiboot.h>

struct multiboot* multiboot_info;

extern uint32_t kernstart;
extern uint32_t kernend;

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
	
	//break_everything_completely(false);

	if(!(mbd->flags & 1))
	{
		printf("\tBit 0 of flags isn't set, invalid data... Aborting boot!\n");
		abort();
	}

	printf("Kernel start: 0x%s\n", itoa_nbuf(&kernstart, 16));
	printf("Kernel end: 0x%s\n", itoa_nbuf(&kernend, 16));
	printf("Kernel size: %s\n", itoa_nbuf((&kernend - &kernstart) / 1024, 10));

	uint32_t kernel_size = &kernend - &kernstart;
	uint32_t mem_size = mbd->mem_upper; // in KB
	printf("Installing pmm...\n");
	pmm_install(mbd, mem_size, &kernend);
	
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

void break_everything_completely(bool burn_it_all)
{
	printf("Poof! Your code is no more.");

	uint32_t* kernel_code = &kernstart;
 	
 	for(int i = 0; i < (&kernend - &kernstart); i++)
 	{
 		kernel_code[i] = burn_it_all ? 0xDEADBEEF : 0x31C0;
 	}

 	printf("I could literally say anything and do anything because I will never ever be executed.");
}