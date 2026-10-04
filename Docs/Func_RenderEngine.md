# RenderEngine

## File Location

Defined in [LemonMain.h](../V0.11/LemonMain.h).

## Syntax
```
void RenderEngine(Camera renderCamera, World *GameWorld, SDL_Renderer *Screen)	
```

## Description

This function is called to render a [World](Data_World.md) from the perspective of a given camera.

## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| [Camera](Data_Camera.md) | **renderCamera** | The provided camera to render the game from. |
| [World](Data_World.md) | **GameWorld** | The provided camera to render the game from. |
| SDL_Renderer* | **Screen** | The SDL renderer that represents the screen. |



## Version

Available since V0.06.

-------------------------------