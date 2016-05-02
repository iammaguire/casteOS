#include <kernel/multiboot.h>
#include <kernel/graphics.h>

#include <stddef.h>

void graphics_install(struct multiboot* mbd)
{
	info  = (struct vbe_mode_info_block *) mbd->vbe_mode_info;
	lfb = (uint8_t*) info->physbase;
	bytes_per_line = info->pitch;
	screen_width = info->Xres;
	screen_height = info->Yres;

	fill_rect(0, 0, screen_width, 12, 0xFFFFFF);
	fill_rect(0, screen_height - 12, screen_width, 12, 0xFFFFFF);
	fill_rect(0, 0, 12, screen_height, 0xFFFFFF);
	fill_rect(screen_width - 12, 0, 12, screen_height, 0xFFFFFF);

	draw_string("casteOS - by Connor Perkins", 11, 1, 0x000000, 0xFFFFFF);

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
}

void update_graphics()
{
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