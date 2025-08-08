#include "gameObjects.h"



Object* AddObject(World *GameWorld, int objectID, int xPos, int yPos, int xSize, int ySize, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return NULL;
	}

	int maxObjects = (GameWorld->GameState == LOADING) ? (MAX_OBJECTS - RESERVED_OBJECTS) : MAX_OBJECTS;

	if (GameWorld->ObjectList->objectCount + GameWorld->ObjectList->cachedCount >= maxObjects)
	{
		return NULL;
	}


	if (objectID >= UNDEFINED_OBJECT || objectID < LEVEL_FLAG_OBJ)
	{
		printf("This object is not defined! Type: %d\n", objectID);
		return NULL;
	}

	ObjectController *ObjectList = GameWorld->ObjectList;

	Object *newObject;
	newObject = createNewObject(ObjectList, xPos, yPos, objectID);

	if (newObject == NULL)
	{
		return NULL;
	}


	// Default settings
	newObject->ObjectDisplay->currentSprite = 1;
	newObject->ObjectBox->ySize = ySize;
	newObject->ObjectBox->xSize = xSize;
	newObject->arg1 = arg1;
	newObject->arg2 = arg2;
	newObject->arg3 = arg3;
	newObject->arg4 = arg4;
	newObject->arg5 = arg5;
	

	// Set Object parameters
	switch (objectID)
	{
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
			newObject->ObjectDisplay->transparencyEffect = 0.9;
			if (arg1 >= 0)
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
			newObject->ObjectBox->solid = UNSOLID;
			break;


		case VERTICAL_GATE:
		// Medium sized, vertical gate
			newObject->ObjectBox->solid = ENTITY_SOLID;
			newObject->arg1 = arg1;
			newObject->arg2 = 0;
			newObject->arg3 = yPos;
			newObject->arg4 = arg2;
			newObject->ObjectBox->ySize = ySize * Y_TILESCALE;
			newObject->ObjectBox->xSize = xSize * X_TILESCALE;
			break;

		case HORIZONTAL_GATE:
		// Medium sized, horizontal gate
			newObject->ObjectBox->solid = ENTITY_SOLID;
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
			InitialiseParticle(newObject, arg1, arg2, arg3, arg4);
			break;


		case LEVEL_FLAG_OBJ:
		// Like particles, arg1 defines the subtype of the level flag object
			InitialiseLevelFlag(newObject, ObjectList);
			break;


		case UI_TEXT:
			InitialiseUIText(GameWorld, newObject);
			break;


		case UI_ELEMENT:
			InitialiseUIElement(GameWorld, newObject);
			break;


		case BASIC_ENEMY:
			newObject->ObjectBox->solid = ENTITY;
			newObject->ObjectBox->forwardVelocity = 3.0;
			newObject->ObjectBox->xSize = 40;
			newObject->ObjectBox->ySize = 60;
			newObject->ObjectBox->friction = 1.0;
			break;


		case PUSHABLE_BOX:
			newObject->ObjectBox->solid = PUSHABLE_SOLID;
			break;


		default:
			break;
	}


	if (newObject->ObjectBox->xSize <= 0)
	{
		newObject->ObjectBox->xSize = X_TILESCALE;
	}
		
	if (newObject->ObjectBox->ySize <= 0)
	{
		newObject->ObjectBox->ySize = Y_TILESCALE;
	}

	newObject->ObjectBox->xPosRight = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;
	newObject->ObjectBox->yPosTop = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;

	if (GameWorld->GameState == LOADING)
	{
		printf("\nCreated object type: %d;\n\n", objectID);
	}

	return newObject;
}


int LoadSpriteSet(SpriteSet *newSet, int ObjectID)
{
	if (newSet == NULL)
	{
		return MISSING_DATA;
	}

	if (ObjectID <= LEVEL_FLAG_OBJ || ObjectID >= UNDEFINED_OBJECT)
	{
		return INVALID_DATA;
	}


	// Fill sprite set with sprites
	switch (ObjectID)
	{
		case PLAYER_OBJECT:
			loadAnimationsFromFile("PlayerObject", newSet);
			return EXECUTION_UNNECESSARY;

		case PARTICLE:
			loadAnimationsFromFile("Particles", newSet);
			return EXECUTION_UNNECESSARY;

		case UI_ELEMENT:
			loadAnimationsFromFile("UIElements", newSet);
			return EXECUTION_UNNECESSARY;

		case UI_TEXT:
			loadAnimationsFromFile("UIText", newSet);
			return EXECUTION_UNNECESSARY;

		case FLAT_SLOPE_FLOOR:
			loadAnimationsFromFile("FlatSlopeFloor", newSet);
			return EXECUTION_UNNECESSARY;

		case SPRING:
			loadAnimationsFromFile("Spring", newSet);
			return EXECUTION_UNNECESSARY;

		case SOLID_BLOCK:
			loadAnimationsFromFile("SolidBlock", newSet);
			return EXECUTION_UNNECESSARY;

		default:
		{
			loadSpriteIntoSpriteSet("Missing", NULL, newSet, TILE);
		} break;
	}


	return LEMON_SUCCESS;
}


int LoadParticleSprites(SpriteSet *newSet)
{
	// Sparkle 
	loadObjectSprite("Sparkle1", newSet, SINGLE);
	loadObjectSprite("Sparkle2", newSet, SINGLE);
	loadObjectSprite("Sparkle3", newSet, SINGLE);
	loadObjectSprite("Sparkle4", newSet, SINGLE);
	loadObjectSprite("Sparkle5", newSet, SINGLE);
	loadObjectSprite("Sparkle6", newSet, SINGLE);
	loadObjectSprite("Sparkle7", newSet, SINGLE);

	return 0;
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
		newObject->ParentXOffset = newObject->ObjectBox->xPos - ParentObject->ObjectBox->xPos;
		newObject->ParentYOffset = newObject->ObjectBox->yPos - ParentObject->ObjectBox->yPos;
	}

	return newObject;
}


Object* AddParticle(World *GameWorld, ParticleSubType animation, int xPos, int yPos, int repeatCount, int frameRate, int particleLifeTime)
{
	return AddObject(GameWorld, PARTICLE, xPos, yPos, 0, 0, animation, repeatCount, frameRate, particleLifeTime, 0);
}


