#include "LemonEngine.h"


static ComponentData *currentComponents = NULL;


Object* AddObject(World *GameWorld, int objectID, int xPos, int yPos, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return NULL;
	}

	ObjectController *ObjectList = GameWorld->ObjectList;
	currentComponents = &ObjectList->objectComponents;

	if (ObjectList->objectCount + ObjectList->cachedCount >= EngineSettings.MaxObjects)
	{
		return NULL;
	}

	if (objectID >= OBJECT_TYPE_COUNT || objectID < LEVEL_FLAG_OBJ)
	{
		putConsoleString("\nThis object is not defined! Type: %d", objectID);
		return NULL;
	}


	Object *newObject = getNewObject(objectID, ObjectList);
	
	if (newObject == NULL)
	{
		return NULL;
	}

	// Debug
	if (DebugSettings.ConsoleTextEnabled == CONSOLE_ALL_EVENTS)
	{
		putConsoleStringTS("Created object ---- ID: %d (%s) ", objectID, getObjectIDName(objectID));
	}


	// Default settings
	newObject->ObjectBox->xSize = X_TILESCALE;
	newObject->ObjectBox->ySize = Y_TILESCALE;
	
	newObject->arg1 = arg1;
	newObject->arg2 = arg2;
	newObject->arg3 = arg3;
	newObject->arg4 = arg4;

	newObject->ObjectBox->xPos = xPos;
	newObject->ObjectBox->yPos = yPos;


	// Set Object parameters
	switch (objectID)
	{
	case PROJECTILE:
		newObject->ObjectBox->xSize = X_TILESCALE;
		newObject->ObjectBox->ySize = 8;

		newObject->ObjectBox->forwardVelocity = 16.0;
		newObject->ObjectBox->solid = UNSOLID;

		setRenderModeOverride(newObject, SINGLE);
		break;


	case PLAYER_OBJECT:
		InitialisePlayerObject(newObject, GameWorld);
		break;

	case LEVEL_DOOR:
		newObject->ObjectBox->solid = JUMP_THROUGH;
		newObject->ObjectBox->xSize = X_TILESCALE << 1;
		newObject->ObjectBox->ySize = Y_TILESCALE * 3;
		break;

	case DOOR:
		newObject->ObjectBox->solid = JUMP_THROUGH;
		newObject->ObjectBox->xSize = X_TILESCALE << 1;
		newObject->ObjectBox->ySize = Y_TILESCALE * 3;

		if (newObject->arg3 == 0)
		{
			newObject->arg3 = 1;
			newObject->Parent = AddObject(GameWorld, DOOR, arg1, arg2, xPos, yPos, 1, 0, 0);

			newObject->Parent->Parent = newObject;
		}
		
	break;

	case SOLID_BLOCK:
		snapPositionToTileGrid(newObject, xPos, yPos);

		if (arg1 < 1 || arg2 < 1)
		{
			arg1 = 1;
			arg2 = 1;
		}
		
		newObject->ObjectBox->xSize = arg1 * X_TILESCALE;
		newObject->ObjectBox->ySize = arg2 * Y_TILESCALE;
		newObject->State = STATIC_STATE;
		addTileMap(newObject, 32, 32, 32);
		
		if (arg3 == 0)		
		{
			arg3 = 1;
		}

		if (arg3 >= 0)
		{
			switchObjectSprite(arg3, newObject);
		}
		else
		{
			setRenderModeOverride(newObject, DO_NOT_RENDER);
		}
		break;


	case FLAT_SLOPE_FLOOR:
	//Angle: Y = (X * ySize/xSize)
	//Angle: X = (Y / (ySize/xSize))
		snapPositionToTileGrid(newObject, xPos, yPos);
		newObject->ObjectBox->solid = FLAT_SLOPE;
		newObject->State = STATIC_STATE;
		newObject->ObjectBox->xSize = arg1;
		newObject->ObjectBox->ySize = arg2;

		// switch out for higher resolution sprites if size is bigger
		if (arg1 + arg2 > 256)
		{
			switchObjectSprite(3, newObject);
		}
		else if (arg1 + arg2 > 128)
		{
			switchObjectSprite(2, newObject);
		}

		// calculate flipping
		if (arg3 == -1)
		{
			newObject->ObjectBox->xFlip = -1;
			arg3 = arg1;
			arg1 = 0;
		}
		else
		{
			arg3 = 0;
		}

		if (arg4 == -1)
		{
			newObject->ObjectBox->yFlip = -1;
			arg4 = 0;
		}
		else
		{
			arg4 = arg2;
			arg2 = 0;
		}

		// addPolygon(newObject, 3, 
		// 	(float)arg3, (float)arg4, 0.0, 1.0,
		// 	(float)arg1, (float)arg4, 1.0, 1.0,
		// 	(float)arg1, (float)arg2, 1.0, 0.0);
		break;


	case JUMP_THRU_BLOCK:
		snapPositionToTileGrid(newObject, xPos, yPos);
		newObject->ObjectBox->solid = JUMP_THROUGH;
		newObject->ObjectBox->xSize = arg1 * X_TILESCALE;
		newObject->ObjectBox->ySize = arg2 * Y_TILESCALE;
		break;

	case COIN:
	// Coin
		newObject->arg1 = arg1;
		newObject->arg2 = arg2;
		newObject->arg3 = 1;
		newObject->ObjectBox->solid = UNSOLID;
		newObject->ParentLink = FINAL_LINK;
		PlayObjectAnimation("Coin_Spin", 0, newObject);
		snapPositionToTileGrid(newObject, xPos, yPos);
		break;

	
	case SPRING:
	// spring
		newObject->arg1 = arg1;
		SetObjectDirection(newObject, (DEFAULT_DIRECTION + (double)arg2));
		newObject->ObjectBox->solid = UNSOLID;
		snapPositionToTileGrid(newObject, xPos, yPos);
		break;


	case VERTICAL_GATE:
	case HORIZONTAL_GATE:
		newObject->ObjectBox->flag = IGNORE_SOLID;
		newObject->arg1 = arg3;	// ID
		newObject->arg2 = arg4;	 // speed
		newObject->arg3 = yPos;
		newObject->Action = IDLE;
		newObject->ObjectBox->xSize = arg1 * X_TILESCALE;
		newObject->ObjectBox->ySize = arg2 * Y_TILESCALE;
		addPhysics(newObject, false);
		snapPositionToTileGrid(newObject, xPos, yPos);
		switchSpriteByName("MissingMeasure", USE_CURRENT_SPRITESET, newObject->ObjectDisplay);
		break;


	case GATE_SWITCH:
	case GATE_SWITCH_TIMED:
	// switch for gate - arg1 is gate ID, arg2 denotes type of switch (0 = or switch, 1 = and switch) 
		if (newObject->ObjectID != GATE_SWITCH_TIMED)
		{
			newObject->arg3 = 0;
			newObject->Action = arg3;
		}
		else
		{
			newObject->Action = arg4;
		}
		
		newObject->ObjectBox->solid = UNSOLID;
		snapPositionToTileGrid(newObject, xPos, yPos);

		toggleGateSwitch(newObject, ObjectList);
		break;


	case MOVING_PLATFORM_VER:
	case MOVING_PLATFORM_HOR:
		InitialiseMovingPlatform(newObject, objectID, xPos, yPos, arg1, arg2, arg3, arg4);
		break;


	case PARTICLE:
		// Do not modify! (Unless you wish to alter rendermode)
		InitialiseParticle(newObject, arg1, arg2, arg3);
		break;


	case LEVEL_FLAG_OBJ:
	// Like particles, arg1 defines the subtype of the level flag object
		InitialiseLevelFlag(newObject, ObjectList);
		break;


	case UI_TEXT:
		InitialiseUIText(newObject, GameWorld);
		break;


	case UI_ELEMENT:
		InitialiseUIElement(newObject, GameWorld);
		break;


	case BASIC_ENEMY:
		newObject->ObjectBox->solid = BODY;
		newObject->ObjectBox->forwardVelocity = 3.0;
		newObject->ObjectBox->xSize = 40;
		newObject->ObjectBox->ySize = 60;
		addHealthComponent(newObject, 1000);
		addPhysics(newObject, true);
		break;


	case PUSHABLE_BOX:
		newObject->ObjectBox->solid = PUSHABLE_SOLID;
		newObject->ObjectBox->xSize = arg1;
		newObject->ObjectBox->ySize = arg2;
		addPhysics(newObject, true);
		break;


	default:
		#ifdef LEMON_USE_CUSTOM_CALLBACKS
		InitialiseObject(newObject, GameWorld);
		#endif
		break;
	}


	newObject->ObjectBox->prevXPos = newObject->ObjectBox->xPos;
	newObject->ObjectBox->prevYPos = newObject->ObjectBox->yPos;

	UpdateObjectDisplay(newObject, 0.0);

	
	return newObject;
}

Object* AddNamedObject(World *GameWorld, const char name[], int objectID, int xPos, int yPos)
{
	if (name == NULL || strlen(name) >= OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	if (DebugSettings.ConsoleTextEnabled == CONSOLE_ALL_EVENTS)
	{
		DebugSettings.ConsoleTextEnabled = 9999;		// this is kind of dumb
	}

	Object *createdObject = AddObject(GameWorld, objectID, xPos, yPos, 0, 0, 0, 0, 0);

	if (createdObject == NULL)
	{
		return NULL;
	}

	strcpy(createdObject->name, name);

	// Debug
	if (DebugSettings.ConsoleTextEnabled == 9999)	// this is still kind of dumb
	{
		DebugSettings.ConsoleTextEnabled = CONSOLE_ALL_EVENTS;
		putConsoleStringTS("Created object ---- ID: %d (%s)  Named: %s", objectID, getObjectIDName(objectID), name);
	}

	return createdObject;
}

Object* AddObjectWithParent(World *GameWorld, Object *ParentObject, int objectID, int xPos, int yPos, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	if (ParentObject == NULL)
	{
		return NULL;
	}

	Object *newObject = AddObject(GameWorld, objectID, xPos, yPos, arg1, arg2, arg3, arg4, arg5);

	if (newObject != NULL)
	{
		newObject->Parent = ParentObject;
	
		UpdateParentChildLink(newObject);
	}

	return newObject;
}


Object* AddParticle(World *GameWorld, ParticleSubType animation, int xPos, int yPos, int repeatCount, int particleLifeTime)
{
	return AddObject(GameWorld, PARTICLE, xPos, yPos, animation, repeatCount, particleLifeTime, 0, 0);
}

Object* cloneObject(Object *input, World *GameWorld)
{
	if (input == NULL)
	{
		return NULL;
	}

	Object *newObject = AddObject(GameWorld, input->ObjectID, 0, 0, 0, 0, 0, 0, 0);

	if (newObject == NULL)
	{
		return NULL;
	}


	memcpy(newObject->ObjectBox, input->ObjectBox, sizeof(PhysicsBox));

	newObject->Action = input->Action;
	newObject->arg1 = input->arg1;
	newObject->arg2 = input->arg2;
	newObject->arg3 = input->arg3;
	newObject->arg4 = input->arg4;
	newObject->Parent = input->Parent;
	newObject->ParentLink = input->ParentLink;
	newObject->State = input->State;
	setObjectName(newObject, "Clone");

	// Debug
	if (DebugSettings.ConsoleTextEnabled == CONSOLE_ALL_EVENTS)
	{
		putConsoleString("as clone of %s", input->name);
	}

	return newObject;
}


Object* getNewObject(ObjectType objectID, ObjectController *ObjectList)
{
	Object *newObject = findNewObject(ObjectList);

	initialiseGenericObject(newObject, objectID, ObjectList);

	return newObject;
}


Object* findNewObject(ObjectController *ObjectList)
{
	if (ObjectList == NULL || ObjectList->availableSlots == NULL)
	{
		return NULL;
	}

	Object *newObject = ObjectList->availableSlots;
	ObjectList->availableSlots = newObject->nextObject;

	if (ObjectList->availableSlots != NULL)
	{
		ObjectList->availableSlots->prevObject = NULL;
	}

	newObject->nextObject = NULL;

	return newObject;
}

int initialiseGenericObject(Object *inputObject, ObjectType objectID, ObjectController *ObjectList)
{
	if (inputObject == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	Object *currentObject = ObjectList->lastObject;

	if (currentObject != NULL)
	{
		inputObject->nextObject = currentObject->nextObject;
		inputObject->prevObject = currentObject;

		if (currentObject->nextObject != NULL)
		{
			// This shouldn't happen, as lastobject should point to the LAST object! (Although i put this in just in case to avoid bad pointer values)
			currentObject->nextObject->prevObject = inputObject;
		}

		currentObject->nextObject = inputObject;
	}
	else
	{
		inputObject->nextObject = NULL;
		inputObject->prevObject = NULL;
		ObjectList->firstObject = inputObject;
	}

	ObjectList->lastObject = inputObject;
	ObjectList->objectCount++;

	// initialise data
	resetPhysicsBox(inputObject->ObjectBox);

	DisplayData *display = getDisplay(inputObject);
	if (display != NULL)
	{
		resetDisplayData(display);
		display->spriteSetSource = loadSpriteSet(ObjectList, objectID);
		display->layer = MIDDLEGROUND;
	}
	
	inputObject->Parent = NULL;
	inputObject->ParentLink = DEFAULT_LINK;
	inputObject->reserved = RFLAG_DEFAULT;
	strcpy(inputObject->name, "Generic");
	inputObject->ObjectID = objectID;
	inputObject->State = DEFAULT_STATE;
	inputObject->Action = IDLE;
	inputObject->arg1 = 0;
	inputObject->arg2 = 0;
	inputObject->arg3 = 0;
	inputObject->arg4 = 0;

	return LEMON_SUCCESS;
}

void clearObjectData(Object *input)
{
	if (input == NULL)
	{
		return;
	}

	PhysicsBox *box = input->ObjectBox;

	resetPhysicsBox(box);
	resetDisplayData(getDisplay(input));

	memset(input->name, 0, OBJECT_NAME_LENGTH);
	input->ObjectID = 0;
	input->Action = IDLE;
	input->reserved = RFLAG_DEFAULT;
	input->Parent = NULL;
	input->ParentLink = 0;
	input->nextObject = NULL;
	input->prevObject = NULL;

	input->State = EMPTY_OBJECT;

	return;
}


SpriteSet* loadSpriteSet(ObjectController *ObjectList, int ObjectID)
{
	if (ObjectID <= LEVEL_FLAG_OBJ || ObjectID >= OBJECT_TYPE_COUNT)
	{
		return NULL;
	}

	// Check for pre-existing spriteset
	SpriteSet *newSet = getSpriteSet(&ObjectList->spriteSets, ObjectID);

	if (newSet != NULL)
	{
		return newSet;
	}

	// Fill sprite set with sprites/animations

	
	newSet = loadSpriteSetFromFile(getObjectIDName(ObjectID), &ObjectList->spriteSets, ObjectID);

	// Searches for a file named "Object [ObjectID]" as default if name does not exist
	if (newSet == NULL)
	{
		char defaultName[32] = {0};
		snprintf(defaultName, 32, "Object %d", ObjectID);

		newSet = loadSpriteSetFromFile(defaultName, &ObjectList->spriteSets, ObjectID);
	}

	// No such file exists, initialise with empty spriteset
	if (newSet == NULL)
	{
		newSet = createNewSpriteSet(&ObjectList->spriteSets, ObjectID);
	}

	return newSet;
} 


/* unused
Object* createNewObject(void)
{
	Object *newObject = calloc(1, sizeof(Object));

	if (newObject == NULL)
	{
		putConsoleString("\nError: Could not allocate memory for new object.\n");
		return NULL;
	}

	newObject->ObjectBox = createPhysicsBox(SOLID);

	if (newObject->ObjectBox == NULL)
	{
		putConsoleString("\nError: Could not allocate memory for new object's physics box.\n");
		free(newObject);
		return NULL;
	}

	newObject->ObjectDisplay = createDisplayData(DEFAULT_TO_SPRITE);

	if (newObject->ObjectDisplay == NULL)
	{
		putConsoleString("\nError: Could not allocate memory for new object's display data.\n");
		free(newObject->ObjectBox);
		free(newObject);
		return NULL;
	}

	newObject->Parent = NULL;
	newObject->nextObject = NULL;
	newObject->prevObject = NULL;
	newObject->State = EMPTY_OBJECT;

	return newObject;
}
*/

int setObjectName(Object *inputObject, const char name[])
{
	if (name == NULL || strlen(name) >= OBJECT_NAME_LENGTH || inputObject == NULL)
	{
		return MISSING_DATA;
	}

	strcpy(inputObject->name, name);

	return LEMON_SUCCESS;
}


Object* FindObject(const char name[], ObjectController *ObjectList)
{
	if (name == NULL || strlen(name) >= OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *currentObj = ObjectList->firstObject;

	while (currentObj != NULL && currentObj->ObjectBox != NULL)
	{
		if (strcmp(currentObj->name, name) == 0)
		{
			return currentObj;
		}

		currentObj = currentObj->nextObject;
	}

	return currentObj;
}


Object* FindObjectID(const char name[], int objectID, ObjectController ObjectList)
{
	if (name == NULL || strlen(name) >= OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *currentObj = ObjectList.firstObject;

	while (currentObj != NULL && currentObj->ObjectBox != NULL)
	{
		if (objectID == currentObj->ObjectID && strcmp(currentObj->name, name) == 0)
		{
			return currentObj;
		}

		currentObj = currentObj->nextObject;
	}

	return currentObj;
}


int snapPositionToTileGrid(Object *input, int xPos, int yPos)
{
	if (input == NULL || input->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}
	
	input->ObjectBox->xPos = (float)(xPos - (xPos % X_TILESCALE));
	input->ObjectBox->yPos = (float)(yPos - (yPos % Y_TILESCALE));
	
	return LEMON_SUCCESS;
}



PhysicsBox* createPhysicsBox(SolidType inputSolid)
{
	PhysicsBox *newRect = malloc(sizeof(PhysicsBox));

	if (newRect == NULL)
	{
		return NULL;
	}

	resetPhysicsBox(newRect);
	newRect->solid = inputSolid;

	return newRect;
}


int resetPhysicsBox(PhysicsBox *input)
{
	if (input == NULL)
	{
		return MISSING_DATA;
	}

	input->xPos = 0.0;
	input->yPos = 0.0;
	input->prevXPos = 0.0;
	input->prevYPos = 0.0;
	input->xSize = 0;
	input->ySize = 0;
	input->xFlip = 1;
	input->yFlip = 1;
	input->direction = DEFAULT_DIRECTION;

	input->solid = SOLID;
	input->flag = DEFAULT_SOLIDFLAG;
	input->collideLayer = MIDDLEGROUND;
	input->xVelocity = 0.0;
	input->yVelocity = 0.0;
	input->PhysicsXVelocity = 0.0;
	input->PhysicsYVelocity = 0.0;
	input->forwardVelocity = 0.0;
	input->crouch = false;
	input->inAir = 0;
	input->GroundBox = NULL;

	return LEMON_SUCCESS;
}


DisplayData* createDisplayData(RenderMode startRenderMode)
{
	DisplayData *newDisplay = malloc(sizeof(DisplayData));

	if (newDisplay == NULL)
	{
		return NULL;
	}

	resetDisplayData(newDisplay);
	newDisplay->RenderModeOverride = startRenderMode;

	return newDisplay;
}


int resetDisplayData(DisplayData *input)
{
	if (input == NULL)
	{
		return MISSING_DATA;
	}

	input->currentSprite = 1;
	input->spriteBuffer = NULL;
	input->spriteSetSource = NULL;
	input->RenderModeOverride = DEFAULT_TO_SPRITE;
	input->animationSpeed = 1.0;

	input->size = 1.0;
	input->rotateMode = NORMAL_ROTATION;
	input->spriteXOffset = 0.0;
	input->spriteYOffset = 0.0;
	input->pixelXOffset = 0;
	input->pixelYOffset = 0;

	input->currentAnimation = 0;
	input->frameBuffer = NULL;
	input->animationBuffer = NULL;
	input->animationTick = 0.0;
	input->animationLoopCount = 0;
	input->transparency = 0.0;
	input->hidden = false;

	return LEMON_SUCCESS;
}


DisplayData* getDisplay(Object *input)
{
	if (input == NULL)
	{
		return NULL;
	}

	return input->ObjectDisplay;
}

inline Layer getDisplayLayer(Object *input)
{
	if (input == NULL || input->ObjectDisplay == NULL)
	{
		return UNDEFINED_LAYER;
	}

	return input->ObjectDisplay->layer;
}

bool setDisplayLayer(Object *input, Layer newLayer)
{
	if (input == NULL || input->ObjectDisplay == NULL || input->ObjectDisplay == &EngineSettings.DefaultDisplay)
	{
		return false;
	}

	input->ObjectDisplay->layer = newLayer;

	return true;
}

RenderMode getRenderModeOverride(Object *input)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL)
	{
		return UNDEFINED_RENDERMODE;
	}

	return display->RenderModeOverride;
}

RenderMode getRenderMode(Object *input)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL)
	{
		return UNDEFINED_RENDERMODE;
	}

	if (display->spriteBuffer == NULL || display->RenderModeOverride != DEFAULT_TO_SPRITE)
	{
		return display->RenderModeOverride;
	}

	return display->spriteBuffer->RenderMode;
}

