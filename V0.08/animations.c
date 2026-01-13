#include "animations.h"



int LoadSpritesAndAnimationData(SpriteSet *newSet, int ObjectID)
{
	if (newSet == NULL)
	{
		return MISSING_DATA;
	}

	if (ObjectID <= LEVEL_FLAG_OBJ || ObjectID >= OBJECT_TYPE_COUNT)
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
			// Searches for a file named "Object [ObjectID]" as default
			char defaultName[32] = {0};
			snprintf(defaultName, 32, "Object %d", ObjectID);

			loadAnimationsFromFile(defaultName, newSet);
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

	FILE *fPtr = openFile(FileName, ANIMATION_ROOT, "--ANIMATION--");

	if (fPtr == NULL)
	{
		return MISSING_DATA;
	}


	char argBuffer[MAX_LEN + 1] = {0};

	Animation *newAnimation = NULL;

	long filePosition = 0;
	int result = LEMON_SUCCESS;


	while (!feof(fPtr) && strcmp(argBuffer, "ENDFILE") != 0)
	{
		getNextArg(fPtr, argBuffer, MAX_LEN);
		filePosition = ftell(fPtr);


		if (strcmp(argBuffer, "NewAnimation:") == 0 || strcmp(argBuffer, "AddAnimation:") == 0)
		{
			char inputName[MAX_LEN] = {0};
			getNextArg(fPtr, inputName, MAX_LEN);

			float frameRate = getNextArgFloat(fPtr);

			newAnimation = initialiseNewAnimation(inputName, frameRate, destSet);

			if (newAnimation == NULL)
			{
				result = LEMON_ERROR;
			}
		}
		else if (strcmp(argBuffer, "AddFrame:") == 0)
		{
			getNextArg(fPtr, argBuffer, MAX_LEN);

			int xOffset = getNextArgInt(fPtr);
			int yOffset = getNextArgInt(fPtr);
			float rotation = getNextArgFloat(fPtr);

			addSpriteToAnimationWithAttributes(argBuffer, newAnimation, destSet, xOffset, yOffset, (rotation * (float)DEGREE_TO_RADIAN_PI) );
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
		else if (argBuffer[0] == '>')
		{
			skipCommentInFile(fPtr, 200);
		}

		if (result != LEMON_SUCCESS)
		{
			fseek(fPtr, filePosition, SEEK_SET);
			result = LEMON_SUCCESS;

			if (DebugSettings.ConsoleTextEnabled == ALL_EVENTS)
			{
				putConsoleStrStr("\nError loading data from file: ", FileName);
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


int stopAnimation(DisplayData *inputData)
{
	if (inputData == NULL || inputData->spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}

	inputData->currentAnimation = 0;

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

	int i = 0;
	while (currentAnimation != NULL && strcmp(currentAnimation->name, desiredName) != 0 && i < 9999)
	{
		currentAnimation = currentAnimation->nextAnimation;
		i++;
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
	inputData->animationLoopCount = clamp(loopCount, -1, 32000);

	if (inputData->spriteBuffer != NULL)
	{
		inputData->currentSprite = inputData->spriteBuffer->spriteID;
	}

	return LEMON_SUCCESS;
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
	int i = 0;

	while (currentAnimation != NULL && index != currentAnimation->animationID && i < 9999)
	{
		currentAnimation = currentAnimation->nextAnimation;
		i++;
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

	return LEMON_SUCCESS;
}


int PlayNewAnimation(const char desiredName[], int loopCount, DisplayData *inputData)
{
	if (inputData == NULL) { return MISSING_DATA; }
	
	if (inputData->currentAnimation != 0 && inputData->animationBuffer != NULL && strcmp(inputData->animationBuffer->name, desiredName) == 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	PlayAnimation(desiredName, loopCount, inputData);

	return LEMON_SUCCESS;
}


int PlayAnimationAfterOther(const char desiredName[], const char otherAnim[], int loopCount, DisplayData *inputData)
{
	if (inputData == NULL || inputData->animationBuffer == NULL) { return MISSING_DATA; }
	
	if (strcmp(inputData->animationBuffer->name, otherAnim) != 0 || inputData->currentAnimation != 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	PlayAnimation(desiredName, loopCount, inputData);


	return LEMON_SUCCESS;
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


	return LEMON_SUCCESS;
}


int getAnimationIndex(const char animationName[], DisplayData *inputData)
{
	if (inputData == NULL || inputData->spriteSetSource == NULL || animationName == NULL)
	{
		return -1;
	}

	if (strlen(animationName) >= MAX_LEN || inputData->spriteSetSource->Animations == NULL)
	{
		return -1;
	}

	Animation *currentAnimation = inputData->spriteSetSource->Animations;

	int i = 0;
	while (currentAnimation != NULL && strcmp(currentAnimation->name, animationName) != 0 && i < 9999)
	{
		currentAnimation = currentAnimation->nextAnimation;
		i++;
	}

	if (currentAnimation == NULL || currentAnimation->animationData == NULL)
	{
		return -1;
	}

	return currentAnimation->animationID;
}


int iterateAnimation(DisplayData *inputData)
{
	// loop count decrements every time animation loops; if == 0, then animation value is 0, which stops the animation. 
	// If value is stareted on <= 0, then on animation end value will be less than 0, and it will loop indefinitely

	if (inputData == NULL || inputData->spriteSetSource == NULL || inputData->animationBuffer == NULL)
	{
		return MISSING_DATA;
	}

	if (inputData->currentAnimation < 1 || inputData->animationLoopCount == ONE_FRAME_INDEFINITE_ANIMATION)
	{
		return ACTION_DISABLED;
	}

	if (inputData->frameBuffer == NULL)
	{
		inputData->currentAnimation = 0;
		return MISSING_DATA;
	}

	float frameRate = inputData->animationBuffer->frameRate;

	inputData->animationTick += inputData->animationSpeed;

	if (frameRate - inputData->animationTick > 0.01)
	{
		return EXECUTION_UNNECESSARY;
	}

	inputData->animationTick -= frameRate;

	if (inputData->animationTick > 1.0)
	{
		inputData->animationTick = inputData->animationTick - floor(inputData->animationTick);
	}

	inputData->frameBuffer = inputData->frameBuffer->nextFrame;

	if (inputData->frameBuffer == NULL)
	{
		// end of animation, loop or end
		if (inputData->animationLoopCount > -1)	// Change this -1 to a 0 to disable the value of 0 representing loop indefinitely alongside the LOOP_INDEFINITELY enum
		{	
			inputData->animationLoopCount--;
		}

		// At the moment a value of 0 in the play animation function's loopcount will also be the same behaviour as LOOP_INDEFINITELY, 
		// but this may change for consistency in the future
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

		if (inputData->frameBuffer->nextFrame == NULL && inputData->animationLoopCount == LOOP_INDEFINITELY)
		{
			inputData->animationLoopCount = ONE_FRAME_INDEFINITE_ANIMATION;
		}
	}

	inputData->spriteBuffer = inputData->frameBuffer->frameSprite;
	inputData->currentSprite = inputData->spriteBuffer->spriteID;

	
	return LEMON_SUCCESS;
}


Animation* initialiseNewAnimation(const char animationName[], float frameRate, SpriteSet *inputSet)
{
	if (inputSet == NULL)
	{
		putConsoleString("Missing SpriteSet for new animation!");
		return NULL;
	}

	if (strlen(animationName) >= MAX_LEN)
	{
		return NULL;
	}

	Animation *newAnimation = malloc(sizeof(Animation));

	if (newAnimation == NULL)
	{
		return NULL;
	}


	int i = 1;

	if (inputSet->Animations == NULL)
	{
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

	frameRate = fClamp(frameRate, 0.1, 1000.0);	
	newAnimation->frameRate = ((float)EngineSettings.GameTicksPerSecond / frameRate);
	newAnimation->frameRate = ceil(newAnimation->frameRate * 100) / 100;

	newAnimation->animationID = i;
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
		putConsoleStrStr("\nCould not find sprite for animation: ", spriteName);
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


AnimationFrame* addSpriteToAnimationWithAttributes(const char spriteName[], Animation *inputAnimation, SpriteSet *sourceSet, float XOffset, float YOffset, float rotation)
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


int deleteAnimation(SpriteSet *inputSet, Animation *deleteAnimation)
{
	if (deleteAnimation == NULL)
	{
		return MISSING_DATA;
	}

	if (inputSet != NULL && inputSet->Animations == deleteAnimation)
	{
		inputSet->Animations = deleteAnimation->nextAnimation;
	}


	AnimationFrame *currentFrame = deleteAnimation->animationData;

	while (currentFrame != NULL)
	{
		AnimationFrame *tempFrame = currentFrame;
		currentFrame = currentFrame->nextFrame;
		free(tempFrame);
	}

	free(deleteAnimation);

	return LEMON_SUCCESS;
}