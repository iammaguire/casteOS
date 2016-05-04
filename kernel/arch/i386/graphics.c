#include <stddef.h>
#include "kernel/multiboot.h"
#include "kernel/graphics.h"
#include "kernel/ps2mouse.h"

uint32_t mouse_pos_x = 200;
uint32_t mouse_pos_y = 200;

uint8_t* buffer;
uint32_t buffer_size;

extern uint32_t kernstart;
extern uint32_t kernend;

uint32_t kernel_start_addr = &kernstart;
uint32_t kernel_end_addr = &kernend;

uint16_t mouse_bmp[19] = {
	0b0110000000000000,
	0b0111000000000000,
	0b0110100000000000,
	0b0110010000000000,
	0b0110001000000000,
	0b0110000100000000,
	0b0110000010000000,
	0b0110000001000000,
	0b0110000000100000,
	0b0110000000010000,
	0b0110000000001000,
	0b0110000000000100,
	0b0110000001111100,
	0b0110001001000000,
	0b0110010100100000,
	0b0110100100100000,
	0b0111000010010000,
	0b0000000010010000,
	0b0000000001100000,
};

void graphics_install(struct multiboot* mbd)
{
	info  = (struct vbe_mode_info_block *) mbd->vbe_mode_info;
	lfb = (uint8_t*) info->physbase;
	bytes_per_line = info->pitch;
	screen_width = info->Xres;
	screen_height = info->Yres;

	buffer_size = screen_width * screen_height * 4;
	buffer = (uint8_t*) (0x100000 * 2); //pmm_alloc_blocks(buffer_size * 4 / 1024 / 4096 + 1);

	memset(buffer, 0xFF, buffer_size);

	/*fill_rect(0, 0, screen_width, 12, 0xFFFFFF);
	fill_rect(0, screen_height - 12, screen_width, 12, 0xFFFFFF);
	fill_rect(0, 0, 12, screen_height, 0xFFFFFF);
	fill_rect(screen_width - 12, 0, 12, screen_height, 0xFFFFFF);

	draw_string("casteOS - by Connor Perkins", 11, 0, 0x000000, 0xFFFFFF);

	draw_string(itoa_nbuf(lfb, 16), 500, 500, 0x00FF00, 0x0000000);

	int x = 12;
	int y = 12;

	for(int i = 0; i < 256; i++)
	{
		draw_char(i, x, y, 0xFFFFFF, 0x000000);

		if(x < screen_width - 20)
		{
			x += 8;
		} else {
			x = 20;
			y += 12;
		}
	}

	for(uint32_t r = 0; r <= 0xFF; r++)
	{
		for(int g = 0; g <= 0xFF; g++)
		{
			put_pixel(12 + r, 36 + g, (uint32_t) (r << 16) | (g << 8));
		}
	}

	for(uint32_t r = 0; r <= 0xFF; r++)
	{
		for(int b = 0; b <= 0xFF; b++)
		{
			put_pixel(12 + r + 0xFF, 36 + b, (uint32_t) (r << 16) | b);
		}
	}

	for(uint32_t g = 0; g <= 0xFF; g++)
	{
		for(int b = 0; b <= 0xFF; b++)
		{
			put_pixel(12 + g + 0xFF * 2, 36 + b, (uint32_t) (g << 8) | b);
		}
	}*/
}

void mouse_packet(mouse_device_packet_t packet)
{
	mouse_pos_x += packet.x_difference;
	mouse_pos_y -= packet.y_difference;
}

int pos = 0;

