# The LemonEngine
**Liam Fuentes Pessoa**
_________________________________________________


Current Version: 0.06
Currently only supported on windows. Sorry!

This is a project that serves more as a proof-of-concept or exercise as opposed to a real useful tool, especially because this project is being made from almost 
the ground up. That said, if you like what you see go ahead and download the source code to try it out! 

This engine uses the C standard library, stb_image.h for PNG loading, and SDL3 for window, audio and input proccessing. 
Everything else is done "in-house" with an emphasis in readability, reusability and performance. 

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

-> The keyboard array doesn't map to a standard keyboard layout and is only a size of 256 for backwards compatibility. Keys from A to Z can be found by indexing 
the char values (i.e. keyboard['A'] will yield the state for the 'A' key.) Other keys such as predefined JUMP and INTERACT are defined in the LemonKeys enum. In 
general, 0 means unpressed and 1 means pressed, but for single inputs you can alter the entry to 2 to signify that the key is still being held but was taken as 
an input for something.

-> The deltaTime, gameRunning and frameThrottle variables are all global variables usable across all files in the engine, but in general only deltaTime need be 
used in this way; frameThrottle and gameRunning are handle by the RunLemonEngine function.


# Engine Initialisation

The core of all data structures for the engine is contained within data.h. In it, all structs, enums and macros are defined, as well as including every other 
external/standard library the engine requires. Every file that comprises the engine includes this file, and it does so by checking if "IS_DEFINED" is defined to 
guard against redefinitions. The main of the engine is LemonMain.c, and should be the starting point of any actions performed in the engine. (NOTE: In the current
version, a separate windows_main.c file is used as the main of the engine to replace the function "RunLemonEngine" conatined within LemonMain. This is done for 
stability reasons and in the future "RunLemonEngine" will used as the starting point.)

The main of the engine is relatively simple, comprising 3 sections: engine initialisation of relevant structs, audio devices and the window, the main game loop, 
and engine quit functions with data cleanup.
The overall engine works mostly of a single struct called the GameWorld. The GameWorld contains pointers to all relevent allocated data for the game scene to 
run, such as the player if it exists, backgrounds, the object list, etc. 

**Struct definition:**
```
struct world
{
	struct Camera MainCamera;

	struct ObjectController *ObjectList;

	struct playerData *Player;

	struct TextInstance *TextQueue;

	struct spriteSet *BackGrounds;
	struct sprite *bgSpriteBuffer;

	float bgParallax;
	int bgParallaxChunkSize;
	double bgChunkParallax;

	int drawnObjects;
	int drawnParticles;
	int drawnHudElements;
	short drawHitboxes;
	short drawSprites;
	short drawBackGround;
	short drawPlayer;
	short drawUI;
	short drawParticles;
	short drawObjects;

	LemonGameState GameState;
	CutsceneID CurrentCutscene;
	int SceneTick;
	int PlayingText;
	int GamePaused;
	int level;

	WorldPhysics PhysicsType;
	float Gravity;
};
```


The GameWorld may be passed in to various functions to perform game actions, although most of the time functions will only take the neccessary data 
to perform its task. The player is represented as another allocated struct, and is accessible via the main function or as a pointer from the 
GameWorld. While the engine has a player controller already created, depending on the needs of a game, the player can be swapped out with minimal 
changes to surrounding code. Only its struct in data.h and the playerController.c files themselves need to be updated to function. All game objects 
in the engine that have a postion/velocity, etc. use a PhysicsRect structure to hold this data. By using this common data structure, functions can be
written to be compatible with any data structure as long as it conatins this PhysicsRect data.

The Player struct holds a series of pointers to an object's components (NULL if it does not exist), and this object is effectively used as the 
player itself. In addition, it also contains more general variables independent of whatever object is being used as the player avatar. This means
on a level transition, the 'avatar' is deleted and effectively reset, while the other values in the PlayerData struct is preserved. For save 
files, the data in the PlayerData struct are what is necessary to be saved, while data such as the avatar's position and state are more optional 
to be saved.

By default, there is a PLAYER_OBJECT object that will automatically assign itself to be the new player avatar when it is created, but this 
functionality can be extended to other objects for multiple types of player avatars, or removed for a completely new system.