bool setRenderModeOverride(Object *input, RenderMode newMode)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL || display == &EngineSettings.DefaultDisplay)
	{
		return false;
	}

	display->RenderModeOverride = newMode;

	return true;
}

double getDisplayDirection(Object *input)
{
	if (input == NULL || input->ObjectDisplay == NULL)
	{
		return 0.0;
	}

	DisplayData *display = input->ObjectDisplay;

	if (display->currentAnimation != 0 && display->frameBuffer != NULL)
	{
		return input->ObjectBox->direction + display->frameBuffer->rotation;
	}

	return input->ObjectBox->direction;
}

bool setTransparency(Object *input, float transparency)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL || display == &EngineSettings.DefaultDisplay)
	{
		return false;
	}

	display->transparency = fClamp(transparency, 0.0, 1.0);

	return true;
}

bool changeTransparency(Object *input, float transparency)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL || display == &EngineSettings.DefaultDisplay)
	{
		return false;
	}

	display->transparency = fClamp(display->transparency + transparency, 0.0, 1.0);

	return true;
}

float getTransparency(Object *input)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL)
	{
		return -1.0;
	}

	return display->transparency;
}

bool hideObject(Object *input)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL || display == &EngineSettings.DefaultDisplay)
	{
		return false;
	}

	display->hidden = true;

	return true;
}

bool showObject(Object *input)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL || display == &EngineSettings.DefaultDisplay)
	{
		return false;
	}
	
	display->hidden = false;

	return true;
}

bool toggleHidden(Object *input)
{
	DisplayData *display = getDisplay(input);

	if (display == NULL || display == &EngineSettings.DefaultDisplay)
	{
		return false;
	}

	display->hidden = !display->hidden;

	return true;
}

bool setRotateMode(Object *input, RotationMode mode)
{
	DisplayData *display = getDisplay(input);

	if (display == NULL || display == &EngineSettings.DefaultDisplay)
	{
		return false;
	}

	display->rotateMode = mode;

	return true;
}


int getSubType(Object *input)
{
	if (input == NULL)
	{
		return -1;
	}

	switch (input->ObjectID)
	{
		case UI_ELEMENT:
		case UI_TEXT:
		case LEVEL_FLAG_OBJ:
			return input->arg1;

		default:
			return -1;
	}
}

int setSubType(Object *input, int subType)
{
	if (input == NULL)
	{
		return UNDEFINED_OBJECT;
	}

	if (getSubType(input) == -1)
	{
		return INVALID_DATA;
	}

	input->arg1 = subType;

	return LEMON_SUCCESS;
}


// These are convienience functions shorthand for calling the normal switchSprite functions without switching spriteset
int switchObjectSprite(int spriteID, Object *inputObject)
{
	DisplayData *ObjectDisplay = getDisplay(inputObject);

	if (ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	if (inputObject->ObjectID == LEVEL_FLAG_OBJ || (ObjectDisplay->spriteBuffer != NULL && ObjectDisplay->spriteBuffer->spriteID == spriteID) )
	{
		ObjectDisplay->currentSprite = spriteID;
		return EXECUTION_UNNECESSARY;
	}


	switchSprite(spriteID, USE_CURRENT_SPRITESET, ObjectDisplay);
		
	return LEMON_SUCCESS;
}

int switchObjectSpriteByName(const char spriteName[], Object *inputObject)
{
	DisplayData *ObjectDisplay = getDisplay(inputObject);

	if (ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	if (ObjectDisplay->spriteBuffer != NULL && strcmp(ObjectDisplay->spriteBuffer->name, spriteName) == 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	switchSpriteByName(spriteName, USE_CURRENT_SPRITESET, ObjectDisplay);

	return LEMON_SUCCESS;
}


Object* deleteObject(Object *input, ObjectController *ObjectList)
{
	if (input == NULL || ObjectList == NULL)
	{
		return NULL;
	}

	ObjectList->objectCount--;
	Object *prevObject = input->prevObject;
	Object *nextObject = input->nextObject;


	if (nextObject != NULL)
	{
		nextObject->prevObject = prevObject;
	}
	else
	{
		ObjectList->lastObject = prevObject;
	}

	if (prevObject != NULL)
	{
		prevObject->nextObject = nextObject;
	}
	else
	{
		ObjectList->firstObject = nextObject;
	}
	
	
	// delete associated data
	deleteAssociatedFrameFunction(input, ObjectList);
	removeComponents(input, ObjectList);
	

	if (ObjectList->availableSlots != NULL)
	{
		ObjectList->availableSlots->prevObject = input;
	}

	input->nextObject = ObjectList->availableSlots;
	ObjectList->availableSlots = input;
	input->State = EMPTY_OBJECT;

	if (DebugSettings.ConsoleTextEnabled == CONSOLE_ALL_EVENTS)
	{
		putConsoleStringTS("Deleted object ---- ID: %d (%s)  Name: '%s'", input->ObjectID, getObjectIDName(input->ObjectID), input->name);
	}

	return nextObject;
}

void deleteAssociatedFrameFunction(Object *input, ObjectController *ObjectList)
{
	if (input == NULL || ObjectList == NULL)
	{
		return;
	}

	FrameUpdateFunction *CurrentFunction = ObjectList->FrameUpdates;
	FrameUpdateFunction *DeleteFunction = NULL;
	FrameUpdateFunction *prevFunction = NULL;

	while (CurrentFunction != NULL)
	{
		if (CurrentFunction->inputObject == input)
		{
			DeleteFunction = CurrentFunction;
			CurrentFunction = CurrentFunction->nextFunction;

			if (prevFunction != NULL)
			{
				prevFunction->nextFunction = CurrentFunction;
			}
			else 
			{
				ObjectList->FrameUpdates = CurrentFunction;
			}

			free(DeleteFunction);
		}
		else
		{
			prevFunction = CurrentFunction;
			CurrentFunction = CurrentFunction->nextFunction;
		}
	}

	return;
}

void removeSceneActionReferences(Object *input, World *GameWorld)	// unused, as is unneccessary
{
	if (input == NULL || GameWorld == NULL || GameWorld->SceneActionQueue == NULL)
	{
		return;
	}

	SceneAction *currentAction = GameWorld->SceneActionQueue;
	while (currentAction != NULL)
	{
		if (currentAction->ActorObject == input)
		{
			currentAction->ActorObject = NULL;
		}

		currentAction = currentAction->nextSceneAction;
	}

	return;
}

void deleteAllObjects(ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return;
	}

	while (ObjectList->firstObject != NULL)
	{
		deleteObject(ObjectList->firstObject, ObjectList);
	}
	
	ObjectList->objectCount = 0;
	ObjectList->firstObject = NULL;
	ObjectList->lastObject = NULL;

	return;
}

void deleteAllEnvironmentObjects(ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return;
	}

	Object *currentObject = ObjectList->firstObject;
	while (currentObject != NULL)
	{
		switch(currentObject->ObjectID)
		{
		case UI_ELEMENT:
		case PLAYER_OBJECT:
		case UI_TEXT:
		case PARTICLE:
			currentObject = currentObject->nextObject;
			break;

		default:
			currentObject = deleteObject(currentObject, ObjectList);
			break;
		}
	}
	
	return;
}

void deleteLevelObjects(ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return;
	}

	Object *current = ObjectList->firstObject;
	Object *cache = NULL;
	while (current != NULL)
	{
		cache = current;
		current = current->nextObject;

		if ((cache->reserved & RFLAG_PRESERVE_OBJECT) == 0)
		{
			deleteObject(cache, ObjectList);
		}
		else if ((cache->reserved & RFLAG_PRESERVE_ONCE) == RFLAG_PRESERVE_ONCE)
		{
			cache->reserved &= (~RFLAG_PRESERVE_ONCE);	// remove 'RFLAG_PRESERVE_ONCE' and 'RFLAG_PRESERVE_OBJECT' flags at same time
		}
	}

	return;
}

int MarkObjectForDeletion(Object *inputObject)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	if (inputObject->State == TO_BE_DELETED)
	{
		return INVALID_DATA;
	}

	inputObject->State = TO_BE_DELETED;

	return LEMON_SUCCESS;
}


int UnmarkObjectForDeletion(Object *inputObject)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	if (inputObject->State != TO_BE_DELETED)
	{
		return INVALID_DATA;
	}

	inputObject->State = DEFAULT_STATE;


	if (DebugSettings.ConsoleTextEnabled == CONSOLE_ALL_EVENTS)
	{
		putConsoleString("Unmarked '%s' for deletion! ---- ID: %d (%s)", 
		inputObject->name, inputObject->ObjectID, getObjectIDName(inputObject->ObjectID));
	}


	return LEMON_SUCCESS;
}


// These functions can be safely called from anywhere, however due to the fact that they modify the order of the object list itself they may
// produce unintended behaviour. Eg: incrementing skips an object in the list, decrementing repeats the previous object, SetToBack 
// skips the rest of the objects in the list and essentially ends that frame's behaviour excecution and SetToFront repeats execution for all objects.
// This may or may not be significant, so when wanting to modify what is rendered above, it is recommended to simplify modify the layer variable
// BASICALLY, unless you know what your doing, avoid using these functions and just use the layer system
void IncrementDrawPriority(ObjectController *ObjectList, Object *input)
{
	if (ObjectList == NULL || input == NULL)
	{
		return;
	}

	Object *nextPtr = input->nextObject;

	Object *prevPtr = input->prevObject;

	if (nextPtr == NULL || ObjectList->lastObject == ObjectList->firstObject)
	{
		return;
	}
	
	if (prevPtr == NULL)
	{
		if (ObjectList->firstObject != input)
		{
			return;
		}

		ObjectList->firstObject = nextPtr;
	}
	else
	{
		prevPtr->nextObject = nextPtr;
	}

	if (nextPtr->nextObject == NULL)
	{
		if (ObjectList->lastObject != nextPtr)
		{
			prevPtr->nextObject = input;
			return;
		}

		ObjectList->lastObject = input;
	}
	
	input->nextObject = nextPtr->nextObject;
	input->prevObject = nextPtr;
	nextPtr->prevObject = prevPtr;
	nextPtr->nextObject = input;

	return;
}


void DecrementDrawPriority(ObjectController *ObjectList, Object *input)
{
	if (ObjectList == NULL || input == NULL)
	{
		return;
	}

	Object *nextPtr = input->nextObject;

	Object *prevPtr = input->prevObject;

	if (prevPtr == NULL || ObjectList->lastObject == ObjectList->firstObject)
	{
		return;
	}
	
	if (nextPtr == NULL)
	{
		if (ObjectList->lastObject != input)
		{
			return;
		}

		ObjectList->lastObject = prevPtr;
	}
	else
	{
		nextPtr->prevObject = prevPtr;
	}
	
	if (prevPtr->prevObject == NULL)
	{
		if (ObjectList->firstObject != prevPtr)
		{
			nextPtr->prevObject = input;
			return;
		}

		ObjectList->firstObject = input;
	}

	input->prevObject = prevPtr->prevObject;
	input->nextObject = prevPtr;
	prevPtr->nextObject = nextPtr;
	prevPtr->prevObject = input;

	return;
}


void SetDrawPriorityToFront(ObjectController *ObjectList, Object *input)
{
	if (ObjectList == NULL || input == NULL)
	{
		return;
	}

	if (input->nextObject == NULL)
	{
		return;
	}

	Object *nextPtr = input->nextObject;

	Object *prevPtr = input->prevObject;


	nextPtr->prevObject = prevPtr;
	
	if (prevPtr == NULL)
	{
		if (ObjectList->firstObject == input)
		{
			ObjectList->firstObject = nextPtr;
		}
		else
		{
			ObjectList->cachedFirstObject = nextPtr;
		}
	}
	else
	{
		prevPtr->nextObject = nextPtr;
	}
	
	if (ObjectList->lastObject != NULL)
	{
		ObjectList->lastObject->nextObject = input;
	}

	input->prevObject = ObjectList->lastObject;
	ObjectList->lastObject = input;
	input->nextObject = NULL;

	return;
}


void SetDrawPriorityToBack(ObjectController *ObjectList, Object *input)
{
	if (ObjectList == NULL || input == NULL)
	{
		return;
	}
	
	Object *nextPtr = input->nextObject;

	Object *prevPtr = input->prevObject;

	if (prevPtr == NULL)
	{
		return;
	}


	prevPtr->nextObject = nextPtr;
	
	if (nextPtr == NULL)
	{
		if (ObjectList->lastObject == input)
		{
			ObjectList->lastObject = prevPtr;
		}
		else
		{
			ObjectList->cachedLastObject = prevPtr;
		}
	}
	else
	{
		nextPtr->prevObject = prevPtr;
	}
	
	if (ObjectList->firstObject != NULL)
	{
		ObjectList->firstObject->prevObject = input;
	}

	input->nextObject = ObjectList->firstObject;
	ObjectList->firstObject = input;
	input->prevObject = NULL;

	return;
}



int getObjectID(char entry[])
{
	if (entry[0] >= '0' && entry[0] <= '9')
	{
		return convertStrToInt(entry, 6);
	}

	stringToLower(entry);

	if (strcmp(entry, "solidblock") == 0)
	{
		return SOLID_BLOCK;
	}
	else if (strcmp(entry, "flatslopefloor") == 0)
	{
		return FLAT_SLOPE_FLOOR;
	}
	else if (strcmp(entry, "jumpthrublock") == 0)
	{
		return JUMP_THRU_BLOCK;
	}
	else if (strcmp(entry, "coin") == 0)
	{
		return COIN;
	}
	else if (strcmp(entry, "playerobject") == 0)
	{
		return PLAYER_OBJECT;
	}
	else if (strcmp(entry, "uielement") == 0)
	{
		return UI_ELEMENT;
	}
	else if (strcmp(entry, "uitext") == 0)
	{
		return UI_TEXT;
	}
	else if (strcmp(entry, "particle") == 0)
	{
		return PARTICLE;
	}
	else if (strcmp(entry, "movingplatformhor") == 0)
	{
		return MOVING_PLATFORM_HOR;
	}
	else if (strcmp(entry, "movingplatformver") == 0)
	{
		return MOVING_PLATFORM_VER;
	}
	else if (strcmp(entry, "spring") == 0)
	{
		return SPRING;
	}
	else if (strcmp(entry, "gateswitch") == 0)
	{
		return GATE_SWITCH;
	}
	else if (strcmp(entry, "gateswitchtimed") == 0)
	{
		return GATE_SWITCH_TIMED;
	}
	else if (strcmp(entry, "verticalgate") == 0)
	{
		return VERTICAL_GATE;
	}
	else if (strcmp(entry, "horizontalgate") == 0)
	{
		return HORIZONTAL_GATE;
	}
	else if (strcmp(entry, "door") == 0)
	{
		return DOOR;
	}
	else if (strcmp(entry, "leveldoor") == 0)
	{
		return LEVEL_DOOR;
	}
	else if (strcmp(entry, "pushablebox") == 0)
	{
		return PUSHABLE_BOX;
	}
	else if (strcmp(entry, "basicenemy") == 0)
	{
		return BASIC_ENEMY;
	}	


	return UNDEFINED_OBJECT;
}

const char* getObjectIDName(ObjectType input)
{
	if (input >= OBJECT_TYPE_COUNT && input < 0)
	{
		return "Undefined";
	}

	switch(input)
	{
	case LEVEL_FLAG_OBJ:
		return "Level Flag";

	case SPRING:
		return "Spring";

	case PARTICLE:
		return "Particle";

	case UI_ELEMENT:
		return "UIElement";

	case UI_TEXT:
		return "UIText";

	case COIN:
		return "Coin";

	case PLAYER_OBJECT:
		return "PlayerObject";

	case SOLID_BLOCK:
		return "SolidBlock";

	case LEVEL_DOOR:
		return "LevelDoor";

	case DOOR:
		return "Door";

	case VERTICAL_GATE:
		return "VerticalGate";

	case HORIZONTAL_GATE:
		return "HorizontalGate";

	case GATE_SWITCH_TIMED:
		return "TimedGateSwitch";

	case MOVING_PLATFORM_HOR:
		return "MovingPlatform_Horizontal";

	case MOVING_PLATFORM_VER:
		return "MovingPlatform_Vertical";

	case GATE_SWITCH:
		return "GateSwitch";

	case PUSHABLE_BOX:
		return "PushableBox";

	case JUMP_THRU_BLOCK:
		return "JumpThroughPlatform";

	case FLAT_SLOPE_FLOOR:
		return "FlatSlopeFloor";

	default:
		static char name[MAX_LEN] = {0};
		snprintf(name, MAX_LEN, "%d", input);
		return name;
	}
}