void update_graphics()
{
	clear_screen(0x000000);
	
	draw_string("casteOS - by Connor Perkins", 11, pos, 0x000000, 0xFFFFFF);
	
	fill_rect(0, 0, screen_width, 12, 0xFFFFFF);
	fill_rect(0, screen_height - 12, screen_width, 12, 0xFFFFFF);
	fill_rect(0, 0, 12, screen_height, 0xFFFFFF);
	fill_rect(screen_width - 12, 0, 12, screen_height, 0xFFFFFF);

	draw_string("casteOS - by Connor Perkins", 11, 0, 0x000000, 0xFFFFFF);

	draw_string(itoa_nbuf(lfb, 16), 500, 500, 0x00FF00, 0x0000000);

	int x = 12;
	int y = 12;

	for(int i = 0; i < 256; i++)
	{
		draw_char(i, x, y, 0xFFFFFF, 0x000000);

		if(x < screen_width - 20)
		{
			x += 8;
		} else {
			x = 20;
			y += 12;
		}
	}

	for(uint32_t r = 0; r <= 0xFF; r++)
	{
		for(int g = 0; g <= 0xFF; g++)
		{
			put_pixel(12 + r, 36 + g, (uint32_t) (r << 16) | (g << 8));
		}
	}

	for(uint32_t r = 0; r <= 0xFF; r++)
	{
		for(int b = 0; b <= 0xFF; b++)
		{
			put_pixel(12 + r + 0xFF, 36 + b, (uint32_t) (r << 16) | b);
		}
	}

	for(uint32_t g = 0; g <= 0xFF; g++)
	{
		for(int b = 0; b <= 0xFF; b++)
		{
			put_pixel(12 + g + 0xFF * 2, 36 + b, (uint32_t) (g << 8) | b);
		}
	}

	draw_mouse(mouse_pos_x, mouse_pos_y);
	
        	while ((inportb(0x3DA) & 0x08));
        	while (!(inportb(0x3DA) & 0x08));

	memcpy((uint8_t*) lfb, (uint8_t*) buffer, buffer_size);
}

void clear_screen(uint32_t color)
{
	fill_rect(0, 0, screen_width, screen_height, color);
}

void put_pixel(uint16_t x, uint16_t y, uint32_t color)
{
	if(x > screen_width || y > screen_height) return;

	unsigned where = x * pixel_width + y * bytes_per_line;
    	buffer[where] = color & 255;
    	buffer[where + 1] = (color >> 8) & 255;
    	buffer[where + 2] = (color >> 16) & 255;
}

void fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
	uint8_t *where = buffer + x * pixel_width + y * bytes_per_line; 

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			where[j * 4] = color & 255;
			where[j * 4 + 1] = (color >> 8) & 255;
			where[j * 4 + 2] = (color >> 16) & 255;
		}

		where += bytes_per_line;
	}
}

void draw_mouse(uint16_t x, uint16_t y)
{
	if(x < screen_width && y < screen_height)
	{
		uint16_t cx, cy;
		uint16_t mask[16] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 1 << 14, 1 << 15, 1 << 16 };

		for(cy = 0; cy < 19; cy++)
		{
			for(cx = 0; cx < 16; cx++)
			{
				put_pixel(x + cx, y + cy, mouse_bmp[cy] & mask[16 - cx] ? 0xFFFFFF : 0x000000);
			}
		}
	}
}

void draw_char(uint8_t c, uint16_t x, uint16_t y, uint32_t foreground, uint32_t background)
{
	uint16_t cx, cy;
	uint16_t mask[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

	for(cy = 0; cy < 12; cy++)
	{
		for(cx = 0; cx < 8; cx++)
		{
			put_pixel(x + cx, y + cy, font_8x12[c][cy] & mask[cx] ? foreground : background);
		}
	}
}

void draw_string(char* string, uint16_t x, uint16_t y, uint32_t foreground, uint32_t background)
{
	for(int i = 0; i < strlen(string); i++)
	{
		draw_char(string[i], x + (i * 8), y, foreground, background);
	}
}

void draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color) // bresenham line algorithm w/ integer arithmetic shits not working
{
	uint16_t delta_x = x2 - x1;
	uint16_t delta_y = y2 - y1;
	uint16_t delta_err = delta_y - delta_x;
	
	uint16_t y = y1;
	uint16_t x = x1;

	for(; x < x2 - 1; x++)
	{
		put_pixel(x, y, color);
		
		if(delta_err >= 0)
		{
			y += 1;
			delta_err -= delta_x;

			if(delta_err == delta_err + delta_y)
				break;
		}
	}
}