#include <kernel/multiboot.h>
#include <kernel/graphics.h>

void graphics_install(struct multiboot* mbd)
{
	info  = (struct vbe_mode_info_block *) mbd->vbe_mode_info;
	lfb = (uint8_t*) info->physbase;
	bytes_per_line = info->pitch;
	screen_width = info->Xres;
	screen_height = info->Yres;

	fill_rect(0, 0, screen_width, 10, 0xFFFFFF);
	fill_rect(0, screen_height - 10, screen_width, 10, 0xFFFFFF);
	fill_rect(0, 0, 10, screen_height, 0xFFFFFF);
	fill_rect(screen_width - 10, 0, 10, screen_height, 0xFFFFFF);

	fill_rect(15, 15, screen_width - 30, screen_height - 30, 0x20BB20);

	draw_string("casteOS - by Connor Perkins", 11, 1, 0x000000, 0xFFFFFF);
}

void put_pixel(uint16_t x, uint16_t y, uint32_t color)
{
	if(x > screen_width || y > screen_height) return;

	unsigned where = x * pixel_width + y * bytes_per_line;
    	lfb[where] = color & 255;
    	lfb[where + 1] = (color >> 8) & 255;
    	lfb[where + 2] = (color >> 16) & 255;
}

void fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
	uint8_t *where = lfb + x * pixel_width + y * bytes_per_line; 

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

void draw_char(uint8_t c, uint16_t x, uint16_t y, uint32_t foreground, uint32_t background)
{
	uint16_t cx, cy;
	uint16_t mask[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

	for(cy = 0; cy < 8; cy++)
	{
		for(cx = 0; cx < 8; cx++)
		{
			put_pixel(x + cx, y + cy, font8x8_basic[c][cy] & mask[cx] ? foreground : background);
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