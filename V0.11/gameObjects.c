#include "LemonEngine.h"


Object* AddObject(World *GameWorld, int objectID, int xPos, int yPos, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	ObjectController *ObjectList = &GameWorld->ObjectList;

	if (ObjectList->objectCount + ObjectList->cachedCount >= EngineSettings.MaxObjects)
	{
		return NULL;
	}

	if (objectID >= OBJECT_TYPE_COUNT || objectID < LEVEL_FLAG_OBJ)
	{
		putConsole("\nThis object is not defined! Type: %d", objectID);
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
		putConsoleTS("Created object ---- ID: %d (%s) ", objectID, getObjectIDName(objectID));
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
		newObject->ObjectBox->ySize = 12;
		newObject->ObjectBox->xSize = 12;

		newObject->ObjectBox->forwardVelocity = 20.0;

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
		addTileMap(newObject, 32, 32, 32, GameWorld);
		
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
		newObject->ObjectBox->shape = FLAT_SLOPE;
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
		addPhysics(newObject, false, GameWorld);
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
		InitialiseMovingPlatform(newObject, arg1, arg2, arg3, arg4, GameWorld);
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
		addHealthComponent(newObject, 1000, GROUP_HOSTILE, GameWorld);
		addPhysics(newObject, true, GameWorld);
		break;


	case PUSHABLE_BOX:
		newObject->ObjectBox->solid = PUSHABLE_SOLID;
		newObject->ObjectBox->xSize = arg1;
		newObject->ObjectBox->ySize = arg2;
		addPhysics(newObject, true, GameWorld);
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
		putConsoleTS("Created object ---- ID: %d (%s)  Named: %s", objectID, getObjectIDName(objectID), name);
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

Object* AddObjectFromMeta(World *GameWorld, ObjectMeta input)
{
	return AddNamedObject(GameWorld, input.name, input.objectID, input.xPos, input.yPos);
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
		putConsole("as clone of %s", input->name);
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

	inputObject->instanceNumber++;	// counts how many times this slot was used

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
		putConsole("\nError: Could not allocate memory for new object.\n");
		return NULL;
	}

	newObject->ObjectBox = createPhysicsBox(SOLID);

	if (newObject->ObjectBox == NULL)
	{
		putConsole("\nError: Could not allocate memory for new object's physics box.\n");
		free(newObject);
		return NULL;
	}

	newObject->ObjectDisplay = createDisplayData(DEFAULT_TO_SPRITE);

	if (newObject->ObjectDisplay == NULL)
	{
		putConsole("\nError: Could not allocate memory for new object's display data.\n");
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

	input->shape = RECTANGLE;
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
	input->currentFrame = 0;
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
	if (input == NULL || input->ObjectDisplay == NULL)
	{
		return false;
	}

	input->ObjectDisplay->layer = newLayer;

	return true;
}

RenderMode getRenderModeOverride(Object *input)
{
	if (input == NULL)
	{
		return UNDEFINED_RENDERMODE;
	}

	return input->ObjectDisplay->RenderModeOverride;
}

RenderMode getRenderMode(Object *input)
{
	if (input == NULL)
	{
		return UNDEFINED_RENDERMODE;
	}
	DisplayData *display = input->ObjectDisplay;

	if (display->spriteBuffer == NULL || display->RenderModeOverride != DEFAULT_TO_SPRITE)
	{
		return display->RenderModeOverride;
	}

	return display->spriteBuffer->RenderMode;
}

bool setRenderModeOverride(Object *input, RenderMode newMode)
{
	if (input == NULL)
	{
		return false;
	}

	input->ObjectDisplay->RenderModeOverride = newMode;

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
	if (input == NULL)
	{
		return false;
	}

	input->ObjectDisplay->transparency = fClamp(transparency, 0.0, 1.0);

	return true;
}

bool changeTransparency(Object *input, float transparency)
{
	if (input == NULL)
	{
		return false;
	}

	input->ObjectDisplay->transparency = fClamp(input->ObjectDisplay->transparency + transparency, 0.0, 1.0);

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
	if (input == NULL)
	{
		return false;
	}

	input->ObjectDisplay->hidden = true;

	return true;
}

bool showObject(Object *input)
{
	if (input == NULL)
	{
		return false;
	}
	
	input->ObjectDisplay->hidden = false;

	return true;
}

bool toggleHidden(Object *input)
{
	if (input == NULL)
	{
		return false;
	}

	DisplayData *display = input->ObjectDisplay;

	display->hidden = !display->hidden;

	return true;
}

bool setRotateMode(Object *input, RotationMode mode)
{
	if (input == NULL)
	{
		return false;
	}

	input->ObjectDisplay->rotateMode = mode;

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
		putConsoleTS("Deleted object ---- ID: %d (%s)  Name: '%s'", input->ObjectID, getObjectIDName(input->ObjectID), input->name);
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

	if (inputObject->State == TO_BE_DELETED || inputObject->State == EMPTY_OBJECT)
	{
		return INVALID_DATA;
	}

	inputObject->State = TO_BE_DELETED;

	return LEMON_SUCCESS;
}

int MarkObjectInstanceForDeletion(Object *inputObject, int instance)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	if (inputObject->State == TO_BE_DELETED || inputObject->State == EMPTY_OBJECT || inputObject->instanceNumber != instance)
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
		putConsole("Unmarked '%s' for deletion! ---- ID: %d (%s)", 
		inputObject->name, inputObject->ObjectID, getObjectIDName(inputObject->ObjectID));
	}


	return LEMON_SUCCESS;
}

bool objectDeleted(Object *input, int instance)
{
	if (input == NULL)
	{
		return true;
	}

	return (input->State == EMPTY_OBJECT || input->instanceNumber != instance);
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


static const char ObjectNames[OBJECT_TYPE_COUNT][64] = {
	[LEVEL_FLAG_OBJ] = "LevelFlag",
	[SOLID_BLOCK] = "SolidBlock",
	[FLAT_SLOPE_FLOOR] = "FlatSlopeFloor",
	[JUMP_THRU_BLOCK] = "JumpThroughPlatform",
	[PLAYER_OBJECT] = "PlayerObject",
	[UI_ELEMENT] = "UIElement",
	[UI_TEXT] = "UIText",
	[PARTICLE] = "Particle",
	[COIN] = "Coin",
	[SPRING] = "Spring",
	[MOVING_PLATFORM_HOR] = "MovingPlatform_Horizontal",
	[MOVING_PLATFORM_VER] = "MovingPlatform_Vertical",
	[GATE_SWITCH] = "GateSwitch",
	[GATE_SWITCH_TIMED] = "GateSwitchTimed",
	[VERTICAL_GATE] = "VerticalGate",
	[HORIZONTAL_GATE] = "HorizontalGate",
	[DOOR] = "Door",
	[LEVEL_DOOR] = "LevelDoor",
	[PUSHABLE_BOX] = "PushableBox",
	[PROJECTILE] = "Projectile",
	[BASIC_ENEMY] = "BasicEnemy"
};

int getObjectID(const char entry[])
{
	if (entry[0] >= '0' && entry[0] <= '9')
	{
		return convertStrToInt(entry, 6);
	}

	char comparison[64] = {0};
	char input[64] = {0};

	LemonStrncpy(input, entry, 64);
	stringToLower(input);

	for (int i = 0; i < OBJECT_TYPE_COUNT; i++)
	{
		strcpy(comparison, ObjectNames[i]);
		stringToLower(comparison);

		if (strcmp(input, comparison) == 0)
		{
			return i;
		}
	}	

	return UNDEFINED_OBJECT;
}

const char* getObjectIDName(ObjectType input)
{
	if (input >= OBJECT_TYPE_COUNT && input < 0)
	{
		return "Undefined";
	}

	return ObjectNames[input];
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

const char* getSolidShapeName(SolidShape input)
{
	switch(input)
	{
	case RECTANGLE:
		return "Rectangle";

	case FLAT_SLOPE:
		return "Flat Slope";

	case CIRCLE:
		return "Circle";

	default:
		return "Undefined";
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

	case JUMP_THROUGH:
		return "Jump-Through Solid";

	case PUSHABLE_SOLID:
		return "Pushable Solid";

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
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState <= EMPTY_GAME || GameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}

	ObjectController *ObjectList = &GameWorld->ObjectList;

	if (ObjectList->firstObject == NULL)
	{
		return MISSING_DATA;
	}

	updateComponents(GameWorld);

	Object *currentObject = ObjectList->firstObject;
	while(currentObject != NULL)
	{
		ObjectBehaviour(GameWorld, currentObject);
		
		currentObject = currentObject->nextObject;
	}

	// Update object state - parent-child links, deletion, etc.
	updateObjectsState(ObjectList, GameWorld);

	return LEMON_SUCCESS;
}


#define cannotUpdateObject(x) (x->State == STATIC_STATE || x->State < DEFAULT_STATE || (GameWorld->GamePaused != 0 && x->ObjectID != UI_ELEMENT && Networking.connectMode == OFFLINE))

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

	bool gameStateDisable = inputObject->State == ACTOR_STATE || (GameWorld->GameState == CUTSCENE); 
	bool immuneObject = (inputObject->reserved & RFLAG_CUTSCENE_IMMUNITY) != 0;

	if ((!immuneObject && gameStateDisable) || inputObject->State == PAUSE_STATE)
	{
		return ACTION_DISABLED;
	}

	inputObject->reserved ^= RFLAG_DISABLE_PHYSICS;
	

	#ifndef LEMON_USE_CUSTOM_CALLBACKS
	if (inputObject == GameWorld->Player.PlayerPtr)
	{
		UpdatePlayer(GameWorld);
	}

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
			UpdateFlagObject(inputObject, GameWorld);
			break;


		case MOVING_PLATFORM_HOR:
			UpdateHorizontalPlatform(inputObject, GameWorld);
		 	break;


		case MOVING_PLATFORM_VER:
			UpdateVerticalPlatform(inputObject, GameWorld);
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
			UpdateGateSwitch(inputObject, GameWorld);
		break;


		case DOOR:
			UpdateDoor(inputObject, GameWorld);
			break;


		case LEVEL_DOOR:
			UpdateLevelDoor(inputObject, GameWorld);
			break;


		case BASIC_ENEMY:
			ApplyFriction(inputObject->ObjectBox, 1.0, 1.0, 1.0);
			break;


		case PUSHABLE_BOX:
			ApplyFriction(inputObject->ObjectBox, 0.9, 0.9, 1.0);

			PhysicsBox *PlayerBox = GameWorld->Player.PlayerPtr->ObjectBox;

			if (PlayerInteractingWithBox(inputObject->ObjectBox, GameWorld) && PlayerBox != NULL)
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
	Object *currentObject = GameWorld->ObjectList.firstObject;

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
	if (inputObject == NULL)	{ return MISSING_DATA; }

	DisplayData *inputDisplay = inputObject->ObjectDisplay;
	
	iterateAnimation(inputDisplay, deltaTime);

	// Assign Sprite   
	if (inputDisplay->currentSprite > 0 && (inputDisplay->spriteBuffer == NULL || inputDisplay->currentSprite != inputDisplay->spriteBuffer->spriteID))
	{
		switchSprite(inputDisplay->currentSprite, USE_CURRENT_SPRITESET, inputDisplay);
	}

	return LEMON_SUCCESS;
}


void updatePreviousPositions(ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return;
	}

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

			objList[i].reserved &= ~RFLAG_GROUND_SET;
			objList[i].ParentLink &= PARENTLINK_MASK;

			if (boxList[i].solid != UNSOLID)
			{
				stackAdd(i, &ObjectList->solidList);
			}
		}

		i++;
	}

	return;
}

