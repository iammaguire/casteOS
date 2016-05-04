#include <stdint.h>
#include <stdio.h>
#include "kernel/isr.h"
#include "kernel/ps2mouse.h"

#define MOUSE_IRQ 12

#define MOUSE_PORT   0x60
#define MOUSE_STATUS 0x64
#define MOUSE_ABIT   0x02
#define MOUSE_BBIT   0x01
#define MOUSE_WRITE  0xD4
#define MOUSE_F_BIT  0x20
#define MOUSE_V_BIT  0x08

#define MOUSE_DEFAULT 0
#define MOUSE_SCROLLWHEEL 1
#define MOUSE_BUTTONS 2

int8_t mouse_mode = MOUSE_DEFAULT;
uint8_t mouse_cycle = 0;
int8_t  mouse_byte[4];

void mouse_handler(registers_t *regs)
{
	uint8_t status = inportb(MOUSE_STATUS);
	mouse_device_packet_t packet;

	while (status & MOUSE_BBIT) {
		int8_t mouse_in = inportb(MOUSE_PORT);
	
		if (status & MOUSE_F_BIT) 
		{
			switch (mouse_cycle) 
			{
				case 0:
					mouse_byte[0] = mouse_in;
					if (!(mouse_in & MOUSE_V_BIT)) { goto read_next; }
					++mouse_cycle;
					break;
				case 1:
					mouse_byte[1] = mouse_in;
					++mouse_cycle;
					break;
				case 2:
					mouse_byte[2] = mouse_in;
					if (mouse_mode == MOUSE_SCROLLWHEEL || mouse_mode == MOUSE_BUTTONS) 
					{
						++mouse_cycle;
						break;
					}
					goto finish_packet;
				case 3:
					mouse_byte[3] = mouse_in;
					goto finish_packet;
			}

			goto read_next;
finish_packet:
			mouse_cycle = 0;
			if (mouse_byte[0] & 0x80 || mouse_byte[0] & 0x40) 
			{
				goto read_next;
			}

			packet.magic = MOUSE_MAGIC;
			packet.x_difference = mouse_byte[1];
			packet.y_difference = mouse_byte[2];

			packet.buttons = 0;

			if (mouse_byte[0] & 0x01) 
			{
				packet.buttons |= LEFT_CLICK;
			}
			
			if (mouse_byte[0] & 0x02) 
			{
				packet.buttons |= RIGHT_CLICK;
			}
			
			if (mouse_byte[0] & 0x04) 
			{
				packet.buttons |= MIDDLE_CLICK;
			}

			if (mouse_mode == MOUSE_SCROLLWHEEL && mouse_byte[3]) {
				if (mouse_byte[3] > 0) 
				{
					packet.buttons |= MOUSE_SCROLL_DOWN;
				} 
				else if (mouse_byte[3] < 0) 
				{
					packet.buttons |= MOUSE_SCROLL_UP;
				}
			}

		}

read_next:
		status = inportb(MOUSE_STATUS);
	}

	irq_ack(MOUSE_IRQ);
	mouse_packet(packet);
}

void mouse_wait(uint8_t a_type) {
	uint32_t timeout = 100000;
	if (!a_type) {
		while (--timeout) {
			if ((inportb(MOUSE_STATUS) & MOUSE_BBIT) == 1) {
				return;
			}
		}
		
		return;
	} else {
		while (--timeout) {
			if (!((inportb(MOUSE_STATUS) & MOUSE_ABIT))) {
				return;
			}
		}
		
		return;
	}
}

void mouse_write(uint8_t write) {
	mouse_wait(1);
	outportb(MOUSE_STATUS, MOUSE_WRITE);
	mouse_wait(1);
	outportb(MOUSE_PORT, write);
}

uint8_t mouse_read(void) {
	mouse_wait(0);
	char t = inportb(MOUSE_PORT);
	return t;
}

void mouse_install() {
	uint8_t status, result;
	
	mouse_wait(1);
	outportb(MOUSE_STATUS, 0xA8);
	mouse_wait(1);
	outportb(MOUSE_STATUS, 0x20);
	mouse_wait(0);
	
	status = inportb(0x60) | 2;
	
	mouse_wait(1);
	outportb(MOUSE_STATUS, 0x60);
	mouse_wait(1);
	outportb(MOUSE_PORT, status);
	
	mouse_write(0xF6);
	mouse_read();
	mouse_write(0xF4);
	mouse_read();
	
	/* Try to enable scroll wheel (but not buttons) */
	if (1) {
		mouse_write(0xF2);
		mouse_read();
		result = mouse_read();
		mouse_write(0xF3);
		mouse_read();
		mouse_write(200);
		mouse_read();
		mouse_write(0xF3);
		mouse_read();
		mouse_write(100);
		mouse_read();
		mouse_write(0xF3);
		mouse_read();
		mouse_write(80);
		mouse_read();
		mouse_write(0xF2);
		mouse_read();
		result = mouse_read();
		if (result == 3) {
			mouse_mode = MOUSE_SCROLLWHEEL;
		}
	}

	irq_install_handler(MOUSE_IRQ, mouse_handler);

	uint8_t tmp = inportb(0x61);
	outportb(0x61, tmp | 0x80);
	outportb(0x61, tmp & 0x7F);
	inportb(MOUSE_PORT);
}