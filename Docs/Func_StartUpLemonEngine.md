# StartUpLemonEngine

## File Location

Defined in [LemonMain.h](../V0.11/LemonMain.h).

## Syntax
```
int StartUpLemonEngine(void)	
```

## Description

This function is called at engine start-up and is used to create the application window, initialise sub-systems and set up global variables.


## Return Value

Returns a [FuncResult](Enum_FuncResult.md), indicating whether the operation was successful. (0 = LEMON_SUCCESS, -1 = LEMON_FAILURE, etc.)


## Version

Available since V0.10.

-------------------------------