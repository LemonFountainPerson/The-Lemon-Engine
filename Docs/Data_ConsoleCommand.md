# ConsoleCommand

## Definition
```
typedef struct ConsoleCommand
{
	char name[MAX_LEN];
	char helpString[CONSOLE_HELP_MAX_LEN];
	ConsoleFlag flags;

	char formatString[MAX_LEN];
	ConsoleCommandFunction function;
} ConsoleCommand;
```

## Description
ConsoleCommands are commands that can be run from the [Developer Console](Doc_DeveloperConsole.md) during runtime. Commands are always lowercase keywords 
that are defined at engine start-up, and never changed. 

The [ConsoleCommandFunction](Data_ConsoleCommandFunction.md) is a pointer to the function that will be called when the command is run. This function is always 
given a pointer to the currently in-use [World](Data_World.md) and the arguments given with this command, if any.

The helpString is simply used to describe the command when information is requested, and the formatString is meant to show how to use the command by showing
what kinds of arguments it expects.

## Version
Available since V0.10.


----