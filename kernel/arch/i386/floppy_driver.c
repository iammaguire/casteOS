#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "kernel/isr.h"
#include "kernel/floppy_driver.h"

void flpydsk_initialize_dma()
{
	outportb(0x0a, 0x06);
	outportb(0xd8, 0xff);
	outportb(0x04, 0);
	outportb(0x05, 0x10);
	outportb(0xd8, 0xff);
	outportb(0x05, 0xff);
	outportb(0x05, 0x23);
	outportb(0x80, 0);
	outportb(0x0a, 0x02);
}

void flpydsk_dma_read()
{
	outportb(0x0a, 0x06);
	outportb(0x0b, 0x56);
	outportb(0x0a, 0x02);
}

void flpydsk_dma_write()
{
	outportb(0x0a, 0x06);
	outportb(0x0b, 0x5a);
	outportb(0x0a, 0x02);
}

void floppy_handler(registers_t *regs)
{
	floppy_irq_fired = 1;
}

void wait_flpydsk_irq()
{
	while(floppy_irq_fired == 0) 
	;
	floppy_irq_fired = 0;
}

void flpydsk_write_dor(uint8_t val)
{
	outportb(FLPYDSK_DOR, val);
}

uint8_t flpydsk_read_status()
{
	return inportb(FLPYDSK_MSR);
}

void flpydsk_send_command(uint8_t cmd)
{
	for(int i = 0; i < 500; i++)
		if(flpydsk_read_status() & FLPYDSK_MSR_MASK_DATAREG)
			return outportb(FLPYDSK_FIFO, cmd);
}

uint8_t flpydsk_read_data()
{
	for(int i = 0; i < 500; i++)
		if(flpydsk_read_status() & FLPYDSK_MSR_MASK_DATAREG)
			return inportb(FLPYDSK_FIFO);	
}

void flpydsk_write_ccr(uint8_t val)
{
	outportb(FLPYDSK_CTRL, val);
}

void flpydsk_check_int(uint32_t* st0, uint32_t* cyl)
{
	flpydsk_send_command(FDC_CMD_CHECK_INT);
	
	*st0 = flpydsk_read_data();
	*cyl = flpydsk_read_data();
}

int flpydsk_seek(uint32_t cyl, uint32_t head)
{
	uint32_t st0, cyl0;
	
	if(current_drive >= 4)
		return -1;

	for(int i = 0; i < 10; i++)
	{
		flpydsk_send_command(FDC_CMD_SEEK);
		flpydsk_send_command((head) << 2 | current_drive);
		flpydsk_send_command(cyl);

		wait_flpydsk_irq();
		flpydsk_check_int(&st0, &cyl);
		
		if(cyl0 == cyl)
			return 0;
	}

	return -1;
}

void flpydsk_disable_controller()
{
	flpydsk_write_dor(0);
}

void flpydsk_enable_controller()
{
	flpydsk_write_dor(FLPYDSK_DOR_MASK_RESET | FLPYDSK_DOR_MASK_DMA);
}

void flpydsk_reset()
{
	uint32_t st0, cyl;
	
	flpydsk_disable_controller();
	flpydsk_enable_controller();

	wait_flpydsk_irq();
	 
	for(int i = 0; i < 4; i++)
		flpydsk_check_int(&st0, &cyl);

	flpydsk_write_ccr(0);

	flpydsk_drive_data(3, 16, 240, true);

	flpydsk_calibrate(current_drive);
}

void flpydsk_drive_data(uint32_t stepr, uint32_t loadt, uint32_t unloadt, bool dma)
{
	uint32_t data = 0;

	flpydsk_send_command(FDC_CMD_SPECIFY);

	data = ((stepr & 0xf) << 4) | (unloadt & 0xf);
	flpydsk_send_command(data);

	data = (loadt) << 1 | (dma == true) ? 1 : 0;
	flpydsk_send_command(data);
}

