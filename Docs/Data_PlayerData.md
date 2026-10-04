# PlayerData

## Definition
```
typedef struct playerData
{
	Object *PlayerPtr;
	ObjectReference PlayerRef;

	PhysicsBox InteractBox;

	// These variables can be freely modified according to your modified player controller
	int jumpProgress;
	bool jumpHeld;
	float jumpForce;
	int coyoteFrames;
	int jumpRange;
	int cancelRange;

	int coinCount;
} PlayerData;
```

## Description
This struct represents the state of the player, and usually has a reference to an [Object](Data_Object.md) which represents 
or is under the control of the player. Every [World](Data_World.md) has one, and typically the engine treats the player as if there
can only be one at a time. It is not impossible to change this, but one player is the default. (This does not apply to networking.)

## Version
Available since V0.04.

----