const char* getObjectStateName(ObjectState input)
{
	switch (input)
	{
	case DEFAULT_STATE:
		return "Default State";

	case EMPTY_OBJECT:
		return "Empty Object";

	case TO_BE_DELETED:
		return "To be deleted";

	case STATIC_STATE:
		return "Static";

	case PAUSE_STATE:
		return "Paused behaviour";

	case ACTOR_STATE:
		return "Actor";

	case BEING_CARRIED_STATE:
		return "Being carried";

	default:
		return "Undefined State";
	}
}

const char* getSolidTypeName(SolidType input)
{
	switch(input)
	{
	case SOLID:
		return "Solid";

	case UNSOLID:
		return "Unsolid";

	case BODY:
		return "Body";

	case FLAT_SLOPE:
		return "Flat Slope";

	case JUMP_THROUGH:
		return "Jump-Through Solid";

	case PUSHABLE_SOLID:
		return "Pushable Solid";

	case CIRCLE:
		return "Solid Circle";

	default:
		return "Undefined";
	}
}

const char* getSolidFlagName(SolidFlag input)
{
	switch(input)
	{
	case ONLY_BODIES:
		return "Only bodies";

	case IGNORE_SOLID:
		return "Ignore solid";

	case IGNORE_SELF:
		return "Ignore self";

	case GET_IGNORED:
		return "Get ignored";

	case IMPACT_COLLISION:
		return "Only Impact collision";

	case PUSH_COLLISION:
		return "Only Push collision";

	case DEFAULT_SOLIDFLAG:
		return "No flag";

	default:
		return "Undefined";
	}	

}


const char* getLayerName(Layer input)
{
	switch(input)
	{
	case BACKGROUND:
		return "Background";

	case MIDDLEGROUND:
		return "Middleground";

	case MIDDLEGROUND_2:
		return "Middleground 2";

	case FOREGROUND:
		return "Foreground";

	case HUD:
		return "HUD layer";

	case FRONT_LAYER:
		return "Front layer";

	case PARTICLES:
		return "Particle layer";

	default:
		return "Undefined";
	}
}



int moveObjectToCachedList(ObjectController *ObjectList, Object *inputObject)
{
	if (ObjectList == NULL || ObjectList->firstObject == NULL || inputObject == NULL)
	{
		return MISSING_DATA;
	}

	SetDrawPriorityToFront(ObjectList, inputObject);

	if (inputObject->prevObject != NULL)
	{
		inputObject->prevObject->nextObject = NULL;
		ObjectList->lastObject = inputObject->prevObject;
		ObjectList->objectCount--;
	}
	else
	{
		ObjectList->firstObject = NULL;
		ObjectList->lastObject = NULL;
		ObjectList->objectCount = 0;
	}


	inputObject->prevObject = ObjectList->cachedLastObject;

	if (ObjectList->cachedFirstObject == NULL)
	{
		ObjectList->cachedFirstObject = inputObject;
		ObjectList->cachedCount = 1;
	}
	else
	{
		ObjectList->cachedLastObject->nextObject = inputObject;
		ObjectList->cachedCount++;
	}

	ObjectList->cachedLastObject = inputObject;

	return LEMON_SUCCESS;
}

int swapMainAndCachedLists(ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	Object *tempFirst = ObjectList->firstObject;
	Object *tempLast = ObjectList->lastObject;
	int tempCount = ObjectList->objectCount;

	ObjectList->firstObject = ObjectList->cachedFirstObject;
	ObjectList->lastObject = ObjectList->cachedLastObject;
	ObjectList->objectCount = ObjectList->cachedCount;

	ObjectList->cachedFirstObject = tempFirst;
	ObjectList->cachedLastObject = tempLast;
	ObjectList->cachedCount = tempCount;


	return LEMON_SUCCESS;
}

int restoreAllCachedObjects(ObjectController *ObjectList)
{
	if (ObjectList == NULL || ObjectList->cachedFirstObject == NULL)
	{
		return MISSING_DATA;
	}

	ObjectList->cachedFirstObject->prevObject = ObjectList->lastObject;
	if (ObjectList->lastObject != NULL)
	{
		ObjectList->lastObject->nextObject = ObjectList->cachedFirstObject;
	}
	else	
	{
		ObjectList->firstObject = ObjectList->cachedFirstObject;
	}

	ObjectList->lastObject = ObjectList->cachedLastObject;
	ObjectList->objectCount += ObjectList->cachedCount;

	ObjectList->cachedFirstObject = NULL;
	ObjectList->cachedLastObject = NULL;
	ObjectList->cachedCount = 0;


	return LEMON_SUCCESS;
}

int deleteAllCachedObjects(ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (ObjectList->cachedFirstObject == NULL)
	{
		return EXECUTION_UNNECESSARY;
	}

	swapMainAndCachedLists(ObjectList);

	deleteAllObjects(ObjectList);

	swapMainAndCachedLists(ObjectList);


	return LEMON_SUCCESS;
}


int cacheObjects(ObjectController *ObjectList, PhysicsBox boundingBox)
{	
	if (ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	Object *currentObject = ObjectList->firstObject;

	while (currentObject != NULL)
	{
		Object *temp = currentObject;

		currentObject = currentObject->nextObject;

		moveObjectToCachedList(ObjectList, temp);
	}

	swapMainAndCachedLists(ObjectList);


	currentObject = ObjectList->firstObject;

	while (currentObject != NULL)
	{
		Object *temp = currentObject;

		currentObject = currentObject->nextObject;

		if (temp->ObjectBox == NULL || temp->ObjectID == UI_ELEMENT || temp->ObjectID == LEVEL_FLAG_OBJ)
		{
			continue;
		}

		if (checkBoxOverlapsBoxBroad(&boundingBox, temp->ObjectBox) == false)
		{
			moveObjectToCachedList(ObjectList, temp);
		}
	}


	return LEMON_SUCCESS;
}



// Updates all objects in GameWorld
FuncResult updateObjects(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState <= EMPTY_GAME || GameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}

	ObjectController *ObjectList = GameWorld->ObjectList;
	currentComponents = &ObjectList->objectComponents;

	if (ObjectList == NULL || ObjectList->firstObject == NULL)
	{
		return MISSING_DATA;
	}

	updatePreviousPositions(ObjectList);

	updateComponents(currentComponents, GameWorld);

	Object *currentObject = ObjectList->firstObject;
	while(currentObject != NULL)
	{
		ObjectBehaviour(GameWorld, currentObject);
		
		currentObject = currentObject->nextObject;
	}

	// Update object state - parent-child links, deletion, etc.
	updateObjectsState(ObjectList, GameWorld);

	ResolveAllObjects(ObjectList, GameWorld->PhysicsType);


	return LEMON_SUCCESS;
}


#define cannotUpdateObject(x) (x->State == STATIC_STATE || x->State < DEFAULT_STATE || (GameWorld->GamePaused != 0 && x->ObjectID != UI_ELEMENT))

int ObjectBehaviour(World *GameWorld, Object *inputObject)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	inputObject->reserved |= RFLAG_DISABLE_PHYSICS;

	if (cannotUpdateObject(inputObject))
	{
		return EXECUTION_UNNECESSARY;
	}

	bool gameStateDisable = inputObject->State == ACTOR_STATE || (GameWorld->GameState == CUTSCENE && inputObject != GameWorld->Player.PlayerPtr); 
	bool immuneObject = (inputObject->reserved & RFLAG_CUTSCENE_IMMUNITY) != 0;

	if ((!immuneObject && gameStateDisable) || inputObject->State == PAUSE_STATE)
	{
		return ACTION_DISABLED;
	}

	inputObject->reserved ^= RFLAG_DISABLE_PHYSICS;
	

	#ifndef LEMON_USE_CUSTOM_CALLBACKS
	switch (inputObject->ObjectID)
	{
		case UI_ELEMENT:
			UpdateUIElement(GameWorld, inputObject);
			break;


		case UI_TEXT:
			UpdateUIText(GameWorld, inputObject);
			break;


		case PARTICLE:
			UpdateParticle(GameWorld, inputObject);
			break;


		case LEVEL_FLAG_OBJ:
			UpdateFlagObject(inputObject, &GameWorld->Player, GameWorld);
			break;


		case MOVING_PLATFORM_HOR:
			UpdateHorizontalPlatform(inputObject);
		 	break;


		case MOVING_PLATFORM_VER:
			UpdateVerticalPlatform(inputObject);
			break;


		case COIN:
		{
			UpdateCoin(inputObject, GameWorld);
		} break;


		case SPRING:
		{
			UpdateSpring(inputObject, GameWorld);
		} break;


		case VERTICAL_GATE:
			UpdateVerticalGate(inputObject, GameWorld);
			break;


		case HORIZONTAL_GATE:
			UpdateHorizontalGate(inputObject, GameWorld);
			break;


		case GATE_SWITCH_TIMED:
		case GATE_SWITCH:
			UpdateGateSwitch(GameWorld->Player, inputObject, GameWorld->ObjectList);
		break;


		case DOOR:
			UpdateDoor(GameWorld->Player, inputObject, GameWorld);
			break;


		case LEVEL_DOOR:
			UpdateLevelDoor(GameWorld->Player, inputObject, GameWorld);
			break;


		case BASIC_ENEMY:
			ApplyFriction(inputObject->ObjectBox, 1.0, 1.0, 1.0);
			break;


		case PUSHABLE_BOX:
			ApplyFriction(inputObject->ObjectBox, 0.9, 0.9, 1.0);

			PhysicsBox *PlayerBox = GameWorld->Player.PlayerBox;

			if (PlayerInteractingWithBox(GameWorld->Player, inputObject->ObjectBox) && PlayerBox != NULL)
			{
				if (PlayerBox->xPos > inputObject->ObjectBox->xPos + (inputObject->ObjectBox->xSize >> 1))
				{
					inputObject->ObjectBox->forwardVelocity = -15.0;
				}
				else
				{
					inputObject->ObjectBox->forwardVelocity = 15.0;
				}

				inputObject->ObjectBox->forwardVelocity += PlayerBox->forwardVelocity;
			}
			break;


		case PLAYER_OBJECT:
			UpdatePlayer(&GameWorld->Player, GameWorld);
			break;


		case PROJECTILE:
			{
				inputObject->arg1++;

				if (inputObject->arg1 > 200)
				{
					MarkObjectForDeletion(inputObject);
				}

				inputObject->ObjectBox->solid = SOLID;
				Object *hitTarget = GetCollidingObject(inputObject->ObjectBox, GameWorld->ObjectList);
				inputObject->ObjectBox->solid = UNSOLID;

				if (hitTarget == NULL || hitTarget == GameWorld->Player.PlayerPtr)
				{
					break;
				}	

				MarkObjectForDeletion(inputObject);

				centerOnObject(AddParticle(GameWorld, SPARKLE, 0, 0, 1, 0), inputObject);

				inflictDamage(40, hitTarget);
			}
			break;


		default:
			break;
	}
	#else
	UpdateObject(inputObject, GameWorld);
	#endif


	MoveObject(inputObject, GameWorld);


	return LEMON_SUCCESS;
}


int updateObjectDisplays(World *GameWorld)
{
	Object *currentObject = GameWorld->ObjectList->firstObject;

	while(currentObject != NULL)
	{
		if (!cannotUpdateObject(currentObject))
		{
			UpdateObjectDisplay(currentObject, deltaTime);
		}
		
		currentObject = currentObject->nextObject;
	}

	return LEMON_SUCCESS;
}

int UpdateObjectDisplay(Object *inputObject, float deltaTime)
{
	DisplayData *inputDisplay = getDisplay(inputObject);

	if (inputDisplay == NULL)	{ return MISSING_DATA; }

	if (inputObject->State < DEFAULT_STATE)
	{
		return ACTION_DISABLED;
	}

	
	iterateAnimation(inputDisplay, deltaTime);

	// Assign Sprite   
	if (inputDisplay->currentSprite > 0 && (inputDisplay->spriteBuffer == NULL || inputDisplay->currentSprite != inputDisplay->spriteBuffer->spriteID))
	{
		switchSprite(inputDisplay->currentSprite, USE_CURRENT_SPRITESET, inputDisplay);
	}

	return LEMON_SUCCESS;
}


int updatePreviousPositions(ObjectController *ObjectList)
{
	PhysicsBox *boxList = ObjectList->objectComponents.PhysicsBoxes;
	Object *objList = ObjectList->objectComponents.Objects;

	ObjectList->solidList.storedElements = 0;

	int i = 0;
	int k = 0;
	while (i < EngineSettings.MaxObjects && k < ObjectList->objectCount)
	{
		if (objList[i].State != EMPTY_OBJECT)
		{
			k++;

			boxList[i].prevXPos = boxList[i].xPos;
			boxList[i].prevYPos = boxList[i].yPos;

			boxList[i].GroundBox = NULL;
			objList[i].ParentLink &= PARENTLINK_MASK;

			if (boxList[i].solid != UNSOLID)
			{
				stackAdd(i, &ObjectList->solidList);
			}
		}

		i++;
	}

	return LEMON_SUCCESS;
}

int updatePreviousPosition(Object *input)
{
	input->ObjectBox->prevXPos = input->ObjectBox->xPos;
	input->ObjectBox->prevYPos = input->ObjectBox->yPos;

	input->ObjectBox->GroundBox = NULL;

	input->ParentLink &= PARENTLINK_CONFIRM;

	return LEMON_SUCCESS;
}


int updateObjectsState(ObjectController *ObjectList, World *GameWorld)
{
	//Object *Objects = ObjectList->objectComponents.Objects;
	//int i = EngineSettings.MaxObjects;
	Object *current = ObjectList->firstObject;
	int k = 0;
	while (current != NULL && k < ObjectList->objectCount)
	{	
		// i--;
		// if (Objects[i].State == EMPTY_OBJECT)
		// {
		// 	continue;
		// }

		UpdateParentChildLink(current);

		if (current->State == TO_BE_DELETED)
		{
			if (PLAYER_OBJECT == current->ObjectID)
			{
				PlayerObjectAboutToBeDeleted(&GameWorld->Player);
			}

			current = deleteObject(current, ObjectList);
		}
		else
		{
			UpdatePhysicsState(current, GameWorld);

			k++;

			current = current->nextObject;
		}
	}

	return LEMON_SUCCESS;
}