int flpydsk_calibrate(uint32_t drive)
{
	uint32_t st0, cyl;
	
	if(drive >= 4)
		return -2;

	flpydsk_control_motor(true);
	
	for(int i = 0; i < 10; i++)
	{
		flpydsk_send_command(FDC_CMD_CALIBRATE);
		flpydsk_send_command(drive);
		wait_flpydsk_irq();
		flpydsk_check_int(&st0, &cyl);
		
		if(!cyl)
		{
			flpydsk_control_motor(false);
			return 0;
		}
	}

	flpydsk_control_motor(false);
	return -1;
}

void flpydsk_read_sector_imp(uint8_t head, uint8_t track, uint8_t sector)
{
	uint8_t st0, cyl;
	
	flpydsk_dma_read();

	flpydsk_send_command(FDC_CMD_READ_SECT | FDC_CMD_EXT_MULTITRACK |  FDC_CMD_EXT_SKIP | FDC_CMD_EXT_DENSITY);
	flpydsk_send_command(head << 2 | current_drive);
	flpydsk_send_command(track);
	flpydsk_send_command(head);
	flpydsk_send_command(sector);
	flpydsk_send_command(FLPYDSK_SECTOR_DTL_512);
	flpydsk_send_command(((sector + 1) >= FLPY_SECTORS_PER_TRACK) ? FLPY_SECTORS_PER_TRACK : sector + 1);
	flpydsk_send_command(FLPYDSK_GAP3_LENGTH_3_5);
	flpydsk_send_command(0xff);
	
	wait_flpydsk_irq();

	for(int j = 0; j < 7; j++)
		flpydsk_read_data();

	flpydsk_check_int(&st0, &cyl);
}

void flpydsk_lba_to_chs(int lba, int *head, int *track, int *sector)
{
	*head = (lba % (FLPY_SECTORS_PER_TRACK * 2)) / (FLPY_SECTORS_PER_TRACK);
	*track = lba / (FLPY_SECTORS_PER_TRACK * 2);
	*sector = lba % FLPY_SECTORS_PER_TRACK + 1;
}

uint8_t* flpydsk_read_sector(int sectorLBA)
{
	if(current_drive >= 4)
		return 0;

	int head = 0, track = 0, sector = 0;
	flpydsk_lba_to_chs(sectorLBA, &head, &track, &sector);
	
	flpydsk_control_motor(true);
	if(flpydsk_seek(track, head) != 0)
		return 0;

	flpydsk_read_sector_imp(head, track, sector);
	flpydsk_control_motor(false);
	
	return (uint8_t*) DMA_BUFFER;
}

void flpydsk_control_motor(bool b)
{
	if (current_drive > 3)
		return;

	uint32_t motor = 0;

	switch (current_drive) {

		case 0:
			motor = FLPYDSK_DOR_MASK_DRIVE0_MOTOR;
			break;
		case 1:
			motor = FLPYDSK_DOR_MASK_DRIVE1_MOTOR;
			break;
		case 2:
			motor = FLPYDSK_DOR_MASK_DRIVE2_MOTOR;
			break;
		case 3:
			motor = FLPYDSK_DOR_MASK_DRIVE3_MOTOR;
			break;
	}

	if (b)
		flpydsk_write_dor(current_drive | motor | FLPYDSK_DOR_MASK_RESET | FLPYDSK_DOR_MASK_DMA);
	else
		flpydsk_write_dor(FLPYDSK_DOR_MASK_RESET);

	timer_wait(20);
}

void flpydsk_detect_drives()
{
	outportb(0x70, 0x10);
	unsigned drives = inportb(0x71);
	
	printf("Detecting drives: \n");
	printf("\t- Floppy drive 0: %s\n", drive_types[drives >> 4]);
	printf("\t- Floppy drive 1: %s\n", drive_types[drives & 0xf]);
}

void floppy_install()
{
	irq_install_handler(6, floppy_handler);

	flpydsk_initialize_dma();

	flpydsk_detect_drives();

	flpydsk_reset();
	flpydsk_drive_data(13, 1, 0xf, true);
}
