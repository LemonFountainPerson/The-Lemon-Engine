# StartGame

## File Location

Defined in [EventManager.h](../V0.11/eventManager.h).

## Syntax
```
int StartGame(World *GameWorld)
```

## Description

This function is called after engine initialisation and by default loads the game into level 1, but you can put in whatever you need your game to do when it 
starts. (For example: playing a cutscene, loading into a main menu, logos, etc.)

This function is one the engine's Custom Callbacks. This means the function is undefined when the 'LEMON_USE_CUSTOM_CALLBACKS' macro is set to true, allowing you 
to define your own implementation, as long as it uses the same prototype. This is useful when working with the engine as a dynamic library instead of the source 
code directly.

## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| [World](Data_World.md) * | **GameWorld** | A pointer to a World struct. By default, this will be the main World created at engine start-up. |

## Return Value

Returns a [FuncResult](Enum_FuncResult.md), indicating whether the operation was successful. (0 = LEMON_SUCCESS, -1 = LEMON_FAILURE, etc.)

## Version

Available since V0.06.

-----