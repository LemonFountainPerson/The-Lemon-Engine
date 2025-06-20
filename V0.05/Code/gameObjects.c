#include "gameObjects.h"



Object* AddObject(World *gameWorld, int objectID, int xPos, int yPos, int xSize, int ySize, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	if (gameWorld == NULL || gameWorld->ObjectList == NULL)
	{
		return NULL;
	}

	if (gameWorld->ObjectList->objectCount >= MAX_OBJECTS)
	{
		return NULL;
	}

	ObjectController *ObjectList = gameWorld->ObjectList;


	if (objectID >= UNDEFINED_OBJECT || objectID < LEVEL_FLAG_OBJ)
	{
		printf("This object is not defined! Type: %d\n", objectID);
		return NULL;
	}


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
	newObject->ObjectBox->solid = SOLID;
	newObject->arg1 = arg1;
	newObject->arg2 = arg2;
	newObject->arg3 = arg3;
	newObject->arg4 = arg4;
	newObject->arg5 = arg5;
	

	// Set Object parameters
	switch (objectID)
	{
		case DOOR:
			newObject->ObjectBox->solid = JUMP_THROUGH;
			newObject->ObjectBox->xSize = X_TILESCALE << 1;
			newObject->ObjectBox->ySize = Y_TILESCALE * 3;

			if (newObject->arg4 == 0)
			{
				newObject->arg4 = 1;
				newObject->ParentObject = AddObject(gameWorld, DOOR, arg1, arg2, xSize, ySize, xPos, yPos, 0, 1, 0);

				newObject->ParentObject->ParentObject = newObject;
			}
			
		break;

		case SOLID_BLOCK:
			newObject->ObjectBox->xSize = xSize * X_TILESCALE;
			newObject->ObjectBox->ySize = ySize * Y_TILESCALE;
			if (arg1 > 0)
			{
				switchObjectSprite(arg1, newObject, ObjectList);
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


		case JUMP_THRU:
			newObject->ObjectBox->solid = JUMP_THROUGH;
			newObject->ObjectBox->xSize = xSize * X_TILESCALE;
			break;

		case COIN:
		// Coin
			newObject->arg1 = arg1;
			newObject->arg2 = arg2;
			newObject->arg3 = 1;
			newObject->ObjectBox->solid = 0;
			break;

		
		case SPRING:
		// spring
			newObject->arg1 = arg1;
			newObject->ObjectBox->solid = 0;
			break;


		case VERTICAL_GATE:
		// Medium sized, vertical gate
			newObject->ObjectBox->solid = SOLID;
			newObject->arg1 = arg1;
			newObject->arg2 = 0;
			newObject->arg3 = yPos;
			newObject->arg4 = arg2;
			newObject->ObjectBox->ySize = ySize * Y_TILESCALE;
			newObject->ObjectBox->xSize = xSize * X_TILESCALE;
			break;

		case HORIZONTAL_GATE:
		// Medium sized, horizontal gate
			newObject->arg1 = arg1;
			newObject->arg2 = 0;
			newObject->arg3 = yPos;
			newObject->arg4 = arg2;
			newObject->ObjectBox->ySize = ySize * Y_TILESCALE;
			newObject->ObjectBox->xSize = xSize * X_TILESCALE;
			break;


		case GATE_SWITCH:
		// switch for gate - arg1 is ID to match switch to gate, arg2 denotes type of switch (0 = or switch, 1 = and switch)
			newObject->arg1 = arg1;
			newObject->arg2 = arg2;
			newObject->ObjectBox->solid = 0;
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
			newObject->ObjectBox->solid = 0;
			newObject->animationTick = abs(arg2);
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
			if (xSize > 0)
			{
				newObject->ObjectBox->xSize = xSize;
			}
		
			if (ySize > 0)
			{
				newObject->ObjectBox->ySize = ySize;
			}

			DefineLevelFlag(newObject, ObjectList);
			break;


		case UI_ELEMENT:
			InitialiseUIElement(gameWorld, newObject);
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

	if (gameWorld->GameState == LOADING)
	{
		printf("\nCreated object type: %d;\n\n", objectID);
	}

	return newObject;
}


Object* AddObjectWithParent(World *gameWorld, Object *ParentObject, int objectID, int xPos, int yPos, int xSize, int ySize, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	Object *newObject = AddObject(gameWorld, objectID, xPos, yPos, xSize, ySize, arg1, arg2, arg3, arg4, arg5);

	if (newObject != NULL && ParentObject != NULL)
	{
		newObject->ParentObject = ParentObject;
	}

	return newObject;
}


int LoadSpriteSet(SpriteSet *newSet, int ObjectID)
{
	if (newSet == NULL)
	{
		return MISSING_DATA;
	}

	if (ObjectID < 1 || ObjectID >= UNDEFINED_OBJECT)
	{
		return INVALID_DATA;
	}


	// Fill sprite set with sprites
	switch (ObjectID)
	{
		case PARTICLE:
		{
			LoadParticleSprites(newSet);
		} break;

		case UI_ELEMENT:
		{
			LoadUISprites(newSet);
		} break;

		case FLAT_SLOPE_FLOOR:
		{
			loadObjectSprite("Grass_Angle_Small", newSet, SCALE);
			loadObjectSprite("Grass_Angle_Medium", newSet, SCALE);
			loadObjectSprite("Grass_Angle_Large", newSet, SCALE);
		} break;

		case SPRING:
			loadObjectSprite("Spring", newSet, SINGLE);
			loadObjectSprite("alphatest", newSet, SINGLE_FULL_ALPHA);
			break;

		case SOLID_BLOCK:
			loadObjectSprite("Grass_Block", newSet, TILE_FAST);
			loadObjectSprite("Dirt_Block", newSet, TILE_FAST);
			loadObjectSprite("Bottom_Block", newSet, TILE_FAST);
			loadObjectSprite("CornerEdge_Left_Block", newSet, TILE_FAST);
			loadObjectSprite("CornerEdge_Right_Block", newSet, TILE_FAST);
			loadObjectSprite("Mud_Block", newSet, TILE_FAST);
			loadObjectSprite("Snow_Block", newSet, TILE_FAST);
			break;

		default:
		{
			loadObjectSprite("OBJ_Missing", newSet, TILE);
		} break;
	}


	return 0;
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


int DefineLevelFlag(Object *inputObject, ObjectController *ObjectList)
{
	if (inputObject == NULL || inputObject->ObjectID != LEVEL_FLAG_OBJ)
	{
		return INVALID_DATA;
	}

	SetDrawPriorityToFront(ObjectList, inputObject);

	inputObject->ObjectBox->solid = UNSOLID;

	switch (inputObject->arg1)
	{
		default:
		break;
	}


	return 0;
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


	int i = 1;

	if (currentObject != NULL)
	{
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
	newObject->layer = MIDDLEGROUND;

	newObject->ObjectBox->xPos = (double)abs(xPos - (xPos % X_TILESCALE));
	newObject->ObjectBox->yPos = (double)abs(yPos - (yPos % Y_TILESCALE));
	newObject->ObjectBox->xSize = X_TILESCALE;
	newObject->ObjectBox->ySize = Y_TILESCALE;
	newObject->ObjectBox->xPosRight = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;
	newObject->ObjectBox->yPosTop = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;

	newObject->currentAnimation = 0;
	newObject->animationTick = 0;

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
	newRect->xPosRight = 0.0;
	newRect->yPosTop = 0.0;
	newRect->xSize = 0;
	newRect->ySize = 0;
	newRect->xFlip = 1;
	newRect->yFlip = 1;
	newRect->direction = RADIAN_90;
	newRect->spriteXOffset = 0;
	newRect->spriteYOffset = 0;

	newRect->solid = inputSolid;
	newRect->crouch = 0;
	newRect->xVelocity = 0.0;
	newRect->yVelocity = 0.0;

	return newRect;
}


DisplayData* createDisplayData(RenderMode startRenderMode)
{
	DisplayData *newDisplay = malloc(sizeof(DisplayData));

	newDisplay->currentSprite = 1;
	newDisplay->spriteBuffer = NULL;
	newDisplay->spriteSetSource = NULL;
	newDisplay->RenderModeOverride = startRenderMode;

	return newDisplay;
}


SpriteSet* createObjectSpriteSet(ObjectController *ObjectList, int ObjectID)
{
	if (ObjectList == NULL || ObjectID == LEVEL_FLAG_OBJ)
	{
		return NULL;
	}

	// Check for pre-existing spriteset
	SpriteSet *currentSetPtr;
	currentSetPtr = ObjectList->startSpriteSetPtr;

	int i = 1;

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

	if (currentSetPtr == NULL && i == 1)
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

	LoadSpriteSet(newSet, ObjectID);
	
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

	(*input) = (*input)->nextObject;


	if ((*input) != NULL)
	{
		(*input)->prevObject = prevObject;
	}
	else
	{
		ObjectList->lastObject = prevObject;
	}

	if (prevObject != NULL)
	{
		prevObject->nextObject = (*input);
	}
	else
	{
		ObjectList->firstObject = (*input);
	}

	Object *currentObject = ObjectList->firstObject;

	while (currentObject != NULL)
	{
		if (currentObject->ParentObject == tempObject)
		{
			deleteObject(&currentObject, ObjectList);
		}
		else
		{
			currentObject = currentObject->nextObject;
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
	
	Object *currentObject;
  	currentObject = ObjectList->firstObject;

	while (currentObject != NULL)
	{
		deleteObject(&currentObject, ObjectList);
	}
	
	ObjectList->objectCount = 0;
	ObjectList->firstObject = NULL;
	ObjectList->lastObject = NULL;

	return;
}


int MarkObjectForDeletion(Object *inputObject, ObjectController *ObjectList)
{
	if (inputObject == NULL || ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (inputObject->State == TO_BE_DELETED || ObjectList->firstObject == NULL)
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
	Object *nextPtr;
	nextPtr = input->nextObject;

	Object *prevPtr;
	prevPtr = input->prevObject;

	if (nextPtr == NULL)
	{
		return;
	}
	
	if (prevPtr == NULL)
	{
		ObjectList->firstObject = nextPtr;
	}
	else
	{
		prevPtr->nextObject = nextPtr;
	}
	
	input->nextObject = nextPtr->nextObject;
	input->prevObject = nextPtr;
	nextPtr->prevObject = prevPtr;
	nextPtr->nextObject = input;

	if (input->nextObject == NULL)
	{
		ObjectList->lastObject = input;
	}

	return;
}


void DecrementDrawPriority(ObjectController *ObjectList, Object *input)
{
	Object *nextPtr;
	nextPtr = input->nextObject;

	Object *prevPtr;
	prevPtr = input->prevObject;

	if (prevPtr == NULL)
	{
		return;
	}
	
	if (nextPtr == NULL)
	{
		ObjectList->lastObject = prevPtr;
	}
	else
	{
		nextPtr->prevObject = prevPtr;
	}
	
	input->prevObject = prevPtr->prevObject;
	input->nextObject = prevPtr;
	prevPtr->nextObject = nextPtr;
	prevPtr->prevObject = input;

	if (input->prevObject == NULL)
	{
		ObjectList->firstObject = input;
	}

	return;
}


void SetDrawPriorityToFront(ObjectController *ObjectList, Object *input)
{
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
		ObjectList->firstObject = nextPtr;
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
		ObjectList->lastObject = prevPtr;
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


// Updates all objects in gameworld
FunctionResult updateObjects(World *gameWorld, int keyboard[256])
{
	if (gameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (gameWorld->GameState == EMPTY_GAME || gameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}

	ObjectController *ObjectList = gameWorld->ObjectList;

	if (ObjectList == NULL || ObjectList->firstObject == NULL)
	{
		return MISSING_DATA;
	}

	Object *currentObject;
	currentObject = ObjectList->firstObject;

	int i = ObjectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		i--;

		if (currentObject->State == PAUSE_BEHAVIOUR || (gameWorld->GameState == CUTSCENE && currentObject->State != CUTSCENE_ACTOR) || currentObject->State == IN_INVENTORY)
		{
			currentObject = currentObject->nextObject;
			continue;
		}


		ObjectBehaviour(gameWorld, currentObject, keyboard);


		if (currentObject == NULL)
		{
			return MISSING_DATA;
		}

		currentObject = currentObject->nextObject;
		
	}


	currentObject = ObjectList->firstObject;

	while (currentObject != NULL)
	{
		if (currentObject->State == TO_BE_DELETED)
		{
			// If object has been deleted, pointer will be incremented
			deleteObject(&currentObject, ObjectList);
		}
		else
		{
			currentObject = currentObject->nextObject;
		}
	}

	return LEMON_SUCCESS;
}


int ObjectBehaviour(World *gameWorld, Object *inputObject, int keyboard[256])
{
	if (gameWorld == NULL || gameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (gameWorld->GamePaused == 1 && inputObject->ObjectID != UI_ELEMENT || inputObject->State == TO_BE_DELETED)
	{
		return EXECUTION_UNNECESSARY;
	}
	

	PlayerData *Player = gameWorld->Player;

	switch (inputObject->ObjectID)
	{
		case UI_ELEMENT:
			UpdateUIElement(gameWorld, inputObject, keyboard);
			break;

		case PARTICLE:
			UpdateParticle(gameWorld, inputObject);
			break;


		case LEVEL_FLAG_OBJ:
			UpdateFlagObject(gameWorld, inputObject);
			break;


		case MOVING_PLATFORM_HOR:
		{
			UpdateHorizontalPlatform(Player, inputObject);
		} break;


		case MOVING_PLATFORM_VER:
		{
			UpdateVerticalPlatform(Player, inputObject);
		} break;


		case COIN:
		{
			if (checkBoxOverlapsBox(Player->PlayerBox, inputObject->ObjectBox) == 1)
			{
				AddObject(gameWorld, PARTICLE, inputObject->ObjectBox->xPos, inputObject->ObjectBox->yPos, 0, 0, SPARKLE, 1, 0, 0, 0);
				MarkObjectForDeletion(inputObject, gameWorld->ObjectList);
				Player->coinCount++;
				LemonPlaySound("Coin_Collect", "Objects", OBJECT_SFX, 0.8);
			}

		} break;


		case SPRING:
		{
			if (inputObject->arg5 < 1 && Player->PlayerBox->yVelocity < -1.0 && checkBoxOverlapsBox(Player->PlayerBox, inputObject->ObjectBox) == 1)
			{
				Player->PlayerBox->yVelocity = (double)inputObject->arg1;
				LemonPlaySound("Spring", "Objects", OBJECT_SFX, 1.0);
				inputObject->arg5 = 20;
			}

			if (inputObject->arg5 > 0)
			{
				inputObject->arg5--;
			}

		} break;


		case VERTICAL_GATE:
		{
			UpdateVerticalGate(inputObject, gameWorld->ObjectList, Player);
		} break;


		case HORIZONTAL_GATE:
		{
			UpdateHorizontalGate(inputObject, gameWorld->ObjectList, Player);
		} break;


		case GATE_SWITCH_TIMED:
		case GATE_SWITCH:
		{
			UpdateGateSwitch(Player, inputObject);
			inputObject->ObjectBox->yPos = 64 + (32 * inputObject->arg3);
		} break;


		case DOOR:
		{
			UpdateDoor(Player, inputObject);
		} break;


		default:
			break;
	}


	if (inputObject->State == TO_BE_DELETED)
	{
		return EXECUTION_UNNECESSARY;
	}


	// Move object
	moveObjectX(inputObject, Player);

	moveObjectY(inputObject, Player);
		
	// Assign Sprite   (0 means do not change spriteset)
	switchSprite(inputObject->ObjectDisplay->currentSprite, 0, inputObject->ObjectDisplay);


	return 0;
}


int InitialiseParticle(Object *particle, int animation, int repeatCount, int frameRate, int particleLifeTime)
{
	particle->layer = PARTICLES;
	particle->ObjectBox->solid = UNSOLID;
	particle->currentAnimation = animation;
	particle->arg2 = frameRate;
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
	particle->animationTick++;

	if (particle->animationTick > 9999 || particle->animationTick < 0)
	{
		particle->animationTick = 0;
	}

	LoopParticleAnimation(particle);


	CustomParticleBehaviour(GameWorld, particle);


	// If repeat count is reached or animationTick exceeds maximum lifetime, mark for deletion
	if (particle->arg3 < 1 && particle->arg1 < 1)
	{
		MarkObjectForDeletion(particle, GameWorld->ObjectList);
	}

	if (particle->arg3 > 0 && particle->animationTick > particle->arg3)
	{
		MarkObjectForDeletion(particle, GameWorld->ObjectList);
	}

	return 0;
}


int CustomParticleBehaviour(World *GameWorld, Object *particle)
{
	// Custom behaviour
	switch(particle->currentAnimation)
	{
		default:
		break;
	}

	return 0;
}


int UpdateGateSwitch(PlayerData *player, Object *gateSwitch)
{
	// arg1 = switch ID
	// arg2 = switch type (0 = or, 1 = and)
	// arg3 = switch off/on (0/1) state
	// arg4 = Player still on switch?
	// arg5 = timer


	if (checkBoxOverlapsBox(player->PlayerBox, gateSwitch->ObjectBox) == 1)
	{
		if (gateSwitch->arg4 == 0)
		{
			gateSwitch->arg3 = (gateSwitch->arg3 + 1) % 2;
			gateSwitch->animationTick = gateSwitch->arg5;
			gateSwitch->arg4 = 1;
		}

		return 0;
	}

	gateSwitch->arg4 = 0;

	if (gateSwitch->arg5 > 0 && gateSwitch->arg3 == 1)
	{
		gateSwitch->animationTick -= 1;

		if (gateSwitch->animationTick < 1.0)
		{
			gateSwitch->arg3 = 0;
			gateSwitch->animationTick = gateSwitch->arg5;
		}
	}

	return 0;
}


int UpdateVerticalGate(Object *gate, ObjectController *ObjectList, PlayerData *player)
{
	// arg1 = door ID
	// arg2 = door open/close (0/1)
	// arg3 = closed gate y position 
	// arg4 = speed
	
	if (ObjectList == NULL)
	{
		return -1;
	}

	// Evaluate state of gate
	gateControl(gate, ObjectList);

	int closedPosition = gate->arg3;
	int speed = gate->arg4;


	// Animation control
	switch (gate->currentAnimation)
	{
		case 0:
		{
			SetObjectYPosition(gate, closedPosition, player);
			gate->ObjectBox->yVelocity = 0.0;

			if (gate->arg2 == 1)
			{
				gate->currentAnimation = 1;
				gate->animationTick = 0;
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

			double prevYPos = gate->ObjectBox->yPos;
			gate->ObjectBox->yPos += (gate->ObjectBox->yVelocity + (speed/abs(speed)));

			if (checkBoxOverlapsBox(player->PlayerBox, gate->ObjectBox) == 1)
			{
				gate->ObjectBox->yVelocity = 0.0;
			}

			gate->ObjectBox->yPos = prevYPos;

			if ( (speed < 0 && gate->ObjectBox->yPos < closedPosition - gate->ObjectBox->ySize) || (speed > 0 && gate->ObjectBox->yPos > closedPosition + gate->ObjectBox->ySize) )
			{
				gate->currentAnimation = 2;
				gate->animationTick = 0;
				gate->ObjectBox->yVelocity = 0.0;
				SetObjectYPosition(gate, closedPosition + (gate->ObjectBox->ySize * (speed/abs(speed))), player);
			}

		} break;


		case 2:
		{
			SetObjectYPosition(gate, closedPosition + (gate->ObjectBox->ySize * (speed/abs(speed))), player);
			gate->ObjectBox->yVelocity = 0.0;

			if (gate->arg2 == 0)
			{
				gate->currentAnimation = 3;
				gate->animationTick = 0;
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

			double prevYPos = gate->ObjectBox->yPos;
			gate->ObjectBox->yPos += (gate->ObjectBox->yVelocity - (speed/abs(speed)));

			if (checkBoxOverlapsBox(player->PlayerBox, gate->ObjectBox) == 1)
			{
				gate->ObjectBox->yVelocity = 0.0;
			}

			gate->ObjectBox->yPos = prevYPos;

			if ( (speed > 0 && gate->ObjectBox->yPos < closedPosition) || (speed < 0 && gate->ObjectBox->yPos > closedPosition) )
			{
				gate->currentAnimation = 0;
				gate->animationTick = 0;
				gate->ObjectBox->yVelocity = 0.0;
				SetObjectYPosition(gate, closedPosition, player);
			}

		} break;
	}


	return 0;
}


int UpdateHorizontalGate(Object *gate, ObjectController *ObjectList, PlayerData *player)
{
	// arg1 = door ID
	// arg2 = door open/close (0/1)
	// arg3 = closed gate y position 
	// arg4 = speed
	
	if (ObjectList == NULL)
	{
		return -1;
	}

	// Evaluate state of gate
	gateControl(gate, ObjectList);

	int closedPosition = gate->arg3;
	int speed = gate->arg4;


	// Animation control
	switch (gate->currentAnimation)
	{
		case 0:
		{
			SetObjectXPosition(gate, closedPosition, player);
			gate->ObjectBox->xVelocity = 0.0;

			if (gate->arg2 == 1)
			{
				gate->currentAnimation = 1;
				gate->animationTick = 0;
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

			double prevYPos = gate->ObjectBox->xPos;
			gate->ObjectBox->xPos += (gate->ObjectBox->xVelocity + (speed/abs(speed)));

			if (checkBoxOverlapsBox(player->PlayerBox, gate->ObjectBox) == 1)
			{
				gate->ObjectBox->xVelocity = 0.0;
			}

			gate->ObjectBox->xPos = prevYPos;

			if ( (speed < 0 && gate->ObjectBox->xPos < closedPosition - gate->ObjectBox->xSize) || (speed > 0 && gate->ObjectBox->xPos > closedPosition + gate->ObjectBox->xSize) )
			{
				gate->currentAnimation = 2;
				gate->animationTick = 0;
				gate->ObjectBox->xVelocity = 0.0;
				SetObjectXPosition(gate, closedPosition + (gate->ObjectBox->xSize * (speed/abs(speed))), player);
			}

		} break;


		case 2:
		{
			SetObjectXPosition(gate, closedPosition + (gate->ObjectBox->xSize * (speed/abs(speed))), player);
			gate->ObjectBox->xVelocity = 0.0;

			if (gate->arg2 == 0)
			{
				gate->currentAnimation = 3;
				gate->animationTick = 0;
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

			double prevXPos = gate->ObjectBox->xPos;
			gate->ObjectBox->xPos += (gate->ObjectBox->xVelocity - (speed/abs(speed)));

			if (checkBoxOverlapsBox(player->PlayerBox, gate->ObjectBox) == 1)
			{
				gate->ObjectBox->xVelocity = 0.0;
			}

			gate->ObjectBox->xPos = prevXPos;

			if ( (speed > 0 && gate->ObjectBox->xPos < closedPosition) || (speed < 0 && gate->ObjectBox->xPos > closedPosition) )
			{
				gate->currentAnimation = 0;
				gate->animationTick = 0;
				gate->ObjectBox->xVelocity = 0.0;
				SetObjectXPosition(gate, closedPosition, player);
			}

		} break;
	}


	return 0;
}


int gateControl(Object *gate, ObjectController *ObjectList)
{
	Object *currentObject;
	currentObject = ObjectList->firstObject;

	if (gate == NULL || currentObject == NULL)
	{
		return -1;
	}


	int onOrOff = -1;

	// Run through object list
	while (currentObject != NULL)
	{	

		// If a matching switch is found, execute switch/gate logic
		if ((currentObject->ObjectID == GATE_SWITCH || currentObject->ObjectID == GATE_SWITCH_TIMED) && gate->arg1 == currentObject->arg1)
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
						gate->arg2 = 1;
						return 0;
					}	
				} 

				if (currentObject->arg2 == CHAIN_SWITCH)
				{		
					onOrOff = onOrOff && currentObject->arg3;
				} 
			}

		}
		

		currentObject = currentObject->nextObject;
	}


	gate->arg2 = onOrOff;

	return 0;
}


Object* InitialiseMovingPlatform(Object *inputObject, int objectID, int xPos, int yPos, int bound1, int bound2, int speed, int timer)
{
	if (inputObject == NULL)
	{
		return NULL;
	}

	// Default settings
	inputObject->layer = BACKGROUND;
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


int UpdateDoor(PlayerData *Player, Object *Door)
{
	// arg3: open/close state
	// arg1, arg2: [x/y]Pos in event of no linked door

	if (Door->arg3 < 1 && checkBoxOverlapsBox(Door->ObjectBox, Player->InteractBox) == 1)
	{
		Door->arg3 = 32;

		LemonPlaySound("DoorOpen", "Objects", OBJECT_SFX, 1.0);
		
		if (Door->ParentObject == NULL || Door->ParentObject->ObjectBox == NULL)
		{
			Player->PlayerBox->xPos = Door->arg1;
			Player->PlayerBox->yPos = Door->arg2;
		}
		else
		{
			Player->PlayerBox->xPos = Door->ParentObject->ObjectBox->xPos;
			Player->PlayerBox->yPos = Door->ParentObject->ObjectBox->yPos;
			Door->ParentObject->arg3 = 32;
		}
	}
	else if (Door->arg3 > 0)
	{
		Door->arg3--;
	}

	return 0;
} 


int moveObjectX(Object *inputObject, PlayerData *player)
{
	if (fabs(inputObject->ObjectBox->xVelocity) < 0.1)
	{
		inputObject->ObjectBox->xVelocity = 0.0;
		return EXECUTION_UNNECESSARY;
	}


	double prevObjXPos = inputObject->ObjectBox->xPos;

	inputObject->ObjectBox->xPos += (inputObject->ObjectBox->xVelocity);
	inputObject->ObjectBox->xPosRight = inputObject->ObjectBox->xPos + inputObject->ObjectBox->xSize;


	if (player == NULL)
	{
		return MISSING_DATA;
	}


	int result = ResolveXCollisionByPush(inputObject->ObjectBox, player->PlayerBox, prevObjXPos);

	if (result == 1)
	{
		return 0;
	}

	inputObject->ObjectBox->xPos = prevObjXPos;
	player->PlayerBox->yPos -= 2.0;

	result = CheckBoxCollidesBox(player->PlayerBox, inputObject->ObjectBox);

	inputObject->ObjectBox->xPos += (inputObject->ObjectBox->xVelocity);
	player->PlayerBox->yPos += 2.0;

	if (result == 1)
	{
		int pixelDifference = (int)inputObject->ObjectBox->xPos - (int)(inputObject->ObjectBox->xPos - (inputObject->ObjectBox->xVelocity));
		player->PlayerBox->xPos += pixelDifference;
	}	


	return 0;
}



int moveObjectY(Object *inputObject, PlayerData *player)
{
	if (fabs(inputObject->ObjectBox->yVelocity) < 0.1)
	{
		inputObject->ObjectBox->yVelocity = 0.0;
		return EXECUTION_UNNECESSARY;
	}

	double prevObjYPos = inputObject->ObjectBox->yPos;
	double prevObjYPosTop = inputObject->ObjectBox->yPos + inputObject->ObjectBox->ySize;

	inputObject->ObjectBox->yPos += (inputObject->ObjectBox->yVelocity);
	inputObject->ObjectBox->yPosTop = inputObject->ObjectBox->yPos + inputObject->ObjectBox->ySize;

	if (player == NULL)
	{
		return MISSING_DATA;
	}


	int result = ResolveYCollisionByPush(inputObject->ObjectBox, player->PlayerBox, prevObjYPos);

	if (result == 1)
	{
		return 0;
	}

	inputObject->ObjectBox->yPos = prevObjYPos;
	player->PlayerBox->yPos -= 2.0;

	result = CheckBoxCollidesBox(player->PlayerBox, inputObject->ObjectBox);

	inputObject->ObjectBox->yPos += inputObject->ObjectBox->yVelocity;
	player->PlayerBox->yPos += 2.0;

	if (result == 1)
	{
		int pixelDifference = (int)inputObject->ObjectBox->yPos - (int)(inputObject->ObjectBox->yPos - (inputObject->ObjectBox->yVelocity));
		player->PlayerBox->yPos += pixelDifference;
	}	

	return 0;
}


int SetObjectXPosition(Object *inputObject, double newXPos, PlayerData *Player)
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

	if (Player == NULL || inputObject->ObjectBox->solid == 0)
	{
		return MISSING_DATA;
	}

	ResolveXCollisionByPush(inputObject->ObjectBox, Player->PlayerBox, prevObjXPos);

	return 0;
}


//Method for setting an object's position to avoid incorrect collision with player
int SetObjectYPosition(Object *inputObject, double newYPos, PlayerData *Player)
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

	if (Player == NULL || inputObject->ObjectBox->solid == 0)
	{
		return MISSING_DATA;
	}

	ResolveYCollisionByPush(inputObject->ObjectBox, Player->PlayerBox, prevObjYPos);


	return 0;
}


// Method for centering object size increase and handling player collisions
int ChangeObjectXSizeBy(int change, Object *inputObject, PlayerData *Player)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	double changeHalf = change / 2.0;

	inputObject->ObjectBox->xSize += change;
	inputObject->ObjectBox->xPos -= changeHalf;
	inputObject->ObjectBox->xPosRight += changeHalf;

	if (Player == NULL || change == 0 || inputObject->ObjectBox->solid < 1)
	{
		return EXECUTION_UNNECESSARY;
	}

	ResolveXCollisionByPush(inputObject->ObjectBox, Player->PlayerBox, inputObject->ObjectBox->xPos + changeHalf);
	
	return 0;
}


int ChangeObjectYSizeBy(int change, Object *inputObject, PlayerData *Player)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	double changeHalf = change / 2.0;

	inputObject->ObjectBox->ySize += change;
	inputObject->ObjectBox->yPos -= changeHalf;
	inputObject->ObjectBox->yPosTop += changeHalf;

	if (Player == NULL || change == 0 || inputObject->ObjectBox->solid < 1)
	{
		return EXECUTION_UNNECESSARY;
	}


	ResolveYCollisionByPush(inputObject->ObjectBox, Player->PlayerBox, inputObject->ObjectBox->yPos + changeHalf);
	
	return 0;
}


int checkBoxOverlapsBox(PhysicsRect *inputBox, PhysicsRect *compareBox)
{
	if (!(inputBox->xSize > 0 && inputBox->ySize > 0 && compareBox->xSize > 0 && compareBox->ySize > 0) )
	{
		return 0;
	}

	return !((int)inputBox->xPos >= (int)(compareBox->xPos + compareBox->xSize) || (int)(inputBox->xPos + inputBox->xSize) <= (int)compareBox->xPos || (int)inputBox->yPos >= (int)(compareBox->yPos + compareBox->ySize) || (int)(inputBox->yPos + inputBox->ySize) <= (int)compareBox->yPos);
}


int checkBoxOverlapsBoxBottom(PhysicsRect *inputBox, PhysicsRect *compareBox)
{
	if (!(inputBox->xSize > 0 && inputBox->ySize > 0 && compareBox->xSize > 0 && compareBox->ySize > 0) )
	{
		return 0;
	}

	return !((int)inputBox->xPos >= (int)(compareBox->xPos + compareBox->xSize) || (int)(inputBox->xPos + inputBox->xSize) <= (int)compareBox->xPos || (inputBox->yPos - 2) >= (int)(compareBox->yPos + compareBox->ySize) || (inputBox->yPos + 8) <= (int)compareBox->yPos);
}



// returns pointer of object overlapping, NULL if no object is detected
Object* GetCollidingObject(PhysicsRect *inputBox, World *gameWorld)
{
	if (inputBox == NULL || inputBox->solid == UNSOLID)
	{
		return NULL;
	}

	Object *currentObject;
	currentObject = gameWorld->ObjectList->firstObject;

	if (currentObject == NULL)
	{
		return NULL;
	}

	while (currentObject != NULL)
	{
		int result = 0;

		if (currentObject->ObjectBox->solid == UNSOLID || currentObject->layer > FOREGROUND || checkBoxOverlapsBox(inputBox, currentObject->ObjectBox) == 0)
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		result = CheckBoxCollidesBox(inputBox, currentObject->ObjectBox);

		if (result == 1)
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

	if (!(inputBox->xSize > 0 && inputBox->ySize > 0 && compareBox->xSize > 0 && compareBox->ySize > 0) )
	{
		return 0;
	}

	int inputX = inputBox->xPos;
	int inputXRight = inputBox->xPos + inputBox->xSize;
	int inputY = inputBox->yPos;
	int inputYTop = inputBox->yPos + inputBox->ySize;
	int prevInputY = inputY - (inputBox->yVelocity * sin(inputBox->direction));

	double inputCosVal = cos(inputBox->direction);

	if (fabs(inputCosVal) > 0.001)
	{
		prevInputY -= inputCosVal * inputBox->xVelocity;
	}


	int compareX = compareBox->xPos;
	int compareXRight = compareBox->xPos + compareBox->xSize;
	int compareY = compareBox->yPos;
	int compareYTop = compareBox->yPos + compareBox->ySize;
	int prevCompareY = compareY - (compareBox->yVelocity * sin(compareBox->direction));

	double compareCosVal = cos(compareBox->direction);

	if (fabs(compareCosVal) > 0.001)
	{
		prevCompareY -= compareCosVal * compareBox->xVelocity;
	}


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
				inputYTop = ((inputBox->xSize - (compareBox->xPos - inputBox->xPos)) * ((double)inputBox->ySize/(double)inputBox->xSize));
			}
			

			if (inputYTop > inputBox->ySize)
			{
				inputYTop = inputBox->ySize;
			}

			inputYTop += inputBox->yPos;
		} break;
			
		case JUMP_THROUGH:
		{
			if (compareBox->yVelocity > 0.0 || compareBox->crouch == 1 || prevCompareY < inputY - 2)
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
			if (inputBox->yVelocity > 0.0 || inputBox->crouch == 1 || prevInputY < compareYTop - 2)
			{
				return 0;
			}
		} break;

		case UNSOLID:
		return 0;
			
		default:
		break;
	}


	return !(inputY >= compareYTop || inputYTop <= compareY || inputX >= compareXRight || inputXRight <= compareX);
}


int AssignDirection(PhysicsRect *inputBox, Object *currentObject)
{
	if (currentObject == NULL)
	{
		inputBox->direction = RADIAN_90;
		return 0;
	}

	
	switch (currentObject->ObjectBox->solid)
	{
			case FLAT_SLOPE:
			{
				double slope = (double)currentObject->ObjectBox->ySize/(double)currentObject->ObjectBox->xSize;
				double slopeFloor;

				if (currentObject->ObjectBox->xFlip == 1)
				{
					slopeFloor = ((inputBox->xPos + inputBox->xSize - currentObject->ObjectBox->xPos) * slope);
				}
				else
				{
					slopeFloor = ((currentObject->ObjectBox->xSize - (inputBox->xPos - currentObject->ObjectBox->xPos)) * slope);
				}

				if (slopeFloor < 0 || slopeFloor > currentObject->ObjectBox->ySize)
				{
					inputBox->direction = RADIAN_90;
					break;
				}
				
				if (currentObject->ObjectBox->xFlip == 1)
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
		if (currentObject->layer == inputObject->layer && currentObject->ObjectID == overlapObjectID && currentObject != inputObject)
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
		if (currentObject->layer == inputObject->layer && currentObject->ObjectBox->solid == solidID && currentObject != inputObject)
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
		if (currentObject->layer == inputObject->layer && currentObject->ObjectBox->solid > 0 && currentObject != inputObject)
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


int ResolveXCollision(PhysicsRect *movingBox, double prevBoxXPos, World *GameWorld)
{
	if (movingBox == NULL || GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}
	
	Object *currentObject;
	currentObject = GameWorld->ObjectList->firstObject;

	if (currentObject == NULL)
	{
		return EXECUTION_UNNECESSARY;
	}

	PhysicsRect *compareBox;

	double destinationX = movingBox->xPos;


	// Collision for movement on special solids when movingBox will not directly hit said special solid but is standing on it
	movingBox->xPos = prevBoxXPos;
	movingBox->yPos -= 2;
	currentObject = GetCollidingObject(movingBox, GameWorld);
	movingBox->yPos += 2;
	movingBox->xPos = destinationX;

	if (currentObject != NULL)
	{
		compareBox = currentObject->ObjectBox;

		switch (compareBox->solid)
		{
			case FLAT_SLOPE:
			{
				movingBox->xPos = prevBoxXPos;

				ClimbSlope(movingBox, compareBox, GameWorld);

				return EXECUTION_UNNECESSARY;
			} break;

			default:
			break;
		}
	}


	// regular collision
	currentObject = GameWorld->ObjectList->firstObject;
	int count = 0;

	while (currentObject != NULL && count < 7)
	{
		currentObject = GetCollidingObject(movingBox, GameWorld);

		if (currentObject == NULL)
		{
			return 0;
		}

		count++;

		compareBox = currentObject->ObjectBox;
		
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
					if ((int)prevBoxXPos >= objXRight)
					{
						movingBox->xPos = objXRight;
						ApplyXPhysics(movingBox, compareBox);
						break;
					}
				}
				else
				{
					if ((int)prevBoxXPos + movingBox->xSize <= objX)
					{
						movingBox->xPos = objX - movingBox->xSize;
						ApplyXPhysics(movingBox, compareBox);
						break;
					}
				}

				movingBox->xPos = prevBoxXPos;

				ClimbSlope(movingBox, compareBox, GameWorld);
			} break;


			case JUMP_THROUGH:
			case UNSOLID:
				break;


			default:
			{
				if (prevBoxXPos < ObjXCenter)
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

	}


	return LEMON_SUCCESS;
}


int ApplyXPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox)
{
	if (inputBox == NULL || physicsBox == NULL)
	{
		return MISSING_DATA;
	}

	if (inputBox->xVelocity > 0.0)
	{
		if (physicsBox->xVelocity > 0.1)
		{
			inputBox->xVelocity = physicsBox->xVelocity;
		}
		else
		{
			inputBox->PhysicsXVelocity = 0.0;
			inputBox->xVelocity = 0.0;
		}

	}
	else
	{
		if (physicsBox->xVelocity < -0.1)
		{
			inputBox->xVelocity = physicsBox->xVelocity;
		}
		else
		{		
			inputBox->PhysicsXVelocity = 0.0;
			inputBox->xVelocity = 0.0;
		}

	}

	return 0;
}


int ResolveYCollision(PhysicsRect *movingBox, double prevYPos, World *GameWorld, int *JumpProgressPtr)
{
	if (movingBox == NULL || GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}
	
	Object *currentObject;
	currentObject = GameWorld->ObjectList->firstObject;

	if (currentObject == NULL)
	{
		return EXECUTION_UNNECESSARY;
	}


	int count = 0;
	int jumpProgressBuffer = 0;

	if (JumpProgressPtr != NULL)
	{
		jumpProgressBuffer = (*JumpProgressPtr);
	}

	while (currentObject != NULL && count < 8)
	{
		currentObject = GetCollidingObject(movingBox, GameWorld);

		count++;

		if (currentObject == NULL)
		{
			continue;
		}


		double objX = currentObject->ObjectBox->xPos;
		double objY = currentObject->ObjectBox->yPos;
		double objXRight = objX + currentObject->ObjectBox->xSize;
		double objYTop = objY + currentObject->ObjectBox->ySize;
		double ObjYCenter = ((objY + objYTop) / 2.0);

		switch(currentObject->ObjectBox->solid)
		{
			case FLAT_SLOPE:
			{
				if ((int)prevYPos < objY - 8)
				{
					movingBox->yPos = (objY - movingBox->ySize);
					jumpProgressBuffer = 100;
					break;
				}
			

				// If movingBox is halfway off edge, floor of slope continues to be calculated as Y = X * slope
				// So here it is reset to the expected maximum if it over
				double slope = ((double)currentObject->ObjectBox->ySize/(double)currentObject->ObjectBox->xSize);
				double slopeFloor;

				if (currentObject->ObjectBox->xFlip == 1)
				{
					slopeFloor = ((movingBox->xPos + movingBox->xSize - objX) * slope);
				}
				else
				{
					slopeFloor = ((currentObject->ObjectBox->xSize - (movingBox->xPos - objX)) * slope);
				}

				slopeFloor = clamp(slopeFloor, 0, currentObject->ObjectBox->ySize);
				
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


		ApplyYPhysics(movingBox, currentObject->ObjectBox);
		
	}

	if (JumpProgressPtr != NULL)
	{
		(*JumpProgressPtr) = jumpProgressBuffer;
	}

	return LEMON_SUCCESS;
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


int ResolveXCollisionByPush(PhysicsRect *movingBox, PhysicsRect *compareBox, double prevObjXPos)
{
	if (movingBox == NULL || compareBox == NULL)
	{
		return 0;
	}

	double ObjXPos = movingBox->xPos;
	double ObjXPosRight = movingBox->xPos + movingBox->xSize;
	double ObjYPos = movingBox->yPos;
	double ObjYPosTop = movingBox->yPos + movingBox->ySize;

	double prevXPosBuffer = prevObjXPos;
	double prevXPosRightBuffer = prevObjXPos + movingBox->xSize;
	double prevXPosCenter = (prevXPosBuffer + prevXPosRightBuffer) / 2.0;


	int result = 0;

	switch(movingBox->solid)
	{
		case FLAT_SLOPE:
			if (movingBox->xFlip == 1)
			{
				int slopeLeftEdge = (int)((compareBox->yPos - ObjYPos) / ((double)movingBox->ySize/(double)movingBox->xSize));
				slopeLeftEdge = clamp(slopeLeftEdge, 0, movingBox->xSize);

				ObjXPos = slopeLeftEdge + ObjXPos;
				prevXPosCenter = prevXPosRightBuffer;
			}
			else
			{
				int slopeLeftEdge = (int)(movingBox->xSize - ((compareBox->yPos - ObjYPos) / ((double)movingBox->ySize/(double)movingBox->xSize)) );
				slopeLeftEdge = clamp(slopeLeftEdge, 0, movingBox->xSize);

				ObjXPosRight = slopeLeftEdge + ObjXPos;
				prevXPosCenter = prevXPosBuffer;
			}
			break;

		case UNSOLID:
			return 0;

		default:
			break;
	}

	result = CheckBoxCollidesBox(compareBox, movingBox);


	if (result == 1)
	{
		if ((int)compareBox->xPos < (int)prevXPosCenter)
		{
			compareBox->xPos = ObjXPos - compareBox->xSize;
		}
		else
		{
			compareBox->xPos = ObjXPosRight;
		}

		return 1;
	}

	return 0;
}


int ResolveYCollisionByPush(PhysicsRect *movingBox, PhysicsRect *compareBox, double prevObjYPos)
{
	if (movingBox == NULL || compareBox == NULL)
	{
		return 0;
	}

	double ObjXPos = movingBox->xPos;
	double ObjXPosRight = movingBox->xPos + movingBox->xSize;
	double ObjYPos = movingBox->yPos;
	double ObjYPosTop = movingBox->yPos + movingBox->ySize;

	double prevYPosBuffer = prevObjYPos;
	double prevYPosTopBuffer = prevObjYPos + movingBox->ySize;


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
			
			prevYPosTopBuffer = prevObjYPos;
			break;

		case JUMP_THROUGH:
			if (compareBox->yVelocity > 0.0 || compareBox->crouch == 1)
			{
				return 0;
			}
			break;

		case UNSOLID:
			return 0;

		default:
			break;
	}


	result = CheckBoxCollidesBox(compareBox, movingBox);
	

	if (result == 1)
	{
		if (compareBox->yPos < ((prevYPosBuffer + prevYPosTopBuffer) / 2.0) )
		{
			compareBox->yPos = ObjYPos - compareBox->ySize;
		}
		else
		{
			compareBox->yPos = ObjYPosTop; 
		}

		return 1;
	}


	return 0;
}


int ClimbSlope(PhysicsRect *inputBox, PhysicsRect *compareBox, World *GameWorld)
{		
	if (inputBox == NULL || compareBox == NULL || GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (fabs(inputBox->xVelocity) < 0.1)
	{
		return 0;
	}

	double slope = ((double)compareBox->ySize/(double)compareBox->xSize);
	Object *objectCheck = NULL;
	int i = 0;

	double orientation = inputBox->xVelocity/fabs(inputBox->xVelocity) * sin(inputBox->direction);


	while (objectCheck == NULL && i < round(fabs(inputBox->xVelocity)) )
	{
		// Move inputBox forward
		inputBox->xPos += orientation; 
		i++;

		// Check for any obstructions
		objectCheck = GetCollidingObject(inputBox, GameWorld);

		// Move up until no longer colliding or until reached slopeclimb limit
		int slopeClimb = 0;
		while (objectCheck != NULL && slopeClimb < 6)
		{
			inputBox->yPos++;
			slopeClimb++;
			objectCheck = GetCollidingObject(inputBox, GameWorld);
		}

		// If couldnt climb over obstacle under climb limit, move back down 
		if (objectCheck != NULL)
		{
			inputBox->yPos -= slopeClimb;
		}

	}

	if (objectCheck != NULL)
	{
		// Shift back to movestep right before colliding with new object
		inputBox->xPos -= orientation;

		ApplyXPhysics(inputBox, compareBox);

		if (CheckBoxCollidesBox(inputBox, compareBox) == 0)
		{
			return EXECUTION_UNNECESSARY;
		}

		// Move up if collided with floor
		int slopeClimb = 0;
		while (objectCheck != NULL && slopeClimb < 6)
		{
			inputBox->yPos++;
			slopeClimb++;
			objectCheck = GetCollidingObject(inputBox, GameWorld);
		}

		return LEMON_SUCCESS;
	}	

					
	return LEMON_SUCCESS;
}
