#include "gameObjects.h"



Object* AddObject(World *gameWorld, int objectID, int xPos, int yPos, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	if (gameWorld == NULL || gameWorld->objectList == NULL)
	{
		return NULL;
	}

	ObjectController *objectList = gameWorld->objectList;


	if (objectID >= UNDEFINED_OBJECT || objectID < LEVEL_FLAG_OBJ)
	{
		printf("This object is not defined! Type: %d\n", objectID);
		return NULL;
	}


	Object *newObject;
	newObject = createNewObject(objectList, xPos, yPos, objectID);

	if (newObject == NULL)
	{
		return NULL;
	}

	// Default settings
	newObject->objectRenderMode = DEFAULT_TO_SPRITE;
	newObject->ySize = Y_TILESCALE;
	newObject->xSize = X_TILESCALE;
	newObject->arg1 = arg1;
	newObject->arg2 = arg2;
	newObject->arg3 = arg3;
	newObject->arg4 = arg4;
	newObject->arg5 = arg5;
	newObject->currentSprite = 1;
	newObject->solid = 1;
	newObject->layer = MIDDLEGROUND;
	

	CreateObjectSpriteSet(objectList, objectID);
	


	// Set Object parameters
	switch (objectID)
	{
		case LEVEL_FLAG_OBJ:
			break;


		case SOLID_BLOCK:
			newObject->xSize = arg1 * X_TILESCALE;
			newObject->ySize = arg2 * Y_TILESCALE;
			break;


		case RIGHT_SLOPE:
		//Angle: Y = (X * ySize/xSize)
		//Angle: X = (Y / (ySize/xSize))
			newObject->xSize = arg1;
			newObject->ySize = arg2;
			newObject->solid = 2;

			if (arg1 + arg2 > 256)
			{
				switchObjectSprite(3, newObject, objectList);
			}
			else if (arg1 + arg2 > 128)
			{
				switchObjectSprite(2, newObject, objectList);
			}
			break;


		case LEFT_SLOPE:
		//Angle: Y = ((xSize - X) * ySize/xSize)
		//Angle: X = xSize - (Y / (ySize/xSize))
			newObject->xSize = arg1;
			newObject->ySize = arg2;
			newObject->solid = 3;
			newObject->xFlip = -1;

			if (arg1 + arg2 > 256)
			{
				switchObjectSprite(3, newObject, objectList);
			}
			else if (arg1 + arg2 > 128)
			{
				switchObjectSprite(2, newObject, objectList);
			}
			break;


		case JUMP_THRU:
			newObject->solid = 4;
			newObject->xSize = arg1 * X_TILESCALE;
			break;

		case COIN:
		// Coin
			newObject->arg1 = arg1;
			newObject->arg2 = arg2;
			newObject->arg3 = 1;
			newObject->solid = 0;
			break;

		
		case SPRING:
		// spring
			newObject->arg1 = arg1;
			newObject->solid = 0;
			break;


		case VERTICAL_GATE:
		// Medium sized, vertical gate
			newObject->arg1 = arg1;
			newObject->arg2 = 0;
			newObject->arg3 = yPos;
			newObject->arg4 = arg2;
			newObject->ySize = 4 * Y_TILESCALE;
			newObject->xSize = 2 * X_TILESCALE;
			break;

		case HORIZONTAL_GATE:
		// Medium sized, horizontal gate
			newObject->arg1 = arg1;
			newObject->arg2 = 0;
			newObject->arg3 = yPos;
			newObject->arg4 = arg2;
			newObject->ySize = 2 * Y_TILESCALE;
			newObject->xSize = 4 * X_TILESCALE;
			break;


		case GATE_SWITCH:
		// switch for gate - arg1 is ID to match switch to gate, arg2 denotes type of switch (0 = or switch, 1 = and switch)
			newObject->arg1 = arg1;
			newObject->arg2 = arg2;
			newObject->solid = 0;
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
			newObject->solid = 0;
			newObject->animationTick = abs(arg2);
			break;


		case MOVING_PLATFORM_VER:
		case MOVING_PLATFORM_HOR:
			DefineMovingPlatform(newObject, objectID, xPos, yPos, arg1, arg2, arg3, arg4);
			break;


		case PARTICLE:
		// Do not modify! (Unless you wish to alter rendermode)
			newObject->layer = PARTICLES;
			newObject->currentAnimation = arg1;
			newObject->arg2 = arg3;
			newObject->arg3 = arg4;
			newObject->arg4 = 0;
			newObject->arg5 = 0;

			// Assign particle sprite
			newObject->arg1 = 1;
			LoopParticleAnimation(newObject);

			// Set repeat count after function call as it may decrement arg1
			newObject->arg1 = arg2;
			break;

		default:
			break;
	}

	newObject->xPosRight = newObject->xPos + newObject->xSize;
	newObject->yPosTop = newObject->xPos + newObject->xSize;

	if (gameWorld->GameState == LOADING)
	{
		printf("\nCreated object type: %d;\n\n", objectID);
	}

	return newObject;
}


