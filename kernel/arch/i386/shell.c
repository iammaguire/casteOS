#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kernel/shell.h"

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