int updatePreviousPosition(Object *input)
{
	input->ObjectBox->prevXPos = input->ObjectBox->xPos;
	input->ObjectBox->prevYPos = input->ObjectBox->yPos;

	input->reserved &= ~RFLAG_GROUND_SET;
	input->ParentLink &= PARENTLINK_CONFIRM;

	return LEMON_SUCCESS;
}


int updateObjectsState(ObjectController *ObjectList, World *GameWorld)
{
	Object *current = ObjectList->firstObject;

	while (current != NULL)
	{	
		UpdateParentChildLink(current);	

		if (current->State == TO_BE_DELETED)
		{
			if (GameWorld->Player.PlayerPtr == current)
			{
				PlayerObjectAboutToBeDeleted(&GameWorld->Player);
			}

			current = deleteObject(current, ObjectList);
		}
		else
		{
			UpdatePhysicsState(current, GameWorld);

			current = current->nextObject;
		}
	}

	ResolveAllObjects(GameWorld);


	return LEMON_SUCCESS;
}


int UpdatePhysicsState(Object *inputObject, World *GameWorld)
{
	if (!LEMON_COLLISION_PHYSICS || inputObject->State == STATIC_STATE || inputObject->State < DEFAULT_STATE)
	{
		return ACTION_DISABLED;
	}

	if (!HasPhysics(inputObject, GameWorld))
	{
		return EXECUTION_UNNECESSARY;
	}

	PhysicsBox *inputBox = inputObject->ObjectBox;

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

	// basic operation for magnetisation is
	// update previous positions
	// all objects apply gravity/check for ground
	// all objects update and move with velocity
	// all objects with detected ground move along with ground

	
	// Ensure that velocity applied is not necessary in the case of it moving against gravity
	float prevX = inputBox->xPos;
	float prevY = inputBox->yPos;

	float xChange = (GroundBox->xPos) - (GroundBox->prevXPos);
	float yChange = (GroundBox->yPos) - (GroundBox->prevYPos);

	inputBox->xPos -= xChange;

	if (!CheckBoxCollidesBox(inputBox, GroundBox))
	{	
		inputBox->PhysicsXVelocity = xChange;
	}
	else
	{
		inputBox->PhysicsXVelocity *= 0.8;
	}

	inputBox->xPos = prevX;
	inputBox->yPos -= yChange;

	if (!CheckBoxCollidesBox(inputBox, GroundBox))
	{	
		inputBox->PhysicsYVelocity = yChange;
	}
	else
	{
		inputBox->PhysicsYVelocity *= 0.8;
	}

	inputBox->yPos = prevY;

	
	if (fabs(inputBox->PhysicsXVelocity) < 0.0001)
	{
		inputBox->PhysicsXVelocity = 0.0;
	}

	if (fabs(inputBox->PhysicsYVelocity) < 0.0001)
	{
		inputBox->PhysicsYVelocity = 0.0;
	}

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

	if ( (inputObject->ParentLink & HIDDEN_LINK) != 0)
	{
		InputDisplay->hidden = ParentDisplay->hidden;
	}

	return LEMON_SUCCESS;
}