**Struct Definition:**
```
struct playerData
{
	struct Object *PlayerPtr;

	struct physicsRect *PlayerBox;
	struct displayData *PlayerDisplay;
	struct physicsRect *InteractBox;

	// These variables can be freely modified according to your modified player controller
	double maxYVel;
	double maxXVel;

	int inAir;
	int jumpHeld;
	int jumpProgress;

	int coinCount;
	int HP;
};

struct physicsRect
{
	double xPos;
	double yPos;
	double xPosRight;
	double yPosTop;

	unsigned int xSize;
	unsigned int ySize;

	double forwardVelocity;
	double yVelocity;
	double xVelocity;

	double PhysicsXVelocity;
	double PhysicsYVelocity;

	SolidType solid;
	CollideType collideMode;

	double direction;
	short xFlip;
	short yFlip;
	short crouch;

	float friction;
};


struct displayData
{
	int currentSprite;
	struct sprite *spriteBuffer;

	struct spriteSet *spriteSetSource;

	RenderMode RenderModeOverride;
	int spriteXOffset;
	int spriteYOffset;
	int pixelXOffset;
	int pixelYOffset;

	short currentAnimation;
	int animationTick;
	short animationLoopCount;

	struct animationFrame *frameBuffer;
	struct animation *animationBuffer;
};
```

The screen of the engine is comprised of a struct called a RenderFrame that contains a pointer to the Uint32_t pixel data of the frame, and the 
screen's intended width and height. This frame is used by the drawing routines as a buffer to render all items for the game's scene. This frame 
is then taken by the window renderer (windows API for windows_main) and displayed to the window.


# Main game loop

For ease of use, it is unrecommended to alter the main game loop code already present. However if you wish to do so, keep in mind the following 
information.

**Initialisation**

After window and audio initialisation, game data is created using the initialiseWorld and initialisePlayer functions. These functions take in a 
PlayerData struct and a World struct respectively. They are formatted so that pointer association between the GameWorld and the Player is handled
automatically, and these functions can be called in any order, and will accept NULL if the respective struct has not been created/initialised yet. 
The functions each return a pointer to allocated data for their respective structs. 

Most functions in the engine at the higher levels will perform checks to ensure the pointers being passed to them are valid, however at lower levels 
of engine operation this may not be the case, so it is unadvised to call smaller functions such as helper functions directly without first ensuring 
Player and GameWorld initialisation.

**Game Loop**

The main structure of the game loop is as follows: 
First, the Game Tick function is called, which comprises of all functions responsible for updating the state of the game. This function runs as 
follows;

MasterControls is run to take input from the keyboard and run any debug commands that can be used for ease of development.

PlayCutscene is run to operate the currently playing cutscene, if the GameWorld's GameState is set to "CUTSCENE".

UpdateObjects is run to update every object in the GameWorld. 

UpdatePlayer is ran to take user input and operate the player. 
After these two functions all game logic has been performed. Any further functionality should go here after these functions (such as menu control, 
other self-implemented object types, etc.)

WorldCameraControl is called to move the GameWorld's camera according to the state of the GameWorld. (i.e: following the player) The camera is represented by a Camera struct that is part of the GameWorld. The separation of the camera and the player means the camera is completely independent of
the player and can be moved independently if you wish. For convinience, it's recommended to do this from the worldCameraControl function. This can be
controlled through the use of the CameraMode variable which holds a CameraState enum defining what the camera should be doing. (Following the player,
staying in place, free roam, etc.)

```
struct Camera 
{
	int CameraX;
	int CameraY;
	int minCameraX;
	int maxCameraX;
	int minCameraY;
	int maxCameraY;

	int CameraXBuffer;
	int CameraYBuffer;
	short CameraLatch;
	CameraState CameraMode;
};

enum CameraState {
	FOLLOW_PLAYER = 0,
	MENU_CAMERA = 1,
	FREE_ROAM = 2,
	FREE_ROAM_RESTRICTED = 3,
	UNDEFINED_CAMERA_STATE
};
```

UpdateText is run to update any playing dialogue/text boxes and is responsible for operating the textQueue itself. (More on this later.)

HandleGameWorldEvents is called to handle more abstract game events such as pausing, level loading, etc.


**Rendering**

Next, rendering begins. First, cleanRenderer is called to reset the frame for new things to be drawn, as well as NULL handling in the event of 
mis-allocated memory. 

