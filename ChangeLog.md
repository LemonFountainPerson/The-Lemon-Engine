
# v0.05:
10/06/25

Internal Structure changes:

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




New additions:

    -> As part of the HUD/Menu system added, a new object type called "UI_ELEMENT" has been added that is immune to the gameWorld being paused, is 
    rendered relative to the camera's center as opposed to the gameWorld, and uses arg1 to denote its subtype i.e: option buttons, animatable 
    graphics, and other prompts.

    -> Implemented an object-based UI system for HUDs, menus and prompts.

    -> Added Pause menu using implemented UI.

    -> Implemented Level Flag triggers such as Set Background Trigger and Set Camera Bounds Trigger. They are spawnable from leveldata files.

    -> Implemented a load command which allows for objects to be loaded with additional parameters from the leveldata. Commands added include "UNSOLID"; 
    sets the solid type of the object to unsolid, "TOBACKGROUND"; sets the object layer to the background, etc.

    -> Implemented the ability to add comments to leveldata files, for internal management or debugging purposes.




Bug fixes/Performance improvements:

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




# v0.06:
15/07/25

Internal Structure changes:

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



New additions:

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

    -> Partially added multi-threaded rendering enablable via the MULTITHREADED_ENABLED macro. 



Bug fixes/Performance improvements:

    -> Objects are first evaluated using the checkBoxOverlapsBox function when checking for collision to save cpu time on unneccesary sine/cosine
    calculations where it is not neccessary. (This has noticably improved performance, especially when more than ~2000 objects are present.)

    -> Optimised the Full-Alpha blending equation to improve xxx_Full_Alpha rendering performance. (~100 -> ~300 fps)



# v0.07
21/09/25

New Features:

-> The Lemon Engine is now 64 bit! Along with some internal changes, this has increased perfomance slightly across the board.

-> Added full sprite rotation for non-tiled rendermodes, via the direction variable in the displayData struct.

-> Added the ability to define custom code execution on a per-frame basis instead of per-Tick - represented as a linked list of FrameUpdateFunction structs.
Object types can create multiple instances, and all instances associated with a specific object will be deleted when that object is deleted.

-> Added full Mouse Input: x, y co-ords, Left mouse button, Right mouse button, Middle mouse button, etc.

-> Added convenience functions for adding different types of strings to the debug string.

-> Animation frames can now store a rotation offset in degrees that is applied dynamically.

-> Added a built-in "invincibility frames" function that can be used on any object.


Structure Changes:

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


Bug fixes/Improvements:

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

-> Fixed the "iterateSound" function to no longer create ambiguity between a 32 bit int and a 32/64 bit pointer.

-> Code reorganisation has made the "prevXPos" and "prevYPos" variables within the PhysicsRect structs more reliable as they now always track the Boxes' position exactly one #
game tick ago.

-> The Parent link "POSITION_LINK" no longer relies on two volitile variables to control a child object's position relative to the parent; instead the difference between the
previous position and current position is used.

-> Textboxes have been overhauled internally; this results in mostly unchanged functionality but easier-to-read code.

-> The debug hitbox view has been changed to show outlines to be more visually clear.



PLANNED UPCOMING FEATURES IN V0.08:

-> Better application management; Dynamic Window resizing, window title/icon editing, improved SDL compatibility, etc.

-> New pre-defined objects

-> Dialog box option prompts

-> A better cutscene manager that can work in tandem with the new dialog option prompts

-> Optimisations to various systems