int ResolveAllObjects(World *GameWorld)
{
	if (!LEMON_COLLISION_PHYSICS || GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->PhysicsType != PLATFORMER)
	{
		return ACTION_DISABLED;
	}

	ObjectController *ObjectList = &GameWorld->ObjectList;

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
			ResolveAllXCollision(&boxList[boxIndex], GameWorld);
			boxList[boxIndex].yPos = savedPos;
		}

		if (fabs(boxList[boxIndex].PhysicsYVelocity) > 0.1)
		{	
			ResolveAllYCollision(&boxList[boxIndex], GameWorld);
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
#define initComponentType(x) 		initialiseSparseList(&ObjectList->objectComponents.x, #x)
#define removeComponentType(x, y) 	removeComponent(x, &GameWorld->ObjectList.objectComponents.y)
#define addComponentType(x, y) 		(y *)addComponent(x, &GameWorld->ObjectList.objectComponents.y)
#define getComponentType(x, y) 		(y *)getComponent(x, &GameWorld->ObjectList.objectComponents.y)
#define hasComponentType(x, y)		(GameWorld->ObjectList.objectComponents.y.sparse[x->index] >= 0)


int initialiseComponents(ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	// initialise new components here
	initComponentType(HealthComponent);
	initComponentType(BulletComponent);
	initComponentType(TileMap);
	initComponentType(Timer);
	initComponentType(StopWatch);
	initComponentType(PhysicsComponent);
	initComponentType(Polygon);
	initComponentType(ObjectEvent);

	return LEMON_SUCCESS;
}

int removeComponents(Object *input, ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	// remove new components here
	removeComponent(input, &ObjectList->objectComponents.HealthComponent);
	removeComponent(input, &ObjectList->objectComponents.BulletComponent);
	removeComponent(input, &ObjectList->objectComponents.TileMap);
	removeComponent(input, &ObjectList->objectComponents.Timer);
	removeComponent(input, &ObjectList->objectComponents.StopWatch);
	removeComponent(input, &ObjectList->objectComponents.PhysicsComponent);
	removeComponent(input, &ObjectList->objectComponents.Polygon);
	removeComponent(input, &ObjectList->objectComponents.ObjectEvent);

	return LEMON_SUCCESS;
}


void initialiseSparseList(SparseList *input, const char name[])
{
	strcpy(input->name, name);

	// -1 is tombstone value (empty slot)
	for (int i = 0; i < EngineSettings.MaxObjects; i++)
	{
		input->sparse[i] = -1;
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
	if (input == NULL || List == NULL)
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

	if (strcmp(List->name, "Polygon") == 0)
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
	else if (strcmp(List->name, "ObjectEvent") == 0)
	{
		ObjectEvent *event = &denseList[denseIndex].ObjectEvent;
		if (event->event != NULL)
		{
			free(event->event);
			event->event = NULL;
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


int updateComponents(World *GameWorld)
{
	updatePhysicsComponents(GameWorld);

	return LEMON_SUCCESS;
}


GameEvent* addObjectEvent(Object *input, bool triggerOnce, World *GameWorld)
{
	ObjectEvent *newEvent = getComponentType(input, ObjectEvent);

	if (newEvent != NULL)
	{
		newEvent->triggerOnce = triggerOnce;
		return newEvent->event;
	}

	newEvent = addComponentType(input, ObjectEvent);

	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->event = malloc(sizeof(GameEvent));
	if (newEvent->event == NULL)
	{
		removeComponentType(input, ObjectEvent);
		return NULL;
	}

	memset(newEvent->event, 0, sizeof(GameEvent));
	newEvent->triggerOnce = triggerOnce;

	return newEvent->event;
}

GameEvent* getObjectEvent(Object *input, World *GameWorld)
{
	ObjectEvent *newEvent = getComponentType(input, ObjectEvent);

	if (newEvent == NULL)
	{
		return NULL;
	}

	return newEvent->event;
}

bool hasObjectEvent(Object *input, World *GameWorld)
{
	return hasComponentType(input, ObjectEvent);
}

void triggerObjectEvent(Object *input, World *GameWorld)
{
	ObjectEvent *event = getComponentType(input, ObjectEvent);

	if (event == NULL || event->event == NULL)
	{
		return;
	}

	// this client has triggered the event, so put your own clientID here
	event->event->clientID = Networking.clientID;	
	triggerGameEvent(event->event, GameWorld);

	if (event->triggerOnce)
	{	
		removeComponentType(input, ObjectEvent);
	}

	return;
}


PhysicsComponent* addPhysics(Object *input, bool gravity, World *GameWorld)
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

PhysicsComponent* addPhysicsDefault(Object *input, World *GameWorld)
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

PhysicsComponent* getPhysicsComponent(Object *input, World *GameWorld)
{
	return getComponentType(input, PhysicsComponent);
}

bool HasPhysics(Object *input, World *GameWorld)
{
	if (input == NULL || (input->reserved & RFLAG_DISABLE_PHYSICS) != 0)
	{
		return false;
	}

	return hasComponentType(input, PhysicsComponent);
}

bool HasGravity(Object *input, World *GameWorld)
{
	PhysicsComponent *myPhys = getComponentType(input, PhysicsComponent);

	if (myPhys)
	{
		return myPhys->gravity;
	}

	return false;
}

void SetPhysicsGravity(Object *input, bool gravity, World *GameWorld)
{
	PhysicsComponent *myPhys = getComponentType(input, PhysicsComponent);

	if (myPhys)
	{
		myPhys->gravity = gravity;
	}

	return;
}

void updatePhysicsComponents(World *GameWorld)
{
	if (!LEMON_COLLISION_PHYSICS || GameWorld->PhysicsType != PLATFORMER)
	{
		return;
	}

	SparseList *List = &GameWorld->ObjectList.objectComponents.PhysicsComponent;
	ComponentType *denseList = List->dense;
	PhysicsComponent *phys;

	for (int i = List->storedComponents - 1; i >= 0; i--)
	{
		phys = &denseList[i].PhysicsComponent;

		if (phys->gravity && (phys->object->reserved & RFLAG_DISABLE_PHYSICS) == 0)
		{
			ApplyGravity(phys->object, GameWorld);
		}
	}

	return;
}


Polygon* addPolygon(Object *input, World *GameWorld, int numOfVertices, ...)
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

Polygon* addQuad(Object *input, World *GameWorld)		
{
	float x = (float)input->ObjectBox->xSize;
	float y = (float)input->ObjectBox->ySize;
	// add a polygon that is a box surrounding the sprite, that without modification appears identically to regular sprite rendering, albeit without rotations
	Polygon *new = addPolygon(input, GameWorld, 4, 
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


Polygon* getPolygon(Object *input, World *GameWorld)
{
	return (Polygon *)getComponent(input, &GameWorld->ObjectList.objectComponents.Polygon);
}

void movePolygonVertex(Object *input, int vertex, float newX, float newY, World *GameWorld)
{
	Polygon *poly = getPolygon(input, GameWorld);

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

SDL_Vertex* getPolygonVertex(Object *input, int vertex, World *GameWorld)
{
	Polygon *poly = getPolygon(input, GameWorld);

	if (poly == NULL || vertex < 0 || vertex >= poly->vertices)
	{
		return NULL;
	}

	return &poly->vertexList[vertex];
}

HealthComponent* addHealthComponent(Object *input, int Health, GroupType group, World *GameWorld)
{
	HealthComponent *newHp = addComponentType(input, HealthComponent);

	if (newHp == NULL)
	{
		return NULL;
	}

	newHp->health = Health;
	newHp->maxHealth = Health;
	newHp->hurtDuration = 0;
	newHp->hurtTick = 0;
	newHp->group = group;

	return newHp;
}

HealthComponent* getHealthComponent(Object *input, World *GameWorld)
{
	return getComponentType(input, HealthComponent);
}

GroupType getGroupAffiliation(Object *input, World *GameWorld)
{
	HealthComponent *health = getComponentType(input, HealthComponent);

	if (health == NULL)
	{
		return NO_GROUP;
	}

	return health->group;
}

int inflictDamage(int damage, Object *input, World *GameWorld)
{
	HealthComponent *targetHp = getHealthComponent(input, GameWorld);

	if (targetHp == NULL)
	{
		return MISSING_DATA;
	}

	targetHp->health -= damage;
	if (targetHp->health < 1)
	{
		MarkObjectForDeletion(input);
	}

	targetHp->hurtTick = TickNumber();
	targetHp->hurtDuration = 5;
	PlayObjectAnimation("Hurt", 1, input);

	return LEMON_SUCCESS;
}

bool isHurt(Object *input, World *GameWorld)
{
	HealthComponent *health = getHealthComponent(input, GameWorld);

	if (health == NULL)
	{		
		return false;
	}

	return (TickNumber() < health->hurtTick + health->hurtDuration);
}


BulletComponent* addBulletComponent(Object *input, Object *owner, int damage, ParticleSubType particleType, World *GameWorld)
{
	centerOnObject(input, owner);
	input->ObjectBox->shape = CIRCLE;
	input->ObjectBox->solid = SOLID;
	input->ObjectBox->flag = GET_IGNORED;

	BulletComponent *newBullet = addComponentType(input, BulletComponent);

	if (newBullet == NULL)
	{
		return NULL;
	}

	newBullet->damage = damage;
	newBullet->owner = owner;
	newBullet->particleType = particleType;
	newBullet->particleLifeTime = 0;
	newBullet->bulletCollide = true;
	newBullet->bulletLifeTime = 200;
	newBullet->group = getGroupAffiliation(owner, GameWorld);
	
	return newBullet;
}


bool isBullet(Object *input, World *GameWorld)
{
	if (input == NULL)
	{		
		return false;
	}

	return hasComponentType(input, BulletComponent);
}


void bulletCollision(Object *bulletObject, World *GameWorld)
{
	BulletComponent *bulletInfo = getComponentType(bulletObject, BulletComponent);
	if (bulletInfo == NULL)
	{
		return;
	}

	bulletInfo->bulletLifeTime--;

	if (bulletInfo->bulletLifeTime < 1)
	{
		MarkObjectForDeletion(bulletObject);
		return;
	}

	// search for collisions with any objects that have health attached first
	SparseList *healthData = &GameWorld->ObjectList.objectComponents.HealthComponent;
	HealthComponent *healthList = (HealthComponent *)healthData->dense;

	PhysicsBox *boxes = GameWorld->ObjectList.objectComponents.PhysicsBoxes;
	Object *objects = GameWorld->ObjectList.objectComponents.Objects;
	int index = 0;

	for (int i = 0; i < healthData->storedComponents; i++)
	{
		// If the bullet's affiliation differs from the hit object, or if the bullet has no affiliation, it should deal damage
		if (healthList[i].group == bulletInfo->group && bulletInfo->group != NO_GROUP)
		{
			continue;
		}

		index = healthData->denseID[i];

		// one last safety check to ensure bullet is not hitting the object who spawned it, although you may remove this if you want that functionality
		if (&objects[index] != bulletInfo->owner && CheckBoxOverlapsBox(bulletObject->ObjectBox, &boxes[index]))
		{
			MarkObjectForDeletion(bulletObject);

			centerOnObject(AddParticle(GameWorld, bulletInfo->particleType, 0, 0, 1, bulletInfo->particleLifeTime), bulletObject);

			inflictDamage(bulletInfo->damage, &objects[index], GameWorld);
		}
	}

	// If bullet is supposed to be destroyed on hitting geometry, check for it here
	// If bullet object has physics attached, it is assumed you want the physics system to take over
	if (!bulletInfo->bulletCollide || HasPhysics(bulletObject, GameWorld))
	{
		return;
	}

	Object *hitObject = GetCollidingObject(bulletObject->ObjectBox, &GameWorld->ObjectList);

	if (hitObject == NULL || hitObject == bulletInfo->owner)
	{
		return;
	}	

	MarkObjectForDeletion(bulletObject);

	centerOnObject(AddParticle(GameWorld, bulletInfo->particleType, 0, 0, 1, bulletInfo->particleLifeTime), bulletObject);

	return;
}


int addTileMap(Object *input, int centerTileX, int centerTileY, int tileSize, World *GameWorld)
{
	TileMap *newMap = addComponentType(input, TileMap);

	if (newMap == NULL)
	{
		return MISSING_DATA;
	}

	newMap->centerTileX = (float)clamp(centerTileX, 0, centerTileX);
	newMap->centerTileY = (float)clamp(centerTileY, 0, centerTileY);
	newMap->tileSize = (float)clamp(tileSize, 1, tileSize);

	return LEMON_SUCCESS;
}


TileMap* getTileMap(Object *input, World *GameWorld)
{
	return (TileMap *)getComponent(input, &GameWorld->ObjectList.objectComponents.TileMap);
}


int startTimer(int ticks, Object *input, World *GameWorld)
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

int startTimerSeconds(float seconds, Object *input, World *GameWorld)
{
	return startTimer((int)(seconds * EngineSettings.GameTicksPerSecond), input, GameWorld);
}

bool timerExpired(Object *input, World *GameWorld)
{
	return (checkTimer(input, GameWorld) == 0);
}

// returns time remaining
Uint64 checkTimer(Object *input, World *GameWorld)
{
	const Timer *timer = getComponentType(input, Timer);

	if (timer == NULL)
	{
		return 0;
	}

	Uint64 elapsed = (timer->pause ? timer->pauseTick : TickNumber()) - timer->startTick;

	if (elapsed >= timer->timerLength)
	{
		removeComponentType(input, Timer);
		return 0;
	}

	return timer->timerLength - elapsed;
}

Timer* getTimer(Object *input, World *GameWorld)
{
	return getComponentType(input, Timer);
}

int endTimer(Object *input, World *GameWorld)
{
	return removeComponentType(input, Timer);
}

void pauseTimer(Object *input, World *GameWorld)
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
void unpauseTimer(Object *input, World *GameWorld)
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

int startStopWatch(Object *input, World *GameWorld)
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

float checkStopWatch(Object *input, World *GameWorld)
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

void pauseStopWatch(Object *input, World *GameWorld)
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
void unpauseStopWatch(Object *input, World *GameWorld)
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

float endStopWatch(Object *input, World *GameWorld)
{
	float time = checkStopWatch(input, GameWorld);

	removeComponentType(input, StopWatch);

	return time;
}


FuncResult updateObjectsFrame(World *GameWorld)  
{
	if (GameWorld == NULL || GameWorld->ObjectList.FrameUpdates == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState == EMPTY_GAME || GameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}

	ObjectController *ObjectList = &GameWorld->ObjectList;
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

	if (coinBox == NULL || Player->PlayerPtr == NULL)
	{
		return MISSING_DATA;
	}

	if (checkBoxOverlapsBoxBroad(Player->PlayerPtr->ObjectBox, coin->ObjectBox))
	{
		Player->coinCount++;

		char text[30];
		snprintf(text, 30, "Coin Count: %d", Player->coinCount);
		updateTextWithName("CoinCounter", text, GameWorld);

		AddParticle(GameWorld, SPARKLE, coinBox->xPos + 20 - (rand() % 40), coinBox->yPos + 20 - (rand() % 40), 1, 0);
		MarkObjectForDeletion(coin);
		PlaySound("Objects/Coin_Collect", 0.75, OBJECT_SFX);
	}


	if (buttons[LMN_INTERACT2] || coin->arg1 > 0)
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


	return LEMON_SUCCESS;
}


int UpdateSpring(Object *spring, World *GameWorld)
{
	if (spring == NULL || GameWorld == NULL || GameWorld->Player.PlayerPtr == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsBox *PlayerBox = GameWorld->Player.PlayerPtr->ObjectBox;
	PhysicsBox *springBox = spring->ObjectBox;
	
	if (checkBoxOverlapsBoxBroad(PlayerBox, springBox) && PlayerBox->prevYPos > springBox->yPos + springBox->ySize)
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
		
		PlaySound("Objects/Spring", 1.0, OBJECT_SFX);
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


int UpdateGateSwitch(Object *gateSwitch, World *GameWorld)
{
	if (gateSwitch == NULL)
	{
		return MISSING_DATA;
	}

	// arg1 = switch ID
	// arg2 = switch type (0 = or, 1 = and)
	// Action = switch off/on (0/1) state
	// arg3 = timerLength

	if (PlayerInteractingWithBox(gateSwitch->ObjectBox, GameWorld))
	{
		gateSwitch->Action = (gateSwitch->Action + 1) % 2;

		// Update any gates
		toggleGateSwitch(gateSwitch, &GameWorld->ObjectList);

		if (gateSwitch->Action == 1 && gateSwitch->ObjectID == GATE_SWITCH_TIMED)
		{
			startTimer(gateSwitch->arg3, gateSwitch, GameWorld);
		}
		else
		{
			endTimer(gateSwitch, GameWorld);	
			// end timer is somewhat redundant, basically ensures that after this point timerExpired is true, although that is never checked
		}

		return LEMON_SUCCESS;
	}

	if (gateSwitch->Action == 1 && gateSwitch->ObjectID == GATE_SWITCH_TIMED && timerExpired(gateSwitch, GameWorld))
	{
		gateSwitch->Action = 0;
		toggleGateSwitch(gateSwitch, &GameWorld->ObjectList);
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
	
	PlaySound("Objects/GateSwitchToggle", 1.0, OBJECT_SFX);

	return LEMON_SUCCESS;
}

int UpdateVerticalGate(Object *gate, World *GameWorld)
{
	// arg1 = gate ID
	// arg2 = speed
	// Action = gate close/open (0/1)
	// arg3 = closed gate y position

	if (gate == NULL || GameWorld == NULL)
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
				PlaySound("Objects/GateClose", 1.0, OBJECT_SFX);
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
				PlaySound("Objects/GateOpen", 1.0, OBJECT_SFX);
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
	
	if (gate == NULL || GameWorld == NULL)
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
				PlaySound("Objects/GateClose", 1.0, OBJECT_SFX);
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
				PlaySound("Objects/GateOpen", 1.0, OBJECT_SFX);
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


Object* InitialiseMovingPlatform(Object *inputObject, int bound1, int bound2, int speed, int timer, World *GameWorld)
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
	inputObject->ObjectBox->solid = SOLID;
	inputObject->ObjectBox->flag = ONLY_BODIES;
	addPhysics(inputObject, false, GameWorld);


	return inputObject;
}


int UpdateHorizontalPlatform(Object *platform, World *GameWorld)
{
	PhysicsBox *platformBox = platform->ObjectBox;
	int XPos = platformBox->xPos;
	int XPos2 = platformBox->xPos + platformBox->xSize;

	int leftBound = platform->arg1;
	int rightBound = platform->arg2;
	float maxSpeed = platform->arg3;


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
		startTimer(platform->arg4, platform, GameWorld);
	}

	// Wait to change direction
	if (platform->Action == 0)
	{
		ApplyFriction(platformBox, 1.0, 0.9, 1.0);

		if (timerExpired(platform, GameWorld) && fabs(platformBox->xVelocity) < 0.01)
		{
			platform->Action = (XPos <= leftBound) ? 1 : 2;
		}
	}

	return LEMON_SUCCESS;
}



int UpdateVerticalPlatform(Object *platform, World *GameWorld)
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
		startTimer(platform->arg4, platform, GameWorld);
	}

	// Wait to change direction
	if (platform->Action == 0)
	{
		ApplyFriction(platformBox, 1.0, 1.0, 0.9);

		if (timerExpired(platform, GameWorld) && fabs(platformBox->yVelocity) < 0.01)
		{
			platform->Action = (YPos <= bottomBound) ? 1 : 2;
		}
	}

	return LEMON_SUCCESS;
}


int UpdateDoor(Object *Door, World *GameWorld)
{
	// arg1, arg2: [x/y]Pos of destination

	Object *player = GameWorld->Player.PlayerPtr;
	if (player == NULL || Door == NULL)
	{
		return MISSING_DATA;
	}

	if (PlayerInteractingWithBox(Door->ObjectBox, GameWorld) && player->State == DEFAULT_STATE)
	{
		float savedXPos = player->ObjectBox->xPos;
		float savedYPos = player->ObjectBox->yPos;

		GoTo(player, Door->arg1, Door->arg2);

		if (GetCollidingObject(player->ObjectBox, &GameWorld->ObjectList) != NULL)
		{
			SayText("The door seems to be blocked on the other side.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		}
		else
		{
			// Test dialogue, replace with prompt "Go through door?" or something
			SayText("<WHT>Watch out for <YLW>Blue stop signs<WHT>.", 
				NULL, SILENT_TEXT, GameWorld);

			SayTextOption("Enter the Door?", "Test_Face", BASIC_FLIP, GameWorld, 3, 
				"Yes", Event_TeleportPlayerToExitDoor(Door->Parent, GameWorld), 
				"No", NO_ACTION,
				"hm... lemme think about it", playCutscene(TEST_SCENE_2, GameWorld));
		}

		GoTo(player, savedXPos, savedYPos);
	}

	return LEMON_SUCCESS;
} 


int TeleportPlayerToExitDoor(Object *Door, World *GameWorld)
{	
	if (GameWorld == NULL || Door == NULL || GameWorld->Player.PlayerPtr == NULL)
	{
		return MISSING_DATA;
	}

	PlayerData *Player = &GameWorld->Player;

	centerOnObject(Player->PlayerPtr, Door);

	PlaySound("Objects/DoorOpen", 1.0, OBJECT_SFX);
			
	ResetPlayer(Player);

	return LEMON_SUCCESS;
}


int UpdateLevelDoor(Object *Door, World *GameWorld)
{
	// arg1: Level to load
	// arg4: Open/close state
	// arg2, arg3: X/Y for Player at start of level

	Object *player = GameWorld->Player.PlayerPtr;

	if (Door == NULL || player == NULL)
	{
		return MISSING_DATA;
	}

	if (Door->arg4 == 0 && PlayerInteractingWithBox(Door->ObjectBox, GameWorld) && player->State == DEFAULT_STATE)
	{
		Door->arg4 = 1;
		SayText("It's a door... \nIt eminates a strange glow.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		char phrase[MAX_TEXT_LENGTH] = {0};
		snprintf(phrase, MAX_TEXT_LENGTH, "This Door will send you to level %d!", Door->arg1);
		SayTextAndTriggerEvent(phrase, NO_PORTRAIT, BASIC_TEXT, GameWorld, switchLevel(Door->arg1, GameWorld));
	}

	if (Door->arg4 != 0 && GameWorld->SceneActionQueue == NULL)
	{
		Event_MovePlayer(Door->arg2, Door->arg3, GameWorld);

		Door->arg4 = 0;
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


int ApplyGravity(Object *inputObject, World *GameWorld)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsBox *inputBox = inputObject->ObjectBox;

	if (GameWorld->PhysicsType == PLATFORMER)
	{
		inputBox->yVelocity += GameWorld->GlobalGravityY;
		inputBox->xVelocity += GameWorld->GlobalGravityX;

		CheckForGround(inputObject, GameWorld);
	}
	else
	{
		inputBox->inAir = 0;
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


int GoToWithCollision(Object *inputObject, float destX, float destY, World *GameWorld)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}


	GoTo(inputObject, destX, destY);


	if (GameWorld == NULL || inputObject->ObjectBox->solid == UNSOLID)
	{
		return MISSING_DATA;
	}

	ResolveAllXCollision(inputObject->ObjectBox, GameWorld);
	ResolveAllYCollision(inputObject->ObjectBox, GameWorld);


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
	input->ObjectBox->prevXPos = input->ObjectBox->xPos;
	input->ObjectBox->prevYPos = input->ObjectBox->yPos;
	

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
	input->ObjectBox->prevXPos = input->ObjectBox->xPos;
	input->ObjectBox->prevYPos = input->ObjectBox->yPos;
	
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
		input->ObjectBox->xPos = getMouseXHUD() - (input->ObjectBox->xSize >> 1);
		input->ObjectBox->yPos = getMouseYHUD() - (input->ObjectBox->ySize >> 1);
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

	// float MouseX = MouseInput.xPos + GameWorld->MainCamera.CameraX;
	// float MouseY = MouseInput.yPos + GameWorld->MainCamera.CameraY;

	if (getDisplayLayer(inputObject) == HUD)
	{
		PointObjectToXY(inputObject, getMouseXHUD(), getMouseYHUD());
	}
	else
	{
		PointObjectToXY(inputObject, getMouseXCam(GameWorld->MainCamera), getMouseYCam(GameWorld->MainCamera));
	}

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
int ChangeXSizeBy(int change, Object *inputObject, World *GameWorld)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	float changeHalf = change >> 1;

	inputObject->ObjectBox->xSize += change;
	inputObject->ObjectBox->xPos -= changeHalf;

	if (GameWorld == NULL || change == 0 || inputObject->ObjectBox->solid == UNSOLID)
	{
		return EXECUTION_UNNECESSARY;
	}

	ResolveAllXCollision(inputObject->ObjectBox, GameWorld);
	
	return LEMON_SUCCESS;
}


int ChangeYSizeBy(int change, Object *inputObject, World *GameWorld)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	float changeHalf = change >> 1;

	inputObject->ObjectBox->ySize += change;
	inputObject->ObjectBox->yPos -= changeHalf;

	if (GameWorld == NULL || change == 0 || inputObject->ObjectBox->solid == UNSOLID)
	{
		return EXECUTION_UNNECESSARY;
	}


	ResolveAllYCollision(inputObject->ObjectBox, GameWorld);
	
	return LEMON_SUCCESS;
}


int setScaleSize(Object *input, float sizePercentage)
{
	if (sizePercentage < 0.001 || sizePercentage > 100.0)		// 1.0 is normal size, 100.0 is 100 times larger
	{
		return INVALID_DATA;
	}

	DisplayData *inputDisplay = input->ObjectDisplay;

	inputDisplay->size = sizePercentage;

	return LEMON_SUCCESS;
}


int changeScaleSize(Object *input, float sizePercentage)
{
	DisplayData *inputDisplay = input->ObjectDisplay;

	return setScaleSize(input, sizePercentage + inputDisplay->size);
}


int smoothSizeChangeTo(Object *input, float desiredSize, float rate)
{
	DisplayData *inputDisplay = input->ObjectDisplay;

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
	DisplayData *inputDisplay = input->ObjectDisplay;

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

	return sqrt( (xDiff * xDiff) + (yDiff * yDiff));
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
		mouseBox.xPos = getMouseXHUD();
		mouseBox.yPos = getMouseYHUD();
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

	if (getDisplayLayer(input) != HUD)
	{
		mouseBox.xPos = getMouseXCam(inputCam);
		mouseBox.yPos = getMouseYCam(inputCam);
	}
	else
	{
		mouseBox.xPos = getMouseXHUD();
		mouseBox.yPos = getMouseYHUD();
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

	depthCounter = 0;

	moveObjectX(inputObject, GameWorld);
	moveObjectY(inputObject, GameWorld);
	moveObjectForward(inputObject, GameWorld);

	if (isBullet(inputObject, GameWorld))
	{
		bulletCollision(inputObject, GameWorld);
	}

	PhysicsBox *inputBox = inputObject->ObjectBox;
	inputBox->xPos = fClamp(inputBox->xPos, -EngineSettings.WorldBoundX, EngineSettings.WorldBoundX - inputBox->xSize);
	inputBox->yPos = fClamp(inputBox->yPos, -EngineSettings.WorldBoundY, EngineSettings.WorldBoundY - inputBox->ySize);

	// this is a hack, but it fixes the case where a platform is moving with gravity (eg. down) and the subject lands on the platform before it has moved for this tick
	redoGroundCheck(inputObject, GameWorld);

	return LEMON_SUCCESS;
}


int moveObjectX(Object *input, World *GameWorld)
{
	PhysicsBox *box = input->ObjectBox;
	
	if (fabs(box->xVelocity) < 0.01)
	{
		return EXECUTION_UNNECESSARY;
	}

	box->xPos += box->xVelocity;

	if (box->solid != UNSOLID && HasPhysics(input, GameWorld) && LEMON_COLLISION_PHYSICS)	
	{
		ResolveAllXCollision(input->ObjectBox, GameWorld);
	}

	
	return LEMON_SUCCESS;
}



int moveObjectY(Object *input, World *GameWorld)
{
	PhysicsBox *box = input->ObjectBox;
	
	if (fabs(box->yVelocity) < 0.01)
	{
		return EXECUTION_UNNECESSARY;
	}


	box->yPos += box->yVelocity;

	if (box->solid != UNSOLID && HasPhysics(input, GameWorld) && LEMON_COLLISION_PHYSICS)
	{
		ResolveAllYCollision(input->ObjectBox, GameWorld);
	}


	return LEMON_SUCCESS;
}


int moveObjectForward(Object *input, World *GameWorld)
{
	PhysicsBox *movingBox = input->ObjectBox;

	if (fabs(movingBox->forwardVelocity) < 0.01)
	{
		return EXECUTION_UNNECESSARY;
	}

	// float xComponent = (movingBox->forwardVelocity * sin(movingBox->direction * DEGREE_TO_RADIAN_PI));
	// float yComponent = (movingBox->forwardVelocity * cos(movingBox->direction * DEGREE_TO_RADIAN_PI));

	// Unsolid objects do not have to do collision detection so it skips the rest of the function by moving all steps instantly
	if (!LEMON_COLLISION_PHYSICS || movingBox->solid == UNSOLID || !HasPhysics(input, GameWorld))
	{
		movingBox->xPos += (movingBox->forwardVelocity * sin(movingBox->direction * DEGREE_TO_RADIAN_PI)) + movingBox->xVelocity;
		movingBox->yPos += (movingBox->forwardVelocity * cos(movingBox->direction * DEGREE_TO_RADIAN_PI)) + movingBox->yVelocity;
		
		return LEMON_SUCCESS;
	}


	// float savedX = movingBox->xVelocity;
	// float savedY = movingBox->yVelocity;

	// movingBox->xVelocity = xComponent;
	// movingBox->yVelocity = yComponent;
	
	// moveObjectX(input, ObjectList);
	// moveObjectY(input, ObjectList);

	// if ((fabs(movingBox->xVelocity) < 0.001 && fabs(savedX) > 0.001) || (fabs(movingBox->yVelocity) < 0.001 && fabs(savedY) > 0.001))
	// {
	// 	movingBox->forwardVelocity = 0.0;
	// }

	// movingBox->xVelocity = savedX;
	// movingBox->yVelocity = savedY;

	// return LEMON_SUCCESS;
	
	return resolveForwardCollision(movingBox, GameWorld);
}

bool checkBoxOverlapsBoxBroad(PhysicsBox *inputBox, PhysicsBox *compareBox)
{
	if (inputBox == compareBox || inputBox->xSize < 1 || inputBox->ySize < 1 || compareBox->xSize < 1 || compareBox->ySize < 1)
	{
		return false;
	}

	return !((int)inputBox->xPos >= (int)compareBox->xPos + compareBox->xSize || (int)inputBox->xPos + inputBox->xSize <= (int)compareBox->xPos 
		|| (int)inputBox->yPos >= (int)compareBox->yPos + compareBox->ySize || (int)inputBox->yPos + inputBox->ySize <= (int)compareBox->yPos);
}


bool OverlapComparison_CircleCircle(PhysicsBox *circle1, PhysicsBox *circle2)
{
	float radius = (float)circle1->ySize / 2.0;	
	float circleCenterX = circle1->xPos + radius;
	float circleCenterY = circle1->yPos + radius;

	float compareRadius = (float)circle2->ySize / 2.0;
	float compareCenterX = circle2->xPos + compareRadius;
	float compareCenterY = circle2->yPos + compareRadius;

	float distX = circleCenterX - compareCenterX;
  	float distY = circleCenterY - compareCenterY;
  	float distance = (distX*distX) + (distY*distY);

	if (distance >= (radius + compareRadius) * (radius + compareRadius))
	{
		return false;
	}
	else 
	{
		return true;
	}
}

bool OverlapComparison_BoxCircle(PhysicsBox *box, PhysicsBox *circle)
{
	float radius = (float)circle->ySize / 2.0;
	float circleCenterX = circle->xPos + radius;
	float circleCenterY = circle->yPos + radius;


	float compareEdgeX = circleCenterX;
	float compareEdgeY = circleCenterY;

	if (circleCenterX < box->xPos)         
	{
		compareEdgeX = box->xPos;      
	}
  	else if (circleCenterX > box->xPos + box->xSize) 
  	{
  		compareEdgeX = box->xPos + box->xSize;  
  	}
  	if (circleCenterY < box->yPos)         
  	{
  		compareEdgeY = box->yPos;      // top edge
  	}
  	else if (circleCenterY > box->yPos + box->ySize) 
  	{
  		compareEdgeY = box->yPos + box->ySize;   // bottom edge
  	}


 	float distX = circleCenterX - compareEdgeX;
  	float distY = circleCenterY - compareEdgeY;
  	float distance = (distX*distX) + (distY*distY);

  	if (distance <= radius * radius) 
  	{
   	 	return true;
  	}
  	else
  	{
  		return false;
  	}
}

bool OverlapComparison_SlopeCircle(PhysicsBox *slope, PhysicsBox *circle)
{
	float radius = (float)(circle->ySize / 2);
	float circleCenterX = circle->xPos + radius;
	float circleCenterY = circle->yPos + radius;


	// check center of circle first
	if (pointOverlapsWithSlope(circleCenterX, circleCenterY, slope))
	{
		return true;
	}

	if (radius < 2)
	{
		return false;
	}

	float point1X;
	float point1Y;
	float point2X;
	float point2Y;

	if (slope->xFlip == 1)
	{
		point1X = slope->xPos + slope->xSize;
	 	point1Y = slope->yPos;
	 	point2X = slope->xPos + slope->xSize; 
		point2Y = slope->yPos + slope->ySize;
	}
	else
	{
		point1X = slope->xPos;
	 	point1Y = slope->yPos;
	 	point2X = slope->xPos; 
		point2Y = slope->yPos + slope->ySize;
	}

	if (circleOverlapsWithLine(point1X, point1Y, point2X, point2Y, circleCenterX, circleCenterY, radius))
	{
		return true;
	}

	if (slope->yFlip == 1)
	{
		point1X = slope->xPos;
	 	point1Y = slope->yPos;
	 	point2X = slope->xPos + slope->xSize; 
		point2Y = slope->yPos;
	}
	else
	{
		point1X = slope->xPos;
	 	point1Y = slope->yPos + slope->ySize;
	 	point2X = slope->xPos + slope->xSize; 
		point2Y = slope->yPos + slope->ySize;
	}

	if (circleOverlapsWithLine(point1X, point1Y, point2X, point2Y, circleCenterX, circleCenterY, radius))
	{
		return true;
	}

	if (slope->yFlip + slope->xFlip != 0)
	{
		point1X = slope->xPos;
	 	point1Y = slope->yPos;
	 	point2X = slope->xPos + slope->xSize; 
		point2Y = slope->yPos + slope->ySize;
	}
	else
	{
		point1X = slope->xPos;
	 	point1Y = slope->yPos + slope->ySize;
	 	point2X = slope->xPos + slope->xSize; 
		point2Y = slope->yPos;
	}

	if (circleOverlapsWithLine(point1X, point1Y, point2X, point2Y, circleCenterX, circleCenterY, radius))
	{
		return true;
	}

	return false;
}

bool pointOverlapsWithSlope(float x, int y, PhysicsBox *slope)
{
	float slopeRight = slope->xPos + slope->xSize;
	float slopeTop = slope->yPos + slope->ySize;

	float point1X;
	float point1Y;
	float point2X;
	float point2Y;

	if (slope->xFlip == 1)
	{
		if (x > slopeRight)
		{
			return false;
		}

		point1X = slope->xPos;
		point2X = slopeRight;
	}
	else
	{
		if (x < slope->xPos)
		{
			return false;
		}

		point1X = slopeRight;
		point2X = slope->xPos;
	}

	if (slope->yFlip == 1)
	{
		if (y < slope->yPos)
		{
			return false;
		}

		point1Y = slope->yPos;
		point2Y = slopeTop;
	}
	else
	{
		if (y > slopeTop)
		{
			return false;
		}

		point1Y = slopeTop;
		point2Y = slope->yPos;
	}

	bool isLeft = ((point2X - point1X)*(y - point1Y)) - ((point2Y - point1Y)*(x - point1X)) > 0;

	if (slope->xFlip + slope->yFlip != 0)
	{
		return !isLeft;
	}
	else
	{
		return isLeft;
	}
}

bool circleOverlapsWithLine(float x1, float y1, float x2, float y2, float circleCenterX, float circleCenterY, float radius)
{
	float distX = x1 - x2;
	float distY = y1 - y2;
	float len = sqrt( (distX*distX) + (distY*distY) );

	if (len < 0.01)
	{
		return true;
	}

	float dot = ( ((circleCenterX-x1) * (x2-x1)) + ((circleCenterY-y1) * (y2-y1)) ) / pow(len, 2);

	float closestX = x1 + (dot * (x2-x1));
	float closestY = y1 + (dot * (y2-y1));

	if ((closestX > x1) == (closestX > x2) && (closestY > y1) == (closestY > y2)) 
	{
		return false;
	}

	distX = closestX - circleCenterX;
	distY = closestY - circleCenterY;
	float distance = sqrt( (distX*distX) + (distY*distY) );

	return distance <= radius;
}



bool CheckBoxOverlapsBox(PhysicsBox *inputBox, PhysicsBox *compareBox)
{
	if (inputBox == NULL || compareBox == NULL)
	{
		return false;
	}

	if (inputBox == compareBox || inputBox->xSize < 1 || inputBox->ySize < 1 || compareBox->xSize < 1 || compareBox->ySize < 1)
	{
		return false;
	}

	if (compareBox->shape == CIRCLE)
	{
		PhysicsBox *temp = inputBox;
		inputBox = compareBox;
		compareBox = temp;
	}

	if (inputBox->shape == CIRCLE)
	{
		if (compareBox->shape == CIRCLE)
		{
			return OverlapComparison_CircleCircle(inputBox, compareBox);
		}
		else if (compareBox->shape == FLAT_SLOPE)
		{
			return OverlapComparison_SlopeCircle(compareBox, inputBox);
		}
		else
		{
			return OverlapComparison_BoxCircle(compareBox, inputBox);
		}
	}


	int inputX = inputBox->xPos;
	int inputXRight = inputBox->xPos + inputBox->xSize;
	int inputY = inputBox->yPos;
	int inputYTop = inputBox->yPos + inputBox->ySize;

	int compareX = compareBox->xPos;
	int compareXRight = compareBox->xPos + compareBox->xSize;
	int compareY = compareBox->yPos;
	int compareYTop = compareBox->yPos + compareBox->ySize;

	if (inputBox->shape == FLAT_SLOPE)
	{
		if (inputBox->xFlip == 1)
		{
			inputYTop = compareBox->xSize - inputBox->xPos + compareBox->xPos;
		}
		else
		{
			inputYTop = inputBox->xSize + inputBox->xPos - compareBox->xPos;
		}
		
		inputYTop = clamp((int)((float)inputYTop * ((float)inputBox->ySize/(float)inputBox->xSize)), 0, inputBox->ySize);

		if (inputBox->yFlip == -1)
		{
			inputY = inputBox->ySize - inputYTop + inputBox->yPos;
			inputYTop = inputBox->ySize;
		}
		
		inputYTop += inputBox->yPos;
	} 

	if (compareBox->shape == FLAT_SLOPE)
	{
		if (compareBox->xFlip == 1)
		{
			compareYTop = inputBox->xSize + inputBox->xPos - compareBox->xPos;
		}
		else
		{
			compareYTop = compareBox->xSize + compareBox->xPos - inputBox->xPos;
		}
		
		compareYTop = clamp((int)((float)compareYTop * ((float)compareBox->ySize/(float)compareBox->xSize)), 0, compareBox->ySize);

		if (compareBox->yFlip == -1)
		{
			compareY = compareBox->ySize - compareYTop + compareBox->yPos;
			compareYTop = compareBox->ySize;
		}
		
		compareYTop += compareBox->yPos;
	}


	return !(inputY >= compareYTop || inputYTop <= compareY || inputX >= compareXRight || inputXRight <= compareX);
}


bool CheckBoxCollidesBox(PhysicsBox *inputBox, PhysicsBox *compareBox)
{
	if (inputBox == NULL || compareBox == NULL)
	{
		return false;
	}

	if (compareBox->collideLayer != inputBox->collideLayer)
	{
		return false;
	}

	if (CheckBoxOverlapsBox(inputBox, compareBox) == false)
	{
		return false;
	}


	switch(inputBox->flag)
	{
		case IGNORE_SOLID:
			if (compareBox->solid == SOLID)
			{
				return false;
			}
			break;

		case IGNORE_SELF:
			if (compareBox->solid == inputBox->solid)
			{
				return false;
			}
			break;

		case ONLY_BODIES:
			if (compareBox->solid != BODY)
			{
				return false;
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
				return false;
			}
			break;

		case IGNORE_SELF:
			if (compareBox->solid == inputBox->solid)
			{
				return false;
			}
			break;

		case ONLY_BODIES:
			if (inputBox->solid != BODY)
			{
				return false;
			}
			break;

		case GET_IGNORED:
			return false;

		default:
		break;
	}


	switch(inputBox->solid)
	{		
		case JUMP_THROUGH:
		{
			if (compareBox->yVelocity > inputBox->yVelocity || compareBox->crouch == true || compareBox->prevYPos < (inputBox->prevYPos + inputBox->ySize - 1) )
			{
				return false;
			}
		} break;

		case UNSOLID:
		return false;

		case BODY:
		if (compareBox->solid == BODY)
		{
			return false;
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
				return false;
			}
		} break;

		case UNSOLID:
		return false;
			
		default:
		break;
	}


	return true;
}


Object* CheckForGround(Object *input, World *GameWorld)
{
	if (!LEMON_COLLISION_PHYSICS || input == NULL || input->ObjectBox->solid == UNSOLID)
	{
		return NULL;
	}

	PhysicsBox *box = input->ObjectBox;

	float xPos = box->xPos;
	float yPos = box->yPos;

	box->xPos += fClamp(GameWorld->GlobalGravityX * 2.0, -32.0, 32.0);
	box->yPos += fClamp(GameWorld->GlobalGravityY * 2.0, -32.0, 32.0);

	if (box->GroundBox != NULL)
	{
		// if there was a ground referenced previously, move the distance it has moved to see if reconnection is possible; with a limit on distance
		PhysicsBox *prevGround = box->GroundBox;

		// dont move if it goes against gravity
		if (((prevGround->xPos - prevGround->prevXPos) < 0.0) == (GameWorld->GlobalGravityX < 0.0))
		{
			box->xPos += fClamp((prevGround->xPos - prevGround->prevXPos), -32.0, 32.0);
		}

		if (((prevGround->yPos - prevGround->prevYPos) < 0.0) == (GameWorld->GlobalGravityY < 0.0))
		{
			box->yPos += fClamp((prevGround->yPos - prevGround->prevYPos), -32.0, 32.0);
		}
	}

	Object *GroundObject = GetCollidingObject(box, &GameWorld->ObjectList);

	box->xPos = xPos;
	box->yPos = yPos;


	if (GroundObject != NULL)
	{ 
		if (box->inAir > 0)
		{
			if (fabs(box->PhysicsXVelocity) > 0.1)
			{
				box->xVelocity = 0.0;
			}

			if (fabs(box->PhysicsYVelocity) > 0.1)
			{
				box->yVelocity = 0.0;
			}
		}

		// GroundBox should only be set once, ideally by the AppyGravity function, so if it has been set, do not override
		if ((input->reserved & RFLAG_GROUND_SET) == 0)
		{
			input->reserved |= RFLAG_GROUND_SET;

			box->GroundBox = GroundObject->ObjectBox;

			box->inAir = 0;
		}

		AssignDirection(box, box->GroundBox);
	}
	else
	{
		// If inAir is 0, that means at last check you were on ground

		box->GroundBox = NULL;

		box->inAir++;

		if (box->inAir > 99)
		{
			box->inAir = 100;

			// Falling for a long time!
		}

		// glide to correct direction
		box->direction += (DEFAULT_DIRECTION - box->direction) / 5.0;

		if (fabs(box->direction - DEFAULT_DIRECTION) < 0.01)
		{
			box->direction = DEFAULT_DIRECTION;
		}
	}

	return GroundObject;
}


// Used to update ground object without affecting other values - SHOULD ONLY BE USED IN PHYSICS APPLICATIONS WHEN NECESSARY
void redoGroundCheck(Object *input, World *GameWorld)
{
	if (!LEMON_COLLISION_PHYSICS || GameWorld->PhysicsType != PLATFORMER || !HasGravity(input, GameWorld))
	{
		return;
	}

	PhysicsBox *inputBox = input->ObjectBox;
	input->reserved &= ~RFLAG_GROUND_SET;
	CheckForGround(input, GameWorld);

	if (inputBox->inAir > 1)
	{
		inputBox->inAir--;
	}
	
	return;
}


int AssignDirection(PhysicsBox *inputBox, PhysicsBox *compareBox)
{
	if (inputBox == NULL || compareBox == NULL)
	{
		return MISSING_DATA;
	}

	
	switch (compareBox->shape)
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
	if (inputBox == NULL || inputBox->solid == UNSOLID || ObjectList == NULL)
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

		if (CheckBoxCollidesBox(inputBox, &boxes[index]))
		{
			return &objects[index];
		}
	}

	return NULL;
}


// returns pointer of object overlapping, NULL if no object is detected; has n^2 complexity, not great!
Object* GetCollidingObject(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	if (inputBox == NULL || inputBox->solid == UNSOLID || ObjectList == NULL)
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

		if (CheckBoxCollidesBox(inputBox, currentObject->ObjectBox))
		{
			return currentObject;
		}

		currentObject = currentObject->nextObject;
	}

	return NULL;
}


Object* GetOverlappingObject(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	if (inputBox == NULL || ObjectList == NULL)
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
	if (inputBox == NULL || ObjectList == NULL)
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
	if (inputBox == NULL || inputBox->solid == UNSOLID || ObjectList == NULL)
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
	if (ObjectList == NULL || inputBox == NULL)
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
	if (ObjectList == NULL || inputBox == NULL)
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

bool objectsOverlap(Object *inputObject, Object *otherObject)
{
	if (inputObject == NULL || otherObject == NULL)
	{
		return false;
	}

	return CheckBoxOverlapsBox(inputObject->ObjectBox, otherObject->ObjectBox);
}


int resolveForwardCollision(PhysicsBox *movingBox, World *GameWorld)
{
	ObjectController *ObjectList = &GameWorld->ObjectList;

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

	float xStep = (orientation * sinVal);
	float yStep = (orientation * cosVal);
	int travelCount = (int)fabs(movingBox->forwardVelocity);
	float remnant = fabs(movingBox->forwardVelocity) - floor(fabs(movingBox->forwardVelocity));
	if (remnant > 0.001)
	{
		travelCount++;
	}

	float lastStepX, lastStepY;

	// regular collision
	Object *currentObject = NULL;
	depthCounter++;

	while (travelCount > 0)
	{
		lastStepX = movingBox->xPos;
		lastStepY = movingBox->yPos;

		if (travelCount == 1 && remnant > 0.001)
		{
			yStep *= remnant;
			xStep *= remnant;
		}

		movingBox->yPos += yStep;
		movingBox->xPos += xStep;

		currentObject = GetCollidingObject(movingBox, ObjectList);

		for (int collideCycle = 0; collideCycle < COLLISION_CYCLES && currentObject != NULL; collideCycle++)
		{
			if (evaluateIfCollidePush(movingBox, currentObject->ObjectBox) && depthCounter < COLLISION_DEPTH)
			{
				PhysicsBox *collideBox = currentObject->ObjectBox;

				float tempVelocity = collideBox->forwardVelocity;
				double tempDirection = collideBox->direction;
				float collideXPos = collideBox->xPos;
				float collideYPos = collideBox->yPos;

				
				collideBox->forwardVelocity = orientation * travelCount;
				collideBox->direction = movingBox->direction;

				moveObjectForward(currentObject, GameWorld);

				collideBox->direction = tempDirection;
				collideBox->forwardVelocity = tempVelocity;

				if (CheckBoxCollidesBox(movingBox, collideBox))
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

				while (slopeClimb > 0 && CheckBoxCollidesBox(movingBox, currentObject->ObjectBox))
				{
					movingBox->xPos -= GameWorld->GlobalGravityX;
					movingBox->yPos -= GameWorld->GlobalGravityY;
					slopeClimb--;
				}

				if (CheckBoxCollidesBox(movingBox, currentObject->ObjectBox))
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


int ResolveAllXCollision(PhysicsBox *movingBox, World *GameWorld)
{
	if (movingBox == NULL || GameWorld == NULL)
	{
		return MISSING_DATA;
	}


	int count = 0;
	ObjectController *ObjectList = &GameWorld->ObjectList;

	Object *currentObject = GetCollidingObject(movingBox, ObjectList);

	while (currentObject != NULL && count < COLLISION_CYCLES)
	{
		if (evaluateIfCollidePush(movingBox, currentObject->ObjectBox))
		{
			PhysicsBox *collideBox = currentObject->ObjectBox;

			float prevXPos = collideBox->xPos;
			float prevYPos = collideBox->yPos;

			SolidType prevSolid = movingBox->solid;

			ResolveXCollision(collideBox, movingBox, GameWorld);

			movingBox->solid = UNSOLID;
			ResolveAllXCollision(collideBox, GameWorld);
			movingBox->solid = prevSolid;

			if (GetCollidingObject(collideBox, ObjectList) != NULL)
			{
				collideBox->xPos = prevXPos;
				collideBox->yPos = prevYPos;
				ResolveXCollision(movingBox, collideBox, GameWorld);
			}
		}
		else
		{
			ResolveXCollision(movingBox, currentObject->ObjectBox, GameWorld);
		}
		

		currentObject = GetCollidingObject(movingBox, ObjectList);

		count++;
	}


	return LEMON_SUCCESS;
}


int ResolveXCollision(PhysicsBox *movingBox, PhysicsBox *compareBox, World *GameWorld)
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
		case JUMP_THROUGH:
		case UNSOLID:
			return EXECUTION_UNNECESSARY;

		default:
			break;
	}

	switch(compareBox->shape)
	{
		case CIRCLE:
		{
			if (movingBox->shape == CIRCLE)
			{
				float radius = (float)(movingBox->ySize >> 1);
				float compareRadius = (float)(compareBox->ySize >> 1);

				float prevDistX = (movingBox->prevXPos + (movingBox->xSize >> 1)) - (compareBox->xPos + (compareBox->xSize >> 1));
				float prevDistY = (movingBox->prevYPos + (movingBox->ySize >> 1)) - (compareBox->yPos + (compareBox->ySize >> 1));
				float direction = atan2(prevDistX, prevDistY);

				movingBox->xPos = compareBox->xPos + ((radius + compareRadius) * sin(direction));
			}
			else
			{
				if (prevXPosInt < ObjXCenter)
				{
					movingBox->xPos = compareBox->xPos - movingBox->xSize;
				}
				else
				{
					movingBox->xPos = compareBox->xPos + compareBox->xSize;
				}
			}
		} break;

		// Y = X * (ySize/xSize)
		case FLAT_SLOPE:
		{
			if (compareBox->xFlip == 1)
			{
				if (prevXPosInt >= objXRight)
				{
					movingBox->xPos = compareBox->xPos + compareBox->xSize;
					break;
				}
			}
			else
			{
				if (prevXPosInt + movingBox->xSize <= compareBox->xPos)
				{
					movingBox->xPos = compareBox->xPos - movingBox->xSize;
					break;
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


			if (GetCollidingObject(movingBox, &GameWorld->ObjectList) != NULL)
			{
				ClimbSlope(movingBox, GameWorld);
			}

			return LEMON_SUCCESS;
		} break;

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
		} break;
	}

	ApplyXPhysics(movingBox, compareBox);


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


int ResolveAllYCollision(PhysicsBox *movingBox, World *GameWorld)
{
	if (movingBox == NULL || GameWorld == NULL)
	{
		return MISSING_DATA;
	}
	
	ObjectController *ObjectList = &GameWorld->ObjectList;

	int count = 0;
	Object *currentObject = GetCollidingObject(movingBox, ObjectList);

	while (currentObject != NULL && count < COLLISION_CYCLES)
	{
		if (evaluateIfCollidePush(movingBox, currentObject->ObjectBox))
		{
			PhysicsBox *collideBox = currentObject->ObjectBox;

			float prevXPos = collideBox->xPos;
			float prevYPos = collideBox->yPos;
			SolidType prevSolid = movingBox->solid;

			ResolveYCollision(collideBox, movingBox);

			movingBox->solid = UNSOLID;
			ResolveAllYCollision(collideBox, GameWorld);
			movingBox->solid = prevSolid;

			if (GetCollidingObject(collideBox, ObjectList) != NULL)
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


	int ObjYCenter = compareBox->yPos + (compareBox->ySize >> 1);

	int prevYPosInt = movingBox->prevYPos;

	switch(compareBox->solid)
	{
		case JUMP_THROUGH:
		{
			if (movingBox->yVelocity > compareBox->yVelocity || movingBox->crouch == true)
			{
				return EXECUTION_UNNECESSARY;
			}
		} break;

		case UNSOLID:
			return EXECUTION_UNNECESSARY;

		default:
			break;
	}


	switch(compareBox->shape)
	{
		case CIRCLE:
		{
			if (movingBox->shape == CIRCLE)
			{
				float radius = (float)(movingBox->ySize >> 1);
				float compareRadius = (float)(compareBox->ySize >> 1);

				float prevDistX = (movingBox->prevXPos + (movingBox->xSize >> 1)) - (compareBox->xPos + (compareBox->xSize >> 1));
				float prevDistY = (movingBox->prevYPos + (movingBox->ySize >> 1)) - (compareBox->yPos + (compareBox->ySize >> 1));
				float direction = atan2(prevDistX, prevDistY);

				movingBox->yPos = compareBox->yPos + ((radius + compareRadius) * cos(direction));
			}
			else
			{
				if (prevYPosInt < ObjYCenter)
				{
					movingBox->yPos = compareBox->yPos - movingBox->ySize;
				}
				else
				{
					movingBox->yPos = compareBox->yPos + compareBox->ySize;
				}
			}
		} break;


		case FLAT_SLOPE:
		{
			if (compareBox->yFlip == 1 && prevYPosInt + movingBox->ySize < compareBox->yPos)
			{
				movingBox->yPos = (compareBox->yPos - movingBox->ySize);
				break;
			}
			else if (compareBox->yFlip == -1 && prevYPosInt >= compareBox->yPos + compareBox->ySize)
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

			slopeFloor = clamp(slopeFloor, 0, compareBox->ySize - 1);

			if (compareBox->yFlip == -1)
			{
				slopeFloor = compareBox->ySize - slopeFloor - movingBox->ySize;
			}

			movingBox->yPos = slopeFloor + compareBox->yPos; 	
		} break;

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


int ResolveAllXCollisionsByPush(PhysicsBox *movingBox, World *GameWorld)
{
	if (movingBox == NULL || GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	ObjectController *ObjectList = &GameWorld->ObjectList;
	Object *collideObject = GetCollidingObject(movingBox, ObjectList);
	int i = 0;

	while (collideObject != NULL && i < 16)
	{
		ResolveXCollision(collideObject->ObjectBox, movingBox, GameWorld);

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
		ResolveYCollision(collideObject->ObjectBox, movingBox);

		collideObject = GetCollidingObject(movingBox, ObjectList);	
		i++;
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


int ClimbSlope(PhysicsBox *inputBox, World *GameWorld)
{		
	if (inputBox == NULL || GameWorld == NULL)
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

	inputBox->xPos = inputBox->prevXPos;
	inputBox->yPos = inputBox->prevYPos;
	inputBox->direction = DEFAULT_DIRECTION;
	inputBox->forwardVelocity = velocity;

	resolveForwardCollision(inputBox, GameWorld);

	inputBox->forwardVelocity = savedForwardVelocity;
	inputBox->direction = savedDirection;

	return LEMON_SUCCESS;
}