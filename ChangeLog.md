# v0.09
5th March 2026

## New Features:

-> Added conditional commands to the level loading routine.

-> Added a command to set an object's name in the level data. (Syntax: SETNAME {newNameHere})

-> Arguments in files that are enclosed in speech marks (e.g: "data here") will always be interpreted as one argument, even if there are spaces or new lines
between them.

-> Added a command to hide an object when it is loaded. (Syntax: [Object info], HIDE)

-> Added a command to snap any object's position to the 'Grid'. (Defined with X_TILESCALE and Y_TILESCALE)

-> Added the ability to load cutscenes from files, located in the CutsceneData directory.

-> Added the component add-on system, where objects can take on additional functionality when it has a component added on.

-> Added the TileMap component for constructing more elaborate structures with less objects.

-> Split the solid type enum into more clearly defined solidType and SolidFlag, where the type denotes the shape/behaviour and the flag
can control extra features such as disabling interactions with a specific solid type.

-> Added the ability to save game data such as display settings and game flags to be loaded at any time via the loadSave/loadSettings functions.

-> Sounds can now be positioned relative to the camera with PositionSound() or simply panned left-right with SetLRPan()/ChangeLRPan().

-> Mp3 and Ogg sounds can now be played, although the default is still Wav, meaning the extensions must be written along with the sound name in these
cases.

-> Added the TILESET background mode that uses a sprite as a tileset to display a background of tiles. These tiles can be programmed with 'TILESET_DATA'
files using an array of numbers alongside some basic commands.

-> Added 'Presets' to level data, which allow different level files to run other, embeded level files for repetition or organisation. 
(Syntax: Preset: "NameOfPresetFile")


## Structure Changes:

-> The layer variable is now contained within the displayData struct instead of the object struct to clarify its use and improve consistency.

-> The engine always preallocates objects, the ability to create and delete object data on the fly is now no longer present, for performance reasons 
and to support the other systems. 

-> All object and component data is now stored as 'arenas'.

-> Remade the game event system to now be based on a static array for performance.

-> The ACTOR state now always disables objects behaviour and movement, to allow them to be used as a puppet. All objects except for the player are also disabled
during the CUTSCENE game state.

-> The direction variable from the displaydata was removed, and is now soley based on the PhysicsBox direction variable. 

-> The direction variable is now based on Degrees, rather than radians for its ease of understanding and better compatibility with SDL. Any calculations that
expect radians must now convert between them first.

-> Objects/PhysicsBoxes/DisplayDatas are now always pre-allocated; it is guaranteed that they are created as one large array.

-> Added the File Reader version marker. Files being read by the engine will now be accepted if the current version number is in the header, (as before) or if
the current filereader version is in the header. Current version: 'FRV<1>'

-> Switched audio system to use SDL_Mixer, enabling finer control over audio positioning/panning, organisation and audio effects.

-> Removed the xPosRight and YPosTop variables from the PhysicsBox, due to lack of usefulness. 


## Bug Fixes/Improvements:

-> Slight performance improvements by utilising the objects as an array instead of a linked list.

-> Re-implemented the maximum objects rendered limiter for performance.

-> Deleted objects no longer need to recursively search for child objects to delete them; deleting objects is now much more performant when there are many objects.

-> Attempting to play a new sound when a channel is already full will now overwrite an existing sound instead of preventing the sound from playing.

-> Re-organised includes to be easier to use.

-> 'putConsoleString' is now formatted like printf, meaning the other variations of the function are no longer necessary.

-> Text boxes will now automatically insert new line breaks into your text to make whole words fit the box correctly.



# v0.08


## New Features:

-> Added full linux support! Lemon now runs natively on windows and linux systems.

-> Added a new cutscene manager that allows for events and actions to be "scheduled" for playback using easy to understand functions #
much like the textbox system.
All text boxes also work with the cutscene manager, and they can be scheduled for playback much like any other action, and can be paired with option prompts to create 
branching scene structures. See "Test_Scene" in cutsceneManger.c for an example of its use.

