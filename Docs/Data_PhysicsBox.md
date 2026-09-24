# PhysicsBox

## Definition
```
typedef struct PhysicsBox
{
	float xPos;
	float yPos;
	float prevXPos;
	float prevYPos;

	int xSize;
	int ySize;

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


## Version
Available since V0.04.

----
