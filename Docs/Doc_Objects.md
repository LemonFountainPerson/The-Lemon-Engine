# Objects
_________________________________________________

The core asset in the Lemon Engine are objects; these represent almost everything in the gameworld such as the player, visible elements, geometry, etc. 
Objects consist of three main parts: 

The main [Object](Data_Object.md) struct is the most important element of an object, and it represents all logical attributes of an object. Objects are organised by IDs, 
with each ID uniquely defining behaviour. (E.g representing what the object is, enemy, collectable, interactable sign, etc.)

[PhysicsBoxes](Data_PhysicsBox.md) represent the shape and collision state of an object. It controls how it will collide with other objects (if it should) as well as its
current position, direction and velocities. The current position (xPos, yPos) is stored alongside the previous position (prevXPos, prevYPos) from 1 GameTick ago.
'XVelocity' and 'YVelocity' are self-explanitory, but 'forwardVelocity' defines a separate velocity that is used to move the object along its pointed direction. Using
this is optional.
'PhysicsXVelocity' and 'PhysicsYVelocity' are used by the built-in physics to control momentum given by a moving platform, and do not need to be modified directly.
Likewise, 'inAir' and 'GroundBox' are also paramters used to controlphysics; inair is set to 0 when on the ground, and is incremented once per tick while the object 
is in the air, up to 100. 
The 'shape' variable defines what shape the hitbox is, while the 'solid' variable describes its behaviour.

[DisplayDatas](Data_DisplayData.md) are used to control the sprites rendered and animations playing on the object, if it has any. For more info on Animations, check out the Animations section.

In addition to these 3 base components, additional components can be created and added via the ObjectComponent system. Each type of component has a SparseList to store
them, allowing for fast and space efficient structs of data that can 'attached' to specific instances of objects to expand their functionality. Examples include TileMaps and 
HealthComponents.

Objects can have a parent object through the ParentObject variable, with the nature of the connection defined in the [ParentLink](Enum_ParentType.md) variable. The minimum consequence of a parent-
child relation is that when the parent is deleted, all children and sub-children are deleted as well on the same tick. Other connections such as matching the position of the 
parent or the animation of the parent can be added as wished by modifying the child's ParentLink variable by bitwise ORing different options. For example, for children who
follow the motion and transparency of the parent are configured by setting the ParentLink to [MOTION_LINK | TRANSPARENCY_LINK] and so on for other options.

