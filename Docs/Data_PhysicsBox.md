# PhysicsBox

## Definition
```
typedef struct PhysicsBox
{
	float xPos;
	float yPos;
	float prevXPos;
	float prevYPos;

	float xSize;
	float ySize;

	float forwardVelocity;
	float yVelocity;
	float xVelocity;

	int inAir;
	float PhysicsXVelocity;
	float PhysicsYVelocity;
	struct PhysicsBox *GroundBox;

	SolidShape shape;
	SolidType solid;
	SolidFlag flag;
	Layer collideLayer;

	double direction;
	short xFlip;
	short yFlip;
	bool crouch;
} PhysicsBox;
```

## Description
PhysicsBoxes represent an [Object's](Data_Object.md) physical position, velocity, size, shape, direction and anything else to do with physics. 
All Objects have a PhysicsBox and it is an essential piece of the engine.

Of note:
- The 'GroundBox' variable defines what Box is the ground, if gravity is enabled. The 'inAir' shows if the Box is in the air, and if so for 
how many Game Ticks it has been.
- The 'forwardVelocity' variable is the velocity along the Boxes' direction it is facing.
- The 'collideLayer' variable is a [Layer](Enum_Layer.md) enum and determines which layer to perform collision checks on. If two Boxes are on 
different layers, they cannot collide. The default value is 'MIDDLEGROUND'.
- The 'shape' variable defines the Boxes' shape but the 'solid' variable is a [SolidType](Enum_SolidType.md) enum and defines what kind of 
collision it uses. 'UNSOLID' means it has no collision, while 'SOLID' means it is fully solid in all directions. 


## Version
Available since V0.04.

----