int UpdatePhysicsState(Object *inputObject, World *GameWorld)
{
	if (!LEMON_COLLISION_PHYSICS || inputObject->State == STATIC_STATE || inputObject->State < DEFAULT_STATE || !HasPhysics(inputObject))
	{
		return EXECUTION_UNNECESSARY;
	}

	PhysicsBox *inputBox = inputObject->ObjectBox;

	if (inputBox == NULL || GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsBox *GroundBox = inputBox->GroundBox;

	applyMagnetisation(inputBox, GroundBox, GameWorld);

	inputBox->xPos += inputBox->PhysicsXVelocity;
	inputBox->yPos += inputBox->PhysicsYVelocity;
	

	return LEMON_SUCCESS;
}


int applyMagnetisation(PhysicsBox *inputBox, PhysicsBox *GroundBox, World *GameWorld)
{
	if (GroundBox == NULL) 
	{
		return ACTION_DISABLED;
	}

	if (GroundBox == inputBox)
	{
		inputBox->GroundBox = NULL;
		
		return EXECUTION_UNNECESSARY;
	}
	
	// Ensure that velocity applied is not necessary in the case of it moving against gravity
	float prevX = inputBox->xPos;
	float prevY = inputBox->yPos;

	float xChange = (GroundBox->xPos) - (GroundBox->prevXPos);
	float yChange = (GroundBox->yPos) - (GroundBox->prevYPos);

	inputBox->xPos -= xChange;
	inputBox->yPos -= yChange;

	if (!CheckBoxCollidesBox(inputBox, GroundBox))
	{	
		inputBox->PhysicsXVelocity = xChange;
		inputBox->PhysicsYVelocity = yChange;
	}
	
	if (fabs(inputBox->PhysicsXVelocity) < 0.0001)
	{
		inputBox->PhysicsXVelocity = 0.0;
	}

	if (fabs(inputBox->PhysicsYVelocity) < 0.0001)
	{
		inputBox->PhysicsYVelocity = 0.0;
	}
	

	inputBox->xPos = prevX;
	inputBox->yPos = prevY;

	return LEMON_SUCCESS;
}


int UpdateParentChildLink(Object *inputObject)
{
	if (inputObject->Parent == NULL || (inputObject->ParentLink & PARENTLINK_CONFIRM))
	{
		return EXECUTION_UNNECESSARY;
	}

	inputObject->ParentLink |= PARENTLINK_CONFIRM;

	Object *parent = inputObject->Parent;

	// If the parent is marked as a 'final link' then it should have no children; remove the parent-child link in this case
	if ((parent->ParentLink & FINAL_LINK) != 0)
	{
		inputObject->Parent = NULL;
		return ACTION_DISABLED;
	}

	// recursively update the parent so that position values, animations, etc are correct up to the grandest parent
	UpdateParentChildLink(parent);

	// mark this object for deletion if its parent is deleted or is marked for deletion as well 
	if (parent->State < 0)
	{
		MarkObjectForDeletion(inputObject);
		return ACTION_DISABLED;
	}
	
	PhysicsBox *InputBox = inputObject->ObjectBox;
	PhysicsBox *ParentBox = parent->ObjectBox;

	if (ParentBox == NULL)
	{
		return MISSING_DATA;
	}

	if ((inputObject->ParentLink & MOTION_LINK) != 0)
	{
		InputBox->xPos += floor(ParentBox->xPos - ParentBox->prevXPos);
		InputBox->yPos += floor(ParentBox->yPos - ParentBox->prevYPos);
	}
	else if ((inputObject->ParentLink & POSITION_LINK) != 0)
	{
		centerOnObject(inputObject, parent);
		InputBox->xPos += InputBox->xVelocity;
		InputBox->yPos += InputBox->yVelocity;
		InputBox->forwardVelocity = 0.0;
	}

	if ((inputObject->ParentLink & PHYSICSBOX_LINK) != 0)
	{
		InputBox->collideLayer = ParentBox->collideLayer;
	}

	DisplayData *InputDisplay = getDisplay(inputObject);
	DisplayData *ParentDisplay = getDisplay(parent);

	if (ParentDisplay == NULL || InputDisplay == NULL)
	{
		return MISSING_DATA;
	}

	if ((inputObject->ParentLink & SPRITE_LINK) != 0)
	{
		InputDisplay->spriteBuffer = ParentDisplay->spriteBuffer;
		InputDisplay->currentSprite = ParentDisplay->currentSprite;
	}


	if ((inputObject->ParentLink & ANIMATION_LINK) != 0)
	{
		InputDisplay->animationBuffer = ParentDisplay->animationBuffer;
		InputDisplay->frameBuffer = ParentDisplay->frameBuffer;
		InputDisplay->currentAnimation = ParentDisplay->currentAnimation;
		InputDisplay->animationTick = ParentDisplay->animationTick;
		InputDisplay->animationLoopCount = ParentDisplay->animationLoopCount;
	}

	if ((inputObject->ParentLink & TRANSPARENCY_LINK) != 0)
	{
		InputDisplay->transparency = ParentDisplay->transparency;
	}

	if ( (inputObject->ParentLink & DISPLAYDATA_LINK) != 0)
	{
		InputDisplay->RenderModeOverride = ParentDisplay->RenderModeOverride;
		InputDisplay->hidden = ParentDisplay->hidden;
		InputDisplay->size = ParentDisplay->size;
		InputDisplay->layer = ParentDisplay->layer;
	}

	return LEMON_SUCCESS;
}


int ResolveAllObjects(ObjectController *ObjectList, WorldPhysics pType)
{
	if (!LEMON_COLLISION_PHYSICS || ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (pType != PLATFORMER)
	{
		return ACTION_DISABLED;
	}

	SparseList *List = &ObjectList->objectComponents.PhysicsComponent;
	ComponentType *physList = List->dense;
	PhysicsBox *boxList = ObjectList->objectComponents.PhysicsBoxes;

	int i = List->storedComponents;
	int boxIndex;
	Object *physObj;

	while (i > 0)
	{
		i--;

		physObj = physList[i].PhysicsComponent.object;

		if (physObj == NULL && physObj->index > -1 && physObj->State != EMPTY_OBJECT)
		{
			continue;
		}

		boxIndex = physObj->index;

		if (fabs(boxList[boxIndex].PhysicsXVelocity) > 0.1)
		{
			float savedPos = boxList[boxIndex].yPos;
			boxList[boxIndex].yPos -= boxList[boxIndex].PhysicsYVelocity;
			ResolveAllXCollision(&boxList[boxIndex], ObjectList);
			boxList[boxIndex].yPos = savedPos;
		}

		if (fabs(boxList[boxIndex].PhysicsYVelocity) > 0.1)
		{	
			ResolveAllYCollision(&boxList[boxIndex], ObjectList);
		}
	}

	return LEMON_SUCCESS;
}


// **READ THIS Before adding new components**
// Because C does not have templates, you must write some boilerplate before adding a new component
// This involves creating the struct for the component itself, and the wrapper to contain an array of them alongside the SparseSet
// you must also create the associated add/remove/get functions, and put the initialisation into the initialiseComponents function
// it's recommended to basically just copy and paste as it should copy the functionality of the existing components

// These macros can simplify the process of adding new components
#define initComponentType(x) 		initialiseSparseList(&currentComponents->x, #x)
#define removeComponentType(x, y) 	removeComponent(x, &currentComponents->y)
#define addComponentType(x, y) 		(y *)addComponent(x, &currentComponents->y)
#define getComponentType(x, y) 		(y *)getComponent(x, &currentComponents->y)


int initialiseComponents(ObjectController *input)
{
	if (input == NULL)
	{
		return MISSING_DATA;
	}

	currentComponents = &input->objectComponents;

	// initialise new components here
	initComponentType(HealthComponent);
	initComponentType(TileMap);
	initComponentType(Timer);
	initComponentType(StopWatch);
	initComponentType(PhysicsComponent);
	initComponentType(Polygon);

	return LEMON_SUCCESS;
}

int removeComponents(Object *input, ObjectController *ObjectList)
{
	if (!ObjectList)
	{
		return MISSING_DATA;
	}

	currentComponents = &ObjectList->objectComponents;

	// remove new components here
	removeComponentType(input, HealthComponent);
	removeComponentType(input, TileMap);
	removeComponentType(input, Timer);
	removeComponentType(input, StopWatch);
	removeComponentType(input, PhysicsComponent);
	removeComponentType(input, Polygon);

	return LEMON_SUCCESS;
}


void initialiseSparseList(SparseList *input, const char name[])
{
	strcpy(input->componentName, name);
	putConsoleString("Initialising %s...", name);
	int *sparse = input->sparse;

	// -1 is tombstone value (empty slot)
	for (int i = 0; i < EngineSettings.MaxObjects; i++)
	{
		sparse[i] = -1;
	}

	input->storedComponents = 0;

	memset(input->dense, 0, sizeof(ComponentType) * MAX_COMPONENT_SLOTS);
	memset(input->denseID, 0, sizeof(int) * MAX_COMPONENT_SLOTS);

	return;
}


ComponentType* addComponent(Object *input, SparseList *List)
{
	if (input == NULL || List == NULL)
	{
		return NULL;
	}

	int denseIndex = List->sparse[input->index];
	if (denseIndex >= 0)
	{
		return &List->dense[denseIndex];
	}

	if (List->storedComponents >= MAX_COMPONENT_SLOTS)
	{
		return NULL;
	}

	ComponentType *newSlot = &List->dense[List->storedComponents];
	List->sparse[input->index] = List->storedComponents;
	List->denseID[List->storedComponents] = input->index;
	List->storedComponents++;

	return newSlot;
}

int removeComponent(Object *input, SparseList *List)
{
	if (input == NULL)
	{
		return INVALID_DATA;
	}

	int denseIndex = List->sparse[input->index];
	if (denseIndex < 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	// set to -1 to indicate its empty, deletion of data is optional
	ComponentType *denseList = List->dense;
	int lastIndex = List->storedComponents - 1;

	if (strcmp(List->componentName, "Polygon") == 0)
	{
		Polygon *poly = &denseList[denseIndex].Polygon;
		
		if (poly->vertexList != NULL)
		{
			free(poly->vertexList);
			poly->vertexList = NULL;
		}

		if (poly->indicies != NULL)
		{
			free(poly->indicies);
			poly->indicies = NULL;
		}
	}

	// swap last and component to delete
	if (denseIndex != lastIndex)
	{
		List->sparse[List->denseID[lastIndex]] = denseIndex;
		denseList[denseIndex] = denseList[lastIndex];
		List->denseID[denseIndex] = List->denseID[lastIndex];
	}

	List->sparse[input->index] = -1;
	List->storedComponents--;

	return LEMON_SUCCESS;
}

ComponentType* getComponentWithIndex(int index, SparseList *List)
{
	if (index < 0 || List->sparse[index] < 0)
	{
		return NULL;
	}

	return &List->dense[List->sparse[index]];
}

ComponentType* getComponent(Object *input, SparseList *List)
{
	if (input == NULL || List->sparse[input->index] < 0)
	{
		return NULL;
	}

	return &List->dense[List->sparse[input->index]];
}


int updateComponents(ComponentData *data, World *GameWorld)
{
	updatePhysicsComponents(data, GameWorld);

	return LEMON_SUCCESS;
}


PhysicsComponent* addPhysics(Object *input, bool gravity)
{
	PhysicsComponent *newPhys = addComponentType(input, PhysicsComponent);

	if (newPhys == NULL)
	{
		return NULL;
	}

	newPhys->object = input;
	newPhys->gravity = gravity;

	return newPhys;
}

PhysicsComponent* addPhysicsDefault(Object *input)
{
	PhysicsComponent *newPhys = addComponentType(input, PhysicsComponent);
	if (newPhys == NULL)
	{
		return NULL;
	}

	newPhys->object = input;
	newPhys->gravity = true;

	return newPhys;
}

PhysicsComponent* getPhysicsComponent(Object *input)
{
	return getComponentType(input, PhysicsComponent);
}

bool HasPhysics(Object *input)
{
	if (input == NULL)
	{
		return false;
	}

	return (currentComponents->PhysicsComponent.sparse[input->index] >= 0);
}

bool HasGravity(Object *input)
{
	PhysicsComponent *myPhys = getComponentType(input, PhysicsComponent);

	if (myPhys)
	{
		return myPhys->gravity;
	}

	return false;
}

void updatePhysicsComponents(ComponentData *data, World *GameWorld)
{
	if (!LEMON_COLLISION_PHYSICS || GameWorld->PhysicsType != PLATFORMER)
	{
		return;
	}

	SparseList *List = &data->PhysicsComponent;
	ComponentType *denseList = List->dense;

	for (int i = List->storedComponents - 1; i >= 0; i--)
	{
		if (denseList[i].PhysicsComponent.gravity)
		{
			ApplyGravity(denseList[i].PhysicsComponent.object, GameWorld);
		}
	}
}


Polygon* addPolygon(Object *input, int numOfVertices, ...)
{
	Polygon *newPolygon = getComponentType(input, Polygon);

	if (newPolygon != NULL)		// necessary because vertex list is allocated on heap
	{
		free(newPolygon->vertexList);
		newPolygon->vertexList = NULL;
	}
	else
	{
		newPolygon = addComponentType(input, Polygon);

		if (newPolygon == NULL)
		{
			return NULL;
		}
	}

	SDL_Vertex *vertexList = malloc(sizeof(SDL_Vertex) * numOfVertices);
	if (vertexList == NULL)
	{
		return NULL;
	}
	memset(vertexList, 0, sizeof(SDL_Vertex) * numOfVertices);

	va_list args;
	va_start(args, numOfVertices);

	for (int i = 0; i < numOfVertices; i++)
	{
		vertexList[i].position.x = (float)va_arg(args, double);
		vertexList[i].position.y = (float)va_arg(args, double);
		vertexList[i].tex_coord.x = (float)va_arg(args, double);
		vertexList[i].tex_coord.y = (float)va_arg(args, double);
		vertexList[i].color.r = vertexList[i].color.b = vertexList[i].color.g = vertexList[i].color.a = 1.0;
	}

	va_end(args);

	newPolygon->vertexList = vertexList;
	newPolygon->vertices = numOfVertices;
	newPolygon->quad = false;
	newPolygon->indicies = NULL;

	return newPolygon;
}

Polygon* addQuad(Object *input)		
{
	float x = (float)input->ObjectBox->xSize;
	float y = (float)input->ObjectBox->ySize;
	// add a polygon that is a box surrounding the sprite, that without modification appears identically to regular sprite rendering, albeit without rotations
	Polygon *new = addPolygon(input, 4, 
		0.0, 0.0, 0.0, 0.0,	
		0.0, y, 0.0, 1.0,
		x, 0.0, 1.0, 0.0,
		x, y, 1.0, 1.0);

	if (new != NULL)
	{
		new->quad = true;

		if (new->indicies != NULL)
		{
			free(new->indicies);
		}

		new->indicies = malloc(6 * sizeof(int));
		new->indicies[0] = 0;
		new->indicies[1] = 1;
		new->indicies[2] = 2;
		new->indicies[3] = 1;
		new->indicies[4] = 2;
		new->indicies[5] = 3;
	}

	return new;
}


Polygon* getPolygon(Object *input)
{
	return getComponentType(input, Polygon);
}

void movePolygonVertex(Object *input, int vertex, float newX, float newY)
{
	Polygon *poly = getPolygon(input);

	if (poly == NULL || vertex < 0 || vertex >= poly->vertices)
	{
		return;
	}

	// kinda messy but allows you to treat the quad as a polygon with 4 points
	if (poly->quad)
	{
		if (vertex > 3)
		{
			return;
		}

		switch(vertex)
		{
		case 1:
			poly->vertexList[3].position.x = newX;
			poly->vertexList[3].position.y = newY;
			break;

		case 2:
			poly->vertexList[4].position.x = newX;
			poly->vertexList[4].position.y = newY;
			break;

		case 3:
			vertex = 5;
			break;

		default:
			break;
		}
	}

	poly->vertexList[vertex].position.x = newX;
	poly->vertexList[vertex].position.y = newY;

	return;
}

SDL_Vertex* getPolygonVertex(Object *input, int vertex)
{
	Polygon *poly = getPolygon(input);

	if (poly == NULL || vertex < 0 || vertex >= poly->vertices)
	{
		return NULL;
	}

	return &poly->vertexList[vertex];
}


int addHealthComponent(Object *input, int Health)
{
	HealthComponent *newHp = addComponentType(input, HealthComponent);

	if (newHp == NULL)
	{
		return MISSING_DATA;
	}

	newHp->health = Health;
	newHp->maxHealth = Health;
	newHp->duration = 0;
	newHp->startTick = 0;
	newHp->inflictedDamage = 0;

	return LEMON_SUCCESS;
}

int removeHealthComponent(Object *input)
{
	return removeComponentType(input, HealthComponent);
}

HealthComponent* getHealthComponent(Object *input)
{
	return getComponentType(input, HealthComponent);
}

int inflictDamage(int damage, Object *input)
{
	HealthComponent *targetHp = getHealthComponent(input);

	if (targetHp != NULL)
	{
		targetHp->health -= abs(damage);
		if (targetHp->health < 1)
		{
			MarkObjectForDeletion(input);
		}

		targetHp->startTick = TickNumber();
		targetHp->duration = 5;
		PlayObjectAnimation("Hurt", 1, input);
		targetHp->inflictedDamage = damage;
	}

	return LEMON_SUCCESS;
}

bool isHurt(Object *input)
{
	HealthComponent *health = getHealthComponent(input);

	if (health == NULL)
	{		
		return false;
	}

	return (TickNumber() < health->startTick + health->duration);
}


int addTileMap(Object *input, unsigned int centerTileX, unsigned int centerTileY, unsigned int tileSize)
{
	TileMap *newMap = addComponentType(input, TileMap);

	if (newMap == NULL)
	{
		return MISSING_DATA;
	}

	newMap->centerTileX = centerTileX;
	newMap->centerTileY = centerTileY;
	newMap->tileSize = clamp(tileSize, 1, tileSize);

	return LEMON_SUCCESS;
}


TileMap* getTileMap(Object *input)
{
	return getComponentType(input, TileMap);
}


int startTimer(int ticks, Object *input)
{
	if (ticks < 1)
	{
		return EXECUTION_UNNECESSARY;
	}

	Timer *newTimer = addComponentType(input, Timer);

	if (newTimer == NULL)
	{
		return MISSING_DATA;
	}

	newTimer->pause = false;
	newTimer->pauseTick = 0;
	newTimer->timerLength = ticks;
	newTimer->startTick = TickNumber();

	return LEMON_SUCCESS;
}

int startTimerSeconds(float seconds, Object *input)
{
	return startTimer((int)(seconds * EngineSettings.GameTicksPerSecond), input);
}

bool timerExpired(Object *input)
{
	const Timer *timer = getComponentType(input, Timer);

	if (timer == NULL)
	{
		return true;
	}

	Uint64 current = timer->pause ? timer->pauseTick : TickNumber();

	if ((current - timer->startTick) >= timer->timerLength)
	{
		removeComponentType(input, Timer);
		return true;
	}

	return false;
}

Timer* getTimer(Object *input)
{
	return getComponentType(input, Timer);
}

int endTimer(Object *input)
{
	return removeComponentType(input, Timer);
}

void pauseTimer(Object *input)
{
	Timer *timer = getComponentType(input, Timer);

	if (timer == NULL || timer->pause)
	{
		return;
	}

	timer->pause = true;
	timer->pauseTick = TickNumber();

	return;
}

#define resumeTimer(x) unpauseTimer(x)
void unpauseTimer(Object *input)
{
	Timer *timer = getComponentType(input, Timer);

	if (timer == NULL || !timer->pause)
	{
		return;
	}

	timer->pause = false;
	timer->startTick += TickNumber() - timer->pauseTick;

	return;
}


// unused
void pauseTimers(ComponentData *data, World *GameWorld)
{
	if (GameWorld->GamePaused == 0)
	{
		return;
	}

	SparseList *List = &data->Timer;
	Object *objects = data->Objects;
	ComponentType *denseList = List->dense;

	for (int i = 0; i < List->storedComponents; i++)
	{
		if (cannotUpdateObject((&objects[List->denseID[i]])))
		{
			denseList[i].Timer.startTick++;
		}
	}	
}


int startStopWatch(Object *input)
{
	StopWatch *newStopWatch = addComponentType(input, StopWatch);

	if (newStopWatch == NULL)
	{
		return MISSING_DATA;
	}

	newStopWatch->startTimeStamp = SDL_GetTicks();
	newStopWatch->pause = false;
	newStopWatch->pauseTimeStamp = 0;

	return LEMON_SUCCESS;
}

float checkStopWatch(Object *input)
{
	const StopWatch *watch = getComponentType(input, StopWatch);

	if (watch == NULL)
	{
		return 0.0;
	}

	if (watch->pause)
	{
		return (float)(watch->pauseTimeStamp - watch->startTimeStamp) / 1000.0;
	}
	else
	{
		return (float)(SDL_GetTicks() - watch->startTimeStamp) / 1000.0;
	}
}

void pauseStopWatch(Object *input)
{
	StopWatch *watch = getComponentType(input, StopWatch);

	if (watch == NULL || watch->pause)
	{
		return;
	}

	watch->pause = true;
	watch->pauseTimeStamp = SDL_GetTicks();

	return;
}

#define resumeStopWatch(x) unpauseStopWatch(x)
void unpauseStopWatch(Object *input)
{
	StopWatch *watch = getComponentType(input, StopWatch);

	if (watch == NULL || !watch->pause)
	{
		return;
	}

	watch->pause = false;
	watch->startTimeStamp += SDL_GetTicks() - watch->pauseTimeStamp;

	return;
}

float endStopWatch(Object *input)
{
	float time = checkStopWatch(input);

	removeComponentType(input, StopWatch);

	return time;
}


FuncResult updateObjectsFrame(World *GameWorld)  
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL || GameWorld->ObjectList->FrameUpdates == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState == EMPTY_GAME || GameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}

	ObjectController *ObjectList = GameWorld->ObjectList;
	FrameUpdateFunction *currentFunction = ObjectList->FrameUpdates;

	// Update Behaviour
	while(currentFunction != NULL)
	{
		ExecuteFrameUpdateFunction(currentFunction, GameWorld);

		currentFunction = currentFunction->nextFunction;
	}


	return LEMON_SUCCESS;
}


int ExecuteFrameUpdateFunction(FrameUpdateFunction *inputFunction, World *GameWorld)
{
	if (inputFunction == NULL || inputFunction->FunctionPointer == NULL || GameWorld == NULL || inputFunction->inputObject == NULL)
	{
		return MISSING_DATA;
	}

	(inputFunction->FunctionPointer)(inputFunction->inputObject, GameWorld);

	UpdateParentChildLink(inputFunction->inputObject);

	return LEMON_SUCCESS;
}


int AddFrameUpdateFunction(TriggerableFunction FunctionPointer, Object *inputObject, ObjectController *ObjectList)
{
	if (inputObject == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	FrameUpdateFunction *function = malloc(sizeof(FrameUpdateFunction));

	if (function == NULL)
	{
		return LEMON_ERROR;
	}

	function->inputObject = inputObject;
	function->nextFunction = NULL;
	function->FunctionPointer = FunctionPointer;

	FrameUpdateFunction *CurrentFrameUpdate = ObjectList->FrameUpdates;

	if (CurrentFrameUpdate == NULL)
	{
		ObjectList->FrameUpdates = function;
	}
	else
	{
		while (CurrentFrameUpdate->nextFunction != NULL)
		{
			CurrentFrameUpdate = CurrentFrameUpdate->nextFunction;
		}

		CurrentFrameUpdate->nextFunction = function;
	}
	
	return LEMON_SUCCESS;
}


int deleteAllFrameUpdateFunctions(ObjectController *ObjectList)
{
	if (ObjectList == NULL || ObjectList->FrameUpdates == NULL)
	{
		return MISSING_DATA;
	}	

	FrameUpdateFunction *currentFunction = ObjectList->FrameUpdates;
	FrameUpdateFunction *DeleteFunction = currentFunction;

	while (currentFunction != NULL)
	{
		currentFunction = currentFunction->nextFunction;

		free(DeleteFunction);
		DeleteFunction = currentFunction;
	}

	ObjectList->FrameUpdates = NULL;

	return LEMON_SUCCESS;
}


int UpdateCoin(Object *coin, World *GameWorld)
{
	if (coin == NULL || GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	PlayerData *Player = &GameWorld->Player;
	PhysicsBox *coinBox = coin->ObjectBox;

	if (coinBox == NULL || Player->PlayerBox == NULL)
	{
		return MISSING_DATA;
	}

	if (checkBoxOverlapsBoxBroad(Player->PlayerBox, coin->ObjectBox))
	{
		Player->coinCount++;
		AddParticle(GameWorld, SPARKLE, coinBox->xPos + 20 - (rand() % 40), coinBox->yPos + 20 - (rand() % 40), 1, 0);
		MarkObjectForDeletion(coin);
		PlaySound("Coin_Collect", "Objects", OBJECT_SFX, 0.75);
	}

/*
	if (keyboard[LMN_INTERACT2] || coin->arg1 > 0)
	{
		if (DistanceBetween(coin, Player->PlayerPtr) < 25000.0)
		{
			coin->arg1 = 1;
		}

		PointObjectTowards(coin, Player->PlayerPtr);
		
		if (coinBox->forwardVelocity < 13.0)
		{
			coinBox->forwardVelocity += 1.5;
		}
	}
*/

	return LEMON_SUCCESS;
}


int UpdateSpring(Object *spring, World *GameWorld)
{
	if (spring == NULL || GameWorld == NULL || GameWorld->Player.PlayerBox == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsBox *PlayerBox = GameWorld->Player.PlayerBox;


	if (!playingAnimation(getDisplay(spring)) && PlayerBox->yVelocity < -1.0 && checkBoxOverlapsBoxBroad(PlayerBox, spring->ObjectBox))
	{
		float xForce = -cos(spring->ObjectBox->direction * DEGREE_TO_RADIAN_PI) * spring->arg1;
		float yForce = sin(spring->ObjectBox->direction * DEGREE_TO_RADIAN_PI) * spring->arg1;

		if (fabs(yForce) > 0.9)
		{
			PlayerBox->yVelocity = yForce;
		}

		if (fabs(xForce) > 0.9)
		{
			PlayerBox->xVelocity = xForce;
		}
		
		PlaySound("Spring", "Objects", OBJECT_SFX, 1.0);
		PlayAnimation("Bounce", 1, getDisplay(spring));
	}

	return LEMON_SUCCESS;
}


int InitialiseParticle(Object *particle, int animation, int repeatCount, int particleLifeTime)
{
	DisplayData *particleDisplay = getDisplay(particle);

	if (particle == NULL || particleDisplay == NULL)
	{
		return MISSING_DATA;
	}

	setDisplayLayer(particle, PARTICLES);
	particle->ObjectBox->solid = UNSOLID;
	particle->arg1 = particleLifeTime;
	particle->ParentLink = FINAL_LINK;
	centerOnXY(particle, particle->ObjectBox->xPos, particle->ObjectBox->yPos);
	particle->reserved |= RFLAG_CUTSCENE_IMMUNITY;


	switch (animation)
	{
	case SPARKLE:
		PlayAnimation("Sparkle", repeatCount, particleDisplay);
		SetObjectDirection(particle, PickRandomFloatBetween(0.0, 359.0));
		break;

	case STATIC:
		setTransparency(particle, 0.76);
	 	PlayAnimation("Static", repeatCount, particleDisplay);
		break;

	case 0:
		MarkObjectForDeletion(particle);
		break;

	default:
		PlayAnimationByIndex(animation, repeatCount, particleDisplay);
		break;
	}

	return LEMON_SUCCESS;
}


int UpdateParticle(World *GameWorld, Object *particle)
{
	// currentAnimation: which particle animation to play
	// arg1: particle max lifetime	(0 to simply default to deleting as soon as animation ends)
	// arg2: current particle lifetime

	if (particle == NULL)
	{
		return MISSING_DATA;
	}


	particle->arg2++;
	CustomParticleBehaviour(GameWorld, particle);


	// If arg2 exceeds maximum lifetime, mark for deletion
	if ((particle->arg1 > 0 && particle->arg2 > particle->arg1) || (particle->arg1 < 1 && !objectPlayingAnimation(particle)))
	{
		MarkObjectForDeletion(particle);
	}

	return LEMON_SUCCESS;
}


int CustomParticleBehaviour(World *GameWorld, Object *particle)
{
	DisplayData *particleDisplay = getDisplay(particle);

	if (particleDisplay == NULL)
	{
		return MISSING_DATA;
	}

	// Custom behaviour
	switch(particleDisplay->currentAnimation)
	{
		default:
		break;
	}

	return LEMON_SUCCESS;
}


int UpdateGateSwitch(PlayerData player, Object *gateSwitch, ObjectController *ObjectList)
{
	if (gateSwitch == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	// arg1 = switch ID
	// arg2 = switch type (0 = or, 1 = and)
	// Action = switch off/on (0/1) state
	// arg3 = timerLength

	if (PlayerInteractingWithBox(player, gateSwitch->ObjectBox))
	{
		gateSwitch->Action = (gateSwitch->Action + 1) % 2;

		// Update any gates
		toggleGateSwitch(gateSwitch, ObjectList);

		if (gateSwitch->Action == 1 && gateSwitch->ObjectID == GATE_SWITCH_TIMED)
		{
			startTimer(gateSwitch->arg3, gateSwitch);
		}
		else
		{
			endTimer(gateSwitch);	
			// end timer is somewhat redundant, basically ensures that after this point timerExpired is true, although that is never checked
		}

		return LEMON_SUCCESS;
	}

	if (gateSwitch->Action == 1 && gateSwitch->ObjectID == GATE_SWITCH_TIMED && timerExpired(gateSwitch))
	{
		gateSwitch->Action = 0;
		toggleGateSwitch(gateSwitch, ObjectList);
	}

	return LEMON_SUCCESS;
}


int toggleGateSwitch(Object *gateSwitch, ObjectController *ObjectList)
{
	gateControl(gateSwitch, ObjectList);
	if (gateSwitch->Action == 0)
	{
		PlayObjectAnimation("FlipSwitchOff", 1, gateSwitch);
	}
	else
	{
		PlayAnimation("FlipSwitchOn", 1, getDisplay(gateSwitch));
	}

	PlaySound("GateSwitchToggle", "Objects", OBJECT_SFX, 1.0);

	return LEMON_SUCCESS;
}

int UpdateVerticalGate(Object *gate, World *GameWorld)
{
	// arg1 = gate ID
	// arg2 = speed
	// Action = gate close/open (0/1)
	// arg3 = closed gate y position

	if (gate == NULL || GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	switchSpriteByName("Missing", USE_CURRENT_SPRITESET, gate->ObjectDisplay);
	switchObjectSpriteByName("Missing", gate);

	int closedPosition = gate->arg3;
	int speed = gate->arg2;
	bool open = (gate->Action == 1);
	PhysicsBox *gateBox = gate->ObjectBox;


	// Animation control
	switch (gate->arg4)
	{
		case 1:
		{
			float direction = (speed/abs(speed));
			gateBox->yVelocity += 0.15 * direction;

			if (fabs(gateBox->yVelocity) > abs(speed))
			{
				gateBox->yVelocity = (float)speed;
			}

			float futureYPos = gateBox->yPos + gateBox->yVelocity;
			if ((speed > 0 && futureYPos > closedPosition + gateBox->ySize) || (speed < 0 && futureYPos < closedPosition - gateBox->ySize))
			{
				gate->arg4 = 2;
				gateBox->yVelocity = 0.0;
				SetYPosition(gate, closedPosition + (gateBox->ySize * direction));
			}

		} break;


		case 2:
		{
			SetYPosition(gate, closedPosition + (gateBox->ySize * (speed/abs(speed))));
			gateBox->yVelocity = 0.0;

			if (!open)
			{
				gate->arg4 = 3;
				PlayObjectAnimation("Closing", 1, gate);
				PlaySound("GateClose", "Objects", 4, 1.0);
			}

		} break;


		case 3:
		{
			float direction = (speed/abs(speed));
			gateBox->yVelocity -= 0.15 * direction;

			if (fabs(gateBox->yVelocity) > abs(speed))
			{
				gateBox->yVelocity = (float)-speed;
			}

			float futureYPos = gateBox->yPos + gateBox->yVelocity;
			if ( (speed > 0 && futureYPos < closedPosition) || (speed < 0 && futureYPos > closedPosition) )
			{
				gate->arg4 = 0;
				gateBox->yVelocity = 0.0;
				SetYPosition(gate, closedPosition);
			}

		} break;


		default:
		{
			SetYPosition(gate, closedPosition);
			gateBox->yVelocity = 0.0;

			if (open)
			{
				gate->arg4 = 1;
				PlayObjectAnimation("Opening", 1, gate);
				PlaySound("GateOpen", "Objects", 4, 1.0);
			}
		
		} break;
	}


	return LEMON_SUCCESS;
}


int UpdateHorizontalGate(Object *gate, World *GameWorld)
{
	// arg1 = gate ID
	// arg2 = speed
	// Action = gate close/open (0/1)
	// arg3 = closed gate x position
	
	if (gate == NULL || GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	int closedPosition = gate->arg3;
	int speed = gate->arg2;
	bool open = (gate->Action == 1);
	PhysicsBox *gateBox = gate->ObjectBox;


	// Animation control
	switch (gate->arg4)
	{
		case 1:
		{
			float direction = (speed/abs(speed));
			gateBox->xVelocity += 0.15 * direction;

			if (fabs(gateBox->yVelocity) > abs(speed))
			{
				gateBox->xVelocity = (float)speed;
			}

			float futureXPos = gateBox->xPos + gateBox->xVelocity;
			if ((speed > 0 && futureXPos > closedPosition + gateBox->xSize) || (speed < 0 && futureXPos < closedPosition - gateBox->xSize))
			{
				gate->arg4 = 2;
				gateBox->xVelocity = 0.0;
				SetXPosition(gate, closedPosition + (gateBox->xSize * direction));
			}

		} break;


		case 2:
		{
			SetXPosition(gate, closedPosition + (gateBox->xSize * (speed/abs(speed))));
			gateBox->xVelocity = 0.0;

			if (!open)
			{
				gate->arg4 = 3;
				PlayObjectAnimation("Closing", 1, gate);
				PlaySound("GateClose", "Objects", 4, 1.0);
			}

		} break;


		case 3:
		{
			float direction = (speed/abs(speed));
			gateBox->xVelocity -= 0.15 * direction;

			if (fabs(gateBox->xVelocity) > abs(speed))
			{
				gateBox->xVelocity = (float)-speed;
			}

			float futureXPos = gateBox->xPos + gateBox->xVelocity;
			if ( (speed > 0 && futureXPos < closedPosition) || (speed < 0 && futureXPos > closedPosition) )
			{
				gate->arg4 = 0;
				gateBox->xVelocity = 0.0;
				SetXPosition(gate, closedPosition);
			}

		} break;


		default:
		{
			SetXPosition(gate, closedPosition);
			gateBox->xVelocity = 0.0;

			if (open)
			{
				gate->arg4 = 1;
				PlayObjectAnimation("Opening", 1, gate);
				PlaySound("GateOpen", "Objects", 4, 1.0);
			}
		
		} break;
	}

	return LEMON_SUCCESS;
}


int gateControl(Object *gateSwitch, ObjectController *ObjectList)
{
	if (gateSwitch == NULL || ObjectList == NULL || ObjectList->firstObject == NULL)
	{
		return MISSING_DATA;
	}

	Object *currentObject = ObjectList->firstObject;

	int onOrOff = -1;

	// Run through object list
	while (currentObject != NULL)
	{	
		// If a matching switch is found, evaluate whether connected gate should be open/closed, do this for every found switch
		if ((currentObject->ObjectID == GATE_SWITCH || currentObject->ObjectID == GATE_SWITCH_TIMED) && gateSwitch->arg1 == currentObject->arg1)
		{
			if (currentObject->arg2 == SINGLE_SWITCH && currentObject->Action == 1)
			{
				onOrOff = 1;
				currentObject = ObjectList->lastObject;
			} 
			else if (currentObject->arg2 == CHAIN_SWITCH)
			{
				if (onOrOff == -1)
				{
					onOrOff = currentObject->Action;
				}
				else
				{
					onOrOff = onOrOff && currentObject->Action;
				}
			}
		}
		
		currentObject = currentObject->nextObject;
	}


	currentObject = ObjectList->firstObject;

	while (currentObject != NULL)
	{	
		// If a matching gate is found, set open/closed value to onOrOff
		if ((currentObject->ObjectID == VERTICAL_GATE || currentObject->ObjectID == HORIZONTAL_GATE) && gateSwitch->arg1 == currentObject->arg1)
		{
			currentObject->Action = onOrOff;
		}
		
		currentObject = currentObject->nextObject;
	}

	return LEMON_SUCCESS;
}


Object* InitialiseMovingPlatform(Object *inputObject, int objectID, int xPos, int yPos, int bound1, int bound2, int speed, int timer)
{
	if (inputObject == NULL)
	{
		return NULL;
	}

	// Default settings
	inputObject->ObjectBox->ySize = Y_TILESCALE;
	inputObject->ObjectBox->xSize = X_TILESCALE * 3;
	inputObject->arg1 = bound1;
	inputObject->arg2 = bound2;
	inputObject->arg3 = abs(speed);
	inputObject->arg4 = abs(timer);
	inputObject->Action = 1;
	inputObject->ObjectBox->solid = JUMP_THROUGH;
	inputObject->ObjectBox->flag = ONLY_BODIES;
	addPhysics(inputObject, false);


	return inputObject;
}


int UpdateHorizontalPlatform(Object *platform)
{
	PhysicsBox *platformBox = platform->ObjectBox;
	int XPos = platformBox->xPos;
	int XPos2 = platformBox->xPos + platformBox->xSize;

	int leftBound = platform->arg1;
	int rightBound = platform->arg2;
	int maxSpeed = platform->arg3;

	// Accelerate
	if (platform->Action == 1 && platformBox->xVelocity < maxSpeed)
	{
		platformBox->xVelocity += 0.5;
	}

	if (platform->Action == 2 && platformBox->xVelocity > -maxSpeed)
	{
		platformBox->xVelocity -= 0.5;
	}

	// Deccelerate
	if ((XPos2 >= rightBound && platform->Action == 1) || (XPos <= leftBound && platform->Action == 2))
	{
		platform->Action = 0;
		startTimer(platform->arg4, platform);
	}

	// Wait to change direction
	if (platform->Action == 0)
	{
		ApplyFriction(platformBox, 1.0, 0.9, 1.0);

		if (timerExpired(platform) && fabs(platformBox->xVelocity) < 0.01)
		{
			platform->Action = (XPos <= leftBound) ? 1 : 2;
		}
	}

	return LEMON_SUCCESS;
}



int UpdateVerticalPlatform(Object *platform)
{
	PhysicsBox *platformBox = platform->ObjectBox;
	int YPos = platformBox->yPos;
	int YPos2 = platformBox->yPos + platformBox->ySize;

	int bottomBound = platform->arg1;
	int topBound = platform->arg2;
	int maxSpeed = platform->arg3;

	// Accelerate
	if (platform->Action == 1 && platformBox->yVelocity < maxSpeed)
	{
		platformBox->yVelocity += 0.5;
	}

	if (platform->Action == 2 && platformBox->yVelocity > -maxSpeed)
	{
		platformBox->yVelocity -= 0.5;
	}

	// Deccelerate
	if ((YPos2 >= topBound && platform->Action == 1) || (YPos <= bottomBound && platform->Action == 2))
	{
		platform->Action = 0;
		startTimer(platform->arg4, platform);
	}

	// Wait to change direction
	if (platform->Action == 0)
	{
		ApplyFriction(platformBox, 1.0, 1.0, 0.9);

		if (timerExpired(platform) && fabs(platformBox->yVelocity) < 0.01)
		{
			platform->Action = (YPos <= bottomBound) ? 1 : 2;
		}
	}

	return LEMON_SUCCESS;
}


int UpdateDoor(PlayerData Player, Object *Door, World *GameWorld)
{
	// arg1, arg2: [x/y]Pos of destination
	if (Player.PlayerPtr == NULL || Player.PlayerBox == NULL || Door == NULL)
	{
		return MISSING_DATA;
	}

	if (PlayerInteractingWithBox(Player, Door->ObjectBox) && Player.PlayerPtr->State == DEFAULT_STATE)
	{
		float savedXPos = Player.PlayerBox->xPos;
		float savedYPos = Player.PlayerBox->yPos;

		GoTo(Player.PlayerPtr, Door->arg1, Door->arg2);

		if (GetCollidingObject(Player.PlayerBox, GameWorld->ObjectList) != NULL)
		{
			SayText("The door seems to be blocked on the other side.", NO_PORTRAIT, BASIC_FADE, GameWorld);
			SayTextOption("", NO_PORTRAIT, BASIC_FADE, GameWorld, 3, 
				"W-what? What is it?", 			NO_ACTION, 
				"I SCREAM I SHOUT", 			NO_ACTION,
				"Actually i'm okay with this",	NO_ACTION);
		}
		else
		{
			// Test dialogue, replace with prompt "Go through door?" or something
			SayText("I", NO_PORTRAIT, BASIC_FADE, GameWorld);
			SayText("....Or is it?\r\nIt just looks like a big pink and black rectangle...", "Test_Face", BASIC_FADE, GameWorld);

			SayTextOption("Enter the Door?", "Test_Face", BASIC_FADE, GameWorld, 3, 
				"Yes", Event_teleportPlayerToExitDoor(Door->Parent, GameWorld), 
				"No", NO_ACTION,
				"hm... lemme think about it", playCutscene(TEST_SCENE_2, GameWorld));
		}

		GoTo(Player.PlayerPtr, savedXPos, savedYPos);
	}

	return LEMON_SUCCESS;
} 


int TeleportPlayerToExitDoor(Object *Door, World *GameWorld)
{	
	if (GameWorld == NULL || Door == NULL || GameWorld->Player.PlayerBox == NULL)
	{
		return MISSING_DATA;
	}

	PlayerData *Player = &GameWorld->Player;

	//GoTo(Player->PlayerPtr, Door->ObjectBox->xPos, Door->ObjectBox->yPos);
	centerOnObject(Player->PlayerPtr, Door);

	PlaySound("DoorOpen", "Objects", OBJECT_SFX, 1.0);
			
	ResetPlayer(Player);

	return LEMON_SUCCESS;
}


int UpdateLevelDoor(PlayerData Player, Object *Door, World *GameWorld)
{
	// arg1: Level to load
	// arg4: Open/close state
	// arg2, arg3: X/Y for Player at start of level
	if (Player.PlayerPtr == NULL || Door == NULL)
	{
		return MISSING_DATA;
	}

	if (Door->arg4 == 0 && PlayerInteractingWithBox(Player, Door->ObjectBox) && Player.PlayerPtr->State == DEFAULT_STATE)
	{
		Door->arg4 = 1;
		SayText("It's a door... \nIt eminates a strange glow.", NO_PORTRAIT, BASIC_FADE, GameWorld);
		char phrase[MAX_TEXT_LENGTH] = {0};
		snprintf(phrase, MAX_TEXT_LENGTH, "This Door will send you to level %d!", Door->arg1);
		SayText(phrase, NO_PORTRAIT, BASIC_FADE, GameWorld);
	}

	if (Door->arg4 != 0 && GameWorld->TextQueue == NULL)
	{
		switchLevel(Door->arg1, GameWorld);
		Event_movePlayer(Door->arg2, Door->arg3, GameWorld);

		Door->arg4 = 0;
	}

	return LEMON_SUCCESS;
} 


int ApplyGravity(Object *inputObject, World *GameWorld)
{
	if ((inputObject->reserved & RFLAG_DISABLE_PHYSICS) != 0)
	{
		return MISSING_DATA;
	}

	PhysicsBox *inputBox = inputObject->ObjectBox;

	if (GameWorld->PhysicsType == PLATFORMER)
	{
		inputBox->yVelocity += GameWorld->GlobalGravityY;
		inputBox->xVelocity += GameWorld->GlobalGravityX;

		AdjustDirection(inputBox, GameWorld);

		CheckForGround(inputBox, GameWorld);
	}
	else
	{
		inputBox->inAir = 0;
	}

	return LEMON_SUCCESS;
}


int ApplyFriction(PhysicsBox *inputBox, float forwardFriction, float xFriction, float yFriction)
{
	if (inputBox == NULL)
	{
		return MISSING_DATA;
	}

	if (forwardFriction > 0.0)
	{
		inputBox->forwardVelocity *= forwardFriction;
	}

	if (xFriction > 0.0)
	{
		inputBox->xVelocity *= xFriction;
		inputBox->PhysicsXVelocity *= 0.98;
	}

	if (yFriction > 0.0)
	{
		inputBox->yVelocity *= yFriction;
		inputBox->PhysicsYVelocity *= 0.98;
	}

	if (fabs(inputBox->xVelocity) < 0.001)
	{
		inputBox->xVelocity = 0.0;
	}

	if (fabs(inputBox->yVelocity) < 0.001)
	{
		inputBox->yVelocity = 0.0;
	}

	if (fabs(inputBox->PhysicsXVelocity) < 0.001)
	{
		inputBox->PhysicsXVelocity = 0.0;
	}

	if (fabs(inputBox->PhysicsYVelocity) < 0.001)
	{
		inputBox->PhysicsYVelocity = 0.0;
	}

	if (fabs(inputBox->forwardVelocity) < 0.001)
	{
		inputBox->forwardVelocity = 0.0;
	}
		

	return LEMON_SUCCESS;
}


Object* CheckForGround(PhysicsBox *movingBox, World *GameWorld)
{
	if (!LEMON_COLLISION_PHYSICS || GameWorld == NULL || movingBox == NULL || movingBox->solid == UNSOLID)
	{
		return NULL;
	}

	float savedX = movingBox->xPos;
	float savedY = movingBox->yPos;

	movingBox->yPos += fClamp(GameWorld->GlobalGravityY * 5.0, -32.0, 32.0);
	movingBox->xPos += fClamp(GameWorld->GlobalGravityX * 5.0, -32.0, 32.0);

	Object *GroundObject = GetCollidingObject(movingBox, GameWorld->ObjectList);

	movingBox->xPos = savedX;
	movingBox->yPos = savedY;


	if (GroundObject != NULL)
	{
		if (movingBox->inAir > 0)
		{
			if (fabs(movingBox->PhysicsXVelocity) > 0.1)
			{
				movingBox->xVelocity = 0.0;
			}

			if (fabs(movingBox->PhysicsYVelocity) > 0.1)
			{
				movingBox->yVelocity = 0.0;
			}
		}

		// GroundBox should only be set once, ideally by the AppyGravity function, so if it has been set, do not override
		if (movingBox->GroundBox == NULL)
		{
			movingBox->GroundBox = GroundObject->ObjectBox;

			movingBox->inAir = 0;
		}
	}
	else
	{
		// If inAir is 0, that means at last check you were on ground
		if (movingBox->inAir == 0)
		{
			// movingBox->xVelocity += movingBox->PhysicsXVelocity;
			// movingBox->yVelocity += movingBox->PhysicsYVelocity;
			// movingBox->PhysicsXVelocity = 0.0;
			// movingBox->PhysicsYVelocity = 0.0;
		}

		// To indicate no ground, the pointer is set to the box itself
		if (movingBox->GroundBox == NULL)
		{
			movingBox->GroundBox = movingBox;

			movingBox->inAir++;
		}

		if (movingBox->inAir > 99)
		{
			movingBox->inAir = 100;

			// Falling for a long time!
		}
	}

	return GroundObject;
}


// Used to update ground object without affecting other values - SHOULD ONLY BE USED IN PHYSICS APPLICATIONS WHEN NECESSARY
int redoGroundCheck(Object *input, World *GameWorld)
{
	if (GameWorld->PhysicsType != PLATFORMER || !HasGravity(input))
	{
		return ACTION_DISABLED;
	}

	PhysicsBox *inputBox = input->ObjectBox;
	if (inputBox->GroundBox != NULL)
	{
		inputBox->GroundBox = NULL;
		CheckForGround(inputBox, GameWorld);

		if (inputBox->inAir > 1)
		{
			inputBox->inAir--;
		}
	}

	return LEMON_SUCCESS;
}


int GoTo(Object *inputObject, float destX, float destY)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsBox *inputBox = inputObject->ObjectBox;

	inputBox->xPos = destX;
	inputBox->yPos = destY;
	inputBox->prevXPos = destX;
	inputBox->prevYPos = destY;

	return LEMON_SUCCESS;
}


int GoToWithCollision(Object *inputObject, float destX, float destY, ObjectController *ObjectList)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}


	GoTo(inputObject, destX, destY);


	if (ObjectList == NULL || inputObject->ObjectBox->solid == UNSOLID)
	{
		return MISSING_DATA;
	}

	ResolveAllXCollisionsByPush(inputObject->ObjectBox, ObjectList);
	ResolveAllYCollisionsByPush(inputObject->ObjectBox, ObjectList);


	return LEMON_SUCCESS;
}


int SetXPosition(Object *inputObject, float newXPos)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsBox *inputBox = inputObject->ObjectBox;

	if (fabs(inputBox->xPos - newXPos) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}


	GoTo(inputObject, newXPos, inputBox->yPos);

	return LEMON_SUCCESS;
}


//Method for setting an object's position to avoid incorrect collision with player
int SetYPosition(Object *inputObject, float newYPos)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsBox *inputBox = inputObject->ObjectBox;

	if (fabs(inputBox->yPos - newYPos) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}


	GoTo(inputObject, inputBox->xPos, newYPos);

	return LEMON_SUCCESS;
}

int centerOnXY(Object *input, float xPos, float yPos)
{
	if (input == NULL)
	{
		return MISSING_DATA;
	}


	input->ObjectBox->xPos = xPos - (float)(input->ObjectBox->xSize >> 1);
	input->ObjectBox->yPos = yPos - (float)(input->ObjectBox->ySize >> 1);

	return LEMON_SUCCESS;
}

int centerOnObject(Object *input, Object *dest)
{
	if (input == NULL || dest == NULL)
	{
		return MISSING_DATA;
	}


	input->ObjectBox->xPos = dest->ObjectBox->xPos + ((dest->ObjectBox->xSize - input->ObjectBox->xSize) >> 1);
	input->ObjectBox->yPos = dest->ObjectBox->yPos + ((dest->ObjectBox->ySize - input->ObjectBox->ySize) >> 1);

	return LEMON_SUCCESS;
}

int centerOnMouse(Object *input, Camera inputCamera)
{
	if (input == NULL)
	{
		return MISSING_DATA;
	}

	if (getDisplayLayer(input) == HUD)
	{
		input->ObjectBox->xPos = MouseInput.xPos - (input->ObjectBox->xSize >> 1);
		input->ObjectBox->yPos = MouseInput.yPos - (input->ObjectBox->ySize >> 1);
	}
	else
	{
		input->ObjectBox->xPos = getMouseXCam(inputCamera) - (input->ObjectBox->xSize >> 1);
		input->ObjectBox->yPos = getMouseYCam(inputCamera) - (input->ObjectBox->ySize >> 1);
	}
	
	return LEMON_SUCCESS;
}

int PointObjectTowards(Object *inputObject, Object *pointDestination)
{
	if (pointDestination == NULL || pointDestination->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	float destX = pointDestination->ObjectBox->xPos + (pointDestination->ObjectBox->xSize >> 1);
	float destY = pointDestination->ObjectBox->yPos + (pointDestination->ObjectBox->ySize >> 1);

	PointObjectToXY(inputObject, destX, destY);

	return LEMON_SUCCESS;
}


int PointObjectToMouse(Object *inputObject, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	float MouseX = MouseInput.xPos + GameWorld->MainCamera.CameraX;
	float MouseY = MouseInput.yPos + GameWorld->MainCamera.CameraY;

	PointObjectToXY(inputObject, MouseX, MouseY);

	return LEMON_SUCCESS;
}


int PointObjectToXY(Object *inputObject, float xPos, float yPos)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	float originX = inputObject->ObjectBox->xPos + (inputObject->ObjectBox->xSize >> 1);
	float originY = inputObject->ObjectBox->yPos + (inputObject->ObjectBox->ySize >> 1);

	double newDirection = atan2(xPos - originX, yPos - originY) * RADIAN_TO_DEGREE_PI;

	inputObject->ObjectBox->direction = newDirection;


	return LEMON_SUCCESS;
}


int RotateObject(Object *inputObject, float rotationDegrees)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	//rotationDegrees = rotationDegrees * inputObject->ObjectBox->xFlip;

	float newDirection = inputObject->ObjectBox->direction + rotationDegrees;

	while (newDirection > 180.0 || newDirection < -180.0)
	if (newDirection > 0.0)
	{
		newDirection -= 360.0;
	}
	else if (newDirection < 0.0)
	{
		newDirection += 360.0;
	}


	inputObject->ObjectBox->direction = newDirection;


	return LEMON_SUCCESS;
}


int SetObjectDirection(Object *inputObject, float rotationDegrees)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	while (rotationDegrees > 180.0 || rotationDegrees < -180.0)
	if (rotationDegrees > 0.0)
	{
		rotationDegrees -= 360.0;
	}
	else if (rotationDegrees < 0.0)
	{
		rotationDegrees += 360.0;
	}

	inputObject->ObjectBox->direction = rotationDegrees;

	if (fabs(DEFAULT_DIRECTION - inputObject->ObjectBox->direction) < 0.01)
	{
		inputObject->ObjectBox->direction = DEFAULT_DIRECTION;
	}

	return LEMON_SUCCESS;
}


