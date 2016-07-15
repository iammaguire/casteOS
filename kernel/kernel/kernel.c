#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kernel/tty.h"
#include "kernel/vga.h"
#include "kernel/multiboot.h"
#include "kernel/descriptor_tables.h"
#include "kernel/mm.h"

#define VIDMEM_PHYSICAL_ADDRESS		0xB8000

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
	mm_init(mbd);
}

void break_everything_completely(bool burn_it_all)
{
	printf("Poof! Your code is no more.");

	uint32_t* kernel_code = &kernel_start;

 	for(uint16_t i = 0; i < (kernel_end - kernel_start); i++)
 	{
 		kernel_code[i] = burn_it_all ? 0xDEADBEEF : 0xE920E81000;
 	}

 	printf("I could literally say anything and do anything because I will never ever be executed.");
}
