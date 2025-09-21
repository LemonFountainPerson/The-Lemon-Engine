#include "animations.h"



int LoadSpritesAndAnimationData(SpriteSet *newSet, int ObjectID)
{
	if (newSet == NULL)
	{
		return MISSING_DATA;
	}

	if (ObjectID <= LEVEL_FLAG_OBJ || ObjectID >= UNDEFINED_OBJECT)
	{
		return INVALID_DATA;
	}


	// Fill sprite set with sprites/animations
	switch (ObjectID)
	{
		case PLAYER_OBJECT:
			loadAnimationsFromFile("PlayerObject", newSet);
			break;

		case PARTICLE:
			loadAnimationsFromFile("Particles", newSet);
			break;

		case UI_ELEMENT:
			loadAnimationsFromFile("UIElements", newSet);
			break;

		case UI_TEXT:
			loadAnimationsFromFile("UIText", newSet);
			break;

		case FLAT_SLOPE_FLOOR:
			loadAnimationsFromFile("FlatSlopeFloor", newSet);
			break;

		case SPRING:
			loadAnimationsFromFile("Spring", newSet);
			break;

		case SOLID_BLOCK:
			loadAnimationsFromFile("SolidBlock", newSet);
			break;


		default:
		{
			// Searches for a file named "[ObjectID]" as default
			char defaultName[16];
			sprintf(defaultName, "%d", ObjectID);

			if (loadAnimationsFromFile((const char*)defaultName, newSet) == LEMON_SUCCESS)
			{
				break;
			}

			// If no sprite/animation data exists use missing texture
			loadSpriteIntoSpriteSet("Missing", NULL, newSet, TILE);
		} break;
	}


	return LEMON_SUCCESS;
}


int loadAnimationsFromFile(const char FileName[], SpriteSet *destSet)
{
	if (destSet == NULL || FileName == NULL)
	{
		return MISSING_DATA;
	}

	// Check string
	if (strlen(FileName) >= MAX_LEN)
	{
		return INVALID_DATA;
	}

	char path[strlen(ANIMATION_ROOT) + strlen(FileName) + 5];
	strcpy(path, ANIMATION_ROOT);
	strcat(path, FileName);
	strcat(path, ".txt");

	FILE *fPtr = fopen(path, "rb");

	if (fPtr == NULL)
	{
		return MISSING_DATA;
	}

	checkFileHeader(fPtr, "-ANIMATION");

	char argBuffer[MAX_LEN + 1] = {0};

	Animation *newAnimation = NULL;

	long filePosition = 0;
	int result = LEMON_SUCCESS;


	while (feof(fPtr) == 0)
	{
		getNextArg(fPtr, argBuffer, MAX_LEN);
		filePosition = ftell(fPtr);


		if (strcmp(argBuffer, "NewAnimation:") == 0 || strcmp(argBuffer, "AddAnimation:") == 0)
		{
			char inputName[MAX_LEN] = {0};
			getNextArg(fPtr, inputName, MAX_LEN);

			getNextArg(fPtr, argBuffer, MAX_LEN);

			newAnimation = initialiseNewAnimation(inputName, atof(argBuffer), destSet);

			if (newAnimation != NULL)
			{
				result = LEMON_SUCCESS;
			}
			else
			{
				result = LEMON_ERROR;
			}
		}
		else if (strcmp(argBuffer, "AddFrame:") == 0)
		{
			getNextArg(fPtr, argBuffer, MAX_LEN);

			int intBuffer[3] = {0};
			readIntArgs(fPtr, intBuffer, 3);

			addSpriteToAnimationWithAttributes(argBuffer, newAnimation, destSet, intBuffer[0], intBuffer[1], (float)((double)intBuffer[2] * DEGREE_TO_RADIAN_PI) );
		}
		else if (strcmp(argBuffer, "AddSprite:") == 0)
		{
			char SpriteName[MAX_LEN];
			getNextArg(fPtr, SpriteName, MAX_LEN);
			

			char FolderName[MAX_LEN];
			getNextArg(fPtr, FolderName, MAX_LEN);
		

		 	long tempFilePosition = ftell(fPtr);
			getNextArg(fPtr, argBuffer, MAX_LEN);
			
			int result = convertStringToRenderMode(argBuffer);

			if (result == UNDEFINED_RENDERMODE)
			{
				// If sprite has no recognised rendermode attribute set to single
				result = SINGLE;
				fseek(fPtr, tempFilePosition, SEEK_SET);
			}
			
			result = loadSpriteIntoSpriteSet(SpriteName, FolderName, destSet, result);
		}

		if (result != LEMON_SUCCESS)
		{
			fseek(fPtr, filePosition, SEEK_SET);
			result = LEMON_SUCCESS;

			if (DebugSettings.DebugTextEnabled == ALL_EVENTS)
			{
				printf("Error loading data from file: %s\n", FileName);
			}
		}
	}


	fclose(fPtr);

	if (destSet->firstSprite == NULL)
	{
		loadSpriteIntoSpriteSet("Missing", NULL, destSet, TILE_FAST);
	}

	return LEMON_SUCCESS;
}



