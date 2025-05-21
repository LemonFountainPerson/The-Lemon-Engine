# The LemonEngine
**Liam Fuentes Pessoa**
_________________________________________________


Current Version: 0.04
Currently only supported on windows. Sorry!

This is a project that serves more as a proof-of-concept or exercise as opposed to a real useful tool, especially because this project is being made from almost the ground up. That said, if you like what you see go ahead and download the source code to try it out! 

This engine uses the C standard library, stb_image.h for PNG loading, and SDL3 for window, audio and input proccessing. 
Everything else is done "in-house" with an emphasis in readability, reusability and performance. 

Everything is subject to change.

# Getting Started

The core of all data structures for the engine is contained within data.h. In it, all structs, enums and macros are defined, as well as including every other external/standard library the engine requires. Every file that comprises the engine includes this file, and it does so by checking if "IS_DEFINED" is defined to guard against redefinitions. The main of the engine is LemonMain.c, and should be the starting point of any actions performed in the engine. (NOTE: In the current version, a separate windows_main.c file is used as the main of the engine to replace the function "RunLemonEngine" conatined within LemonMain. This is done for stability reasons and in the future "RunLemonEngine" will used as the starting point.)

The main of the engine is relatively simple, comprising 3 sections: engine initialisation of relevant structs, audio devices and the window, the main game loop, and engine quit functions with data cleanup.
The overall engine works mostly of a single struct called the GameWorld. The GameWorld contains pointers to all relevent allocated data for the game scene to run, such as the player if it exists, backgrounds, the object list, etc. 

**Struct definition:**
```
struct world
{
	struct objectController *objectList;
	int drawnObjects;

	struct playerData *Player;

	struct spriteSet *BackGrounds;
	struct sprite *bgSpriteBuffer;

	double bgParallax;
	int bgTileVertically;
	int bgParallaxChunkSize;
	double bgChunkParallax;

	int cameraX;
	int cameraY;
	int level;
	double Gravity;

	int drawHitboxes;
	int drawSprites;
	int drawBackGround;
	int drawPlayer;
	int playBgMusic;
};
```


The GameWorld may be passed in to various functions to perform game actions, although most of the time functions will only take the neccessary data to perform its task. The player is represented as another allocated struct, and is accessible via the main function or as a pointer from the GameWorld. While the engine has a player controller already created, depending on the needs of a game, the player can be swapped out with minimal changes to surrounding code. Only its struct in data.h and the playerController.c files themselves need to be updated to function.

**Struct Definition:**
```
struct playerData
{
	double xPos;
	double xPosRight;
	double yPos;
	double yPosTop;
	double yVelocity;
	double xVelocity;
	double maxYVel;
	double maxXVel;

	int inAir;
	int jumpHeld;
	int jumpProgress;
	int crouch;

	double PhysicsXVelocity;
	double PhysicsYVelocity;
	double direction;

	int xFlip;
	int currentSprite;
	struct sprite *spriteBuffer;

	int spriteCount;
	struct spriteSet *spriteSetPtr;

	enum Layer playerLayer;

	int coinCount;
};
```

The screen of the engine is comprised of a struct called a RenderFrame that contains a pointer to the Uint32_t pixel data of the frame, and the screen's intended width and height. This frame is used by the drawing routines as a buffer to render all items for the game's scene. This frame is then taken by the window renderer (windows API for windows_main) and displayed to the window.


# Main game loop

For ease of use, it is unrecommended to alter the main game loop code already present. However if you wish to do so, keep in mind the following information.

**Initialisation**

After window and audio initialisation, game data is created using the initialiseWorld and initialisePlayer functions. These functions take in a PlayerData struct and a World struct respectively. They are formatted so that pointer association between the GameWorld and the Player is handled automatically, and these functions can be called in any order, and will accept NULL if the respective struct has not been created/initialised yet. The functions each return a pointer to allocated data for their respective structs. 

Most functions in the engine at the higher levels will perform checks to insure the pointers being passed to them are valid, however at lower levels of engine operation this may not be the case, so it is unadvised to call smaller functions such as helper functions directly without first ensuring Player and GameWorld initialisation.

**Game Loop**

