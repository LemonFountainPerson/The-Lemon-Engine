# ButtonHeld

## File Location

Defined in [LemonMain.h](../V0.11/LemonMain.h).

## Syntax
```
bool ButtonHeld(LemonKey key)	
```

## Description

This function returns whether or not the provided [LemonKey](Enum_LemonKey.md) is held down. 

In order to check whether a button has been freshly pressed down during this GameTick, use [ButtonPressed()](Func_ButtonPressed.md).

## Inputs 

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| [LemonKey](Enum_LemonKey.md) | **key** | The button to check the state of. |

## Return Value

Returns a boolean value, true if the button is held down and false otherwise.

## Version

Available since V0.09.

-------------------------------