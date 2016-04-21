#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "kernel/fat16_driver.h"
#include "kernel/hard_disk_driver.h"
#include "kernel/vfs.h"

f16_device_t f16_devices[10];

void to_dos_filename(const char* filename, char* fname, unsigned int FNameLength) 
{
	unsigned int  i=0;

	if (FNameLength > 11)
		return;

	if (!fname || !filename)
		return;

	memset (fname, ' ', FNameLength);

	for (i=0; i < strlen(filename)-1 && i < FNameLength; i++) {

		if (filename[i] == '.' || i==8 )
			break;

		fname[i] = toupper_single (filename[i]);
	}

	if (filename[i]=='.') {
		for (int k=0; k<3; k++) {

			++i;
			if ( filename[i] )
				fname[8+k] = filename[i];
		}
	}

	for (i = 0; i < 3; i++)
		fname[8+i] = toupper_single (fname[8+i]);
}

FILE f16_dir(const char* dir_name)
{
	FILE file;
	unsigned short* buf;
	PDIRECTORY directory;

	char dos_fname[11];
	to_dos_filename(dir_name, dos_fname, 11);
	dos_fname[11]=0;

	for(int sector = 0; sector < 14; sector++)
	{ // f16_devices[0].mnt_info->root_offset + sector
		if(!ata_read_lba28(buf, f16_devices[0].mnt_info->root_offset + sector, 1, ATA_PRIMARY_MASTER))
		{
			printf("Read sector failed.\n");
			file.flags = FS_INVALID;
			return file;
		}

		directory = (PDIRECTORY) buf;

		for(int i = 0; i < 16; i++)
		{
			char name[11];
			memcpy(name, directory->filename, 11/2);
			name[11] = 0;
			//printf(" %s ", itoa_nbuf(f16_devices[0].mnt_info->root_offset, 10));

			if(strcmp(dos_fname, name) == 0)
			{
				strcpy(file.name, dir_name);
				file.id = 0;
				file.current_cluster = directory->first_cluster;
				file.eof = 0;
				file.file_length = directory->file_size;

				if(directory->attrib == 0x10)
					file.flags = FS_DIRECTORY;
				else
					file.flags = FS_FILE;

				return file;
			}

			directory++;
		}
	}

	file.flags = FS_INVALID;
	return file;
}

void f16_install()
{
	uint16_t mbr[256];
	uint16_t boot_record[256];
	uint16_t ext_boot_record[256];

	if(!ata_read_lba28(mbr, 0, 1, ATA_PRIMARY_MASTER))
	{
		printf("Read mbr failed.\n");
		return;
	}

	int part1_base = 0x1BE / 2; // divide account for 16 bit entries in array
	
	uint32_t base_sector = (uint32_t) (((mbr[part1_base + 8 / 2]) & 0xFF00) | ((mbr[part1_base + 5] >> 16) & 0x00FF));
	//total_sectors = (uint32_t) (((mbr[part1_base + 12 / 2]) & 0xFF00) | ((mbr[part1_base + 7] >> 16) & 0x00FF));
	
	if(!ata_read_lba28(boot_record, base_sector, 1, ATA_PRIMARY_MASTER))
	{
		printf("Read boot sector failed.\n");
		return;
	}

	if(!ata_read_lba28(ext_boot_record, base_sector + 1, 1, ATA_PRIMARY_MASTER))
	{
		printf("Read extended boot sector failed.\n");
		return;
	}

	p_fat_BS_t bs = (p_fat_BS_t) boot_record;
	p_fat_extBS_16_t ext_bs = (p_fat_extBS_16_t) ext_boot_record;

	p_mount_info_t mnt_info;
	mnt_info->num_sectors = bs->total_sectors_16;
	mnt_info->fat_offset = 1;
	mnt_info->fat_size = 512;
	mnt_info->fat_entry_size = 8;
	mnt_info->num_root_entries = bs->root_entry_count;
	mnt_info->root_offset = (bs->table_count * 512) + 1;
	mnt_info->root_size = (bs->root_entry_count * 32) / bs->bytes_per_sector;

	f16_devices[0] = (f16_device_t) { bs, ext_bs, mnt_info };

	FILE f = f16_dir("rootdir");	
	printf("\ti: %s\n", itoa_nbuf(f.flags, 10));
}