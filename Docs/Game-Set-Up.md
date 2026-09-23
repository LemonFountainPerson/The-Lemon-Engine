
# StartGame

## File Location

Defined in [EventManager.h](../V0.11/eventManager.h).

## Syntax
```
int StartGame(World *GameWorld)
```

## Description

This function is called after engine initialisation and by default loads the game into level 1, but you can put whatever you need your game to do when it starts.
(For example: playing a cutscene, loading into a main menu, logos, etc.)

This function is one the engine's Custom Callbacks. This means the function is undefined when the 'LEMON_USE_CUSTOM_CALLBACKS' macro is set to true, allowing you 
to define your own implementation, as long as it uses the same prototype. This is useful when working with the engine as a dynamic library instead of the source 
code directly.

## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| [World](DataTypes.md#World) * | **GameWorld** | A pointer to a World struct. By default, this will be the main World created at engine start-up. |

## Return Value

Returns a [FuncResult](DataTypes.md#FuncResult), indicating whether the operation was successful. (0 = LEMON_SUCCESS, -1 = LEMON_FAILURE, etc.)

## Version

Available since V0.06.



# InitialiseWorld

## File Location

Defined in [LemonMain.h](../V0.11/LemonMain.h).

## Syntax
```
World* InitialiseWorld(void)
```

## Description

This function will return a pointer to a new initialised World struct, allocated on the heap. 
When finished with this World, you should call '[DestroyWorld](#DestroyWorld)' on it.


## Return Value

Returns a pointer to a [World](DataTypes.md#World) struct.

## Version

Available since V0.08.



# DestroyWorld

## File Location

Defined in [LemonMain.h](../V0.11/LemonMain.h).

## Syntax
```
void DestroyWorld(World *GameWorld)	
```

## Description

This function is used t dispose of a World that is no longer in use. All Objects, SceneActions, Texts, etc. created from this World
will be deleted after this function call, and thus any pointer to this data will become invalid.

## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| [World](DataTypes.md#World) * | **GameWorld** | A pointer to a World struct. This pointer is invalid after this function call. |


## Version

Available since V0.10.