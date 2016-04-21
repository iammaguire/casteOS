#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "kernel/hard_disk_driver.h"

ata_device_t a_p_m = { PRIMARY_BASE, 0 };
ata_device_t a_p_s = { PRIMARY_BASE, 1 };
ata_device_t a_s_m = { SECONDARY_BASE, 0 };
ata_device_t a_s_s = { SECONDARY_BASE, 1 };

ata_device_t *ATA_PRIMARY_MASTER = &a_p_m;
ata_device_t *ATA_PRIMARY_SLAVE = &a_p_s;
ata_device_t *ATA_SECONDARY_MASTER = &a_s_m;
ata_device_t *ATA_SECONDARY_SLAVE = &a_s_s; // hmm...

unsigned char ata_wait_for_drive(ata_device_t *device, unsigned long timeout)
{
	ata_400ns_delay(device);
	unsigned char times = 0;
	do
	{
		unsigned char stat = inportb(device->base_port + 7);
		if(stat & 0x08 && !(stat & 0x80))
			return 0;
		else if((stat & 0x01 || stat & 0x20) && times < 4)
			return stat;
		times++;
		
		timer_wait(1);
	} while(--timeout);

	return -1;
}

void ata_400ns_delay(ata_device_t *device)
{
	for(int i = 0; i < 4; i++)
		inportb(device->base_port + 0x0C);
}

void ata_cache_flush(ata_device_t *device)
{
	//outportb(device->base_port + 6,  0xE0 | (device->is_slave << 4));
	outportb(device->base_port + 7, 0xE7);
	
	retry: 	ata_400ns_delay(device);
	unsigned char stat = inportb(device->base_port + 7);
	
	if(stat & 0x80) goto retry;
}

void ata_print_info(ata_device_t *device)
{
	printf("\t\tCylinders: %s, ", itoa_nbuf(device->cylinders, 10));
	printf("Heads: %s, ", itoa_nbuf(device->heads, 10));
	printf("Sectors: %s\n", itoa_nbuf(device->sectors, 10));

	printf("\t\tDMA: %s, ", itoa_nbuf(device->dma, 10));
	printf("LBA: %s\n", itoa_nbuf(device->lba, 10));
	
	printf("\t\tCapacity: %s", itoa_nbuf(device->capacity, 10));
}

unsigned char ata_poll_drive_info(unsigned short drive, unsigned char arg, ata_device_t *device)
{
	outportb(drive + 6, arg);
	outportb(drive + 2, 0x00);
	outportb(drive + 3, 0x00);
	outportb(drive + 4, 0x00);
	outportb(drive + 5, 0x00);
	outportb(drive + 7, 0xEC);
	//timer_wait(10);
	unsigned char iden = inportb(drive + 7);

	if(iden == 0) return 0;

	while((inportb(drive + 7) & 0x80));

	uint8_t stat = inportb(drive + 4) + inportb(drive + 5);
	if(stat != 0) return 0;

	while(1)
	{
		unsigned char s = inportb(drive + 7);
		if(s & 1) 
		{
			printf("\tError occurred while polling drive at 0x%s\n", itoa(s, 0, 16));
			return 0;
		}
		else if (s & 0x8)
			break;
	}

	unsigned short buffer[256];
	insw(drive, buffer, 256);

	device->base_port = drive;
	device->is_slave = arg == 0xB0;
	
	device->lba = (buffer[49] >> 9) & 1;  
    device->dma = (buffer[49] >> 8) & 1;
    
    device->cylinders = (uint16_t) buffer[1];  
    device->heads = (uint16_t) buffer[3];  
    device->sectors = (uint16_t) buffer[6];  
  
    if (device->lba) {  
        device->capacity = (uint16_t) buffer[60];  
    	if(device->capacity == 0) goto other_capacity;
    } else {  
    	other_capacity: device->capacity = device->heads * device->sectors * device->cylinders;  
    }

	return 1;
}

unsigned char ata_read_one_lba28(unsigned short buffer[], uint32_t lba, ata_device_t *device)
{
	outportb(device->base_port + 6,  0xE0 | (device->is_slave << 4) | ((lba >> 24) & 0x0F));
	outportb(device->base_port + 1, 0);
	outportb(device->base_port + 2, (unsigned char) 1);
	outportb(device->base_port + 3, (unsigned char) lba);
	outportb(device->base_port + 4, (unsigned char)(lba >> 8));
	outportb(device->base_port + 5, (unsigned char)(lba >> 16));
	outportb(device->base_port + 7, 0x20);

	timer_wait(1);

	unsigned char stat = ata_wait_for_drive(device, ATA_TIMEOUT);
	if(stat == -1)
	{
		printf("Drive timed out...");
		return 0;
	}

	if(stat != 0)
	{
		char* str;
		if(stat & 0x01 && !(stat & 0x20))
			str = "ERR";
		else if(stat & 0x20 && !(stat & 0x01))
			str = "DF";
		else if(stat & 0x20 && stat & 0x01)
			str = "ERR and DF";
		
		printf("Error reading from drive at 0x%s", itoa(device->base_port, 0, 16));
		printf(" with %s flag(s) set.\n", str);
		return 0;
	}

	insw(device->base_port, buffer, 256);

	ata_400ns_delay(device);
}

