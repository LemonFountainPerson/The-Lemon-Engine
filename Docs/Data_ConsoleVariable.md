# ConsoleVariable

## Definition
```
typedef struct ConsoleVariable
{
	int nameLength;
	char name[MAX_LEN];
	char helpString[CONSOLE_HELP_MAX_LEN];
	ConsoleCommandFlag flags;

	ConsoleVariableData value;
	ConsoleVariableType valueType;
} ConsoleVariable;
```

## Description
ConsoleVariables are a type of variable that are exposed during runtime in the Developer Console. ConsoleVariables have identifying 
names, a data type defined by valueType ([ConsoleVariableType](Enum_ConsoleVariableType.md)) and can have attributes controlled by flags 
([ConsoleCommandFlag](Enum_ConsoleCommandFlag.md)).

New variables can be defined via [NewConsoleVariable()](Func_NewConsoleVariable.md).

## Version
Available since V0.11.


----