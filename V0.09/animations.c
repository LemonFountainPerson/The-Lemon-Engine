#include "LemonEngine.h"



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

	// Searches for a file named "Object [ObjectID]" as default if name does not exist
	if (loadAnimationsFromFile(ConvertIDToObjectName(ObjectID), newSet) != LEMON_SUCCESS)
	{
		char defaultName[32] = {0};
		snprintf(defaultName, 32, "Object %d", ObjectID);

		return loadAnimationsFromFile(defaultName, newSet);
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

	while (!endOfFile(fPtr, argBuffer))
	{
		getNextArg(fPtr, argBuffer, MAX_LEN);
		stringToLower(argBuffer);

		if (strcmp(argBuffer, "newanimation:") == 0 || strcmp(argBuffer, "addanimation:") == 0)
		{
			char inputName[MAX_LEN] = {0};
			getNextArg(fPtr, inputName, MAX_LEN);

			float frameRate = getNextArgFloat(fPtr);

			newAnimation = initialiseNewAnimation(inputName, frameRate, destSet);
		}
		else if (strcmp(argBuffer, "addframe:") == 0)
		{
			getNextArg(fPtr, argBuffer, MAX_LEN);

			int xOffset = getNextArgFloat(fPtr);
			int yOffset = getNextArgFloat(fPtr);
			float rotation = getNextArgFloat(fPtr);

			addSpriteToAnimationWithAttributes(argBuffer, newAnimation, destSet, xOffset, yOffset, rotation);
		}
		else if (strcmp(argBuffer, "addsprite:") == 0 || strcmp(argBuffer, "newsprite:") == 0)
		{
			char SpriteName[MAX_LEN];
			getNextArg(fPtr, SpriteName, MAX_LEN);
			
			char FolderName[MAX_LEN];
			getNextArg(fPtr, FolderName, MAX_LEN);

		 	long tempFilePosition = ftell(fPtr);
			getNextArg(fPtr, argBuffer, MAX_LEN);
			
			int renderMode = convertStringToRenderMode(argBuffer);

			if (renderMode == UNDEFINED_RENDERMODE)
			{
				// If sprite has no recognised rendermode attribute set to single
				renderMode = SINGLE;
				fseek(fPtr, tempFilePosition, SEEK_SET);
			}
			
			loadSpriteIntoSpriteSet(SpriteName, FolderName, destSet, renderMode);

			Sprite *newSprite = getSpriteSpriteSet(SpriteName, destSet);

			tempFilePosition = ftell(fPtr);
			getNextArg(fPtr, argBuffer, MAX_LEN);
			stringToLower(argBuffer);

			if (!strcmp(argBuffer, "pixelmode") && newSprite != NULL)
			{
				SDL_SetTextureScaleMode(newSprite->texture, SDL_SCALEMODE_PIXELART);
			}
			else if ((!strcmp(argBuffer, "aliasingmode") || !strcmp(argBuffer, "linearmode")) && newSprite != NULL)
			{
				SDL_SetTextureScaleMode(newSprite->texture, SDL_SCALEMODE_LINEAR);
			}
			else
			{
				fseek(fPtr, tempFilePosition, SEEK_SET);
			}
		}
		else if (argBuffer[0] == '>')
		{
			skipCommentInFile(fPtr);
		}
	}


	closeFile(fPtr);

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


int useThisAnimation(Animation *anim, int loopCount, DisplayData *inputData)
{
	if (anim == NULL || anim->animationData == NULL)
	{
		return MISSING_DATA;
	}
	
	inputData->frameBuffer = anim->animationData;
	inputData->animationBuffer = anim;

	inputData->currentAnimation = anim->animationID;
	inputData->animationTick = 0.0;
	inputData->animationLoopCount = loopCount;

	inputData->spriteBuffer = anim->animationData->frameSprite;
	if (inputData->spriteBuffer != NULL)
	{
		inputData->currentSprite = inputData->spriteBuffer->spriteID;
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

	int i = 0;
	while (currentAnimation != NULL && strcmp(currentAnimation->name, desiredName) != 0 && i < 9999)
	{
		currentAnimation = currentAnimation->nextAnimation;
		i++;
	}

	return useThisAnimation(currentAnimation, loopCount, inputData);
}

int PlayObjectAnimation(const char desiredName[], int loopCount, Object *input)
{
	return PlayAnimation(desiredName, loopCount, getDisplay(input));
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

	return useThisAnimation(currentAnimation, loopCount, inputData);
}

int PlayObjectAnimationByIndex(int index, int loopCount, Object *input)
{
	return PlayAnimationByIndex(index, loopCount, getDisplay(input));
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

int PlayNewObjectAnimation(const char desiredName[], int loopCount, Object *input)
{
	return PlayNewAnimation(desiredName, loopCount, getDisplay(input));
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


bool playingAnimation(DisplayData *inputData)
{
	if (inputData == NULL)
	{
		return false;
	}

	return (inputData->currentAnimation != 0);
}

bool objectPlayingAnimation(Object *input)
{
	return playingAnimation(getDisplay(input));
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

	if (frameRate - 0.001 > inputData->animationTick)
	{
		return EXECUTION_UNNECESSARY;
	}

	inputData->animationTick -= frameRate;

	if (inputData->animationTick > 1.0)
	{
		inputData->animationTick = inputData->animationTick - floor(inputData->animationTick);
	}

	if (inputData->frameBuffer->nextFrame == NULL)
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
	else
	{
		inputData->frameBuffer = inputData->frameBuffer->nextFrame;
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

	while (currentSprite->nextSprite != NULL && strcmp(currentSprite->name, spriteName) != 0)
	{
		currentSprite = currentSprite->nextSprite;
	}

	if (EngineSettings.DefaultTexture != NULL && strcmp(spriteName, EngineSettings.DefaultTexture->name) == 0)
	{
		currentSprite = EngineSettings.DefaultTexture;
	}
	else if (strcmp(currentSprite->name, spriteName) != 0)
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


int makeNewBackgroundTileMap(TilePlane *input, int gridWidth, int gridHeight)
{
	if (input == NULL)
	{
		return MISSING_DATA;
	}

	if (input->tiles)
	{
		free(input->tiles);
	}

	input->tiles = calloc(gridWidth * gridHeight, 1);

	if (input->tiles == NULL)
	{
		return LEMON_ERROR;
	}

	input->GridWidth = gridWidth;
	input->GridHeight = gridHeight;

	// testv
	for (int i = 0; i < input->GridWidth * gridHeight; i++)
	{
		input->tiles[i] = i % 16;
	}

	return LEMON_SUCCESS;
}


int loadTileSetBackground(const char *name, BackgroundData *bg)
{
	if (bg == NULL)
	{
		return MISSING_DATA;
	}

	FILE *file = openFile(name, ANIMATION_ROOT, "--TILESET_DATA--");
	if (file == NULL)
	{
		return FILE_NOT_FOUND;
	}

	char arg[MAX_LEN] = {0};

	int width = getNextArgInt(file);
	int height = getNextArgInt(file);
	TilePlane *tPlane = &bg->tileBG;
	// default; centered
	tPlane->xPos = (-width * tPlane->tileWidth) / 2;
	tPlane->yPos = (-height * tPlane->tileHeight) / 2;


	makeNewBackgroundTileMap(tPlane, width, height);

	if (tPlane->tiles == NULL)
	{
		closeFile(file);
		return LEMON_ERROR;
	}


	int size = width * height;
	memset(tPlane->tiles, 0, size);

	int i = 0;

	while (!endOfFile(file, arg) && i < size)
	{
		if (hasNextArgInt(file))
		{
			tPlane->tiles[i] = getNextArgInt(file);
			i++;
			continue;
		}
		
		getNextArg(file, arg, MAX_LEN);
		stringToLower(arg);

		if (!strcmp("copyrow:", arg))
		{
			int startIndex = i - (i % width);
			int rowIndex = width * (getNextArgInt(file) - 1);
			int repeat = 1;

			if (hasNextArgInt(file))
			{
			 	repeat = getNextArgInt(file) * width;
			}

			if (repeat > size - startIndex || rowIndex >= startIndex)
			{
				continue;
			}

			for (int k = 0; k < repeat; k += width)
			{
				memcpy(tPlane->tiles + startIndex, tPlane->tiles + rowIndex, width);
				startIndex += width;
			}
				
			i = startIndex;
		}
		else if (!strcmp("repeat:", arg))
		{
			Uint8 val = (Uint8)getNextArgInt(file);
			int repeat = clamp(getNextArgInt(file), 0, size - i);

			memset(tPlane->tiles + i, val, repeat);
			i += repeat;
		}
		else if (!strcmp("fill:", arg))
		{
			Uint8 val = (Uint8)getNextArgInt(file);

			memset(tPlane->tiles + i, val, size - i);
			i = size;
		}
		else if (!strcmp("usesprite:", arg))
		{
			getNextArg(file, arg, MAX_LEN);
			int spriteSet = USE_CURRENT_SPRITESET;

			switchBackGroundSpriteName(arg, spriteSet, bg);
		}
		else if (!strcmp("xpos:", arg))
		{
			tPlane->xPos = getNextArgFloat(file);
		}
		else if (!strcmp("ypos:", arg))
		{
			tPlane->yPos = getNextArgFloat(file);
		}
		else if (!strcmp("pos:", arg) || !strcmp("position:", arg))
		{
			tPlane->xPos = getNextArgInt(file);
			tPlane->yPos = getNextArgInt(file);
		}
		else if (arg[0] == '>')
		{
			skipCommentInFile(file);
		}
	}

	closeFile(file);

	return LEMON_SUCCESS;
}