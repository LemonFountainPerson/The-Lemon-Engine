#include "gameObjects.h"



Object* AddObject(World *GameWorld, int objectID, int xPos, int yPos, int xSize, int ySize, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return NULL;
	}

	ObjectController *ObjectList = GameWorld->ObjectList;
	int maxObjects = EngineSettings.MaxObjects;

	if (GameWorld->GameState == LOADING)
	{
		maxObjects -= EngineSettings.ReservedObjects;
	}

	if (ObjectList->objectCount + ObjectList->cachedCount >= maxObjects)
	{
		return NULL;
	}


	if (objectID >= OBJECT_TYPE_COUNT || objectID < LEVEL_FLAG_OBJ)
	{
		putConsoleStrInt("\nThis object is not defined! Type: ", objectID);
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
		putConsoleStrIntStr("\nCreated object type: ", objectID, "\n");
	}


	// Default settings
	if (xSize <= 0)
	{
		newObject->ObjectBox->xSize = X_TILESCALE;
	}
	else
	{
		newObject->ObjectBox->xSize = xSize;
	}
		
	if (ySize <= 0)
	{
		newObject->ObjectBox->ySize = Y_TILESCALE;
	}
	else
	{
		newObject->ObjectBox->ySize = ySize;
	}
	
	newObject->arg1 = arg1;
	newObject->arg2 = arg2;
	newObject->arg3 = arg3;
	newObject->arg4 = arg4;
	newObject->arg5 = arg5;

	newObject->layer = MIDDLEGROUND;

	newObject->ObjectDisplay->currentSprite = 1;

	newObject->ObjectBox->xPos = xPos;
	newObject->ObjectBox->yPos = yPos;


	// Set Object parameters
	switch (objectID)
	{
		case PROJECTILE:
			newObject->ObjectBox->xSize = X_TILESCALE;
			newObject->ObjectBox->ySize = 8;

			newObject->ObjectBox->forwardVelocity = 20.0;
			newObject->ObjectBox->solid = UNSOLID;

			newObject->ObjectDisplay->RenderModeOverride = SINGLE;
			break;


		case PLAYER_OBJECT:
			InitialisePlayerObject(newObject, GameWorld);
			break;

		case LEVEL_DOOR:
			snapPositionToTileGrid(newObject->ObjectBox, xPos, yPos);
			newObject->ObjectBox->solid = JUMP_THROUGH;
			newObject->ObjectBox->xSize = X_TILESCALE << 1;
			newObject->ObjectBox->ySize = Y_TILESCALE * 3;
			break;

		case DOOR:
			snapPositionToTileGrid(newObject->ObjectBox, xPos, yPos);
			newObject->ObjectBox->solid = JUMP_THROUGH;
			newObject->ObjectBox->xSize = X_TILESCALE << 1;
			newObject->ObjectBox->ySize = Y_TILESCALE * 3;

			if (newObject->arg3 == 0)
			{
				newObject->arg3 = 1;
				newObject->ParentObject = AddObject(GameWorld, DOOR, arg1, arg2, xSize, ySize, xPos, yPos, 1, 0, 0);

				newObject->ParentObject->ParentObject = newObject;
			}
			
		break;

		case SOLID_BLOCK:
			snapPositionToTileGrid(newObject->ObjectBox, xPos, yPos);
			newObject->ObjectBox->xSize = xSize * X_TILESCALE;
			newObject->ObjectBox->ySize = ySize * Y_TILESCALE;
			newObject->State = STATIC;
			strcpy(newObject->name, "Block");

			if (arg1 > 0)
			{
				switchObjectSprite(arg1, newObject);
			}
			else
			{
				newObject->ObjectDisplay->RenderModeOverride = DO_NOT_RENDER;
			}
			break;


		case FLAT_SLOPE_FLOOR:
		//Angle: Y = (X * ySize/xSize)
		//Angle: X = (Y / (ySize/xSize))
			snapPositionToTileGrid(newObject->ObjectBox, xPos, yPos);
			newObject->ObjectBox->solid = FLAT_SLOPE;
			newObject->State = STATIC;

			if (arg1 == -1)
			{
				newObject->ObjectBox->xFlip = -1;
			}

			if (arg2 == -1)
			{
				newObject->ObjectBox->yFlip = -1;
			}


			if (xSize + ySize > 256)
			{
				switchObjectSprite(3, newObject);
			}
			else if (xSize + ySize > 128)
			{
				switchObjectSprite(2, newObject);
			}
			break;


		case JUMP_THRU_BLOCK:
			snapPositionToTileGrid(newObject->ObjectBox, xPos, yPos);
			newObject->ObjectBox->solid = JUMP_THROUGH;
			newObject->ObjectBox->xSize = xSize * X_TILESCALE;
			newObject->ObjectBox->ySize = ySize * Y_TILESCALE;
			break;

		case COIN:
		// Coin
			newObject->arg1 = arg1;
			newObject->arg2 = arg2;
			newObject->arg3 = 1;
			newObject->ObjectBox->solid = UNSOLID;
			newObject->ParentLink = FINAL_LINK;
			PlayAnimation("Coin_Spin", 0, newObject->ObjectDisplay);
			snapPositionToTileGrid(newObject->ObjectBox, xPos, yPos);
			break;

		
		case SPRING:
		// spring
			newObject->arg1 = arg1;
			SetObjectDirection(newObject, (double)(90 + arg2), ROTATE_ALL);
			newObject->ObjectBox->solid = UNSOLID;
			snapPositionToTileGrid(newObject->ObjectBox, xPos, yPos);
			break;


		case VERTICAL_GATE:
		// Medium sized, vertical gate
			newObject->ObjectBox->solid = IGNORE_SOLID;
			newObject->arg1 = arg1;
			newObject->arg2 = 0;
			newObject->arg3 = yPos;
			newObject->arg4 = arg2;
			newObject->ObjectBox->ySize = ySize * Y_TILESCALE;
			newObject->ObjectBox->xSize = xSize * X_TILESCALE;
			snapPositionToTileGrid(newObject->ObjectBox, xPos, yPos);
			break;

		case HORIZONTAL_GATE:
		// Medium sized, horizontal gate
			newObject->ObjectBox->solid = IGNORE_SOLID;
			newObject->arg1 = arg1;
			newObject->arg2 = 0;
			newObject->arg3 = xPos;
			newObject->arg4 = arg2;
			newObject->ObjectBox->ySize = ySize * Y_TILESCALE;
			newObject->ObjectBox->xSize = xSize * X_TILESCALE;
			snapPositionToTileGrid(newObject->ObjectBox, xPos, yPos);
			break;


		case GATE_SWITCH:
		// switch for gate - arg1 is ID to match switch to gate, arg2 denotes type of switch (0 = or switch, 1 = and switch)
			newObject->arg1 = arg1;
			newObject->arg2 = arg2;
			newObject->ObjectBox->solid = UNSOLID;
			snapPositionToTileGrid(newObject->ObjectBox, xPos, yPos);
			break;


		case GATE_SWITCH_TIMED:
		// switch for gate - arg2 denotes type of switch (<0 = or switch, >0 = and switch) arg2 also denotes timer length
			newObject->arg1 = arg1;

			if (arg2 < 0)
			{
				newObject->arg2 = SINGLE_SWITCH;
			}
			else
			{
				newObject->arg2 = CHAIN_SWITCH;
			}
			newObject->arg5 = abs(arg2);
			newObject->ObjectBox->solid = UNSOLID;
			snapPositionToTileGrid(newObject->ObjectBox, xPos, yPos);
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
			break;


		case PUSHABLE_BOX:
			newObject->ObjectBox->solid = PUSHABLE_SOLID;
			break;


		default:
			break;
	}


	newObject->ObjectBox->xPosRight = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;
	newObject->ObjectBox->yPosTop = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;
	newObject->ObjectBox->prevXPos = newObject->ObjectBox->xPos;
	newObject->ObjectBox->prevYPos = newObject->ObjectBox->yPos;

	UpdateObjectDisplay(newObject);

	
	return newObject;
}