The main structure of the game loop is as follows: 
First, updatePlayer is ran to take user input and operate the player. 

Next, updateObjects is run to update every object in the GameWorld. 
After these two functions all game logic has been performed. Any further functionality should go here after these functions (such as menu control, other self-implemented object types, etc.)

Next, worldCameraControl is called to move the GameWorld's camera according to the state of the GameWorld. (i.e: following the player) The seperation of the camera and the player means the camera is completely independent of the player and can be moved independently if you wish. For convinience, it's recommended to do this from the worldCameraControl function.

**Rendering**

Next, rendering begins. First, cleanRenderer is called to reset the frame for new things to be drawn, as well as NULL handling in the event of mis-allocated memory. 

Objects in the GameWorld are drawn in the order they are placed within the linked list that comprises all objects. (except the player) In order to facilitate better control, there is a layering system in which an object can be defined to only be drawn on a specific layer, and the main game loop does one rendering pass for each of these layers. This means objects in the FOREGROUND will always be drawn over objects in the MIDDLEGROUND and BACKGROUND, and so on. 
By default, the player is drawn on the MIDDLEGROUND layer, however due to the player not being an object contained within the GameWorld's object list, it is always drawn after the objects of that layer have been drawn. (For example, on the MIDDLEGROUND the player will be drawn behind the objects on the FOREGROUND but always in front of the objects on the MIDDLEGROUND and BACKGROUND)
More layers can be defined in the Layers enum contained within data.h, but additional rendering passes must be implemented within the main game loop to account for this.

After the RenderFrame's screen buffer has finished being rendered for the frame, PutScreenOnWindow is run to well...you know. This is a function that is simply comprised of some SDL functions responsible for creating a surface, a texture forom that surface and finally rendering the texture to the renderer to be placed on the window. (This function is replaced by two windows API functions in windows_main.)

**Audio**

Next, the iterateAudio function is called to handle audio events. SDL handles audio multi-threadedly so this function is mostly used to repeat sounds playing on looping channels, but can also be used to apply other sound modifications to the engine's audio. The LemonEngine uses SDL's BindAudioToStream function to effectively create a channel of audio that plays asyncroniously and can play multiple sounds at once. 

However, the engine uses multiple channels for a few reasons: multiple sounds playing on a channel means that only the most recently played sound can be accessed via SDL such as for stopping audio or repeating it. There is no (easy) way to distinguish between sounds you would like to repeat and those you do not. It also helps with organisation to seperate different channels for different purposes, especially if problems arise. For these reasons and more, there are multiple channels created as defined by the CHANNEL_COUNT macro in data.h.

The LOOP_CHANNELS macro also defines how many of those channels are channels where only one sound is played at a time and looped. It defines the first (LOOP_CHANNELS) channels as being these such channels. For example, by default there are 12 total channels created, and 4 Loop-Channels. This means channels 0-3 comprise channels where sounds will repeat and only one at a time will play, while channels 4-11 are regular, not repeating channels.
For ease of operation, macros or enums can be defined to assist with organisation, such as defining "MUSIC_CHANNEL" as 0 to signify that channel 0 is reserved for music.

**Frame Throttling**

Next, the frame throttling functionality. The function frameRate is called with an integer to determine how much to delay to maintain the specified frames per second. Ideally, without this function the engine will run unencumbered at approximately 400-600 fps. (NOTE: while partially implemented, deltaTime calculations and the frameRate function are not 100% accurate and are currently being worked on for a future version. As such, this information and its functionality may be subject to change.)

**Extra Stuff**

After that, the frame has finished rendering and the remaining space in the main game loop can be used for other features, such as debug functions, some of which are already present, or anything else. If any new feature requires interaction or rendering, its recommended to format them as simple function calls and to place them prior to the frame throttling code.

The variable "gameRunning" is a global variable which is used by the game loop to decide when to terminate the engine program. When gameRunning equals 0, the game loop ends and clean-up begins.

The "keyboard" int array is used to keep track of user input, where each entry corresponds to a key being pressed fo 1 and not for 0. In order for simplicity (especially for when switching between windows_main.c and RunLemonEngine) an enum called LemonKeys is used to map keys in this array.


# Game Objects

