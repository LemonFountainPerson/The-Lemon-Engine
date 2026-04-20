#include "LemonEngine.h"


int switchBackGroundSprite(int spriteID, int desiredSetID, BackgroundData *inputData)
{
	if (spriteID < 0 || desiredSetID < 0)
	{
		return INVALID_DATA;
	}

	SpriteSet *currentSet = getSpriteSet(&inputData->bgSpriteSets, desiredSetID);

	if (currentSet == NULL)
	{
		putConsoleError("No sprite set for Backgrounds found");
		return MISSING_DATA;
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
		putConsoleError("Could not find sprite %d for Background set %d", spriteID, desiredSetID);
		return MISSING_DATA;
	}


	inputData->BackgroundSpriteBuffer = currentSprite;

	return LEMON_SUCCESS;
}


int switchBackGroundSpriteName(const char spriteName[], int desiredSetID, BackgroundData *inputData)
{
	SpriteSet *currentSet = getSpriteSet(&inputData->bgSpriteSets, desiredSetID);

	if (currentSet == NULL)
	{
		putConsoleError("Could not find set ID %d from backgrounds", desiredSetID);
		return MISSING_DATA;
	}

	Sprite *currentSprite = currentSet->firstSprite;
	int count = 1;

	while (count < currentSet->spriteCount && currentSprite != NULL && strcmp(currentSprite->name, spriteName) != 0)
	{
		currentSprite = currentSprite->nextSprite;
		count++;
	}

	if (currentSprite == NULL || strcmp(currentSprite->name, spriteName) != 0)
	{
		putConsoleError("Could not find sprite '%s' for Background set %d", spriteName, desiredSetID);
		return MISSING_DATA;
	}


	inputData->BackgroundSpriteBuffer = currentSprite;
	
	return LEMON_SUCCESS;
}


int loadSpriteFromPath(Sprite *inputSprite, const char inputPath[])
{
	static int maxLength = strlen(SPRITE_ROOT) + MAX_LEN;
	if (inputPath == NULL || inputSprite == NULL || strlen(inputPath) >= maxLength)
	{
		return MISSING_DATA;
	}

	int result = LEMON_SUCCESS;

	char path[maxLength + 9];
	strcpy(path, inputPath);

	int pathLength = strlen(path);
	char extensions[][10] = {".png", ".jpeg", ".jpg", ""};	

	for (int attempt = 0; attempt < 4; attempt++)
	{
		strcpy(path + pathLength, extensions[attempt]);
	 	inputSprite->texture = IMG_LoadTexture(ScreenData.Renderer, path);

	 	if (inputSprite->texture != NULL)
	 	{
	 		goto Texture_Loaded;
	 	}
	}

	
	// couldn't find any file
	result = FILE_NOT_FOUND;
	putConsoleError("Image load failed! %s", path);

	strcpy(path + strlen(SPRITE_ROOT), DEFAULT_TEXTURE);

	inputSprite->texture = IMG_LoadTexture(ScreenData.Renderer, path);

	if (inputSprite->texture == NULL)
	{
		putConsoleError("Object Sprite data missing. Is the default texture missing?\n");
		free(inputSprite);

		return LEMON_ERROR;
	}
	

	Texture_Loaded:

	inputSprite->width = inputSprite->texture->w;
	inputSprite->height = inputSprite->texture->h;

	if (inputSprite->width * inputSprite->height > 90000000)	// just in case
	{
		putConsoleError("Object Sprite too large.\n");
		free(inputSprite);

		return LEMON_ERROR;
	}

	SDL_SetTextureScaleMode(inputSprite->texture, DEFAULT_SCALEMODE);


	return result;
}


void initialiseSpriteSetList(SpriteSetList *input)
{
	if (input == NULL)
	{
		return;
	}

	input->start = NULL;

	return;
}

