
# v0.05:

** Internal Structure changes: **

-> Added UIObjects and eventManager files. UIObjects handles all UI_Elements while eventManager controls in-game events such as levelflags, game pausing, and other triggerable events.

-> The main game loop no longer throttles the framerate (by default) and instead uses a GameTick system where all update functions are run at most 60 times per second. Any time-reliant 
scripts can now assume a framerate of 60, or whatever is defined in TICKS_PER_SECOND and TICK_DELTA.

-> PhysicsRect now holds xFlip and yFlip to control hitbox orientation. These are now used to decide which way to render the displayed sprite. Also moved PhysicsXVelocity and 
PhysicsYVelocity to PhysicsRect for compatibility.

-> Physicsrect now holds SpriteXOffset and SpriteYOffset, which can be used to move the location of the rendered sprite reltive to the PhysicsRect hitbox.

-> Refactored the Collision functions to now be usable by any object with a physicsRect. There are now two sets of collision functions: Resolve[X/Y]Collision and 
Resolve[X/Y]CollisionByPush. The former is used when the moving object in question should conoform to the world around it, and the latter should be used to push collided objects out of 
its way. Both of these functions take in the previous position as an arguement as they expect the moving object in question to have already moved for that frame.
NOTE: Currently the Resolve[X/Y]Collision function handles all collision neccessary, but Resolve[X/Y]CollisionByPush can only handle collision with two specific objects at a time, for
example for handling an object pushing the player. For full world interaction, a loop must be manually created similar to that in Resolve[X/Y]Collision. This may change in the future.

-> Objects now contain a ParentObject pointer which is initialised as NULL. When an object is marked as deleted, all objects that has it as its parent will also be deleted recursively, 
meaning it will propogate down. 

-> Added the DisplayData struct to standardise how sprites are loaded/rendered across objects and players, etc. - DisplayData holds the spriteBuffer, the spriteSet pointer and more.

-> All objects (objects, player, gameWorld backgrounds) that switch sprites has been updated so that the spriteSet pointer itself is also moved to the appropriate spriteset for faster 
contiguous sprite loading.

-> Changed the behaviour of LemonGameState - CUTSCENE state will now only disable objects that do NOT have the "CUTSCENE_ACTOR" state. This means any object can be brought into a 
cutscene to be animated. CLOSE_GAME state indicates the game instance is being closed, and when set the engine will end execution.

-> Expanded the functionality of the camera; added MENU_CAMERA and FREE_ROAM_RESTRICTED camera modes. Removed FREEZE_POSITION (functionality for freezing camera is available from 
FREE_ROAM or FREE_ROAM_RESTRICTED).

-> Merged FLAT_SLOPE_LR and FLAT_SLOPE_RL into a single FLAT_SLOPE solid type, with the xFlip denoting the rotation of the hitboxes' shape as well.



** New additions: **

-> As part of the HUD/Menu system added, a new object type called "UI_ELEMENT" has been added that is immune to the gameWorld being paused, is rendered relative to the camera's center 
as opposed to the gameWorld, and uses arg1 to denote its subtype i.e: option buttons, animatable graphics, and other prompts.

-> Implemented an object-based UI system for HUDs, menus and prompts.

-> Added Pause menu using implemented UI.

-> Implemented Level Flag triggers such as Set Background Trigger and Set Camera Bounds Trigger. They are spawnable from leveldata files.

-> Implemented a load command which allows for objects to be loaded with additional parameters from the leveldata. Commands added include "UNSOLID"; sets the solid type of the object 
to unsolid, "TOBACKGROUND"; sets the object layer to the background, etc.

-> Implemented the ability to add comments to leveldata files, for internal management or debugging purposes.



** Bug fixes/Performance improvements: **

-> Lowered Dependency count for new solid types from 10-12 to 5-6. (Dependency count in this case refers to how many functions must be updated to allow for full functionality.) 

-> Fixed collisions with moving slope solids to behave as expected.

-> Fixed Jump-Through solid type to function with new collision system. (Removed use of previous y step for colliding object)

-> Fixed sounds reseting their volume to full when looping in the loop channel. 
(As a side effect of the method of this fix, volume can now be dynamically updated on a per sound instance basis, however it will not take into effect until looping or manually mixed 
via SDL_MixAudio.)

-> As a side effect of the new DisplayData, objects can now skip the spriteSet query step of the switch sprite function, meaning less overhead when switching the sprite in the 
spriteBuffer.

-> Replaced all sprite-loading and handling variables/functions to now use the new DisplayData struct.

-> Refactored the sprite loading code to be more in-line with more up-to-date coding practices and to make it easier to understand.

-> Added loadSprite/loadSpriteIntoSpriteSet function which is compatible with all objects and lets you define the folder it pulls the sprite from.