int setSize(Object *input, int xSize, int ySize)
{
	if (input == NULL || input->ObjectBox == NULL || xSize < 0 || ySize < 0)
	{
		return INVALID_DATA;
	}

	input->ObjectBox->xSize = xSize;
	input->ObjectBox->ySize = ySize;

	return LEMON_SUCCESS;
}

// Method for centering object size increase and handling player collisions
int ChangeObjectXSizeBy(int change, Object *inputObject, ObjectController *ObjectList)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	float changeHalf = change >> 1;

	inputObject->ObjectBox->xSize += change;
	inputObject->ObjectBox->xPos -= changeHalf;

	if (ObjectList == NULL || change == 0 || inputObject->ObjectBox->solid == UNSOLID)
	{
		return EXECUTION_UNNECESSARY;
	}

	ResolveAllXCollisionsByPush(inputObject->ObjectBox, ObjectList);
	
	return LEMON_SUCCESS;
}


int ChangeObjectYSizeBy(int change, Object *inputObject, ObjectController *ObjectList)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	float changeHalf = change >> 1;

	inputObject->ObjectBox->ySize += change;
	inputObject->ObjectBox->yPos -= changeHalf;

	if (ObjectList == NULL || change == 0 || inputObject->ObjectBox->solid == UNSOLID)
	{
		return EXECUTION_UNNECESSARY;
	}


	ResolveAllYCollisionsByPush(inputObject->ObjectBox, ObjectList);
	
	return LEMON_SUCCESS;
}