SpriteSet* getSpriteSet(SpriteSetList *setList, int desiredSetID)
{
	if (setList == NULL || setList->start == NULL)
	{
		return NULL;
	}

	SpriteSet *search = setList->start;

	while (search != NULL)
	{
		if (search->setID == desiredSetID)
		{
			return search;
		}

		if (search->copies != NULL)
		{
			for (int i = 0; i < search->copyCount; i++)
			{
				if (search->copies[i] == desiredSetID)
				{
					return search;
				}

				i++;
			}
		}

		search = search->nextSet;
	}

	return NULL;
}


SpriteSet* createNewSpriteSet(SpriteSetList *setList, int desiredSetID)
{
	if (setList == NULL)
	{
		return NULL;
	}

	SpriteSet *newSet = malloc(sizeof(SpriteSet));

	if (newSet == NULL)
	{
		putConsoleError("Could not allocate memory for new sprite set! %d", desiredSetID);
		return NULL;
	}

	SpriteSet *current = setList->start;

	if (current != NULL)
	{
		while (current->prevSet != NULL)
		{
			current = current->prevSet;
		}

		while (current->nextSet != NULL && current->setID < desiredSetID)
		{
			current = current->nextSet;
		}

		if (current->setID < desiredSetID)
		{
			newSet->nextSet = current->nextSet;
			newSet->prevSet = current;

			current->nextSet = newSet;
		}
		else
		{
			newSet->nextSet = current;
			newSet->prevSet = current->prevSet;

			if (current->prevSet != NULL)
			{
				current->prevSet->nextSet = newSet;
			}
			
			current->prevSet = newSet;
		}
	}
	else
	{
		newSet->nextSet = NULL;
		newSet->prevSet = NULL;
	}

	if (newSet->prevSet == NULL)
	{
		setList->start = newSet;
	}
		
	// initialise new set
	newSet->setID = desiredSetID;
	newSet->spriteCount = 0;
	newSet->lastSprite = NULL;
	newSet->firstSprite = NULL;
	newSet->Animations = NULL;
	newSet->copies = NULL;
	newSet->copyCount = 0;
	
	if (DebugSettings.showSpriteset)
	{
		putConsoleStringTS("Creating new spriteset with ID: %d\n", desiredSetID);
	}

	return newSet;
}



Sprite* loadSprite(const char spriteName[], const char folderName[], RenderMode renderMode)
{
	if (spriteName == NULL)
	{
		return NULL;
	}

	int folderLength = 0;
	
	if (folderName != NULL)
	{
		folderLength = strlen(folderName);
	}

	if (strlen(spriteName) >= MAX_LEN + 4 || folderLength >= MAX_LEN)
	{
		return NULL;
	}


	Sprite *newSprite = malloc(sizeof(Sprite));

	if (newSprite == NULL)
	{
		putConsoleError("Could not allocate memory for new sprite! %s", spriteName);
		return NULL;
	}

	char path[strlen(spriteName) + folderLength + strlen(SPRITE_ROOT) + 2];

	strcpy(path, SPRITE_ROOT);
	if (folderName != NULL && strcmp(folderName, "NULL") != 0)
	{
		strcat(path, folderName);
		strcat(path, "/");
	}
	strcat(path, spriteName);

	loadSpriteFromPath(newSprite, path);


	// Assign sprite values
	char buffer[MAX_LEN + 4] = {0};
	removeFileExtension(buffer, spriteName);

	strcpy(newSprite->name, buffer);

	newSprite->RenderMode = renderMode;
	newSprite->nextSprite = NULL;
	newSprite->prevSprite = NULL;
	newSprite->spriteID = 0;


	return newSprite;
}


int loadSpriteIntoDesiredSet(const char spriteName[], const char folderName[], SpriteSetList *spriteSetSource, int desiredSetID, RenderMode renderMode)
{
	SpriteSet *desiredSet = getSpriteSet(spriteSetSource, desiredSetID);

	return loadSpriteIntoSpriteSet(spriteName, folderName, desiredSet, renderMode);
}