void CreateObjectSpriteSet(ObjectController *objectList, int objectID)
{
	if (objectList == NULL)
	{
		return;
	}

	// Check for pre-existing spriteset
	SpriteSet *currentSetPtr;
	currentSetPtr = objectList->startSpriteSetPtr;

	int i = 1;

	if (currentSetPtr != NULL)
	{
		while(currentSetPtr->nextSet != NULL && currentSetPtr->setID != objectID)
		{
			currentSetPtr = currentSetPtr->nextSet;
			i++;
		}

		if (currentSetPtr->setID == objectID)
		{
			return;
		}
	}

	// If no sprite set is present, allocate and create one
	SpriteSet *newSet = malloc(sizeof(SpriteSet));

	if (newSet == NULL)
	{
		printf("Failed to allocate memory for new sprite set.\n");
		fflush(stdout);
		return;
	}

	if (currentSetPtr == NULL && i == 1)
	{
		objectList->startSpriteSetPtr = newSet;
	}
	else
	{
		currentSetPtr->nextSet = newSet;
	}

	newSet->prevSet = currentSetPtr;
	newSet->nextSet = NULL;
	newSet->firstSprite = NULL;
	newSet->lastSprite = NULL;
	newSet->setID = objectID;
	newSet->spriteCount = 0;
	objectList->spriteSetCount = i + 1;


	// Fill sprite set with sprites
	switch (objectID)
	{
		case PARTICLE:
		{
			LoadParticleSprites(newSet);
		} break;

		case RIGHT_SLOPE:
		case LEFT_SLOPE:
		{
			loadObjectSprite("Grass_Angle_Small", newSet, SCALE);
			loadObjectSprite("Grass_Angle_Medium", newSet, SCALE);
			loadObjectSprite("Grass_Angle_Large", newSet, SCALE);
		} break;

		case SPRING:
			loadObjectSprite("Spring", newSet, SINGLE);
			break;

		case SOLID_BLOCK:
			loadObjectSprite("Grass_Block", newSet, TILE_FAST);
			break;

		default:
		{
			loadObjectSprite("OBJ_Missing", newSet, TILE);
		} break;
	}

	return;
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



Object* DefineMovingPlatform(Object *inputObject, int objectID, int xPos, int yPos, int bound1, int bound2, int speed, int timer)
{
	if (inputObject == NULL)
	{
		return NULL;
	}

	// Default settings
	inputObject->layer = BACKGROUND;
	inputObject->ySize = Y_TILESCALE;
	inputObject->xSize = X_TILESCALE * 3;
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
	inputObject->solid = 1;


	switch (objectID)
	{

	default:
		break;
	}

	return inputObject;
}


Object* AddFlagObject(World *gameWorld, Flags flagID, int xPos, int yPos, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	if (gameWorld == NULL || gameWorld->objectList == NULL)
	{
		return NULL;
	}

	ObjectController *objectList = gameWorld->objectList;

	Object *newObject;
	newObject = createNewObject(objectList, xPos, yPos, LEVEL_FLAG_OBJ);

	if (newObject == NULL)
	{
		return NULL;
	}

	// Default settings
	newObject->objectRenderMode = DO_NOT_RENDER;
	newObject->ySize = 0;
	newObject->xSize = 0;
	newObject->arg1 = arg1;
	newObject->arg2 = arg2;
	newObject->arg3 = arg3;
	newObject->arg4 = arg4;
	newObject->arg5 = arg5;
	newObject->currentSprite = 0;
	newObject->spriteBuffer = NULL;
	newObject->solid = 0;
	newObject->layer = LEVELFLAGS;

	printf("\nCreated object flag %d;\n\n", flagID);

	// Set Flag parameters
	switch (flagID)
	{

	default:
		break;
	}

	newObject->xPosRight = newObject->xPos + newObject->xSize;
	newObject->yPosTop = newObject->xPos + newObject->xSize;

	SetDrawPriorityToFront(objectList, newObject);
	// Counter intuitively, by setting draw priority to front, the flag is placed at the end of the object list
	// This is to help with efficiency somewhat as most searches begin from the start of the list as opposed to the end;
	// Flag objects should never be directly accessed/affected by an object unless you are designing a special circumstance

	return newObject;
}


void deleteAllObjects(ObjectController *objectList)
{
	if (objectList == NULL )
	{
		return;
	}
	
	Object *currentObject;
  	currentObject = objectList->firstObject;

	while (currentObject != NULL)
	{
		deleteObject(objectList, &currentObject);
	}
	
	objectList->objectCount = 0;
	objectList->firstObject = NULL;
	objectList->lastObject = NULL;

	return;
}


int switchObjectSprite(int spriteID, Object *inputObject, ObjectController *objectList)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	if (inputObject->spriteBuffer != NULL && inputObject->spriteBuffer->spriteID == spriteID)
	{
		inputObject->currentSprite = spriteID;
		return EXECUTION_UNNECESSARY;
	}


	// Find correct sprite set
	SpriteSet *currentSet;
	currentSet = objectList->startSpriteSetPtr;

	if (currentSet == NULL)
	{
		printf("No sprite sets found\n");
		fflush(stdout);
		return -1;
	}

	while (currentSet->setID != inputObject->objectID && currentSet->nextSet != NULL)
	{
		currentSet = currentSet->nextSet;
	}

	if (currentSet->setID != inputObject->objectID)
	{
		printf("Missing sprite set for object %d\n", inputObject->objectID);
		fflush(stdout);
		return -1;
	}

	if (currentSet->spriteCount < 1 || currentSet->firstSprite == NULL)
	{
		printf("Sprite set does not contain sprites for object %d\n", inputObject->objectID);
		fflush(stdout);
		currentSet->spriteCount = 0;
		return -1;
	}

	if (spriteID > currentSet->spriteCount || spriteID < 1)
	{
		printf("Invalid spriteID (%d) for set %d\n", spriteID, currentSet->setID);
		fflush(stdout);
		return INVALID_DATA;
	}


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
		printf("Could not find sprite %d for object %d\n", spriteID, inputObject->objectID);
		fflush(stdout);
		return -1;
	}

	inputObject->spriteBuffer = currentSprite;
	inputObject->currentSprite = spriteID;
		
	return 0;
}


int switchObjectSpriteName(char spriteName[], Object *inputObject, ObjectController *objectList)
{
	// Find correct sprite set
	SpriteSet *currentSet;
	currentSet = objectList->startSpriteSetPtr;

	if (currentSet == NULL)
	{
		printf("No sprite sets found\n");
		fflush(stdout);
		return -1;
	}

	while (currentSet->setID != inputObject->objectID && currentSet->nextSet != NULL)
	{
		currentSet = currentSet->nextSet;
	}

	if (currentSet->setID != inputObject->objectID)
	{
		printf("Missing sprite set for object %d\n", inputObject->objectID);
		fflush(stdout);
		return -1;
	}

	if (currentSet->spriteCount < 1 || currentSet->firstSprite == NULL)
	{
		printf("Sprite set does not contain sprites for object %d\n", inputObject->objectID);
		fflush(stdout);
		currentSet->spriteCount = 0;
		return -1;
	}


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
		printf("Could not find sprite %s for object %d\n", spriteName, inputObject->objectID);
		fflush(stdout);
		return -1;
	}

	inputObject->spriteBuffer = currentSpritePtr;
	inputObject->currentSprite = i;

	return 0;
}


