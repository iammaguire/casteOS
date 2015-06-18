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

	printf("Welcome to casteOS!\n\nInitializing GDT and IDT..."); // eyy shitty \t's to do formatting. implement actual logging with pretty colors later.. 
	init_descriptor_tables();
	printf("\t\t[OK]\nInstalling IRQ...");
	irq_install();
	printf("\t\t\t\t[OK]\nInstalling paging...");
	printf("\t\t\t  [OK]\nInstalling keyboard driver...");
	keyboard_install();
	timer_install();
	floppy_install();
	printf("\t   [OK]\nInitializing stdio...");
	init_stdio();
	init_shell();
	printf("\t\t    [OK]\nFinished initialization successfully.\n\n");
	__asm__ __volatile__ ("sti");
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