Object* createNewObject(ObjectController *ObjectList, int xPos, int yPos, int objectID)
{
	Object *currentObject;
	currentObject = ObjectList->firstObject;


	Object *newObject = malloc(sizeof(Object));

	if (newObject == NULL)
	{
		printf("\nError: Could not allocate memory for new object.\n\n");
		return NULL;
	}


	newObject->ObjectBox = createPhysicsRect(SOLID);

	if (newObject->ObjectBox == NULL)
	{
		printf("\nError: Could not allocate memory for new object's physics box.\n\n");
		free(newObject);
		return NULL;
	}


	newObject->ObjectDisplay = createDisplayData(DEFAULT_TO_SPRITE);

	if (newObject->ObjectDisplay == NULL)
	{
		printf("\nError: Could not allocate memory for new object's display data.\n\n");
		free(newObject);
		return NULL;
	}


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
	newObject->ParentObject = NULL;
	newObject->ObjectID = objectID;
	newObject->State = DEFAULT;
	newObject->Action = IDLE;
	newObject->layer = MIDDLEGROUND;
	newObject->ParentLink = DEFAULT_LINK;

	newObject->ObjectBox->xSize = X_TILESCALE;
	newObject->ObjectBox->ySize = Y_TILESCALE;
	newObject->ObjectBox->xPosRight = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;
	newObject->ObjectBox->yPosTop = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;


	if (objectID == UI_ELEMENT || objectID == UI_TEXT){
		newObject->ObjectBox->xPos = xPos;
		newObject->ObjectBox->yPos = yPos;
	}
	else
	{
		newObject->ObjectBox->xPos = (double)abs(xPos - (xPos % X_TILESCALE));
		newObject->ObjectBox->yPos = (double)abs(yPos - (yPos % Y_TILESCALE));
	}

	newObject->ObjectDisplay->spriteSetSource = createObjectSpriteSet(ObjectList, objectID);

	return newObject;
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
	newRect->collideMode = IMPACT;
	newRect->crouch = 0;
	newRect->xVelocity = 0.0;
	newRect->yVelocity = 0.0;
	newRect->forwardVelocity = 0.0;
	newRect->friction = 0.7;
	newRect->collideLayer = MIDDLEGROUND;

	return newRect;
}


DisplayData* createDisplayData(RenderMode startRenderMode)
{
	DisplayData *newDisplay = malloc(sizeof(DisplayData));

	newDisplay->currentSprite = 1;
	newDisplay->spriteBuffer = NULL;
	newDisplay->spriteSetSource = NULL;
	newDisplay->RenderModeOverride = startRenderMode;

	newDisplay->spriteXOffset = 0;
	newDisplay->spriteYOffset = 0;
	newDisplay->pixelXOffset = 0;
	newDisplay->pixelYOffset = 0;

	newDisplay->currentAnimation = 0;
	newDisplay->frameBuffer = NULL;
	newDisplay->animationBuffer = NULL;
	newDisplay->animationTick = 0;
	newDisplay->animationLoopCount = 0;
	newDisplay->transparencyEffect = 0.0;
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
		printf("Failed to allocate memory for new sprite set.\n");
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

	if (LoadSpriteSet(newSet, ObjectID) == LEMON_SUCCESS)
	{
		LoadAnimations(newSet, ObjectID);
	}
	
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
		printf("Could not find sprite %d for object %d\n", spriteID, inputObject->ObjectID);
		return MISSING_DATA;
	}

	ObjectDisplay->spriteBuffer = currentSprite;
	ObjectDisplay->currentSprite = spriteID;
		
	return 0;
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

	return 0;
}


void deleteObject(Object **input, ObjectController *ObjectList)
{
	if (*input == NULL || ObjectList == NULL)
	{
		return;
	}

	Object *tempObject;
	tempObject = *input;

	Object *prevObject;
	prevObject = (*input)->prevObject;

	(*input) = NULL;


	if (tempObject->nextObject != NULL)
	{
		tempObject->nextObject->prevObject = prevObject;
	}
	else
	{
		ObjectList->lastObject = prevObject;
	}

	if (prevObject != NULL)
	{
		prevObject->nextObject = tempObject->nextObject;
	}
	else
	{
		ObjectList->firstObject = tempObject->nextObject;
	}

	Object *currentObject = ObjectList->lastObject;
	Object *toDelete;

	while (currentObject != NULL)
	{
		toDelete = currentObject;
		currentObject = currentObject->prevObject;

		if (toDelete->ParentObject == tempObject)
		{
			deleteObject(&toDelete, ObjectList);
		}
	}


	free(tempObject->ObjectDisplay);
	free(tempObject->ObjectBox);
	free(tempObject);

	return;
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
		deleteObject(&temp, ObjectList);
	}
	
	ObjectList->objectCount = 0;
	ObjectList->firstObject = NULL;
	ObjectList->lastObject = NULL;


	currentObject = ObjectList->cachedLastObject;

	while (currentObject != NULL)
	{
		temp = currentObject;
		currentObject = currentObject->prevObject;
		deleteObject(&temp, ObjectList);
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

	return 0;
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

	return 0;
}


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

	Object *nextPtr;
	nextPtr = input->nextObject;

	Object *prevPtr;
	prevPtr = input->prevObject;

	if (nextPtr == NULL)
	{
		return;
	}


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

	return 0;
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


	return 0;
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

		if (checkBoxOverlapsBox(boundingBox, temp->ObjectBox) == 0)
		{
			moveObjectToCachedList(ObjectList, temp);
		}
	}


	return 0;
}


// Updates all objects in GameWorld
FunctionResult updateObjects(World *GameWorld, int keyboard[256])
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

	int i = ObjectList->objectCount;
	Object *currentObject = ObjectList->firstObject;

	// Update Behaviour
	while(currentObject != NULL && i > 0)
	{
		i--;

		if (currentObject != GameWorld->Player->PlayerPtr)
		{
			ObjectBehaviour(GameWorld, currentObject, keyboard);
		}

		currentObject = currentObject->nextObject;
	}


	// Update object state - animations, parent-child links, deletion, etc.
	currentObject = ObjectList->lastObject;
	Object *toDelete;

	while (currentObject != NULL)
	{
		UpdateObjectDisplay(GameWorld, currentObject);

		UpdateParentChildLink(currentObject);

		toDelete = currentObject;
		currentObject = currentObject->prevObject;

		if (toDelete->State == TO_BE_DELETED || toDelete->ObjectBox == NULL || toDelete->ObjectDisplay == NULL)
		{
			deleteObject(&toDelete, ObjectList);
		}
	}


	return LEMON_SUCCESS;
}