Objects in the GameWorld are drawn in the order they are placed within the linked list that comprises all objects (except the player). In order to 
facilitate better control, there is a layering system in which an object can be defined to only be drawn on a specific layer, and the main game loop 
does one rendering pass for each of these layers. This means objects in the FOREGROUND will always be drawn over objects in the MIDDLEGROUND and 
BACKGROUND, and so on. 
By default, the player is drawn on the MIDDLEGROUND layer, however due to the player not being an object contained within the GameWorld's object list, 
it is always drawn after the objects of that layer have been drawn. (For example, on the MIDDLEGROUND the player will be drawn behind the objects on 
the FOREGROUND but always in front of the objects on the MIDDLEGROUND and BACKGROUND)

More layers can be defined in the Layers enum contained within data.h.

After the RenderFrame's screen buffer has finished being rendered for the frame, PutScreenOnWindow is run to well...you know. This is a function that 
is simply comprised of some SDL functions responsible for creating a surface, a texture forom that surface and finally rendering the texture to the 
renderer to be placed on the window. (This function is replaced by two windows API functions in windows_main.)

**Audio**

Next, the iterateAudio function is called to handle audio events. SDL handles audio multi-threadedly so this function is mostly used to repeat sounds 
playing on looping channels, but can also be used to apply other sound modifications to the engine's audio. The LemonEngine uses SDL's BindAudioToStream 
function to effectively create a channel of audio that plays asyncroniously and can play multiple sounds at once. 

However, the engine uses multiple channels oraganisation and for controlling whether to loop a sound. All channels are currently identical except for the 
LOOP_CHANNEL channel as defined in the ChannelNames enum. The only difference between these channels is that sounds played in most channels will only be 
played once, while those played in LOOP_CHANNEL will loop forever until manually stopped. 

Every sound is its own memory-allocated struct in a linked list attached to the SoundChannels array, essentially to represent each instance of a sound as 
its own "object". This means each sound can be modified individually, or on a channel-wise basis. However, a limitation of this implementation is that 
there is a defined limit to the amount of sounds that can be played on a channel at one time, in order to conserve memory.

For ease of operation, macros or enums can be defined to assist with organisation, such as defining "MUSIC_CHANNEL" as 0 to signify that channel 0 is 
reserved for music.

**Game Ticks**

All of the previous code which pertains to updating the state of the game is only run during a game tick. By default, this is set to run at 60 times per 
second. Game ticks are a way of maintaining a maximum constant speed without having to use deltaTime. By setting the TICK_DELTA to 60, it will attempt 
to run the update functions only after approx 16 ms have passed. This simplifies code execution relative to timing, as this system essentially emulates 
a FixedUpdate function like those found in Unity. A limitation of this approach is that it does not account for the frames per second dipping below 60, 
which means while the game cannot speed up faster than the expected value, it can slow down. This should not be a significant issue as normal running 
of the engine hovers around 400 fps, however in a particularly busy scene with lots of transparency/scaled sprites it may slow down considerably.

**Extra Stuff**

After that, the frame has finished rendering and the remaining space in the main game loop can be used for other features, such as debug functions, some of which 
are already present, or anything else. If any new feature requires interaction or rendering, its recommended to format them as simple function calls and to place 
them prior to the frame throttling code.

The variable "gameRunning" is a global variable which is used by the game loop to decide when to terminate the engine program. When gameRunning equals 0, the 
game loop ends and clean-up begins.

The "keyboard" int array is used to keep track of user input, where each entry corresponds to a key being pressed, 1 for true and 0 for false. In order for 
simplicity (especially for when switching between windows_main.c and RunLemonEngine) an enum called LemonKeys is used to map common game inputs in this array. 
For example, INTERACT is mapped to 'Z' and 'E', while JUMP is mapped to the spacebar.


# Game Objects

As previously mentioned, all objects in a game scene other than the player are comprised of an allocated Object struct that is part of a linked list set pointed 
to via GameWorld->ObjectList->firstObject .... etc. While the player is the user's way of interacting with the game world, objects are the building blocks of 
that world and as such are the main way of adding content to the game engine. Loading a level from an external .Lem file (currently actually .txt) is essentially 
just a long list of definitions for objects.

**Creating New Objects**

In order to create a new object type, a few additions must be made. First, for readability, you should add a new slot in the ObjectTypes enum to make it clear 
what it is while simultaniously assigning it an integer ID. ID 0 is reserved for level flags and should not be changed, however all the others may be reorganised 
as you wish.

