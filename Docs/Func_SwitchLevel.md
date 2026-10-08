# SwitchLevel

## File Location

Defined in [eventManager.h](../V0.11/eventManager.h).

## Syntax
```
GameEvent* SwitchLevel(int level, World *GameWorld)
```

## Description

This function is called to load into a new level identified by a numerical ID. This function will use the [Game Event](Data_GameEvent.md)
system to perform the action safely, as depending on where in the code you are, manually loading a level may not be memory safe.
This is because loading a level involves clearing existing [Objects](Data_Object.md) and other data.


## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| int | **level** | The ID of the level to load. If it is not hardcoded, it will be searched for as a [LemonScript](Doc_LemonScript.md) file in the form 'LevelX' where X is the ID. |
| [World](Data_World.md) * | **GameWorld** | A pointer to the World struct to play the cutscene on. |


## Return Value

Returns a pointer to the [GameEvent](Data_GameEvent.md) created that will queue the level to load, or NULL if unsuccessful.


## Version

Available since V0.08.

-------------------------------