-> Added the ability to alter the camera's zoom via SET_SCREEN_ZOOM and CHANGE_SCREEN_ZOOM LemonGameEvents (does not affect the HUD layer).

-> Added the ability to change the main window's size via the LemonGameEvent system.

-> Added the ability to toggle fullscreen via the LemonGameEvent system.

-> Added "DebugText" which allows for text to be drawn to the screen without using objects, primarily for debug purposes. (DebugString was also renamed to
ConsoleString for clarity.) A pre-implemented use for the DebugText is to display object info on screen in real time. Refer to the 'MasterControls' function 
or section by the same name in the Documentation for a full set of bindings.

-> Added the ability to set the window title and icon.

-> Added the ability to modify a sound's playback speed. (via Lemon_PlaySoundSpeed)

-> Added the ability to hide/show objects via the DisplayData's "hidden" boolean variable.

-> Added the ability to create textbox "option prompts" with triggerable functions and configurable options. (See UpdateDoor in gameObjects.c for an example.)

-> Added the ability to modify the size of displayed sprites using the 'size' variable within the displaydata struct and a few convinience functions. Works with
SINGLE and SCALE render modes; for SINGLE, it will act as a scale based on the size of the original image, for SCALE it is simply an additional scale factor which
can size the sprite independent of the physicsRect attached; although it can only increase the size on both axis' equally, instead of on either axis independently.

-> Text boxes and option prompts can now be used with the mouse.

-> Added the ability to show specific object details in debug mode by hovering over them with your mouse.


## Structure Changes:

-> Rendering is now handled via the SDL texture renderer, meaning rendering is now hardware accelerated and there is improved scaling functionality.

-> Added the LemonGameEventData structure, a union which will contain information required for any game event being triggered, e.g: width/height 
dimensions to change the screen size to.

-> Changed the way input is received/acknowledged by the engine; input values above 0 indicate the button is being held down, and for one-time inputs
"AcknowledgeHeldButtons" can be used to set all inputs being held to a value other than 1, which can be used to differentiate between new inputs. All held buttons
automatically get set to 2 or above when the first game tick ends after they are pressed.

-> Objects that attempt to switch to a sprite that it cannot find will no longer still be visible with an incorrect sprite, and instead will render using
the default texture contained within the EngineSettings (EngineData) struct.

-> Redid how the background is rendered (Due to new SDL rendering method); Background struct now has a renderMode value that is used to determine rendering mode.
Added new rendermodes specifically for backgrounds. Any type that is not a Background render mode used on the background struct is treated as DEFAULT_TO_SPRITE 
or DO_NOT_RENDER. 

-> SINGLE and SCALE modes no longer tile at all, even when manipulated with the pixel offsets. Pixel offsets on TILE mode cannot allow the tiled image to itself tile.
(Any offset that exceeds the bounds of the sprite will no longer tile, but instead be clamped to the edges.) However, when rendering in TILE mode the scale of the 
tiling effect can be modified via "size" in the displayData.

-> Removed the BG_ROW_PARRALAX rendermode (mainly because of difficulty to implement it with the SDL renderer, but also because of its niche use case).

-> Changed all object position and velocity variables to use floats instead of doubles (for efficiency reasons) and the Camera's position values to floats instead of 
integers (for compatibility). Direction values will remain as doubles for precision, but with most variables set as floats this should improve interoperability and 
ease of use.

-> The Windows-only version is no longer supported; the engine is written exclusively with SDL, to be compatible with both Windows and Linux.



## Bug fixes/Improvements:

-> The ObjectController's (ObjectList) object count is now decremented when objects are deleted. (This variable is now always up-to-date.)

-> Any detected incorrect/corrupt animation files are now properly aborted from loading data.

-> Textbox presets have new names for their enums to make it clearer what each does.

-> All files read for data are now more consistent; leveldata files uses new lines to indicate new entries but is still backwards-compatible with the 
four slashes method, animation files can now use '>' for comments and ENDFILE to denote when the file should stop being read. 

