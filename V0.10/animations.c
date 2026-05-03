#include "LemonEngine.h"


AnimationFrame *globalFrame = NULL;

SpriteSet* loadSpriteSetFromFile(const char FileName[], SpriteSetList *setList, int desiredID)
{
	if (setList == NULL || FileName == NULL)
	{
		return NULL;
	}

	FILE *fPtr = openFile(FileName, ANIMATION_ROOT, "--ANIMATION--");

	if (fPtr == NULL)
	{
		return NULL;
	}

	SpriteSet *newSet = createNewSpriteSet(setList, desiredID);

	if (newSet == NULL)
	{
		closeFile(fPtr);
		return NULL;
	}

	char argBuffer[MAX_LEN + 1] = {0};
	Animation *newAnimation = NULL;

	while (!endOfFile(fPtr))
	{
		getNextArg(fPtr, argBuffer, MAX_LEN);
		stringToLower(argBuffer);

		if (strcmp(argBuffer, "newanimation:") == 0 || strcmp(argBuffer, "addanimation:") == 0)
		{
			char inputName[MAX_LEN] = {0};
			getNextArg(fPtr, inputName, MAX_LEN);

			float frameRate = getNextArgFloat(fPtr);

			newAnimation = initialiseNewAnimation(inputName, frameRate, newSet);
		}
		else if (strcmp(argBuffer, "addframe:") == 0 && newAnimation != NULL)
		{
			if (!EngineSettings.ContiguousAnimAllocation && newAnimation->contiguousFrames == false)		// if disabled, frames will be allocated one at a time as a true linked list
			{
				getNextArg(fPtr, argBuffer, MAX_LEN);

				int xOffset = getNextArgFloat(fPtr);
				int yOffset = getNextArgFloat(fPtr);
				float rotation = getNextArgFloat(fPtr);

				AnimationFrame *newFrame = addSpriteToAnimationWithAttributes(argBuffer, newAnimation, newSet, xOffset, yOffset, rotation);

				loadFrameSound(fPtr, newFrame, newAnimation);

				continue;
			}


			// otherwise, allocates frames as one contiguous chunk for memory optimisation
			if (newAnimation->animationData != NULL)
			{
				continue;
			}

			int count = 0;
			long filePos = ftell(fPtr);

			// get number of frames
			while (strcmp(argBuffer, "addframe:") == 0)
			{
				count++;
				getNextArg(fPtr, argBuffer, MAX_LEN);
				getNextArgFloat(fPtr);
				getNextArgFloat(fPtr);
				getNextArgFloat(fPtr);
				getNextArg(fPtr, argBuffer, MAX_LEN);
				stringToLower(argBuffer);

				// in case there is a sound instruction to skip
				if (strcmp(argBuffer, "playsound:") == 0)
				{
					getNextArg(fPtr, argBuffer, MAX_LEN);
					getNextArgInt(fPtr);
					getNextArgFloat(fPtr);
					getNextArg(fPtr, argBuffer, MAX_LEN);
					stringToLower(argBuffer);
				}
			} 

			fseek(fPtr, filePos, SEEK_SET);

			// allocate data for count frames
			AnimationFrame *frameList = malloc(sizeof(AnimationFrame) * count);
			if (frameList == NULL)
			{
				continue;
			}

			if (DebugSettings.ConsoleTextEnabled == CONSOLE_ALL_EVENTS)
			{
				putConsoleString("Allocating animation (%s) of size: %d + %d", newAnimation->name, sizeof(Animation), (sizeof(AnimationFrame) * count));
			}

			newAnimation->contiguousFrames = true;

			// load frames
			for (int i = newAnimation->frameCount; i < count; i++)
			{
				if (i != 0)
				{
					getNextArg(fPtr, argBuffer, MAX_LEN);
				}

				getNextArg(fPtr, argBuffer, MAX_LEN);

				int xOffset = getNextArgFloat(fPtr);
				int yOffset = getNextArgFloat(fPtr);
				float rotation = getNextArgFloat(fPtr);

				globalFrame = &frameList[i];
				AnimationFrame *newFrame = addSpriteToAnimationWithAttributes(argBuffer, newAnimation, newSet, xOffset, yOffset, rotation);

				loadFrameSound(fPtr, newFrame, newAnimation);
			}

			globalFrame = NULL;
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
			
			loadSpriteIntoSpriteSet(SpriteName, FolderName, newSet, renderMode);

			// Sprite *newSprite = getSpriteSpriteSet(SpriteName, newSet);

			// tempFilePosition = ftell(fPtr);
			// getNextArg(fPtr, argBuffer, MAX_LEN);
			// stringToLower(argBuffer);

			// if (!strcmp(argBuffer, "pixelmode") && newSprite != NULL)
			// {
			// 	SDL_SetTextureScaleMode(newSprite->texture, SDL_SCALEMODE_PIXELART);
			// }
			// else if ((!strcmp(argBuffer, "aliasingmode") || !strcmp(argBuffer, "linearmode")) && newSprite != NULL)
			// {
			// 	SDL_SetTextureScaleMode(newSprite->texture, SDL_SCALEMODE_LINEAR);
			// }
			// else
			// {
			// 	fseek(fPtr, tempFilePosition, SEEK_SET);
			// }
		}
		else if (strcmp(argBuffer, "copy:") == 0)
		{
			// get ID of set to copy
			int copyID;

			if (hasNextArgInt(fPtr))
			{
				copyID = getNextArgInt(fPtr);
			}
			else
			{
				getNextArg(fPtr, argBuffer, MAX_LEN);
				copyID = getObjectID(argBuffer);
			}

			// if ID is current set, or current set already has some unique data loaded, ignore this instruction
			if (copyID == newSet->setID || copyID <= LEVEL_FLAG_OBJ || newSet->Animations != NULL || newSet->firstSprite != NULL)
			{
				continue;
			}

			// Find set to copy from if already exists
			SpriteSet *set = getSpriteSet(setList, copyID);

			if (set == NULL)
			{
				// set doesn't exist, so load it
				set = loadSpriteSetFromFile(getObjectIDName(copyID), setList, copyID);
				if (set == NULL)
				{
					char defaultName[32] = {0};
					snprintf(defaultName, 32, "Object %d", copyID);

					set = loadSpriteSetFromFile(defaultName, setList, copyID);
				}

				if (set == NULL)
				{
					continue;
				}
			}

			// update copies list of source set
			int *newCopies = malloc((set->copyCount + 1) * sizeof(int));
			if (newCopies == NULL)
			{
				continue;
			}

			// delete set previously allocated for this object to switch to copied set
			deleteSpriteSet(newSet, setList);
			newSet = set;

			if (set->copies != NULL)
			{
				memcpy(newCopies, set->copies, set->copyCount * sizeof(int));
				free(set->copies);
			}
			
			set->copies = newCopies;
			newCopies[set->copyCount] = desiredID;
			set->copyCount++;
		}
	}


	closeFile(fPtr);

	if (newSet->firstSprite == NULL)
	{
		loadSpriteIntoSpriteSet("Missing", NULL, newSet, TILE_FAST);
	}

	return newSet;
}