int ObjectBehaviour(World *GameWorld, Object *inputObject, int keyboard[256])
{
	if (inputObject == NULL || inputObject->ObjectBox == NULL || inputObject->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GamePaused == 1 && inputObject->ObjectID != UI_ELEMENT || inputObject->State < DEFAULT)
	{
		return EXECUTION_UNNECESSARY;
	}

	if (inputObject->State == PAUSE_BEHAVIOUR || (GameWorld->GameState == CUTSCENE && inputObject->State != ACTOR) || inputObject->State == IN_INVENTORY)
	{
		return ACTION_DISABLED;
	}

	GameWorld->depthCounter = 0;
	

	switch (inputObject->ObjectID)
	{
		case UI_ELEMENT:
			UpdateUIElement(GameWorld, inputObject, keyboard);
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
			UpdateEntityPhysics(inputObject, GameWorld->Player, GameWorld);
			break;


		case PUSHABLE_BOX:
			UpdateEntityPhysics(inputObject, GameWorld->Player, GameWorld);
			break;


		default:
			break;
	}


	// Move object
	moveObjectX(inputObject, GameWorld);

	moveObjectY(inputObject, GameWorld, NULL);

	MoveForward(inputObject->ObjectBox, GameWorld);
		

	return 0;
}


int UpdateObjectDisplay(World *GameWorld, Object *inputObject)
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

	// Assign Sprite   (0 means do not change spriteset)
	switchSprite(inputObject->ObjectDisplay->currentSprite, 0, inputObject->ObjectDisplay);


	return LEMON_SUCCESS;
}


int UpdateParentChildLink(Object *inputObject)
{
	if (inputObject->ParentObject == NULL)
	{
		return EXECUTION_UNNECESSARY;
	}

	DisplayData *ParentDisplay = inputObject->ParentObject->ObjectDisplay;
	PhysicsRect *ParentBox = inputObject->ParentObject->ObjectBox;

	if (ParentDisplay == NULL || ParentBox == NULL)
	{
		return MISSING_DATA;
	}


	if ((inputObject->ParentLink & SPRITE_LINK) != 0)
	{
		inputObject->ObjectDisplay->spriteBuffer = ParentDisplay->spriteBuffer;
		inputObject->ObjectDisplay->currentSprite = ParentDisplay->spriteBuffer->spriteID;
	}


	if ((inputObject->ParentLink & ANIMATION_LINK) != 0)
	{
		inputObject->ObjectDisplay->animationBuffer = ParentDisplay->animationBuffer;
		inputObject->ObjectDisplay->frameBuffer = ParentDisplay->frameBuffer;
		inputObject->ObjectDisplay->currentAnimation = ParentDisplay->currentAnimation;
	}

	if ((inputObject->ParentLink & TRANSPARENCY_LINK) != 0)
	{
		inputObject->ObjectDisplay->transparencyEffect = ParentDisplay->transparencyEffect;
	}


	if ((inputObject->ParentLink & POSITION_LINK) != 0)
	{
		inputObject->ObjectBox->xPos = ParentBox->xPos + inputObject->ParentXOffset;
		inputObject->ObjectBox->xPosRight = inputObject->ObjectBox->xPos + inputObject->ObjectBox->xSize;

		inputObject->ObjectBox->yPos = ParentBox->yPos + inputObject->ParentYOffset;
		inputObject->ObjectBox->yPosTop = inputObject->ObjectBox->yPos + inputObject->ObjectBox->ySize;
	}

	if ((inputObject->ParentLink & VELOCITY_LINK) != 0)
	{
		inputObject->ObjectBox->xVelocity = ParentBox->xVelocity;
		inputObject->ObjectBox->yVelocity = ParentBox->yVelocity;
		inputObject->ObjectBox->direction = ParentBox->direction;
		inputObject->ObjectBox->forwardVelocity = ParentBox->forwardVelocity;
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


	if (inputData->invincibilityFrames % 20 != 0)
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

	if (checkBoxOverlapsBox(PlayerBox, coin->ObjectBox) == 1)
	{
		GameWorld->Player->coinCount++;
		AddParticle(GameWorld, SPARKLE, coin->ObjectBox->xPos, coin->ObjectBox->yPos, 1, 0, 0);
		MarkObjectForDeletion(coin);
		LemonPlaySound("Coin_Collect", "Objects", OBJECT_SFX, 0.75);
	}

	return 0;
}


int UpdateSpring(Object *spring, World *GameWorld)
{
	if (spring == NULL || GameWorld == NULL || GameWorld->Player == NULL || GameWorld->Player->PlayerBox == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsRect *PlayerBox = GameWorld->Player->PlayerBox;

	if (spring->arg5 < 1 && PlayerBox->yVelocity < -1.0 && checkBoxOverlapsBox(PlayerBox, spring->ObjectBox) == 1)
	{
		PlayerBox->yVelocity = (double)spring->arg1;
		spring->arg5 = 20;
		LemonPlaySound("Spring", "Objects", OBJECT_SFX, 1.0);
		PlayAnimation("Bounce", 1, spring->ObjectDisplay);
	}

	if (spring->arg5 > 0)
	{
		spring->arg5--;
	}

	return 0;
}


int InitialiseParticle(Object *particle, int animation, int repeatCount, int frameRate, int particleLifeTime)
{
	if (particle == NULL)
	{
		return MISSING_DATA;
	}

	if (frameRate < 1 || frameRate > 999)
	{
		frameRate = 30;
	}

	particle->layer = PARTICLES;
	particle->ObjectBox->solid = UNSOLID;
	particle->ObjectDisplay->currentAnimation = animation;
	particle->arg2 = 60 / frameRate;
	particle->arg3 = particleLifeTime;
	particle->arg4 = 0;
	particle->arg5 = 0;

	// Assign particle sprite
	particle->arg1 = 1;
	LoopParticleAnimation(particle);

	// Set repeat count after function call as it may decrement arg1
	particle->arg1 = repeatCount;


	switch (animation)
	{
		default:
		break;
	}

	return 0;
}


int UpdateParticle(World *GameWorld, Object *particle)
{
	// currentAnimation: which particle animation to play
	// arg1: number of times to repeat animation
	// arg2: How many ticks before a frame change
	// arg3: particle max lifetime	(0 to simply default to deleting as soon as repeat count has been reached)

	if (particle == NULL)
	{
		return MISSING_DATA;
	}


	// Animation
	particle->ObjectDisplay->animationTick++;

	if (particle->ObjectDisplay->animationTick > 9999 || particle->ObjectDisplay->animationTick < 0)
	{
		particle->ObjectDisplay->animationTick = 0;
	}

	LoopParticleAnimation(particle);


	CustomParticleBehaviour(GameWorld, particle);


	// If repeat count is reached or animationTick exceeds maximum lifetime, mark for deletion
	if (particle->arg3 < 1 && particle->arg1 < 1)
	{
		MarkObjectForDeletion(particle);
	}

	if (particle->arg3 > 0 && particle->ObjectDisplay->animationTick > particle->arg3)
	{
		MarkObjectForDeletion(particle);
	}

	return 0;
}


int CustomParticleBehaviour(World *GameWorld, Object *particle)
{
	// Custom behaviour
	switch(particle->ObjectDisplay->currentAnimation)
	{
		default:
		break;
	}

	return 0;
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

		return 0;
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

	return 0;
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
			SetObjectYPosition(gate, closedPosition, GameWorld->ObjectList);
			gate->ObjectBox->yVelocity = 0.0;

			if (gate->arg2 == 1)
			{
				gate->ObjectDisplay->currentAnimation = 1;
				gate->ObjectDisplay->animationTick = 0; 
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
				gate->ObjectDisplay->animationTick = 0;
				gate->ObjectBox->yVelocity = 0.0;
				SetObjectYPosition(gate, closedPosition + (gate->ObjectBox->ySize * (speed/abs(speed))), GameWorld->ObjectList);
			}

		} break;


		case 2:
		{
			SetObjectYPosition(gate, closedPosition + (gate->ObjectBox->ySize * (speed/abs(speed))), GameWorld->ObjectList);
			gate->ObjectBox->yVelocity = 0.0;

			if (gate->arg2 == 0)
			{
				gate->ObjectDisplay->currentAnimation = 3;
				gate->ObjectDisplay->animationTick = 0;
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
				gate->ObjectDisplay->animationTick = 0;
				gate->ObjectBox->yVelocity = 0.0;
				SetObjectYPosition(gate, closedPosition, GameWorld->ObjectList);
			}

		} break;
	}


	return 0;
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
			SetObjectXPosition(gate, closedPosition, GameWorld->ObjectList);
			gate->ObjectBox->xVelocity = 0.0;

			if (gate->arg2 == 1)
			{
				gate->ObjectDisplay->currentAnimation = 1;
				gate->ObjectDisplay->animationTick = 0;
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
				gate->ObjectDisplay->animationTick = 0;
				gate->ObjectBox->xVelocity = 0.0;
				SetObjectXPosition(gate, closedPosition + (gate->ObjectBox->xSize * (speed/abs(speed))), GameWorld->ObjectList);
			}

		} break;


		case 2:
		{
			SetObjectXPosition(gate, closedPosition + (gate->ObjectBox->xSize * (speed/abs(speed))), GameWorld->ObjectList);
			gate->ObjectBox->xVelocity = 0.0;

			if (gate->arg2 == 0)
			{
				gate->ObjectDisplay->currentAnimation = 3;
				gate->ObjectDisplay->animationTick = 0;
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
				gate->ObjectDisplay->animationTick = 0;
				gate->ObjectBox->xVelocity = 0.0;
				SetObjectXPosition(gate, closedPosition, GameWorld->ObjectList);
			}

		} break;
	}


	return 0;
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

	return 0;
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


	return 0;
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

	return 0;
}


