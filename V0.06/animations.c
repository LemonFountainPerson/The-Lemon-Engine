#include "animations.h"



int LoadAnimations(SpriteSet *newSet, int ObjectID)
{
	if (newSet == NULL)
	{
		return MISSING_DATA;
	}

	if (ObjectID <= LEVEL_FLAG_OBJ  || ObjectID >= UNDEFINED_OBJECT)
	{
		return INVALID_DATA;
	}


	// Fill sprite set with sprites
	switch (ObjectID)
	{
		case SPRING:
		{
			Animation *newAnim = initialiseNewAnimation("Bounce", 24, newSet);
			addSpriteToAnimation("Spring3", newAnim, newSet);
			addSpriteToAnimation("Spring4", newAnim, newSet);
			addSpriteToAnimation("Spring5", newAnim, newSet);
			addSpriteToAnimation("Spring4", newAnim, newSet);
			addSpriteToAnimation("Spring3", newAnim, newSet);
			addSpriteToAnimation("Spring2", newAnim, newSet);
			addSpriteToAnimation("Spring", newAnim, newSet);
		} break;

		case COIN:
		{
			Animation *newAnim = initialiseNewAnimation("Coin_Spin", 20, newSet);
			addSpriteToAnimation("Coin1", newAnim, newSet);
		} break;

		default:
		break;
	}

	return LEMON_SUCCESS;
}


int loadAnimationsFromFile(const char FileName[], SpriteSet *destSet)
{
	if (destSet == NULL)
	{
		return MISSING_DATA;
	}

	// Check string
	unsigned int i = 0;
	while (i < MAX_LEN && FileName[i] > 31)
	{
		i++;
	}

	if (i >= MAX_LEN)
	{
		return INVALID_DATA;
	}

	char path[25 + MAX_LEN] = "LemonData/Animations/";
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


	while (feof(fPtr) == 0)
	{
		getNextArg(fPtr, argBuffer, MAX_LEN);

		if (strcmp(argBuffer, "NewAnimation:") == 0)
		{
			int result = getNextArg(fPtr, argBuffer, MAX_LEN);

			if (result != LEMON_SUCCESS)
			{
				fclose(fPtr);
				return LEMON_ERROR;
			}

			int intBuffer[1] = {0};
			result = readIntArgs(fPtr, intBuffer, 1);

			if (result != LEMON_SUCCESS)
			{
				fclose(fPtr);
				return LEMON_ERROR;
			}


			newAnimation = initialiseNewAnimation(argBuffer, intBuffer[0], destSet);
		}
		else if (strcmp(argBuffer, "AddFrame:") == 0)
		{
			int result = getNextArg(fPtr, argBuffer, MAX_LEN);

			if (result != LEMON_SUCCESS)
			{
				fclose(fPtr);
				return LEMON_ERROR;
			}

			int intBuffer[2] = {0};
			result = readIntArgs(fPtr, intBuffer, 2);

			if (result != LEMON_SUCCESS)
			{
				fclose(fPtr);
				return LEMON_ERROR;
			}

			addSpriteToAnimationWithOffsets(argBuffer, newAnimation, destSet, intBuffer[0], intBuffer[1]);
		}
		else if (strcmp(argBuffer, "AddSprite:") == 0)
		{
			char FileName[MAX_LEN];
			int result = getNextArg(fPtr, FileName, MAX_LEN);
			
			if (result != LEMON_SUCCESS)
			{
				fclose(fPtr);
				return LEMON_ERROR;
			}

			char FolderName[MAX_LEN];
			result = getNextArg(fPtr, FolderName, MAX_LEN);
			
			if (result != LEMON_SUCCESS)
			{
				fclose(fPtr);
				return LEMON_ERROR;
			}

			result = getNextArg(fPtr, argBuffer, MAX_LEN);
			
			if (result != LEMON_SUCCESS)
			{
				fclose(fPtr);
				return LEMON_ERROR;
			}

			result = convertStringToRenderMode(argBuffer);

			if (result == UNDEFINED_RENDERMODE)
			{
				fclose(fPtr);
				return LEMON_ERROR;
			}

			loadSpriteIntoSpriteSet(FileName, FolderName, destSet, result);
		}
	}


	fclose(fPtr);

	return LEMON_SUCCESS;
}