-> The camera and objects in the game scene are no longer restricted to positive x, y values. (You can place objects at negative values.)

-> In the event of a fatal error, the process will now create a dialog box to explain the error before the program ends.

-> Fixed a bug with deleting objects where parent-child links that were in backwards order within the object list caused a crash.

-> Scaled rendermodes now no longer crash the game when there is a pixel[x/y] offset, and now renders correctly.

-> Animations with only one frame and set to playback indefinitely will automatically halt until a new animation is played for optimisation.

-> Renamed/retooled some functions to make them easier to understand or more consistent with other functions in the codebase.




# v0.07
21st September 2025

## New Features:

-> The Lemon Engine is now 64 bit! Along with some internal changes, this has increased perfomance slightly across the board.

-> Added full sprite rotation for non-tiled rendermodes, via the direction variable in the displayData struct.

-> Added the ability to define custom code execution on a per-frame basis instead of per-Tick - represented as a linked list of FrameUpdateFunction structs.
Object types can create multiple instances, and all instances associated with a specific object will be deleted when that object is deleted.

-> Added full Mouse Input: x, y co-ords, Left mouse button, Right mouse button, Middle mouse button, etc.

-> Added convenience functions for adding different types of strings to the debug string.

-> Animation frames can now store a rotation offset in degrees that is applied dynamically.

-> Added a built-in "invincibility frames" function that can be used on any object.


## Structure Changes:

-> Objects on the HUD layer now render relative to the CENTER of the screen, (0, 0) is now the center instead of the bottom left corner. 
For example, x: -500 y: 20 would be on the left edge near the middle of the screen (on 1280 x 720).

-> The screenBuffer, keyboard and MouseInput are now all global variables/structs accessible from wherever in the project.

-> As a consequence of the new per-frame custom behaviour functions, all engine updates are now split between two functions: GameTick for events that require to be timed
or synced to real-time such as movement, animations, cutscenes, etc. and GameFrame for events that should happen on each available frame. 

-> Removed FrameThrottle functionality.

-> Rendering frequency can now be controlled independently of the GameTick frequency via the RendersPerSecond/RenderDelta variables and the RENDERS_PER_SECOND macro.

-> All object velocity movement is now run via the moveObject function, to simplify the functionality.

-> All objects are now explicitly bound to the "WorldBoundX/Y" co-ords that are held in the EngineSettings struct. This means there is a modifiable, but finite and 
defined boundary for the game scene.

-> Sound instances are now created via the createEmptySoundInstance function. 

-> Added the "GroundBox" pointer to the PhysicsRect which is used to keep track of the ground that the object is on when gravity is applied.

-> Text box portraits can now be moved relative to the box and creating a text instance is much more streamlined through a total of 3 functions. (Instead of 7.)

-> Rewrote the particle system to utilise the new animation system to make it easier to use and create new particles.


## Bug fixes/Improvements:

-> Fixed the blendPixel's formula for blending the additional transparencyEffect - it is now correctly rendered in the SDL renderer.

-> Restructured some systems to not rely on as many pre-processor defined constants, to allow for dynamic setting adjustment. (Pre-processor constants are now treated as
default settings.) Such data includes Max sounds per channel, max rendered objects/particles/UIElements, ticks per second, renders per second, etc.

-> The data inside the World struct pertaining to backgrounds is now part of a self-contained struct that now has more clearly labelled variables.

-> GameRunning is no longer a global variable - shutting down the game should be done via the GameState variable in the World struct. (GameWorld->GameState = CLOSE_GAME)

-> The functions responsible for magnetising objects to objects in motion have been replaced with an automatic updatePhysicsState function. This functionality can be
enabled by simply calling the ApplyGravity function within the ObjectBehaviour function. Any future physics functionality should be added via the updatePhysicsState 
function.

-> Rendering objects with sizes less than 1 (on either the PhysicsRect's x/y axis or Sprite's width/height axis) no longer causes unintended effects.