Object* createNewObject(ObjectController *objectList, int xPos, int yPos, int objectID)
{
	Object *currentObject;
	currentObject = objectList->firstObject;


	Object *newObject = malloc(sizeof(Object));

	if (newObject == NULL)
	{
		printf("\nError: Could not allocate memory for new object.\n\n");
		fflush(stdout);
		return NULL;
	}


	int i = 1;

	if (currentObject != NULL)
	{
		while (currentObject->nextObject != NULL && i < objectList->objectCount)
		{
			currentObject = currentObject->nextObject;
			i++;
		}

		currentObject->nextObject = newObject;
	}
	else
	{
		objectList->firstObject = newObject;
	}

	objectList->lastObject = newObject;


	objectList->objectCount = i + 1;

	newObject->nextObject = NULL;
	newObject->prevObject = currentObject;
	newObject->xPos = (double)abs(xPos - (xPos % X_TILESCALE));
	newObject->yPos = (double)abs(yPos - (yPos % Y_TILESCALE));
	newObject->yVel = 0.0;
	newObject->xVel = 0.0;

	newObject->objectID = objectID;
	newObject->currentAnimation = 0;
	newObject->animationTick = 0;
	newObject->spriteBuffer = NULL;
	newObject->xFlip = 1;
	newObject->yFlip = 1;
	newObject->State = DEFAULT;

	return newObject;
}



void deleteObject(ObjectController *objectList, Object **input)
{
	Object *tempObject;
	tempObject = *input;

	if (*input == NULL)
	{
		return;
	}

	Object *prevObject;
	prevObject = (*input)->prevObject;

	(*input) = (*input)->nextObject;


	if ((*input) != NULL)
	{
		(*input)->prevObject = prevObject;
	}
	else
	{
		objectList->lastObject = prevObject;
	}

	if (prevObject != NULL)
	{
		prevObject->nextObject = (*input);
	}
	else
	{
		objectList->firstObject = (*input);
	}


	free(tempObject);

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


int UnmarkObjectForDeletion(Object *inputObject)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	if (inputObject->State != TO_BE_DELETED || inputObject->objectID >= UNDEFINED_OBJECT ||  inputObject->objectID < LEVEL_FLAG_OBJ)
	{
		return INVALID_DATA;
	}

	inputObject->State = DEFAULT;

	return 0;
}


void IncrementDrawPriority(ObjectController *objectList, Object *input)
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
		objectList->firstObject = nextPtr;
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
		objectList->lastObject = input;
	}

	return;
}


void DecrementDrawPriority(ObjectController *objectList, Object *input)
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
		objectList->lastObject = prevPtr;
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
		objectList->firstObject = input;
	}

	return;
}


void SetDrawPriorityToFront(ObjectController *objectList, Object *input)
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
		objectList->firstObject = nextPtr;
	}
	else
	{
		prevPtr->nextObject = nextPtr;
	}
	
	if (objectList->lastObject != NULL)
	{
		objectList->lastObject->nextObject = input;
	}

	input->prevObject = objectList->lastObject;
	objectList->lastObject = input;
	input->nextObject = NULL;

	return;
}


void SetDrawPriorityToBack(ObjectController *objectList, Object *input)
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
		objectList->lastObject = prevPtr;
	}
	else
	{
		nextPtr->prevObject = prevPtr;
	}
	
	if (objectList->firstObject != NULL)
	{
		objectList->firstObject->prevObject = input;
	}

	input->nextObject = objectList->firstObject;
	objectList->firstObject = input;
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

	if (gameWorld->GamePaused > 0 || gameWorld->GameState != GAMEPLAY)
	{
		return ACTION_DISABLED;
	}

	ObjectController *objectList = gameWorld->objectList;

	if (objectList == NULL || objectList->firstObject == NULL)
	{
		return MISSING_DATA;
	}

	Object *currentObject;
	currentObject = objectList->firstObject;

	int i = objectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		i--;

		if (currentObject->State == PAUSE_BEHAVIOUR)
		{
			currentObject = currentObject->nextObject;
			continue;
		}


		ObjectBehaviour(gameWorld, currentObject);


		if (currentObject == NULL)
		{
			return MISSING_DATA;
		}


		if (currentObject->State == TO_BE_DELETED)
		{
			// If object has been deleted, pointer will be incremented and so the rest of the iteration is skipped
			deleteObject(objectList, &currentObject);
			continue;
		}
		
		// Move object
		moveObjectX(currentObject, gameWorld->Player);

		moveObjectY(currentObject, gameWorld->Player);
		
		// Assign Sprite
		switchObjectSprite(currentObject->currentSprite, currentObject, objectList);

		currentObject = currentObject->nextObject;
		
	}

	return LEMON_SUCCESS;
}


