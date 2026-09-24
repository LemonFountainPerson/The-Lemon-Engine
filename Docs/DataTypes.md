# World

## Definition
```
typedef struct World
{
	Camera MainCamera;
	CameraView views[VIEW_COUNT];

	PlayerData Player;
	ObjectController ObjectList;

	BackgroundData WorldBackground;

	TextList TextList;
	FontList FontList;

	int GamePaused;
	int level;
	LemonGameState GameState;
	GameEventManager GameEvents;
	
	CutsceneID CurrentCutscene;
	bool TextBox;
	SceneAction *SceneActionQueue;
	SceneAction *nextSceneAction;
	int SceneActionCount;

	WorldPhysics PhysicsType;
	float GlobalGravityY;
	float GlobalGravityX;
} World;
```

## Description
The Backbone of the entire Lemon Engine, this struct represents an entire world with Objects, GameEvents, SceneActions, etc. 
A World represents the game's entire state and switching to another is equivalent to switching the type of game being played.
The state of the application window, developer console and settings are not contained within the World.


## Version
Available since V0.04.

----


# Object

## Definition
```
typedef struct Object
{
	char name[OBJECT_NAME_LENGTH];	// unique identifier for Object 		
	int ObjectID;
	ObjectState State;
	CurrentAction Action;
	ReservedFlags reserved;
	int instanceNumber;

	struct Object *Parent;
	ParentType ParentLink;

	PhysicsBox * const ObjectBox;
	DisplayData * const ObjectDisplay;

	struct Object *nextObject;
	struct Object *prevObject;	
	const int index;

	// Multi-purpose args
	int arg1;
	int arg2;
	int arg3;
	int arg4;
} Object;
```

## Description
The Object struct is used to represent a distinct instance of an [Object Type](EnumTypes.md#ObjectType) within the GameWorld. 
They will always have an attached [PhysicsBox](#PhysicsBox) and a [DisplayData](#DisplayData), and can have additional components added on to expand its functionality.


## Version
Available since V0.04.

----


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


# DisplayData

## Definition
```
typedef struct displayData
{
	int currentSprite;
	Sprite *spriteBuffer;

	int currentAnimation;
	int currentFrame;
	float animationTick;
	int animationLoopCount;
	float animationSpeed;
	AnimationFrame *frameBuffer;
	Animation *animationBuffer;

	SpriteSet *spriteSetSource;

	Layer layer;
	RenderMode RenderModeOverride;
	float size;
	RotationMode rotateMode;
	float spriteXOffset;
	float spriteYOffset;
	unsigned int pixelXOffset;
	unsigned int pixelYOffset;

	float transparency;		// 0.0 is no transparency - 1.0 is full transparency (invisible) -- SDL uses 0-255 where 255 is no transparency and 0 is fully transparent
	bool hidden;
} DisplayData;
```

## Description


## Version
Available since V0.05.


----