int UpdateDoor(PlayerData *Player, Object *Door, World *GameWorld)
{
	// arg3: open/close state
	// arg1, arg2: [x/y]Pos in event of no linked door
	if (Player->PlayerBox == NULL || Door == NULL)
	{
		return MISSING_DATA;
	}

	if (Door->arg3 < 1 && PlayerInteractingWithBox(Player, Door->ObjectBox) == 1 && Player->PlayerPtr->State == DEFAULT)
	{
		SayText("It's a door.", NO_PORTRAIT, DEFAULT_BOTTOM, GameWorld);
		SayText("....Or is it?\n\rIt just looks like a big pink and black rectangle...", NO_PORTRAIT, DEFAULT_BOTTOM, GameWorld);

		LemonPlaySound("DoorOpen", "Objects", OBJECT_SFX, 1.0);
		ResetPlayer(Player);
		Player->PlayerPtr->State = PAUSE_BEHAVIOUR;

		Door->arg3 = 32;

		Player->PlayerBox->xPos = Door->arg1;
		Player->PlayerBox->yPos = Door->arg2;
		
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

	return 0;
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
		SayText("It's a door.\f.\f.\r \nIt eminates a strange glow.", NO_PORTRAIT, DEFAULT_BOTTOM, GameWorld);
		char phrase[] = "This Door will send you to level 0!";
		phrase[33] += Door->arg1;
		SayText(phrase, NO_PORTRAIT, DEFAULT_BOTTOM, GameWorld);
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

	return 0;
} 


int UpdateEntityPhysics(Object *entity, PlayerData *Player, World *GameWorld)
{
	if (entity == NULL || entity->ObjectBox == NULL || GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->PhysicsType == PLATFORMER)
	{
		entity->ObjectBox->yVelocity += GameWorld->Gravity;
	}

	if (entity->ObjectBox->yVelocity < -32.0)
	{
		entity->ObjectBox->yVelocity = -32.0;
	}

	return 0;
}


int moveObjectX(Object *inputObject, World *GameWorld)
{
	if (inputObject == NULL || inputObject->State < DEFAULT || GameWorld == NULL)	{ return  MISSING_DATA; }

	inputObject->ObjectBox->prevXPos = inputObject->ObjectBox->xPos;
	inputObject->ObjectBox->prevYPos = inputObject->ObjectBox->yPos;


	if (fabs(inputObject->ObjectBox->xVelocity) < 0.1)
	{
		inputObject->ObjectBox->xVelocity = 0.0;
		return EXECUTION_UNNECESSARY;
	}


	inputObject->ObjectBox->xPos += (inputObject->ObjectBox->xVelocity);
	inputObject->ObjectBox->xPosRight = inputObject->ObjectBox->xPos + inputObject->ObjectBox->xSize;


	if (inputObject->ObjectBox->solid == UNSOLID)
	{
		return ACTION_DISABLED;
	}

	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}


	ResolveAllXCollision(inputObject->ObjectBox, GameWorld->ObjectList);
	

	if (GameWorld->PhysicsType == PLATFORMER)
	{
		magnetiseObjectsX(inputObject->ObjectBox, GameWorld->ObjectList);
	}


	inputObject->ObjectBox->xPosRight = inputObject->ObjectBox->xPos + inputObject->ObjectBox->xSize;


	return 0;
}



