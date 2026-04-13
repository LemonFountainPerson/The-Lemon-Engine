# The LemonEngine
**Liam Fuentes Pessoa**
_________________________________________________


Current Version: 0.09

This is a project that serves more as a proof-of-concept or exercise as opposed to a real useful tool, especially because this project is being made from almost 
the ground up. That said, if you like what you see go ahead and download the source code to try it out! 

This engine uses the C standard library and SDL3. 
Everything is subject to change.

# Getting Started

**Core design methodology**

This is not yet 100% uniform across the entire codebase, but in general:

-> Functions starting with a lowercase letter are intended to operate a core function for the engine and should not be changed unless the core functionality of 
the engine needs to be changed. Conversely, functions that start with a capital letter are intended to be modified or added to in order to facilitate your game.

-> The FunctionResult enum defines a few simple exit conditions for functions to take - these can be used to clarify why a function is returning, primarily for 
debugging purposes. For compatibility, 0 is still success (LEMON_SUCCESS) and -1 is generally error (LEMON_ERROR).

-> The playerController is intended to be able to be swapped out for a custom one, and should only require a PhysicsRect structure to be added in order to keep 
it compatible with other engine functions. Objects however, are not. All functions that use objects will assume it contains the variables that the engine defaults
with. While data may be added to objects for custom function, no variable should be removed. Any added variable should be initialised in the createNewObject 
function.

-> Some functions will purposely abstract its process or take a more roundabout algorithm in order to facilitate eligability: the only exception to this are 
extremely perfomance-sensitive functions such as those used in rendering sprites.

-> Any enums which define data categories that are expected to be expanded/added for a game will contain an additional UNDEFINED_XXX value to denote where the 
defined data ends. This means when checking for out of bounds data (at least in the > 0 direction) you may simply ensure [data] < UNDEFINED_DATA.



# Objects

The core asset in the Lemon Engine are objects; these represent almost everything in the gameworld such as the player, visible elements, geometry, etc. 
Objects consist of three main parts: 
```
typedef struct Object
{
	char name[OBJECT_NAME_LENGTH];	// unique identifier for Object 		
	int ObjectID;
	...

	PhysicsBox *ObjectBox;
	DisplayData *ObjectDisplay;

	struct Object *ParentObject;
	ParentType ParentLink;

	...
} Object;
```

The main Object struct is the most important element of an object, and it represents all logical attributes of an object. Objects are organised with IDs, with each 
ID uniquely defining behaviour. 

```
typedef struct PhysicsBox
{
	float xPos;
	float yPos;
	float prevXPos;
	float prevYPos;

	int xSize;
	int ySize;

	float forwardVelocity;
	float yVelocity;
	float xVelocity;

	int inAir;
	float PhysicsXVelocity;
	float PhysicsYVelocity;
	struct PhysicsBox *GroundBox;

	SolidType solid;
	SolidFlag flag;
	CollideType collideMode;
	Layer collideLayer;

	double direction;
	short xFlip;
	short yFlip;
	bool crouch;
} PhysicsBox;
```


```
typedef struct displayData
{
	int currentSprite;
	struct sprite *spriteBuffer;

	int currentAnimation;
	float animationTick;
	int animationLoopCount;
	float animationSpeed;
	struct animationFrame *frameBuffer;
	struct animation *animationBuffer;

	struct spriteSet *spriteSetSource;

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



In addition to these 3 base components, additional components can be created and added via the ObjectComponent system. Each type of component has a SparseList to store
them, allowing for fast and space efficient structs of data that can 'attached' to specific instances of objects to expand their functionality. Examples include TileMaps and 
HealthComponents.

Objects can have a parent object through the ParentObject variable, with the nature of the connection defined in the ParentLink variable. The minimum consequence of a parent-
child relation is that when the parent is deleted, all children and sub-children are deleted as well on the same tick. Other connections such as matching the position of the 
parent or the animation of the parent can be added as wished by modifying the child's ParentLink variable by bitwise ORing different options. For example, for children who
follow the motion and transparency of the parent are configured by setting the parentLink to [MOTION_LINK | TRANSPARENCY_LINK] and so on for other options.



# Animations

The animation system is operated through the spriteSets and the DisplayData. They are stored with AnimationFrame structs as linked lists connected to 
an Animation struct representing each animation. These Animation structs are themselves stored as a linked list from the animations pointer located
in the spriteSet.

```
struct animationFrame 
{
	struct animationFrame *nextFrame;

	struct sprite *frameSprite;
	float SpriteXOffset;
	float SpriteYOffset;
	float rotation;
};


struct animation 
{
	struct animation *nextAnimation;

	int animationID;
	char name[MAX_LEN]; 
	float frameRate;

	struct animationFrame *animationData;
};
```

The currently playing animation is referenced by the DisplayData in the animationBuffer pointer, although it is not updated when no animation is 
playing. The currentAnimation integer is set to the value of the currently playing animation and is set to 0 when no animation is playing. The
currentAnimation variable being 0 is how to check if the displayData isn't playing an animation.
The frameBuffer pointer contains the reference to the currently frame of the Animation that the object is displaying. Due to each frame being
contained in a linked list that makes up the animation, in order to progress the animation the program simply sets the frameBuffer to [frameBuffer->nextFrame].

To play an animation, the PlayAnimation function is called, with the number of repititions being the second arguement. (0 for repeating infinitely.)

```
int PlayAnimation(const char desiredName[], int loopCount, DisplayData *inputData);
```

To create an animation, you may either encode them via the animation data files located within the Animations folder in the LemonData or by hard-coding them 
with code. These animation files are organised by which object ID they correspond to, and are the same files used to load sprites into an object's spriteset.
To see how to use these files, refer to the LemonData section.

To create animations via code, two functions are used; initialiseNewAnimation and addSpriteToAnimation. The initialiseNewAnimation function returns a
pointer to a newly allocated animation struct, and automatically assigns it to the provided spriteSet. The addSpriteToAnimation function is 
used to add a new sprite to the animation. In order to create an animation, the desired frames should be sequentially added via this function 
in the order of the animation. Any created animations should be done within the LoadAnimations function in animations.c.

Frames can be created with additional arguements to allow for dynamic position offsetting or rotation.

```
Animation* initialiseNewAnimation(const char animationName[], int frameRate, SpriteSet *inputSet)

			Animation *newAnim = initialiseNewAnimation("Bounce", 24, newSet);
			addSpriteToAnimation("Spring3", newAnim, newSet);
			addSpriteToAnimation("Spring4", newAnim, newSet);
			addSpriteToAnimation("Spring5", newAnim, newSet);
			addSpriteToAnimation("Spring4", newAnim, newSet);
			addSpriteToAnimation("Spring3", newAnim, newSet);
			addSpriteToAnimation("Spring2", newAnim, newSet);
			addSpriteToAnimation("Spring", newAnim, newSet);
```


# LemonData