int ObjectBehaviour(World *gameWorld, Object *inputObject)
{
	if (gameWorld == NULL || gameWorld->objectList == NULL)
	{
		return MISSING_DATA;
	}

	PlayerData *player = gameWorld->Player;


	switch (inputObject->objectID)
	{
		case PARTICLE:
			UpdateParticle(gameWorld, inputObject);
			break;


		case MOVING_PLATFORM_HOR:
		{
			UpdateHorizontalPlatform(player, inputObject);
		} break;


		case MOVING_PLATFORM_VER:
		{
			UpdateVerticalPlatform(player, inputObject);
		} break;


		case COIN:
		{
			// temp
			double ObjYPos = inputObject->yPos;
			double ObjYPos2 = inputObject->yPos + inputObject->ySize;
			double ObjXPos = inputObject->xPos;
			double ObjXPos2 = inputObject->xPos + inputObject->xSize;

			if (boxOverlapsPlayer(player, ObjXPos, ObjXPos2, ObjYPos, ObjYPos2) == 1)
			{
				AddObject(gameWorld, PARTICLE, ObjXPos, ObjYPos, SPARKLE, 1, 0, 0, 0);
				MarkObjectForDeletion(inputObject);
				player->coinCount++;
				LemonPlaySound("Coin_Collect", "Objects", OBJECT_SFX, 0.8);
			}

		} break;


		case SPRING:
		{
			// temp
			double ObjYPos = inputObject->yPos;
			double ObjYPos2 = inputObject->yPos + inputObject->ySize;
			double ObjXPos = inputObject->xPos;
			double ObjXPos2 = inputObject->xPos + inputObject->xSize;

			if (inputObject->arg5 < 1 && player->yVelocity < -1.0 && boxOverlapsPlayer(player, ObjXPos, ObjXPos2, ObjYPos, ObjYPos2) == 1)
			{
				player->yVelocity = (double)inputObject->arg1;
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
			UpdateVerticalGate(inputObject, gameWorld->objectList, player);
		} break;


		case HORIZONTAL_GATE:
		{
			UpdateHorizontalGate(inputObject, gameWorld->objectList, player);
		} break;


		case GATE_SWITCH_TIMED:
		case GATE_SWITCH:
		{
			UpdateGateSwitch(player, inputObject);
			inputObject->yPos = 64 + (32 * inputObject->arg3);
		} break;


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


	customParticleBehaviour(GameWorld, particle);


	// If repeat count is reached or animationTick exceeds maximum lifetime, mark for deletion
	if (particle->arg3 < 1 && particle->arg1 < 1)
	{
		MarkObjectForDeletion(particle);
	}

	if (particle->arg3 > 0 && particle->animationTick > particle->arg3)
	{
		MarkObjectForDeletion(particle);
	}

	return 0;
}


int customParticleBehaviour(World *GameWorld, Object *particle)
{
	// Custom behaviour
	switch(particle->currentAnimation)
	{
		default:
		break;
	}

	return 0;
}


int LoopParticleAnimation(Object *particle)
{
	if (particle->arg2 < 1 || particle->arg2 > 999)
	{
		particle->arg2 = 3;
	}

	if (particle->arg1 < 1 || particle->animationTick % particle->arg2 != 0)
	{
		return EXECUTION_UNNECESSARY;
	
	}

	int firstSprite = 1;
	int lastSprite = 1;
	int animateType = 1;

	// Add a new case whenever you make a new particle here
	switch(particle->currentAnimation)
	{
		case SPARKLE:
		lastSprite = 7;
		break;

		default:
		break;
	}

	if (lastSprite < firstSprite)
	{
		return INVALID_DATA;
	}

	if (particle->arg2 < 1 || particle->arg2 > 999)
	{
		particle->arg2 = 3;
	}


	// Add a new case for specific animation sequences
	switch (animateType)
	{
		case 2:
		{
			int onOddLoop = 1 - ( particle->animationTick / ((lastSprite - firstSprite + 1) * particle->arg2) ) % 2;

			if (onOddLoop == 1)
			{
				particle->currentSprite++;
			}
			else
			{
				particle->currentSprite--;
			}

			if (particle->currentSprite > lastSprite || particle->currentSprite < firstSprite)
			{
				particle->arg1--;

				if (onOddLoop == 1)
				{
					particle->currentSprite = lastSprite;
				}
				else
				{
					particle->currentSprite = firstSprite;
				}
			}

		} break;


		case -2:
		{
			int onOddLoop = 1 - ( particle->animationTick / ((lastSprite - firstSprite + 1) * particle->arg2) ) % 2;

			if (onOddLoop == 1)
			{
				particle->currentSprite--;
			}
			else
			{
				particle->currentSprite++;
			}


			if (particle->currentSprite > lastSprite || particle->currentSprite < firstSprite)
			{
				particle->arg1--;

				if (onOddLoop == 1)
				{
					particle->currentSprite = firstSprite;
				}
				else
				{
					particle->currentSprite = lastSprite;
				}
			}

		} break;


		case -1:
		{
			particle->currentSprite--;

			if (particle->currentSprite > lastSprite || particle->currentSprite < firstSprite)
			{
				particle->arg1--;

				particle->currentSprite = lastSprite;
			}

		} break;

		default:
		{
			particle->currentSprite++;

			if (particle->currentSprite > lastSprite || particle->currentSprite < firstSprite)
			{
				particle->arg1--;

				particle->currentSprite = firstSprite;
			}

		} break;
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

	int ObjXPos = gateSwitch->xPos;
	int ObjXPos2 = gateSwitch->xPos + gateSwitch->xSize - 1;
	int ObjYPos = gateSwitch->yPos;
	int ObjYPos2 = gateSwitch->yPos + gateSwitch->ySize - 1;

	if (boxOverlapsPlayer(player, ObjXPos, ObjXPos2, ObjYPos, ObjYPos2) == 1)
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
		gateSwitch->animationTick -= deltaTime;

		if (gateSwitch->animationTick < 1.0)
		{
			gateSwitch->arg3 = 0;
			gateSwitch->animationTick = gateSwitch->arg5;
		}
	}

	return 0;
}


int UpdateVerticalGate(Object *gate, ObjectController *objectList, PlayerData *player)
{
	// arg1 = door ID
	// arg2 = door open/close (0/1)
	// arg3 = closed gate y position 
	// arg4 = speed
	
	if (objectList == NULL)
	{
		return -1;
	}


	// Evaluate state of gate
	gateControl(gate, objectList);


	int closedPosition = gate->arg3;
	int speed = gate->arg4;

	// Animation control
	switch (gate->currentAnimation)
	{
		case 0:
		{
			gate->yPos = closedPosition;
			gate->yVel = 0.0;

			if (gate->arg2 == 1)
			{
				gate->currentAnimation = 1;
				gate->animationTick = 0;
				LemonPlaySound("GateOpen", "Objects", 4, 1.0);
			}
		
		} break;

		case 1:
		{
			gate->yVel += 0.1 * (speed/abs(speed));

			if (fabs(gate->yVel) > abs(speed))
			{
				gate->yVel = speed;
			}


			if (boxOverlapsPlayer(player, gate->xPos, gate->xPos + gate->xSize, gate->yPos + (gate->yVel * deltaTime), gate->yPos + gate->ySize + (gate->yVel * deltaTime)) == 1)
			{
				gate->yVel = 0.0;
			}

			if ( (speed < 0 && gate->yPos + (gate->yVel * deltaTime) < closedPosition - gate->ySize) || (speed > 0 && gate->yPos + (gate->yVel * deltaTime) > closedPosition + gate->ySize) )
			{
				gate->currentAnimation = 2;
				gate->animationTick = 0;
				gate->yVel = 0.0;
				SetObjectYPosition(gate, closedPosition + (gate->ySize * (speed/abs(speed))), player);
			}

		} break;


		case 2:
		{
			gate->yPos = closedPosition + (gate->ySize * (speed/abs(speed)));
			gate->yVel = 0.0;

			if (gate->arg2 == 0)
			{
				gate->currentAnimation = 3;
				gate->animationTick = 0;
				LemonPlaySound("GateClose", "Objects", 4, 1.0);
			}

		} break;


		case 3:
		{
			gate->yVel -= 0.1 * (speed/abs(speed));

			if (fabs(gate->yVel) > abs(speed))
			{
				gate->yVel = -speed;
			}

			if (boxOverlapsPlayer(player, gate->xPos, gate->xPos + gate->xSize, gate->yPos + (gate->yVel * deltaTime), gate->yPos + gate->ySize + (gate->yVel * deltaTime)) == 1)
			{
				gate->yVel = 0.0;
			}


			if ( (speed > 0 && gate->yPos < closedPosition) || (speed < 0 && gate->yPos > closedPosition) )
			{
				gate->currentAnimation = 0;
				gate->animationTick = 0;
				gate->yVel = 0.0;
				SetObjectYPosition(gate, closedPosition, player);
			}

		} break;
	}


	return 0;
}


int UpdateHorizontalGate(Object *gate, ObjectController *objectList, PlayerData *player)
{
	// arg1 = door ID
	// arg2 = door open/close (0/1)
	// arg3 = closed gate y position 
	// arg4 = speed
	
	if (objectList == NULL)
	{
		return -1;
	}


	// Evaluate state of gate
	gateControl(gate, objectList);


	int closedPosition = gate->arg3;
	int speed = gate->arg4;

	// Animation control
	switch (gate->currentAnimation)
	{
		case 0:
		{
			gate->yPos = closedPosition;
			gate->yVel = 0.0;

			if (gate->arg2 == 1)
			{
				gate->currentAnimation = 1;
				gate->animationTick = 0.0;
				LemonPlaySound("GateOpen", "Objects", 4, 1.0);
			}
		
		} break;

		case 1:
		{
			gate->xVel += 0.1 * (speed/abs(speed));

			if (fabs(gate->xVel) > abs(speed))
			{
				gate->xVel = speed;
			}


			if (boxOverlapsPlayer(player, gate->xPos + (gate->xVel * deltaTime), gate->xPos + gate->xSize + (gate->xVel * deltaTime), gate->yPos, gate->yPos + gate->ySize) == 1)
			{
				gate->xVel = 0.0;
			}


			if ( (speed < 0 && gate->xPos < closedPosition - gate->xSize) || (speed > 0 && gate->xPos > closedPosition + gate->xSize) )
			{
				gate->currentAnimation = 2;
				gate->animationTick = 0;
				gate->xVel = 0.0;
				gate->xPos = closedPosition + (gate->ySize * (speed/abs(speed)));
			}

		} break;


		case 2:
		{
			gate->xPos = closedPosition + (gate->ySize * (speed/abs(speed)));
			gate->xVel = 0.0;

			if (gate->arg2 == 0)
			{
				gate->currentAnimation = 3;
				gate->animationTick = 0;
				LemonPlaySound("GateClose", "Objects", 4, 1.0);
			}

		} break;


		case 3:
		{
			gate->xVel -= 0.1 * (speed/abs(speed));

			if (fabs(gate->xVel) > abs(speed))
			{
				gate->xVel = speed * (speed/abs(speed));
			}


			if (boxOverlapsPlayer(player, gate->xPos + (gate->xVel * deltaTime), gate->xPos + gate->xSize + (gate->xVel * deltaTime), gate->yPos, gate->yPos + gate->ySize) == 1)
			{
				gate->xVel = 0.0;
			}


			if ( (speed > 0 && gate->xPos < closedPosition) || (speed < 0 && gate->xPos > closedPosition) )
			{
				gate->currentAnimation = 0;
				gate->animationTick = 0;
				gate->xVel = 0.0;
				gate->xPos = closedPosition;
			}

		} break;
	}


	return 0;
}


int gateControl(Object *gate, ObjectController *objectList)
{
	Object *currentObject;
	currentObject = objectList->firstObject;

	if (gate == NULL || currentObject == NULL)
	{
		return -1;
	}


	int onOrOff = -1;

	// Run through object list
	while (currentObject != NULL)
	{	

		// If a matching switch is found, execute switch/gate logic
		if ((currentObject->objectID == GATE_SWITCH || currentObject->objectID == GATE_SWITCH_TIMED) && gate->arg1 == currentObject->arg1)
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


int UpdateHorizontalPlatform(PlayerData *player, Object *platform)
{
	double YPos = platform->yPos;
	double YPos2 = platform->yPos + platform->ySize - 1;
	double XPos = platform->xPos;
	double XPos2 = platform->xPos + platform->xSize - 1;

	int leftBound = platform->arg1;
	int rightBound = platform->arg2;
	int maxSpeed = platform->arg3;
	int timer = platform->arg5;

	// Accelerate
	if (platform->arg4 > 0 && platform->xVel < (double)maxSpeed && XPos < (double)rightBound)
	{
		platform->xVel += 0.5 * deltaTime;
	}

	if (platform->arg4 < 0 && platform->xVel > -(double)maxSpeed && XPos > (double)leftBound)
	{
		platform->xVel -= 0.5 * deltaTime;
	}

	// Deccelerate
	if (XPos > (double)rightBound && platform->arg4 > 0 || XPos < (double)leftBound && platform->arg4 < 0)
	{
		platform->xVel *= 0.9;
	}
	
	if (fabs(platform->xVel) < 0.1)
	{
		platform->xVel = 0.0;
	}


	// Wait
	if (platform->arg4 > 0 && fabs(platform->xVel) < 0.5 && XPos > (double)rightBound)
	{
		platform->arg4++;
		if (platform->arg4 > timer)
		{
			platform->arg4 = -1;
		}
	}

	if (platform->arg4 < 0 && fabs(platform->xVel) < 0.5 && XPos < (double)leftBound)
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
	double YPos = platform->yPos;
	double YPos2 = platform->yPos + platform->ySize - 1;
	double XPos = platform->xPos;
	double XPos2 = platform->xPos + platform->xSize - 1;

	int bottomBound = platform->arg1;
	int topBound = platform->arg2;
	int maxSpeed = platform->arg3;
	int timer = platform->arg5;

	// Accelerate
	if (platform->arg4 > 0 && platform->yVel < maxSpeed && YPos < (double)topBound)
	{
		platform->yVel += 0.75 * deltaTime;
	}

	if (platform->arg4 < 0 && platform->yVel > -maxSpeed && YPos > (double)bottomBound)
	{
		platform->yVel -= 0.75 * deltaTime;
	}

	// Deccelerate
	if (YPos > (double)topBound && platform->arg4 > 0 || YPos < (double)bottomBound && platform->arg4 < 0)
	{
		platform->yVel *= 0.9;
	}

	if (fabs(platform->yVel) < 0.1)
	{
		platform->yVel = 0.0;
	}

	// Wait
	if (platform->arg4 > 0 && fabs(platform->yVel) < 0.5 && YPos > (double)topBound)
	{
		platform->arg4++;
		if (platform->arg4 > timer)
		{
			platform->arg4 = -1;
		}
	}

	if (platform->arg4 < 0 && fabs(platform->yVel) < 0.5 && YPos < (double)bottomBound)
	{
		platform->arg4--;
		if (platform->arg4 < -timer)
		{
			platform->arg4 = 1;
		}
	}

	return 0;
}


int boxOverlapsPlayer(PlayerData *player, double X1, double X2, double Y1, double Y2)
{
	return !((int)player->xPos >= (int)X2 || (int)(player->xPos + PLAYERWIDTH) <= (int)X1 || (int)player->yPos >= (int)Y2 || (int)(player->yPos + PLAYERHEIGHT) <= (int)Y1);
}


int boxOverlapsPlayerFeet(PlayerData *player, double X1, double X2, double Y1, double Y2)
{
	return !(player->xPos >= X2 || (player->xPos + PLAYERWIDTH) <= X1 || (player->yPos - 8) >= Y2 || (player->yPos + 8) <= Y1);
}


int rightSlopeOverlapsPlayer(PlayerData *player, Object *inputObject)
{
	return 0;

	double slopeFloor = ((player->xPos + PLAYERWIDTH - inputObject->xPos) * ((double)inputObject->ySize/(double)inputObject->xSize));

	if (slopeFloor > inputObject->ySize)
	{
		slopeFloor = inputObject->ySize;
	}

	double relativePlayerPos = player->yPos - inputObject->yPos;

	return !(relativePlayerPos >= slopeFloor || relativePlayerPos < 0 || (int)player->xPos >= (int)(inputObject->xPos + inputObject->xSize) || (int)(player->xPos + PLAYERWIDTH) <= (int)inputObject->xPos);
}


int leftSlopeOverlapsPlayer(PlayerData *player, Object *inputObject)
{
	return 0;

	double slopeFloor = ((inputObject->xSize - (player->xPos - inputObject->xPos)) * ((double)inputObject->ySize/(double)inputObject->xSize));

	if (slopeFloor > inputObject->ySize)
	{
		slopeFloor = inputObject->ySize;
	}

	double relativePlayerPos = player->yPos - inputObject->yPos;

	return !(relativePlayerPos >= slopeFloor || relativePlayerPos < 0 || (int)player->xPos >=  (int)(inputObject->xPos + inputObject->xSize) || (int)(player->xPos + PLAYERWIDTH) < (int)inputObject->xPos);
}


int OverlapsObjectType(ObjectController *objectList, int overlapObjectID, Object *inputObject)
{
	if (objectList == NULL || objectList->firstObject == NULL)
	{
		return -1;
	}

	Object *currentObject;
	currentObject = objectList->firstObject;

	int i = objectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		if (currentObject->layer == inputObject->layer && currentObject->objectID == overlapObjectID && currentObject != inputObject)
		{
			if (OverlapsObject(inputObject, currentObject) == 1)
			{
				return 1;
			}
		}

		currentObject = currentObject->nextObject;

		i++;
	}

	return 0;
}


int OverlapsObjectSolid(ObjectController *objectList, int solidID, Object *inputObject)
{
	if (objectList == NULL || objectList->firstObject == NULL)
	{
		return -1;
	}

	Object *currentObject;
	currentObject = objectList->firstObject;

	int i = objectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		if (currentObject->layer == inputObject->layer && currentObject->solid == solidID && currentObject != inputObject)
		{
			if (OverlapsObject(inputObject, currentObject) == 1)
			{
				return 1;
			}
		}

		currentObject = currentObject->nextObject;

		i++;
	}

	return 0;
}


int OverlapsObjectAllSolids(ObjectController *objectList, Object *inputObject)
{
	if (objectList == NULL || objectList->firstObject == NULL)
	{
		return -1;
	}

	Object *currentObject;
	currentObject = objectList->firstObject;

	int i = objectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		if (currentObject->layer == inputObject->layer && currentObject->solid > 0 && currentObject != inputObject)
		{
			if (OverlapsObject(inputObject, currentObject) == 1)
			{
				return 1;
			}
		}

		currentObject = currentObject->nextObject;

		i++;
	}

	return 0;
}


// check if two objects are overlapping according to their shape
int OverlapsObject(Object *inputObject, Object *otherObject)
{
	int xOverlap, yOverlap;
	int inputXRight = inputObject->xPos + inputObject->xSize;
	int inputYTop = inputObject->yPos + inputObject->ySize;
	int otherXRight = otherObject->xPos + otherObject->xSize;
	int otherYTop = otherObject->yPos + otherObject->ySize;

	double slope;

	switch (inputObject->solid)
	{
		case 2:
		{
			double inputSlope = ((double)inputObject->ySize/(double)inputObject->xSize);
			
			if ((otherXRight - inputObject->xPos) * inputSlope < inputObject->ySize)
			{
				inputYTop = ((otherXRight - inputObject->xPos) * inputSlope) + inputObject->yPos;
			}
		} break;


		case 3:
		{
			double inputSlope = ((double)inputObject->ySize/(double)inputObject->xSize);
			
			if ((inputXRight - otherObject->xPos) * inputSlope < inputObject->ySize)
			{
				inputYTop = ((inputXRight - otherObject->xPos) * inputSlope) + inputObject->yPos;
			}
		} break;


		default:
		break;
	}


	switch (otherObject->solid)
	{
		case 2:
		slope = ((double)otherObject->ySize/(double)otherObject->xSize);

		xOverlap = !(inputObject->xPos >= otherXRight || (inputXRight) <= otherObject->xPos);
		yOverlap = !((inputObject->yPos - otherObject->yPos) >= ((inputXRight - otherObject->xPos) * slope) || inputObject->yPos >= otherYTop || inputYTop <= otherObject->yPos);
		break;

		case 3:
		slope = ((double)otherObject->ySize/(double)otherObject->xSize);

		xOverlap = !(inputObject->xPos >= otherXRight || (inputXRight) <= otherObject->xPos);
		yOverlap = !(((inputObject->yPos - otherObject->yPos) <= ((otherXRight - inputObject->xPos) * slope)) || inputObject->yPos > otherYTop || inputYTop < otherObject->yPos);
		break;

		default:
		xOverlap = !(inputObject->xPos >= otherXRight || inputXRight <= otherObject->xPos);
		yOverlap = !(inputObject->yPos >= otherYTop || inputYTop <= otherObject->yPos);
		break;
	}
	
	if (xOverlap == 1 && yOverlap == 1)
	{
		return 1;
	}

	return 0;
}


int ResolvePlayerToObjectCollisionX(Object *inputObject, PlayerData *player, double prevObjXPos, double prevObjXPosRight)
{
	double ObjXPos = inputObject->xPos;
	double ObjXPosRight = inputObject->xPosRight;
	double ObjYPos = inputObject->yPos;
	double ObjYPosTop = inputObject->yPos + inputObject->ySize;

	double prevXPosBuffer = prevObjXPos;
	double prevXPosRightBuffer = prevObjXPosRight;


	int result = 0;

	switch(inputObject->solid)
	{
		case 2:
			ObjXPos = ObjYPos / ((double)inputObject->ySize/(double)inputObject->xSize);
			prevXPosBuffer = prevObjXPosRight;
			result = rightSlopeOverlapsPlayer(player, inputObject);
			break;

		case 3:
			ObjXPosRight = inputObject->xSize - ( ObjYPos / ((double)inputObject->ySize/(double)inputObject->xSize) );
			prevXPosRightBuffer = prevObjXPos;
			result = leftSlopeOverlapsPlayer(player, inputObject);
			break;

		case 1:
			result = boxOverlapsPlayer(player, ObjXPos, ObjXPosRight, ObjYPos, ObjYPosTop);
			break;

		default:
			break;
	}


	if (result == 1)
	{
		if (player->xPos < ((prevXPosBuffer + prevXPosRightBuffer) / 2.0) )
		{
			player->xPos = ObjXPos - PLAYERWIDTH;
		}
		else
		{
			player->xPos = ObjXPosRight;
		}

		return 1;
	}

	return 0;
}


int ResolvePlayerToObjectCollisionY(Object *inputObject, PlayerData *player, double prevObjYPos, double prevObjYPosTop)
{
	double ObjXPos = inputObject->xPos;
	double ObjXPosRight = inputObject->xPos + inputObject->xSize;
	double ObjYPos = inputObject->yPos;
	double ObjYPosTop = inputObject->yPosTop;

	double prevYPosBuffer = prevObjYPos;
	double prevYPosTopBuffer = prevObjYPosTop;


	int result = 0;

	switch(inputObject->solid)
	{
		case 2:
			ObjYPosTop = ((player->xPos + PLAYERWIDTH - inputObject->xPos) * ((double)inputObject->ySize/(double)inputObject->xSize));
			prevYPosTopBuffer = prevObjYPos;
			result = rightSlopeOverlapsPlayer(player, inputObject);
			break;

		case 3:
			ObjYPosTop = ((inputObject->xSize - (player->xPos - inputObject->xPos)) * ((double)inputObject->ySize/(double)inputObject->xSize));
			prevYPosTopBuffer = prevObjYPos;
			result = leftSlopeOverlapsPlayer(player, inputObject);
			break;

		case 4:
			result = boxOverlapsPlayer(player, ObjXPos, ObjXPosRight, ObjYPos, ObjYPosTop) && player->yVelocity < 0.0 && player->yPos > prevObjYPosTop - (inputObject->ySize >> 1);
			break;

		case 1:
			result = boxOverlapsPlayer(player, ObjXPos, ObjXPosRight, ObjYPos, ObjYPosTop);
			break;

		default:
			break;
	}
	

	if (result == 1)
	{
		if (player->yPos < ((prevYPosBuffer + prevYPosTopBuffer) / 2.0) )
		{
			player->yPos = ObjYPos - PLAYERHEIGHT;
		}
		else
		{
			player->yPos = ObjYPosTop; 
		}

		return 1;
	}

	return 0;
}


int moveObjectX(Object *inputObject, PlayerData *player)
{
	if (fabs(inputObject->xVel) < 0.1)
	{
		inputObject->xVel = 0.0;
		return EXECUTION_UNNECESSARY;
	}


	double prevObjXPos = inputObject->xPos;
	double prevObjXPosRight = inputObject->xPos + inputObject->xSize;

	inputObject->xPos += (inputObject->xVel * deltaTime);
	inputObject->xPosRight = inputObject->xPos + inputObject->xSize;


	if (player == NULL)
	{
		return MISSING_DATA;
	}


	int result = ResolvePlayerToObjectCollisionX(inputObject, player, prevObjXPos, prevObjXPosRight);

	if (result == 1)
	{
		return 0;
	}


	switch(inputObject->solid)
	{
		case 4:
		case 1:
			result = (boxOverlapsPlayerFeet(player, prevObjXPos, prevObjXPosRight, inputObject->yPos, inputObject->yPos + inputObject->ySize + 2.0) == 1 && player->yVelocity < 1.0);
			break;

		default:
			result = 0;
			break;
	}

	if (result == 1)
	{
		int pixelDifference = (int)inputObject->xPos - (int)(inputObject->xPos - (inputObject->xVel * deltaTime));
		player->xPos += pixelDifference;
	}	


	return 0;
}



int moveObjectY(Object *inputObject, PlayerData *player)
{
	if (fabs(inputObject->yVel) < 0.1)
	{
		inputObject->yVel = 0.0;
		return EXECUTION_UNNECESSARY;
	}

	double prevObjYPos = inputObject->yPos;
	double prevObjYPosTop = inputObject->yPos + inputObject->ySize;

	inputObject->yPos += (inputObject->yVel * deltaTime);
	inputObject->yPosTop = inputObject->yPos + inputObject->ySize;


	if (player == NULL)
	{
		return MISSING_DATA;
	}

	int result = ResolvePlayerToObjectCollisionY(inputObject, player, prevObjYPos, prevObjYPosTop);

	if (result == 1)
	{
		return 0;
	}


	switch(inputObject->solid)
	{
		case 4:
		case 1:
			result = (boxOverlapsPlayerFeet(player, inputObject->xPos, inputObject->xPos + inputObject->xSize, prevObjYPos, prevObjYPosTop + 2.0 ) == 1 && player->yVelocity < 0.1);
			break;

		default:
			result = 0;
			break;
	}

	if (result == 1)
	{
		int pixelDifference = (int)inputObject->yPos - (int)(inputObject->yPos - (inputObject->yVel * deltaTime));
		player->yPos += pixelDifference;
	}	

	return 0;
}


int SetObjectXPosition(Object *inputObject, double newXPos, PlayerData *Player)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	if (fabs(inputObject->xPos - newXPos) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}


	double prevObjXPos = inputObject->xPos;
	double prevObjXPosRight = inputObject->xPos + inputObject->xSize;

	inputObject->xPos = newXPos;
	inputObject->xPosRight = newXPos = inputObject->xSize;

	if (Player == NULL || inputObject->solid == 0)
	{
		return MISSING_DATA;
	}

	ResolvePlayerToObjectCollisionX(inputObject, Player, prevObjXPos, prevObjXPosRight);

	return 0;
}