int moveObjectY(Object *inputObject, World *GameWorld, int *jumpProgressPtr)
{
	if (inputObject == NULL || inputObject->State < DEFAULT || GameWorld == NULL)	{ return  MISSING_DATA; }

	inputObject->ObjectBox->prevXPos = inputObject->ObjectBox->xPos;
	inputObject->ObjectBox->prevYPos = inputObject->ObjectBox->yPos;


	if (fabs(inputObject->ObjectBox->yVelocity) < 0.1)
	{
		inputObject->ObjectBox->yVelocity = 0.0;
		return EXECUTION_UNNECESSARY;
	}

	inputObject->ObjectBox->yPos += (inputObject->ObjectBox->yVelocity);
	inputObject->ObjectBox->yPosTop = inputObject->ObjectBox->yPos + inputObject->ObjectBox->ySize;


	if (inputObject->ObjectBox->solid == UNSOLID)
	{
		return ACTION_DISABLED;
	}

	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}


	ResolveAllYCollision(inputObject->ObjectBox, GameWorld->ObjectList, jumpProgressPtr);


	if (GameWorld->PhysicsType == PLATFORMER)
	{
		magnetiseObjectsY(inputObject->ObjectBox, GameWorld->ObjectList);
	}


	inputObject->ObjectBox->yPosTop = inputObject->ObjectBox->yPos + inputObject->ObjectBox->ySize;

	return 0;
}


int SetObjectXPosition(Object *inputObject, double newXPos, ObjectController *ObjectList)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	if (fabs(inputObject->ObjectBox->xPos - newXPos) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}


	double prevObjXPos = inputObject->ObjectBox->xPos;

	inputObject->ObjectBox->xPos = newXPos;
	inputObject->ObjectBox->xPosRight = newXPos = inputObject->ObjectBox->xSize;

	if (ObjectList == NULL || inputObject->ObjectBox->solid == 0)
	{
		return MISSING_DATA;
	}

	ResolveAllXCollisionsByPush(inputObject->ObjectBox, ObjectList);

	return 0;
}


//Method for setting an object's position to avoid incorrect collision with player
int SetObjectYPosition(Object *inputObject, double newYPos, ObjectController *ObjectList)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	if (fabs(inputObject->ObjectBox->yPos - newYPos) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}


	double prevObjYPos = inputObject->ObjectBox->yPos;
	double prevObjYPosTop = inputObject->ObjectBox->yPos + inputObject->ObjectBox->ySize;

	inputObject->ObjectBox->yPos = newYPos;
	inputObject->ObjectBox->yPosTop = newYPos + inputObject->ObjectBox->ySize;

	if (ObjectList == NULL || inputObject->ObjectBox->solid == 0)
	{
		return MISSING_DATA;
	}

	ResolveAllYCollisionsByPush(inputObject->ObjectBox, ObjectList);


	return 0;
}


// Method for centering object size increase and handling player collisions
int ChangeObjectXSizeBy(int change, Object *inputObject, ObjectController *ObjectList)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	double changeHalf = change / 2.0;

	inputObject->ObjectBox->xSize += change;
	inputObject->ObjectBox->xPos -= changeHalf;
	inputObject->ObjectBox->xPosRight += changeHalf;

	if (ObjectList == NULL || change == 0 || inputObject->ObjectBox->solid < 1)
	{
		return EXECUTION_UNNECESSARY;
	}

	ResolveAllXCollisionsByPush(inputObject->ObjectBox, ObjectList);
	
	return 0;
}


int ChangeObjectYSizeBy(int change, Object *inputObject, ObjectController *ObjectList)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	double changeHalf = change / 2.0;

	inputObject->ObjectBox->ySize += change;
	inputObject->ObjectBox->yPos -= changeHalf;
	inputObject->ObjectBox->yPosTop += changeHalf;

	if (ObjectList == NULL || change == 0 || inputObject->ObjectBox->solid < 1)
	{
		return EXECUTION_UNNECESSARY;
	}


	ResolveAllYCollisionsByPush(inputObject->ObjectBox, ObjectList);
	
	return 0;
}


int checkBoxOverlapsBox(PhysicsRect *inputBox, PhysicsRect *compareBox)
{
	if (inputBox == compareBox || inputBox->xSize == 0 || inputBox->ySize == 0 || compareBox->xSize == 0 || compareBox->ySize == 0)
	{
		return 0;
	}

	return !((int)inputBox->xPos >= (int)(compareBox->xPos + compareBox->xSize) || (int)(inputBox->xPos + inputBox->xSize) <= (int)compareBox->xPos || (int)inputBox->yPos >= (int)(compareBox->yPos + compareBox->ySize) || (int)(inputBox->yPos + inputBox->ySize) <= (int)compareBox->yPos);
}


int checkBoxOverlapsBoxBottom(PhysicsRect *inputBox, PhysicsRect *compareBox)
{
	if (inputBox == compareBox || inputBox->xSize == 0 || inputBox->ySize == 0 || compareBox->xSize == 0 || compareBox->ySize == 0)
	{
		return 0;
	}

	return !((int)inputBox->xPos >= (int)(compareBox->xPos + compareBox->xSize) || (int)(inputBox->xPos + inputBox->xSize) <= (int)compareBox->xPos || (inputBox->yPos - 2) >= (int)(compareBox->yPos + compareBox->ySize) || (inputBox->yPos + 8) <= (int)compareBox->yPos);
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
		if (currentObject->ObjectBox->solid == UNSOLID || checkBoxOverlapsBox(inputBox, currentObject->ObjectBox) == 0)
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


// Compares two physics rects and Returns a 1 if overlapping a solid object, 0 if not
int CheckBoxCollidesBox(PhysicsRect *inputBox, PhysicsRect *compareBox)
{
	if (inputBox == NULL || compareBox == NULL)
	{
		return MISSING_DATA;
	}

	if (compareBox->collideLayer != inputBox->collideLayer || inputBox == compareBox || inputBox->xSize == 0 || inputBox->ySize == 0 || compareBox->xSize == 0 || compareBox->ySize == 0)
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
			

			inputYTop += inputBox->yPos;
		} break;
			
		case JUMP_THROUGH:
		{
			if (compareBox->yVelocity > 0.0 || compareBox->crouch == 1 || compareBox->prevYPos < inputYTop - 2)
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

			if (compareYTop > compareBox->ySize)
			{
				compareYTop = compareBox->ySize;
			}

			compareYTop += compareBox->yPos;
		} break;

		case JUMP_THROUGH:
		{
			if (inputBox->yVelocity > 0.0 || inputBox->crouch == 1 || inputBox->prevYPos < compareYTop - 2)
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

		case UNSOLID:
		return 0;
			
		default:
		break;
	}


	return !(inputY >= compareYTop || inputYTop <= compareY || inputX >= compareXRight || inputXRight <= compareX);
}