```
enum ObjectType {
	LEVEL_FLAG_OBJ = 0,
	SOLID_BLOCK = 1,
	FLAT_SLOPE_FLOOR = 2,
	JUMP_THRU_BLOCK = 3,
	UI_ELEMENT = 4,
	PARTICLE = 5,
	COIN = 6,
	MOVING_PLATFORM_HOR = 7,
	MOVING_PLATFORM_VER = 8,
	SPRING = 9,
	GATE_SWITCH = 10,
	GATE_SWITCH_TIMED = 11,
	VERTICAL_GATE = 12,
	HORIZONTAL_GATE = 13,
	DOOR = 14,
	BASIC_ENEMY = 15,
	PLAYER_OBJECT = 16,
	PUSHABLE_BOX = 17,
	UNDEFINED_OBJECT
};
```

Next, if you wish to assign custom attributes such as size, shape, collision type, multi-use args, etc. you should create a new case in the switch statement of 
your choice. If you are making a moving platform, you should put it in the defineMovingPlatform function, if it is a level flag, you should put it in the 
addLevelFlag function, etc. If it is more generic than any of the defined presets, then simply add a slot to the addObject function. The object system is 
designed such that all objects can be spawned via the addObject function.

```
void addObject(ObjectController *objectList, int objectID, int xPos, int yPos, int xSize, int ySize, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	Object *newObject;
	newObject = createNewObject(objectList, xPos, yPos, objectID);

	// ...


	// Set Object parameters
	switch (objectID)
	{
	case LEVEL_FLAG:
		break;

	case SOLID_BLOCK:
		newObject->xSize = xSize * X_TILESCALE;
		newObject->ySize = ySize * Y_TILESCALE;
		break;


	case NEW_ITEM:
		// New item's custom attributes go here!
		break;


	default:
		break;
	}

	return;
}
```

Finally, the sprite set should be defined if it is not intended to be an invisible object. To do this, go to the createSpriteSet function in the same file, and 
enter a new case in its switch statement using the loadObjectSprite function to load each sprite you wish to uses for that object. The third argument of 
loadObjectSprite defines what RenderMode the sprite will use, 0/SINGLE, 3/TILE, 6/SCALE, etc. (More on this in the Sprite section)

```
void createObjectSpriteSet(ObjectController *objectList, int objectID)
{

	// Fill sprite set with sprites
	switch (objectID)
	{
        // ...

        case NEW_ITEM:
            loadObjectSprite("Example_Tiled", newSet, TILE);
            loadObjectSprite("Example_Scaled", newSet, SCALE);
            // etc.
            break;


        default:
        {
            loadObjectSprite("Missing", newSet, TILE);
        } break;
	}

	return;
}
```

With this, the basic set-up of the object is complete! The final optional step is to add custom behaviour in the objectBehhaviour function in the same file. 
This is done by making your own function that takes in at least the pointer to the object itself as an arguement, and can be used to create interactivity, 
animations, custom actions, etc. for that specific object.

```
void objectBehaviour(World *gameWorld, Object *inputObject)
{
	// ...

		
	switch (currentObject->objectID)
	{
		case MOVING_PLATFORM_HOR:
		{
			updateHorizontalPlatform(player, currentObject, deltaTime);
		} break;


		case MOVING_PLATFORM_VER:
		{
			updateVerticalPlatform(player, currentObject, deltaTime);
		} break;


		case NEW_ITEM:
			updateNewItem(currentObject, ...);
			// Custom behaviour execution goes here!
			break;


		default:
			break;
	}


	// ...

	return 0;
}
```

Levels are created by reading from a leveldata file, which has a specific format. All files have a header which define the intended version number 
for the engine to be read from, as well as what type of file it is. Arguments can be separated by either spaces or double underscores. Four slashes
in a leveldata file indicate a new Object entry. 
Files will stop being read when encountering "////ENDFILE". 
OBJECT- simply indicates a single object being created, while OBJREP- indicates the same object being created multiple times as a 2D-array with 
adjustable X and Y offsets from each other. This means objects created this way can be arranged into a 2D grid. Comments can be put inside of
leveldata files with "//// >" that will continue until the next entry marked by the "////". For example:

```
V0.06-LEVELDATA
////OBJECT-__17__
////OBJECT-__1__100__32__480__1__
////OBJREP-__10__5__64__64__6__2200__256__		(OBJREP-__XCount__YCount__XOffset__YOffset__ObjectID__StartXPos__StartYPos__...)
...
////> This is a comment!!!!!!
////ENDFILE
```

In order to load your object from a (.lem) file, 
you would simply type "OBJECT-\_\_[Object ID]\_\_[X position]\_\_[Y position]\_\_[xSize]\_\_[ySize]\_\_////..." 

