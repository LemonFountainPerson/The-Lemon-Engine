#include "gameObjects.h"



Object* AddObject(World *GameWorld, int objectID, int xPos, int yPos, int xSize, int ySize, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return NULL;
	}

	int maxObjects = EngineSettings.MaxObjects;

	if (GameWorld->GameState == LOADING)
	{
		maxObjects -= EngineSettings.ReservedObjects;
	}

	if (GameWorld->ObjectList->objectCount + GameWorld->ObjectList->cachedCount >= maxObjects)
	{
		return NULL;
	}


	if (objectID >= UNDEFINED_OBJECT || objectID < LEVEL_FLAG_OBJ)
	{
		putDebugStrInt("\nThis object is not defined! Type: ", objectID);
		return NULL;
	}

	ObjectController *ObjectList = GameWorld->ObjectList;

	Object *newObject;

	if (EngineSettings.ObjectPreAllocationEnabled == 1)
	{
		//newObject = findNewObject(ObjectList);
	}
	else
	{
		newObject = createNewObject(objectID, ObjectList);
	} 

	if (newObject == NULL)
	{
		return NULL;
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

	newObject->ParentObject = NULL;
	newObject->ObjectID = objectID;
	newObject->State = DEFAULT;
	newObject->Action = IDLE;
	newObject->layer = MIDDLEGROUND;
	newObject->ParentLink = DEFAULT_LINK;

	newObject->ObjectDisplay->currentSprite = 1;

	if (objectID == UI_ELEMENT || objectID == UI_TEXT)
	{
		newObject->ObjectBox->xPos = xPos;
		newObject->ObjectBox->yPos = yPos;
	}
	else
	{
		newObject->ObjectBox->xPos = (double)abs(xPos - (xPos % X_TILESCALE));
		newObject->ObjectBox->yPos = (double)abs(yPos - (yPos % Y_TILESCALE));
	}

	// Set Object parameters
	switch (objectID)
	{
		case PROJECTILE:
			newObject->ObjectBox->xSize = X_TILESCALE;
			newObject->ObjectBox->ySize = 8;

			newObject->ObjectBox->forwardVelocity = 10.0;
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
				newObject->ParentObject = AddObject(GameWorld, DOOR, arg1, arg2, xSize, ySize, xPos, yPos, 1, 0, 0);

				newObject->ParentObject->ParentObject = newObject;
			}
			
		break;

		case SOLID_BLOCK:
			newObject->ObjectBox->xSize = xSize * X_TILESCALE;
			newObject->ObjectBox->ySize = ySize * Y_TILESCALE;
			newObject->State = STATIC;

			if (arg1 > 0)
			{
				switchObjectSprite(arg1, newObject, ObjectList);
			}
			else
			{
				newObject->ObjectDisplay->RenderModeOverride = DO_NOT_RENDER;
			}
			break;


		case FLAT_SLOPE_FLOOR:
		//Angle: Y = (X * ySize/xSize)
		//Angle: X = (Y / (ySize/xSize))
			newObject->ObjectBox->solid = FLAT_SLOPE;

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
				switchObjectSprite(3, newObject, ObjectList);
			}
			else if (xSize + ySize > 128)
			{
				switchObjectSprite(2, newObject, ObjectList);
			}
			break;


		case JUMP_THRU_BLOCK:
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
			PlayAnimation("Coin_Spin", 0, newObject->ObjectDisplay);
			break;

		
		case SPRING:
		// spring
			newObject->arg1 = arg1;
			SetObjectDirection(newObject, (double)(90 + arg2), ROTATE_ALL);
			newObject->ObjectBox->solid = UNSOLID;
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
			break;


		case GATE_SWITCH:
		// switch for gate - arg1 is ID to match switch to gate, arg2 denotes type of switch (0 = or switch, 1 = and switch)
			newObject->arg1 = arg1;
			newObject->arg2 = arg2;
			newObject->ObjectBox->solid = UNSOLID;
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



	// Debug
	if (DebugSettings.DebugTextEnabled == ALL_EVENTS)
	{
		putDebugStrIntStr("\nCreated object type: ", objectID, ";\n");
	}
	
	return newObject;
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


Object* createNewObject(ObjectType objectID, ObjectController *ObjectList)
{
	Object *currentObject;
	currentObject = ObjectList->firstObject;


	Object *newObject = malloc(sizeof(Object));

	if (newObject == NULL)
	{
		putDebugString("\nError: Could not allocate memory for new object.\n");
		return NULL;
	}

	memset(newObject, 0, sizeof(Object));


	newObject->ObjectBox = createPhysicsRect(SOLID);

	if (newObject->ObjectBox == NULL)
	{
		putDebugString("\nError: Could not allocate memory for new object's physics box.\n");
		free(newObject);
		return NULL;
	}


	newObject->ObjectDisplay = createDisplayData(DEFAULT_TO_SPRITE);

	if (newObject->ObjectDisplay == NULL)
	{
		putDebugString("\nError: Could not allocate memory for new object's display data.\n");
		free(newObject);
		return NULL;
	}

	newObject->ObjectDisplay->spriteSetSource = createObjectSpriteSet(ObjectList, objectID);


	int i = 0;

	if (currentObject != NULL)
	{
		i = 1;

		while (currentObject->nextObject != NULL && i < ObjectList->objectCount)
		{
			currentObject = currentObject->nextObject;
			i++;
		}

		currentObject->nextObject = newObject;
	}
	else
	{
		ObjectList->firstObject = newObject;
	}

	ObjectList->lastObject = newObject;


	ObjectList->objectCount = i + 1;

	newObject->nextObject = NULL;
	newObject->prevObject = currentObject;
	newObject->State = EMPTY_OBJECT;

	return newObject;
}


Object* findNewObject(ObjectController *ObjectList)
{
	if (ObjectList == NULL || ObjectList->firstObject == NULL)
	{
		return NULL;
	}

	Object *currentObject;

	if (ObjectList->lastObject == NULL)
	{
		currentObject = ObjectList->firstObject;
	}
	else
	{
		currentObject = ObjectList->lastObject->nextObject;
	}

	if (currentObject == NULL || currentObject->State != EMPTY_OBJECT)
	{
		return NULL;
	}


	ObjectList->lastObject = currentObject;

	ObjectList->objectCount++;

	return currentObject;
}