-> Errors encountered when loading a level will now point out the line where the error occured on, if applicable.

-> Object load commands can now be strung together without length limitation in the same way other arguements can be added (seperating different commands with a space or a comma).

-> The inAir variable of the physicsRects are now updated via the applyGravity function and also after the object moves via the
moveObject function. (This means it is now up-to-date at essentially all times.)

-> Animation file loading can now continue reading past a malformed entry/incorrect data instead of giving up as soon as it encounters an error.

-> The "getNextArg" function used to read data from files can no longer index out of bounds, assuming provided capacity value is correctly set to the capacity of the array
passed in or less.

-> Fixed the "iterateSound" function to no longer create ambiguity between a 32 bit int and a 64 bit pointer.

-> Code reorganisation has made the "prevXPos" and "prevYPos" variables within the PhysicsRect structs more reliable as they now always track the Boxes' position exactly one #
game tick ago.

-> The Parent link "POSITION_LINK" no longer relies on two volitile variables to control a child object's position relative to the parent; instead the difference between the
previous position and current position is used.

-> Textboxes have been overhauled internally; this results in mostly unchanged functionality but easier-to-read code.

-> The debug hitbox view has been changed to show outlines to be more visually clear.



## PLANNED UPCOMING FEATURES IN V0.08:

-> Better application management; Dynamic Window resizing, window title/icon editing, improved SDL compatibility, etc.

-> New pre-defined objects

-> Dialog box option prompts

-> A better cutscene manager that can work in tandem with the new dialog option prompts

-> Optimisations to various systems




# v0.06:
15/07/25

## Internal Structure changes:

   -> Separated the objectState enum into two enums: objectState and currentAction. ObjectState can be used to control what the object should do at 
    the engine level, handling deletion, special cross-object interactions such as being carried, and whether the object is an actor in a cutscene.
    The currentAction enum can be used to control the object's current action being performed at a higher level such as 'Attacking', 'Defeated' or
    'Chasing' for enemies. The two enums can be added to in order to facilitate any objects' function, although many low level functions rely on 
    the objectState enum containing the TO_BE_DELETED, PAUSE_BEHAVIOUR and ACTOR states.

   -> Moved the sprite[X/Y]Offset(s) from the physicsRect to the displaydata struct.

   -> CurrentAnimation and animationTick variables have been moved from the Object struct into the DisplayData struct.

   -> Restructured player handling so that the player is represented by both the playerdata and a player object - playerdata now contains pointers
   to parts of this player object.

   -> Fleshed out collision handling functions to automatically account for whether the object is a IMPACT type collide or an PUSH type via the 
   evaluateCollideType function. (i.e: does the object stop when hitting a wall or push the wall out of its way, respectively.)

   -> Restructured the level loading routine, and now the object that represents the player must be manually created from the leveldata file, 
   although different objects can be specified and error-handling has improved.

   -> Changed the method for deciding whether to render sprites relative to the camera from checking if the objectID is UI_ELEMENT to checking
   if the object is located on the HUD layer. Now any object can be rendered as part of the UI and vice-versa.



