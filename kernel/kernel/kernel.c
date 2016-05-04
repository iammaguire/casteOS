#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kernel/tty.h"
#include "kernel/vga.h"
#include "kernel/multiboot.h"

struct multiboot* multiboot_info;

extern uint32_t kernstart;
extern uint32_t kernend;

uint32_t kernel_start = &kernstart;
uint32_t kernel_end = &kernend;

void kernel_early(struct multiboot* mbd, unsigned int magic)
{
	multiboot_info = mbd;

	//kernel_end = &kernend;
	//kernel_start = &kernstart;

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

	uint32_t kernel_size = kernel_end - kernel_start;
	uint32_t mem_size = mbd->mem_upper + mbd->mem_lower + 1024; // in KB

	printf("Kernel start: 0x%s\n", itoa_nbuf(kernel_start, 16));
	printf("Kernel end: 0x%s\n", itoa_nbuf(kernel_end, 16));
	printf("Kernel size: 0x%s\n", itoa_nbuf(kernel_size, 16));

	printf("Installing pmm...\n");
	pmm_install(mbd, kernel_size, kernel_start, mem_size, kernel_end);

	printf("PMM regions initialized: %s\n", itoa_nbuf(pmm_get_block_count(), 10));
	printf("PMM allocation blocks; used or reserved: %s\n", itoa_nbuf(pmm_get_use_block_count(), 10));
	printf("PMM free blocks: %s\n", itoa_nbuf(pmm_get_free_block_count(), 10));

	printf("\nPerforming PMM test:\n");

	uint32_t* p1 = (uint32_t*) pmm_alloc_block();
	printf("\tp1 allocated at 0x%s\n", itoa_nbuf(p1, 16));

	uint32_t* p2 = (uint32_t*) pmm_alloc_blocks(2);
	printf("\tp2 allocated at 0x%s with two blocks\n", itoa_nbuf(p2, 16));

	uint32_t* p3 = (uint32_t*) pmm_alloc_block();
	printf("\tp3 allocated at 0x%s\n", itoa_nbuf(p3, 16));

	pmm_free_block(p1);
	pmm_free_blocks(p2, 2);
	pmm_free_block(p3);

	printf("Freed p1, p2, p3 successfully\n");

	printf("\nInstalling keyboard driver...\n");
	keyboard_install();
	printf("\nInstalling PS/2 mouse driver...\n");
	mouse_install();
	printf("Installing timer driver...\n");
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

	//	init_shell();
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
		update_graphics();
		update_graphics();
		update_graphics();//shell();
		//timer_wait(10);
	}
	
	for(;;);
}

void break_everything_completely(bool burn_it_all)
{
	printf("Poof! Your code is no more.");

	uint32_t* kernel_code = &kernstart;
 	
 	for(int i = 0; i < (kernel_start - kernel_end); i++)
 	{
 		kernel_code[i] = burn_it_all ? 0xDEADBEEF : 0xE920E81000;
 	}

 	printf("I could literally say anything and do anything because I will never ever be executed.");
}