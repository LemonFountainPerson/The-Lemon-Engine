#include "LemonEngine.h"



Object* AddObject(World *GameWorld, int objectID, int xPos, int yPos, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return NULL;
	}

	ObjectController *ObjectList = GameWorld->ObjectList;

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
	if (DebugSettings.ConsoleTextEnabled == ALL_EVENTS)
	{
		putConsoleString("\nCreated object type: %d\n", objectID);
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

		newObject->ObjectDisplay->RenderModeOverride = SINGLE;
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
			newObject->ParentObject = AddObject(GameWorld, DOOR, arg1, arg2, xPos, yPos, 1, 0, 0);

			newObject->ParentObject->ParentObject = newObject;
		}
		
	break;

	case SOLID_BLOCK:
		snapPositionToTileGrid(newObject, xPos, yPos);
		newObject->ObjectBox->xSize = arg1 * X_TILESCALE;
		newObject->ObjectBox->ySize = arg2 * Y_TILESCALE;
		newObject->State = STATIC;
		addTileMap(newObject, 32, 32, 32, GameWorld->ObjectList);
		
		if (arg3 >= 0)
		{
			switchObjectSprite(clamp(arg3, 1, arg3), newObject);
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
		newObject->State = STATIC;
		newObject->ObjectBox->xSize = arg1;
		newObject->ObjectBox->ySize = arg2;

		if (arg3 == -1)
		{
			newObject->ObjectBox->xFlip = -1;
		}

		if (arg4 == -1)
		{
			newObject->ObjectBox->yFlip = -1;
		}

		if (arg1 + arg2 > 256)
		{
			switchObjectSprite(3, newObject);
		}
		else if (arg1 + arg2 > 128)
		{
			switchObjectSprite(2, newObject);
		}
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
		snapPositionToTileGrid(newObject, xPos, yPos);
		break;


	case GATE_SWITCH:
	case GATE_SWITCH_TIMED:
	// switch for gate - arg1 is gate ID, arg2 denotes type of switch (0 = or switch, 1 = and switch) 
		if (newObject->ObjectID == GATE_SWITCH_TIMED)
		{
			newObject->arg4 = abs(arg3);
			newObject->arg3 = arg4;
		}
		else
		{
			newObject->arg4 = 0;
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
		newObject->ObjectBox->solid = ENTITY;
		newObject->ObjectBox->forwardVelocity = 3.0;
		newObject->ObjectBox->xSize = 40;
		newObject->ObjectBox->ySize = 60;
		addHealthComponent(newObject, 1000, ObjectList);
		break;


	case PUSHABLE_BOX:
		newObject->ObjectBox->solid = PUSHABLE_SOLID;
		newObject->ObjectBox->xSize = arg1;
		newObject->ObjectBox->ySize = arg2;
		break;


	default:
		break;
	}


	newObject->ObjectBox->prevXPos = newObject->ObjectBox->xPos;
	newObject->ObjectBox->prevYPos = newObject->ObjectBox->yPos;

	UpdateObjectDisplay(newObject);

	
	return newObject;
}

Object* AddNamedObject(World *GameWorld, const char name[], int objectID, int xPos, int yPos)
{
	if (name == NULL || strlen(name) >= OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *createdObject = AddObject(GameWorld, objectID, xPos, yPos, 0, 0, 0, 0, 0);

	if (createdObject == NULL)
	{
		return NULL;
	}

	strcpy(createdObject->name, name);

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
		newObject->ParentObject = ParentObject;
	
		UpdateParentChildLink(newObject);
	}

	return newObject;
}


Object* AddParticle(World *GameWorld, ParticleSubType animation, int xPos, int yPos, int repeatCount, int particleLifeTime)
{
	return AddObject(GameWorld, PARTICLE, xPos, yPos, animation, repeatCount, particleLifeTime, 0, 0);
}


Object* getNewObject(ObjectType objectID, ObjectController *ObjectList)
{
	Object *newObject = findNewObject(ObjectList);

	initialiseGenericObject(newObject, objectID, ObjectList);

	return newObject;
}


int initialiseGenericObject(Object *inputObject, ObjectType objectID, ObjectController *ObjectList)
{
	if (inputObject == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	// reset values
	resetPhysicsBox(inputObject->ObjectBox);
	resetDisplayData(getDisplay(inputObject));


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
	inputObject->ObjectDisplay->spriteSetSource = createObjectSpriteSet(ObjectList, objectID);

	setDisplayLayer(inputObject, MIDDLEGROUND);
	inputObject->Interrupt = NO_INTERRUPT;
	inputObject->ParentObject = NULL;
	inputObject->ParentLink = DEFAULT_LINK;
	inputObject->reserved = AWAITING_UPDATE;
	strcpy(inputObject->name, "Generic");
	inputObject->ObjectID = objectID;
	inputObject->State = DEFAULT;
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
	DisplayData *display = getDisplay(input);

	resetPhysicsBox(box);
	resetDisplayData(display);

	memset(input, 0, sizeof(Object));

	input->ObjectBox = box;
	input->ObjectDisplay = display;
	input->State = EMPTY_OBJECT;

	return;
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

	newObject->ParentObject = NULL;
	newObject->nextObject = NULL;
	newObject->prevObject = NULL;
	newObject->State = EMPTY_OBJECT;

	return newObject;
}
*/


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

void removeSceneActionReferences(Object *input, World *GameWorld)
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


int setObjectName(Object *inputObject, const char name[])
{
	if (name == NULL || strlen(name) >= OBJECT_NAME_LENGTH || inputObject == NULL)
	{
		return MISSING_DATA;
	}

	strcpy(inputObject->name, name);

	return LEMON_SUCCESS;
}


Object* FindObject(const char name[], ObjectController ObjectList)
{
	if (name == NULL || strlen(name) >= OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *currentObj = ObjectList.firstObject;

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
	input->flag = NORMAL_SOLID;
	input->collideMode = NO_COLLIDE_TYPE;
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

	// disgusting code right here
	/*
	Object *objects = input - input->index;
	ComponentData *arenaPtrs = (ArenaData*)(objects - (Object*)OFFSETOF(ComponentData, Objects));
	printf("\n%d", arenaPtrs->TileMaps.dense[0].tileSize);
	*/

	return input->ObjectDisplay;
}

Layer getDisplayLayer(Object *input)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL)
	{
		return UNDEFINED_LAYER;
	}

	return display->layer;
}