## New additions:

    -> Added an animation system via the DisplayData that handles animation creation and playback with modifiable framerate and loop amount.

    -> Added MoveForward and the "forwardVelocity" double variable to the physicsRect.

    -> Added a text display system that allows you to queue multiple dialogues and specify position on screen, voice, font, and more.

    -> Added the "IMPACT" collision type and the "PUSH" collision type, which can recursively push other objects when it is pushed.

    -> Added a caching system to the ObjectControllers so that unused objects in a scene can be put away to avoid unnecessary processing. This can be
    done by using a "CACHE-TRIGGER" level flag or directly via the cacheObjects function; an objectBox will be needed to represented the area in which
    objects will remain in the scene, while the rest are 'cached' away.

    -> Sprite loading and animation loading can now be 'hot-loaded' into the engine via "ANIMATION" files located in the Animations folder in
    LemonData. This method is optional, and objects can choose between loading via files or being hard-coded or both. (The game does not need to be
    recompiled in order to reorganise/modify sprites and animations if you want.)

    -> Added the pixel[X/Y]Offset(s) to the displayData struct so that sprites can be drawn from any location of its image.

    -> Added the "cutsceneManager" file to handle cutscene initialisation and playback.

    -> Added the "StartGame" function to initialise the game on start-up.

    -> Added MoveForward function which takes a physicsRect and automatically handles moving the box according to its "forwardVelocity" and direction, 
       as well as collision handling. X and Y velocities and its respective functions can now be used exclusively for absolute X/Y movements. e.g: 
       an entity with gravity (in any direction) can use MoveForward to handle moving along its actual direction, while Resolve[Y/X]Collision handles
       its gravity interactions.

    -> Added the PLAYER_OBJECT object.

    -> Added the Font, TextBox, TextPreset, VoiceMode and CollideType enums.

    -> Added the "friction" double variable which controls how fast forward velocity decays.

    -> Added the "SWITCHING_LEVEL" GameState to the LemonGameState enum to denote when the engine should switch the level so that objects can 'request'
    a level change whenever they want without having to worry about crashing due to the contents of the objectlist changing in the middle of the program
    iterating through it for object behaviour, etc. This works by setting the desired new level to the level variable in the GameWorld struct and by
    setting the GameState variable to "SWITCHING_LEVEL".

    -> Added the "HandleGameWorldEvents" function to handle level-switching, game pausing or other triggerable events for a provided GameWorld struct,
    located in the GameTick function.

    -> Added the "SET_CAMMODE" level flag so that a level can start with a specific camera mode. (Following player, free roam, etc.)

    -> Added the "transparency" variable to the displayData struct. Any objects rendered in xxx_Full_Alpha mode can now have
    an additional transparency effect applied in real-time.

    -> Added the "ParentLink" variable to control how child objects react to parent objects. (Position link, Sprite link,
    Velocity link, etc.) This can be added to an object by bitwise ORing each desired trait together as defined in the enum.
    Any part can be turned on or off.

    -> Added "invincibilityFrames" to the displayData struct to accomodate the new damaged object action; objects in this state
    will count down the invincibilityFrames variables until 0 and will revert to IDLE when reached. It will also be
    accompanied by a flashing animation independent of the animation system.

    -> Added "ApplyGravity" and "SetGravity" functions to handle applying gravity to objects and modifying the gravity value(s)
    respectively. SetGravity allows for a force and direction to be supplied, generating a vector via the GlobalGravityX and
    GlobalGravityY variables in the World structure.


## Bug fixes/Performance improvements:

    -> Objects are first evaluated using the checkBoxOverlapsBox function when checking for collision to save cpu time on unneccesary sine/cosine
    calculations where it is not neccessary. (This has noticably improved performance, especially when more than ~2000 objects are present.)

    -> Optimised the Full-Alpha blending equation to improve xxx_Full_Alpha rendering performance. (~100 -> ~300 fps)




# v0.05:
10/06/25

