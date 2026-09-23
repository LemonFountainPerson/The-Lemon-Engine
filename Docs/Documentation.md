# The LemonEngine
**Liam Fuentes Pessoa**
_________________________________________________


Current Version: 0.11

This engine uses the C standard library and SDL3, and is compiled using GCC (Although any good C compiler should work).
Everything is subject to change.

## Getting Started

**Core design methodology**

This is not 100% uniform across the entire codebase, but in general:

-> Functions starting with a lowercase letter are intended to operate a core function for the engine and should not be changed unless the core functionality of 
the engine needs to be changed. Conversely, functions that start with a capital letter are intended to be modified or added to in order to facilitate your game.

-> The FunctionResult enum defines a few simple exit conditions for functions to take - these can be used to clarify why a function is returning, primarily for 
debugging purposes. For compatibility, 0 is still success (LEMON_SUCCESS) and -1 is a general error (LEMON_ERROR). Most functions will use these enums as their 
return value.

-> The playerController can have its functionality completely swapped out or removed. If you are planning to have multiple physics formats in your game (for 
example, switching between top-down and platforming), you should include multiple controllers than can be switched to dependent on the state of the game.


## Sections:

[Objects](#objects)

[Animations](#animations)

[Functions Quick Reference](#functions-by-category)


# Objects	

The core asset in the Lemon Engine are objects; these represent almost everything in the gameworld such as the player, visible elements, geometry, etc. 
Objects consist of three main parts: 

The main Object struct is the most important element of an object, and it represents all logical attributes of an object. Objects are organised by IDs, 
with each ID uniquely defining behaviour. (E.g representing what the object is, enemy, collectable, interactable sign, etc.)

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

PhysicsBoxes represent the shape and collision state of an object. It controls how it will collide with other objects (if it should) as well as its
current position, direction and velocities. The current position (xPos, yPos) is stored alongside the previous position (prevXPos, prevYPos) from 1 GameTick ago.
'XVelocity' and 'YVelocity' are self-explanitory, but 'forwardVelocity' defines a separate velocity that is used to move the object along its pointed direction. Using
this is optional.
'PhysicsXVelocity' and 'PhysicsYVelocity' are used by the built-in physics to control momentum given by a moving platform, and do not need to be modified directly.
Likewise, 'inAir' and 'GroundBox' are also paramters used to controlphysics; inair is set to 0 when on the ground, and is incremented once per tick while the object 
is in the air, up to 100. 
The 'shape' variable defines what shape the hitbox is, while the 'solid' variable describes its behaviour.

DisplayDatas are used to control the sprites rendered and animations playing on the object, if it has any. For more info on Animations, check out the Animations section.

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

The currently playing animation is referenced by the DisplayData in the 'animationBuffer' pointer, and by the 'currentAnimation' variable which refers to the animation's ID. 
'CurrentAnimation' is set to the ID value of the currently playing animation and is set to 0 when no animation is playing. The 'currentFrame' and 'frameBuffer' variables
store the frame number and a reference to that frame respectively. 
To play an animation, the PlayAnimation function is called, with the number of repititions being the second arguement. (0 for repeating infinitely.)

```
int PlayAnimation(const char desiredName[], int loopCount, DisplayData *inputData);
```

The 'animationBuffer' and 'frameBuffer' pointers should not and don't ever have to be modified other than by the engine itself. If you want to manually control which animation
or which frame is playing, you can simply set the 'currentAnimation' and 'currentFrame' variables, and it will assign the correct data automatically.



## Functions by Category

[Game Set-Up](Game-Set-Up.md)