#include "kernel/fsys.h"

PFILESYSTEM _file_systems[DEVICE_MAX];

FILE vol_open_file (const char* fname)
{
	if(fname)
	{
		unsigned char device = 'a';
		char* filename = (char*) fname;

		if(fname[1] == ':')
		{
			device = fname[0];
			filename += 2;
		}

		if(_file_systems[device - 'a'])
		{
			FILE file = _file_systems[device - 'a']->open(filename);
			file.device = device;
			return file;
		}
	}

	FILE file;
	file.flags = FS_INVALID;
	return file;
}

void vol_register_file_system (PFILESYSTEM fsys, unsigned int device_id)
{
	if(device_id < DEVICE_MAX)
		if(fsys)
			_file_systems[device_id] = fsys;
}

void vol_unregister_file_system (PFILESYSTEM fsys)
{
	for(int i = 0; i < DEVICE_MAX; i++)
	{
		if(_file_systems[i] == fsys)
			_file_systems[i] = 0;
	}
}

void vol_unregister_file_system_by_id (unsigned int device_id)
{
	if(device_id < DEVICE_MAX)
			_file_systems[device_id] = 0;
}