# SetLRPan

## File Location

Defined in [soundProcessor.h](../V0.11/soundProcessor.h).

## Syntax
```
SoundInstance* SetLRPan(SoundInstance *input, float pan)
```

## Description

Set a [SoundInstance's](Data_SoundInstance.md) panning ratio.

## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| [SoundInstance](Data_SoundInstance.md) | **input** | The sound instance to modify the panning on. |
| float | **pan** | The value used for panning; 0.0 for centered, -1.0 for all the way to the left, 1.0 for the right, etc. |



## Return Value

Returns a pointer to the [SoundInstance](Data_SoundInstance.md) you passed in.


## Version

Available since V0.09.

-------------------------------