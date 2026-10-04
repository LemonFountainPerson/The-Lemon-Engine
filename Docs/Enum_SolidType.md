# SolidType

## Definition
```
typedef enum SolidType 
{
	UNSOLID = 0,
	SOLID,
	JUMP_THROUGH,
	BODY,
	PUSHABLE_SOLID,
	UNDEFINED_SOLID
} SolidType;
```

## Description
This enum defines the type of collision a [PhysicsBox](Data_PhysicsBox.md) is using, and how others 
will interact with it. For example:
- UNSOLID: No collision.
- SOLID: Full collision, from all sides.
- JUMP_THROUGH: Collision but only from Boxes approaching from above. Anything moving from below or
to its' sides will not collide.
- BODY: Fully solid, but will not collide with other Boxes with the BODY solid type. E.g: two enemies
will pass through each other but will collide with the ground and walls as normal.
- PUSHABLE_SOLID: Full collision, but can be pushed by other Boxes.


## Version
Available since V0.04.


----