int PlayAnimation(const char desiredName[], int loopCount, DisplayData *inputData)
{
	if (inputData == NULL || inputData->spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}

	if (strlen(desiredName) >= MAX_LEN)
	{
		return INVALID_DATA;
	}

	if (inputData->spriteSetSource->Animations == NULL)
	{
		return EXECUTION_UNNECESSARY;
	}


	Animation *currentAnimation = inputData->spriteSetSource->Animations;

	while (currentAnimation != NULL && strcmp(currentAnimation->name, desiredName) != 0)
	{
		currentAnimation = currentAnimation->nextAnimation;
	}

	if (currentAnimation == NULL || currentAnimation->animationData == NULL)
	{
		return MISSING_DATA;
	}
	
	inputData->spriteBuffer = currentAnimation->animationData->frameSprite;
	inputData->frameBuffer = currentAnimation->animationData;
	inputData->animationBuffer = currentAnimation;

	inputData->currentAnimation = currentAnimation->animationID;
	inputData->animationTick = -1.0;
	inputData->animationLoopCount = loopCount;

	if (inputData->spriteBuffer != NULL)
	{
		inputData->currentSprite = inputData->spriteBuffer->spriteID;
	}

	return 0;
}


int PlayAnimationByIndex(int index, int loopCount, DisplayData *inputData)
{
	if (inputData == NULL || inputData->spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}

	if (inputData->spriteSetSource->Animations == NULL)
	{
		return EXECUTION_UNNECESSARY;
	}


	Animation *currentAnimation = inputData->spriteSetSource->Animations;

	while (currentAnimation != NULL && index != currentAnimation->animationID)
	{
		currentAnimation = currentAnimation->nextAnimation;
	}

	if (currentAnimation == NULL || currentAnimation->animationData == NULL)
	{
		return MISSING_DATA;
	}
	
	inputData->spriteBuffer = currentAnimation->animationData->frameSprite;
	inputData->frameBuffer = currentAnimation->animationData;
	inputData->animationBuffer = currentAnimation;

	inputData->currentAnimation = currentAnimation->animationID;
	inputData->animationTick = -1.0;
	inputData->animationLoopCount = loopCount;

	if (inputData->spriteBuffer != NULL)
	{
		inputData->currentSprite = inputData->spriteBuffer->spriteID;
	}

	return LEMON_SUCCESS;
}


