#define ATA_SR_BSY     				0x80    // Busy
#define ATA_SR_DRDY    				0x40    // Drive ready
#define ATA_SR_DF      				0x20    // Drive write fault
#define ATA_SR_DSC     				0x10    // Drive seek complete
#define ATA_SR_DRQ     				0x08    // Data request ready
#define ATA_SR_CORR    				0x04    // Corrected data
#define ATA_SR_IDX     				0x02    // Inlex
#define ATA_SR_ERR     				0x01    // Error

#define ATA_ER_BBK      			0x80    // Bad sector
#define ATA_ER_UNC      			0x40    // Uncorrectable data
#define ATA_ER_MC       			0x20    // No media
#define ATA_ER_IDNF     			0x10    // ID mark not found
#define ATA_ER_MCR      			0x08    // No media
#define ATA_ER_ABRT     			0x04    // Command aborted
#define ATA_ER_TK0NF    			0x02    // Track 0 not found
#define ATA_ER_AMNF     			0x01    // No address mark

#define ATA_CMD_READ_PIO          	0x20
#define ATA_CMD_READ_PIO_EXT      	0x24
#define ATA_CMD_READ_DMA          	0xC8
#define ATA_CMD_READ_DMA_EXT      	0x25
#define ATA_CMD_WRITE_PIO         	0x30
#define ATA_CMD_WRITE_PIO_EXT     	0x34
#define ATA_CMD_WRITE_DMA         	0xCA
#define ATA_CMD_WRITE_DMA_EXT     	0x35
#define ATA_CMD_CACHE_FLUSH      	0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   	0xEA
#define ATA_CMD_PACKET            	0xA0
#define ATA_CMD_IDENTIFY_PACKET   	0xA1
#define ATA_CMD_IDENTIFY          	0xEC

#define ATAPI_CMD_READ       		0xA8
#define ATAPI_CMD_EJECT      		0x1B

#define ATA_IDENT_DEVICETYPE   		0
#define ATA_IDENT_CYLINDERS    		2
#define ATA_IDENT_HEADS        		6
#define ATA_IDENT_SECTORS      		12
#define ATA_IDENT_SERIAL       		20
#define ATA_IDENT_MODEL        		54
#define ATA_IDENT_CAPABILITIES 		98
#define ATA_IDENT_FIELDVALID   		106
#define ATA_IDENT_MAX_LBA      		120
#define ATA_IDENT_COMMANDSETS  		164
#define ATA_IDENT_MAX_LBA_EXT  		200

#define IDE_ATA        				0x00
#define IDE_ATAPI      				0x01
 
#define ATA_MASTER     				0x00
#define ATA_SLAVE      				0x01

#define ATA_REG_DATA       			0x00
#define ATA_REG_ERROR      			0x01
#define ATA_REG_FEATURES   			0x01
#define ATA_REG_SECCOUNT0  			0x02
#define ATA_REG_LBA0       			0x03
#define ATA_REG_LBA1       			0x04
#define ATA_REG_LBA2       			0x05
#define ATA_REG_HDDEVSEL   			0x06
#define ATA_REG_COMMAND    			0x07
#define ATA_REG_STATUS     			0x07
#define ATA_REG_SECCOUNT1  			0x08
#define ATA_REG_LBA3       			0x09
#define ATA_REG_LBA4       			0x0A
#define ATA_REG_LBA5       			0x0B
#define ATA_REG_CONTROL    			0x0C
#define ATA_REG_ALTSTATUS  			0x0C
#define ATA_REG_DEVADDRESS 			0x0D

// Channels:
#define ATA_PRIMARY      			0x00
#define ATA_SECONDARY    			0x01
 
// Directions:
#define ATA_READ      				0x00
#define ATA_WRITE     				0x01

#define ATA_DATA(x)					(x+ATA_REG_DATA)	// Read or write PIO data bytes here
#define ATA_FEATURES(x)				(x+ATA_REG_FEATURES)	// Features or error information, usually used for ATAPI devices
#define ATA_SECTOR_COUNT(x)			(x+ATA_REG_SECCOUNT0)	// Number of sectors to read or write
#define ATA_SECTOR_NUM(x)			(x+ATA_REG_LBA0)	// CHS/LBA28/LBA48 specific, or LBAlo
#define ATA_CYLINDER_LOW(x)			(x+ATA_REG_LBA1)	// Partial disk sector address, or LBAmid
#define ATA_CYLINDER_HI(x)			(x+ATA_REG_LBA2)	// Partial disk sector address, or LBAhi
#define ATA_DRIVE(x)				(x+ATA_REG_HDDEVSEL)	// Select a drive and/or head here
#define ATA_COMMAND(x)				(x+ATA_REG_COMMAND)	// Send commands or read Regular Status here

#define PRIMARY_BASE 				0x1F0
#define SECONDARY_BASE				0x170

#define FLOATING_BUS				0xFF

#define ATA_TIMEOUT					30000000

typedef struct
{
	uint16_t base_port;
	uint8_t is_slave;
	uint8_t lba;
	uint8_t dma;
	uint16_t cylinders;
	uint16_t heads;
	uint16_t sectors;
	uint16_t capacity;
} ata_device_t;

extern ata_device_t a_p_m;
extern ata_device_t a_p_s;
extern ata_device_t a_s_m;
extern ata_device_t a_s_s;

extern ata_device_t *ATA_PRIMARY_MASTER;
extern ata_device_t *ATA_PRIMARY_SLAVE;
extern ata_device_t *ATA_SECONDARY_MASTER;
extern ata_device_t *ATA_SECONDARY_SLAVE;