void deleteObject(Object *input, ObjectController *ObjectList)
{
	if (input == NULL || ObjectList == NULL)
	{
		return;
	}


	Object *prevObject;
	prevObject = input->prevObject;


	if (input->nextObject != NULL)
	{
		input->nextObject->prevObject = prevObject;
	}
	else
	{
		ObjectList->lastObject = prevObject;
	}

	if (prevObject != NULL)
	{
		prevObject->nextObject = input->nextObject;
	}
	else
	{
		ObjectList->firstObject = input->nextObject;
	}

	Object *currentObject = ObjectList->lastObject;
	Object *toDelete;

	while (currentObject != NULL)
	{
		toDelete = currentObject;
		currentObject = currentObject->prevObject;

		if (toDelete->ParentObject == input)
		{
			deleteObject(toDelete, ObjectList);
		}
	}


	// Delete associated frame function(s)
	if (ObjectList->FrameUpdates == NULL)
	{
		return;
	}

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


	free(input->ObjectDisplay);
	free(input->ObjectBox);
	free(input);
	

	return;
}


void removePreAllocatedObject(Object *inputObject, ObjectController *ObjectList)
{
	if (inputObject == NULL || ObjectList == NULL || ObjectList->lastObject == NULL)
	{
		return;
	}

	inputObject->State = EMPTY_OBJECT;
	Object *prevObject = inputObject->prevObject;
	Object *nextObject = inputObject->nextObject;

	inputObject->prevObject = ObjectList->lastObject;
	inputObject->nextObject = ObjectList->lastObject->nextObject;

	if (ObjectList->lastObject->nextObject == NULL)
	{
		ObjectList->lastObject->nextObject;	
	}
	else
	{
		ObjectList->lastObject->nextObject->prevObject = inputObject;
	}
	
	ObjectList->lastObject->nextObject = inputObject;

	if (prevObject != NULL)
	{
		prevObject->nextObject = nextObject;
	}
	else
	{
		ObjectList->firstObject = nextObject;
	}

	if (nextObject != NULL)
	{
		nextObject->prevObject = prevObject;
	}
	else
	{
		ObjectList->lastObject = prevObject;
	}



	return;
}


PhysicsRect* createPhysicsRect(SolidType inputSolid)
{
	PhysicsRect *newRect = malloc(sizeof(PhysicsRect));

	if (newRect == NULL)
	{
		return NULL;
	}

	newRect->xPos = 0.0;
	newRect->yPos = 0.0;
	newRect->prevXPos = 0.0;
	newRect->prevYPos = 0.0;
	newRect->xPosRight = 0.0;
	newRect->yPosTop = 0.0;
	newRect->xSize = 0;
	newRect->ySize = 0;
	newRect->xFlip = 1;
	newRect->yFlip = 1;
	newRect->direction = RADIAN_90;

	newRect->solid = inputSolid;
	newRect->collideMode = NO_COLLIDE_TYPE;
	newRect->collideLayer = MIDDLEGROUND;
	newRect->xVelocity = 0.0;
	newRect->yVelocity = 0.0;
	newRect->PhysicsXVelocity = 0.0;
	newRect->PhysicsYVelocity = 0.0;
	newRect->forwardVelocity = 0.0;
	newRect->crouch = 0;
	newRect->inAir = 0;
	newRect->GroundBox = NULL;

	return newRect;
}