As previously mentioned, all objects in a game scene other than the player are comprised of an allocated Object struct that is part of a linked list set pointed to via 
GameWorld->ObjectList->firstObject .... etc. While the player is the user's way of interacting with the game world, objects are the building blocks of that world and as such are the main way of adding content to the game engine. Loading a level from an external .Lem file (currently actually .txt) is essentially just a long list of definitions for objects.

**Creating New Objects**

In order to create a new object type, a few additions must be made. First, for readability, you should add a new slot in the ObjectTypes enum to make it clear what it is while simultaniously assigning it an integer ID. ID 0 is reserved for level flags and should not be changed, however all the others may be reorganised as you wish.
```
enum ObjectType {
	LEVEL_FLAG = 0,
	SOLID_BLOCK = 1,
	RIGHT_SLOPE = 2,
	LEFT_SLOPE = 3,
	JUMP_THRU = 4,
	COIN = 5,
	MOV_HOR = 6,
	MOV_VER = 7,
	SPRING = 8,
	VERTICAL_GATE = 9,
	GATE_SWITCH = 10,
	GATE_SWITCH_TIMED = 11,
        NEW_ITEM = 12
        // ...
};
```

Next, if you wish to assign custom attributes such as size, shape, collision type, multi-use args, etc. you should create a new case in the switch statement of your choice. If you are making a moving platform, you should put it in the addMovingPlatform function, if it is a level flag, you should put it in the addLevelFlag function, etc. If it is more generic than any of the defined presets, then simply add a slot to the addObject function.
```
void addObject(ObjectController *objectList, int xPos, int yPos, int objectID, int arg1, int arg2)
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
		newObject->xSize = arg1 * X_TILESCALE;
		newObject->ySize = arg2 * Y_TILESCALE;
		break;

	// ...


        case NEW_ITEM:
            // New item's custom attributes go here!
            break;


	default:
		break;
	}

	return;
}
```

Finally, the sprite set should be defined if it is not intended to be an invisible object. To do this, go to the createSpriteSet function in the same file, and enter a new case in its switch statement using the loadObjectSprite function to load each sprite you wish to uses for that object. The third argument of loadObjectSprite defines what RenderMode the sprite will use, 0 for tiled, 1 for scaled, 2 for X tiling and Y scaling, etc. (More on this in the Sprite section)
```
void createObjectSpriteSet(ObjectController *objectList, int objectID)
{
	// ...


	// Fill sprite set with sprites
	switch (objectID)
	{
        // ...

        case NEW_ITEM:
            loadObjectSprite("Example_Tiled", newSet, 0);
            loadObjectSprite("Example_Scaled", newSet, 1);
            // etc.
            break;


        default:
        {
            loadObjectSprite("Missing", newSet, 0);
        } break;
	}

	return;
}
```

With this, the basic set-up of the object is complete! The final optional step is to add custom behaviour in the updateObjects function in the same file. This is done by making your own function that takes in at least the pointer to the object itself as an arguement, and can be used to create interactivity, animations, custom actions, etc. for that specific object.
```
void updateObjects(World *gameWorld, int keyboard[256], double deltaTime)
{
	// ...

	while(currentObject != NULL && i > 0)
	{
		//...
        
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


			case COIN:
			{
				if (overlapsPlayer(player, ObjXPos, ObjXPos2, ObjYPos, ObjYPos2) == 1)
				{
					deleteObject(objectList, &currentObject);
					deleteFlag++;
					player->coinCount++;
					LemonPlaySound("Coin_Collect", "Objects", 4, 0.8);
				}

			} break;

			//...


                    case NEW_ITEM:
                        updateNewItem(currentObject, ...);
                        // Custom behaviour execution goes here!
                        break;


			default:
				break;
		}


		// ...
	}

	return;
}
```

In order to load your object from a (.lem) file, 
you would simply type "OBJECT-\_\_[Object ID]\_\_[X position]\_\_[Y position]\_\_[arg1]\_\_[arg2]\_\_////..." 

For a moving platform, you would type "OBJMOV-\_\_[Object ID]\_\_...[bound1]\_\_[bound2]\_\_[speed]\_\_[timer]\_\_////..."

