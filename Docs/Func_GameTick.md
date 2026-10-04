# GameTick

## File Location

Defined in [LemonMain.h](../V0.11/LemonMain.h).

## Syntax
```
int GameTick(World *GameWorld)	
```

## Description

This function is called a consitent number of times per second, defined by the engine's TickRate. (For example tickrate = 60 means GameTick
will run 60 times per second.)
This function is responsible for delegating [Object](Data_Object.md) updates, Cutscene management, Camera control among other things. This
function does not normally need to be called by you, as the engine will handle calling it automatically. However, if you wish to inject 
custom code to be executed per GameTick, defining 'LEMON_USE_CUSTOM_CALLBACKS' will cause the [Tick()](Func_Tick.md) callback function to be called, which
you can define.


## Return Value

Returns a [FuncResult](Enum_FuncResult.md), indicating whether the operation was successful. (0 = LEMON_SUCCESS, -1 = LEMON_FAILURE, etc.)


## Version

Available since V0.05.

-------------------------------