int AssignDirection(PhysicsRect *inputBox, PhysicsRect *compareBox)
{
	if (inputBox == NULL)
	{
		return MISSING_DATA;
	}

	if (compareBox == NULL)
	{
		inputBox->direction = RADIAN_90;
		return 0;
	}

	
	switch (compareBox->solid)
	{
			case FLAT_SLOPE:
			{
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

				if (slopeFloor < 0 || slopeFloor > compareBox->ySize)
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

	return 0;
}


int OverlapsObjectType(ObjectController *ObjectList, int overlapObjectID, Object *inputObject)
{
	if (ObjectList == NULL || ObjectList->firstObject == NULL)
	{
		return -1;
	}

	Object *currentObject;
	currentObject = ObjectList->firstObject;

	int i = ObjectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		if (currentObject->ObjectBox->collideLayer == inputObject->ObjectBox->collideLayer && currentObject->ObjectID == overlapObjectID && currentObject != inputObject)
		{
			if (CheckBoxCollidesBox(inputObject->ObjectBox, currentObject->ObjectBox) == 1)
			{
				return 1;
			}
		}

		currentObject = currentObject->nextObject;

		i++;
	}

	return 0;
}


int OverlapsObjectSolid(ObjectController *ObjectList, int solidID, Object *inputObject)
{
	if (ObjectList == NULL || ObjectList->firstObject == NULL)
	{
		return -1;
	}

	Object *currentObject;
	currentObject = ObjectList->firstObject;

	int i = ObjectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		if (currentObject->ObjectBox->collideLayer == inputObject->ObjectBox->collideLayer && currentObject->ObjectBox->solid == solidID && currentObject != inputObject)
		{
			if (CheckBoxCollidesBox(inputObject->ObjectBox, currentObject->ObjectBox) == 1)
			{
				return 1;
			}
		}

		currentObject = currentObject->nextObject;

		i++;
	}

	return 0;
}


int OverlapsObjectAllSolids(ObjectController *ObjectList, Object *inputObject)
{
	if (ObjectList == NULL || ObjectList->firstObject == NULL)
	{
		return -1;
	}

	Object *currentObject;
	currentObject = ObjectList->firstObject;

	int i = ObjectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		if (currentObject->ObjectBox->collideLayer == inputObject->ObjectBox->collideLayer && currentObject->ObjectBox->solid > 0 && currentObject != inputObject)
		{
			if (CheckBoxCollidesBox(inputObject->ObjectBox, currentObject->ObjectBox) == 1)
			{
				return 1;
			}
		}

		currentObject = currentObject->nextObject;

		i++;
	}

	return 0;
}


CollideType evaluateCollideMode(PhysicsRect *movingBox, PhysicsRect *collideBox)
{
	if (movingBox == NULL || collideBox == NULL)
	{
		return NO_COLLIDE_TYPE;
	}


	if (movingBox->collideMode == PUSH || (collideBox->solid == PUSHABLE_SOLID && (movingBox->solid == ENTITY || movingBox->solid == PUSHABLE_SOLID)) )
	{
		return PUSH;
	}

	return IMPACT;
}


int AdjustDirection(World *GameWorld, PhysicsRect *movingBox)
{	
	if (GameWorld == NULL || GameWorld->ObjectList == NULL || movingBox == NULL)
	{
		return MISSING_DATA;
	}

	Object *detectedObject;
	detectedObject = GameWorld->ObjectList->firstObject;

	int i = 0;
	int result = 0;


	movingBox->yPos += GameWorld->Gravity;

	detectedObject = GetCollidingObject(movingBox, GameWorld->ObjectList);

	movingBox->yPos -= GameWorld->Gravity;


	if (detectedObject == NULL)
	{
		movingBox->direction = RADIAN_90;

		return 0;
	}

	AssignDirection(movingBox, detectedObject->ObjectBox);


	return 0;
}