For a level flag, "LVFLAG-\_\_[Flag name]\_\_[args]..." The number of arguments changes depending on the flag name given.


**Game Object's attributes**


Objects have fairly self-explanitory attributes, except for args 1-5. These are multi-purpose and are used for different things depending on the object 
type. For example, for moving platforms arg1 and arg2 are the bounds the platform will move between, arg3 is its maxspeed while arg4 and arg5 are used 
for the timer between laps.

```
struct object
{
	int ObjectID;
	ObjectState State;
	CurrentAction Action;

	struct Object *nextObject;
	struct Object *prevObject;
	struct Object *ParentObject;

	struct physicsRect *ObjectBox;
	struct displayData *ObjectDisplay;
	Layer layer;

	// Multi-purpose args
	int arg1;
	int arg2;
	int arg3;
	int arg4;
	int arg5;
};
```

Objects use xVelocity/yVelocity to move every frame via updateObjects, even if no custom behaviour is implemented. Using the velocity is preferable 
instead of changing the position directly as the moveObject functions also handle interaction/collision with the player if it solid. By default, 
objects will have no velocity, and so static props can dynamically be moved. Several helper functions part of gameObjects.c can also be used to 
supplement functionality. As mentioned before, objects store all values associated with positioning and velocity within its PhysicsRect structure.

Another such set of helper functions are changeXSizeBy and changeYSizeBy. These can be used to keep the object centered as you change its size, and also 
handles collision with the player. 

Objects also have the overlapsObjectType, overlapsObjectSolid and OverlapsObjectAllSolids functions. These can be used to detect overlap/collision with 
specific groups of objects depending on which one you use. OverlapsObjectAllSolids is particularly helpful with collision detection for objects as it 
essentially acts the same way that the player detection does.

The ParentObject pointer is normally set to NULL, but can be assigned to another object to classify a parent relationship between objects. This is 
useful for syncing state with another object without having to search for it. When a parent object is marked for deletion, all objects that have it as 
its parent will be marked for deletion as well, and this will continue recursively until the end of the parent 'tree' is reached.


**Technical Details**

Objects use a double (floating-point) system to keep track of its co-ords in the gameworld - xPos and yPos are typically whats used for work and calculations, 
where xPosRight and yPosTop are provided for convinience. This is because xPosRight and yPosTop only get updated in the moveObjectX/moveObjectY functions each 
frame, and by other helper functions that modify the object's position directly such as changeSizeBy. 

CurrentAnimation is used to denote what animation is currently playing while animationTick defines the frame of said animation to display. Neither of these 
should be negative during normal operation however you may still set them as negative for your own reasons. 

Currently 5 multi-purpose integer args are provided in each object, however this may increase in the future. These can be used for whatever you like, such as 
health, ammo and armour, etc. for enemies, X and Y locations for pathing, IDs for connecting to other objects and more.

The updatePlayer function is run after the updateObjects function. This means objects can move the player freely but for player to object interactions, they 
will most likely have to be handled rom the object's perspective in order to prevent a frame delay.

Objects can be directly deleted from update objects function, but this requires extra logic handling so that an object in the list does not get skipped for 
evaluation that frame. The delete object function works by deleting the object you provide the pointer to, and will change the given pointer to point to the 
next item in the object list. Because of these complications, a helper function called "MarkObjectForDeletion" is preferable as it can used at any point in the 
program's execution safely and will provide additional checks for you to ensure the object can be deleted without error. When an object is marked for deletion, 
its state is set to (TO_BE_DELETED), and at the end of its iteration in the update objects function it will be deleted. If you wish to undo the mark before its 
actual deletion you may use "UnmarkObjectForDeletion" to reverse this and preserve the object, although as a consequence the objects state will be reset to 
DEFAULT, and you may lose some data. (Such as if an object is in a unique state, this data will be lost when marked for deletion, which may cause logic errors.)


**Solid Types**

The solid types are relatively self-expanitory, however when adding a new type it requires making edits to 5-6 functions in order to facilitate their 
functionality.

The functions in question are CheckBoxCollidesBox function, which must be edited to include the logic for checking if an object overlaps with this new solidtype, 
i.e: defining the shape of the solidtype, any extra requirments for collision to happen such as one-way collisions, etc. Next, the Resolve[X/Y]Collision 
functions must be modified to accomodate the new solid type to define how a moving PhysicsRect can collide with a static instance of your solid type. The last 
two functions are the Resolve[X/Y]CollisionByPush in order to define what happens when your solid type is meant to push other objects in its way when moving.