unsigned char ata_read_lba28(unsigned short buffer[], uint32_t lba, unsigned char sectors, ata_device_t *device)
{
	for(int i = 0; i < sectors; i++)
	{
		if(!ata_read_one_lba28(buffer + (i * 256), lba + i, device))
		{
			printf("Failed to read drive 0x%s", itoa(device->base_port, 0, 16));
			printf(" at sector 0x%s.\n", itoa(lba + i, 0, 16));
			return 0;
		}
	}
}

unsigned char ata_write_one_lba28(unsigned short buffer[], uint32_t lba, ata_device_t *device)
{
	outportb(device->base_port + 6,  0xE0 | (device->is_slave << 4) | ((lba >> 24) & 0x0F));
	outportb(device->base_port + 1, 0);
	outportb(device->base_port + 2, (unsigned char) 1);
	outportb(device->base_port + 3, (unsigned char) lba);
	outportb(device->base_port + 4, (unsigned char)(lba >> 8));
	outportb(device->base_port + 5, (unsigned char)(lba >> 16));
	outportb(device->base_port + 7, 0x30);

	timer_wait(1);

	unsigned char stat = ata_wait_for_drive(device, ATA_TIMEOUT);
	if(stat == -1)
	{
		printf("Drive timed out...");
		return 0;
	}

	if(stat != 0)
	{
		char* str;
		if(stat & 0x01 && !(stat & 0x20))
			str = "ERR";
		else if(stat & 0x20 && !(stat & 0x01))
			str = "DF";
		else if(stat & 0x20 && stat & 0x01)
			str = "ERR and DF";
		
		printf("Error writing to drive at 0x%s", itoa(device->base_port, 0, 16));
		printf(" with %s flag(s) set.\n", str);
		return 0;
	}

	ata_cache_flush(device);

	//insw(device->base_port, buffer, 256);
	for(int i = 0; i < 256; i++)
	{
		outportw(device->base_port, buffer[i]);
		inportb(device->base_port + 0x0C);
	}
}

unsigned char ata_write_lba28(unsigned short buffer[], uint32_t lba, unsigned char sectors, ata_device_t *device)
{
	for(int i = 0; i < sectors; i++)
	{
		if(!ata_write_one_lba28(buffer + (i * 256), lba + i, device))
		{
			printf("Failed to write drive 0x%s", itoa(device->base_port, 0, 16));
			printf(" at sector 0x%s.\n", itoa(lba + i, 0, 16));
			return 0;
		}
	}
}

void ata_install()
{
	printf("Detecting hard drives:\n");
	printf("\tPolling controllers...\n");

	outportb(ATA_SECTOR_NUM(PRIMARY_BASE), 0x88); // 0x88 magic value; if inb from port == magic value then controller exists
	unsigned char ex = inportb(ATA_SECTOR_NUM(PRIMARY_BASE));

	if(ex == 0x88)
		printf("\t\t- Primary controller exists.\n");
	
	ex = 0;

	outportb(ATA_SECTOR_NUM(SECONDARY_BASE), 0x88);
	ex = inportb(ATA_SECTOR_NUM(SECONDARY_BASE));
	
	if(ex == 0x88)
		printf("\t\t- Secondary controller exists.\n");

	outportb(ATA_DRIVE(PRIMARY_BASE), 0xA0);
	timer_wait(1);
	ex = inportb(ATA_COMMAND(PRIMARY_BASE));

	uint8_t primary_comm = ATA_COMMAND(PRIMARY_BASE);
	uint8_t secondary_comm = ATA_COMMAND(SECONDARY_BASE);

	if(ata_poll_drive_info(PRIMARY_BASE, 0xA0, ATA_PRIMARY_MASTER))
	{
		printf("\tPrimary master drive exists.\n");
		ata_print_info(ATA_PRIMARY_MASTER);
	}
	else
		printf("\tPrimary master drive doesn't exist, or an error occurred while polling.\n");

	if(ata_poll_drive_info(PRIMARY_BASE, 0xB0, ATA_PRIMARY_SLAVE))
	{
		printf("\tPrimary slave drive exists.\n");
		ata_print_info(ATA_PRIMARY_SLAVE);
	}
	else
		printf("\tPrimary slave drive doesn't exist, or an error occurred while polling.\n");

	if(ata_poll_drive_info(SECONDARY_BASE, 0xA0, ATA_SECONDARY_MASTER))
	{
		printf("\tSecondary master drive exists.\n");
		ata_print_info(ATA_SECONDARY_MASTER);
	}
	else
		printf("\tSecondary master drive doesn't exist, or an error occurred while polling.\n");

	if(ata_poll_drive_info(SECONDARY_BASE, 0xB0, ATA_SECONDARY_SLAVE))
	{
 		printf("\tSecondary slave drive exists.\n");
		ata_print_info(ATA_SECONDARY_SLAVE);
	}
	else
		printf("\tSecondary slave drive doesn't exist, or an error occurred while polling.\n");
}