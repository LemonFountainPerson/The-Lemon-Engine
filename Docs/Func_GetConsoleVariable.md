# GetConsoleVariable

## File Location

Defined in [Console.h](../V0.11/console.h).

## Syntax
```
ConsoleVariable* GetConsoleVariable(ConsoleVariable *variable)
```

## Description

This function is used to retrieve an existing [Console Variable](Data_ConsoleVariable.md) by its identifying name.

To set the value of a variable, use [SetConsoleVariable()](Func_SetConsoleVariable.md).


## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| const char [] | **name** | The name of the variable to get. The input will be forced to lowercase. |


## Return Value

This function returns a pointer to a [ConsoleVariable](Data_ConsoleVariable.md).


## Version

Available since V0.11.

-----