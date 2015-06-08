typedef struct _BOOT_SECTOR
{
	uint8_t ignore[3];
	BIOSPARAMETERBLOCK bpb;
	BIOSPARAMETERBLOCKEXT bpb_ext;
	uint8_t filler[448];
} BOOTSECTOR, *PBOOTSECTOR;