int loadSpriteIntoSpriteSet(const char spriteName[], const char folderName[], SpriteSet *inputSet, RenderMode renderMode)
{
	if (inputSet == NULL || spriteName == NULL)
	{
		return MISSING_DATA;
	}

	if (getSpriteIndexSpriteSet(spriteName, inputSet) != -1)
	{
		return EXECUTION_UNNECESSARY;
	}

	Sprite *newSprite = loadSprite(spriteName, folderName, renderMode);

	if (newSprite == NULL)
	{
		return LEMON_ERROR;
	}

	Sprite *currentSpritePtr = inputSet->lastSprite;

	newSprite->prevSprite = currentSpritePtr;

	if (currentSpritePtr == NULL)
	{
		inputSet->firstSprite = newSprite;
		newSprite->spriteID = 1;
		inputSet->spriteCount = 1;
	}
	else
	{
		newSprite->nextSprite = currentSpritePtr->nextSprite;
		if (currentSpritePtr->nextSprite != NULL)
		{
			currentSpritePtr->nextSprite->prevSprite = newSprite;
		}
		currentSpritePtr->nextSprite = newSprite;
		inputSet->spriteCount++;
		newSprite->spriteID = inputSet->spriteCount;
	}

	inputSet->lastSprite = newSprite;
	
	return LEMON_SUCCESS;
}


int getSpriteIndex(const char spriteName[], DisplayData *inputData)
{
	if (inputData == NULL || inputData->spriteSetSource == NULL || spriteName == NULL || strlen(spriteName) >= MAX_LEN)
	{
		return -1;
	}

	if (inputData->spriteBuffer != NULL && strcmp(inputData->spriteBuffer->name, spriteName) == 0)
	{
		return inputData->spriteBuffer->spriteID;
	}

	SpriteSet *currentSet = inputData->spriteSetSource;

	int result = getSpriteIndexSpriteSet(spriteName, currentSet);


	if (result == -1)
	{
		putConsoleError("Could not find sprite %s", spriteName);
	}

	return result;
}

Sprite* getSpriteSpriteSet(const char spriteName[], SpriteSet *currentSet)
{
	if (currentSet == NULL || spriteName == NULL || strlen(spriteName) >= MAX_LEN)
	{
		return NULL;
	}

	// Find correct sprite from sprite set
	Sprite *currentSprite = currentSet->firstSprite;
	int i = 1;

	while (i <= currentSet->spriteCount && currentSprite != NULL && strcmp(currentSprite->name, spriteName) != 0)
	{
		currentSprite = currentSprite->nextSprite;
		i++;
	}

	return currentSprite;
}


int getSpriteIndexSpriteSet(const char spriteName[], SpriteSet *currentSet)
{
	if (currentSet == NULL || spriteName == NULL || strlen(spriteName) >= MAX_LEN)
	{
		return -1;
	}

	// Find correct sprite from sprite set
	Sprite *currentSprite = currentSet->firstSprite;
	int i = 1;

	while (i <= currentSet->spriteCount && currentSprite != NULL && strcmp(currentSprite->name, spriteName) != 0)
	{
		currentSprite = currentSprite->nextSprite;
		i++;
	}


	if (currentSprite == NULL || strcmp(currentSprite->name, spriteName) != 0)
	{
		return -1;
	}


	return currentSprite->spriteID;
}


