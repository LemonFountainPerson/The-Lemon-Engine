# PlayAnimation

## File Location

Defined in [animations.h](../V0.11/animations.h).

## Syntax
```
int PlayNewAnimation(const char desiredName[], int loopCount, DisplayData *inputData)
```

## Description

This function starts the requested animation, if it exists, on the given [DisplayData](Data_DisplayData.md). However, the Animation will
only play if the [DisplayData](Data_DisplayData.md) is already playing the Aniamtion you are requesting. The 
'loopCount' argument decides how many times the animation will repeat (1 = play once, 0 or less = play forever).

In order to play a requested Animation regardless of whatever animation is already playing, use [PlayAnimation()](Func_PlayAnimation.md).


## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| const char[] | **desiredName** | The name of the animation to play. |
| int | **loopCount** | The number of times to loop the animation. |
| [DisplayData](Data_DisplayData.md) * | **inputData** | The display data to play the animations on, usually connected to an [Object](Data_Object.md). |


## Return Value

Returns a [FuncResult](Enum_FuncResult.md), indicating whether the operation was successful. (0 = LEMON_SUCCESS, -1 = LEMON_FAILURE, etc.)


## Version

Available since V0.06.

-------------------------------