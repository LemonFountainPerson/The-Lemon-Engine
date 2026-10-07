# SetConsoleVariable

## File Location

Defined in [Console.h](../V0.11/console.h).

## Syntax
```
void setConsoleVariable(ConsoleVariable *variable, const char value[])
```

## Description

This function is used to set an existing [ConsoleVariable's](Data_ConsoleVariable.md) value. The value is input as a string to be
independent of the variable's ['valueType'](Enum_ConsoleVariableType.md). The input is appropriately converted to whatever type the
variable expects. (E.g: "1.56" will be converted to 1.56 if the variable is a Float type, and remain unchanged if it is a String 
type.)

## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| [ConsoleVariable](Data_ConsoleVariable.md) * | **variable** | The name of the variable to modify. The input will be forced to lowercase. |
| const char[] | **value** | This is the value to set the variable to, written as a string. |

## Version

Available since V0.11.

-----