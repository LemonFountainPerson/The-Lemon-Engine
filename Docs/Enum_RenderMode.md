# RenderMode

## Definition
```
typedef enum RenderMode 
{
	DO_NOT_RENDER = -2,
	DEFAULT_TO_SPRITE = -1,
	SINGLE,
	TILE,
	TILE_FAST,
	SCALE,
	STATIC_BACKGROUND,
	SINGLE_BACKGROUND,
	TILEPLANE_BACKGROUND,
	TILE_BACKGROUND,
	UNDEFINED_RENDERMODE
} RenderMode;
```

## Description
This defines the method through which to render a [Sprite](Data_Sprite.md), usually to an [Object](Data_Object.md).
For example, SINGLE renders the entire image once centered on the Object.
SCALE renders the entire image once stretched or squeezed to fit the exact size of the Object.
TILE renders the image tiled to fit the size of the Object, and cannot be rotated.
DEFAULT_TO_SPRITE is not to be used by actual Sprites as it is used by Objects to allow the Sprites' default rendermode to be used.


## Version
Available since V0.04.


----
