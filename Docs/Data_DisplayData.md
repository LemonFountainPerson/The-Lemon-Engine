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


## Version
Available since V0.05.


----