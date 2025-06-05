#include "gameObjects.h"



Object* AddObject(World *gameWorld, int objectID, int xPos, int yPos, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	if (gameWorld == NULL || gameWorld->ObjectList == NULL)
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
	newObject->objectRenderMode = DEFAULT_TO_SPRITE;
	newObject->currentSprite = 1;
	newObject->layer = MIDDLEGROUND;
	newObject->ObjectBox->ySize = Y_TILESCALE;
	newObject->ObjectBox->xSize = X_TILESCALE;
	newObject->ObjectBox->solid = SOLID;
	newObject->arg1 = arg1;
	newObject->arg2 = arg2;
	newObject->arg3 = arg3;
	newObject->arg4 = arg4;
	newObject->arg5 = arg5;
	

	CreateObjectSpriteSet(ObjectList, objectID);
	


	// Set Object parameters
	switch (objectID)
	{
		case LEVEL_FLAG_OBJ:
			break;


		case SOLID_BLOCK:
			newObject->ObjectBox->xSize = arg1 * X_TILESCALE;
			newObject->ObjectBox->ySize = arg2 * Y_TILESCALE;
			break;


		case RIGHT_SLOPE:
		//Angle: Y = (X * ySize/xSize)
		//Angle: X = (Y / (ySize/xSize))
			newObject->ObjectBox->xSize = arg1;
			newObject->ObjectBox->ySize = arg2;
			newObject->ObjectBox->solid = FLAT_SLOPE_LR;

			if (arg1 + arg2 > 256)
			{
				switchObjectSprite(3, newObject, ObjectList);
			}
			else if (arg1 + arg2 > 128)
			{
				switchObjectSprite(2, newObject, ObjectList);
			}
			break;


		case LEFT_SLOPE:
		//Angle: Y = ((xSize - X) * ySize/xSize)
		//Angle: X = xSize - (Y / (ySize/xSize))
			newObject->ObjectBox->xSize = arg1;
			newObject->ObjectBox->ySize = arg2;
			newObject->ObjectBox->solid = FLAT_SLOPE_RL;
			newObject->xFlip = -1;

			if (arg1 + arg2 > 256)
			{
				switchObjectSprite(3, newObject, ObjectList);
			}
			else if (arg1 + arg2 > 128)
			{
				switchObjectSprite(2, newObject, ObjectList);
			}
			break;


		case JUMP_THRU:
			newObject->ObjectBox->solid = JUMP_THROUGH;
			newObject->ObjectBox->xSize = arg1 * X_TILESCALE;
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
			newObject->ObjectBox->ySize = 4 * Y_TILESCALE;
			newObject->ObjectBox->xSize = 2 * X_TILESCALE;
			break;

		case HORIZONTAL_GATE:
		// Medium sized, horizontal gate
			newObject->arg1 = arg1;
			newObject->arg2 = 0;
			newObject->arg3 = yPos;
			newObject->arg4 = arg2;
			newObject->ObjectBox->ySize = 2 * Y_TILESCALE;
			newObject->ObjectBox->xSize = 4 * X_TILESCALE;
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
			DefineMovingPlatform(newObject, objectID, xPos, yPos, arg1, arg2, arg3, arg4);
			break;


		case PARTICLE:
		// Do not modify! (Unless you wish to alter rendermode)
			newObject->layer = PARTICLES;
			newObject->ObjectBox->solid = UNSOLID;
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

		case UI_ELEMENT:
			newObject->layer = FOREGROUND;
			newObject->ObjectBox->solid = UNSOLID;
			SetDrawPriorityToFront(ObjectList, newObject);
			break;

		default:
			break;
	}

	newObject->ObjectBox->xPosRight = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;
	newObject->ObjectBox->yPosTop = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;

	if (gameWorld->GameState == LOADING)
	{
		printf("\nCreated object type: %d;\n\n", objectID);
	}

	return newObject;
}


