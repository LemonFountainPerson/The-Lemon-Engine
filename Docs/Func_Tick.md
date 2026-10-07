# Tick

## File Location

Defined in [LemonMain.h](../V0.11/LemonMain.h).

## Syntax
```
[Any] Tick(World *GameWorld)	
```

## Description

This function is called once every GameTick.

This function is one the engine's Custom Callbacks. This function is not defined and is only called when the 'LEMON_USE_CUSTOM_CALLBACKS' macro
is defined. This means you provide a custom implementation for this function to then be invoked by the engine. This is useful when working with 
the engine as a dynamic library instead of the source code directly.

## Inputs 

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| [World](Data_World.md) * | **GameWorld** | A pointer to a World struct. By default, this will be the main World created at engine start-up. |

## Return Value

You can define any return value, as this is a custom callback.

## Version

Available since V0.10.

-------------------------------