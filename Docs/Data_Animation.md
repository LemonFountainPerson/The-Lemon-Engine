# Animation

## Definition
```
typedef struct Animation 
{
	struct Animation *nextAnimation;

	AnimationFrame *animationData;
	int frameCount;

	SoundMeta *animationSounds;
	int soundCount;

	char name[ANIMATION_NAME_LENGTH]; 
	int animationID;
	float frameRate;
} Animation;
```

## Description
This struct is used to store the list of [Sprite](Data_Sprite.md) references and [sounds](Data_SoundInstance.md) to play that make up a specific animation.
Animations can contain any number of frames, at any framerate and any amount of sounds to be played on specific 
frames of the animation.

Animations can have a name but internally they are primarily referenced by their 'animationID' value.


## Version
Available since V0.06.


----