Object* createNewObject(ObjectController *ObjectList, int xPos, int yPos, int objectID)
{
	Object *currentObject;
	currentObject = ObjectList->firstObject;


	Object *newObject = malloc(sizeof(Object));

	if (newObject == NULL)
	{
		printf("\nError: Could not allocate memory for new object.\n\n");
		fflush(stdout);
		return NULL;
	}


	newObject->ObjectBox = malloc(sizeof(PhysicsRect));

	if (newObject->ObjectBox == NULL)
	{
		printf("\nError: Could not allocate memory for new object's physics box.\n\n");
		fflush(stdout);
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

	newObject->ObjectBox->xPos = (double)abs(xPos - (xPos % X_TILESCALE));
	newObject->ObjectBox->yPos = (double)abs(yPos - (yPos % Y_TILESCALE));
	newObject->ObjectBox->xPosRight = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;
	newObject->ObjectBox->yPosTop = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;
	newObject->ObjectBox->yVelocity = 0.0;
	newObject->ObjectBox->xVelocity = 0.0;
	newObject->ObjectBox->direction = RADIAN_90;
	newObject->ObjectBox->solid = SOLID;
	newObject->ObjectBox->xSize = 0;
	newObject->ObjectBox->ySize = 0;
	memset(newObject->argArray, 0, sizeof(int) * 16);

	newObject->ObjectID = objectID;
	newObject->currentAnimation = 0;
	newObject->animationTick = 0;
	newObject->spriteBuffer = NULL;
	newObject->layer = MIDDLEGROUND;
	newObject->xFlip = 1;
	newObject->yFlip = 1;
	newObject->State = DEFAULT;

	return newObject;
}


void CreateObjectSpriteSet(ObjectController *ObjectList, int objectID)
{
	if (ObjectList == NULL)
	{
		return;
	}

	// Check for pre-existing spriteset
	SpriteSet *currentSetPtr;
	currentSetPtr = ObjectList->startSpriteSetPtr;

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
	newSet->setID = objectID;
	newSet->spriteCount = 0;
	ObjectList->spriteSetCount = i + 1;


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
	inputObject->ObjectBox->solid = TEST;


	switch (objectID)
	{

	default:
		break;
	}

	return inputObject;
}


Object* AddFlagObject(World *gameWorld, Flags flagID, int xPos, int yPos, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	if (gameWorld == NULL || gameWorld->ObjectList == NULL)
	{
		return NULL;
	}

	ObjectController *ObjectList = gameWorld->ObjectList;

	Object *newObject;
	newObject = createNewObject(ObjectList, xPos, yPos, LEVEL_FLAG_OBJ);

	if (newObject == NULL)
	{
		return NULL;
	}

	// Default settings
	newObject->objectRenderMode = DO_NOT_RENDER;
	newObject->ObjectBox->ySize = 0;
	newObject->ObjectBox->xSize = 0;
	newObject->arg1 = arg1;
	newObject->arg2 = arg2;
	newObject->arg3 = arg3;
	newObject->arg4 = arg4;
	newObject->arg5 = arg5;
	newObject->currentSprite = 0;
	newObject->spriteBuffer = NULL;
	newObject->ObjectBox->solid = UNSOLID;
	newObject->layer = LEVELFLAGS;

	printf("\nCreated object flag %d;\n\n", flagID);

	// Set Flag parameters
	switch (flagID)
	{

	default:
		break;
	}

	newObject->ObjectBox->xPosRight = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;
	newObject->ObjectBox->yPosTop = newObject->ObjectBox->xPos + newObject->ObjectBox->xSize;

	SetDrawPriorityToFront(ObjectList, newObject);
	// Counter intuitively, by setting draw priority to front, the flag is placed at the end of the object list
	// This is to help with efficiency somewhat as most searches begin from the start of the list as opposed to the end;
	// Flag objects should never be directly accessed/affected by an object unless you are designing a special circumstance

	return newObject;
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
		deleteObject(ObjectList, &currentObject);
	}
	
	ObjectList->objectCount = 0;
	ObjectList->firstObject = NULL;
	ObjectList->lastObject = NULL;

	return;
}


