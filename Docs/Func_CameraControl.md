# NewConsoleVariable

## File Location

Defined in [Console.h](../V0.11/console.h).

## Syntax
```
int CameraControl(World *GameWorld, Camera *inputCamera)
```

## Description

This function is called every Game Tick to update the Camera's state and position. 

When adding new [CameraStates](Enum_CameraStates.md) this function should be where you make your additions. You may also modify the 
existing camera states.

## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| [World](Data_World.md)* | **GameWorld** | A pointer to the World that the camera is located in. |
| [Camera](Data_Camera.md) | **inputCamera** | A pointer to the camera to update. |

## Return Value

Returns a [FuncResult](Enum_FuncResult.md), indicating whether the operation was successful. (0 = LEMON_SUCCESS, -1 = LEMON_FAILURE, etc.)

## Version

Available since V0.10.

-----