In the engine, the method of collision is decided based on the PhysicsRect's collideMode - "PUSH" for using Resolve[X/Y]CollisionBYPush, and "IMPACT" for
Resolve[X/Y]Collision. In addition, solid types and other aspects can help define what method is used. The evaluateCollideMode function is used to decide
which collision mode to use.

**Descriptions of solid types:**

SOLID: Fully solid; the shape of the hitbox is always a rectangle conforming to the shape/size of the physicsRect itself.

UNSOLID: No collisions are possible with this solidtype.

FLAT_SLOPE: Fully solid; the shape is a triangle with a flat hypoteneuse with a slope calculated from the height of the PhysicsRect over the width of 
the PhysicsRect. 
(Y = X * ySize/xSize)

JUMP_THROUGH: Semi-solid; has no x axis collision, and is only solid when approaching from above. For example, the player can jump through these 
solids from below, only to land on top of it when falling back down.

ENTITY: Fully solid; acts identically to SOLID, however ignores all collision with other PhysicsRects with the same solidtype. For example, a player 
and an enemy could both use ENTITY, meaning they would both interact with the gameWorld as you would expect but the player and enemy can pass through 
each other as two entities cannot collide.

ENTITY_SOLID: Fully solid; these solids only collide with boxes that have the "ENTITY" solid type.

PUSHABLE_SOLID: Fully solid; these solids act just like the default SOLID type, however when an ENTITY or another PUSHABLE_SOLID collides with it, it 
is always pushed instead of depending on its collideMode variable. This interaction only happens in one direction, for example if an ENTITY is hit by
a PUSHABLE_SOLID it is not forced to be pushed, only when an ENTITY hits the PUSHABLE_SOLID does this happen. 



**Particles**

The Lemon engine has a built-in particle system; although basic it provides an easy-to-use template for 2D animated effects to be created. All particles use the 
PARTICLE object ID (5) and each different particle is defined simply by their animation number as defined in the currentAnimation variable. (E.g: animation 0 
corresponds to the coin sparkle particle effect.) Its position, velocity, amount of animation loops, framerate, animation direction and particle lifetime can all 
be defined when spawning a particle but have default values if left at 0. For example, if particle lifetime is left at 0, the particle will default to deleting 
itself when it has run through all its loops. If additional effects such as movement are required, you should simply add the behaviour logic to the 
updateParticles function as you would in the updateObjects function. Although, by default only two integer arg variables are available for free use.

```
	// Arg1: particle sub type (SPARKLE)
	// Arg2: # of times to loop (1)
	// Arg3: Framerate (0) 					(eg. 0 or 1 for each frame, 3 for every 3 frames, etc.)
	// Arg4: Particle maximum lifetime (0) 			(e.g: 0 will default to deleting as soon as loops finish)

	AddObject(gameWorld->objectList, PARTICLE, ObjXPos, ObjYPos, SPARKLE, 1, 0, 0, 0);

	MarkObjectForDeletion(currentObject);
	player->coinCount++;
	LemonPlaySound("Coin_Collect", "Objects", OBJECT_SFX, 0.8);
	// ...
```

To add a new particle, only three or four additions are required. (Other than the sprites added to the objects folder,
and any animations necessary to the spriteSet.)

First, its identifier should be added to the ParticleType enum.

```
enum ParticleSubType {
	EMPTY = 0,
	SPARKLE = 1,
	NEW_PARTICLE = 2,
	UNDEFINED_PARTICLE
};
```

Next, in the LoadParticleSprites function, you will add the sprites to be loaded for the particle here. The only restriction is that without edits the particle 
object only supports animating with sprites that are all next to each other in the sprite set. Ergo, you should load your frames one after another, in order.

```

int LoadParticleSprites(SpriteSet *newSet)
{
	// Sparkle 
	loadObjectSprite("Sparkle1", newSet, SINGLE);
	loadObjectSprite("Sparkle2", newSet, SINGLE);
	loadObjectSprite("Sparkle3", newSet, SINGLE);
	loadObjectSprite("Sparkle4", newSet, SINGLE);
	loadObjectSprite("Sparkle5", newSet, SINGLE);
	loadObjectSprite("Sparkle6", newSet, SINGLE);
	loadObjectSprite("Sparkle7", newSet, SINGLE);

	// New particle sprites
	//.....


	return 0;
}
```

Finally, the first and last sprites for your particle to use should be defined in the LoopParticleAnimation function as a case in its switch statement. In 
addition, the animateType variable can be modified to specify a type of animation the particle will use. (0/1 for forwards then loop, -1 for backwards then loop, 
etc.)