DisplayData* createDisplayData(RenderMode startRenderMode)
{
	DisplayData *newDisplay = malloc(sizeof(DisplayData));

	newDisplay->currentSprite = 1;
	newDisplay->spriteBuffer = NULL;
	newDisplay->spriteSetSource = NULL;
	newDisplay->RenderModeOverride = startRenderMode;

	newDisplay->direction = RADIAN_90;
	newDisplay->spriteXOffset = 0;
	newDisplay->spriteYOffset = 0;
	newDisplay->pixelXOffset = 0;
	newDisplay->pixelYOffset = 0;

	newDisplay->currentAnimation = 0;
	newDisplay->frameBuffer = NULL;
	newDisplay->animationBuffer = NULL;
	newDisplay->animationTick = 0.0;
	newDisplay->animationLoopCount = 0;
	newDisplay->transparencyEffect = 1.0;
	newDisplay->invincibilityFrames = 0;

	return newDisplay;
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
		putDebugString("\nFailed to allocate memory for new sprite set.");
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


int switchObjectSprite(int spriteID, Object *inputObject, ObjectController *ObjectList)
{
	if (inputObject == NULL || inputObject->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	DisplayData *ObjectDisplay = inputObject->ObjectDisplay;

	if (inputObject->ObjectID == LEVEL_FLAG_OBJ || ObjectDisplay->spriteBuffer != NULL && ObjectDisplay->spriteBuffer->spriteID == spriteID)
	{
		ObjectDisplay->currentSprite = spriteID;
		return EXECUTION_UNNECESSARY;
	}


	SpriteSet *currentSet = ObjectDisplay->spriteSetSource;


	// Find correct sprite from sprite set
	Sprite *currentSprite;
	int i;

	if (spriteID > currentSet->spriteCount >> 1)
	{
		currentSprite = currentSet->lastSprite;
		i = currentSet->spriteCount;

		while (i > 0 && currentSprite != NULL && currentSprite->spriteID != spriteID)
		{
			currentSprite = currentSprite->prevSprite;
			i--;
		}

	}
	else
	{
		currentSprite = currentSet->firstSprite;
		i = 1;

		while (i < currentSet->spriteCount && currentSprite != NULL && currentSprite->spriteID != spriteID)
		{
			currentSprite = currentSprite->nextSprite;
			i++;
		}

	}

	if (currentSprite == NULL || currentSprite->spriteID != spriteID)
	{
		printf("\nCould not find sprite %d for object %d", spriteID, inputObject->ObjectID);
		return MISSING_DATA;
	}

	ObjectDisplay->spriteBuffer = currentSprite;
	ObjectDisplay->currentSprite = spriteID;
		
	return LEMON_SUCCESS;
}


int switchObjectSpriteName(char spriteName[], Object *inputObject, ObjectController *ObjectList)
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


	SpriteSet *currentSet = ObjectDisplay->spriteSetSource;

	// Find correct sprite from sprite set
	Sprite *currentSpritePtr;
	currentSpritePtr = currentSet->lastSprite;
	
	int i = currentSet->spriteCount;

	while (i > 0 && currentSpritePtr != NULL && strcmp(currentSpritePtr->spriteName, spriteName) != 0)
	{
		currentSpritePtr = currentSpritePtr->prevSprite;
		i--;
	}

	
	if (currentSpritePtr == NULL || strcmp(currentSpritePtr->spriteName, spriteName) != 0)
	{
		printf("Could not find sprite %s for object %d\n", spriteName, inputObject->ObjectID);
		return MISSING_DATA;
	}

	ObjectDisplay->spriteBuffer = currentSpritePtr;
	ObjectDisplay->currentSprite = i;

	return LEMON_SUCCESS;
}


void deleteAllObjects(ObjectController *ObjectList)
{
	if (ObjectList == NULL )
	{
		return;
	}
	
	Object *temp;
	Object *currentObject = ObjectList->lastObject;

	while (currentObject != NULL)
	{
		temp = currentObject;
		currentObject = currentObject->prevObject;
		deleteObject(temp, ObjectList);
	}
	
	ObjectList->objectCount = 0;
	ObjectList->firstObject = NULL;
	ObjectList->lastObject = NULL;


	currentObject = ObjectList->cachedLastObject;

	while (currentObject != NULL)
	{
		temp = currentObject;
		currentObject = currentObject->prevObject;
		deleteObject(temp, ObjectList);
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


	if (DebugSettings.DebugTextEnabled == ALL_EVENTS)
	{
		putDebugStrIntStr("\nMarked object type for deletion:", inputObject->ObjectID, "\n");
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

	putDebugStrIntStr("\nUnMarked object type for deletion!:", inputObject->ObjectID, "\n");


	return LEMON_SUCCESS;
}


// These functions can be safely called from anywhere, however due to the fact that they modify the order of the object list itself they may
// produce unintended behaviour. Eg: incrementing skips an object in the list, decrementing repeats the previous object, SetToBack 
// skips the rest of the objects in the list and essentially ends that frame's behaviour excecution and SetToFront repeats execution for all objects.
// This may or may not be significant, so when wanting to modify what is rendered above, it is recommended to simplify modify the layer variable
// BASICALLY, unless you know what your doing, avoid using these functions if possible
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


int cacheObjects(ObjectController *ObjectList, PhysicsRect *boundingBox, PlayerData *Player)
{	
	if (ObjectList == NULL || boundingBox == NULL)
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

		if (checkBoxOverlapsBoxBroad(boundingBox, temp->ObjectBox) == 0)
		{
			moveObjectToCachedList(ObjectList, temp);
		}
	}


	return LEMON_SUCCESS;
}


// Updates all objects in GameWorld
FunctionResult updateObjects(World *GameWorld)
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


	Object *currentObject = ObjectList->firstObject;

	// Update Behaviour
	while(currentObject != NULL)
	{
		ObjectBehaviour(GameWorld, currentObject);
		UpdateObjectDisplay(currentObject, GameWorld);

		currentObject = currentObject->nextObject;
	}

	// Update object state - animations, parent-child links, deletion, etc.
	currentObject = ObjectList->lastObject;
	Object *toDelete;

	while (currentObject != NULL)
	{
		if (currentObject->State == TO_BE_DELETED || currentObject->ObjectBox == NULL || currentObject->ObjectDisplay == NULL)
		{
			toDelete = currentObject;
			currentObject = currentObject->prevObject;

			if (GameWorld->Player->PlayerPtr == toDelete)
			{
				PlayerObjectAboutToBeDeleted(GameWorld->Player);
			}

			if (EngineSettings.ObjectPreAllocationEnabled == 1)
			{
				removePreAllocatedObject(toDelete, ObjectList);
			}
			else
			{
				deleteObject(toDelete, ObjectList);
			}
		}
		else
		{
			UpdatePhysicsState(currentObject, GameWorld);
			UpdateParentChildLink(currentObject);

			currentObject = currentObject->prevObject;
		}

	}


	return LEMON_SUCCESS;
}


int ObjectBehaviour(World *GameWorld, Object *inputObject)
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL || inputObject->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GamePaused == 1 && inputObject->ObjectID != UI_ELEMENT || inputObject->State == STATIC || inputObject->State < DEFAULT)
	{
		return EXECUTION_UNNECESSARY;
	}

	if (inputObject->State == PAUSE_BEHAVIOUR || (GameWorld->GameState == CUTSCENE && inputObject->State != ACTOR) || inputObject->State == IN_INVENTORY)
	{
		return ACTION_DISABLED;
	}


	switch (inputObject->ObjectID)
	{
		case UI_ELEMENT:
			UpdateUIElement(GameWorld, inputObject);
			break;

		case PARTICLE:
			UpdateParticle(GameWorld, inputObject);
			break;


		case LEVEL_FLAG_OBJ:
			UpdateFlagObject(GameWorld, inputObject);
			break;


		case MOVING_PLATFORM_HOR:
			UpdateHorizontalPlatform(GameWorld->Player, inputObject);
		 	break;


		case MOVING_PLATFORM_VER:
			UpdateVerticalPlatform(GameWorld->Player, inputObject);
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
			printf("\n%lf %lf\n", inputObject->ObjectBox->xVelocity, inputObject->ObjectBox->forwardVelocity);
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
			ApplyFriction(inputObject->ObjectBox, 0.9, 0.9, 1.0);
			ApplyGravity(inputObject, GameWorld);
			break;


		case PUSHABLE_BOX:
			ApplyFriction(inputObject->ObjectBox, 0.9, 0.9, 1.0);
			ApplyGravity(inputObject, GameWorld);

			if (PlayerInteractingWithBox(GameWorld->Player, inputObject->ObjectBox) == 1)
			{
				PointObjectTowards(inputObject, GameWorld->Player->PlayerPtr, ROTATE_BOX);

				if (sin(inputObject->ObjectBox->direction) > 0.0)
				{
					inputObject->ObjectBox->forwardVelocity = -15.0;
				}
				else
				{
					inputObject->ObjectBox->forwardVelocity = 15.0;
				}

				SetObjectDirection(inputObject, 90.0, ROTATE_BOX);
			}
			break;


		case PLAYER_OBJECT:
			return EXECUTION_UNNECESSARY;
			break;

		case PROJECTILE:
			{
				inputObject->arg1++;

				if (inputObject->arg1 > 9999)
				{
					MarkObjectForDeletion(inputObject);
				}

				break;
				Object *hitTarget;
				hitTarget = GetOverlappingObjectAllSolids(inputObject, GameWorld->ObjectList);

				if (hitTarget == NULL || hitTarget == GameWorld->Player->PlayerPtr)
				{
					break;
				}	

				if (hitTarget->ObjectBox->solid == ENTITY && hitTarget->Action != DAMAGED)
				{
					hitTarget->ObjectDisplay->invincibilityFrames = 150;
					hitTarget->Action = DAMAGED;
					MarkObjectForDeletion(inputObject);
				}
				else
				{
					MarkObjectForDeletion(inputObject);
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


int UpdateObjectDisplay(Object *inputObject, World *GameWorld)
{
	if (GameWorld == NULL || inputObject == NULL || inputObject->ObjectDisplay == NULL)	{ return MISSING_DATA; }

	if (GameWorld->GamePaused == 1 && inputObject->ObjectID != UI_ELEMENT || inputObject->State < DEFAULT)
	{
		return ACTION_DISABLED;
	}


	if (inputObject->Action == DAMAGED)
	{
		DamagedFrames(inputObject);
		return EXECUTION_UNNECESSARY;
	}


	iterateAnimation(inputObject->ObjectDisplay);

	// Assign Sprite   
	if (inputObject->ObjectDisplay->currentSprite > 0)
	{
		switchSprite(inputObject->ObjectDisplay->currentSprite, USE_CURRENT_SPRITESET, inputObject->ObjectDisplay);
	}

	return LEMON_SUCCESS;
}


int UpdatePhysicsState(Object *inputObject, World *GameWorld)
{
	PhysicsRect *inputBox = inputObject->ObjectBox;
	PhysicsRect *GroundBox = inputBox->GroundBox;

	if (GroundBox == NULL)
	{
		return ACTION_DISABLED;
	}

	if (GroundBox == inputBox)
	{
		inputBox->GroundBox = NULL;
		//CheckForGround(inputBox, GameWorld);
		
		return EXECUTION_UNNECESSARY;
	}
	
	// Apply magnetisation
	double savedXVelocity = inputBox->xVelocity;
	double savedYVelocity = inputBox->yVelocity;

	
	// Ensure that velocity applied is not necessary in the case of it moving against gravity
	double pixelXDifference = (int)GroundBox->xPos - (int)GroundBox->prevXPos;

	GroundBox->xPos += pixelXDifference; 
		
	if (fabs(GameWorld->GlobalGravityX) < 0.1 || CheckBoxCollidesBox(GroundBox, inputBox) == 0)
	{
		inputBox->PhysicsXVelocity = pixelXDifference;
	}

	GroundBox->xPos -= pixelXDifference; 

	double pixelYDifference = (int)GroundBox->yPos - (int)GroundBox->prevYPos;
	GroundBox->yPos += pixelYDifference; 
		
	if (fabs(GameWorld->GlobalGravityY) < 0.1 || CheckBoxCollidesBox(GroundBox, inputBox) == 0)
	{
		inputBox->PhysicsYVelocity = pixelYDifference;
	}

	GroundBox->yPos -= pixelYDifference; 


	inputBox->xVelocity = inputBox->PhysicsXVelocity;
	inputBox->yVelocity = inputBox->PhysicsYVelocity;

	moveObjectX(inputBox, GameWorld->ObjectList);
	moveObjectY(inputBox, GameWorld->ObjectList);

	inputBox->xVelocity = savedXVelocity;
	inputBox->yVelocity = savedYVelocity;


	inputBox->GroundBox = NULL;


	return LEMON_SUCCESS;
}


int UpdateParentChildLink(Object *inputObject)
{
	if (inputObject->ParentObject == NULL)
	{
		return EXECUTION_UNNECESSARY;
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


FunctionResult updateObjectsFrame(World *GameWorld)
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


int AddFrameUpdateFunction(int (*FunctionPointer)(Object*, World*), Object *inputObject, World *GameWorld)
{
	if (inputObject == NULL || GameWorld == NULL)
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

	FrameUpdateFunction *CurrentFrameUpdate = (FrameUpdateFunction *)GameWorld->ObjectList->FrameUpdates;

	if (CurrentFrameUpdate == NULL)
	{
		GameWorld->ObjectList->FrameUpdates = (void *)function;
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


int DamagedFrames(Object *inputObject)
{
	DisplayData *inputData = inputObject->ObjectDisplay;


	inputData->invincibilityFrames--;

	if (inputData->invincibilityFrames < 1)
	{
		inputObject->Action = IDLE;
		inputData->RenderModeOverride = DEFAULT_TO_SPRITE;
		return ACTION_DISABLED;
	}


	if (inputData->invincibilityFrames % 12 != 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	if (inputData->RenderModeOverride == DEFAULT_TO_SPRITE)
	{
		inputData->RenderModeOverride = DO_NOT_RENDER;
		inputData->invincibilityFrames -= 4;
	}
	else
	{
		inputData->RenderModeOverride = DEFAULT_TO_SPRITE;
	}
	

	return LEMON_SUCCESS;
}


int UpdateCoin(Object *coin, World *GameWorld)
{
	if (coin == NULL || GameWorld == NULL || GameWorld->Player == NULL || GameWorld->Player->PlayerBox == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsRect *PlayerBox = GameWorld->Player->PlayerBox;

	if (checkBoxOverlapsBoxBroad(PlayerBox, coin->ObjectBox) == 1)
	{
		GameWorld->Player->coinCount++;
		AddParticle(GameWorld, SPARKLE, coin->ObjectBox->xPos, coin->ObjectBox->yPos, 1, 0);
		MarkObjectForDeletion(coin);
		LemonPlaySound("Coin_Collect", "Objects", OBJECT_SFX, 0.75);
	}

	return LEMON_SUCCESS;
}


int UpdateSpring(Object *spring, World *GameWorld)
{
	if (spring == NULL || GameWorld == NULL || GameWorld->Player == NULL || GameWorld->Player->PlayerBox == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsRect *PlayerBox = GameWorld->Player->PlayerBox;


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
		
		LemonPlaySound("Spring", "Objects", OBJECT_SFX, 1.0);
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
	particle->arg2 = 0;
	particle->arg3 = 0;

	PlayAnimationByIndex(animation, repeatCount, particle->ObjectDisplay);


	switch (animation)
	{
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
	if (particle->arg1 > 0 && particle->arg2 > particle->arg1)
	{
		MarkObjectForDeletion(particle);
	}
	else if (particle->arg1 < 1 && particle->ObjectDisplay->currentAnimation == 0)
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


int UpdateGateSwitch(PlayerData *player, Object *gateSwitch, ObjectController *ObjectList)
{
	if (player == NULL || gateSwitch == NULL || ObjectList == NULL)
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
	// arg1 = door ID
	// arg2 = door open/close (0/1)
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
			SetObjectYPosition(gate->ObjectBox, closedPosition, GameWorld->ObjectList);
			gate->ObjectBox->yVelocity = 0.0;

			if (gate->arg2 == 1)
			{
				gate->ObjectDisplay->currentAnimation = 1;
				LemonPlaySound("GateOpen", "Objects", 4, 1.0);
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
				SetObjectYPosition(gate->ObjectBox, closedPosition + (gate->ObjectBox->ySize * (speed/abs(speed))), GameWorld->ObjectList);
			}

		} break;


		case 2:
		{
			SetObjectYPosition(gate->ObjectBox, closedPosition + (gate->ObjectBox->ySize * (speed/abs(speed))), GameWorld->ObjectList);
			gate->ObjectBox->yVelocity = 0.0;

			if (gate->arg2 == 0)
			{
				gate->ObjectDisplay->currentAnimation = 3;
				LemonPlaySound("GateClose", "Objects", 4, 1.0);
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
				SetObjectYPosition(gate->ObjectBox, closedPosition, GameWorld->ObjectList);
			}

		} break;
	}


	return LEMON_SUCCESS;
}


int UpdateHorizontalGate(Object *gate, World *GameWorld)
{
	// arg1 = door ID
	// arg2 = door open/close (0/1)
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
			SetObjectXPosition(gate->ObjectBox, closedPosition, GameWorld->ObjectList);
			gate->ObjectBox->xVelocity = 0.0;

			if (gate->arg2 == 1)
			{
				gate->ObjectDisplay->currentAnimation = 1;
				LemonPlaySound("GateOpen", "Objects", 4, 1.0);
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
				SetObjectXPosition(gate->ObjectBox, closedPosition + (gate->ObjectBox->xSize * (speed/abs(speed))), GameWorld->ObjectList);
			}

		} break;


		case 2:
		{
			SetObjectXPosition(gate->ObjectBox, closedPosition + (gate->ObjectBox->xSize * (speed/abs(speed))), GameWorld->ObjectList);
			gate->ObjectBox->xVelocity = 0.0;

			if (gate->arg2 == 0)
			{
				gate->ObjectDisplay->currentAnimation = 3;
				LemonPlaySound("GateClose", "Objects", 4, 1.0);
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
				SetObjectXPosition(gate->ObjectBox, closedPosition, GameWorld->ObjectList);
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
	inputObject->ObjectBox->collideMode = PUSH;


	switch (objectID)
	{

		default:
		break;
	}

	return inputObject;
}


int UpdateHorizontalPlatform(PlayerData *player, Object *platform)
{
	double YPos = platform->ObjectBox->yPos;
	double YPos2 = platform->ObjectBox->yPos + platform->ObjectBox->ySize;
	double XPos = platform->ObjectBox->xPos;
	double XPos2 = platform->ObjectBox->xPos + platform->ObjectBox->xSize;

	int leftBound = platform->arg1;
	int rightBound = platform->arg2;
	int maxSpeed = platform->arg3;
	int timer = platform->arg5;

	// Accelerate
	if (platform->arg4 > 0 && platform->ObjectBox->xVelocity < (double)maxSpeed && XPos < (double)rightBound)
	{
		platform->ObjectBox->xVelocity += 0.5;
	}

	if (platform->arg4 < 0 && platform->ObjectBox->xVelocity > -(double)maxSpeed && XPos > (double)leftBound)
	{
		platform->ObjectBox->xVelocity -= 0.5;
	}

	// Deccelerate
	if (XPos > (double)rightBound && platform->arg4 > 0 || XPos < (double)leftBound && platform->arg4 < 0)
	{
		platform->ObjectBox->xVelocity *= 0.9;
	}
	
	if (fabs(platform->ObjectBox->xVelocity) < 0.1)
	{
		platform->ObjectBox->xVelocity = 0.0;
	}


	// Wait
	if (platform->arg4 > 0 && fabs(platform->ObjectBox->xVelocity) < 0.5 && XPos > (double)rightBound)
	{
		platform->arg4++;
		if (platform->arg4 > timer)
		{
			platform->arg4 = -1;
		}
	}

	if (platform->arg4 < 0 && fabs(platform->ObjectBox->xVelocity) < 0.5 && XPos < (double)leftBound)
	{
		platform->arg4--;
		if (platform->arg4 < -timer)
		{
			platform->arg4 = 1;
		}
	}


	return LEMON_SUCCESS;
}



int UpdateVerticalPlatform(PlayerData *player, Object *platform)
{
	double YPos = platform->ObjectBox->yPos;
	double YPos2 = platform->ObjectBox->yPos + platform->ObjectBox->ySize;
	double XPos = platform->ObjectBox->xPos;
	double XPos2 = platform->ObjectBox->xPos + platform->ObjectBox->xSize;

	int bottomBound = platform->arg1;
	int topBound = platform->arg2;
	int maxSpeed = platform->arg3;
	int timer = platform->arg5;

	// Accelerate
	if (platform->arg4 > 0 && platform->ObjectBox->yVelocity < maxSpeed && YPos < (double)topBound)
	{
		platform->ObjectBox->yVelocity += 0.75;
	}

	if (platform->arg4 < 0 && platform->ObjectBox->yVelocity > -maxSpeed && YPos > (double)bottomBound)
	{
		platform->ObjectBox->yVelocity -= 0.75;
	}

	// Deccelerate
	if (YPos > (double)topBound && platform->arg4 > 0 || YPos < (double)bottomBound && platform->arg4 < 0)
	{
		platform->ObjectBox->yVelocity *= 0.9;
	}

	if (fabs(platform->ObjectBox->yVelocity) < 0.1)
	{
		platform->ObjectBox->yVelocity = 0.0;
	}

	// Wait
	if (platform->arg4 > 0 && fabs(platform->ObjectBox->yVelocity) < 0.5 && YPos > (double)topBound)
	{
		platform->arg4++;
		if (platform->arg4 > timer)
		{
			platform->arg4 = -1;
		}
	}

	if (platform->arg4 < 0 && fabs(platform->ObjectBox->yVelocity) < 0.5 && YPos < (double)bottomBound)
	{
		platform->arg4--;
		if (platform->arg4 < -timer)
		{
			platform->arg4 = 1;
		}
	}

	return LEMON_SUCCESS;
}


int UpdateDoor(PlayerData *Player, Object *Door, World *GameWorld)
{
	// arg3: open/close state
	// arg1, arg2: [x/y]Pos in event of no linked door
	if (Player->PlayerPtr == NULL || Door == NULL)
	{
		return MISSING_DATA;
	}

	if (Door->arg3 < 1 && PlayerInteractingWithBox(Player, Door->ObjectBox) == 1 && Player->PlayerPtr->State == DEFAULT)
	{
		//SayText("It's a door.", NO_PORTRAIT, WB_BOTTOM, GameWorld);
		//SayText("....Or is it?\n\rIt just looks like a big pink and black rectangle...", NO_PORTRAIT, WB_BOTTOM, GameWorld);
		
		LemonPlaySound("DoorOpen", "Objects", OBJECT_SFX, 1.0);
		ResetPlayer(Player);
		Player->PlayerPtr->State = PAUSE_BEHAVIOUR;

		Door->arg3 = 32;

		GoTo(Player->PlayerBox, Door->arg1, Door->arg2);
		
		if (Door->ParentObject != NULL && Door->ParentObject->ObjectBox != NULL)
		{
			Door->ParentObject->arg3 = 32;
		}
	}
	else if (Door->arg3 > 0)
	{
		Door->arg3--;

		if (Player->PlayerPtr->State == PAUSE_BEHAVIOUR && Door->arg3 < 1)
		{
			Player->PlayerPtr->State = DEFAULT;
		}
	}

	return LEMON_SUCCESS;
} 


int UpdateLevelDoor(PlayerData *Player, Object *Door, World *GameWorld)
{
	// arg1: Level to load
	// arg2: Open/close state
	if (Player->PlayerBox == NULL || Door == NULL)
	{
		return MISSING_DATA;
	}

	if (Door->arg2 < 1 && PlayerInteractingWithBox(Player, Door->ObjectBox) == 1 && Player->PlayerPtr->State == DEFAULT)
	{
		Door->arg2 = 1;
		SayText("It's a door.\f.\f.\r \nIt eminates a strange glow.", NO_PORTRAIT, WB_BOTTOM, GameWorld);
		char phrase[] = "This Door will send you to level 0!";
		phrase[33] += Door->arg1;
		SayText(phrase, NO_PORTRAIT, WB_BOTTOM, GameWorld);
	}

	if (Door->arg2 > 0 && GameWorld->TextQueue == NULL)
	{
		Door->arg2++;

		if (Door->arg2 > 1)
		{
			GameWorld->GameEvent = SWITCH_LEVEL;
			GameWorld->level = Door->arg1;
			Door->arg2 = 0;
		}
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


int ApplyFriction(PhysicsRect *inputBox, double forwardFriction, double xFriction, double yFriction)
{
	if (inputBox == NULL)
	{
		return MISSING_DATA;
	}

	if (forwardFriction < 0.999)
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
	

	if (xFriction < 0.999)
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


	if (yFriction < 0.999)
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


	movingBox->yPos += GameWorld->GlobalGravityY * 2.0;
	movingBox->xPos += GameWorld->GlobalGravityX * 2.0;

	Object *GroundObject = GetCollidingObject(movingBox, GameWorld->ObjectList);

	movingBox->yPos -= GameWorld->GlobalGravityY * 2.0;
	movingBox->xPos -= GameWorld->GlobalGravityX * 2.0;


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

		movingBox->inAir = 0;

		// GroundBox should only be set once, ideally by the AppyGravity function, so if it has been set, do not override
		if (movingBox->GroundBox == NULL)
		{
			movingBox->GroundBox = GroundObject->ObjectBox;
		}
	}
	else
	{
		movingBox->inAir++;

		if (movingBox->inAir > 100)
		{
			movingBox->inAir = 100;

			// Falling for a long time!
		}

		// To indicate no ground, the pointer is set to the box itself
		if (movingBox->GroundBox == NULL)
		{
			movingBox->GroundBox = movingBox;
		}
	}

	return GroundObject;
}


int GoTo(PhysicsRect *inputObject, double destX, double destY)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	inputObject->xPos = destX;
	inputObject->yPos = destY;
	inputObject->prevXPos = destX;
	inputObject->prevYPos = destY;
	inputObject->xPosRight = destX + inputObject->xSize;
	inputObject->yPosTop = destY + inputObject->ySize;

	return LEMON_SUCCESS;
}


int GoToWithCollision(PhysicsRect *inputObject, double destX, double destY,  ObjectController *ObjectList)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}


	GoTo(inputObject, destX, destY);


	if (ObjectList == NULL || inputObject->solid == UNSOLID)
	{
		return MISSING_DATA;
	}

	ResolveAllXCollisionsByPush(inputObject, ObjectList);
	ResolveAllYCollisionsByPush(inputObject, ObjectList);


	return LEMON_SUCCESS;
}


int SetObjectXPosition(PhysicsRect *inputObject, double newXPos, ObjectController *ObjectList)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	if (fabs(inputObject->xPos - newXPos) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}


	GoTo(inputObject, newXPos, inputObject->yPos);


	if (ObjectList == NULL || inputObject->solid == UNSOLID)
	{
		return MISSING_DATA;
	}

	ResolveAllXCollisionsByPush(inputObject, ObjectList);

	return LEMON_SUCCESS;
}


//Method for setting an object's position to avoid incorrect collision with player
int SetObjectYPosition(PhysicsRect *inputObject, double newYPos, ObjectController *ObjectList)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	if (fabs(inputObject->yPos - newYPos) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}


	GoTo(inputObject, inputObject->xPos, newYPos);


	if (ObjectList == NULL || inputObject->solid == UNSOLID)
	{
		return MISSING_DATA;
	}

	ResolveAllYCollisionsByPush(inputObject, ObjectList);

	return LEMON_SUCCESS;
}


int PointObjectTowards(Object *inputObject, Object *pointDestination, RotateMode RotateSetting)
{
	if (pointDestination == NULL || pointDestination->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	double destX = pointDestination->ObjectBox->xPos + (pointDestination->ObjectBox->xSize >> 1);
	double destY = pointDestination->ObjectBox->yPos + (pointDestination->ObjectBox->ySize >> 1);

	PointObjectToXY(inputObject, destX, destY, RotateSetting);

	return LEMON_SUCCESS;
}


int PointObjectToMouse(Object *inputObject, World *GameWorld, RotateMode RotateSetting)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	double MouseX = MouseInput.xPos + GameWorld->MainCamera.CameraX;
	double MouseY = MouseInput.yPos + GameWorld->MainCamera.CameraY;

	PointObjectToXY(inputObject, MouseX, MouseY, RotateSetting);

	return LEMON_SUCCESS;
}


int PointObjectToXY(Object *inputObject, double xPos, double yPos, RotateMode RotateSetting)
{
	if (inputObject == NULL || inputObject->ObjectDisplay == NULL || inputObject->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	double originX = inputObject->ObjectBox->xPos + (inputObject->ObjectBox->xSize >> 1);
	double originY = inputObject->ObjectBox->yPos + (inputObject->ObjectBox->ySize >> 1);

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

	double newDirection = inputObject->ObjectBox->direction + rotationDegrees;

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

	double changeHalf = change >> 1;

	inputObject->ObjectBox->xSize += change;
	inputObject->ObjectBox->xPos -= changeHalf;
	inputObject->ObjectBox->xPosRight += changeHalf;

	if (ObjectList == NULL || change == 0 || inputObject->ObjectBox->solid < 1)
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

	double changeHalf = change >> 1;

	inputObject->ObjectBox->ySize += change;
	inputObject->ObjectBox->yPos -= changeHalf;
	inputObject->ObjectBox->yPosTop += changeHalf;

	if (ObjectList == NULL || change == 0 || inputObject->ObjectBox->solid < 1)
	{
		return EXECUTION_UNNECESSARY;
	}


	ResolveAllYCollisionsByPush(inputObject->ObjectBox, ObjectList);
	
	return LEMON_SUCCESS;
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
		case JUMP_THROUGH:
		{
			if (compareBox->yVelocity > 0.0 || compareBox->crouch == 1 || compareBox->prevYPos < (inputBox->prevYPos + inputBox->ySize - 2) )
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
			if (inputBox->yVelocity > 0.0 || inputBox->crouch == 1 || inputBox->prevYPos < (compareBox->prevYPos + compareBox->ySize - 2) )
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

	if (compareBox->xSize < 1 || compareBox->ySize < 1)
	{
		return INVALID_DATA;
	}

	
	switch (compareBox->solid)
	{
			case FLAT_SLOPE:
			{
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
				
				if (compareBox->xFlip == 1)
				{
					inputBox->direction = (RADIAN_90 - (atan(slope)));
				}
				else
				{
					inputBox->direction = (RADIAN_90 + (atan(slope)));
				}
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
	if (inputObject == NULL || inputObject->ObjectBox == NULL || inputObject->State < DEFAULT || GameWorld == NULL)	
	{ 
		return  MISSING_DATA; 
	}


	ObjectController *ObjectList = GameWorld->ObjectList;
	ObjectList->depthCounter = 0;
	PhysicsRect *inputBox = inputObject->ObjectBox;

	inputBox->prevXPos = inputBox->xPos;
	inputBox->prevYPos = inputBox->yPos;

	moveObjectX(inputBox, ObjectList);
	moveObjectY(inputBox, ObjectList);
	moveObjectForward(inputBox, ObjectList);

	inputBox->xPos = dClamp(inputBox->xPos, -EngineSettings.WorldBoundX, EngineSettings.WorldBoundX);	
	inputBox->yPos = dClamp(inputBox->yPos, -EngineSettings.WorldBoundY, EngineSettings.WorldBoundY);

	return LEMON_SUCCESS;
}


int moveObjectX(PhysicsRect *inputBox, ObjectController *ObjectList)
{
	double velocity = inputBox->xVelocity;

	if (inputBox->inAir > 0)
	{
		velocity += inputBox->PhysicsXVelocity;
	}

	if (fabs(velocity) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}

	inputBox->xPos += velocity;
	inputBox->xPosRight = inputBox->xPos + inputBox->xSize;


	if (inputBox->solid == UNSOLID)
	{
		return ACTION_DISABLED;
	}


	ResolveAllXCollision(inputBox, ObjectList);

	inputBox->xPosRight = inputBox->xPos + inputBox->xSize;


	return LEMON_SUCCESS;
}



int moveObjectY(PhysicsRect *inputBox, ObjectController *ObjectList)
{
	double velocity = inputBox->yVelocity;

	if (inputBox->inAir > 0)
	{
		velocity += inputBox->PhysicsYVelocity;
	}

	if (fabs(velocity) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}

	inputBox->yPos += velocity;
	inputBox->yPosTop = inputBox->yPos + inputBox->ySize;


	if (inputBox->solid == UNSOLID)
	{
		return ACTION_DISABLED;
	}


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

	// "0 &&" is used here to disable any un-forced push interactions
	int movingPushable = movingBox->solid == PUSHABLE_SOLID || movingBox->solid == ENTITY;
	int collidePushable = collideBox->solid == PUSHABLE_SOLID;

	if (movingBox->collideMode == PUSH || (movingPushable && collidePushable) )
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

	double xDest = movingBox->xPos + (sinVal * movingBox->forwardVelocity);
	double yDest = movingBox->yPos + (cosVal * movingBox->forwardVelocity);

	double xStep = orientation * sinVal;
	double yStep = orientation * cosVal;
	int travelCount = (int)fabs(movingBox->forwardVelocity);

	int collideCycle;
	double lastStepX, lastStepY;

	// regular collision
	Object *currentObject = NULL;

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
				ObjectList->depthCounter++;
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
		newVelocity = physicsBoxXVel * sinVal;
	}

	if (fabs(physicsBoxYVel) > 0.01 && fabs(cosVal) > 0.001 && (physicsBoxYVel > 0.1) == (forwardY > 0.1))
	{
		newVelocity = physicsBoxYVel * cosVal;
	}


	inputBox->forwardVelocity = newVelocity;

	return LEMON_SUCCESS;
}


int ResolveVectorCollision(PhysicsRect *movingBox, PhysicsRect *compareBox)
{
	if (movingBox == NULL || compareBox == NULL)
	{
		return MISSING_DATA;
	}

	double movingVectorX = movingBox->xPos - movingBox->prevXPos;
	double movingVectorY = movingBox->yPos - movingBox->prevYPos;

	double movingMagnitude = sqrt((movingVectorX * movingVectorX) + (movingVectorY * movingVectorY));

	if (fabs(movingMagnitude) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}

	double movingNormalX = movingVectorX / movingMagnitude;
	double movingNormalY = movingVectorY / movingMagnitude;

	int count = (int)movingMagnitude + 1;	

	while (CheckBoxCollidesBox(movingBox, compareBox) == 1 & count > 0)
	{
		movingBox->xPos -= movingNormalX;
		movingBox->yPos -= movingNormalY;

		count--;
	}


	return LEMON_SUCCESS;
}


int ResolveAllXCollision(PhysicsRect *movingBox, ObjectController *ObjectList)
{
	if (movingBox == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}


	int count = 0;

	Object *currentObject = GetCollidingObject(movingBox, ObjectList);

	while (currentObject != NULL && count < COLLISION_CYCLES)
	{
		if (evaluateCollideMode(movingBox, currentObject->ObjectBox) == PUSH)
		{
			ResolveXCollisionByPush(movingBox, currentObject->ObjectBox);

			double prevXVel = currentObject->ObjectBox->xVelocity;
			currentObject->ObjectBox->xVelocity = movingBox->xVelocity;
			SolidType prevType = movingBox->solid;
			movingBox->solid = UNSOLID;

			ResolveAllXCollision(currentObject->ObjectBox, ObjectList);

			movingBox->solid = prevType;
			currentObject->ObjectBox->xVelocity = prevXVel;

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

	if ((inputBox->xVelocity > 0.1) == (physicsBox->xVelocity > 0.1))
	{
		inputBox->xVelocity = physicsBox->xVelocity;
	}
	else
	{
		inputBox->xVelocity = 0.0;
		inputBox->PhysicsXVelocity = 0.0;
	}


	return LEMON_SUCCESS;
}


int ResolveAllYCollision(PhysicsRect *movingBox, ObjectController *ObjectList)
{
	if (movingBox == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}
	

	int count = 0;
	Object *currentObject;
	currentObject = GetCollidingObject(movingBox, ObjectList);

	while (currentObject != NULL && count < COLLISION_CYCLES)
	{
		if (evaluateCollideMode(movingBox, currentObject->ObjectBox) == PUSH)
		{
			ResolveYCollisionByPush(movingBox, currentObject->ObjectBox);

			double prevYVel = currentObject->ObjectBox->yVelocity;
			currentObject->ObjectBox->yVelocity = movingBox->yVelocity;
			SolidType prevType = movingBox->solid;
			movingBox->solid = UNSOLID;

			ResolveAllYCollision(currentObject->ObjectBox, ObjectList);

			currentObject->ObjectBox->yVelocity = prevYVel;
			movingBox->solid = prevType;

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
			if (movingBox->yVelocity < 0.1 && movingBox->crouch < 1)
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
		inputBox->PhysicsYVelocity = 0.0;
		inputBox->yVelocity = 0.0;
	}


	return LEMON_SUCCESS;
}


int ResolveXCollisionByPush(PhysicsRect *movingBox, PhysicsRect *compareBox)
{
	if (movingBox == NULL || compareBox == NULL || movingBox == compareBox)
	{
		return MISSING_DATA;
	}


	double ObjXPos = movingBox->xPos;
	double ObjXPosRight = movingBox->xPos + movingBox->xSize;
	double ObjYPos = movingBox->yPos;
	double ObjYPosTop = movingBox->yPos + movingBox->ySize;

	double prevXPosRight = movingBox->prevXPos + movingBox->xSize;
	double prevXPosCenter = movingBox->prevXPos + (movingBox->xSize >> 1);


	int result = 0;

	switch(movingBox->solid)
	{
		case FLAT_SLOPE:
			if (movingBox->xFlip == 1)
			{
				int slopeLeftEdge = (int)((compareBox->yPos - ObjYPos) / ((double)movingBox->ySize/(double)movingBox->xSize));
				slopeLeftEdge = clamp(slopeLeftEdge, 0, movingBox->xSize);

				ObjXPos = slopeLeftEdge + ObjXPos;
				prevXPosCenter = prevXPosRight;
			}
			else
			{
				int slopeLeftEdge = (int)(movingBox->xSize - ((compareBox->yPos - ObjYPos) / ((double)movingBox->ySize/(double)movingBox->xSize)) );
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

	double ObjXPos = movingBox->xPos;
	double ObjXPosRight = movingBox->xPos + movingBox->xSize;
	double ObjYPos = movingBox->yPos;
	double ObjYPosTop = movingBox->yPos + movingBox->ySize;

	double prevYCenter = movingBox->prevYPos + (movingBox->ySize >> 1);


	int result = 0;

	switch(movingBox->solid)
	{
		case FLAT_SLOPE:
			if (movingBox->xFlip == 1)
			{
				int slopeFloor = (int)( ((compareBox->xPos + compareBox->xSize - movingBox->xPos) * ((double)movingBox->ySize/(double)movingBox->xSize)) );
				slopeFloor = clamp(slopeFloor, 0, movingBox->ySize);

				ObjYPosTop = slopeFloor + ObjYPos;
			}
			else
			{
				int slopeFloor = (int)( ((movingBox->xSize - (compareBox->xPos - movingBox->xPos)) * ((double)movingBox->ySize/(double)movingBox->xSize)) );
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

	/*
	inputBox->xPos = inputBox->prevXPos;

	double velocity = inputBox->xVelocity + inputBox->PhysicsXVelocity;

	if (fabs(velocity) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}

	AssignDirection(inputBox, compareBox);

	double step = velocity/fabs(velocity) * sin(inputBox->direction);

	double slope = ((double)compareBox->ySize/(double)compareBox->xSize);
	Object *objectCheck = NULL;
	int i = round(fabs(velocity));


	while (objectCheck == NULL && i > 0 )
	{
		// Move inputBox forward
		inputBox->xPos += step; 

		double slopeTop = (compareBox->xFlip == 1) ? (slope * (inputBox->xPos + inputBox->xSize - compareBox->xPos)) : (slope * (compareBox->xSize - inputBox->xPos + compareBox->xPos));

		slopeTop = dClamp(slopeTop, 0.0, (double)compareBox->ySize);

		if (compareBox->yFlip == -1)
		{
			slopeTop = compareBox->ySize - slopeTop;
		}

		inputBox->yPos = slopeTop + compareBox->yPos;


		objectCheck = GetCollidingObject(inputBox, ObjectList);

		if (objectCheck != NULL && evaluateCollideMode(inputBox, objectCheck->ObjectBox) == PUSH)
		{
			double savedXVel = objectCheck->ObjectBox->xVelocity;
			double savedXPos = objectCheck->ObjectBox->xPos;
			double savedYPos = objectCheck->ObjectBox->yPos;

			objectCheck->ObjectBox->xVelocity = step * i;

			SolidType savedType = inputBox->solid;
			inputBox->solid = UNSOLID;

			ClimbSlope(objectCheck->ObjectBox, compareBox, ObjectList);

			objectCheck->ObjectBox->xVelocity = savedXVel;

			inputBox->solid = savedType;

			if (CheckBoxCollidesBox(inputBox, objectCheck->ObjectBox) == 0)
			{
				objectCheck = NULL;
			}
			else
			{
				objectCheck->ObjectBox->xPos = savedXPos;
				objectCheck->ObjectBox->yPos = savedYPos;
			}
		}

		i--;
	}


	if (objectCheck != NULL)
	{
		// Shift back to movestep right before colliding with new object
		inputBox->xPos -= step;

		if (evaluateCollideMode(inputBox, objectCheck->ObjectBox) != PUSH)
		{
			ApplyXPhysics(inputBox, compareBox);
		}

		double slopeTop = (compareBox->xFlip == 1) ? (slope * (inputBox->xPos + inputBox->xSize - compareBox->xPos)) : (slope * (compareBox->xSize - inputBox->xPos + compareBox->xPos));

		slopeTop = dClamp(slopeTop, 0.0, (double)compareBox->ySize);

		if (compareBox->yFlip == -1)
		{
			slopeTop = compareBox->ySize - slopeTop;
		}

		inputBox->yPos = slopeTop + compareBox->yPos;
	}	

		*/		
	return LEMON_SUCCESS;
}