int setScaleSize(Object *input, float sizePercentage)
{
	if (sizePercentage < 0.001 || sizePercentage > 100.0)		// 1.0 is normal size, 100.0 is 100 times larger
	{
		return INVALID_DATA;
	}

	DisplayData *inputDisplay = getDisplay(input);

	if (inputDisplay == NULL)
	{
		return MISSING_DATA;
	}

	inputDisplay->size = sizePercentage;

	return LEMON_SUCCESS;
}


int changeScaleSize(Object *input, float sizePercentage)
{
	DisplayData *inputDisplay = getDisplay(input);

	if (inputDisplay == NULL)
	{
		return MISSING_DATA;
	}

	return setScaleSize(input, sizePercentage + inputDisplay->size);
}


int smoothSizeChangeTo(Object *input, float desiredSize, float rate)
{
	DisplayData *inputDisplay = getDisplay(input);

	if (inputDisplay == NULL)
	{
		return MISSING_DATA;
	}

	if (rate < 0.1 || desiredSize < 0.0001)
	{
		return INVALID_DATA;
	}

	if (fabs(inputDisplay->size - desiredSize) < 0.001)
	{
		inputDisplay->size = desiredSize;
		return EXECUTION_UNNECESSARY;
	}

	return changeScaleSize(input, (desiredSize - inputDisplay->size) / rate);
}


int matchBoxToDisplayDimensions(Object *input)
{
	DisplayData *inputDisplay = getDisplay(input);

	if (inputDisplay == NULL)
	{
		return MISSING_DATA;
	}

	Sprite *sprite = inputDisplay->spriteBuffer;

	if (sprite == NULL || sprite == EngineSettings.DefaultTexture)
	{
		return MISSING_DATA;
	}

	input->ObjectBox->xSize = (int)(sprite->width * inputDisplay->size);
	input->ObjectBox->ySize = (int)(sprite->height * inputDisplay->size);

	return LEMON_SUCCESS;
}


float DistanceBetween(Object *Source, Object *Target)
{
	if (Source == NULL || Source->ObjectBox == NULL || Target == NULL || Target->ObjectBox == NULL)
	{
		return -1.0;
	}

	PhysicsBox *box1 = (Source->ObjectBox);
	PhysicsBox *box2 = (Target->ObjectBox);

	float xDiff = (box2->xPos + (box2->xSize >> 1)) - (box1->xPos + (box1->xSize >> 1));
	float yDiff = (box2->yPos + (box2->ySize >> 1)) - (box1->yPos + (box1->ySize >> 1));

	float xDistance = xDiff * xDiff;
	float yDistance = yDiff * yDiff;

	return (float)sqrt(xDistance + yDistance);
}


bool onScreen(Object *inputObject, World *GameWorld)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL || GameWorld == NULL)
	{
		return false;
	}

	int camX = -(GameWorld->MainCamera.width >> 1);
	int camY = -(GameWorld->MainCamera.height >> 1);
	PhysicsBox *objBox = inputObject->ObjectBox;

	if (getDisplayLayer(inputObject) != HUD)
	{
		camX +=	GameWorld->MainCamera.CameraX;
		camY += GameWorld->MainCamera.CameraY;
	}

	int camXRight = camX + GameWorld->MainCamera.width;
	int camYTop = camY + GameWorld->MainCamera.height;

	if ((int)objBox->xPos > camXRight || (int)objBox->xPos + objBox->xSize < camX || (int)objBox->yPos > camYTop || (int)objBox->yPos + objBox->ySize < camY)
	{
		return false;
	}

	return true;
}


bool MouseOverlappingBox(Object *input, Camera inputCam)
{
	if (input == NULL || input->ObjectBox == NULL)
	{
		return false;
	}

	PhysicsBox mouseBox = {0};

	mouseBox.xSize = 1;
	mouseBox.ySize = 1;
	
	if (getDisplayLayer(input) != HUD)
	{
		mouseBox.xPos = getMouseXCam(inputCam);
		mouseBox.yPos = getMouseYCam(inputCam);
	}
	else
	{
			mouseBox.xPos = MouseInput.xPos;
		mouseBox.yPos = MouseInput.yPos;
	}
	
	return checkBoxOverlapsBoxBroad(input->ObjectBox, &mouseBox);
}

