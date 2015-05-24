#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kernel/shell.h"

command_table_t commands[MAX_COMMANDS];

int num_of_commands;

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
	add_new_command("help", "Displays this message.", help_command);
	add_new_command("hello", "Ayyy.", hello_command);
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