int SwitchAnimation(const char desiredName[], int loopCount, DisplayData *inputData)
{
	if (inputData == NULL) { return MISSING_DATA; }
	
	if (inputData->animationBuffer != NULL && strcmp(inputData->animationBuffer->name, desiredName) == 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	PlayAnimation(desiredName, loopCount, inputData);

	return 0;
}


int PlayNewAnimation(const char desiredName[], int loopCount, DisplayData *inputData)
{
	if (inputData == NULL) { return MISSING_DATA; }
	
	if (inputData->currentAnimation != 0 && inputData->animationBuffer != NULL && strcmp(inputData->animationBuffer->name, desiredName) == 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	PlayAnimation(desiredName, loopCount, inputData);

	return 0;
}


int PlayAnimationAfterOther(const char desiredName[], const char otherAnim[], int loopCount, DisplayData *inputData)
{
	if (inputData == NULL || inputData->animationBuffer == NULL) { return MISSING_DATA; }
	
	if (strcmp(inputData->animationBuffer->name, otherAnim) != 0 || inputData->currentAnimation != 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	PlayAnimation(desiredName, loopCount, inputData);


	return 0;
}


int PlayAnimationAfterOtherPrefix(const char desiredName[], const char otherPrefix[], int loopCount, DisplayData *inputData)
{
	if (inputData == NULL || inputData->animationBuffer == NULL) { return MISSING_DATA; }

	int i = strlen(otherPrefix);

	char buffer[MAX_LEN];
	strcpy(buffer, inputData->animationBuffer->name);
	buffer[i] = 0;
	
	if (strcmp(buffer, otherPrefix) != 0 || inputData->currentAnimation != 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	PlayAnimation(desiredName, loopCount, inputData);


	return 0;
}


int iterateAnimation(DisplayData *inputData)
{
	// loop count decrements every time animation loops; if == 0, then animation value is 0, which stops the animation. 
	// If value is stareted on <= 0, then on animation end value will be less than 0, and it will loop indefinitely

	if (inputData == NULL || inputData->spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}

	if (inputData->currentAnimation < 1 || inputData->animationBuffer == NULL)
	{
		return ACTION_DISABLED;
	}

	if (inputData->frameBuffer == NULL)
	{
		inputData->currentAnimation = 0;
		return MISSING_DATA;
	}

	inputData->animationTick += 1.0;

	if (inputData->animationBuffer->frameRate - inputData->animationTick > 0.1)
	{
		return EXECUTION_UNNECESSARY;
	}

	inputData->animationTick -= inputData->animationBuffer->frameRate;

	if (inputData->animationTick > 1.0 || inputData->animationTick < -1.0)
	{
		inputData->animationTick = inputData->animationTick - floor(inputData->animationTick);
	}

	inputData->frameBuffer = inputData->frameBuffer->nextFrame;

	if (inputData->frameBuffer == NULL)
	{
		// end of animation
		if (inputData->animationLoopCount > -1) // this check is done to prevent underflow
		{
			inputData->animationLoopCount--;
		}

		if (inputData->animationLoopCount == 0)
		{
			inputData->currentAnimation = 0;
			return EXECUTION_UNNECESSARY;
		}

		inputData->frameBuffer = inputData->animationBuffer->animationData;

		if (inputData->frameBuffer == NULL)
		{
			return MISSING_DATA;
		}	
	}

	inputData->spriteBuffer = inputData->frameBuffer->frameSprite;
	inputData->currentSprite = inputData->spriteBuffer->spriteID;

	
	return 0;
}


Animation* initialiseNewAnimation(const char animationName[], double frameRate, SpriteSet *inputSet)
{
	if (inputSet == NULL)
	{
		putDebugString("Missing SpriteSet for new animation!");
		return NULL;
	}

	int i = 0;

	if (strlen(animationName) >= MAX_LEN)
	{
		return NULL;
	}

	Animation *newAnimation = malloc(sizeof(Animation));

	if (newAnimation == NULL)
	{
		return NULL;
	}


	if (inputSet->Animations == NULL)
	{
		i = 1;
		inputSet->Animations = newAnimation;
	}
	else
	{
		i = 2;
		Animation *currentAnimation = inputSet->Animations;

		while (currentAnimation->nextAnimation != NULL)
		{
			currentAnimation = currentAnimation->nextAnimation;
			i++;
		}

		currentAnimation->nextAnimation = newAnimation;
	}


	newAnimation->nextAnimation = NULL;
	newAnimation->animationData = NULL;

	newAnimation->animationID = i;
	frameRate = dClamp(frameRate, 0.1, 1000.0);	
	newAnimation->frameRate = ((float)TICKS_PER_SECOND / (float)frameRate);
	memset(newAnimation->name, 0, MAX_LEN);
	strcpy(newAnimation->name, animationName);


	return newAnimation;
}


AnimationFrame* addSpriteToAnimation(const char spriteName[], Animation *inputAnimation, SpriteSet *sourceSet)
{
	if (sourceSet == NULL || inputAnimation == NULL || spriteName == NULL || strlen(spriteName) >= MAX_LEN)
	{
		return NULL;
	}

	Sprite *currentSprite = sourceSet->firstSprite;

	if (currentSprite == NULL)
	{
		return NULL;
	}

	while (currentSprite->nextSprite != NULL && strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		currentSprite = currentSprite->nextSprite;
	}

	if (strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		return NULL;
	}


	AnimationFrame *lastFrame = inputAnimation->animationData;

	if (lastFrame != NULL)
	{
		while (lastFrame->nextFrame != NULL)
		{
			lastFrame = lastFrame->nextFrame;
		}
	}

	AnimationFrame *newFrame = malloc(sizeof(AnimationFrame));

	if (newFrame == NULL)
	{
		return NULL;
	}

	newFrame->frameSprite = currentSprite;
	newFrame->SpriteXOffset = 0;
	newFrame->SpriteYOffset = 0;
	newFrame->rotation = 0.0;
	newFrame->nextFrame = NULL;

	if (lastFrame == NULL)
	{
		inputAnimation->animationData = newFrame;
	}
	else
	{
		lastFrame->nextFrame = newFrame;
	}

	return newFrame;
}


AnimationFrame* addSpriteToAnimationWithAttributes(const char spriteName[], Animation *inputAnimation, SpriteSet *sourceSet, int XOffset, int YOffset, float rotation)
{
	AnimationFrame *createdFrame = addSpriteToAnimation(spriteName, inputAnimation, sourceSet);

	if (createdFrame != NULL)
	{
		createdFrame->SpriteXOffset = XOffset;
		createdFrame->SpriteYOffset = YOffset;
		createdFrame->rotation = rotation;
	}

	return createdFrame;
}


int deleteAnimation(Animation *deleteAnimation)
{
	if (deleteAnimation == NULL)
	{
		return MISSING_DATA;
	}


	AnimationFrame *currentFrame = deleteAnimation->animationData;

	while (currentFrame != NULL)
	{
		AnimationFrame *tempFrame = currentFrame;
		currentFrame = currentFrame->nextFrame;
		free(tempFrame);
	}


	free(deleteAnimation);

	return 0;
}