int switchObjectSprite(int spriteID, Object *inputObject, ObjectController *ObjectList)
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
	currentSet = ObjectList->startSpriteSetPtr;

	if (currentSet == NULL)
	{
		printf("No sprite sets found\n");
		fflush(stdout);
		return -1;
	}

	while (currentSet->setID != inputObject->ObjectID && currentSet->nextSet != NULL)
	{
		currentSet = currentSet->nextSet;
	}

	if (currentSet->setID != inputObject->ObjectID)
	{
		printf("Missing sprite set for object %d\n", inputObject->ObjectID);
		fflush(stdout);
		return -1;
	}

	if (currentSet->spriteCount < 1 || currentSet->firstSprite == NULL)
	{
		printf("Sprite set does not contain sprites for object %d\n", inputObject->ObjectID);
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
		printf("Could not find sprite %d for object %d\n", spriteID, inputObject->ObjectID);
		fflush(stdout);
		return -1;
	}

	inputObject->spriteBuffer = currentSprite;
	inputObject->currentSprite = spriteID;
		
	return 0;
}


int switchObjectSpriteName(char spriteName[], Object *inputObject, ObjectController *ObjectList)
{
	// Find correct sprite set
	SpriteSet *currentSet;
	currentSet = ObjectList->startSpriteSetPtr;

	if (currentSet == NULL)
	{
		printf("No sprite sets found\n");
		fflush(stdout);
		return -1;
	}

	while (currentSet->setID != inputObject->ObjectID && currentSet->nextSet != NULL)
	{
		currentSet = currentSet->nextSet;
	}

	if (currentSet->setID != inputObject->ObjectID)
	{
		printf("Missing sprite set for object %d\n", inputObject->ObjectID);
		fflush(stdout);
		return -1;
	}

	if (currentSet->spriteCount < 1 || currentSet->firstSprite == NULL)
	{
		printf("Sprite set does not contain sprites for object %d\n", inputObject->ObjectID);
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
		printf("Could not find sprite %s for object %d\n", spriteName, inputObject->ObjectID);
		fflush(stdout);
		return -1;
	}

	inputObject->spriteBuffer = currentSpritePtr;
	inputObject->currentSprite = i;

	return 0;
}


void deleteObject(ObjectController *ObjectList, Object **input)
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


	free(tempObject->ObjectBox);
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

	if (inputObject->State != TO_BE_DELETED || inputObject->ObjectID >= UNDEFINED_OBJECT ||  inputObject->ObjectID < LEVEL_FLAG_OBJ)
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

	if (gameWorld->GamePaused > 0 || gameWorld->GameState != GAMEPLAY)
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
			deleteObject(ObjectList, &currentObject);
			continue;
		}
	
		// Move object
		moveObjectX(currentObject, gameWorld->Player);

		moveObjectY(currentObject, gameWorld->Player);
		
		// Assign Sprite
		switchObjectSprite(currentObject->currentSprite, currentObject, ObjectList);

		currentObject = currentObject->nextObject;
		
	}

	return LEMON_SUCCESS;
}