For a level flag, "LVFLAG-\_\_[Flag name]\_\_[args]..." The number of arguments changes depending on the flag name given.


**Game Object's attributes**


Objects have fairly self-explanitory attributes, except for args 1-5. These are multi-purpose and are used for different things depending on the object type. For example, for moving platforms arg1 and arg2 are the bounds the platform will move between, arg3 is its maxspeed while arg4 and arg5 are used for the timer between laps.

```
struct object
{
	struct object *nextObject;
	struct object *prevObject;
	int objectID;

	struct sprite *spriteBuffer;
	int currentSprite;
	int xFlip;
	int yFlip;

	enum RenderMode objectRenderMode;

	enum Layer layer;

	double xPos;
	double yPos;
	double xPosRight;
	double yPosTop;
	int xSize;
	int ySize;
	double xVel;
	double yVel;
	int solid;

	int currentAnimation;
	int animationTick;

	// Multi-purpose args
	int arg1;
	int arg2;
	int arg3;
	int arg4;
	int arg5;
};
```

Objects use xVelocity/yVelocity to move every frame via updateObjects, even if no custom behaviour is implemented. Using the velocity is preferable instead of changing the position directly as the moveObject functions also handle interaction/collision with the player if it solid. By default, objects will have no velocity, and so static props can dynamically be moved. Several helper functions part of gameObjects.c can also be used to supplement functionality.

Another such set of helper functions are changeXSizeBy and changeYSizeBy. These can be used to keep the object centered as you change its size, and also handles collision with the player. 

Objects also have the overlapsObjectType, overlapsObjectSolid and OverlapsObjectAllSolids functions. These can be used to detect overlap/collision with specific groups of objects depending on which one you use. OverlapsObjectAllSolids is particularly helpful with collision detection for objects as it essentially acts the same way that the player detection does.

The method for animating an object uses currentAnimation to set the animaton currently playing, while animationTick can be used to determine the current frame in the animation its on. Having two spearate variables allows for some control such as switching to a near identical animation as the currently playing one and not changing the animationTick in order to preserve fluidity of motion if the two animation's timings are similiar. 

While these two variables can (optionally) be used to control the state of the animation of the object, in order to actually switch the currently drawn sprite the function switchObjectSprite or SwitchObjectSpriteName is used. It simply takes in an input object pointer and an integer ID or string respectively to switch the sprite to. All sprites have both a sprite ID (which simply starts from 1 and counts upwards, effectively acting as an index) and a char string as a sprite name.



**Technical Details**

Objects use a double (floating-point) system to keep track of its co-ords in the gameworld - xPos and yPos are typically whats used for work and calculations, where xPosRight and yPosTop are provided for convinience. This is because xPosRight and yPosTop only get updated in the moveObjectX/moveObjectY functions each frame, and by other helper functions that modify the object's position directly such as changeSizeBy. 

CurrentAnimation is used to denote what animation is currently playing while animationTick defines the frame of said animation to display. Neither of these should be negative during normal operation however you may still set them as negative for your own reasons. 

Currently 5 multi-purpose integer args are provided in each object, however this may increase in the future. These can be used for whatever you like, such as health, ammo and armour, etc. for enemies, X and Y locations for pathing, IDs for connecting to other objects and more.

The updatePlayer function is run before the updateObjects function, which will result in some difficulty when trying to perform interactions from the player to the objects - this was chosen deliberately to make custom behaviour less reliant on the player controller which is designed to be able to be swapped out with minimal changes. This does mean that some interactions do have to be specially handled by objects such as movement matching with moving platforms, enemy attacks hitting the player, etc. This is relatively easy to do as the update objects function provides the pointer to the player via the GameWorld struct.

Objects can be deleted from update objects function, but do not attempt to delete an object while 


# Audio

Audio uses SDL as a layer to load, play and modify sounds in real-time. The channel system is explained in the game loop section, but the method to play sounds is relatively simple. Simply by calling the LemonPlaySound function with four arguments is enough for playing sounds normally, as the engine will handle the rest. 

The first two arguments are the name of the sound file (with or without .wav) and the name of the folder to find it in from the sounds folder in LemonData respectively. (e.g: LemonPlaySound("StartUp", "Music", ...) or LemonPlaySound("Jump.wav", "Player", ...) )

