#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/descriptor_tables.h>

void kernel_early(void)
{
	terminal_initialize();
	printf("Welcome to casteOS!\n\nInitializing GDT and IDT...");
	init_descriptor_tables();
	printf("\t\t[OK]\nInstalling IRQ...");
	irq_install();
	printf("\t\t\t\t[OK]\nInstalling keyboard driver...");
	install_keyboard();
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
