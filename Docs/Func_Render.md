# Render

## File Location

Defined in [LemonMain.h](../V0.11/LemonMain.h).

## Syntax
```
int Render(World *GameWorld, RenderFrame *ScreenData)
```

## Description

This function is called to render the game to the screen.

## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| [World](Data_World.md) | **GameWorld** | The provided camera to render the game from. |
| [RenderFrame](Data_RenderFrame.md)* | **ScreenData** | The data representing the Screen, such as the global 'ScreenData' which is used as the default screen. |


## Return Value

Returns a [FuncResult](Enum_FuncResult.md), indicating whether the operation was successful. (0 = LEMON_SUCCESS, -1 = LEMON_FAILURE, etc.)


## Version

Available since V0.09.

-------------------------------