int MoveForward(PhysicsRect *movingBox, World *GameWorld)
{
	if (movingBox == NULL || GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}
	
	movingBox->prevXPos = movingBox->xPos;
	movingBox->prevYPos = movingBox->yPos;

	if (fabs(movingBox->forwardVelocity) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}


	if (GameWorld->depthCounter < 1 && movingBox->solid != UNSOLID)
	{
		AdjustDirection(GameWorld, movingBox);
	}


	// step and count set-up
	double orientation = (movingBox->forwardVelocity)/fabs(movingBox->forwardVelocity);
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

	// Unsolid objects do not have to do collision detection so it skips the rest of the function by moving all steps instantly
	if (movingBox->solid == UNSOLID)
	{
		movingBox->xPos += movingBox->forwardVelocity * sinVal;
		movingBox->yPos += movingBox->forwardVelocity * cosVal;
		movingBox->forwardVelocity *= movingBox->friction;
		return LEMON_SUCCESS;
	}

	double xStep = orientation * sinVal;
	double yStep = orientation * cosVal;
	int travelCount = 1 + (int)fabs(movingBox->forwardVelocity);

	int collideCycle = COLLISION_CYCLES;
	double tempX = movingBox->xPos;
	double tempY = movingBox->yPos;

	Object *currentObject = NULL;

	// regular collision
	while (travelCount > 0)
	{
		movingBox->yPos += yStep;
		movingBox->xPos += xStep;

		currentObject = GetCollidingObject(movingBox, GameWorld->ObjectList);

		collideCycle = COLLISION_CYCLES;
		tempX = movingBox->xPos;
		tempY = movingBox->yPos;

		while (collideCycle > 0 && currentObject != NULL)
		{
			collideCycle--;

			// using (travelcount - 1) result in pixel perfect collision but erroneous extra collisions when on slope
			// using (travelcount) results in more smooth slope interactions but creates a 1 pixel gap
			if (evaluateCollideMode(movingBox, currentObject->ObjectBox) == PUSH && GameWorld->depthCounter < COLLISION_DEPTH)
			{
				GameWorld->depthCounter++;

				double tempVelocity = currentObject->ObjectBox->forwardVelocity;
				double tempDirection = currentObject->ObjectBox->direction;
				SolidType prevSolidType = movingBox->solid;
				movingBox->solid = UNSOLID;

				currentObject->ObjectBox->forwardVelocity = orientation * travelCount;
				currentObject->ObjectBox->direction = movingBox->direction;

				int result = MoveForward(currentObject->ObjectBox, GameWorld);

				currentObject->ObjectBox->forwardVelocity = tempVelocity;
				currentObject->ObjectBox->direction = tempDirection;
				movingBox->solid = prevSolidType;

				if (result != LEMON_SUCCESS || CheckBoxCollidesBox(movingBox, currentObject->ObjectBox) == 1)
				{
					movingBox->yPos = tempY;
					movingBox->xPos = tempX;

					movingBox->yPos -= yStep;
					movingBox->xPos -= xStep;
					ApplyForwardPhysics(movingBox, currentObject->ObjectBox);

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
					movingBox->yPos = tempY;
					movingBox->xPos = tempX;

					movingBox->yPos -= yStep;
					movingBox->xPos -= xStep;
					ApplyForwardPhysics(movingBox, currentObject->ObjectBox);

					return TASK_FAILED;
				}
				
			}

			currentObject = GetCollidingObject(movingBox, GameWorld->ObjectList);
		}

		travelCount--;
	}

	movingBox->forwardVelocity *= movingBox->friction;


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

	if (fabs(sinVal) > 0.001)
	{
		if ((forwardX > 0.1 && physicsBox->xVelocity > 0.1) || (forwardX < -0.1 && physicsBox->xVelocity < -0.1))
		{
			inputBox->forwardVelocity = physicsBox->xVelocity * sinVal;
		}
		else
		{
			inputBox->forwardVelocity = 0.0;
		}
	}


	if (fabs(cosVal) > 0.001)
	{
		if ((forwardY > 0.1 && physicsBox->yVelocity > 0.1) || (forwardY < -0.1 && physicsBox->yVelocity < -0.1))
		{
			inputBox->forwardVelocity = physicsBox->yVelocity * cosVal;
		}
		else
		{
			inputBox->forwardVelocity = 0.0;
		}
	}


	return 0;
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

	
	int objX = compareBox->xPos;
	int objY = compareBox->yPos;
	int objXRight = objX + compareBox->xSize;
	int objYTop = objY + compareBox->ySize;
	int ObjXCenter = (objX + objXRight) >> 1;


	switch(compareBox->solid)
	{
		// Y = X * (ySize/xSize)
		case FLAT_SLOPE:
		{
			if (compareBox->xFlip == 1)
			{
				if ((int)movingBox->prevXPos >= objXRight)
				{
					movingBox->xPos = objXRight;
					ApplyXPhysics(movingBox, compareBox);
					return 0;
				}
			}
			else
			{
				if ((int)movingBox->prevXPos + movingBox->xSize <= objX)
				{
					movingBox->xPos = objX - movingBox->xSize;
					ApplyXPhysics(movingBox, compareBox);
					return 0;
				}
			}

			double slope = ((double)compareBox->ySize/(double)compareBox->xSize);
			double slopeFloor;

			if (compareBox->xFlip == 1)
			{
				slopeFloor = (movingBox->xPos + movingBox->xSize - objX) * slope;
			}
			else
			{
				slopeFloor = (compareBox->xSize - movingBox->xPos + objX) * slope;
			}

			slopeFloor = clamp(slopeFloor, 0, compareBox->ySize);

			movingBox->yPos = slopeFloor + objY; 


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
			if (movingBox->prevXPos < ObjXCenter)
			{
				movingBox->xPos = objX - movingBox->xSize;
			}
			else
			{
				movingBox->xPos = objXRight;
			}
			
			ApplyXPhysics(movingBox, compareBox);
		
		} break;
	}


	return 0;
}


int magnetiseObjectsX(PhysicsRect *movingBox, ObjectController *ObjectList)
{
	if (movingBox == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	movingBox->xPos -= (movingBox->xVelocity);
	int pixelDifference = (int)movingBox->xPos - (int)(movingBox->xPos - (movingBox->xVelocity));

	movingBox->yPos += 2.0;

	Object *currentObject = ObjectList->firstObject;


	while (currentObject != NULL)
	{
		int result = checkBoxOverlapsBox(movingBox, currentObject->ObjectBox);

		if (result == 0)
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		result = CheckBoxCollidesBox(movingBox, currentObject->ObjectBox);

		if (result == 1)
		{
			currentObject->ObjectBox->xPos += pixelDifference;
		}

		currentObject = currentObject->nextObject;
	}
	

	movingBox->xPos += (movingBox->xVelocity);
	movingBox->yPos -= 2.0;


	return 0;
}


int ApplyXPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox)
{
	if (inputBox == NULL || physicsBox == NULL)
	{
		return MISSING_DATA;
	}

	if ((inputBox->xVelocity > 0.0 && physicsBox->xVelocity > 0.1) || (inputBox->xVelocity < 0.0 && physicsBox->xVelocity < -0.1))
	{
		inputBox->xVelocity = physicsBox->xVelocity;
	}
	else
	{
		inputBox->xVelocity = 0.0;
		inputBox->PhysicsXVelocity = 0.0;
	}


	return 0;
}


int ResolveAllYCollision(PhysicsRect *movingBox, ObjectController *ObjectList, int *JumpProgressPtr)
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

			ResolveAllYCollision(currentObject->ObjectBox, ObjectList, NULL);

			currentObject->ObjectBox->yVelocity = prevYVel;
			movingBox->solid = prevType;

			if (CheckBoxCollidesBox(movingBox, currentObject->ObjectBox) == 1)
			{
				ResolveYCollision(movingBox, currentObject->ObjectBox, JumpProgressPtr);
			}
		}
		else
		{
			ResolveYCollision(movingBox, currentObject->ObjectBox, JumpProgressPtr);
		}

		currentObject = GetCollidingObject(movingBox, ObjectList);

		count++;
	}


	return LEMON_SUCCESS;
}


