#include <stdint.h>

#define FS_FILE 0
#define FS_DIRECTORY 1
#define FS_INVALID 2

#define DEVICE_MAX 26

typedef struct _FILE
{
	char name[32];
	uint32_t flags;
	uint32_t file_length;
	uint32_t id;
	uint32_t eof;
	uint32_t position;
	uint32_t current_cluster;
	uint32_t device;
} FILE, *PFILE;

typedef struct _FILE_SYSTEM
{
	char name[20];
	FILE (*directory) (const char* dir_name);
	void (*mount) ();
	void (*read) (PFILE file, unsigned char* buffer, unsigned int length);
	void (*close) (PFILE);
	FILE (*open) (const char* file_name);
} FILESYSTEM, *PFILESYSTEM;

extern FILE vol_open_file (const char* fname);
extern void vol_read_file (PFILE file, unsigned char* buffer, unsigned int length);
extern void vol_close_file (PFILE file);
extern void vol_register_file_system (PFILESYSTEM, unsigned int device_id);
extern void vol_unregister_file_system (PFILESYSTEM);
extern void vol_unregister_file_system_by_id (unsigned int device_id);