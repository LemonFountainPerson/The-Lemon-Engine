# PlayAnimationByIndex

## File Location

Defined in [animations.h](../V0.11/animations.h).

## Syntax
```
int PlayAnimationByIndex(int index, int loopCount, DisplayData *inputData)
```

## Description

This function starts the requested animation, if it exists, on the given [DisplayData](Data_DisplayData.md). Animations are counted
up from 1, entering < 1 will cause the DisplayData to stop whatever animation it is currently playing, if there is any. The 
'loopCount' argument decides how many times the animation will repeat (1 = play once, 0 or less = play forever).

In order to play an Animation based on its name, use [PlayAnimation()](Func_PlayAnimation.md).


## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| int | **index** | The ID of the animation to play. |
| int | **loopCount** | The number of times to loop the animation. |
| [DisplayData](Data_DisplayData.md) * | **inputData** | The display data to play the animations on, usually connected to an [Object](Data_Object.md). |


## Return Value

Returns a [FuncResult](Enum_FuncResult.md), indicating whether the operation was successful. (0 = LEMON_SUCCESS, -1 = LEMON_FAILURE, etc.)


## Version

Available since V0.06.

-------------------------------