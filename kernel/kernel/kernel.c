#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/tty.h>
#include <kernel/vga.h>

void kernel_early(void)
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
	// printf("Installing paging...\n");
	// paging_install();
	printf("Installing keyboard driver...\n");
	keyboard_install();
	printf("Installing timer driver...\n");
	timer_install();
	__asm__ __volatile__ ("sti");
	printf("Installing floppy driver...\n");
	floppy_install();
	printf("Initializing stdio...\n");
	init_stdio();
	printf("\nFinished initialization successfully, starting CLI...\n\n");
	init_shell();
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