//Method for setting an object's position to avoid incorrect collision with player
int SetObjectYPosition(Object *inputObject, double newYPos, PlayerData *Player)
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	if (fabs(inputObject->yPos - newYPos) < 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}


	double prevObjYPos = inputObject->yPos;
	double prevObjYPosTop = inputObject->yPos + inputObject->ySize;

	inputObject->yPos = newYPos;
	inputObject->yPosTop = newYPos + inputObject->ySize;

	if (Player == NULL || inputObject->solid == 0)
	{
		return MISSING_DATA;
	}

	ResolvePlayerToObjectCollisionY(inputObject, Player, prevObjYPos, prevObjYPosTop);


	return 0;
}


// Method for centering object size increase and handling player collisions
int ChangeObjectXSizeBy(int change, Object *inputObject, PlayerData *player)
{
	if (inputObject == NULL)
	{
		return -1;
	}

	inputObject->xSize += change;
	inputObject->xPos -= (double)(change >> 1);
	inputObject->xPosRight += change >> 1;

	if (player == NULL || change == 0 || inputObject->solid < 1)
	{
		return -1;
	}

	ResolvePlayerToObjectCollisionX(inputObject, player, inputObject->xPos + (change >> 1), inputObject->xPosRight - (change >> 1));
	
	return 0;
}


int ChangeObjectYSizeBy(int change, Object *inputObject, PlayerData *player)
{
	if (inputObject == NULL)
	{
		return -1;
	}

	inputObject->ySize += change;
	inputObject->yPos -= change >> 1;
	inputObject->yPosTop += change >> 1;

	if (player == NULL || change == 0 || inputObject->solid < 1)
	{
		return -1;
	}


	ResolvePlayerToObjectCollisionY(inputObject, player, inputObject->yPos + (change >> 1), inputObject->yPosTop - (change >> 1));
	
	return 0;
}