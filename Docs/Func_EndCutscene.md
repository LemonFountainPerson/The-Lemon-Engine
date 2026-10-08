# EndCutscene

## File Location

Defined in [cutsceneManager.h](../V0.11/cutsceneManager.h).

## Syntax
```
int EndCutscene(World *GameWorld)
```

## Description

This function is used to end whatever cutscene is currently playing, and free up the associated data if there is any. This function is 
called automatically when a scene reaches its end. 

The [Console Command](Data_ConsoleCommand.md) 'cutscene stop' also invokes this function.


## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| [World](Data_World.md) * | **GameWorld** | A pointer to a World struct. By default, this will be the main World created at engine start-up. |


## Return Value

Returns a [FuncResult](Enum_FuncResult.md), indicating whether the operation was successful. (0 = LEMON_SUCCESS, -1 = LEMON_FAILURE, etc.)


## Version

Available since V0.07.

-------------------------------