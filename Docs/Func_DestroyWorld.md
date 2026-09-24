# DestroyWorld

## File Location

Defined in [LemonMain.h](../V0.11/LemonMain.h).

## Syntax
```
void DestroyWorld(World *GameWorld)	
```

## Description

This function is used to dispose of a World that is no longer in use. All [Objects](Data_Object.md), [SceneActions](Data_SceneAction.md), [Texts](Data_Text.md), etc. created from this World will be deleted after this function call, and thus any pointer to this data will become invalid.

## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| [World](Data_World.md) * | **GameWorld** | A pointer to a World struct. This pointer is invalid after this function call. |


## Version

Available since V0.10.

-------------------------------