```
int LoopParticleAnimation(Object *particle)
{
	int firstSprite = 1;
	int lastSprite = 1;
	int animateType = 1;

	// Add a new case whenever you make a new particle here
	switch(particle->currentAnimation)
	{
		case SPARKLE:
		lastSprite = 7;
		break;

		default:
		break;
	}
	//....
}
```

An optional step is to define your own animateType in the subsequent switch statement. If done so, keep in mind that the particle's arg1 variable is how many 
loops are remaining to be performed, and so whenever a "loop" of your new animation type occurs, this variable should be decremented.

```
// Add a new case for specific animation sequences
switch (animateType)
{
	case 2:
	{
		// ...
	} break;


	case -2:
	{
		// ...
	} break;


	case -1:
	{
		// ...
	} break;

	case 3:
	// New animation type!
	break;

	default:
	{
		particle->currentSprite++;

		if (particle->currentSprite > lastSprite || particle->currentSprite < firstSprite)
		{
			particle->arg1--;

			particle->currentSprite = firstSprite;
		}

	} break;
}
```

A limitation of this implementation is that particles will only support one animation at a time, so multiple particles will need to be created for different 
animations, however they can share the same sprite data as they all share a spriteset.


# Audio

Audio uses SDL as a layer to load, play and modify sounds in real-time. The channel system is explained in the game loop section, but the method to play sounds 
is relatively simple. Simply by calling the LemonPlaySound function with four arguments is enough for playing sounds normally, as the engine will handle the rest. 

The first two arguments are the name of the sound file (with or without .wav) and the name of the folder to find it in from the sounds folder in LemonData 
respectively. (e.g: LemonPlaySound("StartUp", "Music", ...) or LemonPlaySound("Jump.wav", "Player", ...) )

The third argument is what channel to play the sound on. Enums can be used to make this more readable. The fourth argument is the volume level as a float, going 
from 0.0 to 1.0. As mentioned before, sounds played on Loop channels will repeat indefinitely and sounds played elswhere will only play once.

```
int LemonPlaySound(char fileName[], char folderName[], int channel, float volume)
{
    // ...
    return 0;
}
```

Functions such as PauseChannel, ResumeChannel, StopAudioInChannel, etc. are for convenience as they are really just wrappers for already existing SDL functions. 
This allows for the potential to expand on its functionality, but more importantly it makes the code easier to read and work with.


# Sprites

**Overall Structure: spriteSets, spriteBuffers, etc.**

All sprites in the engine are handled almost identically. Sprites are connected via a linked list, and sets of sprites are grouped together by a Sprite Set 
struct which contains Meta data on the set of sprites, such as amount of sprites, SetID, and pointers to the first and last sprite in the chain. Sprite sets 
themselves also connect to each other via a linked list, although since there is no struct that can group together sprite sets all in one place the same way 
Sprite sets group sprites, It is usually handled by simply having a pointer to the first sprite set in a chain and each set is connected to the next but not 
vice-versa. (at least currently)

Sprites can be identified in one of two ways: its spriteID and its spriteName. The spriteID will simply be the order in which each sprite was added, starting 
from 1. Eg sprite ID 3 is the third sprite in the sprite set. The spriteName is a char string that is set to the file name of the sprite without its extension. 
(Built-in functionality to alter the name of a sprite and altering the ID/ordering of a sprite will be added in a future version, likely v0.05)

With this system, the player can have multiple sets of sprites, backgrounds can work similiarly with groups of backgrounds, but more importantly objects can 
self-manage sprite creation. Because there is no practical limit to the amount of objects on screen, if every object had copies of the sprite data they needed, 
it would bloat the memory usage massively. To solve this in an easy-to-use way, the GameWorld's Object List struct is connected to two linked lists: one for 
objects and one for spritesets. When an object is created, it searches through the list of spritesets looking for a set with a setID that equals its objectID. 
If such a spriteset exists already, then it simply uses the sprites from there. If it cannot find a matching spriteset, then it creates the appropriate set and 
appends it to the linked list.

This system means there only ever needs to be 1 allocated sprite set per object type, as opposed to individual objects. Another feature of the sprite system is 
the spriteBuffer. Without a buffer, the object would need to query the Object List's spritesets every single frame it needed to be drawn. While this would be a 
very inexpensive operation due to the nature of pointers, it would clearly scale up with the number of objects present in the scene or on screen, and it is after 
all an unnecessary calculation. Instead, a sprite pointer inside the object struct simply points to the sprite in its spriteset which the object is currently 
using. The only time the Sprite Sets need to be queried is when changing sprites, which for most objects will not happen frequently or at all.