int switchSprite(int spriteID, int spriteSet, DisplayData *inputData)
{
	if (inputData == NULL)
	{
		return MISSING_DATA;
	}

	if (inputData->spriteSetSource == NULL || spriteID < 1)
	{
		inputData->spriteBuffer = EngineSettings.DefaultTexture;
		inputData->currentSprite = -1;

		return MISSING_DATA;
	}

	if (spriteSet == USE_CURRENT_SPRITESET)
	{
		spriteSet = inputData->spriteSetSource->setID;
	}

	if (inputData->spriteSetSource->setID == spriteSet && inputData->spriteBuffer != NULL && inputData->spriteBuffer->spriteID == spriteID)
	{
		inputData->currentSprite = spriteID;
		return EXECUTION_UNNECESSARY;
	}

	// Find correct sprite set
	SpriteSet *currentSet = inputData->spriteSetSource;

	if (spriteSet != inputData->spriteSetSource->setID && spriteSet > 0)
	{
		if (spriteSet > currentSet->setID)
		{
			while (currentSet->setID != spriteSet && currentSet->nextSet != NULL)
			{
				currentSet = currentSet->nextSet;
			}

		}
		else
		{
			while (currentSet->setID != spriteSet && currentSet->prevSet != NULL)
			{
				currentSet = currentSet->prevSet;
			}
		}

		if (currentSet->setID != spriteSet)
		{
			putConsoleError("Couldn't find sprite set %d", spriteSet);
			return MISSING_DATA;
		}
	}

	// Find correct sprite from sprite set
	Sprite *currentSprite;
	int i;

	if (spriteID > currentSet->spriteCount >> 1)
	{
		// Start from end
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
		// Start from front
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
		if (spriteID != 1)
		{
			putConsoleError("Could not find sprite %d from sprite set %d", spriteID, spriteSet);
		}

		// Switch to Default if it exists
		inputData->spriteBuffer = EngineSettings.DefaultTexture;
		inputData->currentSprite = -1;

		return MISSING_DATA;
	}

	inputData->spriteBuffer = currentSprite;
	inputData->currentSprite = spriteID;
	inputData->spriteSetSource = currentSet;

	return LEMON_SUCCESS;
}


int switchSpriteByName(const char spriteName[], int spriteSet, DisplayData *inputData)
{
	if (inputData == NULL || spriteName == NULL)
	{
		return MISSING_DATA;
	}

	if (strlen(spriteName) > MAX_LEN + 3 || spriteName[0] == 0)
	{
		return INVALID_DATA;
	}

	if (inputData->spriteSetSource == NULL)
	{
		inputData->spriteBuffer = EngineSettings.DefaultTexture;
		inputData->currentSprite = -1;

		return MISSING_DATA;
	}

	if (spriteSet == USE_CURRENT_SPRITESET)
	{
		spriteSet = inputData->spriteSetSource->setID;
	}

	if (inputData->spriteSetSource->setID == spriteSet && inputData->spriteBuffer != NULL && strcmp(inputData->spriteBuffer->name, spriteName) == 0)
	{
		inputData->currentSprite = inputData->spriteBuffer->spriteID;
		return EXECUTION_UNNECESSARY;
	}


	// Find correct sprite set
	SpriteSet *currentSet = inputData->spriteSetSource;

	if (spriteSet != inputData->spriteSetSource->setID && spriteSet > 0)
	{
		if (spriteSet > currentSet->setID)
		{
			while (currentSet->setID != spriteSet && currentSet->nextSet != NULL)
			{
				currentSet = currentSet->nextSet;
			}

		}
		else
		{
			while (currentSet->setID != spriteSet && currentSet->prevSet != NULL)
			{
				currentSet = currentSet->prevSet;
			}
		}

		if (currentSet->setID != spriteSet)
		{
			return MISSING_DATA;
		}
	}


	// remove file extension if it exists
	char desiredSpriteName[MAX_LEN + 4] = {0};
	removeFileExtension(desiredSpriteName, spriteName);


	// Find correct sprite from sprite set
	Sprite *currentSprite;
	currentSprite = currentSet->firstSprite;
	int i = 1;

	while (i < currentSet->spriteCount && currentSprite != NULL && strcmp(currentSprite->name, desiredSpriteName) != 0)
	{
		currentSprite = currentSprite->nextSprite;
		i++;
	}

	
	if (currentSprite == NULL || strcmp(currentSprite->name, desiredSpriteName) != 0)
	{
		// Switch to Default if it exists
		putConsoleError("Could not find sprite '%s' from sprite set %d", desiredSpriteName, spriteSet);

		inputData->spriteBuffer = EngineSettings.DefaultTexture;
		inputData->currentSprite = -1;
		
		return MISSING_DATA;
	}

	inputData->spriteBuffer = currentSprite;
	inputData->currentSprite = currentSprite->spriteID;
	inputData->spriteSetSource = currentSet;

	return LEMON_SUCCESS;
}