## Internal Structure changes:

    -> Added UIObjects and eventManager files. UIObjects handles all UI_Elements while eventManager controls in-game events such as levelflags, 
    game pausing, and other triggerable events.

    -> The main game loop no longer throttles the framerate (by default) and instead uses a GameTick system where all update functions are run 
    at most 60 times per second. Any time-reliant scripts can now assume a framerate of 60, or whatever is defined in TICKS_PER_SECOND and 
    TICK_DELTA.

    -> PhysicsRect now holds xFlip and yFlip to control hitbox orientation. These are now used to decide which way to render the displayed sprite. 
    Also moved PhysicsXVelocity and PhysicsYVelocity to PhysicsRect for compatibility.

    -> PhysicsRect now holds SpriteXOffset and SpriteYOffset, which can be used to move the location of the rendered sprite relative to the 
    PhysicsRect hitbox.

    -> Refactored the Collision functions to now be usable by any object with a PhysicsRect. There are now two sets of collision functions: 
    Resolve[X/Y]Collision and Resolve[X/Y]CollisionByPush. The former is used when the moving object in question should conoform to the world 
    around it, and the latter should be used to push collided objects out of its way. Both of these functions take in the previous position as 
    an arguement as they expect the moving object in question to have already moved for that frame.
    NOTE: Currently the Resolve[X/Y]Collision function handles all collision neccessary, but Resolve[X/Y]CollisionByPush can only handle collision 
    with two specific objects at a time, for example for handling an object pushing the player. For full world interaction, a loop must be manually 
    created similar to that in Resolve[X/Y]Collision. This may change in the future.

    -> Objects now contain a ParentObject pointer which is initialised as NULL. When an object is marked as deleted, all objects that has it as 
    its parent will also be deleted recursively, meaning it will propogate down. 

    -> Added the DisplayData struct to standardise how sprites are loaded/rendered across objects and players, etc. - DisplayData holds the 
    spriteBuffer, the spriteSet pointer and more.

    -> All objects (objects, player, gameWorld backgrounds) that switch sprites has been updated so that the spriteSet pointer itself is also 
    moved to the appropriate spriteset for faster contiguous sprite loading.

    -> Changed the behaviour of LemonGameState - CUTSCENE state will now only disable objects that do NOT have the "ACTOR" state. This 
    means any object can be brought into a cutscene to be animated. CLOSE_GAME state indicates the game instance is being closed, and when set the 
    engine will end execution.

    -> Expanded the functionality of the camera; added MENU_CAMERA and FREE_ROAM_RESTRICTED camera modes. Removed FREEZE_POSITION (functionality 
    for freezing camera is available from FREE_ROAM or FREE_ROAM_RESTRICTED).

    -> Merged FLAT_SLOPE_LR and FLAT_SLOPE_RL into a single FLAT_SLOPE solid type, with the xFlip denoting the rotation of the hitboxes' shape as 
    well.




## New additions:

    -> As part of the HUD/Menu system added, a new object type called "UI_ELEMENT" has been added that is immune to the gameWorld being paused, is 
    rendered relative to the camera's center as opposed to the gameWorld, and uses arg1 to denote its subtype i.e: option buttons, animatable 
    graphics, and other prompts.

    -> Implemented an object-based UI system for HUDs, menus and prompts.

    -> Added Pause menu using implemented UI.

    -> Implemented Level Flag triggers such as Set Background Trigger and Set Camera Bounds Trigger. They are spawnable from leveldata files.

    -> Implemented a load command which allows for objects to be loaded with additional parameters from the leveldata. Commands added include "UNSOLID"; 
    sets the solid type of the object to unsolid, "TOBACKGROUND"; sets the object layer to the background, etc.

    -> Implemented the ability to add comments to leveldata files, for internal management or debugging purposes.




## Bug fixes/Performance improvements:

    -> Lowered Dependency count for new solid types from 10-12 to 5-6. (Dependency count in this case refers to how many functions must be updated 
    to allow for full functionality.) 

    -> Fixed collisions with moving slope solids to behave as expected.

    -> Fixed Jump-Through solid type to function with new collision system. (Removed use of previous y step for colliding object)

    -> Fixed sounds reseting their volume to full when looping in the loop channel. 
    (As a side effect of the method of this fix, volume can now be dynamically updated on a per sound instance basis, however it will not take 
    into effect until looping or manually mixed via SDL_MixAudio.)

    -> As a side effect of the new DisplayData, objects can now skip the spriteSet query step of the switch sprite function, meaning less overhead 
    when switching the sprite in the spriteBuffer.

    -> Replaced all sprite-loading and handling variables/functions to now use the new DisplayData struct.

    -> Refactored the sprite loading code to be more in-line with more up-to-date coding practices and to make it easier to understand.

    -> Added loadSprite/loadSpriteIntoSpriteSet function which is compatible with all objects and lets you define the folder it pulls the sprite from.