int ResolveYCollision(PhysicsRect *movingBox, PhysicsRect *compareBox, int *jumpProgressPtr)
{
	if (movingBox == NULL || compareBox == NULL || movingBox == compareBox)
	{
		return MISSING_DATA;
	}

	int jumpProgressBuffer = 0;

	if (jumpProgressPtr != NULL)
	{
		jumpProgressBuffer = (*jumpProgressPtr);
	}


	double prevYPos = movingBox->yPos - movingBox->yVelocity - movingBox->PhysicsYVelocity;
	double objX = compareBox->xPos;
	double objY = compareBox->yPos;
	double objXRight = objX + compareBox->xSize;
	double objYTop = objY + compareBox->ySize;
	double ObjYCenter = ((objY + objYTop) / 2.0);

	switch(compareBox->solid)
	{
		case FLAT_SLOPE:
		{
			if ((int)prevYPos + movingBox->ySize < objY)
			{
				movingBox->yPos = (objY - movingBox->ySize);
				jumpProgressBuffer = 100;
				break;
			}
		

			// If movingBox is halfway off edge, floor of slope continues to be calculated as Y = X * slope
			// So here it is reset to the expected maximum if it over
			double slope = ((double)compareBox->ySize/(double)compareBox->xSize);
			double slopeFloor;

			if (compareBox->xFlip == 1)
			{
				slopeFloor = (movingBox->xPos + movingBox->xSize - objX) * slope;
			}
			else
			{
				slopeFloor = (compareBox->xSize - movingBox->xPos + objX) * slope;
			}

			slopeFloor = clamp(slopeFloor, 0, compareBox->ySize);

			movingBox->yPos = slopeFloor + objY; 

			jumpProgressBuffer = 0;
					
		} break;


		case JUMP_THROUGH:
		{
			if (movingBox->yVelocity < 0.1 && movingBox->crouch < 1)
			{
				movingBox->yPos = objYTop;
				jumpProgressBuffer = 0;
			}

		} break;


		case UNSOLID:
			break;


		default:
		{
			if (prevYPos < ObjYCenter)
			{
				movingBox->yPos = objY - movingBox->ySize;
				jumpProgressBuffer = 100;
			}
			else
			{
				movingBox->yPos = objYTop;
				jumpProgressBuffer = 0;
			}

		} break;

	}


	ApplyYPhysics(movingBox, compareBox);


	if (jumpProgressPtr != NULL)
	{
		(*jumpProgressPtr) = jumpProgressBuffer;
	}


	return 0;
}



int magnetiseObjectsY(PhysicsRect *movingBox, ObjectController *ObjectList)
{
	if (movingBox == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	movingBox->yPos -= (movingBox->yVelocity);
	int pixelDifference = (int)movingBox->yPos - (int)(movingBox->yPos - (movingBox->yVelocity));

	movingBox->yPos += 2.0;

	Object *currentObject = ObjectList->firstObject;


	while (currentObject != NULL)
	{
		int result = checkBoxOverlapsBox(movingBox, currentObject->ObjectBox);

		if (result == 0 || currentObject->ObjectBox->yVelocity < 0.1)
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		result = CheckBoxCollidesBox(movingBox, currentObject->ObjectBox);

		if (result == 1)
		{
			currentObject->ObjectBox->yPos += pixelDifference;
		}

		currentObject = currentObject->nextObject;
	}
	

	movingBox->yPos += (movingBox->yVelocity);
	movingBox->yPos -= 2.0;


	return 0;
}


int ApplyYPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox)
{
	if (inputBox == NULL || physicsBox == NULL)
	{
		return MISSING_DATA;
	}

	if (inputBox->yVelocity > 0.0)
	{
		if (physicsBox->yVelocity > 0.0)
		{
			inputBox->yVelocity = physicsBox->yVelocity;
		}
		else
		{
			inputBox->PhysicsYVelocity = 0.0;
			inputBox->yVelocity = 0.0;
		}

	}
	else if (inputBox->yVelocity < 0.0)
	{
		if (physicsBox->yVelocity < 0.0)
		{
			inputBox->yVelocity = physicsBox->yVelocity;
		}
		else
		{			
			inputBox->PhysicsYVelocity = 0.0;
			inputBox->yVelocity = 0.0;
		}
	}

	return 0;
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

	double prevXPos = movingBox->xPos - movingBox->xVelocity;
	double prevXPosRight = prevXPos + movingBox->xSize;
	double prevXPosCenter = (prevXPos + prevXPosRight) / 2.0;


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
				prevXPosCenter = prevXPos;
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

	return 0;
}


int ResolveAllXCollisionsByPush(PhysicsRect *movingBox, ObjectController *ObjectList)
{
	if (movingBox == NULL || ObjectList == NULL)
	{
		return 0;
	}


	Object *collideObject = GetCollidingObject(movingBox, ObjectList);
	int i = 0;

	while (collideObject != NULL && i < 16)
	{
		ResolveXCollisionByPush(movingBox, collideObject->ObjectBox);

		collideObject = GetCollidingObject(movingBox, ObjectList);	
		i++;
	}


	return 0;
}


int ResolveAllYCollisionsByPush(PhysicsRect *movingBox, ObjectController *ObjectList)
{
	if (movingBox == NULL || ObjectList == NULL)
	{
		return 0;
	}


	Object *collideObject = GetCollidingObject(movingBox, ObjectList);
	int i = 0;

	while (collideObject != NULL && i < 16)
	{
		ResolveYCollisionByPush(movingBox, collideObject->ObjectBox);

		collideObject = GetCollidingObject(movingBox, ObjectList);	
		i++;
	}


	return 0;
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

	double prevYPos = movingBox->yPos - movingBox->yVelocity;
	double prevYPosTop = prevYPos + movingBox->ySize;


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
			
			prevYPosTop = prevYPos;
			break;


		default:
			break;
	}

	
	if (compareBox->yPos < ((prevYPos + prevYPosTop) / 2.0) )
	{
		compareBox->yPos = ObjYPos - compareBox->ySize;
	}
	else
	{
		compareBox->yPos = ObjYPosTop; 
	}


	return 0;
}


int ClimbSlope(PhysicsRect *inputBox, PhysicsRect *compareBox, ObjectController *ObjectList)
{		
	if (inputBox == NULL || compareBox == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	inputBox->xPos = inputBox->prevXPos;

	double velocity = inputBox->xVelocity + inputBox->PhysicsXVelocity;

	if (fabs(velocity) < 0.1)
	{
		return 0;
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
		i--;

		double slopeTop = (compareBox->xFlip == 1) ? (slope * (inputBox->xPos + inputBox->xSize - compareBox->xPos)) : (slope * (compareBox->xSize - inputBox->xPos + compareBox->xPos));

		slopeTop = dClamp(slopeTop, 0.0, (double)compareBox->ySize) + compareBox->yPos;

		inputBox->yPos = slopeTop;


		objectCheck = GetCollidingObject(inputBox, ObjectList);
	}


	if (objectCheck != NULL)
	{
		// Shift back to movestep right before colliding with new object
		inputBox->xPos -= step;

		ApplyXPhysics(inputBox, compareBox);

		double slopeTop = (compareBox->xFlip == 1) ? (slope * (inputBox->xPos + inputBox->xSize - compareBox->xPos)) : (slope * (compareBox->xSize - inputBox->xPos + compareBox->xPos));

		slopeTop = dClamp(slopeTop, 0.0, (double)compareBox->ySize) + compareBox->yPos;

		inputBox->yPos = slopeTop;
	}	

				
	return LEMON_SUCCESS;
}
