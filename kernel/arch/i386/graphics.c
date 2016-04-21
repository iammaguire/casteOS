#include <kernel/multiboot.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t *lfb;
uint8_t pixel_width = 4; // booting into 32bbp so whatever

uint16_t screen_width;
uint16_t screen_height;
uint16_t bytes_per_line;

struct vbe_mode_info_block *info;

void graphics_install(struct multiboot* mbd)
{
	info  = (struct vbe_mode_info_block *) mbd->vbe_mode_info;
	lfb = (uint8_t*) info->physbase;
	bytes_per_line = info->pitch;
	screen_width = info->Xres;
	screen_height = info->Yres;
}

void put_pixel(uint16_t x, uint16_t y, uint32_t color)
{
	if(x > screen_width || y > screen_height) return;

	unsigned where = x * pixel_width + y * bytes_per_line;
    	lfb[where] = color & 255;              	// BLUE
    	lfb[where + 1] = (color >> 8) & 255;   	// GREEN
    	lfb[where + 2] = (color >> 16) & 255;  	// RED
}