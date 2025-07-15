
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




# v0.06:
15/07/25

## Internal Structure changes:

    -> Separated the objectState enum into two enums: objectState and currentAction. ObjectState can be used to control what the object should do at 
       the engine level, handling deletion, special cross-object interactions such as being carried, and whether the object is an actor in a cutscene.
       The currentAction enum can be used to control the object's current action being performed at a higher level such as 'Attacking', 'Defeated' or
       'Chasing' for enemies. The two enums can be added to in order to facilitate any objects' function, although many low level functions rely on 
       the objectState enum containing the TO_BE_DELETED, PAUSE_BEHAVIOUR and ACTOR states.

   -> Moved the sprite[X/Y]Offset(s) from the physicsRect to the displaydata struct.

   -> Added the "Fonts", "VoiceMode" and "TextPreset" enums to facilitate text box functionality.

   -> Restructured player handling so that the player is represented by both the playerdata and a player object - playerdata now contains pointers
   to parts of this player object.

   -> Fleshed out collision handling functions to automatically account for whether the object is a IMPACT type collide or an PUSH type via the 
   evaluateCollideType function. (i.e: does the object stop when hitting a wall or push the wall out of its way, respectively.)



## New additions:

    -> Added an animation system via the DisplayData that handles animation creation and playback with modifiable framerate and loop amount.

    -> Added MoveForward and the "forwardVelocity" double variable to the physicsRect.

    -> Added a text display system that allows you to queue multiple dialogues and specify position on screen, voice, font, and more.

    -> Added the "IMPACT" collision type and the "PUSH" collision type, which can recursively push other objects when it is pushed.

    -> Added a caching system to the ObjectControllers so that unused objects in a scene can be put away to avoid unnecessary processing. This can be
    done by using a "CACHE-TRIGGER" level flag or directly via the cacheObjects function; an objectBox will be needed to represented the area in which
    objects will remain in the scene, while the rest are 'cached' away.

    -> Added the pixel[X/Y]Offset(s) to the displayData struct so that sprites can be drawn from any location of its image.

    -> Added the "cutsceneManager" file to handle cutscene initialisation and playback.

    -> Added the "StartGame" function to initialise the game on start-up.

    -> Added MoveForward function which takes a physicsRect and automatically handles moving the box according to its "forwardVelocity" and direction, 
       as well as collision handling. X and Y velocities and its respective functions can now be used exclusively for absolute X/Y movements. e.g: 
       an entity with gravity (in any direction) can use MoveForward to handle moving along its actual direction, while Resolve[Y/X]Collision handles
       its gravity interactions.

    -> Added the PLAYER_OBJECT object.

    -> Added the CollideType enum.

    -> Added the "friction" double variable which controls how fast forward velocity decays.



## Bug fixes/Performance improvements:

    -> Objects are first evaluated using the checkBoxOverlapsBox function when checking for collision to save cpu time on unneccesary sine/cosine
    calculations where it is not neccessary. (This has noticably improved performance, especially when more than ~2000 objects are present.)
