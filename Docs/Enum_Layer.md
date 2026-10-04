# Layer

## Definition
```
typedef enum Layer 
{
	BACKGROUND = 0,
	MIDDLEGROUND,
	MIDDLEGROUND_2,
	FOREGROUND,
	PARTICLES,
	HUD,
	FRONT_LAYER,
	LAYER_COUNT,
	UNDEFINED_LAYER
} Layer;
```

## Description
This enum defines a layer for something in the engine to render on, or collide with. The smaller values are 
behind larger values. 

The 'HUD' layer is special and represents a layer that is centered on the [Camera's](Data_Camera.md) position. 
(E.g: position 0, 0 will always be the center of the screen regardless of the actual position of the camera.) 
This is most useful when creating Heads-Up Display elements (hence the name).


## Version
Available since V0.04.


----