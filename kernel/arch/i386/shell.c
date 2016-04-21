#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kernel/shell.h"
//#include "kernel/floppy_driver.h"
#include "kernel/hard_disk_driver.h"

command_table_t commands[MAX_COMMANDS];
command_table_t history[MAX_COMMANDS_HISTORY];

int num_of_commands;
int commands_history_index;
int commands_history_operating_index = -1;

void scroll_history() // stupid thing isn't working, not using for now
{
	if(commands_history_operating_index == -1)
		return;

	char* name = history[commands_history_operating_index].name;
	for(unsigned int i = 0; i < strlen(name); i++)
	{
		((uint8_t*)stdin)[in_size + i] = name[i];
	}

	if(commands_history_operating_index >= 0)
		commands_history_operating_index--;
}

void add_new_command(char *name, char *description, void *func)
{
	if(num_of_commands + 1 < MAX_COMMANDS)
	{
		num_of_commands++;

		commands[num_of_commands].name = name;
		commands[num_of_commands].description = description;
		commands[num_of_commands].func = func;
	}
}

int findCommand(char *command)
{
	int ret;

	for(int i = 1; i < num_of_commands; i++)
	{
		ret = memcmp(command, commands[i].name, sizeof(command));
		if(ret == 0)
			return i;
	}

	return -1;
}

void init_shell()
{
	//add_key_hook(0xA1, scroll_history);

	add_new_command("help", "Displays this message.", help_command);
	add_new_command("hello", "Ayyy.", hello_command);
	add_new_command("reboot", "Reboots the computer.", reboot_command);
	add_new_command("read", "Reads some stuff.", read_command);
	add_new_command("write", "Writes some stuff.", write_command);
	add_new_command("mbr", "Reads mbr and displays data of primary master ata drive.", mbr_command);
	add_new_command("clear", "Clears the screen.", clear_command);
	add_new_command("", "", empty_command);
}

void shell()
{
	printf("le prompt>");

	char* input_string;
	getstr(input_string);

	int i = findCommand(input_string);

	if(i >= 0)
	{
	    void (*command_function)(void);
		command_function = commands[i].func;
		(*command_function)();
		history[commands_history_index++] = commands[i];

		commands_history_operating_index++;
	}
	else 
	{
		printf("Command not found...\n");
	}
}

void empty_command()
{

}

void help_command()
{
	printf("This is a cool and useful help command, huh?\n");
}

void hello_command()
{
	printf("Hi!! :)\n");
}

void reboot_command()
{
	reboot();
}

void write_command()
{	
	unsigned short buf[256 * 2];
	for(int i = 0; i < 256 * 2; i++)
	{
		buf[i] = 0;
	}
	buf[0] = 0xDEAD;
	buf[1] = 0xBEEF;
	buf[300] = 0x1EE7;
	buf[301] = 0xBAB3;

	if(!ata_write_lba28(buf, 0, 2, ATA_PRIMARY_MASTER))
	{
		printf("Write failed.\n");
		return;
	}
}

void mbr_command()
{
	uint16_t buf[256];

	if(!ata_read_lba28(buf, 0, 1, ATA_PRIMARY_MASTER))
	{
		printf("Read failed.\n");
		return;
	}

	int part1_base = 0x1BE / 2; // divide account for 16 bit entries in array
	
	printf("Starting sector (LBA): %s\n", itoa((uint32_t) (buf[part1_base + 8 / 2] | buf[part1_base + 5] >> 16), 0, 10));
	printf("Total sectors: %s\n", itoa((uint32_t) (buf[part1_base + 12 / 2] | buf[part1_base + 7] >> 16), 0, 10));
}

void read_command()
{
	char addrbuf[10];
	char secbuf[4];
	uint32_t addr = 0;
	uint32_t sec_ct = 0;

	printf("Start addr: ");
	getstr(addrbuf);
	printf("Sector count: ");
	getstr(secbuf);

	addr = atoi(addrbuf);
	sec_ct = atoi(secbuf);
	
	unsigned short buf[256 * sec_ct];

	if(!ata_read_lba28(buf, addr, sec_ct, ATA_PRIMARY_MASTER))
	{
		printf("Read failed.\n");
		return;
	}

	for (int i = 0; i < 256 * sec_ct; i++)
	{
    	//printf("%c", (uint8_t) buf[i]);//itoa((uint16_t) (buf[i]), 0, 16));
		if(buf[i] == 0)
			printf(".");
		else
		{
			//printf("%s ", itoa((uint16_t) (buf[i]), 0, 16));
			printf("%c", (uint8_t) buf[i]);printf("%c", (uint8_t) (buf[i] >> 8));
		}
	}

    printf("\n");
}

void clear_command()
{
	terminal_clear();
	//printf("\n");
}