bool setDisplayLayer(Object *input, Layer newLayer)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL)
	{
		return false;
	}

	display->layer = newLayer;

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

bool setRenderModeOverride(Object *input, RenderMode newMode)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL)
	{
		return false;
	}

	display->RenderModeOverride = newMode;

	return true;
}

bool setTransparency(Object *input, float transparency)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL)
	{
		return false;
	}

	display->transparency = fClamp(transparency, 0.0, 1.0);

	return true;
}

bool changeTransparency(Object *input, float transparency)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL)
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
	if (display == NULL)
	{
		return false;
	}

	display->hidden = true;

	return true;
}

bool showObject(Object *input)
{
	DisplayData *display = getDisplay(input);
	if (display == NULL)
	{
		return false;
	}
	
	display->hidden = false;

	return true;
}

bool toggleHidden(Object *input)
{
	DisplayData *inputDisplay = getDisplay(input);

	if (inputDisplay == NULL)
	{
		return false;
	}

	inputDisplay->hidden = !inputDisplay->hidden;

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


SpriteSet* createObjectSpriteSet(ObjectController *ObjectList, int ObjectID)
{
	if (ObjectList == NULL || ObjectID <= LEVEL_FLAG_OBJ)
	{
		return NULL;
	}

	// Check for pre-existing spriteset
	SpriteSet *currentSetPtr = ObjectList->startSpriteSetPtr;

	int i = 0;

	if (currentSetPtr != NULL)
	{
		while(currentSetPtr->nextSet != NULL && currentSetPtr->setID != ObjectID)
		{
			currentSetPtr = currentSetPtr->nextSet;
			i++;
		}

		if (currentSetPtr->setID == ObjectID)
		{
			return currentSetPtr;
		}
	}

	// If no sprite set is present, allocate and create one
	SpriteSet *newSet = malloc(sizeof(SpriteSet));

	if (newSet == NULL)
	{
		putConsoleString("\nFailed to allocate memory for new sprite set.");
		return NULL;
	}

	if (currentSetPtr == NULL)
	{
		ObjectList->startSpriteSetPtr = newSet;
	}
	else
	{
		currentSetPtr->nextSet = newSet;
	}

	newSet->prevSet = currentSetPtr;
	newSet->nextSet = NULL;
	newSet->firstSprite = NULL;
	newSet->lastSprite = NULL;
	newSet->setID = ObjectID;
	newSet->spriteCount = 0;
	newSet->Animations = NULL;

	LoadSpritesAndAnimationData(newSet, ObjectID);
	
	return newSet;
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


void deleteAllObjects(ObjectController *ObjectList)
{
	if (ObjectList == NULL )
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

void deleteAllObjectsSafe(ObjectController *ObjectList)
{
	if (ObjectList == NULL )
	{
		return;
	}

	int i = 0;

	while (ObjectList->firstObject != NULL && i < ObjectList->objectCount)
	{
		if (ObjectList->firstObject->State == RESERVED_STATE)
		{
			SetDrawPriorityToFront(ObjectList, ObjectList->firstObject);
		}
		else
		{
			deleteObject(ObjectList->firstObject, ObjectList);
		}

		i++;
	}
	

	return;
}

void deleteAllEnvironmentObjects(ObjectController *ObjectList)
{
	if (ObjectList == NULL )
	{
		return;
	}

	Object *currentObject = ObjectList->firstObject;
	while (ObjectList->firstObject != NULL)
	{
		switch(currentObject->ObjectID)
		{
		case UI_ELEMENT:
		case PLAYER_OBJECT:
		case UI_TEXT:
			currentObject = currentObject->nextObject;
			break;

		default:
			currentObject = deleteObject(ObjectList->firstObject, ObjectList);
			break;
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


	if (DebugSettings.ConsoleTextEnabled == ALL_EVENTS)
	{
		putConsoleStrIntStr("\nMarked object type for deletion: ", inputObject->ObjectID, "\n");
	}
	
	return LEMON_SUCCESS;
}


int UnmarkObjectForDeletion(Object *inputObject, ObjectController *ObjectList)
{
	if (inputObject == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (inputObject->State != TO_BE_DELETED || ObjectList->firstObject == NULL)
	{
		return INVALID_DATA;
	}

	inputObject->State = DEFAULT;

	putConsoleStrIntStr("\nUnmarked object type for deletion!:", inputObject->ObjectID, "\n");


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

	Object *nextPtr;
	nextPtr = input->nextObject;

	Object *prevPtr;
	prevPtr = input->prevObject;

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

	Object *nextPtr;
	nextPtr = input->nextObject;

	Object *prevPtr;
	prevPtr = input->prevObject;

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
	
	Object *nextPtr;
	nextPtr = input->nextObject;

	Object *prevPtr;
	prevPtr = input->prevObject;

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
static Uint64 tickNum = 0;
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

	if (ObjectList == NULL || ObjectList->firstObject == NULL)
	{
		return MISSING_DATA;
	}

	tickNum++;

	updatePreviousPositions(ObjectList);

	Object *currentObject = ObjectList->firstObject;

	while(currentObject != NULL)
	{
		if (GameWorld->GamePaused == 0 || currentObject->ObjectID == UI_ELEMENT)
		{
			ObjectBehaviour(GameWorld, currentObject);
			UpdateObjectDisplay(currentObject);
		}
		
		currentObject = currentObject->nextObject;
	}

	// Update object state - parent-child links, deletion, etc.
	updateObjectsState(ObjectList, GameWorld);

	ResolveAllObjects(ObjectList, GameWorld->PhysicsType);


	return LEMON_SUCCESS;
}


int ObjectBehaviour(World *GameWorld, Object *inputObject)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	inputObject->reserved = PHYSICS_DISABLED;

	if (inputObject->State == STATIC || inputObject->State < DEFAULT)
	{
		return EXECUTION_UNNECESSARY;
	}

	bool gameStateDisable = (GameWorld->GameState == CUTSCENE && inputObject != GameWorld->Player.PlayerPtr) || inputObject->State == ACTOR; 
	bool immuneObject = inputObject->ObjectID == UI_ELEMENT || inputObject->ObjectID == UI_TEXT || inputObject->ObjectID == PARTICLE;


	if ((!immuneObject && gameStateDisable) || inputObject->State == IN_INVENTORY)
	{
		return ACTION_DISABLED;
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
			ApplyGravity(inputObject, GameWorld);
			break;


		case PUSHABLE_BOX:
			ApplyFriction(inputObject->ObjectBox, 0.9, 0.9, 1.0);
			ApplyGravity(inputObject, GameWorld);

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

				if (inputObject->arg1 > 9900)
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

				AddParticle(GameWorld, SPARKLE, inputObject->ObjectBox->xPos + (inputObject->ObjectBox->xSize >> 1), inputObject->ObjectBox->yPos + (inputObject->ObjectBox->ySize >> 1), 1, 0);

				inflictDamage(40, hitTarget, GameWorld->ObjectList);
			}
			break;


		default:
			break;
	}


	MoveObject(inputObject, GameWorld);


	return LEMON_SUCCESS;
}


int UpdateObjectDisplay(Object *inputObject)
{
	DisplayData *inputDisplay = getDisplay(inputObject);

	if (inputDisplay == NULL)	{ return MISSING_DATA; }

	if (inputObject->State < DEFAULT)
	{
		return ACTION_DISABLED;
	}

	
	iterateAnimation(inputDisplay);

	// Assign Sprite   
	if (inputDisplay->currentSprite > 0)
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
	//PhysicsBox *boxList = ObjectList->objectComponents.PhysicsBoxes;
	//Object *objList = ObjectList->objectComponents.Objects;

	input->ObjectBox->prevXPos = input->ObjectBox->xPos;
	input->ObjectBox->prevYPos = input->ObjectBox->yPos;

	input->ObjectBox->GroundBox = NULL;

	return LEMON_SUCCESS;
}



// better name please
int updateObjectsState(ObjectController *ObjectList, World *GameWorld)
{
	Object *Objects = ObjectList->objectComponents.Objects;
	int i = EngineSettings.MaxObjects - 1;
	int k = 0;

	while (i > 0 && k < ObjectList->objectCount)
	{	
		UpdateParentChildLink(&Objects[i]);

		if (Objects[i].State == TO_BE_DELETED)
		{
			if (PLAYER_OBJECT == Objects[i].ObjectID)
			{
				PlayerObjectAboutToBeDeleted(&GameWorld->Player);
			}

			deleteObject(&Objects[i], ObjectList);
		}
		else if (Objects[i].State != EMPTY_OBJECT)
		{
			UpdatePhysicsState(&Objects[i], GameWorld);

			k++;
		}

		i--;
	}

	return LEMON_SUCCESS;
}


int UpdatePhysicsState(Object *inputObject, World *GameWorld)
{
	if (inputObject->State == STATIC || inputObject->State < DEFAULT || inputObject->reserved != AWAITING_MAGNETISATION || !LEMON_COLLISION_PHYSICS)
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


	if (!(fabs(inputBox->PhysicsXVelocity) < 0.1) )
	{
		inputBox->xPos += inputBox->PhysicsXVelocity;
	}
	if (!(fabs(inputBox->PhysicsYVelocity) < 0.1) )
	{
		inputBox->yPos += inputBox->PhysicsYVelocity;
	}

	if (inputBox->solid != UNSOLID)
	{
		inputObject->reserved = AWAITING_RESOLUTION;
	}

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
	float pixelXDifference = (GroundBox->xPos) - (GroundBox->prevXPos);

	GroundBox->xPos += pixelXDifference; 
		
	if (fabs(GameWorld->GlobalGravityX) < 0.1 || CheckBoxCollidesBox(GroundBox, inputBox) == 0)
	{
		inputBox->PhysicsXVelocity = pixelXDifference;
	}

	GroundBox->xPos -= pixelXDifference; 

	float pixelYDifference = (GroundBox->yPos) - (GroundBox->prevYPos);
	GroundBox->yPos += pixelYDifference; 
		
	if (fabs(GameWorld->GlobalGravityY) < 0.1 || CheckBoxCollidesBox(GroundBox, inputBox) == 0)
	{
		inputBox->PhysicsYVelocity = pixelYDifference;
	}

	GroundBox->yPos -= pixelYDifference;

	if (fabs(inputBox->PhysicsXVelocity) < 0.1)
	{
		inputBox->PhysicsXVelocity = 0.0;
	}

	if (fabs(inputBox->PhysicsYVelocity) < 0.1)
	{
		inputBox->PhysicsYVelocity = 0.0;
	}
	

	return LEMON_SUCCESS;
}


int UpdateParentChildLink(Object *inputObject)
{
	Object *parent = inputObject->ParentObject;

	if (parent == NULL || inputObject->State == EMPTY_OBJECT)
	{
		return EXECUTION_UNNECESSARY;
	}

	// If a third-order or more child (aka 'grandchild') is 'lagging' behind the grandparent then try postioning the objectlist as   
	// grandparent -> parent -> child		instead of 		child -> parent -> grandParent
	// This will fix any inconsistencies, although it will mean you must be careful with object order changing functions (such as setDrawPriority)
	// As a note; deletion will never lag behind in this way

	Object *grandParent = parent;
	Object *loopCheck = grandParent->ParentObject;

	while (grandParent->ParentObject != NULL)
	{
		grandParent = grandParent->ParentObject;

		if (grandParent->State < 0)
		{
			MarkObjectForDeletion(inputObject);
			return ACTION_DISABLED;
		}

		if (loopCheck == NULL || loopCheck->ParentObject == NULL)
		{
			continue;		// stop checking for potential loops
		}

		loopCheck = loopCheck->ParentObject->ParentObject;

		if (grandParent == loopCheck)
		{
			return INVALID_DATA;
		}
	}
	

	// mark this object for deletion if its parent is deleted or is marked for deletion as well 
	if (parent->State < 0)
	{
		MarkObjectForDeletion(inputObject);
		return ACTION_DISABLED;
	}

	if ((parent->ParentLink & FINAL_LINK) != 0)
	{
		inputObject->ParentObject = NULL;
		return ACTION_DISABLED;
	}

	PhysicsBox *InputBox = inputObject->ObjectBox;
	PhysicsBox *ParentBox = parent->ObjectBox;

	if (ParentBox == NULL)
	{
		return MISSING_DATA;
	}

	if ((inputObject->ParentLink & POSITION_LINK) != 0)
	{
		InputBox->xPos += floor(ParentBox->xPos - ParentBox->prevXPos);

		InputBox->yPos += floor(ParentBox->yPos - ParentBox->prevYPos);
	}

	if ((inputObject->ParentLink & VELOCITY_LINK) != 0)
	{
		InputBox->xVelocity = ParentBox->xVelocity;
		InputBox->yVelocity = ParentBox->yVelocity;
		InputBox->direction = ParentBox->direction;
		InputBox->forwardVelocity = ParentBox->forwardVelocity;
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
	}

	return LEMON_SUCCESS;
}


int ResolveAllObjects(ObjectController *ObjectList, WorldPhysics pType)
{
	if (ObjectList == NULL || !LEMON_COLLISION_PHYSICS)
	{
		return MISSING_DATA;
	}

	if (pType != PLATFORMER)
	{
		return ACTION_DISABLED;
	}

	int objectCount = ObjectList->objectCount;
	Object *objects = ObjectList->objectComponents.Objects;
	PhysicsBox *boxes = ObjectList->objectComponents.PhysicsBoxes;

	int i = EngineSettings.MaxObjects - 1;
	int k = 0;

	while (i >= 0 && k < objectCount)
	{
		if (objects[i].reserved == AWAITING_RESOLUTION && objects[i].State != EMPTY_OBJECT)
		{
			if (fabs(boxes[i].PhysicsXVelocity) > 0.1)
			{
				float savedPos = boxes[i].yPos;
				boxes[i].yPos -= boxes[i].PhysicsYVelocity;
				ResolveAllXCollision(&boxes[i], ObjectList);
				boxes[i].yPos = savedPos;
			}

			if (fabs(boxes[i].PhysicsYVelocity) > 0.1)
			{	
				ResolveAllYCollision(&boxes[i], ObjectList);
			}

			objects[i].reserved = AWAITING_UPDATE;
			k++;
		}

		i--;
	}

	return LEMON_SUCCESS;
}


// **READ THIS Before adding new components**
// Because C does not have templates, you must write some boilerplate before adding a new component
// This involves creating the struct for the component itself, and the wrapper to contain an array of them alongside the SparseSet
// you must also create the associated add/remove/get functions, and put the initialisation into the initialiseComponents function
// it's recommended to basically just copy and paste as it should copy the functionality of the existing components
int initialiseComponents(ObjectController *input)
{
	if (input == NULL)
	{
		return MISSING_DATA;
	}

	ComponentData *compData = (&input->objectComponents);

	// initialise new components here
	initialiseSparseList(&compData->HealthComponents);
	initialiseSparseList(&compData->TileMaps);

	return LEMON_SUCCESS;
}

int initialiseSparseList(SparseList *input)
{
	int *sparse = input->sparse;

	// -1 is tombstone value (empty slot)
	for (int i = 0; i < EngineSettings.MaxObjects; i++)
	{
		sparse[i] = -1;
	}

	input->storedComponents = 0;

	memset(input->dense, 0, sizeof(ComponentType) * MAX_COMPONENT_SLOTS);
	memset(input->denseID, 0, sizeof(int) * MAX_COMPONENT_SLOTS);

	return LEMON_SUCCESS;
}

int removeComponents(Object *input, ObjectController *ObjectList)
{
	// remove new components here
	removeHealthComponent(input, ObjectList);
	removeTileMap(input, ObjectList);


	return LEMON_SUCCESS;
}

ComponentType* addComponent(Object *input, SparseList *List)
{
	if (input == NULL || input->index < 0 || List == NULL)
	{
		return NULL;
	}

	if (List->storedComponents >= MAX_COMPONENT_SLOTS)
	{
		return NULL;
	}

	// dont add/overwrite if the slot is already present (-1 indicates empty)
	if (List->sparse[input->index] >= 0)
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
	if (input == NULL || input->index < 0)
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

	// swap last and component to delete
	if (denseIndex != lastIndex)
	{
		ComponentType temp = denseList[denseIndex];
		int tempID = List->denseID[denseIndex];

		denseList[denseIndex] = denseList[lastIndex];
		List->denseID[denseIndex] = List->denseID[lastIndex];

		denseList[lastIndex] = temp;
		List->denseID[lastIndex] = tempID;
	}

	List->sparse[input->index] = -1;
	List->storedComponents--;

	return LEMON_SUCCESS;
}

ComponentType* getComponent(Object *input, SparseList *List)
{
	if (input == NULL || input->index < 0 || List == NULL)
	{
		return NULL;
	}

	int *sparse = List->sparse;
	ComponentType *dense = List->dense;

	if (sparse[input->index] < 0)
	{
		return NULL;
	}
	else
	{
		return &dense[sparse[input->index]];
	}
}


int addHealthComponent(Object *input, int Health, ObjectController *inputData)
{
	if (inputData == NULL)
	{
		return MISSING_DATA;
	}

	SparseList *List = &inputData->objectComponents.HealthComponents;
	HealthComponent *newHp = (HealthComponent*)addComponent(input, List);

	if (newHp == NULL)
	{
		return LEMON_ERROR;
	}

	newHp->health = Health;
	newHp->maxHealth = Health;
	newHp->TakeDamageCallBack = &takeDamageStandard;

	return LEMON_SUCCESS;
}

int removeHealthComponent(Object *input, ObjectController *inputData)
{
	SparseList *List = &inputData->objectComponents.HealthComponents;

	return removeComponent(input, List);
}

HealthComponent* getHealthComponent(Object *input, ObjectController *inputData)
{
	if (inputData == NULL)
	{
		return NULL;
	}

	SparseList *List = &inputData->objectComponents.HealthComponents;
	return (HealthComponent*)getComponent(input, List);
}

int takeDamageStandard(int damage, Object *input, HealthComponent *HP)
{
	if (HP == NULL)
	{
		return MISSING_DATA;
	}

	HP->health -= abs(damage);
	if (HP->health < 1)
	{
		MarkObjectForDeletion(input);
	}

	return LEMON_SUCCESS;
}

// convenience function
int inflictDamage(int damage, Object *input, ObjectController *ObjectList)
{
	HealthComponent *targetHp = getHealthComponent(input, ObjectList);

	if (targetHp != NULL)
	{
		(targetHp->TakeDamageCallBack)(damage, input, targetHp);
	}

	return LEMON_SUCCESS;
}


// idea; to deal damage simply add a damage component and increment the damage value, 
// then at the end of the tick it will run through all damage components to remove 
// them for objects that have no health component, and inflict damage to those that do
/*
int updateDamage(SparseList *damageList, ObjectController *ObjectList)
{
	int i = 0;
	Object *objects = ObjectList->objectComponents.Objects;
	Object *current = NULL;
	HealthComponent *objHp;

	for (int i = 0; i < damageList->storedComponents; i++)
	{
		current = &objects[damageList->denseID[i]];
		objHp = getHealthComponent(current, ObjectList);

		if (objHp == NULL)
		{
			continue;
		}

		(objHp->TakeDamageCallBack)(damageList.dense[i].damage, current, objHp);

		if (objHp->health <= 0)
		{
			MarkObjectForDeletion(current);
		}

		removeComponent(current, damageList);
	}

	return LEMON_SUCCESS;
}
*/

int addTileMap(Object *input, unsigned int centerTileX, unsigned int centerTileY, unsigned int tileSize, ObjectController *inputData)
{
	if (inputData == NULL)
	{
		return MISSING_DATA;
	}

	SparseList *List = &inputData->objectComponents.TileMaps;
	TileMap *newMap = (TileMap*)addComponent(input, List);

	if (newMap == NULL)
	{
		return LEMON_ERROR;
	}

	newMap->centerTileX = centerTileX;
	newMap->centerTileY = centerTileY;
	newMap->tileSize = clamp(tileSize, 1, tileSize);

	return LEMON_SUCCESS;
}

int removeTileMap(Object *input, ObjectController *inputData)
{
	SparseList *List = &inputData->objectComponents.TileMaps;

	return removeComponent(input, List);
}

TileMap* getTileMap(Object *input, ObjectController *inputData)
{
	if (inputData == NULL)
	{
		return NULL;
	}

	SparseList *List = &inputData->objectComponents.TileMaps;
	return (TileMap*)getComponent(input, List);
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

	PlayAnimationByIndex(animation, repeatCount, particleDisplay);


	switch (particleDisplay->currentAnimation)
	{
	case SPARKLE:
		SetObjectDirection(particle, (double)(rand() % 180));
		break; 

	case 0:
		MarkObjectForDeletion(particle);
		break;

		default:
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
	// arg4 = timerLength
	// arg3 = timer


	if (PlayerInteractingWithBox(player, gateSwitch->ObjectBox))
	{
		gateSwitch->Action = (gateSwitch->Action + 1) % 2;
		gateSwitch->arg3 = gateSwitch->arg4;

		// Update any gates
		toggleGateSwitch(gateSwitch, ObjectList);

		return LEMON_SUCCESS;
	}


	if (gateSwitch->arg4 > 0 && gateSwitch->Action == 1)
	{
		gateSwitch->arg3--;

		if (gateSwitch->arg3 < 1)
		{
			gateSwitch->Action = 0;

			toggleGateSwitch(gateSwitch, ObjectList);
		}
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
	inputObject->ObjectBox->flag = ENTITY_SOLID;


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
	int timer = platform->arg4;

	// Wait to change direction
	if (platform->Action == 0)
	{
		platform->Interrupt++;

		if (platform->Interrupt > timer)
		{
			platform->Interrupt = 0;
			platform->Action = (XPos <= leftBound) ? 1 : 2;
		}
	}

	// Accelerate
	if (platform->Action == 1 && platformBox->xVelocity < maxSpeed && XPos2 <= rightBound)
	{
		platformBox->xVelocity += 0.5;
	}

	if (platform->Action == 2 && platformBox->xVelocity > -maxSpeed && XPos >= leftBound)
	{
		platformBox->xVelocity -= 0.5;
	}

	// Deccelerate
	if ((XPos2 >= rightBound && platform->Action == 1) || (XPos <= leftBound && platform->Action == 2))
	{
		ApplyFriction(platformBox, 1.0, 0.9, 1.0);
		platform->Action = 0;
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
	int timer = platform->arg4;

	// Wait to change direction
	if (platform->Action == 0)
	{
		platform->Interrupt++;

		if (platform->Interrupt > timer)
		{
			platform->Interrupt = 0;
			platform->Action = (YPos <= bottomBound) ? 1 : 2;
		}
	}

	// Accelerate
	if (platform->Action == 1 && platformBox->yVelocity < maxSpeed && YPos2 <= topBound)
	{
		platformBox->yVelocity += 0.5;
	}

	if (platform->Action == 2 && platformBox->yVelocity > -maxSpeed && YPos >= bottomBound)
	{
		platformBox->yVelocity -= 0.5;
	}

	// Deccelerate
	if ((YPos2 >= topBound && platform->Action == 1) || (YPos <= bottomBound && platform->Action == 2))
	{
		ApplyFriction(platformBox, 1.0, 1.0, 0.9);
		platform->Action = 0;
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

	if (PlayerInteractingWithBox(Player, Door->ObjectBox) && Player.PlayerPtr->State == DEFAULT)
	{
		float savedXPos = Player.PlayerBox->xPos;
		float savedYPos = Player.PlayerBox->yPos;

		GoTo(Player.PlayerPtr, Door->arg1, Door->arg2);

		if (GetCollidingObject(Player.PlayerBox, GameWorld->ObjectList) != NULL)
		{
			SayText("The door seems to be blocked on the \nother side.", NO_PORTRAIT, BASIC_FADE, GameWorld);
			SayTextOption("", NO_PORTRAIT, BASIC_FADE, GameWorld, 3, 
				"W-what? What is it?", 			NO_ACTION, 
				"I SCREAM I SHOUT", 			NO_ACTION,
				"Actually i'm okay with this",	NO_ACTION);
		}
		else
		{
			// Test dialogue, replace with prompt "Go through door?" or something
			SayText("I", NO_PORTRAIT, BASIC_FADE, GameWorld);
			SayText("....Or is it?\n\rIt just looks like a big pink and black rectangle...", "Test_Face", BASIC_FADE, GameWorld);

			SayTextOption("Enter the Door?", "Test_Face", BASIC_FADE, GameWorld, 3, 
				"Yes", &TeleportPlayerToExitDoor, Door, 
				"No", NO_ACTION,
				"hm... lemme think about it", &StartCutscene, TEST_SCENE_2);
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

	GoTo(Player->PlayerPtr, Door->arg1, Door->arg2);

	PlaySound("DoorOpen", "Objects", OBJECT_SFX, 1.0);
			
	ResetPlayer(Player);

	return LEMON_SUCCESS;
}


int UpdateLevelDoor(PlayerData Player, Object *Door, World *GameWorld)
{
	// arg1: Level to load
	// arg2: Open/close state
	if (Player.PlayerPtr == NULL || Door == NULL)
	{
		return MISSING_DATA;
	}

	if (Door->arg2 == 0 && PlayerInteractingWithBox(Player, Door->ObjectBox) && Player.PlayerPtr->State == DEFAULT)
	{
		Door->arg2 = 1;
		SayText("It's a door.\f.\f.\r \nIt eminates a strange glow.", NO_PORTRAIT, BASIC_FADE, GameWorld);
		char phrase[MAX_TEXT_LENGTH] = {0};
		snprintf(phrase, MAX_TEXT_LENGTH, "This Door will send you to level %d!", Door->arg1);
		SayText(phrase, NO_PORTRAIT, BASIC_FADE, GameWorld);
	}

	if (Door->arg2 > 0 && GameWorld->TextQueue == NULL)
	{
		switchLevel(Door->arg1, GameWorld);
		Door->arg2 = 0;
	}

	return LEMON_SUCCESS;
} 


int ApplyGravity(Object *inputObject, World *GameWorld)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL || GameWorld == NULL)
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

		inputObject->reserved = AWAITING_MAGNETISATION;	
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
	}

	if (yFriction > 0.0)
	{
		inputBox->yVelocity *= yFriction;
	}

	if (fabs(inputBox->xVelocity) < 0.001)
	{
		inputBox->xVelocity = 0.0;
	}

	if (fabs(inputBox->yVelocity) < 0.001)
	{
		inputBox->yVelocity = 0.0;
	}

	if (fabs(inputBox->forwardVelocity) < 0.001)
	{
		inputBox->forwardVelocity = 0.0;
	}
		

	return LEMON_SUCCESS;
}


Object* CheckForGround(PhysicsBox *movingBox, World *GameWorld)
{
	if (GameWorld == NULL || movingBox == NULL || movingBox->solid == UNSOLID || !LEMON_COLLISION_PHYSICS)
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
int resetGroundCheck(PhysicsBox *inputBox, World *GameWorld)
{
	if (GameWorld != NULL && GameWorld->PhysicsType != PLATFORMER)
	{
		return ACTION_DISABLED;
	}

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


	input->ObjectBox->xPos = xPos - (input->ObjectBox->xSize >> 1);
	input->ObjectBox->yPos = yPos - (input->ObjectBox->ySize >> 1);

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

	int camX = -(screenWidth >> 1);
	int camY = -(screenHeight >> 1);
	PhysicsBox *objBox = inputObject->ObjectBox;

	if (getDisplayLayer(inputObject) != HUD)
	{
		camX +=	GameWorld->MainCamera.CameraX;
		camY += GameWorld->MainCamera.CameraY;
	}

	int camXRight = camX + screenWidth;
	int camYTop = camY + screenHeight;

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
	mouseBox.xPos = MouseInput.xPos;
	mouseBox.yPos = MouseInput.yPos;

	if (getDisplayLayer(input) != HUD)
	{
		mouseBox.xPos = getMouseXCamRelative(inputCam);
		mouseBox.yPos = getMouseYCamRelative(inputCam);
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
		mouseBox.xPos = getMouseXCamRelative(inputCam);
		mouseBox.yPos = getMouseYCamRelative(inputCam);
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
		case ENTITY_SOLID:
		if (compareBox->solid != ENTITY)
		{
			return 0;
		}
		break;

		case IGNORE_SOLID:
		if (compareBox->solid == SOLID)
		{
			return 0;
		}
		break;

		default:
		break;
	}

	switch(compareBox->flag)
	{
		case ENTITY_SOLID:
		if (inputBox->solid != ENTITY)
		{
			return 0;
		}
		break;

		case IGNORE_SOLID:
		if (inputBox->solid == SOLID)
		{
			return 0;
		}
		break;

		default:
		break;
	}


	switch(inputBox->solid)
	{		
		case JUMP_THROUGH:
		{
			if (compareBox->yVelocity > 0.0 || compareBox->crouch == true || compareBox->prevYPos < (inputBox->prevYPos + inputBox->ySize - 1) )
			{
				return 0;
			}
		} break;

		case UNSOLID:
		return 0;

		case ENTITY:
		if (compareBox->solid == ENTITY)
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
			if (inputBox->yVelocity > 0.0 || inputBox->crouch == true || inputBox->prevYPos < (compareBox->prevYPos + compareBox->ySize - 1) )
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
	if (inputBox == NULL || inputBox->solid == UNSOLID || ObjectList == NULL || !LEMON_COLLISION_PHYSICS)
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
			return &objects[list[i]];
		}
	}

	return NULL;
}


// returns pointer of object overlapping, NULL if no object is detected; has n^2 complexity, not great!
Object* GetCollidingObject(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	if (inputBox == NULL || inputBox->solid == UNSOLID || ObjectList == NULL || !LEMON_COLLISION_PHYSICS)
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

		if (currentObject->ObjectBox->solid == UNSOLID || checkBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox) == false)
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
	if (inputBox == NULL || ObjectList == NULL || !LEMON_COLLISION_PHYSICS)
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
	if (inputBox == NULL || ObjectList == NULL || !LEMON_COLLISION_PHYSICS)
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
	if (inputBox == NULL || inputBox->solid == UNSOLID || ObjectList == NULL || !LEMON_COLLISION_PHYSICS)
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
	if (ObjectList == NULL || inputBox == NULL || !LEMON_COLLISION_PHYSICS)
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
	if (ObjectList == NULL || inputBox == NULL || !LEMON_COLLISION_PHYSICS)
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

	ObjectController *ObjectList = GameWorld->ObjectList;
	depthCounter = 0;
	PhysicsBox *inputBox = inputObject->ObjectBox;

	moveObjectX(inputBox, ObjectList);
	moveObjectY(inputBox, ObjectList);
	moveObjectForward(inputBox, ObjectList);

	inputBox->xPos = fClamp(inputBox->xPos, -EngineSettings.WorldBoundX, EngineSettings.WorldBoundX - inputBox->xSize);
	inputBox->yPos = fClamp(inputBox->yPos, -EngineSettings.WorldBoundY, EngineSettings.WorldBoundY - inputBox->ySize);

	resetGroundCheck(inputBox, GameWorld);

	return LEMON_SUCCESS;
}


int moveObjectX(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	if (fabs(inputBox->xVelocity) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}

	inputBox->xPos += inputBox->xVelocity;

	ResolveAllXCollision(inputBox, ObjectList);

	return LEMON_SUCCESS;
}



int moveObjectY(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	if (fabs(inputBox->yVelocity) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}

	inputBox->yPos += inputBox->yVelocity;

	ResolveAllYCollision(inputBox, ObjectList);


	return LEMON_SUCCESS;
}


CollideType evaluateCollideMode(PhysicsBox *movingBox, PhysicsBox *collideBox)
{
	if (movingBox == NULL || collideBox == NULL)
	{
		return NO_COLLIDE_TYPE;
	}

	if (movingBox->collideMode == IMPACT)
	{
		return IMPACT;
	}


	int canPush = (movingBox->solid != PUSHABLE_SOLID) || (collideBox->solid != ENTITY);
	int collidePushable = collideBox->solid == PUSHABLE_SOLID || collideBox->solid == ENTITY;
	
	
	if (movingBox->collideMode == PUSH || (canPush && collidePushable) )
	{
		return PUSH;
	}

	return IMPACT;
}


int AdjustDirection(PhysicsBox *movingBox, World *GameWorld)
{	
	if (GameWorld == NULL || GameWorld->ObjectList == NULL || movingBox == NULL || !LEMON_COLLISION_PHYSICS)
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


int moveObjectForward(PhysicsBox *movingBox, ObjectController *ObjectList)
{
	if (movingBox == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (fabs(movingBox->forwardVelocity) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}

	// Unsolid objects do not have to do collision detection so it skips the rest of the function by moving all steps instantly
	if (movingBox->solid == UNSOLID || !LEMON_COLLISION_PHYSICS)
	{
		movingBox->xPos += movingBox->forwardVelocity * sin(movingBox->direction * DEGREE_TO_RADIAN_PI);
		movingBox->yPos += movingBox->forwardVelocity * cos(movingBox->direction * DEGREE_TO_RADIAN_PI);
		
		return LEMON_SUCCESS;
	}

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

			if (evaluateCollideMode(movingBox, currentObject->ObjectBox) == PUSH && depthCounter < COLLISION_DEPTH)
			{
				PhysicsBox *collideBox = currentObject->ObjectBox;

				float tempVelocity = collideBox->forwardVelocity;
				double tempDirection = collideBox->direction;
				float collideXPos = collideBox->xPos;
				float collideYPos = collideBox->yPos;

				
				collideBox->forwardVelocity = orientation * travelCount;
				collideBox->direction = movingBox->direction;

				moveObjectForward(collideBox, ObjectList);

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
	if (movingBox == NULL || ObjectList == NULL || !LEMON_COLLISION_PHYSICS)
	{
		return MISSING_DATA;
	}

	if (movingBox->solid == UNSOLID)
	{
		return ACTION_DISABLED;
	}


	int count = 0;

	Object *currentObject = GetCollidingObject(movingBox, ObjectList);

	while (currentObject != NULL && count < COLLISION_CYCLES)
	{
		if (evaluateCollideMode(movingBox, currentObject->ObjectBox) == PUSH)
		{
			PhysicsBox *collideBox = currentObject->ObjectBox;
			ResolveXCollisionByPush(movingBox, collideBox);

			float prevXVel = collideBox->xVelocity;
			SolidType prevType = movingBox->solid;
			CollideType prevCollideMode = collideBox->collideMode;

			movingBox->solid = UNSOLID;
			collideBox->collideMode = movingBox->collideMode;
			collideBox->xVelocity = movingBox->xVelocity;

			ResolveAllXCollision(collideBox, ObjectList);

			movingBox->solid = prevType;
			collideBox->collideMode = prevCollideMode;
			collideBox->xVelocity = prevXVel;

			if (CheckBoxCollidesBox(movingBox, currentObject->ObjectBox) == 1)
			{
				ResolveXCollision(movingBox, currentObject->ObjectBox, ObjectList);
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
				ClimbSlope(movingBox, compareBox, ObjectList);
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
	if (movingBox == NULL || ObjectList == NULL || !LEMON_COLLISION_PHYSICS)
	{
		return MISSING_DATA;
	}
	
	if (movingBox->solid == UNSOLID)
	{
		return ACTION_DISABLED;
	}


	int count = 0;
	Object *currentObject = GetCollidingObject(movingBox, ObjectList);

	while (currentObject != NULL && count < COLLISION_CYCLES)
	{
		if (evaluateCollideMode(movingBox, currentObject->ObjectBox) == PUSH)
		{
			PhysicsBox *collideBox = currentObject->ObjectBox;
			ResolveYCollisionByPush(movingBox, collideBox);

			float prevYVel = collideBox->yVelocity;
			SolidType prevType = movingBox->solid;
			CollideType prevCollideMode = collideBox->collideMode;

			collideBox->yVelocity = movingBox->yVelocity;
			movingBox->solid = UNSOLID;
			collideBox->collideMode = movingBox->collideMode;

			ResolveAllYCollision(collideBox, ObjectList);

			movingBox->solid = prevType;
			collideBox->collideMode = prevCollideMode;
			collideBox->yVelocity = prevYVel;

			if (CheckBoxCollidesBox(movingBox, currentObject->ObjectBox) == 1)
			{
				ResolveYCollision(movingBox, currentObject->ObjectBox);
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
			if (movingBox->yVelocity < 0.1 && movingBox->crouch == false)
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


int ClimbSlope(PhysicsBox *inputBox, PhysicsBox *compareBox, ObjectController *ObjectList)
{		
	if (inputBox == NULL || compareBox == NULL || ObjectList == NULL)
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

	moveObjectForward(inputBox, ObjectList);

	inputBox->forwardVelocity = savedForwardVelocity;
	inputBox->direction = savedDirection;

	return LEMON_SUCCESS;
}