Object* AddNamedObject(World *GameWorld, const char name[], int objectID, int xPos, int yPos)
{
	if (name == NULL || strlen(name) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *createdObject = AddObject(GameWorld, objectID, xPos, yPos, 0, 0, 0, 0, 0, 0, 0);

	if (createdObject == NULL)
	{
		return NULL;
	}

	strcpy(createdObject->name, name);

	return createdObject;
}

Object* AddObjectWithParent(World *GameWorld, Object *ParentObject, int objectID, int xPos, int yPos, int xSize, int ySize, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	if (ParentObject == NULL)
	{
		return NULL;
	}

	Object *newObject = AddObject(GameWorld, objectID, xPos, yPos, xSize, ySize, arg1, arg2, arg3, arg4, arg5);

	if (newObject != NULL)
	{
		newObject->ParentObject = ParentObject;
	
		UpdateParentChildLink(newObject);
	}

	return newObject;
}


Object* AddParticle(World *GameWorld, ParticleSubType animation, int xPos, int yPos, int repeatCount, int particleLifeTime)
{
	return AddObject(GameWorld, PARTICLE, xPos, yPos, 0, 0, animation, repeatCount, particleLifeTime, 0, 0);
}


Object* getNewObject(ObjectType objectID, ObjectController *ObjectList)
{
	Object *newObject = NULL;

	if (EngineSettings.ObjectPreAllocationEnabled == 1)
	{
		newObject = findNewObject(ObjectList);
	}
	else
	{
		newObject = createNewObject();
	}

	initialiseGenericObject(newObject, objectID, ObjectList);

	return newObject;
}


int initialiseGenericObject(Object *inputObject, ObjectType objectID, ObjectController *ObjectList)
{
	if (inputObject == NULL || ObjectList == NULL || inputObject->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	// reset values
	resetPhysicsRect(inputObject->ObjectBox);
	resetDisplayData(inputObject->ObjectDisplay);


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

	inputObject->layer = MIDDLEGROUND;
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
	inputObject->arg5 = 0;

	return LEMON_SUCCESS;
}


Object* createNewObject(void)
{
	Object *newObject = malloc(sizeof(Object));

	if (newObject == NULL)
	{
		putConsoleString("\nError: Could not allocate memory for new object.\n");
		return NULL;
	}

	memset(newObject, 0, sizeof(Object));


	newObject->ObjectBox = createPhysicsRect(SOLID);

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


	// Delete child objects
	if ((input->ParentLink & FINAL_LINK) == 0)
	{
		Object *currentObject = ObjectList->firstObject;
		int i = 0;
		
		while (currentObject != NULL && i < ObjectList->objectCount)
		{
			if (currentObject->ParentObject == input)
			{
				if (nextObject == currentObject)
				{
					currentObject = deleteObject(currentObject, ObjectList);
					nextObject = currentObject;
				}
				else
				{
					currentObject = deleteObject(currentObject, ObjectList);
				}

				continue;
			}
			
			currentObject = currentObject->nextObject;
			i++;
		}
	}
	
	
	// Delete associated frame function(s)
	FrameUpdateFunction *CurrentFunction = (FrameUpdateFunction *)ObjectList->FrameUpdates;
	FrameUpdateFunction *DeleteFunction;

	while (CurrentFunction != NULL)
	{
		if (CurrentFunction->inputObject == input)
		{
			DeleteFunction = CurrentFunction;
			CurrentFunction = CurrentFunction->nextFunction;

			if (ObjectList->FrameUpdates == DeleteFunction)
			{
				ObjectList->FrameUpdates = (void *)CurrentFunction;
			}

			free(DeleteFunction);
		}
		else
		{
			CurrentFunction = CurrentFunction->nextFunction;
		}
	}
	

	if (EngineSettings.ObjectPreAllocationEnabled == 1)
	{
		if (ObjectList->availableSlots != NULL)
		{
			ObjectList->availableSlots->prevObject = input;
		}

		input->nextObject = ObjectList->availableSlots;
		ObjectList->availableSlots = input;
		input->State = EMPTY_OBJECT;

		return nextObject;
	}
	
	free(input->ObjectDisplay);
	free(input->ObjectBox);
	free(input);

	return nextObject;
}


int SetObjectName(const char name[], Object *inputObject)
{
	if (name == NULL || strlen(name) > OBJECT_NAME_LENGTH || inputObject == NULL)
	{
		return MISSING_DATA;
	}

	strcpy(inputObject->name, name);

	return LEMON_SUCCESS;
}


Object* FindObject(const char name[], ObjectController ObjectList)
{
	if (name == NULL || strlen(name) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *currentObj = ObjectList.firstObject;

	while (currentObj != NULL && strcmp(currentObj->name, name) != 0)
	{
		currentObj = currentObj->nextObject;
	}

	return currentObj;
}


Object* FindObjectID(const char name[], int objectID, ObjectController ObjectList)
{
	if (name == NULL || strlen(name) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *currentObj = ObjectList.firstObject;

	while (currentObj != NULL && (objectID != currentObj->ObjectID || strcmp(currentObj->name, name) != 0))
	{
		currentObj = currentObj->nextObject;
	}

	return currentObj;
}


int snapPositionToTileGrid(PhysicsRect *ObjectBox, int xPos, int yPos)
{
	if (ObjectBox == NULL)
	{
		return MISSING_DATA;
	}
	
	ObjectBox->xPos = (double)(xPos - (xPos % X_TILESCALE));
	ObjectBox->yPos = (double)(yPos - (yPos % Y_TILESCALE));
	
	return LEMON_SUCCESS;
}


PhysicsRect* createPhysicsRect(SolidType inputSolid)
{
	PhysicsRect *newRect = malloc(sizeof(PhysicsRect));

	if (newRect == NULL)
	{
		return NULL;
	}

	resetPhysicsRect(newRect);
	newRect->solid = inputSolid;

	return newRect;
}


int resetPhysicsRect(PhysicsRect *input)
{
	if (input == NULL)
	{
		return MISSING_DATA;
	}

	input->xPos = 0.0;
	input->yPos = 0.0;
	input->prevXPos = 0.0;
	input->prevYPos = 0.0;
	input->xPosRight = 0.0;
	input->yPosTop = 0.0;
	input->xSize = 0;
	input->ySize = 0;
	input->xFlip = 1;
	input->yFlip = 1;
	input->direction = RADIAN_90;

	input->solid = SOLID;
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
	input->direction = RADIAN_90;
	input->spriteXOffset = 0;
	input->spriteYOffset = 0;
	input->pixelXOffset = 0;
	input->pixelYOffset = 0;

	input->currentAnimation = 0;
	input->frameBuffer = NULL;
	input->animationBuffer = NULL;
	input->animationTick = 0.0;
	input->animationLoopCount = 0;
	input->transparencyEffect = 1.0;
	input->hidden = false;

	return LEMON_SUCCESS;
}


SpriteSet* createObjectSpriteSet(ObjectController *ObjectList, int ObjectID)
{
	if (ObjectList == NULL || ObjectID <= LEVEL_FLAG_OBJ)
	{
		return NULL;
	}

	// Check for pre-existing spriteset
	SpriteSet *currentSetPtr;
	currentSetPtr = ObjectList->startSpriteSetPtr;

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

	if (currentSetPtr == NULL && i == 0)
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
	ObjectList->spriteSetCount = i + 1;
	newSet->Animations = NULL;

	LoadSpritesAndAnimationData(newSet, ObjectID);
	
	return newSet;
}


// These are convienience functions shorthand for calling the normal switchSprite functions without switching spriteset
int switchObjectSprite(int spriteID, Object *inputObject)
{
	if (inputObject == NULL || inputObject->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	DisplayData *ObjectDisplay = inputObject->ObjectDisplay;

	if (inputObject->ObjectID == LEVEL_FLAG_OBJ || (ObjectDisplay->spriteBuffer != NULL && ObjectDisplay->spriteBuffer->spriteID == spriteID) )
	{
		ObjectDisplay->currentSprite = spriteID;
		return EXECUTION_UNNECESSARY;
	}


	switchSprite(spriteID, USE_CURRENT_SPRITESET, ObjectDisplay);
		
	return LEMON_SUCCESS;
}


int switchObjectSpriteName(const char spriteName[], Object *inputObject)
{
	if (inputObject == NULL || inputObject->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	DisplayData *ObjectDisplay = inputObject->ObjectDisplay;

	if (ObjectDisplay->spriteBuffer != NULL && strcmp(ObjectDisplay->spriteBuffer->spriteName, spriteName) == 0)
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


	while (ObjectList->cachedFirstObject != NULL)
	{
		deleteObject(ObjectList->cachedFirstObject, ObjectList);
	}

	ObjectList->cachedCount = 0;
	ObjectList->cachedFirstObject = NULL;
	ObjectList->cachedLastObject = NULL;

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
	inputObject->ObjectDisplay->size = 1.5;


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

	Object *nextPtr;
	nextPtr = input->nextObject;

	Object *prevPtr;
	prevPtr = input->prevObject;


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


int cacheObjects(ObjectController *ObjectList, PhysicsRect boundingBox)
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

		if (checkBoxOverlapsBoxBroad(&boundingBox, temp->ObjectBox) == 0)
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

	if (GameWorld->GameState == EMPTY_GAME || GameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}

	ObjectController *ObjectList = GameWorld->ObjectList;

	if (ObjectList == NULL || ObjectList->firstObject == NULL)
	{
		return MISSING_DATA;
	}

	// UNIMPLEMENTED
	// Set-up spatial-partioned quad tree for optimised collision detection
	//deleteQuadTree(ObjectList->QuadTreeRoot);
	//ObjectList->QuadTreeRoot = NULL;
	//constructQuadTree(ObjectList);


	// Set previous position values - removing this will break object magnetisation, parent movement links and certain collision detections
	UpdatePreviousPositions(ObjectList);


	// Update Behaviour
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
	currentObject = ObjectList->firstObject;

	while (currentObject != NULL)
	{
		if (currentObject->State == TO_BE_DELETED || currentObject->ObjectBox == NULL || currentObject->ObjectDisplay == NULL)
		{
			if (PLAYER_OBJECT == currentObject->ObjectID)
			{
				PlayerObjectAboutToBeDeleted(&GameWorld->Player);
			}

			currentObject = deleteObject(currentObject, ObjectList);
		}
		else
		{
			UpdatePhysicsState(currentObject, GameWorld);
			UpdateParentChildLink(currentObject);

			currentObject = currentObject->nextObject;
		}
	}


	ResolveAllObjects(GameWorld);


	return LEMON_SUCCESS;
}


int ObjectBehaviour(World *GameWorld, Object *inputObject)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL || inputObject->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	inputObject->reserved = PHYSICS_DISABLED;

	if (inputObject->State == STATIC || inputObject->State < DEFAULT)
	{
		return EXECUTION_UNNECESSARY;
	}

	bool gameStateDisable = GameWorld->GameState == CUTSCENE && inputObject->State != ACTOR;
	bool immuneObject = inputObject->ObjectID == UI_ELEMENT || inputObject->ObjectID == UI_TEXT || inputObject->ObjectID == PARTICLE;

	if ((!immuneObject && gameStateDisable) || inputObject->State == PAUSE_BEHAVIOUR || inputObject->State == IN_INVENTORY)
	{
		return ACTION_DISABLED;
	}
		
	// This variable is used to communicate with other components of the engine, do not modify unless you know what you're doing!
	inputObject->reserved = AWAITING_MAGNETISATION;	


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

			if (PlayerInteractingWithBox(GameWorld->Player, inputObject->ObjectBox) == 1)
			{
				if (GameWorld->Player.PlayerBox->xPos > inputObject->ObjectBox->xPos + (inputObject->ObjectBox->xSize >> 1))
				{
					inputObject->ObjectBox->forwardVelocity = -15.0;
				}
				else
				{
					inputObject->ObjectBox->forwardVelocity = 15.0;
				}

				if (GameWorld->Player.PlayerBox != NULL)
				{
					inputObject->ObjectBox->forwardVelocity += GameWorld->Player.PlayerBox->forwardVelocity;
				}
			}
			break;


		case PLAYER_OBJECT:
			UpdatePlayer(&GameWorld->Player, GameWorld);
			break;


		case PROJECTILE:
			{
				inputObject->arg1++;

				if (inputObject->arg1 > 400)
				{
					MarkObjectForDeletion(inputObject);
				}

				Object *hitTarget;
				hitTarget = GetOverlappingObjectAllSolids(inputObject, GameWorld->ObjectList);

				if (hitTarget == NULL || hitTarget == GameWorld->Player.PlayerPtr)
				{
					break;
				}	

				MarkObjectForDeletion(inputObject);
				if (hitTarget->ObjectBox->solid != ENTITY)
				{
					AddParticle(GameWorld, SPARKLE, inputObject->ObjectBox->xPos + (inputObject->ObjectBox->xSize >> 1), inputObject->ObjectBox->yPos + (inputObject->ObjectBox->ySize >> 1), 1, 0);
				}
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
	if (inputObject == NULL || inputObject->ObjectDisplay == NULL)	{ return MISSING_DATA; }

	if (inputObject->State < DEFAULT)
	{
		return ACTION_DISABLED;
	}

	iterateAnimation(inputObject->ObjectDisplay);

	// Assign Sprite   
	if (inputObject->ObjectDisplay->currentSprite > 0)
	{
		switchSprite(inputObject->ObjectDisplay->currentSprite, USE_CURRENT_SPRITESET, inputObject->ObjectDisplay);
	}

	return LEMON_SUCCESS;
}


int UpdatePreviousPositions(ObjectController *ObjectList)
{
	Object *currentObject = ObjectList->firstObject;

	PhysicsRect *currentBox = NULL;

	while (currentObject != NULL)
	{
		currentBox = currentObject->ObjectBox;
		currentObject = currentObject->nextObject;

		if (currentBox != NULL)
		{
			currentBox->prevXPos = currentBox->xPos;
			currentBox->prevYPos = currentBox->yPos;

			currentBox->GroundBox = NULL;
		}
	}

	return LEMON_SUCCESS;
}


int UpdatePhysicsState(Object *inputObject, World *GameWorld)
{
	if (inputObject->State == STATIC || inputObject->State < DEFAULT || inputObject->reserved != AWAITING_MAGNETISATION)
	{
		return EXECUTION_UNNECESSARY;
	}

	PhysicsRect *inputBox = inputObject->ObjectBox;

	if (inputBox == NULL || GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsRect *GroundBox = inputBox->GroundBox;

	applyMagnetisation(inputBox, GroundBox, GameWorld);


	if (!(fabs(inputBox->PhysicsXVelocity) < 0.1) )
	{
		inputBox->xPos += inputBox->PhysicsXVelocity;
	}
	if (!(fabs(inputBox->PhysicsYVelocity) < 0.1) )
	{
		inputBox->yPos += inputBox->PhysicsYVelocity;
	}

	inputObject->reserved = AWAITING_RESOLUTION;


	return LEMON_SUCCESS;
}


int applyMagnetisation(PhysicsRect *inputBox, PhysicsRect *GroundBox, World *GameWorld)
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
	double pixelXDifference = (int)GroundBox->xPos - (int)GroundBox->prevXPos;

	GroundBox->xPos += pixelXDifference; 
		
	if (fabs(GameWorld->GlobalGravityX) < 0.1 || CheckBoxCollidesBox(GroundBox, inputBox) == 0)
	{
		inputBox->PhysicsXVelocity = pixelXDifference;
	}

	GroundBox->xPos -= pixelXDifference; 

	if (fabs(inputBox->PhysicsXVelocity) < 0.1)
	{
		inputBox->PhysicsXVelocity = 0.0;
	}

	double pixelYDifference = (int)GroundBox->yPos - (int)GroundBox->prevYPos;
	GroundBox->yPos += pixelYDifference; 
		
	if (fabs(GameWorld->GlobalGravityY) < 0.1 || CheckBoxCollidesBox(GroundBox, inputBox) == 0)
	{
		inputBox->PhysicsYVelocity = pixelYDifference;
	}

	GroundBox->yPos -= pixelYDifference;

	if (fabs(inputBox->PhysicsYVelocity) < 0.1)
	{
		inputBox->PhysicsYVelocity = 0.0;
	}
	

	return LEMON_SUCCESS;
}


int UpdateParentChildLink(Object *inputObject)
{
	if (inputObject->ParentObject == NULL)
	{
		return EXECUTION_UNNECESSARY;
	}

	if ((inputObject->ParentObject->ParentLink & FINAL_LINK) != 0)
	{
		inputObject->ParentObject = NULL;
		return ACTION_DISABLED;
	}

	PhysicsRect *InputBox = inputObject->ObjectBox;
	DisplayData *InputDisplay = inputObject->ObjectDisplay;

	DisplayData *ParentDisplay = inputObject->ParentObject->ObjectDisplay;
	PhysicsRect *ParentBox = inputObject->ParentObject->ObjectBox;

	if (ParentDisplay == NULL || ParentBox == NULL)
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
		InputDisplay->transparencyEffect = ParentDisplay->transparencyEffect;
	}


	if ((inputObject->ParentLink & POSITION_LINK) != 0)
	{
		InputBox->xPos += floor(ParentBox->xPos - ParentBox->prevXPos);
		InputBox->xPosRight = InputBox->xPos + InputBox->xSize;

		InputBox->yPos += floor(ParentBox->yPos - ParentBox->prevYPos);
		InputBox->yPosTop = InputBox->yPos + InputBox->ySize;
	}

	if ((inputObject->ParentLink & VELOCITY_LINK) != 0)
	{
		InputBox->xVelocity = ParentBox->xVelocity;
		InputBox->yVelocity = ParentBox->yVelocity;
		InputBox->direction = ParentBox->direction;
		InputBox->forwardVelocity = ParentBox->forwardVelocity;
	}

	if ( (inputObject->ParentLink & DISPLAYDATA_LINK) != 0)
	{
		InputDisplay->RenderModeOverride = ParentDisplay->RenderModeOverride;
	}

	return LEMON_SUCCESS;
}


int ResolveAllObjects(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->PhysicsType != PLATFORMER)
	{
		return ACTION_DISABLED;
	}

	PhysicsRect *currentBox = NULL;
	Object *currentObject = GameWorld->ObjectList->firstObject;

	while (currentObject != NULL)
	{
		currentBox = currentObject->ObjectBox;

		if (currentBox != NULL && currentObject->reserved == AWAITING_RESOLUTION)
		{
			if (fabs(currentBox->PhysicsXVelocity) > 0.1)
			{
				double savedPos = currentBox->yPos;
				currentBox->yPos -= currentBox->PhysicsYVelocity;
				ResolveAllXCollision(currentObject->ObjectBox, GameWorld->ObjectList);
				currentBox->yPos = savedPos;
			}

			if (fabs(currentBox->PhysicsYVelocity) > 0.1)
			{	
				ResolveAllYCollision(currentObject->ObjectBox, GameWorld->ObjectList);
			}

			currentObject->reserved = AWAITING_UPDATE;
		}

		currentObject = currentObject->nextObject;
	}

	return LEMON_SUCCESS;
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
	FrameUpdateFunction *currentFunction = (FrameUpdateFunction *)ObjectList->FrameUpdates;

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


int AddFrameUpdateFunction(int (*FunctionPointer)(Object*, World*), Object *inputObject, ObjectController *ObjectList)
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

	FrameUpdateFunction *CurrentFrameUpdate = (FrameUpdateFunction *)ObjectList->FrameUpdates;

	if (CurrentFrameUpdate == NULL)
	{
		ObjectList->FrameUpdates = (void *)function;
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
	FrameUpdateFunction *DeleteFunction;

	while (currentFunction != NULL)
	{
		DeleteFunction = currentFunction;
		currentFunction = currentFunction->nextFunction;

		free(DeleteFunction);
	}

	ObjectList->FrameUpdates = NULL;

	return LEMON_SUCCESS;
}


/*
int DamagedFrames(Object *inputObject)
{
	DisplayData *inputData = inputObject->ObjectDisplay;


	inputData->invincibilityFrames--;

	if (inputData->invincibilityFrames < 1)
	{
		inputData->hidden = false;
		return ACTION_DISABLED;
	}


	if (inputData->invincibilityFrames % 12 != 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	if (inputData->hidden == false)
	{
		inputData->hidden = true;
		inputData->invincibilityFrames -= 4;
	}
	else
	{
		inputData->hidden = false;
	}
	

	return LEMON_SUCCESS;
}
*/


int UpdateCoin(Object *coin, World *GameWorld)
{
	if (coin == NULL || coin->ObjectBox == NULL || GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	PlayerData *Player = &GameWorld->Player;
	PhysicsRect *coinBox = coin->ObjectBox;

	if (checkBoxOverlapsBoxBroad(Player->PlayerBox, coin->ObjectBox) == 1)
	{
		Player->coinCount++;
		AddParticle(GameWorld, SPARKLE, coinBox->xPos + 20 - (rand() % 40), coinBox->yPos + 20 - (rand() % 40), 1, 0);
		MarkObjectForDeletion(coin);
		Lemon_PlaySound("Coin_Collect", "Objects", OBJECT_SFX, 0.75);
	}

/*
	if (keyboard[LMN_INTERACT2] || coin->arg1 > 0)
	{
		if (DistanceBetween(coin, Player->PlayerPtr) < 25000.0)
		{
			coin->arg1 = 1;
		}

		PointObjectTowards(coin, Player->PlayerPtr, ROTATE_BOX);
		
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

	PhysicsRect *PlayerBox = GameWorld->Player.PlayerBox;


	if (spring->ObjectDisplay->currentAnimation == 0 && PlayerBox->yVelocity < -1.0 && checkBoxOverlapsBoxBroad(PlayerBox, spring->ObjectBox) == 1)
	{
		double xForce = -cos(spring->ObjectBox->direction) * spring->arg1;
		double yForce = sin(spring->ObjectBox->direction) * spring->arg1;

		if (fabs(yForce) > 0.9)
		{
			PlayerBox->yVelocity = yForce;
		}

		if (fabs(xForce) > 0.9)
		{
			PlayerBox->xVelocity = xForce;
		}
		
		Lemon_PlaySound("Spring", "Objects", OBJECT_SFX, 1.0);
		PlayAnimation("Bounce", 1, spring->ObjectDisplay);
	}

	return LEMON_SUCCESS;
}


int InitialiseParticle(Object *particle, int animation, int repeatCount, int particleLifeTime)
{
	if (particle == NULL)
	{
		return MISSING_DATA;
	}

	particle->layer = PARTICLES;
	particle->ObjectBox->solid = UNSOLID;
	particle->ObjectDisplay->currentAnimation = animation;
	particle->arg1 = particleLifeTime;
	particle->ParentLink = FINAL_LINK;

	PlayAnimationByIndex(animation, repeatCount, particle->ObjectDisplay);


	switch (animation)
	{
	case SPARKLE:
		SetObjectDirection(particle, (double)(rand() % 180), ROTATE_SPRITE);
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
	if ((particle->arg1 > 0 && particle->arg2 > particle->arg1) || (particle->arg1 < 1 && particle->ObjectDisplay->currentAnimation == 0))
	{
		MarkObjectForDeletion(particle);
	}

	return LEMON_SUCCESS;
}


int CustomParticleBehaviour(World *GameWorld, Object *particle)
{
	// Custom behaviour
	switch(particle->ObjectDisplay->currentAnimation)
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
	// arg3 = switch off/on (0/1) state
	// arg4 = counter
	// arg5 = timer


	if (PlayerInteractingWithBox(player, gateSwitch->ObjectBox) == 1)
	{
		gateSwitch->arg3 = (gateSwitch->arg3 + 1) % 2;
		gateSwitch->arg4 = gateSwitch->arg5;

		// Update any gates
		gateControl(gateSwitch, ObjectList);

		if (gateSwitch->arg3 == 0)
		{
			PlayAnimation("OpenSwitch", 1, gateSwitch->ObjectDisplay);
		}
		else
		{
			PlayAnimation("CloseSwitch", 1, gateSwitch->ObjectDisplay);
		}

		return LEMON_SUCCESS;
	}


	if (gateSwitch->arg5 > 0 && gateSwitch->arg3 == 1)
	{
		gateSwitch->arg4--;

		if (gateSwitch->arg4 < 1)
		{
			gateSwitch->arg3 = 0;

			// Update any gates
			gateControl(gateSwitch, ObjectList);

			PlayAnimation("CloseSwitch", 1, gateSwitch->ObjectDisplay);
		}
	}

	return LEMON_SUCCESS;
}


int UpdateVerticalGate(Object *gate, World *GameWorld)
{
	// arg1 = gate ID
	// arg2 = gate open/close (0/1)
	// arg3 = closed gate y position 
	// arg4 = speed
	
	if (gate == NULL || GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	int closedPosition = gate->arg3;
	int speed = gate->arg4;


	// Animation control
	switch (gate->ObjectDisplay->currentAnimation)
	{
		case 0:
		{
			SetYPosition(gate, closedPosition);
			gate->ObjectBox->yVelocity = 0.0;

			if (gate->arg2 == 1)
			{
				gate->ObjectDisplay->currentAnimation = 1;
				Lemon_PlaySound("GateOpen", "Objects", 4, 1.0);
			}
		
		} break;

		case 1:
		{
			gate->ObjectBox->yVelocity += 0.1 * (speed/abs(speed));

			if (fabs(gate->ObjectBox->yVelocity) > abs(speed))
			{
				gate->ObjectBox->yVelocity = speed;
			}

			if ( (speed < 0 && gate->ObjectBox->yPos < closedPosition - gate->ObjectBox->ySize) || (speed > 0 && gate->ObjectBox->yPos > closedPosition + gate->ObjectBox->ySize) )
			{
				gate->ObjectDisplay->currentAnimation = 2;
				gate->ObjectBox->yVelocity = 0.0;
				SetYPosition(gate, closedPosition + (gate->ObjectBox->ySize * (speed/abs(speed))));
			}

		} break;


		case 2:
		{
			SetYPosition(gate, closedPosition + (gate->ObjectBox->ySize * (speed/abs(speed))));
			gate->ObjectBox->yVelocity = 0.0;

			if (gate->arg2 == 0)
			{
				gate->ObjectDisplay->currentAnimation = 3;
				Lemon_PlaySound("GateClose", "Objects", 4, 1.0);
			}

		} break;


		case 3:
		{
			gate->ObjectBox->yVelocity -= 0.1 * (speed/abs(speed));

			if (fabs(gate->ObjectBox->yVelocity) > abs(speed))
			{
				gate->ObjectBox->yVelocity = -speed;
			}

			if ( (speed > 0 && gate->ObjectBox->yPos < closedPosition) || (speed < 0 && gate->ObjectBox->yPos > closedPosition) )
			{
				gate->ObjectDisplay->currentAnimation = 0;
				gate->ObjectBox->yVelocity = 0.0;
				SetYPosition(gate, closedPosition);
			}

		} break;
	}


	return LEMON_SUCCESS;
}


int UpdateHorizontalGate(Object *gate, World *GameWorld)
{
	// arg1 = gate ID
	// arg2 = gate open/close (0/1)
	// arg3 = closed gate y position 
	// arg4 = speed
	
	if (gate == NULL || GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	int closedPosition = gate->arg3;
	int speed = gate->arg4;


	// Animation control
	switch (gate->ObjectDisplay->currentAnimation)
	{
		case 0:
		{
			SetXPosition(gate, closedPosition);
			gate->ObjectBox->xVelocity = 0.0;

			if (gate->arg2 == 1)
			{
				gate->ObjectDisplay->currentAnimation = 1;
				Lemon_PlaySound("GateOpen", "Objects", 4, 1.0);
			}
		
		} break;

		case 1:
		{
			gate->ObjectBox->xVelocity += 0.1 * (speed/abs(speed));

			if (fabs(gate->ObjectBox->xVelocity) > abs(speed))
			{
				gate->ObjectBox->xVelocity = speed;
			}

			if ( (speed < 0 && gate->ObjectBox->xPos < closedPosition - gate->ObjectBox->xSize) || (speed > 0 && gate->ObjectBox->xPos > closedPosition + gate->ObjectBox->xSize) )
			{
				gate->ObjectDisplay->currentAnimation = 2;
				gate->ObjectBox->xVelocity = 0.0;
				SetXPosition(gate, closedPosition + (gate->ObjectBox->xSize * (speed/abs(speed))));
			}

		} break;


		case 2:
		{
			SetXPosition(gate, closedPosition + (gate->ObjectBox->xSize * (speed/abs(speed))));
			gate->ObjectBox->xVelocity = 0.0;

			if (gate->arg2 == 0)
			{
				gate->ObjectDisplay->currentAnimation = 3;
				Lemon_PlaySound("GateClose", "Objects", 4, 1.0);
			}

		} break;


		case 3:
		{
			gate->ObjectBox->xVelocity -= 0.1 * (speed/abs(speed));

			if (fabs(gate->ObjectBox->xVelocity) > abs(speed))
			{
				gate->ObjectBox->xVelocity = -speed;
			}

			if ( (speed > 0 && gate->ObjectBox->xPos < closedPosition) || (speed < 0 && gate->ObjectBox->xPos > closedPosition) )
			{
				gate->ObjectDisplay->currentAnimation = 0;
				gate->ObjectBox->xVelocity = 0.0;
				SetXPosition(gate, closedPosition);
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

	Object *currentObject;
	currentObject = ObjectList->firstObject;

	int onOrOff = -1;

	// Run through object list
	while (currentObject != NULL)
	{	
		// If a matching switch is found, evaluate whether connected gate should be open/closed, do this for every found switch
		if ((currentObject->ObjectID == GATE_SWITCH || currentObject->ObjectID == GATE_SWITCH_TIMED) && gateSwitch->arg1 == currentObject->arg1)
		{
			if (onOrOff == -1)
			{
				onOrOff = currentObject->arg3;
			}
			else
			{
				if (currentObject->arg2 == SINGLE_SWITCH)
				{
					if (currentObject->arg3 == 1)
					{
						onOrOff = 1;
						currentObject = ObjectList->lastObject;
					}	
				} 
				else if (currentObject->arg2 == CHAIN_SWITCH)
				{		
					onOrOff = onOrOff && currentObject->arg3;
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
			currentObject->arg2 = onOrOff;
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
	inputObject->layer = MIDDLEGROUND;
	inputObject->ObjectBox->ySize = Y_TILESCALE;
	inputObject->ObjectBox->xSize = X_TILESCALE * 3;
	inputObject->arg1 = bound1;
	inputObject->arg2 = bound2;

	if (abs(inputObject->arg3) > 16)
	{
		inputObject->arg3 = 16;
	}
	else
	{
		inputObject->arg3 = abs(speed);
	}
	
	inputObject->arg4 = 1;
	inputObject->arg5 = abs(timer);
	inputObject->ObjectBox->solid = JUMP_THROUGH;

	switch (objectID)
	{

		default:
		break;
	}

	return inputObject;
}


int UpdateHorizontalPlatform(Object *platform)
{
	int XPos = platform->ObjectBox->xPos;
	int XPos2 = platform->ObjectBox->xPos + platform->ObjectBox->xSize;

	int leftBound = platform->arg1;
	int rightBound = platform->arg2;
	int maxSpeed = platform->arg3;
	int timer = platform->arg5;

	// Wait to change direction
	if (platform->arg4 > 0 && fabs(platform->ObjectBox->xVelocity) < 0.5)
	{
		platform->arg4++;
		if (platform->arg4 > timer)
		{
			platform->arg4 = -1;
		}
	}

	if (platform->arg4 < 0 && fabs(platform->ObjectBox->xVelocity) < 0.5)
	{
		platform->arg4--;
		if (platform->arg4 < -timer)
		{
			platform->arg4 = 1;
		}
	}

	// Accelerate
	if (platform->arg4 > 0 && platform->ObjectBox->xVelocity < maxSpeed && XPos2 <= rightBound)
	{
		platform->ObjectBox->xVelocity += 0.5;
	}

	if (platform->arg4 < 0 && platform->ObjectBox->xVelocity > -maxSpeed && XPos >= leftBound)
	{
		platform->ObjectBox->xVelocity -= 0.5;
	}

	// Deccelerate
	if ((XPos2 >= rightBound && platform->arg4 > 0) || (XPos <= leftBound && platform->arg4 < 0))
	{
		platform->ObjectBox->xVelocity *= 0.9;
	}
	
	if (fabs(platform->ObjectBox->xVelocity) < 0.1)
	{
		platform->ObjectBox->xVelocity = 0.0;
	}


	return LEMON_SUCCESS;
}



int UpdateVerticalPlatform(Object *platform)
{
	int YPos = platform->ObjectBox->yPos;
	int YPos2 = platform->ObjectBox->yPos + platform->ObjectBox->ySize;

	int bottomBound = platform->arg1;
	int topBound = platform->arg2;
	int maxSpeed = platform->arg3;
	int timer = platform->arg5;

	// Wait to change direction
	if (platform->arg4 > 0 && fabs(platform->ObjectBox->yVelocity) < 0.5)
	{
		platform->arg4++;
		if (platform->arg4 > timer)
		{
			platform->arg4 = -1;
		}
	}

	if (platform->arg4 < 0 && fabs(platform->ObjectBox->yVelocity) < 0.5)
	{
		platform->arg4--;
		if (platform->arg4 < -timer)
		{
			platform->arg4 = 1;
		}
	}

	// Accelerate
	if (platform->arg4 > 0 && platform->ObjectBox->yVelocity < maxSpeed && YPos2 <= topBound)
	{
		platform->ObjectBox->yVelocity += 0.75;
	}

	if (platform->arg4 < 0 && platform->ObjectBox->yVelocity > -maxSpeed && YPos >= bottomBound)
	{
		platform->ObjectBox->yVelocity -= 0.75;
	}

	// Deccelerate
	if ((YPos2 >= topBound && platform->arg4 > 0) || (YPos <= bottomBound && platform->arg4 < 0))
	{
		platform->ObjectBox->yVelocity *= 0.9;
	}

	if (fabs(platform->ObjectBox->yVelocity) < 0.1)
	{
		platform->ObjectBox->yVelocity = 0.0;
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

	if (PlayerInteractingWithBox(Player, Door->ObjectBox) == 1 && Player.PlayerPtr->State == DEFAULT)
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
				"Yes", MOVE_PLAYER_TO_EXIT_DOOR, Door, 
				"No", NO_ACTION,
				"hm... lemme think aout it", START_CUTSCENE, TEST_SCENE_2);
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

	Lemon_PlaySound("DoorOpen", "Objects", OBJECT_SFX, 1.0);
			
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

	if (Door->arg2 < 1 && PlayerInteractingWithBox(Player, Door->ObjectBox) == 1 && Player.PlayerPtr->State == DEFAULT)
	{
		Door->arg2 = 1;
		SayText("It's a door.\f.\f.\r \nIt eminates a strange glow.", NO_PORTRAIT, BASIC_FADE, GameWorld);
		char phrase[] = "This Door will send you to level 0!";
		phrase[33] += Door->arg1;
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

	PhysicsRect *inputBox = inputObject->ObjectBox;

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


int ApplyFriction(PhysicsRect *inputBox, float forwardFriction, float xFriction, float yFriction)
{
	if (inputBox == NULL)
	{
		return MISSING_DATA;
	}

	if (forwardFriction > 0.0)
	{
		inputBox->forwardVelocity = inputBox->forwardVelocity * forwardFriction;
	}

	if (inputBox->forwardVelocity > MAX_FORWARD_VELOCITY)
	{
		inputBox->forwardVelocity = MAX_FORWARD_VELOCITY;
	}
	else if (inputBox->forwardVelocity < -MAX_FORWARD_VELOCITY)
	{
		inputBox->forwardVelocity = -MAX_FORWARD_VELOCITY;
	}
	

	if (xFriction > 0.0)
	{
		inputBox->xVelocity = inputBox->xVelocity * xFriction;
	}

	if (inputBox->xVelocity > MAX_X_VELOCITY) 
	{
		inputBox->xVelocity = MAX_X_VELOCITY;
	}
	else if (inputBox->xVelocity < -MAX_X_VELOCITY) 
	{
		inputBox->xVelocity = -MAX_X_VELOCITY;
	}


	if (yFriction > 0.0)
	{
		inputBox->yVelocity = inputBox->yVelocity * yFriction;
	}

	if (inputBox->yVelocity > MAX_Y_VELOCITY) 
	{
		inputBox->yVelocity = MAX_Y_VELOCITY;
	}
	else if (inputBox->yVelocity < -MAX_Y_VELOCITY) 
	{
		inputBox->yVelocity = -MAX_Y_VELOCITY;
	}
		

	return LEMON_SUCCESS;
}


Object* CheckForGround(PhysicsRect *movingBox, World *GameWorld)
{
	if (GameWorld == NULL || movingBox == NULL || movingBox->solid == UNSOLID)
	{
		return NULL;
	}

	float savedX = movingBox->xPos;
	float savedY = movingBox->yPos;

	movingBox->yPos += GameWorld->GlobalGravityY * 3.0;
	movingBox->xPos += GameWorld->GlobalGravityX * 3.0;

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
int resetGroundCheck(PhysicsRect *inputBox, World *GameWorld)
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

	PhysicsRect *inputBox = inputObject->ObjectBox;

	inputBox->xPos = destX;
	inputBox->yPos = destY;
	inputBox->prevXPos = destX;
	inputBox->prevYPos = destY;
	inputBox->xPosRight = destX + inputBox->xSize;
	inputBox->yPosTop = destY + inputBox->ySize;

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

	PhysicsRect *inputBox = inputObject->ObjectBox;

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

	PhysicsRect *inputBox = inputObject->ObjectBox;

	if (fabs(inputBox->yPos - newYPos) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}


	GoTo(inputObject, inputBox->xPos, newYPos);

	return LEMON_SUCCESS;
}


int PointObjectTowards(Object *inputObject, Object *pointDestination, RotateMode RotateSetting)
{
	if (pointDestination == NULL || pointDestination->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	float destX = pointDestination->ObjectBox->xPos + (pointDestination->ObjectBox->xSize >> 1);
	float destY = pointDestination->ObjectBox->yPos + (pointDestination->ObjectBox->ySize >> 1);

	PointObjectToXY(inputObject, destX, destY, RotateSetting);

	return LEMON_SUCCESS;
}


int PointObjectToMouse(Object *inputObject, World *GameWorld, RotateMode RotateSetting)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	float MouseX = MouseInput.xPos + GameWorld->MainCamera.CameraX;
	float MouseY = MouseInput.yPos + GameWorld->MainCamera.CameraY;

	PointObjectToXY(inputObject, MouseX, MouseY, RotateSetting);

	return LEMON_SUCCESS;
}


int PointObjectToXY(Object *inputObject, float xPos, float yPos, RotateMode RotateSetting)
{
	if (inputObject == NULL || inputObject->ObjectDisplay == NULL || inputObject->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	float originX = inputObject->ObjectBox->xPos + (inputObject->ObjectBox->xSize >> 1);
	float originY = inputObject->ObjectBox->yPos + (inputObject->ObjectBox->ySize >> 1);

	double newDirection = atan2(xPos - originX, yPos - originY);


	if (RotateSetting == ROTATE_SPRITE)
	{
		inputObject->ObjectDisplay->direction = newDirection;
	}
	else if (RotateSetting == ROTATE_BOX)
	{
		inputObject->ObjectBox->direction = newDirection;
	}
	else
	{
		inputObject->ObjectBox->direction = newDirection;
		inputObject->ObjectDisplay->direction = newDirection;
	}

	return LEMON_SUCCESS;
}


int RotateObject(Object *inputObject, double rotationDegrees, RotateMode RotateSetting)
{
	if (inputObject == NULL || inputObject->ObjectDisplay == NULL || inputObject->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	if (rotationDegrees < -359.9 || rotationDegrees > 359.9)
	{
		return ACTION_DISABLED;
	}

	rotationDegrees = rotationDegrees * DEGREE_TO_RADIAN_PI * inputObject->ObjectBox->xFlip;

	double newDirection;

	if (RotateSetting == ROTATE_SPRITE)
	{
		newDirection = inputObject->ObjectDisplay->direction + rotationDegrees;
	}
	else
	{
		newDirection = inputObject->ObjectBox->direction + rotationDegrees;
	}


	if (newDirection >= RADIAN_360)
	{
		newDirection -= RADIAN_360;
	}
	else if (newDirection < 0.0)
	{
		newDirection += RADIAN_360;
	}


	if (RotateSetting == ROTATE_SPRITE)
	{
		inputObject->ObjectDisplay->direction = newDirection;
	}
	else if (RotateSetting == ROTATE_BOX)
	{
		inputObject->ObjectBox->direction = newDirection;
	}
	else
	{
		inputObject->ObjectBox->direction = newDirection;
		inputObject->ObjectDisplay->direction = newDirection;
	}

	return LEMON_SUCCESS;
}


int SetObjectDirection(Object *inputObject, double rotationDegrees, RotateMode RotateSetting)
{
	if (inputObject == NULL || inputObject->ObjectDisplay == NULL || inputObject->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	if (rotationDegrees < -360 || rotationDegrees > 360)
	{
		return ACTION_DISABLED;
	}

	if (rotationDegrees < 0.0)
	{
		rotationDegrees += 360;
	}

	rotationDegrees = rotationDegrees * DEGREE_TO_RADIAN_PI;


	if (RotateSetting == ROTATE_SPRITE)
	{
		inputObject->ObjectDisplay->direction = rotationDegrees;
	}
	else if (RotateSetting == ROTATE_BOX)
	{
		inputObject->ObjectBox->direction = rotationDegrees;
	}
	else
	{
		inputObject->ObjectBox->direction = rotationDegrees;
		inputObject->ObjectDisplay->direction = rotationDegrees;
	}


	if (fabs(RADIAN_90 - inputObject->ObjectDisplay->direction) < 0.01)
	{
		inputObject->ObjectDisplay->direction = RADIAN_90;
	}

	if (fabs(RADIAN_90 - inputObject->ObjectBox->direction) < 0.01)
	{
		inputObject->ObjectBox->direction = RADIAN_90;
	}

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
	inputObject->ObjectBox->xPosRight += changeHalf;

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
	inputObject->ObjectBox->yPosTop += changeHalf;

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

	if (input == NULL || input->ObjectBox == NULL || input->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	input->ObjectDisplay->size = sizePercentage;

	return LEMON_SUCCESS;
}


int changeScaleSize(Object *input, float sizePercentage)
{
	if (input == NULL || input->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	return setScaleSize(input, sizePercentage + input->ObjectDisplay->size);
}


int smoothSizeChangeTo(Object *input, float desiredSize, float rate)
{
	if (input == NULL || input->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	if (rate < 0.1 || desiredSize < 0.0001)
	{
		return INVALID_DATA;
	}

	return changeScaleSize(input, (desiredSize - input->ObjectDisplay->size) / rate);
}


int matchBoxToDisplayDimensions(Object *input)
{
	if (input == NULL || input->ObjectDisplay == NULL || input->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	Sprite *sprite = input->ObjectDisplay->spriteBuffer;

	if (sprite == NULL)
	{
		return MISSING_DATA;
	}

	input->ObjectBox->xSize = sprite->width;
	input->ObjectBox->ySize = sprite->height;

	return LEMON_SUCCESS;
}


float DistanceBetween(Object *Source, Object *Target)
{
	if (Source == NULL || Source->ObjectBox == NULL || Target == NULL || Target->ObjectBox == NULL)
	{
		return -1.0;
	}

	PhysicsRect *box1 = (Source->ObjectBox);
	PhysicsRect *box2 = (Target->ObjectBox);

	double xDistance = (box2->xPos - box1->xPos) * (box2->xPos - box1->xPos);
	double yDistance = (box2->yPos - box1->yPos) * (box2->yPos - box1->yPos);

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
	PhysicsRect *objBox = inputObject->ObjectBox;

	if (inputObject->layer != HUD)
	{
		camX +=	GameWorld->MainCamera.CameraX;
		camY += GameWorld->MainCamera.CameraY;
	}

	if ((int)objBox->xPos > camX + screenWidth || (int)objBox->xPosRight < camX || (int)objBox->yPos > camY + screenHeight || (int)objBox->yPosTop < camY)
	{
		return false;
	}

	return true;
}


int MouseOverlappingBox(Object *input, World *GameWorld)
{
	if (input == NULL || input->ObjectBox == NULL)
	{
		return 0;
	}

	int result = 0;

	PhysicsRect inputBox = *(input->ObjectBox);
	PhysicsRect mouseBox = {0};

	mouseBox.xSize = 1;
	mouseBox.ySize = 1;
	mouseBox.xPos = (double)MouseInput.xPos;
	mouseBox.yPos = (double)MouseInput.yPos;


	if (input->layer != HUD)
	{
		inputBox.xPos -= GameWorld->MainCamera.CameraX;
		inputBox.yPos -= GameWorld->MainCamera.CameraY;
	}
	
	result = checkBoxOverlapsBoxBroad(&inputBox, &mouseBox);

	return result;
}


int MouseOverlappingSprite(Object *input, World *GameWorld)
{
	if (input == NULL || input->ObjectDisplay == NULL || input->ObjectBox == NULL || input->ObjectDisplay->spriteBuffer == NULL)
	{
		return 0;
	}

	int result = 0;

	PhysicsRect inputBox = *(input->ObjectBox);
	DisplayData *inputDisplay = input->ObjectDisplay;
	int renderMode = inputDisplay->RenderModeOverride == DEFAULT_TO_SPRITE ? inputDisplay->spriteBuffer->RenderMode : inputDisplay->RenderModeOverride;


	switch (renderMode)
	{
	case SINGLE:
	case SINGLE_FAST:
	case SINGLE_FULL_ALPHA:
		inputBox.xPos = inputBox.xPos + (inputBox.xSize >> 1) - (inputDisplay->spriteBuffer->width >> 1);
		inputBox.xSize = inputDisplay->spriteBuffer->width;
		inputBox.yPos = inputBox.yPos + (inputBox.ySize >> 1) - (inputDisplay->spriteBuffer->height >> 1);
		inputBox.ySize = inputDisplay->spriteBuffer->height;
		break;


	default:
		break;
	}

	inputBox.xPos += inputDisplay->spriteXOffset;
	inputBox.yPos += inputDisplay->spriteYOffset;


	PhysicsRect mouseBox = {0};
	mouseBox.xSize = 1;
	mouseBox.ySize = 1;
	mouseBox.xPos = (double)MouseInput.xPos;
	mouseBox.yPos = (double)MouseInput.yPos;


	if (input->layer != HUD)
	{
		if (GameWorld == NULL)
		{
			return 0;
		}

		inputBox.xPos -= GameWorld->MainCamera.CameraX;
		inputBox.yPos -= GameWorld->MainCamera.CameraY;
	}

	result = checkBoxOverlapsBoxBroad(&inputBox, &mouseBox);

	return result;
}


int checkBoxOverlapsBoxBroad(PhysicsRect *inputBox, PhysicsRect *compareBox)
{
	if (inputBox == compareBox)
	{
		return 0;
	}

	return !((int)inputBox->xPos >= (int)(compareBox->xPos + compareBox->xSize) || (int)(inputBox->xPos + inputBox->xSize) <= (int)compareBox->xPos || (int)inputBox->yPos >= (int)(compareBox->yPos + compareBox->ySize) || (int)(inputBox->yPos + inputBox->ySize) <= (int)compareBox->yPos);
}


int CheckBoxOverlapsBox(PhysicsRect *inputBox, PhysicsRect *compareBox)
{
	if (inputBox == NULL || compareBox == NULL)
	{
		return MISSING_DATA;
	}

	if (inputBox == compareBox || inputBox->xSize <= 0 || inputBox->ySize <= 0 || compareBox->xSize <= 0 || compareBox->ySize <= 0)
	{
		return 0;
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
				inputYTop = ((compareBox->xPos + compareBox->xSize - inputBox->xPos) * ((double)inputBox->ySize/(double)inputBox->xSize));
			}
			else
			{
				inputYTop = ((inputBox->xSize - compareBox->xPos + inputBox->xPos) * ((double)inputBox->ySize/(double)inputBox->xSize));
			}
			
			inputYTop = dClamp(inputYTop, 0.0, (double)inputBox->ySize);

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
				compareYTop = ((inputBox->xPos + inputBox->xSize - compareBox->xPos) * ((double)compareBox->ySize/(double)compareBox->xSize));
			}
			else
			{
				compareYTop = ((compareBox->xSize - (inputBox->xPos - compareBox->xPos)) * ((double)compareBox->ySize/(double)compareBox->xSize));
			}

			compareYTop = dClamp(compareYTop, 0.0, (double)compareBox->ySize);

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


int CheckBoxCollidesBox(PhysicsRect *inputBox, PhysicsRect *compareBox)
{
	if (inputBox == NULL || compareBox == NULL)
	{
		return MISSING_DATA;
	}

	if (compareBox->collideLayer != inputBox->collideLayer)
	{
		return 0;
	}

	if (CheckBoxOverlapsBox(inputBox, compareBox) == 0)
	{
		return 0;
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


	switch(compareBox->solid)
	{
		case JUMP_THROUGH:
		{
			if (inputBox->yVelocity > 0.0 || inputBox->crouch == true || inputBox->prevYPos < (compareBox->prevYPos + compareBox->ySize - 1) )
			{
				return 0;
			}
		} break;

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

		case UNSOLID:
		return 0;
			
		default:
		break;
	}


	return 1;
}


int AssignDirection(PhysicsRect *inputBox, PhysicsRect *compareBox)
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
				inputBox->direction = RADIAN_90;
				break;
			}

			// Check against flat side
			int difference = (int)(inputBox->prevYPos - compareBox->yPos);

			if ( (compareBox->yFlip == 1 && difference < -inputBox->ySize + 1) || (compareBox->yFlip == -1 && difference > compareBox->ySize - 1))
			{
				inputBox->direction = RADIAN_90;
				break;
			}

			double slope = (double)compareBox->ySize/(double)compareBox->xSize;
			double slopeFloor;

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
				inputBox->direction = RADIAN_90;
				break;
			}
			
			
			inputBox->direction = (RADIAN_90 - ((atan(slope))) * compareBox->xFlip);
			
		} break;


		default:
			inputBox->direction = RADIAN_90;
			break;
	}

	return LEMON_SUCCESS;
}


int deleteQuadTree(struct QuadTree *inputTree)
{
	if (inputTree == NULL)
	{
		return MISSING_DATA;
	}

	deleteQuadTree(inputTree->Children[0]);
	deleteQuadTree(inputTree->Children[1]);
	deleteQuadTree(inputTree->Children[2]);
	deleteQuadTree(inputTree->Children[3]);

	free(inputTree);


	return LEMON_SUCCESS;
}


// UNIMPLEMENTED - Should be added later to potentially optimise collision detection 
int constructQuadTree(ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	struct QuadTree *rootTree = ObjectList->QuadTreeRoot;

	if (rootTree == NULL)
	{
		ObjectList->QuadTreeRoot = malloc(sizeof(struct QuadTree));
		if (ObjectList->QuadTreeRoot == NULL)
		{
			return LEMON_ERROR;
		}

		rootTree->BoundingBox.xPos = -EngineSettings.WorldBoundX;
		rootTree->BoundingBox.yPos = -EngineSettings.WorldBoundY;
		rootTree->BoundingBox.Width = (int)EngineSettings.WorldBoundX;
		rootTree->BoundingBox.Height = (int)EngineSettings.WorldBoundY;

		rootTree->ObjectList[0] = NULL;
		rootTree->Children[0] = NULL;
	}


	Object *currentObject = ObjectList->firstObject;

	while (currentObject != NULL)
	{
		// Create quadTree
		insertObjectIntoQuadTree(currentObject, rootTree);

		currentObject = currentObject->nextObject;
	}

	return LEMON_SUCCESS;
}


int insertObjectIntoQuadTree(Object *inputObject, struct QuadTree *inputTree)
{
	if (inputObject == NULL || inputTree == NULL)
	{
		return MISSING_DATA;
	}





	return LEMON_SUCCESS;
}



// returns pointer of object overlapping, NULL if no object is detected
Object* GetCollidingObject(PhysicsRect *inputBox, ObjectController *ObjectList)
{
	if (inputBox == NULL || inputBox->solid == UNSOLID || ObjectList == NULL)
	{
		return NULL;
	}

	Object *currentObject;
	currentObject = ObjectList->firstObject;

	if (currentObject == NULL)
	{
		return NULL;
	}

	while (currentObject != NULL)
	{
		if (currentObject->ObjectBox->solid == UNSOLID || checkBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox) == 0)
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


Object* GetOverlappingObject(Object *inputObject, ObjectController *ObjectList)
{
	if (inputObject == NULL || ObjectList == NULL)
	{
		return NULL;
	}

	PhysicsRect *inputBox = inputObject->ObjectBox;
	Object *currentObject = ObjectList->firstObject;

	int i = ObjectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		if (checkBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox) == 0)
		{
			currentObject = currentObject->nextObject;
			continue;
		}
		
		if (CheckBoxOverlapsBox(inputBox, currentObject->ObjectBox) == 1)
		{
			return currentObject;
		}

		currentObject = currentObject->nextObject;

		i++;
	}

	return NULL;
}


Object* GetOverlappingObjectType(Object *inputObject, int overlapObjectID, ObjectController *ObjectList)
{
	if (inputObject == NULL || ObjectList == NULL)
	{
		return NULL;
	}

	PhysicsRect *inputBox = inputObject->ObjectBox;
	Object *currentObject = ObjectList->firstObject;

	int i = ObjectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		if (checkBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox) == 0)
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		
		if (currentObject->ObjectID == overlapObjectID && CheckBoxOverlapsBox(inputBox, currentObject->ObjectBox) == 1)
		{
			return currentObject;
		}


		currentObject = currentObject->nextObject;

		i++;
	}

	return NULL;
}


Object* GetOverlappingObjectSolid(Object *inputObject, int solidID, ObjectController *ObjectList)
{
	if (ObjectList == NULL || inputObject == NULL)
	{
		return NULL;
	}

	PhysicsRect *inputBox = inputObject->ObjectBox;
	Object *currentObject = ObjectList->firstObject;

	int i = ObjectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		if (checkBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox) == 0)
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		
		if (currentObject->ObjectBox->solid == solidID && CheckBoxOverlapsBox(inputBox, currentObject->ObjectBox) == 1)
		{
			return currentObject;
		}
		

		currentObject = currentObject->nextObject;

		i++;
	}

	return NULL;
}


Object* GetOverlappingObjectAllSolids(Object *inputObject, ObjectController *ObjectList)
{
	if (ObjectList == NULL || inputObject == NULL)
	{
		return NULL;
	}

	PhysicsRect *inputBox = inputObject->ObjectBox;
	Object *currentObject = ObjectList->firstObject;

	int i = ObjectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		if (checkBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox) == 0)
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		
		if (currentObject->ObjectBox->solid != UNSOLID && CheckBoxOverlapsBox(inputBox, currentObject->ObjectBox) == 1)
		{
			return currentObject;
		}
		

		currentObject = currentObject->nextObject;

		i++;
	}

	return NULL;
}


int MoveObject(Object *inputObject, World *GameWorld)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL || GameWorld == NULL)	
	{ 
		return MISSING_DATA; 
	}


	ObjectController *ObjectList = GameWorld->ObjectList;
	ObjectList->depthCounter = 0;
	PhysicsRect *inputBox = inputObject->ObjectBox;

	moveObjectX(inputBox, ObjectList);
	moveObjectY(inputBox, ObjectList);
	moveObjectForward(inputBox, ObjectList);

	inputBox->xPos = dClamp(inputBox->xPos, -EngineSettings.WorldBoundX, EngineSettings.WorldBoundX);
	inputBox->xPosRight = inputBox->xPos + inputBox->xSize;
	inputBox->yPos = dClamp(inputBox->yPos, -EngineSettings.WorldBoundY, EngineSettings.WorldBoundY);
	inputBox->yPosTop = inputBox->yPos + inputBox->ySize;

	resetGroundCheck(inputBox, GameWorld);

	return LEMON_SUCCESS;
}


int moveObjectX(PhysicsRect *inputBox, ObjectController *ObjectList)
{
	double velocity = inputBox->xVelocity;// + inputBox->PhysicsXVelocity;

	if (fabs(velocity) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}

	inputBox->xPos += velocity;

	ResolveAllXCollision(inputBox, ObjectList);

	inputBox->xPosRight = inputBox->xPos + inputBox->xSize;


	return LEMON_SUCCESS;
}



int moveObjectY(PhysicsRect *inputBox, ObjectController *ObjectList)
{
	double velocity = inputBox->yVelocity;// + inputBox->PhysicsYVelocity;


	if (fabs(velocity) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}

	inputBox->yPos += velocity;

	ResolveAllYCollision(inputBox, ObjectList);

	inputBox->yPosTop = inputBox->yPos + inputBox->ySize;


	return LEMON_SUCCESS;
}


CollideType evaluateCollideMode(PhysicsRect *movingBox, PhysicsRect *collideBox)
{
	if (movingBox == NULL || collideBox == NULL)
	{
		return NO_COLLIDE_TYPE;
	}

	if (movingBox->collideMode == IMPACT)
	{
		return IMPACT;
	}


	int canPush = !(movingBox->solid == PUSHABLE_SOLID) || (collideBox->solid != ENTITY);
	int collidePushable = collideBox->solid == PUSHABLE_SOLID || collideBox->solid == ENTITY;
	
	
	if (movingBox->collideMode == PUSH || (canPush && collidePushable) )
	{
		return PUSH;
	}

	return IMPACT;
}


int AdjustDirection(PhysicsRect *movingBox, World *GameWorld)
{	
	if (GameWorld == NULL || GameWorld->ObjectList == NULL || movingBox == NULL)
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

	double sinVal = 4 * sin(movingBox->direction);
	double cosVal = 4 * cos(movingBox->direction);


	movingBox->yPos -= sinVal;
	movingBox->xPos += cosVal;

	detectedObject = GetCollidingObject(movingBox, GameWorld->ObjectList);

	movingBox->yPos += sinVal;
	movingBox->xPos -= cosVal;


	if (detectedObject == NULL)
	{
		movingBox->direction = RADIAN_90;

		return LEMON_SUCCESS;
	}

	AssignDirection(movingBox, detectedObject->ObjectBox);


	return LEMON_SUCCESS;
}


int moveObjectForward(PhysicsRect *movingBox, ObjectController *ObjectList)
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
	if (movingBox->solid == UNSOLID)
	{
		movingBox->xPos += movingBox->forwardVelocity * sin(movingBox->direction);
		movingBox->yPos += movingBox->forwardVelocity * cos(movingBox->direction);
		
		return LEMON_SUCCESS;
	}


	// step and count set-up
	double orientation = (movingBox->forwardVelocity > 0.0) ? 1.0 : -1.0;
	double sinVal = sin(movingBox->direction);
	double cosVal = cos(movingBox->direction);

	if (fabs(sinVal) < 0.1)
	{
		sinVal = 0.0;
	}

	if (fabs(cosVal) < 0.1)
	{
		cosVal = 0.0;
	}

	//double xDest = movingBox->xPos + (sinVal * movingBox->forwardVelocity);
	//double yDest = movingBox->yPos + (cosVal * movingBox->forwardVelocity);

	//double xTravel = floor(xDest) - floor(movingBox->xPos);
	//double yTravel = floor(yDest) - floor(movingBox->yPos);
	//double travelDistance = sqrt(pow(xTravel, 2.0) + pow(yTravel, 2.0));
	

	double xStep = orientation * sinVal;
	double yStep = orientation * cosVal;
	int travelCount = (int)fabs(movingBox->forwardVelocity);

	int collideCycle;
	double lastStepX, lastStepY;

	// regular collision
	Object *currentObject = NULL;
	ObjectList->depthCounter++;

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

			if (evaluateCollideMode(movingBox, currentObject->ObjectBox) == PUSH && ObjectList->depthCounter < COLLISION_DEPTH)
			{
				PhysicsRect *collideBox = currentObject->ObjectBox;

				double tempVelocity = collideBox->forwardVelocity;
				double tempDirection = collideBox->direction;
				double collideXPos = collideBox->xPos;
				double collideYPos = collideBox->yPos;

				
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



int ApplyForwardPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox)
{
	if (inputBox == NULL || physicsBox == NULL)
	{
		return MISSING_DATA;
	}

	double sinVal = sin(inputBox->direction);
	double cosVal = cos(inputBox->direction);

	double forwardX = inputBox->forwardVelocity * sinVal;
	double forwardY = inputBox->forwardVelocity * cosVal;

	double physicsBoxXVel = physicsBox->xVelocity;
	double physicsBoxYVel = physicsBox->yVelocity;

	if (fabs(physicsBox->forwardVelocity) > 0.1)
	{
		physicsBoxXVel += physicsBox->forwardVelocity * sin(physicsBox->direction);
		physicsBoxYVel += physicsBox->forwardVelocity * cos(physicsBox->direction);
	}

	double newVelocity = 0.0;

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


int ResolveAllXCollision(PhysicsRect *movingBox, ObjectController *ObjectList)
{
	if (movingBox == NULL || ObjectList == NULL)
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
			PhysicsRect *collideBox = currentObject->ObjectBox;
			ResolveXCollisionByPush(movingBox, collideBox);

			double prevXVel = collideBox->xVelocity;
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


int ResolveXCollision(PhysicsRect *movingBox, PhysicsRect *compareBox, ObjectController *ObjectList)
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

			double slope = ((double)compareBox->ySize/(double)compareBox->xSize);
			double slopeFloor;

			if (compareBox->xFlip == 1)
			{
				slopeFloor = (movingBox->xPos + movingBox->xSize - compareBox->xPos) * slope;
			}
			else
			{
				slopeFloor = (compareBox->xSize - movingBox->xPos + compareBox->xPos) * slope;
			}

			slopeFloor = dClamp(slopeFloor, 0.0, compareBox->ySize);

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


int ApplyXPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox)
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


int ResolveAllYCollision(PhysicsRect *movingBox, ObjectController *ObjectList)
{
	if (movingBox == NULL || ObjectList == NULL)
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
			PhysicsRect *collideBox = currentObject->ObjectBox;
			ResolveYCollisionByPush(movingBox, collideBox);

			double prevYVel = collideBox->yVelocity;
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


int ResolveYCollision(PhysicsRect *movingBox, PhysicsRect *compareBox)
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
			double slope = ((double)compareBox->ySize/(double)compareBox->xSize);
			double slopeFloor;

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


int ApplyYPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox)
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


int ResolveXCollisionByPush(PhysicsRect *movingBox, PhysicsRect *compareBox)
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


int ResolveAllXCollisionsByPush(PhysicsRect *movingBox, ObjectController *ObjectList)
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


int ResolveAllYCollisionsByPush(PhysicsRect *movingBox, ObjectController *ObjectList)
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


int ResolveYCollisionByPush(PhysicsRect *movingBox, PhysicsRect *compareBox)
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


int ClimbSlope(PhysicsRect *inputBox, PhysicsRect *compareBox, ObjectController *ObjectList)
{		
	if (inputBox == NULL || compareBox == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	double savedForwardVelocity = inputBox->forwardVelocity;
	double savedDirection = inputBox->direction;

	double velocity = inputBox->xVelocity + inputBox->PhysicsXVelocity;

	if (fabs(velocity) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}


	inputBox->direction = RADIAN_90;
	inputBox->forwardVelocity = velocity;


	moveObjectForward(inputBox, ObjectList);

	inputBox->forwardVelocity = savedForwardVelocity;
	inputBox->direction = savedDirection;

	return LEMON_SUCCESS;
}
