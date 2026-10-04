# Sprite

## Definition
```
typedef struct sprite
{
	struct sprite *nextSprite;
	struct sprite *prevSprite;

	SDL_Texture *texture;
	unsigned int height;
	unsigned int width;
	RenderMode RenderMode;

	int spriteID;
	char name[MAX_LEN];
} Sprite;
```

## Description
Used to store the texture data for a loaded image. This 'Sprite' can then be rendered by any [Object](Data_Object.md) that uses the [SpriteSet](Data_SpriteSet.md)
that this 'Sprite' belongs to. Any Animation within the same SpriteSet can also use this 'Sprite' as many times as it wants without needing more 
than one copy of the 'Sprite' and its texture data.

Sprites can have unique names but are primarily referenced internally by their 'spriteID'. The '[RenderMode](Enum_RenderMode.md)' value defines the 
default way the Sprite is rendered, although this can be overriden on a per-Object basis.

## Version
Available since V0.04.


----