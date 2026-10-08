# PlayCutscene

## File Location

Defined in [eventManager.h](../V0.11/eventManager.h).

## Syntax
```
GameEvent* PlayCutscene(int scene, World *GameWorld)
```

## Description

This function is called to start a cutscene identified by a numerical ID. This function will use the [Game Event](Data_GameEvent.md)
system to perform the action safely, as depending on where in the code you are, manually starting a cutscene may not be memory safe.
This is because starting a cutscene involves clearing existing [SceneActions](Data_SceneAction.md) and certain functions can be invoked 
by these SceneActions, hence the conflict.


## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| int | **scene** | The ID of the cutscene to play. If it is not hardcoded, it will be searched for as a [LemonScript](Doc_LemonScript.md) file in the form 'SceneX' where X is the ID. |
| [World](Data_World.md) * | **GameWorld** | A pointer to the World struct to play the cutscene on. |


## Return Value

Returns a pointer to the [GameEvent](Data_GameEvent.md) created that will queue the cutscene to play, or NULL if unsuccessful.


## Version

Available since V0.09.

-------------------------------