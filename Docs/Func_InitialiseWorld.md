# InitialiseWorld

## File Location

Defined in [LemonMain.h](../V0.11/LemonMain.h).

## Syntax
```
World* InitialiseWorld(void)
```

## Description

This function will return a pointer to a new initialised World struct, allocated on the heap. 
When finished with this World, you should call '[DestroyWorld()](Func_DestroyWorld.md)' on it.


## Return Value

Returns a pointer to a [World](Data_World.md) struct.

## Version

Available since V0.08.

---------------------------------------------------------------
