typedef struct fat_extBS_16
{
	//extended fat12 and fat16 stuff
	unsigned char		bios_drive_num;
	unsigned char		reserved1;
	unsigned char		boot_signature;
	unsigned int		volume_id;
	unsigned char		volume_label[11];
	unsigned char		fat_type_label[8];
 
}__attribute__((packed)) fat_extBS_16_t, *p_fat_extBS_16_t;
 
typedef struct fat_BS
{
	unsigned char 		bootjmp[3];
	unsigned char 		oem_name[8];
	unsigned short      bytes_per_sector;
	unsigned char		sectors_per_cluster;
	unsigned short		reserved_sector_count;
	unsigned char		table_count;
	unsigned short		root_entry_count;
	unsigned short		total_sectors_16;
	unsigned char		media_type;
	unsigned short		table_size_16;
	unsigned short		sectors_per_track;
	unsigned short		head_side_count;
	unsigned int 		hidden_sector_count;
	unsigned int 		total_sectors_32;
 
	//this will be cast to it's specific type once the driver actually knows what type of FAT this is.
	unsigned char		extended_section[54];
 
}__attribute__((packed)) fat_BS_t, *p_fat_BS_t;

typedef struct mount_info {
	uint32_t num_sectors;
	uint32_t fat_offset;
	uint32_t num_root_entries;
	uint32_t root_offset;
	uint32_t root_size;
	uint32_t fat_size;
	uint32_t fat_entry_size;

} mount_info_t, *p_mount_info_t;

typedef struct
{
	p_fat_BS_t boot_sector;
	p_fat_extBS_16_t ext_boot_sector;
	p_mount_info_t mnt_info;
} f16_device_t;

typedef struct _DIRECTORY {
	uint8_t   filename[8];           //filename
	uint8_t   ext[3];                //extension (8.3 filename format)
	uint8_t   attrib;                //file attributes
	uint8_t   reserved;
	uint8_t   time_created_ms;         //creation time
	uint16_t  time_created;
	uint16_t  date_created;           //creation date
	uint16_t  date_last_accessed;
	uint16_t  first_cluster_hi_bytes;
	uint16_t  last_mod_time;           //last modification date/time
	uint16_t  last_mod_date;
	uint16_t  first_cluster;          //first cluster of file data
	uint32_t  file_size;              //size in bytes

} DIRECTORY, *PDIRECTORY;

extern f16_device_t f16_devices[10];