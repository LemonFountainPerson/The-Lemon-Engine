# ButtonPressed

## File Location

Defined in [LemonMain.h](../V0.11/LemonMain.h).

## Syntax
```
bool ButtonPressed(LemonKey key)	
```

## Description

This function returns whether or not the provided [LemonKey](Enum_LemonKey.md) is pressed. If the button 
was pressed on the current GameTick then this function will return true. If it is released or if it has 
been held down for longer it will return false.

In order to check whether a button is simply held down regardless of timing, use [ButtonHeld()](Func_ButtonHeld.md).

## Inputs 

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| [LemonKey](Enum_LemonKey.md) | **key** | The button to check the state of. |

## Return Value

Returns a boolean value, true if the button is pressed and false otherwise.

## Version

Available since V0.09.

-------------------------------