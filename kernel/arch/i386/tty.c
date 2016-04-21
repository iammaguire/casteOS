#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/vga.h>

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void update_cursor()
{
	unsigned short pos = (terminal_row * 80) + terminal_column;
	outportb(0x3D4, 0x0F);
	outportb(0x3D5, (unsigned char) (pos & 0xFF));
	outportb(0x3D4, 0x0E);
	outportb(0x3D5, (unsigned char) (pos >> 8) & 0xFF);
}

void scroll()
{
	terminal_row = VGA_HEIGHT - 1;
	
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			const size_t lastIndex = (y + 1) * VGA_WIDTH + x;
			terminal_buffer[index] = terminal_buffer[lastIndex];
		}
	}
}

void terminal_clear()
{
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			const size_t lastIndex = (y + 1) * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}

	terminal_row = 0;
	terminal_column = 0;
}

void terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_putchar(char c)
{
	if(c == '\b')
	{
		terminal_putentryat(' ', terminal_color, terminal_column - 1, terminal_row);
		terminal_column--;
		update_cursor();
		return;
	}

	if(c == '\n')
	{
		terminal_column = 0;
		terminal_row++;
		update_cursor();
		
		if(terminal_row == VGA_HEIGHT)
			scroll();
		
		return;
	}

	if(c == '\t')
	{
		terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
		terminal_putentryat(' ', terminal_color, terminal_column + 1, terminal_row);
		terminal_putentryat(' ', terminal_color, terminal_column + 2, terminal_row);
		terminal_putentryat(' ', terminal_color, terminal_column + 3, terminal_row);
		terminal_putentryat(' ', terminal_color, terminal_column + 4, terminal_row);
		terminal_putentryat(' ', terminal_color, terminal_column + 5, terminal_row);
		terminal_column += 5;
		update_cursor();
		return;
	}

	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

	if ( ++terminal_column == VGA_WIDTH )
	{
		terminal_column = 0;
		if ( ++terminal_row == VGA_HEIGHT )
		{
			scroll();
		}
	}

	update_cursor();
}

void terminal_write(const char* data, size_t size)
{
	for ( size_t i = 0; i < size; i++)
	{
		terminal_putchar(data[i]);
	}
}

void terminal_writestring(const char* data)
{
	terminal_write(data, strlen(data));
}