bool MouseOverlappingSprite(Object *input, Camera inputCam)
{
	DisplayData *inputDisplay = getDisplay(input);

	if (inputDisplay == NULL || input == NULL || input->ObjectBox == NULL || inputDisplay->spriteBuffer == NULL)
	{
		return false;
	}

	PhysicsBox inputBox = *(input->ObjectBox);
	int renderMode = inputDisplay->RenderModeOverride == DEFAULT_TO_SPRITE ? inputDisplay->spriteBuffer->RenderMode : inputDisplay->RenderModeOverride;

	if (renderMode == SINGLE)
	{
		inputBox.xPos = inputBox.xPos + (inputBox.xSize >> 1) - (inputDisplay->spriteBuffer->width >> 1);
		inputBox.xSize = inputDisplay->spriteBuffer->width;
		inputBox.yPos = inputBox.yPos + (inputBox.ySize >> 1) - (inputDisplay->spriteBuffer->height >> 1);
		inputBox.ySize = inputDisplay->spriteBuffer->height;
	}


	inputBox.xPos += inputDisplay->spriteXOffset;
	inputBox.yPos += inputDisplay->spriteYOffset;


	PhysicsBox mouseBox = {0};
	mouseBox.xSize = 1;
	mouseBox.ySize = 1;
	mouseBox.xPos = MouseInput.xPos;
	mouseBox.yPos = MouseInput.yPos;

	if (getDisplayLayer(input) != HUD)
	{
		mouseBox.xPos = getMouseXCam(inputCam);
		mouseBox.yPos = getMouseYCam(inputCam);
	}

	return checkBoxOverlapsBoxBroad(&inputBox, &mouseBox);
}

bool MouseClickedObject(Object *input, Camera inputCam)
{
	if (input == NULL)
	{
		return false;
	}

	return MouseOverlappingBox(input, inputCam) && (MouseInput.LeftButton == 1);
}


bool checkBoxOverlapsBoxBroad(PhysicsBox *inputBox, PhysicsBox *compareBox)
{
	if (inputBox == compareBox)
	{
		return false;
	}

	return !((int)inputBox->xPos >= (int)compareBox->xPos + compareBox->xSize || (int)inputBox->xPos + inputBox->xSize <= (int)compareBox->xPos 
		|| (int)inputBox->yPos >= (int)compareBox->yPos + compareBox->ySize || (int)inputBox->yPos + inputBox->ySize <= (int)compareBox->yPos);
}


bool CheckBoxOverlapsBox(PhysicsBox *inputBox, PhysicsBox *compareBox)
{
	if (inputBox == NULL || compareBox == NULL)
	{
		return false;
	}

	if (inputBox == compareBox || inputBox->xSize <= 0 || inputBox->ySize <= 0 || compareBox->xSize <= 0 || compareBox->ySize <= 0)
	{
		return false;
	}

	int inputX = inputBox->xPos;
	int inputXRight = inputBox->xPos + inputBox->xSize;
	int inputY = inputBox->yPos;
	int inputYTop = inputBox->yPos + inputBox->ySize;


	int compareX = compareBox->xPos;
	int compareXRight = compareBox->xPos + compareBox->xSize;
	int compareY = compareBox->yPos;
	int compareYTop = compareBox->yPos + compareBox->ySize;


	switch(inputBox->solid)
	{
		case FLAT_SLOPE:
		{
			if (inputBox->xFlip == 1)
			{
				inputYTop = ((compareBox->xPos + compareBox->xSize - inputBox->xPos) * ((float)inputBox->ySize/(float)inputBox->xSize));
			}
			else
			{
				inputYTop = ((inputBox->xSize - compareBox->xPos + inputBox->xPos) * ((float)inputBox->ySize/(float)inputBox->xSize));
			}
			
			inputYTop = fClamp(inputYTop, 0.0, (float)inputBox->ySize);

			if (inputBox->yFlip == -1)
			{
				inputY = inputBox->ySize - inputYTop + inputBox->yPos;
				inputYTop = inputBox->ySize;
			}
			
			inputYTop += inputBox->yPos;
		} break;
			

		default:
		break;
	}


	switch(compareBox->solid)
	{
		case FLAT_SLOPE:
		{
			if (compareBox->xFlip == 1)
			{
				compareYTop = ((inputBox->xPos + inputBox->xSize - compareBox->xPos) * ((float)compareBox->ySize/(float)compareBox->xSize));
			}
			else
			{
				compareYTop = ((compareBox->xSize - (inputBox->xPos - compareBox->xPos)) * ((float)compareBox->ySize/(float)compareBox->xSize));
			}

			compareYTop = fClamp(compareYTop, 0.0, (float)compareBox->ySize);

			if (compareBox->yFlip == -1)
			{
				compareY = compareBox->ySize - compareYTop + compareBox->yPos;
				compareYTop = compareBox->ySize;
			}
			
			compareYTop += compareBox->yPos;
		} break;
			
		default:
		break;
	}


	return !(inputY >= compareYTop || inputYTop <= compareY || inputX >= compareXRight || inputXRight <= compareX);
}


int CheckBoxCollidesBox(PhysicsBox *inputBox, PhysicsBox *compareBox)
{
	if (inputBox == NULL || compareBox == NULL)
	{
		return MISSING_DATA;
	}

	if (compareBox->collideLayer != inputBox->collideLayer)
	{
		return 0;
	}

	if (CheckBoxOverlapsBox(inputBox, compareBox) == false)
	{
		return 0;
	}


	switch(inputBox->flag)
	{
		case IGNORE_SOLID:
			if (compareBox->solid == SOLID)
			{
				return 0;
			}
			break;

		case IGNORE_SELF:
			if (compareBox->solid == inputBox->solid)
			{
				return 0;
			}
			break;

		case ONLY_BODIES:
			if (compareBox->solid != BODY)
			{
				return 0;
			}
			break;

		default:
		break;
	}

	switch(compareBox->flag)
	{
		case IGNORE_SOLID:
			if (inputBox->solid == SOLID)
			{
				return 0;
			}
			break;

		case IGNORE_SELF:
			if (compareBox->solid == inputBox->solid)
			{
				return 0;
			}
			break;

		case ONLY_BODIES:
			if (inputBox->solid != BODY)
			{
				return 0;
			}
			break;

		case GET_IGNORED:
			return 0;

		default:
		break;
	}


	switch(inputBox->solid)
	{		
		case JUMP_THROUGH:
		{
			if (compareBox->yVelocity > inputBox->yVelocity || compareBox->crouch == true || compareBox->prevYPos < (inputBox->prevYPos + inputBox->ySize - 1) )
			{
				return 0;
			}
		} break;

		case UNSOLID:
		return 0;

		case BODY:
		if (compareBox->solid == BODY)
		{
			return 0;
		}
		break;

		default:
		break;
	}


	switch(compareBox->solid)
	{
		case JUMP_THROUGH:
		{
			if (inputBox->yVelocity > compareBox->yVelocity || inputBox->crouch == true || inputBox->prevYPos < (compareBox->prevYPos + compareBox->ySize - 1) )
			{
				return 0;
			}
		} break;

		case UNSOLID:
		return 0;
			
		default:
		break;
	}


	return 1;
}


int AssignDirection(PhysicsBox *inputBox, PhysicsBox *compareBox)
{
	if (inputBox == NULL || compareBox == NULL)
	{
		return MISSING_DATA;
	}

	
	switch (compareBox->solid)
	{
		case FLAT_SLOPE:
		{
			if (compareBox->xSize < 1 || compareBox->ySize < 1)
			{
				inputBox->direction = DEFAULT_DIRECTION;
				break;
			}

			// Check against flat side
			int difference = (int)(inputBox->prevYPos - compareBox->yPos);

			if ( (compareBox->yFlip == 1 && difference < -inputBox->ySize + 1) || (compareBox->yFlip == -1 && difference > compareBox->ySize - 1))
			{
				inputBox->direction = DEFAULT_DIRECTION;
				break;
			}

			float slope = (float)compareBox->ySize/(float)compareBox->xSize;
			float slopeFloor;

			if (compareBox->xFlip == 1)
			{
				slopeFloor = ((inputBox->xPos + inputBox->xSize - compareBox->xPos) * slope);
			}
			else
			{
				slopeFloor = ((compareBox->xSize - (inputBox->xPos - compareBox->xPos)) * slope);
			}

			if (compareBox->yFlip == -1)
			{
				slopeFloor = compareBox->ySize - slopeFloor;
			}

			if (slopeFloor < 0.0 || slopeFloor > compareBox->ySize)
			{
				inputBox->direction = DEFAULT_DIRECTION;
				break;
			}
			
			
			inputBox->direction = (RADIAN_90 - ((atan(slope))) * compareBox->xFlip) * RADIAN_TO_DEGREE_PI;
			
		} break;


		default:
			inputBox->direction = DEFAULT_DIRECTION;
			break;
	}

	return LEMON_SUCCESS;
}


Object* GetCollidingObjectFast(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	if (!LEMON_COLLISION_PHYSICS || inputBox == NULL || inputBox->solid == UNSOLID || ObjectList == NULL)
	{
		return NULL;
	}

	int i = -1;
	int *list = ObjectList->solidList.list;
	Object *objects = ObjectList->objectComponents.Objects;
	PhysicsBox *boxes = ObjectList->objectComponents.PhysicsBoxes;
	int index = 0;

	while (i < ObjectList->solidList.storedElements)
	{
		i++;
		index = list[i];

		if (boxes[index].solid == UNSOLID || checkBoxOverlapsBoxBroad(inputBox, &boxes[index]) == false)
		{
			continue;
		}

		if (CheckBoxCollidesBox(inputBox, &boxes[index]) == 1)
		{
			return &objects[index];
		}
	}

	return NULL;
}


// returns pointer of object overlapping, NULL if no object is detected; has n^2 complexity, not great!
Object* GetCollidingObject(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	if (!LEMON_COLLISION_PHYSICS || inputBox == NULL || inputBox->solid == UNSOLID || ObjectList == NULL)
	{
		return NULL;
	}

	int i = ObjectList->objectCount;

	if (i > FAST_COLLISION_THRESHOLD)
	{
		return GetCollidingObjectFast(inputBox, ObjectList);
	}

	Object *currentObject = ObjectList->firstObject;

	while (currentObject != NULL && i > 0)
	{
		i--;

		if (currentObject->ObjectBox->solid == UNSOLID || !checkBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox))
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		if (CheckBoxCollidesBox(inputBox, currentObject->ObjectBox) == 1)
		{
			return currentObject;
		}

		currentObject = currentObject->nextObject;
	}

	return NULL;
}


Object* GetOverlappingObject(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	if (!LEMON_COLLISION_PHYSICS || inputBox == NULL || ObjectList == NULL)
	{
		return NULL;
	}

	Object *currentObject = ObjectList->firstObject;

	int i = ObjectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		i--;

		if (checkBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox) == false)
		{
			currentObject = currentObject->nextObject;
			continue;
		}
		
		if (CheckBoxOverlapsBox(inputBox, currentObject->ObjectBox))
		{
			return currentObject;
		}

		currentObject = currentObject->nextObject;
	}

	return NULL;
}


Object* GetOverlappingObjectType(PhysicsBox *inputBox, int overlapObjectID, ObjectController *ObjectList)
{
	if (!LEMON_COLLISION_PHYSICS || inputBox == NULL || ObjectList == NULL)
	{
		return NULL;
	}

	Object *currentObject = ObjectList->firstObject;

	int i = ObjectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		if (checkBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox) == false)
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		
		if (currentObject->ObjectID == overlapObjectID && CheckBoxOverlapsBox(inputBox, currentObject->ObjectBox))
		{
			return currentObject;
		}


		currentObject = currentObject->nextObject;

		i--;
	}

	return NULL;
}


Object* GetOverlappingSolidFast(PhysicsBox *inputBox, int solidID, ObjectController *ObjectList)
{
	if (!LEMON_COLLISION_PHYSICS || inputBox == NULL || inputBox->solid == UNSOLID || ObjectList == NULL)
	{
		return NULL;
	}

	int i = -1;
	int *list = ObjectList->solidList.list;
	Object *objects = ObjectList->objectComponents.Objects;
	PhysicsBox *boxes = ObjectList->objectComponents.PhysicsBoxes;
	int index = 0;

	while (i < ObjectList->solidList.storedElements)
	{
		i++;
		index = list[i];
		if (boxes[index].solid == UNSOLID || checkBoxOverlapsBoxBroad(inputBox, &boxes[index]) == false)
		{
			continue;
		}

		if (CheckBoxOverlapsBox(inputBox, &boxes[index]) && (solidID == UNDEFINED_SOLID || solidID == boxes[index].solid))
		{
			return &objects[list[i]];
		}
	}

	return NULL;
}


Object* GetOverlappingObjectSolid(PhysicsBox *inputBox, int solidID, ObjectController *ObjectList)
{
	if (!LEMON_COLLISION_PHYSICS || ObjectList == NULL || inputBox == NULL)
	{
		return NULL;
	}

	int i = ObjectList->objectCount;

	if (i > FAST_COLLISION_THRESHOLD)
	{
		return GetOverlappingSolidFast(inputBox, solidID, ObjectList);
	}

	Object *currentObject = ObjectList->firstObject;

	while(currentObject != NULL && i > 0)
	{
		i--;

		if (checkBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox) == false)
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		
		if (currentObject->ObjectBox->solid == solidID && CheckBoxOverlapsBox(inputBox, currentObject->ObjectBox))
		{
			return currentObject;
		}
		

		currentObject = currentObject->nextObject;
	}

	return NULL;
}


Object* GetOverlappingObjectAllSolids(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	if (!LEMON_COLLISION_PHYSICS || ObjectList == NULL || inputBox == NULL)
	{
		return NULL;
	}

	int i = ObjectList->objectCount;

	if (i > FAST_COLLISION_THRESHOLD)
	{
		return GetOverlappingSolidFast(inputBox, UNDEFINED_SOLID, ObjectList);
	}

	Object *currentObject = ObjectList->firstObject;


	while(currentObject != NULL && i > 0)
	{
		i--;

		if (checkBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox) == false)
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		
		if (currentObject->ObjectBox->solid != UNSOLID && CheckBoxOverlapsBox(inputBox, currentObject->ObjectBox))
		{
			return currentObject;
		}
		

		currentObject = currentObject->nextObject;
	}

	return NULL;
}

bool OverlapsObject(Object *inputObject, Object *otherObject)
{
	if (inputObject == NULL || otherObject == NULL)
	{
		return false;
	}

	return CheckBoxOverlapsBox(inputObject->ObjectBox, otherObject->ObjectBox);
}


static int depthCounter = 0;

int MoveObject(Object *inputObject, World *GameWorld)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL || GameWorld == NULL)	
	{ 
		return MISSING_DATA; 
	}

	if ((inputObject->ParentLink & POSITION_LINK) && inputObject->Parent != NULL)	// If this object must follow its parent, there is no need to perform movement/collision
	{
		return ACTION_DISABLED;
	}

	ObjectController *ObjectList = GameWorld->ObjectList;
	depthCounter = 0;

	moveObjectX(inputObject, ObjectList);
	moveObjectY(inputObject, ObjectList);
	moveObjectForward(inputObject, ObjectList);

	PhysicsBox *inputBox = inputObject->ObjectBox;
	inputBox->xPos = fClamp(inputBox->xPos, -EngineSettings.WorldBoundX, EngineSettings.WorldBoundX - inputBox->xSize);
	inputBox->yPos = fClamp(inputBox->yPos, -EngineSettings.WorldBoundY, EngineSettings.WorldBoundY - inputBox->ySize);

	redoGroundCheck(inputObject, GameWorld);

	return LEMON_SUCCESS;
}


int moveObjectX(Object *input, ObjectController *ObjectList)
{
	if (fabs(input->ObjectBox->xVelocity) < 0.01)
	{
		return EXECUTION_UNNECESSARY;
	}

	input->ObjectBox->xPos += input->ObjectBox->xVelocity;

	// if object is set as UNSOLID or has no physics attached, it does not perform collision detection
	// In addition, the LEMON_COLLISION_PHYSICS constant mean that when compiling with this flag disabled, the entire collision system is disabled, usually to be optimised away by the compiler
	if (input->ObjectBox->solid != UNSOLID && HasPhysics(input) && LEMON_COLLISION_PHYSICS)	
	{
		ResolveAllXCollision(input->ObjectBox, ObjectList);
	}
	
	return LEMON_SUCCESS;
}



int moveObjectY(Object *input, ObjectController *ObjectList)
{
	if (fabs(input->ObjectBox->yVelocity) < 0.01)
	{
		return EXECUTION_UNNECESSARY;
	}

	input->ObjectBox->yPos += input->ObjectBox->yVelocity;

	if (input->ObjectBox->solid != UNSOLID && HasPhysics(input) && LEMON_COLLISION_PHYSICS)
	{
		ResolveAllYCollision(input->ObjectBox, ObjectList);
	}

	return LEMON_SUCCESS;
}


bool evaluateIfCollidePush(PhysicsBox *movingBox, PhysicsBox *collideBox)
{
	if (movingBox == NULL || collideBox == NULL)
	{
		return false;
	}

	if (movingBox->flag == IMPACT_COLLISION)
	{
		return false;
	}


	int canPush = (movingBox->solid != PUSHABLE_SOLID) || (collideBox->solid != BODY);
	int collidePushable = collideBox->solid == PUSHABLE_SOLID || collideBox->solid == BODY;
	
	
	return (movingBox->flag == PUSH_COLLISION || (canPush && collidePushable) );
}


int AdjustDirection(PhysicsBox *movingBox, World *GameWorld)
{	
	if (!LEMON_COLLISION_PHYSICS || GameWorld == NULL || GameWorld->ObjectList == NULL || movingBox == NULL)
	{
		return MISSING_DATA;
	}

	if (movingBox->solid <= UNSOLID)
	{
		return EXECUTION_UNNECESSARY;
	}

	if (GameWorld->PhysicsType == PLATFORMER && movingBox->GroundBox != NULL && movingBox->GroundBox != movingBox)
	{
		AssignDirection(movingBox, movingBox->GroundBox);

		return LEMON_SUCCESS;
	}

	Object *detectedObject;
	detectedObject = GameWorld->ObjectList->firstObject;

	double sinVal = 4 * sin(movingBox->direction * DEGREE_TO_RADIAN_PI);
	double cosVal = 4 * cos(movingBox->direction * DEGREE_TO_RADIAN_PI);


	movingBox->yPos -= sinVal;
	movingBox->xPos += cosVal;

	detectedObject = GetCollidingObject(movingBox, GameWorld->ObjectList);

	movingBox->yPos += sinVal;
	movingBox->xPos -= cosVal;


	if (detectedObject == NULL)
	{
		movingBox->direction = DEFAULT_DIRECTION;

		return LEMON_SUCCESS;
	}

	AssignDirection(movingBox, detectedObject->ObjectBox);


	return LEMON_SUCCESS;
}