int removeFileExtension(char input[], const char fileName[])
{
	if (fileName == NULL || input == NULL)
	{
		return MISSING_DATA;
	}

	strcpy(input, fileName);

	int index = strlen(fileName);
	while (index > 0)
	{
		if (input[index] == '.')
		{
			input[index] = 0;
			index = 0;
		}

		index--;
	}

	return LEMON_SUCCESS;
}



int compareFileNames(const char firstInput[], const char secondInput[])
{
	if (firstInput == NULL || secondInput == NULL)
	{
		return MISSING_DATA;
	}

	int firstLen = strlen(firstInput);
	int secondLen = strlen(secondInput);
	int i = min(firstLen, secondLen);

	while (firstInput[i] != 0 && secondInput[i] != 0 && i > 0)
	{
		if (firstInput[i] == '.' && secondInput[i] == '.')
		{
			firstLen = i;
			secondLen = i;
			i = 0;
		}

		i--;
	}

	if (i != -1)
	{
		return INVALID_DATA;
	}


	char firstStr[MAX_LEN] = {0};
	char secondStr[MAX_LEN] = {0};

	memcpy(firstStr, firstInput, firstLen);
	memcpy(secondStr, secondInput, secondLen);

	return strcmp(firstStr, secondStr);
}



int loadBackGroundSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, BackgroundData *inputData)
{
	if (spriteName == NULL || inputData == NULL)
	{
		return MISSING_DATA;
	}

	SpriteSet *desiredSet = getSpriteSet(&inputData->bgSpriteSets, desiredSetID);

	return loadSpriteIntoSpriteSet(spriteName, "BackGrounds", desiredSet, renderMode);
}


int loadObjectSprite(const char spriteName[], SpriteSet *inputSet, RenderMode renderMode)
{
	if (spriteName == NULL || inputSet == NULL)
	{
		return MISSING_DATA;
	}

	if (inputSet->setID == PARTICLE)
	{
		return loadSpriteIntoSpriteSet(spriteName, "Particles", inputSet, renderMode);
	}
	else if (inputSet->setID == PLAYER_OBJECT)
	{
		return loadSpriteIntoSpriteSet(spriteName, "Player", inputSet, renderMode);
	}
	else
	{
		return loadSpriteIntoSpriteSet(spriteName, "Objects", inputSet, renderMode);
	}
}


int deleteSprite(SpriteSet *spriteSet, Sprite *input)
{
	if (input == NULL)
	{
		return MISSING_DATA;
	}

	Sprite *nextSprite = input->nextSprite;
	Sprite *prevSprite = input->prevSprite;

	if (nextSprite != NULL)
	{
		nextSprite->prevSprite = prevSprite;
	}
	
	if (spriteSet->lastSprite == input)	
	{
		spriteSet->lastSprite = prevSprite;
	}

	if (prevSprite != NULL)
	{
		prevSprite->nextSprite = nextSprite;
	}
	else if (spriteSet != NULL)
	{
		spriteSet->firstSprite = nextSprite;
	}

	SDL_DestroyTexture(input->texture);

	free(input);

	return LEMON_SUCCESS;
}


int deleteSpriteSet(SpriteSet *inputSet, SpriteSetList *setList)
{
	if (inputSet == NULL)
	{
		return MISSING_DATA;
	}

	if (DebugSettings.showSpriteset)
	{
		putConsoleStringTS("Deleting spriteset with ID: %d", inputSet->setID);
	}

	int check = 0;
	while (inputSet->firstSprite != NULL && check < 9999)
	{
		check++;
		deleteSprite(inputSet, inputSet->firstSprite);
	}

	check = 0;
	while (inputSet->Animations != NULL && check < 9999)
	{
		check++;
		deleteAnimation(inputSet, inputSet->Animations);
	}


	SpriteSet *prevSet = inputSet->prevSet;
	SpriteSet *nextSet = inputSet->nextSet;

	if (nextSet != NULL)
	{
		nextSet->prevSet = prevSet;
	}

	if (prevSet != NULL)
	{
		prevSet->nextSet = nextSet;
	}
	else if (setList != NULL)
	{
		setList->start = nextSet;
	}

	free(inputSet);

	return LEMON_SUCCESS;
}