The third argument is what channel to play the sound on. Enums can be used to make this more readable. The fourth argument is the volume level as a double, going from 0.0 to 1.0. As mentioned before, sounds played on Loop channels will repeat indefinitely and sounds played elswhere will only play once.

```
int LemonPlaySound(char fileName[], char folderName[], int channel, double volume)
{
    // ...
    return 0;
}
```

Functions such as PauseChannel, ResumeChannel, StopAudioInChannel, etc. are for convenience as they are really just wrappers for already existing SDL functions. This allows for the potential to expand on its functionality, but more importantly it makes the code easier to read and work with.

# Sprites

**Overall Structure: spriteSets, spriteBuffers, etc.**

All sprites in the engine are handled almost identically. Sprites are connected via a linked list, and sets of sprites are grouped together by a Sprite Set struct which contains Meta data on the set of sprites, such as amount of sprites, SetID, and pointers to the first and last sprite in the chain. Sprite sets themselves also connect to each other via a linked list, although since there is no struct that can group together sprite sets all in one place the same way Sprite sets group sprites, It is usually handled by simply having a pointer to the first sprite set in a chain and each set is connected to the next but not vice-versa. (at least currently)

With this system, the player can have multiple sets of sprites, backgrounds can work similiarly with groups of backgrounds, but more importantly objects can self-manage sprite creation. Because there is no practical limit to the amount of objects on screen, if every object had copies of the sprite data they needed, it would bloat the memory usage massively. To solve this in an easy-to-use way, the GameWorld's Object List struct is connected to two linked lists: one for objects and one for spritesets. When an object is created, it searches through the list of spritesets looking for a set with a setID that equals its objectID. If such a spriteset exists already, then it simply uses the sprites from there. If it cannot find a matching spriteset, then it creates the appropriate set and appends it to the linked list.

This system means there only ever needs to be 1 allocated sprite set per object type, as opposed to individual objects. Another feature of the sprite system is the spriteBuffer. Without a buffer, the object would need to query the Object List's spritesets every single frame it needed to be drawn. While this would be a very inexpensive operation due to the nature of pointers, it would clearly scale up with the number of objects present in the scene or on screen, and it is after all an unnecessary calculation. Instead, a sprite pointer inside the object struct simply points to the sprite in its spriteset which the object is currently using. The only time the Sprite Sets need to be queried is when changing sprites, which for most objects will not happen frequently or at all.

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
```

The last thing of note is the RenderMode of the sprite. Currently in the engine there are 3 supported render modes, although 2 more will be added soon. The rendermode, clearly enough, changes how the image is rendered to the screen relative to the object, player, etc. By default, objects will use whatever sprite it's currently using to decide the render mode, although you have the option of overriding this with the objectRenderMode integer within the object struct, by setting it to a value higher than -1. This has the effect of forcing every sprite rendered for that specific object to be rendered using that mode.

Every image can be rendered in any mode, they are not restrictive. Mode 0 is Tilemode, and is the most common. This mode will render the image at a 1:1 scale to be the exact size of the object's bounding box. If it is smaller than the object, it will tile across it. The tiling is based of the object's bottom-left pixel and so changing the size will not alter the position of the texture on the object relative to that corner. 

Mode 1 is Scalemode, and this mode draws the sprite once, but stretches/shrinks it to fit the object's bounding box perfectly. This mode is more hardware intensive than Tilemode, and although I have optimised it over time, the engine will struggle to maintain 60 fps with 3+ instances of a scaled object covering the screen. In it's current state, it's easier on the game to use another render mode wherever possible.

Mode 2-3 are for X tiling and Y scaling, or Y Tiling and X scaling respectively. These act identically to modes 0 and 1 on their respective axis. (Yet to be implemented)

Mode 4 is straight rendering. This mode will simply draw the image as it is in the png file, 1:1 with no tiling. This method will render the whole image regardless of the Object's size or dimensions. It is not centered, so it is drawn from the bottom-left corner of the object. (This will change in a future update to center it by default, or control its offset, or both)

Mode 4 internally uses the same functions as Mode 0, so performatively, they are the same.