void loadFrameSound(FILE *fPtr, AnimationFrame *newFrame, Animation *anim)
{
	if (newFrame == NULL)
	{
		return;
	}

	char buffer[MAX_LEN] = {0};

	long filePos = ftell(fPtr);
	getNextArg(fPtr, buffer, MAX_LEN);
	stringToLower(buffer);

	if (strcmp(buffer, "playsound:") == 0)
	{
		// load sound
		getNextArg(fPtr, buffer, MAX_LEN);

		int channel = getNextArgInt(fPtr);
		float volume = getNextArgFloat(fPtr);
		if (volume < 0.01)
		{
			volume = 1.0;
		}

		newFrame->soundIndex = addSoundToAnimation(buffer, channel, volume, anim);
	}
	else
	{
		fseek(fPtr, filePos, SEEK_SET);
	}

	return;
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

	int soundIndex = inputData->frameBuffer->soundIndex;
	if (soundIndex > -1 && soundIndex < anim->soundCount)
	{
		SoundMeta *meta = &anim->animationSounds[soundIndex];
		PlaySound(meta->soundName, meta->channel, meta->volume);
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

	if (index < 1)
	{
		inputData->currentAnimation = 0;
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

	int i = 1;
	while (currentAnimation != NULL && strcmp(currentAnimation->name, animationName) != 0 && i < 9999)
	{
		currentAnimation = currentAnimation->nextAnimation;
		i++;
	}

	if (currentAnimation == NULL || currentAnimation->animationData == NULL)
	{
		return -1;
	}

	return i;
}


bool playingAnimation(DisplayData *inputData)
{
	if (inputData == NULL)
	{
		return false;
	}

	return (inputData->currentAnimation != 0);
}

bool playingThisAnimation(DisplayData *inputData, const char name[])
{
	if (inputData == NULL || inputData->currentAnimation == 0 || inputData->animationBuffer == NULL)
	{
		return false;
	}
	return (strcmp(inputData->animationBuffer->name, name) == 0);
}

bool objectPlayingAnimation(Object *input)
{
	return playingAnimation(getDisplay(input));
}

bool objectPlayingThisAnimation(Object *input, const char name[])
{
	return playingThisAnimation(getDisplay(input), name);
}


int iterateAnimation(DisplayData *inputData, float deltaTime)
{
	// loop count decrements every time animation loops; if == 0, then animation value is 0, which stops the animation. 
	// If value is stareted on <= 0, then on animation end value will be less than 0, and it will loop indefinitely

	if (inputData == NULL || inputData->animationBuffer == NULL)
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

	Animation *anim = inputData->animationBuffer;
	float frameRate = anim->frameRate;

	inputData->animationTick += inputData->animationSpeed * deltaTime;

	if (inputData->animationTick < frameRate)
	{
		return EXECUTION_UNNECESSARY;
	}

	while (inputData->animationTick >= frameRate)
	{
		inputData->animationTick -= frameRate;

		if (inputData->frameBuffer->nextFrame == NULL)
		{
			// end of animation, decide to loop or end
			if (inputData->animationLoopCount > -1)	// Change this -1 to a 0 to disable the value of 0 representing loop indefinitely alongside the LOOP_INDEFINITELY enum
			{	
				inputData->animationLoopCount--;
			}

			// At the moment a value of 0 in the play animation function's loopcount will also be the same behaviour as LOOP_INDEFINITELY, 
			// but this may change for consistency in the future
			if (inputData->animationLoopCount == 0 || anim->animationData == NULL)
			{
				inputData->frameBuffer = NULL;
				inputData->currentAnimation = 0;
				return EXECUTION_UNNECESSARY;
			}

			inputData->frameBuffer = anim->animationData;
		}
		else
		{
			inputData->frameBuffer = inputData->frameBuffer->nextFrame;
		}

		inputData->spriteBuffer = inputData->frameBuffer->frameSprite;
		inputData->currentSprite = inputData->spriteBuffer->spriteID;

		int soundIndex = inputData->frameBuffer->soundIndex;
		if (soundIndex > -1 && soundIndex < anim->soundCount)
		{
			SoundMeta *meta = &anim->animationSounds[soundIndex];
			PlaySound(meta->soundName, meta->channel, meta->volume);
		}
	}
	
	return LEMON_SUCCESS;
}


Animation* initialiseNewAnimation(const char animationName[], float frameRate, SpriteSet *inputSet)
{
	if (inputSet == NULL)
	{
		putConsoleError("Missing SpriteSet for new animation!");
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
	newAnimation->frameCount = 0;

	newAnimation->animationSounds = NULL;
	newAnimation->soundCount = 0;

	frameRate = fClamp(frameRate, 0.1, 1000.0);	
	newAnimation->frameRate = 1.0 / frameRate;

	newAnimation->animationID = i;
	memset(newAnimation->name, 0, MAX_LEN);
	strcpy(newAnimation->name, animationName);
	newAnimation->contiguousFrames = false;


	return newAnimation;
}


int addSoundToAnimation(const char name[], float volume, int channel, Animation *anim)
{
	if (anim == NULL)
	{
		return -1;
	}

	if (anim->animationSounds != NULL)
	{
		for (int i = 0; i < anim->soundCount; i++)
		{
			if (strcmp(anim->animationSounds[i].soundName, name) == 0 && anim->animationSounds[i].channel == channel)
			{
				return i;
			}
		}
	}
	
	SoundMeta *newList = malloc((anim->soundCount + 1) * sizeof(SoundMeta));
	if (newList == NULL)
	{
		return -1;
	}

	memcpy(newList, anim->animationSounds, anim->soundCount * sizeof(SoundMeta));
	free(anim->animationSounds);
	anim->animationSounds = newList;

	int index = anim->soundCount;
	anim->soundCount++;

	memset(&newList[index], 0, sizeof(SoundMeta));
	newList[index].volume = volume;
	newList[index].channel = channel;
	strcpy(newList[index].soundName, name);

	return index;
}


AnimationFrame* addSpriteToAnimation(const char spriteName[], Animation *inputAnimation, SpriteSet *sourceSet)
{
	if (sourceSet == NULL || inputAnimation == NULL || spriteName == NULL || strlen(spriteName) >= MAX_LEN)
	{
		return NULL;
	}

	Sprite *currentSprite = getSpriteSpriteSet(spriteName, sourceSet);

	if (currentSprite == NULL)
	{
		putConsoleError("Could not find sprite for animation: %s", spriteName);
		currentSprite = EngineSettings.DefaultTexture;
	}
	else if (EngineSettings.DefaultTexture != NULL && strcmp(spriteName, EngineSettings.DefaultTexture->name) == 0)
	{
		currentSprite = EngineSettings.DefaultTexture;
	}


	AnimationFrame *newFrame;

	if (inputAnimation->contiguousFrames && globalFrame != NULL)
	{
		newFrame = globalFrame;
	}
	else
	{
		newFrame = malloc(sizeof(AnimationFrame));

		if (newFrame == NULL)
		{
			return NULL;
		}

	}

	newFrame->frameSprite = currentSprite;
	newFrame->SpriteXOffset = 0;
	newFrame->SpriteYOffset = 0;
	newFrame->rotation = 0.0;
	newFrame->nextFrame = NULL;
	newFrame->soundIndex = -1;

	inputAnimation->frameCount++;

	AnimationFrame *lastFrame = inputAnimation->animationData;

	if (lastFrame == NULL)
	{
		inputAnimation->animationData = newFrame;
	}
	else
	{
		while (lastFrame->nextFrame != NULL)
		{
			lastFrame = lastFrame->nextFrame;
		}

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

	if (deleteAnimation->contiguousFrames)
	{
		free(currentFrame);
	}
	else
	{
		while (currentFrame != NULL)
		{
			AnimationFrame *tempFrame = currentFrame;
			currentFrame = currentFrame->nextFrame;
			free(tempFrame);
		}
	}

	if (deleteAnimation->animationSounds != NULL)
	{
		free(deleteAnimation->animationSounds);
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

	// test
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

	while (!endOfFile(file) && i < size)
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