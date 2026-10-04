# DisplayData

## Definition
```
typedef struct displayData
{
	int currentSprite;
	Sprite *spriteBuffer;

	int currentAnimation;
	int currentFrame;
	float animationTick;
	int animationLoopCount;
	float animationSpeed;
	AnimationFrame *frameBuffer;
	Animation *animationBuffer;

	SpriteSet *spriteSetSource;

	Layer layer;
	RenderMode RenderModeOverride;
	float size;
	RotationMode rotateMode;
	float spriteXOffset;
	float spriteYOffset;
	unsigned int pixelXOffset;
	unsigned int pixelYOffset;

	float transparency;		// 0.0 is no transparency - 1.0 is full transparency (invisible) -- SDL uses 0-255 where 255 is no transparency and 0 is fully transparent
	bool hidden;
} DisplayData;
```

## Description
This struct represents the visual state of the Object it is attached to. The DisplayData holds a reference to a 
[SpriteSet](Data_SpriteSet.md) through which all of its [Sprites](Data_Sprite.md) and [Animations](Data_Animation.md) 
come from. All [Objects](Data_Object.md) have a DisplayData.

Of note:
- The 'animationSpeed' variable controls the speed of the animation playback, by default it is set to 1.0.
- The 'size' variable multiplies the size of the rendered image on both the x and y axis. The default value is 1.0.
- The 'RenderModeOverride' variable can be set to a specific [RenderMode](Enum_RenderMode.md) value to force whatever
[Sprite](Data_Sprite.md) is being rendered to use this mode instead. The default value is 'DEFAULT_TO_SPRITE' which allows
the sprite's default mode to be used.
- The 'animationLoopCount' variable indicates how many more times the currently playing animation should loop for. A value
less than 1 indicates that the animation should loop forever, or until interrupted.


## Version
Available since V0.05.


----