int moveObjectForward(Object *input, ObjectController *ObjectList)
{
	PhysicsBox *movingBox = input->ObjectBox;

	if (fabs(movingBox->forwardVelocity) < 0.01)
	{
		return EXECUTION_UNNECESSARY;
	}

	// Unsolid objects do not have to do collision detection so it skips the rest of the function by moving all steps instantly
	if (!LEMON_COLLISION_PHYSICS || movingBox->solid == UNSOLID || !HasPhysics(input))
	{
		movingBox->xPos += movingBox->forwardVelocity * sin(movingBox->direction * DEGREE_TO_RADIAN_PI);
		movingBox->yPos += movingBox->forwardVelocity * cos(movingBox->direction * DEGREE_TO_RADIAN_PI);
		
		return LEMON_SUCCESS;
	}

	return resolveForwardCollision(movingBox, ObjectList);
}

int resolveForwardCollision(PhysicsBox *movingBox, ObjectController *ObjectList)
{
	// step and count set-up
	float orientation = (movingBox->forwardVelocity > 0.0) ? 1.0 : -1.0;
	double sinVal = sin(movingBox->direction * DEGREE_TO_RADIAN_PI);
	double cosVal = cos(movingBox->direction * DEGREE_TO_RADIAN_PI);

	if (fabs(sinVal) < 0.1)
	{
		sinVal = 0.0;
	}

	if (fabs(cosVal) < 0.1)
	{
		cosVal = 0.0;
	}

	float xStep = orientation * sinVal;
	float yStep = orientation * cosVal;
	int travelCount = (int)fabs(movingBox->forwardVelocity);

	int collideCycle;
	float lastStepX, lastStepY;

	// regular collision
	Object *currentObject = NULL;
	depthCounter++;

	while (travelCount > 0)
	{
		collideCycle = COLLISION_CYCLES;
		lastStepX = movingBox->xPos;
		lastStepY = movingBox->yPos;

		movingBox->yPos += yStep;
		movingBox->xPos += xStep;

		currentObject = GetCollidingObject(movingBox, ObjectList);

		while (collideCycle > 0 && currentObject != NULL)
		{
			collideCycle--;

			if (evaluateIfCollidePush(movingBox, currentObject->ObjectBox) && depthCounter < COLLISION_DEPTH)
			{
				PhysicsBox *collideBox = currentObject->ObjectBox;

				float tempVelocity = collideBox->forwardVelocity;
				double tempDirection = collideBox->direction;
				float collideXPos = collideBox->xPos;
				float collideYPos = collideBox->yPos;

				
				collideBox->forwardVelocity = orientation * travelCount;
				collideBox->direction = movingBox->direction;

				moveObjectForward(currentObject, ObjectList);

				collideBox->direction = tempDirection;
				collideBox->forwardVelocity = tempVelocity;

				if (CheckBoxCollidesBox(movingBox, collideBox) == 1)
				{
					movingBox->yPos = lastStepY;
					movingBox->xPos = lastStepX;
					collideBox->xPos = collideXPos;
					collideBox->yPos = collideYPos;

					ApplyForwardPhysics(movingBox, collideBox);

					return TASK_FAILED;
				}
			
			}
			else
			{
				// edge tolerance
				int slopeClimb = 5;

				while (slopeClimb > 0 && CheckBoxCollidesBox(movingBox, currentObject->ObjectBox) == 1)
				{
					movingBox->yPos += sinVal;
					movingBox->xPos -= cosVal;
					slopeClimb--;
				}

				if (CheckBoxCollidesBox(movingBox, currentObject->ObjectBox) == 1)
				{
					movingBox->yPos = lastStepY;
					movingBox->xPos = lastStepX;

					ApplyForwardPhysics(movingBox, currentObject->ObjectBox);

					return LEMON_SUCCESS;
				}
				
			}
			

			currentObject = GetCollidingObject(movingBox, ObjectList);
		}

		travelCount--;
	}

	return LEMON_SUCCESS;
}



int ApplyForwardPhysics(PhysicsBox *inputBox, PhysicsBox *physicsBox)
{
	if (inputBox == NULL || physicsBox == NULL)
	{
		return MISSING_DATA;
	}

	double sinVal = sin(inputBox->direction * DEGREE_TO_RADIAN_PI);
	double cosVal = cos(inputBox->direction * DEGREE_TO_RADIAN_PI);

	float forwardX = inputBox->forwardVelocity * sinVal;
	float forwardY = inputBox->forwardVelocity * cosVal;

	float physicsBoxXVel = physicsBox->xVelocity;
	float physicsBoxYVel = physicsBox->yVelocity;

	if (fabs(physicsBox->forwardVelocity) > 0.1)
	{
		physicsBoxXVel += physicsBox->forwardVelocity * sin(physicsBox->direction * DEGREE_TO_RADIAN_PI);
		physicsBoxYVel += physicsBox->forwardVelocity * cos(physicsBox->direction * DEGREE_TO_RADIAN_PI);
	}

	float newVelocity = 0.0;

	if (fabs(physicsBoxXVel) > 0.01 && fabs(sinVal) > 0.001 && (physicsBoxXVel > 0.1) == (forwardX > 0.1))
	{
		newVelocity += physicsBoxXVel * sinVal;
	}

	if (fabs(physicsBoxYVel) > 0.01 && fabs(cosVal) > 0.001 && (physicsBoxYVel > 0.1) == (forwardY > 0.1))
	{
		newVelocity += physicsBoxYVel * cosVal;
	}


	inputBox->forwardVelocity = newVelocity;

	return LEMON_SUCCESS;
}


int ResolveAllXCollision(PhysicsBox *movingBox, ObjectController *ObjectList)
{
	if (!LEMON_COLLISION_PHYSICS || movingBox == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}


	int count = 0;

	Object *currentObject = GetCollidingObject(movingBox, ObjectList);

	while (currentObject != NULL && count < COLLISION_CYCLES)
	{
		if (evaluateIfCollidePush(movingBox, currentObject->ObjectBox))
		{
			PhysicsBox *collideBox = currentObject->ObjectBox;

			float prevXPos = collideBox->xPos;
			float prevYPos = collideBox->yPos;

			ResolveAllXCollision(collideBox, ObjectList);

			if (GetCollidingObject(collideBox, ObjectList) == NULL)
			{
				collideBox->xPos = prevXPos;
				collideBox->yPos = prevYPos;
				ResolveXCollisionByPush(movingBox, collideBox);
			}
			else
			{
				collideBox->xPos = prevXPos;
				collideBox->yPos = prevYPos;
				ResolveXCollision(movingBox, collideBox, ObjectList);
			}
		}
		else
		{
			ResolveXCollision(movingBox, currentObject->ObjectBox, ObjectList);
		}
		

		currentObject = GetCollidingObject(movingBox, ObjectList);

		count++;
	}


	return LEMON_SUCCESS;
}


int ResolveXCollision(PhysicsBox *movingBox, PhysicsBox *compareBox, ObjectController *ObjectList)
{
	if (movingBox == NULL || compareBox == NULL || compareBox == movingBox)
	{
		return MISSING_DATA;
	}


	int objXRight = compareBox->xPos + compareBox->xSize;
	int ObjXCenter = compareBox->xPos + (compareBox->xSize >> 1);

	int prevXPosInt = movingBox->prevXPos;


	switch(compareBox->solid)
	{
		// Y = X * (ySize/xSize)
		case FLAT_SLOPE:
		{
			if (compareBox->xFlip == 1)
			{
				if (prevXPosInt >= objXRight)
				{
					movingBox->xPos = compareBox->xPos + compareBox->xSize;
					ApplyXPhysics(movingBox, compareBox);
					return LEMON_SUCCESS;
				}
			}
			else
			{
				if (prevXPosInt + movingBox->xSize <= compareBox->xPos)
				{
					movingBox->xPos = compareBox->xPos - movingBox->xSize;
					ApplyXPhysics(movingBox, compareBox);
					return LEMON_SUCCESS;
				}
			}

			float slope = ((float)compareBox->ySize/(float)compareBox->xSize);
			float slopeFloor;

			if (compareBox->xFlip == 1)
			{
				slopeFloor = (movingBox->xPos + movingBox->xSize - compareBox->xPos) * slope;
			}
			else
			{
				slopeFloor = (compareBox->xSize - movingBox->xPos + compareBox->xPos) * slope;
			}

			slopeFloor = fClamp(slopeFloor, 0.0, compareBox->ySize);

			if (compareBox->yFlip == -1)
			{
				slopeFloor = compareBox->ySize - slopeFloor - movingBox->ySize;
			}

			movingBox->yPos = slopeFloor + compareBox->yPos; 


			if (GetCollidingObject(movingBox, ObjectList) != NULL)
			{
				ClimbSlope(movingBox, ObjectList);
			}
		} break;


		case JUMP_THROUGH:
		case UNSOLID:
			break;


		default:
		{
			if (prevXPosInt < ObjXCenter)
			{
				movingBox->xPos = compareBox->xPos - movingBox->xSize;
			}
			else
			{
				movingBox->xPos = compareBox->xPos + compareBox->xSize;
			}
			
			ApplyXPhysics(movingBox, compareBox);
		
		} break;
	}


	return LEMON_SUCCESS;
}


int ApplyXPhysics(PhysicsBox *inputBox, PhysicsBox *physicsBox)
{
	if (inputBox == NULL || physicsBox == NULL)
	{
		return MISSING_DATA;
	}

	if ((inputBox->xVelocity > 0.0) == (physicsBox->xVelocity > 0.0))
	{
		inputBox->xVelocity = physicsBox->xVelocity;
	}
	else
	{
		inputBox->xVelocity = 0.0;
	}

	inputBox->PhysicsXVelocity = 0.0;


	return LEMON_SUCCESS;
}


int ResolveAllYCollision(PhysicsBox *movingBox, ObjectController *ObjectList)
{
	if (!LEMON_COLLISION_PHYSICS || movingBox == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}
	

	int count = 0;
	Object *currentObject = GetCollidingObject(movingBox, ObjectList);

	while (currentObject != NULL && count < COLLISION_CYCLES)
	{
		if (evaluateIfCollidePush(movingBox, currentObject->ObjectBox))
		{
			PhysicsBox *collideBox = currentObject->ObjectBox;

			float prevXPos = collideBox->xPos;
			float prevYPos = collideBox->yPos;

			ResolveAllYCollision(collideBox, ObjectList);

			if (GetCollidingObject(collideBox, ObjectList) == NULL)
			{
				collideBox->xPos = prevXPos;
				collideBox->yPos = prevYPos;
				ResolveYCollisionByPush(movingBox, collideBox);
			}
			else
			{
				collideBox->xPos = prevXPos;
				collideBox->yPos = prevYPos;
				ResolveYCollision(movingBox, collideBox);
			}
		}
		else
		{
			ResolveYCollision(movingBox, currentObject->ObjectBox);
		}

		currentObject = GetCollidingObject(movingBox, ObjectList);

		count++;
	}


	return LEMON_SUCCESS;
}


int ResolveYCollision(PhysicsBox *movingBox, PhysicsBox *compareBox)
{
	if (movingBox == NULL || compareBox == NULL || movingBox == compareBox)
	{
		return MISSING_DATA;
	}


	int objY = compareBox->yPos;
	int ObjYCenter = compareBox->yPos + (compareBox->ySize >> 1);

	int prevYPosInt = movingBox->prevYPos;

	switch(compareBox->solid)
	{
		case FLAT_SLOPE:
		{
			if (compareBox->yFlip == 1 && prevYPosInt + movingBox->ySize < objY)
			{
				movingBox->yPos = (compareBox->yPos - movingBox->ySize);
				break;
			}
			else if (compareBox->yFlip == -1 && prevYPosInt >= objY + compareBox->ySize)
			{
				movingBox->yPos = compareBox->yPos + compareBox->ySize;
				break;
			}
		

			// If movingBox is halfway off edge, floor of slope continues to be calculated as Y = X * slope
			// So here it is reset to the expected maximum if it over
			float slope = ((float)compareBox->ySize/(float)compareBox->xSize);
			float slopeFloor;

			if (compareBox->xFlip == 1)
			{
				slopeFloor = (movingBox->xPos + movingBox->xSize - compareBox->xPos) * slope;
			}
			else
			{
				slopeFloor = (compareBox->xSize - movingBox->xPos + compareBox->xPos) * slope;
			}

			slopeFloor = clamp(slopeFloor, 0, compareBox->ySize);

			if (compareBox->yFlip == -1)
			{
				slopeFloor = compareBox->ySize - slopeFloor - movingBox->ySize;
			}

			movingBox->yPos = slopeFloor + compareBox->yPos; 		
		} break;


		case JUMP_THROUGH:
		{
			if (movingBox->yVelocity < compareBox->yVelocity + 0.001 && movingBox->crouch == false)
			{
				movingBox->yPos = compareBox->yPos + compareBox->ySize;
			}

		} break;


		case UNSOLID:
			break;


		default:
		{
			if (prevYPosInt < ObjYCenter)
			{
				movingBox->yPos = compareBox->yPos - movingBox->ySize;
			}
			else
			{
				movingBox->yPos = compareBox->yPos + compareBox->ySize;
			}

		} break;

	}


	ApplyYPhysics(movingBox, compareBox);

	return LEMON_SUCCESS;
}


int ApplyYPhysics(PhysicsBox *inputBox, PhysicsBox *physicsBox)
{
	if (inputBox == NULL || physicsBox == NULL)
	{
		return MISSING_DATA;
	}

	if ((inputBox->yVelocity > 0.0) == (physicsBox->yVelocity > 0.0))
	{
		inputBox->yVelocity = physicsBox->yVelocity;
	}
	else 
	{	
		inputBox->yVelocity = 0.0;
	}

	inputBox->PhysicsYVelocity = 0.0;

	return LEMON_SUCCESS;
}


int ResolveXCollisionByPush(PhysicsBox *movingBox, PhysicsBox *compareBox)
{
	if (movingBox == NULL || compareBox == NULL || movingBox == compareBox)
	{
		return MISSING_DATA;
	}


	float ObjXPos = movingBox->xPos;
	float ObjXPosRight = movingBox->xPos + movingBox->xSize;
	float ObjYPos = movingBox->yPos;

	float prevXPosRight = movingBox->prevXPos + movingBox->xSize;
	float prevXPosCenter = movingBox->prevXPos + (movingBox->xSize >> 1);

	switch(movingBox->solid)
	{
		case FLAT_SLOPE:
			if (movingBox->xFlip == 1)
			{
				int slopeLeftEdge = (int)((compareBox->yPos - ObjYPos) / ((float)movingBox->ySize/(float)movingBox->xSize));
				slopeLeftEdge = clamp(slopeLeftEdge, 0, movingBox->xSize);

				ObjXPos = slopeLeftEdge + ObjXPos;
				prevXPosCenter = prevXPosRight;
			}
			else
			{
				int slopeLeftEdge = (int)(movingBox->xSize - ((compareBox->yPos - ObjYPos) / ((float)movingBox->ySize/(float)movingBox->xSize)) );
				slopeLeftEdge = clamp(slopeLeftEdge, 0, movingBox->xSize);

				ObjXPosRight = slopeLeftEdge + ObjXPos;
				prevXPosCenter = movingBox->prevXPos;
			}
			break;


		default:
			break;
	}


	if ((int)compareBox->xPos < (int)prevXPosCenter)
	{
		compareBox->xPos = ObjXPos - compareBox->xSize;
	}
	else
	{
		compareBox->xPos = ObjXPosRight;
	}

	return LEMON_SUCCESS;
}


int ResolveAllXCollisionsByPush(PhysicsBox *movingBox, ObjectController *ObjectList)
{
	if (movingBox == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}


	Object *collideObject = GetCollidingObject(movingBox, ObjectList);
	int i = 0;

	while (collideObject != NULL && i < 16)
	{
		ResolveXCollisionByPush(movingBox, collideObject->ObjectBox);

		collideObject = GetCollidingObject(movingBox, ObjectList);	
		i++;
	}


	return LEMON_SUCCESS;
}


int ResolveAllYCollisionsByPush(PhysicsBox *movingBox, ObjectController *ObjectList)
{
	if (movingBox == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}


	Object *collideObject = GetCollidingObject(movingBox, ObjectList);
	int i = 0;

	while (collideObject != NULL && i < 16)
	{
		ResolveYCollisionByPush(movingBox, collideObject->ObjectBox);

		collideObject = GetCollidingObject(movingBox, ObjectList);	
		i++;
	}


	return LEMON_SUCCESS;
}


int ResolveYCollisionByPush(PhysicsBox *movingBox, PhysicsBox *compareBox)
{
	if (movingBox == NULL || compareBox == NULL || movingBox == compareBox)
	{
		return MISSING_DATA;
	}

	float ObjYPos = movingBox->yPos;
	float ObjYPosTop = movingBox->yPos + movingBox->ySize;

	float prevYCenter = movingBox->prevYPos + (movingBox->ySize >> 1);


	switch(movingBox->solid)
	{
		case FLAT_SLOPE:
			if (movingBox->xFlip == 1)
			{
				int slopeFloor = (int)( ((compareBox->xPos + compareBox->xSize - movingBox->xPos) * ((float)movingBox->ySize/(float)movingBox->xSize)) );
				slopeFloor = clamp(slopeFloor, 0, movingBox->ySize);

				ObjYPosTop = slopeFloor + ObjYPos;
			}
			else
			{
				int slopeFloor = (int)( ((movingBox->xSize - (compareBox->xPos - movingBox->xPos)) * ((float)movingBox->ySize/(float)movingBox->xSize)) );
				slopeFloor = clamp(slopeFloor, 0, movingBox->ySize);

				ObjYPosTop = slopeFloor + ObjYPos;
			}
			
			prevYCenter = movingBox->prevYPos;
			break;


		default:
			break;
	}

	
	if (compareBox->yPos < prevYCenter)
	{
		compareBox->yPos = ObjYPos - compareBox->ySize;
	}
	else
	{
		compareBox->yPos = ObjYPosTop; 
	}

	return LEMON_SUCCESS;
}


int ClimbSlope(PhysicsBox *inputBox, ObjectController *ObjectList)
{		
	if (inputBox == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	float savedForwardVelocity = inputBox->forwardVelocity;
	double savedDirection = inputBox->direction;

	float velocity = inputBox->xVelocity + inputBox->PhysicsXVelocity;

	if (fabs(velocity) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}

	inputBox->direction = DEFAULT_DIRECTION;
	inputBox->forwardVelocity = velocity;

	resolveForwardCollision(inputBox, ObjectList);

	inputBox->forwardVelocity = savedForwardVelocity;
	inputBox->direction = savedDirection;

	return LEMON_SUCCESS;
}