All required sprite data is contained within the Displaydata struct, and this struct is what is used by the rendering functions.

```
struct sprite
{
	struct sprite *nextSprite;
	struct sprite *prevSprite;
	unsigned char *spriteData;

	int height;
	int width;
	enum RenderMode RenderMode;

	int spriteID;
	char spriteName[MAX_LEN];
};

struct spriteSet
{
	struct sprite *firstSprite;
	struct sprite *lastSprite;

	struct spriteSet *nextSet;
	struct spriteSet *prevSet;

	struct animation *Animations;

	int setID;
	int spriteCount;
};
```

The last thing of note is the RenderMode of the sprite. The rendermode is determined by an enum that contains all rendermodes available. By default, objects will 
use whatever sprite it's currently using to decide the render mode, although you have the option of overriding this with the objectRenderMode integer within the 
object struct, by setting it to a value higher other than DEFAULT_TO_SPRITE (-1). This has the effect of forcing every sprite rendered for that specific object 
to be rendered using that mode. Similarly, DO_NOT_RENDER (-2) is used for telling the renderer to skip rendering that sprite/object altogether. 


```
enum RenderMode {
	DO_NOT_RENDER = -2,
	DEFAULT_TO_SPRITE = -1,
	SINGLE = 0,
	SINGLE_FULL_ALPHA = 1,
	SINGLE_FAST = 2,
	TILE = 3,
	TILE_FULL_ALPHA = 4,
	TILE_FAST = 5,
	SCALE = 6,
	SCALE_FULL_ALPHA = 7,
	TILE_SCALE = 8,
	TILE_SCALE_FULL_ALPHA = 9,
	SCALE_TILE = 10,
	SCALE_TILE_FULL_ALPHA = 11
};
```


Every image can be rendered in any mode, they are not restrictive. 

TILE: This mode will render the image at a 1:1 scale to be the exact size of the object's bounding box. If it is smaller than the object, it will tile across it. 
The tiling is based of the object's bottom-left pixel and so changing the size will not alter the position of the texture on the object relative to that corner. 
This mode handles transparent pixels.

SCALE: Draws the sprite once, but stretches/shrinks it to fit the object's bounding box perfectly. This mode is more hardware intensive than Tilemode, and 
although I have optimised it over time, the engine will struggle to maintain 60 fps with 3+ instances of a scaled object covering the screen. In it's current 
state, it's easier on the game to use another render mode wherever possible. This mode handles transparent pixels.

SINGLE: This mode will simply draw the image as it is in the png file, 1:1 with no tiling. This method will render the whole image regardless of the Object's 
size or dimensions. It is not centered, so it is drawn from the bottom-left corner of the object. (This will change in a future update to center it by default, 
or control its offset, or both). Mode 4 internally uses the same functions as Mode 0, so performatively, they are the same.

XXX_FAST: This is a faster version of the other rendermodes. Each type of rendering except for SCALE has a fastmode version. This renders sprites in lines and 
has tiling, however due to the nature of its rendering it cannot handle transparent pixels. Normally TileMode is fast enough on its own for mode 5 to be 
unnecessary, however for objects that are known to not require transparency this is preferred as it does improve performance overall.

XXX_FULL_ALPHA: This is a version of rendering that enables full proper transparency, at the cost of performance. Every type of rendering has a FULL_ALPHA 
version, however it's recommended to avoid using this mode where possible. (Until an update comes that improves performance!) 


# Animations

The animation system is operated through the spriteSets and the DisplayData. They are stored with AnimationFrame structs as linked lists connected to 
an Animation struct representing each animation. These Animation structs are themselves stored as a linked list from the animations pointer located
in the spriteSet.

```
struct animationFrame 
{
	struct animationFrame *nextFrame;

	struct sprite *frameSprite;
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

To create an animation, two functions are used; initialiseNewAnimation and addSpriteToAnimation. The initialiseNewAnimation function returns a
pointer to a newly allocated animation struct, and automatically assigns it to the provided spriteSet. The addSpriteToAnimation function is 
used to add a new sprite to the animation. In order to create an animation, the desired frames should be sequentially added via this function 
in the order of the animation. Any created animations should be done within the LoadAnimations function in animations.c.

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