int LoopParticleAnimation(Object *particle)
{
	if (particle == NULL || particle->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	DisplayData *particleDisplay = particle->ObjectDisplay;

	if (particle->arg1 < 1 || particleDisplay->animationTick < particle->arg2)
	{
		return EXECUTION_UNNECESSARY;
	}

	particleDisplay->animationTick = 0;

	int firstSprite = 1;
	int lastSprite = 1;
	int animateType = 1;

	// Add a new case whenever you make a new particle here
	switch(particleDisplay->currentAnimation)
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
			int onOddLoop = 1 - ( particleDisplay->animationTick / ((lastSprite - firstSprite + 1) * particle->arg2) ) % 2;

			if (onOddLoop == 1)
			{
				particleDisplay->currentSprite++;
			}
			else
			{
				particleDisplay->currentSprite--;
			}

			if (particleDisplay->currentSprite > lastSprite || particleDisplay->currentSprite < firstSprite)
			{
				particle->arg1--;

				if (onOddLoop == 1)
				{
					particleDisplay->currentSprite = lastSprite;
				}
				else
				{
					particleDisplay->currentSprite = firstSprite;
				}
			}

		} break;


		case -2:
		{
			int onOddLoop = 1 - ( particleDisplay->animationTick / ((lastSprite - firstSprite + 1) * particle->arg2) ) % 2;

			if (onOddLoop == 1)
			{
				particleDisplay->currentSprite--;
			}
			else
			{
				particleDisplay->currentSprite++;
			}


			if (particleDisplay->currentSprite > lastSprite || particleDisplay->currentSprite < firstSprite)
			{
				particle->arg1--;

				if (onOddLoop == 1)
				{
					particleDisplay->currentSprite = firstSprite;
				}
				else
				{
					particleDisplay->currentSprite = lastSprite;
				}
			}

		} break;


		case -1:
		{
			particleDisplay->currentSprite--;

			if (particleDisplay->currentSprite > lastSprite || particleDisplay->currentSprite < firstSprite)
			{
				particle->arg1--;

				particleDisplay->currentSprite = lastSprite;
			}

		} break;

		default:
		{
			particleDisplay->currentSprite++;

			if (particleDisplay->currentSprite > lastSprite || particleDisplay->currentSprite < firstSprite)
			{
				particle->arg1--;

				particleDisplay->currentSprite = firstSprite;
			}

		} break;
	}

	return 0;
}


int PlayAnimation(const char desiredName[], int loopCount, DisplayData *inputData)
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
	inputData->animationTick = -1;
	inputData->animationLoopCount = loopCount;

	if (inputData->spriteBuffer != NULL)
	{
		inputData->currentSprite = inputData->spriteBuffer->spriteID;
	}

	return 0;
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

	if (inputData == NULL || inputData->animationBuffer == NULL || inputData->spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}

	if (inputData->currentAnimation < 1)
	{
		return ACTION_DISABLED;
	}

	if (inputData->frameBuffer == NULL)
	{
		inputData->currentAnimation = 0;
		return MISSING_DATA;
	}

	inputData->animationTick++;

	if (inputData->animationTick < inputData->animationBuffer->frameRate)
	{
		return EXECUTION_UNNECESSARY;
	}

	inputData->animationTick = 0;
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


Animation* initialiseNewAnimation(const char animationName[], int frameRate, SpriteSet *inputSet)
{
	int i = 0;

	while (i < MAX_LEN && animationName[i] != 0)
	{
		i++;
	}

	if (i >= MAX_LEN)
	{
		return NULL;
	}


	Animation *newAnimation = malloc(sizeof(Animation));

	if (newAnimation == NULL)
	{
		return NULL;
	}


	i = 1;

	if (inputSet->Animations == NULL)
	{
		inputSet->Animations = newAnimation;
	}
	else{
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
	frameRate = clamp(frameRate, 1, 60);
	newAnimation->frameRate = (float)(TICKS_PER_SECOND / frameRate);
	memset(newAnimation->name, 0, MAX_LEN);
	strcpy(newAnimation->name, animationName);


	return newAnimation;
}


AnimationFrame* addSpriteToAnimation(const char spriteName[], Animation *inputAnimation, SpriteSet *sourceSet)
{
	if (sourceSet == NULL || inputAnimation == NULL)
	{
		return NULL;
	}

	printf("Added frame to %s\n", inputAnimation->name);

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


AnimationFrame* addSpriteToAnimationWithOffsets(const char spriteName[], Animation *inputAnimation, SpriteSet *sourceSet, int XOffset, int YOffset)
{
	AnimationFrame *createdFrame = addSpriteToAnimation(spriteName, inputAnimation, sourceSet);

	if (createdFrame != NULL)
	{
		createdFrame->SpriteXOffset = XOffset;
		createdFrame->SpriteYOffset = YOffset;
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