int ObjectBehaviour(World *gameWorld, Object *inputObject)
{
	if (gameWorld == NULL || gameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	PlayerData *player = gameWorld->Player;


	switch (inputObject->ObjectID)
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
			if (boxOverlapsBox(player->PlayerBox, inputObject->ObjectBox) == 1)
			{
				AddObject(gameWorld, PARTICLE, inputObject->ObjectBox->xPos, inputObject->ObjectBox->yPos, SPARKLE, 1, 0, 0, 0);
				MarkObjectForDeletion(inputObject);
				player->coinCount++;
				LemonPlaySound("Coin_Collect", "Objects", OBJECT_SFX, 0.8);
			}

		} break;


		case SPRING:
		{
			if (inputObject->arg5 < 1 && player->PlayerBox->yVelocity < -1.0 && boxOverlapsBox(player->PlayerBox, inputObject->ObjectBox) == 1)
			{
				player->PlayerBox->yVelocity = (double)inputObject->arg1;
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
			UpdateVerticalGate(inputObject, gameWorld->ObjectList, player);
		} break;


		case HORIZONTAL_GATE:
		{
			UpdateHorizontalGate(inputObject, gameWorld->ObjectList, player);
		} break;


		case GATE_SWITCH_TIMED:
		case GATE_SWITCH:
		{
			UpdateGateSwitch(player, inputObject);
			inputObject->ObjectBox->yPos = 64 + (32 * inputObject->arg3);
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


	if (boxOverlapsBox(player->PlayerBox, gateSwitch->ObjectBox) == 1)
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
			gate->ObjectBox->yPos += (gate->ObjectBox->yVelocity + (speed/abs(speed))) * deltaTime;

			if (boxOverlapsBox(player->PlayerBox, gate->ObjectBox) == 1)
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
			gate->ObjectBox->yPos += (gate->ObjectBox->yVelocity - (speed/abs(speed))) * deltaTime;

			if (boxOverlapsBox(player->PlayerBox, gate->ObjectBox) == 1)
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
		platform->ObjectBox->xVelocity += 0.5 * deltaTime;
	}

	if (platform->arg4 < 0 && platform->ObjectBox->xVelocity > -(double)maxSpeed && XPos > (double)leftBound)
	{
		platform->ObjectBox->xVelocity -= 0.5 * deltaTime;
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
		platform->ObjectBox->yVelocity += 0.75 * deltaTime;
	}

	if (platform->arg4 < 0 && platform->ObjectBox->yVelocity > -maxSpeed && YPos > (double)bottomBound)
	{
		platform->ObjectBox->yVelocity -= 0.75 * deltaTime;
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


int boxOverlapsBox(PhysicsRect *inputBox, PhysicsRect *compareBox)
{
	return !((int)inputBox->xPos >= (int)(compareBox->xPos + compareBox->xSize) || (int)(inputBox->xPos + inputBox->xSize) <= (int)compareBox->xPos || (int)inputBox->yPos >= (int)(compareBox->yPos + compareBox->ySize) || (int)(inputBox->yPos + inputBox->ySize) <= (int)compareBox->yPos);
}


int boxOverlapsBoxBottom(PhysicsRect *inputBox, PhysicsRect *compareBox)
{
	return !((int)inputBox->xPos >= (int)(compareBox->xPos + compareBox->xSize) || (int)(inputBox->xPos + inputBox->xSize) <= (int)compareBox->xPos || (inputBox->yPos - 2) >= (int)(compareBox->yPos + compareBox->ySize) || (inputBox->yPos + 8) <= (int)compareBox->yPos);
}



// returns pointer of object overlapping, NULL if no object is detected
Object* GetObjectOverlappingBox(PhysicsRect *inputBox, World *gameWorld)
{
	Object *currentObject;
	currentObject = gameWorld->ObjectList->firstObject;

	if (currentObject == NULL)
	{
		return NULL;
	}

	while (currentObject != NULL)
	{
		int result = 0;

		if (currentObject->layer > FOREGROUND)
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		result = CheckBoxOverlapsBox(inputBox, currentObject->ObjectBox);


		if (result == 1)
		{
			return currentObject;
		}

		currentObject = currentObject->nextObject;
	}

	return NULL;
}


// Compares two physics rects and Returns a 1 if overlapping a solid object, 0 if not
int CheckBoxOverlapsBox(PhysicsRect *inputBox, PhysicsRect *compareBox)
{
	if (inputBox == NULL || compareBox == NULL)
	{
		return MISSING_DATA;
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
		case FLAT_SLOPE_LR:
		{
			inputYTop = ((compareBox->xPos + compareBox->xSize - inputBox->xPos) * ((double)inputBox->ySize/(double)inputBox->xSize));

			if (inputYTop > inputBox->ySize)
			{
				inputYTop = inputBox->ySize;
			}

			inputYTop += inputBox->yPos;
		} break;

		case FLAT_SLOPE_RL: 
		{
			inputYTop = ((inputBox->xSize - (compareBox->xPos - inputBox->xPos)) * ((double)inputBox->ySize/(double)inputBox->xSize));

			if (inputYTop > inputBox->ySize)
			{
				inputYTop = inputBox->ySize;
			}

			inputYTop += inputBox->yPos;
		} break;
			
		case UNSOLID:
		return 0;

		default:
		break;
	}


	switch(compareBox->solid)
	{
		case FLAT_SLOPE_LR:
		{
			compareYTop = ((inputBox->xPos + inputBox->xSize - compareBox->xPos) * ((double)compareBox->ySize/(double)compareBox->xSize));

			if (compareYTop > compareBox->ySize)
			{
				compareYTop = compareBox->ySize;
			}

			compareYTop += compareBox->yPos;
		} break;

		case FLAT_SLOPE_RL: 
		{
			compareYTop = ((compareBox->xSize - (inputBox->xPos - compareBox->xPos)) * ((double)compareBox->ySize/(double)compareBox->xSize));

			if (compareYTop > compareBox->ySize)
			{
				compareYTop = compareBox->ySize;
			}

			compareYTop += compareBox->yPos;
		} break;

		case UNSOLID:
		return 0;
			
		default:
		break;
	}


	return !(inputY >= compareYTop || inputYTop <= compareY || inputX >= compareXRight || inputXRight <= compareX);
}


int assignDirection(PhysicsRect *inputBox, Object *currentObject)
{
	if (currentObject == NULL)
	{
		inputBox->direction = RADIAN_90;
		return 0;
	}

	
	switch (currentObject->ObjectBox->solid)
	{
			case FLAT_SLOPE_LR:
			{
				double slope = (double)currentObject->ObjectBox->ySize/(double)currentObject->ObjectBox->xSize;
				inputBox->direction = (RADIAN_90 - atan(slope));

				if (inputBox->xVelocity < 0.0)
				{
					inputBox->yVelocity = -16 * cos(inputBox->direction);
				}
				else if (inputBox->yVelocity < -2.0)
				{
					inputBox->yVelocity *= 0.7;
				}
			} break;

			case FLAT_SLOPE_RL:
			{
				double slope = (double)currentObject->ObjectBox->ySize/(double)currentObject->ObjectBox->xSize;
				inputBox->direction = (RADIAN_90 + atan(slope));

				if (inputBox->xVelocity > 0.0)
				{
					inputBox->yVelocity = 16 * cos(inputBox->direction);
				}
				else if (inputBox->yVelocity < -2.0)
				{
					inputBox->yVelocity *= 0.7;
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
			if (CheckBoxOverlapsBox(inputObject->ObjectBox, currentObject->ObjectBox) == 1)
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
			if (CheckBoxOverlapsBox(inputObject->ObjectBox, currentObject->ObjectBox) == 1)
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
			if (CheckBoxOverlapsBox(inputObject->ObjectBox, currentObject->ObjectBox) == 1)
			{
				return 1;
			}
		}

		currentObject = currentObject->nextObject;

		i++;
	}

	return 0;
}


int ResolvePlayerToObjectCollisionX(Object *inputObject, PlayerData *player, double prevObjXPos, double prevObjXPosRight)
{
	double ObjXPos = inputObject->ObjectBox->xPos;
	double ObjXPosRight = inputObject->ObjectBox->xPosRight;
	double ObjYPos = inputObject->ObjectBox->yPos;
	double ObjYPosTop = inputObject->ObjectBox->yPos + inputObject->ObjectBox->ySize;

	double prevXPosBuffer = prevObjXPos;
	double prevXPosRightBuffer = prevObjXPosRight;


	int result = 0;

	switch(inputObject->ObjectBox->solid)
	{
		case FLAT_SLOPE_LR:
			ObjXPos = (ObjYPos / ((double)inputObject->ObjectBox->ySize/(double)inputObject->ObjectBox->xSize)) + ObjXPos;
			prevXPosBuffer = prevObjXPosRight;
			break;

		case FLAT_SLOPE_RL:
			ObjXPosRight = ( inputObject->ObjectBox->xSize - (ObjYPos / ((double)inputObject->ObjectBox->ySize/(double)inputObject->ObjectBox->xSize)) ) + ObjXPos;
			prevXPosRightBuffer = prevObjXPos;
			break;


		case UNSOLID:
			return 0;

		default:
			break;
	}

	result = CheckBoxOverlapsBox(player->PlayerBox, inputObject->ObjectBox);


	if (result == 1)
	{
		if (player->PlayerBox->xPos < ((prevXPosBuffer + prevXPosRightBuffer) / 2.0) )
		{
			player->PlayerBox->xPos = ObjXPos - player->PlayerBox->xSize;
		}
		else
		{
			player->PlayerBox->xPos = ObjXPosRight;
		}

		return 1;
	}

	return 0;
}


int ResolvePlayerToObjectCollisionY(Object *inputObject, PlayerData *player, double prevObjYPos, double prevObjYPosTop)
{
	double ObjXPos = inputObject->ObjectBox->xPos;
	double ObjXPosRight = inputObject->ObjectBox->xPos + inputObject->ObjectBox->xSize;
	double ObjYPos = inputObject->ObjectBox->yPos;
	double ObjYPosTop = inputObject->ObjectBox->yPos + inputObject->ObjectBox->ySize;

	double prevYPosBuffer = prevObjYPos;
	double prevYPosTopBuffer = prevObjYPosTop;


	int result = 0;

	switch(inputObject->ObjectBox->solid)
	{
		case FLAT_SLOPE_LR:
			ObjYPosTop = ( ((player->PlayerBox->xPos + player->PlayerBox->xSize - inputObject->ObjectBox->xPos) * ((double)inputObject->ObjectBox->ySize/(double)inputObject->ObjectBox->xSize)) ) + ObjYPos;
			prevYPosTopBuffer = prevObjYPos;
			break;

		case FLAT_SLOPE_RL:
			ObjYPosTop = ( ((inputObject->ObjectBox->xSize - (player->PlayerBox->xPos - inputObject->ObjectBox->xPos)) * ((double)inputObject->ObjectBox->ySize/(double)inputObject->ObjectBox->xSize)) ) + ObjYPos;
			prevYPosTopBuffer = prevObjYPos;
			break;

		case JUMP_THROUGH:
			if ( player->PlayerBox->yVelocity > 0.0 || player->PlayerBox->yPos < prevObjYPosTop - (inputObject->ObjectBox->ySize >> 1) )
			{
				return 0;
			}
			break;

		case UNSOLID:
			return 0;

		default:
			break;
	}


	result = CheckBoxOverlapsBox(player->PlayerBox, inputObject->ObjectBox);
	

	if (result == 1)
	{

		if (player->PlayerBox->yPos < ((prevYPosBuffer + prevYPosTopBuffer) / 2.0) )
		{
			player->PlayerBox->yPos = ObjYPos - player->PlayerBox->ySize;
		}
		else
		{
			player->PlayerBox->yPos = ObjYPosTop; 
		}



		return 1;
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
	double prevObjXPosRight = inputObject->ObjectBox->xPos + inputObject->ObjectBox->xSize;

	inputObject->ObjectBox->xPos += (inputObject->ObjectBox->xVelocity * deltaTime);
	inputObject->ObjectBox->xPosRight = inputObject->ObjectBox->xPos + inputObject->ObjectBox->xSize;


	if (player == NULL)
	{
		return MISSING_DATA;
	}


	int result = ResolvePlayerToObjectCollisionX(inputObject, player, prevObjXPos, prevObjXPosRight);

	if (result == 1)
	{
		return 0;
	}


	int prevYSize = player->PlayerBox->ySize;
	player->PlayerBox->ySize = 8;
	player->PlayerBox->yPos -= 2.0;

	result = CheckBoxOverlapsBox(player->PlayerBox, inputObject->ObjectBox);

	player->PlayerBox->ySize = prevYSize;
	player->PlayerBox->yPos += 2.0;

	if (result == 1)
	{
		int pixelDifference = (int)inputObject->ObjectBox->xPos - (int)(inputObject->ObjectBox->xPos - (inputObject->ObjectBox->xVelocity * deltaTime));
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

	inputObject->ObjectBox->yPos += (inputObject->ObjectBox->yVelocity * deltaTime);
	inputObject->ObjectBox->yPosTop = inputObject->ObjectBox->yPos + inputObject->ObjectBox->ySize;

	if (player == NULL)
	{
		return MISSING_DATA;
	}


	int result = ResolvePlayerToObjectCollisionY(inputObject, player, prevObjYPos, prevObjYPosTop);

	if (result == 1)
	{
		return 0;
	}

	int prevYSize = player->PlayerBox->ySize;
	player->PlayerBox->ySize = 8;
	player->PlayerBox->yPos -= 2.0;

	result = CheckBoxOverlapsBox(player->PlayerBox, inputObject->ObjectBox);

	player->PlayerBox->ySize = prevYSize;
	player->PlayerBox->yPos += 2.0;

	if (result == 1)
	{
		int pixelDifference = (int)inputObject->ObjectBox->yPos - (int)(inputObject->ObjectBox->yPos - (inputObject->ObjectBox->yVelocity * deltaTime));
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
	double prevObjXPosRight = inputObject->ObjectBox->xPos + inputObject->ObjectBox->xSize;

	inputObject->ObjectBox->xPos = newXPos;
	inputObject->ObjectBox->xPosRight = newXPos = inputObject->ObjectBox->xSize;

	if (Player == NULL || inputObject->ObjectBox->solid == 0)
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

	inputObject->ObjectBox->xSize += change;
	inputObject->ObjectBox->xPos -= (double)(change >> 1);
	inputObject->ObjectBox->xPosRight += change >> 1;

	if (player == NULL || change == 0 || inputObject->ObjectBox->solid < 1)
	{
		return -1;
	}

	ResolvePlayerToObjectCollisionX(inputObject, player, inputObject->ObjectBox->xPos + (change >> 1), inputObject->ObjectBox->xPosRight - (change >> 1));
	
	return 0;
}


int ChangeObjectYSizeBy(int change, Object *inputObject, PlayerData *player)
{
	if (inputObject == NULL)
	{
		return -1;
	}

	inputObject->ObjectBox->ySize += change;
	inputObject->ObjectBox->yPos -= change >> 1;
	inputObject->ObjectBox->yPosTop += change >> 1;

	if (player == NULL || change == 0 || inputObject->ObjectBox->solid < 1)
	{
		return -1;
	}


	ResolvePlayerToObjectCollisionY(inputObject, player, inputObject->ObjectBox->yPos + (change >> 1), inputObject->ObjectBox->yPosTop - (change >> 1));
	
	return 0;
}



int ClimbFlatSlope(PhysicsRect *inputBox, PhysicsRect *compareBox, World *GameWorld)
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


	while (objectCheck == NULL && i < round(fabs(inputBox->xVelocity * deltaTime)) )
	{
		inputBox->xPos += orientation; 
		i++;

		int slopeClimb = 0;
		while (CheckBoxOverlapsBox(inputBox, compareBox) == 1 && slopeClimb < 6)
		{
			inputBox->yPos++;
			slopeClimb++;
		}

		objectCheck = GetObjectOverlappingBox(inputBox, GameWorld);
	}

	if (objectCheck != NULL)
	{
		inputBox->xPos -= orientation;

		double slopeFloor;

		if (compareBox->solid == FLAT_SLOPE_LR)
		{
			slopeFloor = (inputBox->xPos + inputBox->xSize - compareBox->xPos) * slope;
		}
		else
		{
			slopeFloor = (compareBox->xSize - (inputBox->xPos - compareBox->xPos)) * slope;
		}

		if (slopeFloor > compareBox->ySize)
		{
			slopeFloor = compareBox->ySize;
		}

		inputBox->yPos = slopeFloor + compareBox->yPos;

		inputBox->xVelocity = 0.0;
	}	


	if (compareBox->yPosTop - inputBox->yPos < 8.0 && compareBox->yPosTop - inputBox->yPos > -1.0)
	{
		inputBox->yPos+=compareBox->yPosTop - inputBox->yPos;
		printf("WHY\n");
	}
					
	return 0;
}


int GetPathToBox(Object *inputObject, PhysicsRect *target)
{



	return 0;
}