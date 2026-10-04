# NewConsoleVariable

## File Location

Defined in [Console.h](../V0.11/console.h).

## Syntax
```
ConsoleVariable* NewConsoleVariable(const char name[], const char helpString[], ConsoleVariableType valueType, const char value[], ConsoleFlag flags)
```

## Description

This function is used to create a new [ConsoleVariable](Data_ConsoleVariable.md). Console Variables cannot be deleted once created.

## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| const char[] | **name** | The name of the new variable. Will be forced to lower case and should not contain any whitespace. |
| const char[] | **helpString** | This is a short string that will be shown when info is requested on the variable. You can put what you like here, but generally you should give a short description of the variable's purpose. |
| [ConsoleVariableType](Enum_ConsoleVariableType.md) | **valueType** | The type of data this variable will store. It can store integers, floats, strings, etc. |
| const char[] | **value** | This is the initial value of the variable, given as a string. It will be converted automatically based on the given 'valueType'. |
| [ConsoleFlag](Enum_ConsoleFlag.md) | **flags** | This can be any combination of ConsoleFlags OR'd together to define additional attributes for your variable. |

## Return Value

Returns a pointer to the newly created [ConsoleVariable](Data_ConsoleVariable.md), or NULL on failure.

## Version

Available since V0.11.

-----