int deleteAllSpriteSets(SpriteSetList *setList)
{
	int i = 0;

	while (setList->start != NULL && i < 9999)
	{
		deleteSpriteSet(setList->start, setList);
		i++;
	}

	return LEMON_SUCCESS;
}


bool setNotBeingUsed(SpriteSet *input, ObjectController *ObjectList)
{
	if (ObjectList != NULL)
	{
		Object *obj = ObjectList->firstObject;

		while (obj != NULL && input != NULL)
		{
			if (obj->ObjectDisplay->spriteSetSource == input)
			{
				// there is an object that is still using this set
				return false;
			}
			obj = obj->nextObject;
		}

		obj = ObjectList->cachedFirstObject;

		while (obj != NULL && input != NULL)
		{
			if (obj->ObjectDisplay->spriteSetSource == input)
			{
				// there is an object that is still using this set
				return false;
			}
			obj = obj->nextObject;
		}
	}

	return true;
}

int deleteExcessSpriteSets(ObjectController *ObjectList, int numberToKeep)
{
	if (ObjectList == NULL || ObjectList->spriteSets.start == NULL)
	{
		return MISSING_DATA;
	}

	SpriteSetList *setList = &ObjectList->spriteSets;
	SpriteSet *currentSet = setList->start;
	int i = 0;

	while (currentSet->nextSet != NULL && i < numberToKeep)
	{
		currentSet = currentSet->nextSet;
		i++;
	}
	
	SpriteSet *delete = currentSet;
	while (currentSet != NULL && i < 9999)
	{
		i++;
		delete = currentSet;
		currentSet = currentSet->nextSet;
		
		if (setNotBeingUsed(delete, ObjectList))
		{
			deleteSpriteSet(delete, setList);
		}
	}

	return LEMON_SUCCESS;
}


RenderMode convertStringToRenderMode(char string[])
{
	if (string == NULL || strlen(string) >= MAX_LEN)
	{
		return UNDEFINED_RENDERMODE;
	}

	if (strcmp(string, "TILE") == 0 || strcmp(string, "TILE_FULL_ALPHA") == 0)
	{
		return TILE;
	}
	else if (strcmp(string, "SINGLE") == 0 || strcmp(string, "SINGLE_FULL_ALPHA") == 0 || strcmp(string, "SINGLE_FAST") == 0)
	{
		return SINGLE;
	}
	else if (strcmp(string, "SCALE") == 0 || strcmp(string, "SCALE_FULL_ALPHA") == 0)
	{
		return SCALE;
	}
	else if (strcmp(string, "TILE_FAST") == 0)
	{
		return TILE_FAST;
	}
	else if (strcmp(string, "STATIC_BACKGROUND") == 0)
	{
		return STATIC_BACKGROUND;
	}
	else if (strcmp(string, "SINGLE_BACKGROUND") == 0)
	{
		return SINGLE_BACKGROUND;
	}
	else if (strcmp(string, "TILEPLANE_BACKGROUND") == 0)
	{
		return TILEPLANE_BACKGROUND;
	}
	else if (strcmp(string, "TILE_BACKGROUND") == 0)
	{
		return TILE_BACKGROUND;
	}
	else 
	{
		return UNDEFINED_RENDERMODE;
	}
}

const char* getRenderModeName(RenderMode input)
{
	switch (input)
	{
	case SINGLE:
		return "SINGLE";

	case TILE:
		return "TILE";

	case TILE_FAST:
		return "TILE_FAST";

	case SCALE:
		return "SCALE";

	case STATIC_BACKGROUND:
		return "STATIC_BACKGROUND";

	case SINGLE_BACKGROUND:
		return "STATIC_BACKGROUND";

	case TILEPLANE_BACKGROUND:
		return "TILEPLANE_BACKGROUND";

	case TILE_BACKGROUND:
		return "TILE_BACKGROUND";

	case DEFAULT_TO_SPRITE:
		return "DEFAULT_TO_SPRITE";

	case DO_NOT_RENDER:
		return "DO_NOT_RENDER";

	default:
		return "UNDEFINED_RENDERMODE";
	}
}