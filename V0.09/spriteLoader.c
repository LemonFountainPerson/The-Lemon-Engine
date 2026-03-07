#include "LemonEngine.h"


int switchBackGroundSprite(int spriteID, int desiredSetID, BackgroundData *inputData)
{
	if (spriteID < 0 || desiredSetID < 0)
	{
		return INVALID_DATA;
	}

	SpriteSet *currentSet = inputData->BackgroundSpriteSet;

	if (currentSet == NULL)
	{
		putConsoleString("\nNo sprite set for Backgrounds found");
		return MISSING_DATA;
	}

	if (desiredSetID != USE_CURRENT_SPRITESET)
	{
		while (currentSet->setID != desiredSetID && currentSet->nextSet != NULL)
		{
			currentSet = currentSet->nextSet;
		}

		if (currentSet->setID != desiredSetID)
		{
			putConsoleStrInt("\nCouldn't find Backgrounds Sprite Set ", desiredSetID);
			return MISSING_DATA;
		}

		if (currentSet->spriteCount < 1 || currentSet->firstSprite == NULL)
		{
			putConsoleString("\nBackground Sprite set does not contain sprites");
			currentSet->spriteCount = 0;
			return MISSING_DATA;
		}
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
		putConsoleStrInt("\nCould not find sprite ", spriteID);
		putConsoleStrInt("for Background set ", desiredSetID);
		return MISSING_DATA;
	}


	inputData->BackgroundSpriteBuffer = currentSprite;

	return LEMON_SUCCESS;
}


int switchBackGroundSpriteName(const char spriteName[], int desiredSetID, BackgroundData *inputData)
{
	createSpriteSet(&inputData->BackgroundSpriteSet, desiredSetID);

	SpriteSet *currentSet = inputData->BackgroundSpriteSet;

	if (currentSet == NULL)
	{
		putConsoleString("\nNo sprite set for Backgrounds found");
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
		putConsoleStrStr("\nCould not find sprite '", spriteName);
		putConsoleStrInt("' for Background set ", desiredSetID);
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
	int extLength;
	char extensions[][10] = {".png", "", ".jpeg", ".jpg"};		// changing the order of this list modifies its priority; eg first it checks without ext, then with .lem, etc.

	for (int attempt = 0; attempt < 4; attempt++)
	{
		extLength = strlen(extensions[attempt]);
		memcpy(path + pathLength, extensions[attempt], extLength);
		path[pathLength + extLength] = 0;
	 	inputSprite->texture = IMG_LoadTexture(ScreenData.Renderer, path);

	 	if (inputSprite->texture != NULL)
	 	{
	 		goto Texture_Loaded;
	 	}
	}

	
	// couldn't find any file
	result = FILE_NOT_FOUND;
	putConsoleStrStr("\nImage load failed! ", path);

	strcpy(path + strlen(SPRITE_ROOT), DEFAULT_TEXTURE);

	inputSprite->texture = IMG_LoadTexture(ScreenData.Renderer, path);

	if (inputSprite->texture == NULL)
	{
		putConsoleString("\nError: Object Sprite data missing. Is the default texture missing?\n");
		free(inputSprite);

		return LEMON_ERROR;
	}
	

	Texture_Loaded:

	inputSprite->width = inputSprite->texture->w;
	inputSprite->height = inputSprite->texture->h;

	if (inputSprite->width * inputSprite->height > 90000000)	// just in case
	{
		putConsoleString("\nError: Object Sprite too large.\n");
		free(inputSprite);

		return LEMON_ERROR;
	}

	SDL_SetTextureScaleMode(inputSprite->texture, DEFAULT_SCALEMODE);


	return result;
}


SpriteSet* createSpriteSet(SpriteSet **setList, int desiredSetID)
{
	SpriteSet *currentSet = (*setList);

	if (currentSet == NULL)
	{
		SpriteSet *newSet = malloc(sizeof(SpriteSet));

		if (newSet == NULL)
		{
			putConsoleStrInt("\nCould not allocate memory for new sprite set! ", desiredSetID);
			return NULL;
		}
		memset(newSet, 0, sizeof(SpriteSet));

		newSet->setID = desiredSetID;
		newSet->spriteCount = 0;
		newSet->nextSet = NULL;
		newSet->prevSet = NULL;
		newSet->lastSprite = NULL; 
		newSet->firstSprite = NULL;
		newSet->Animations = NULL;

		*setList = newSet;
		return newSet;
	}

	if (currentSet->setID == desiredSetID || desiredSetID == USE_CURRENT_SPRITESET)
	{
		return currentSet;
	}


	// Create sprite set if desired set does not exist - find correct slot
	while (currentSet->prevSet != NULL && currentSet->setID != desiredSetID)
	{
		currentSet = currentSet->prevSet;
	}

	while (currentSet->nextSet != NULL && currentSet->setID < desiredSetID)
	{
		currentSet = currentSet->nextSet;
	}

	if (currentSet->setID == desiredSetID)
	{
		// Set the pointer contained in the displaydata to now point to this new set for faster access next time
		(*setList) = currentSet;
		return currentSet;
	}

	SpriteSet *newSet = malloc(sizeof(SpriteSet));

	if (newSet == NULL)
	{
		putConsoleStrInt("\nCould not allocate memory for new sprite set! ", desiredSetID);
		return NULL;
	}
	memset(newSet, 0, sizeof(SpriteSet));
	
	newSet->nextSet = currentSet;
	newSet->prevSet = currentSet->prevSet;

	if (currentSet->prevSet != NULL)
	{
		currentSet->prevSet->nextSet = newSet;
	}
	
	currentSet->prevSet = newSet;
		
	// initialise new set
	newSet->setID = desiredSetID;
	newSet->spriteCount = 0;
	newSet->lastSprite = NULL;
	newSet->firstSprite = NULL;
	newSet->Animations = NULL;
	
	// Set the pointer contained in the displaydata to now point to this new set for faster access next time
	(*setList) = newSet;

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
		putConsoleStrStr("\nCould not allocate memory for new sprite! ", spriteName);
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


int loadSpriteIntoDesiredSet(const char spriteName[], const char folderName[], SpriteSet **spriteSetSource, int desiredSetID, RenderMode renderMode)
{
	createSpriteSet(spriteSetSource, desiredSetID);

	if (spriteSetSource == NULL || *spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}

	if (getSpriteIndexSpriteSet(spriteName, (*spriteSetSource)) != -1)
	{
		return EXECUTION_UNNECESSARY;
	}

	Sprite *newSprite = loadSprite(spriteName, folderName, renderMode);

	if (newSprite == NULL)
	{
		return LEMON_ERROR;
	}


	Sprite *currentSpritePtr = (*spriteSetSource)->lastSprite;

	newSprite->prevSprite = currentSpritePtr;

	if (currentSpritePtr == NULL)
	{
		(*spriteSetSource)->firstSprite = newSprite;
		newSprite->spriteID = 1;
	}
	else
	{
		currentSpritePtr->nextSprite = newSprite;
		newSprite->spriteID = (*spriteSetSource)->spriteCount + 1;
	}

	(*spriteSetSource)->lastSprite = newSprite;

	(*spriteSetSource)->spriteCount++;

	
	return LEMON_SUCCESS;
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
	}
	else
	{
		currentSpritePtr->nextSprite = newSprite;
		newSprite->spriteID = inputSet->spriteCount + 1;
	}

	inputSet->lastSprite = newSprite;

	inputSet->spriteCount++;
	
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
		putConsoleStrStr("\nCould not find sprite ", spriteName);
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


	if (inputData->spriteSetSource->setID == spriteSet && inputData->spriteBuffer != NULL && inputData->spriteBuffer->spriteID == spriteID)
	{
		inputData->currentSprite = spriteID;
		return EXECUTION_UNNECESSARY;
	}

	// Find correct sprite set
	SpriteSet *currentSet = inputData->spriteSetSource;

	if (spriteSet != USE_CURRENT_SPRITESET && spriteSet > 0)
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
			putConsoleStrInt("\nCouldn't find sprite set ", spriteSet);
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
		putConsoleStrInt("\nCould not find sprite ", spriteID);

		if (spriteSet != USE_CURRENT_SPRITESET)
		{
			putConsoleStrInt(" from sprite set ", spriteSet);
		}
		else
		{
			putConsoleStrInt(" from sprite set ", currentSet->setID);
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

	if (inputData->spriteSetSource->setID == spriteSet && inputData->spriteBuffer != NULL && strcmp(inputData->spriteBuffer->name, spriteName) == 0)
	{
		inputData->currentSprite = inputData->spriteBuffer->spriteID;
		return EXECUTION_UNNECESSARY;
	}


	// Find correct sprite set
	SpriteSet *currentSet = inputData->spriteSetSource;

	if (spriteSet != USE_CURRENT_SPRITESET && spriteSet > 0)
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
		putConsoleStrStr("\nCould not find sprite '", desiredSpriteName);
		putConsoleString("' from display data");

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
	if (firstInput == NULL || secondInput == NULL )
	{
		return MISSING_DATA;
	}

	int i = 0;

	while (firstInput[i] != 0 && secondInput[i] != 0 && i < MAX_LEN)
	{
		i++;
	}

	if (firstInput[i] != 0 || secondInput[i] != 0)
	{
		return INVALID_DATA;
	}


	char firstStr[MAX_LEN] = {0};
	char secondStr[MAX_LEN] = {0};

	strcpy(firstStr, firstInput);
	strcpy(secondStr, secondInput);

	i = 0;
	while (i < MAX_LEN && (firstStr[i] != 0 || secondStr[i] != 0))
	{
		if (firstStr[i] == '.')
		{
			firstStr[i] = 0;
		}

		if (secondStr[i] == '.')
		{
			secondStr[i] = 0;
		}

		i++;
	}

	return strcmp(firstStr, secondStr);
}



int loadBackGroundSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, BackgroundData *inputData)
{
	if (spriteName == NULL || inputData == NULL)
	{
		return MISSING_DATA;
	}

	return loadSpriteIntoDesiredSet(spriteName, "BackGrounds", &inputData->BackgroundSpriteSet, desiredSetID, renderMode);
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
	else
	{
		return loadSpriteIntoSpriteSet(spriteName, "Objects", inputSet, renderMode);
	}
}


int loadPlayerSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, PlayerData *player)
{
	if (player == NULL || spriteName == NULL || player->PlayerDisplay == NULL)
	{
		return MISSING_DATA;
	}

	return loadSpriteIntoDesiredSet(spriteName, "Player", &player->PlayerDisplay->spriteSetSource, desiredSetID, renderMode);
}


int deleteSprite(SpriteSet *spriteSet, Sprite *input)
{
	// input is a double pointer so that the pointer value of the input sprite ptr gets incremented to next sprite in chain
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
	else if (spriteSet != NULL)
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


int deleteSpriteSet(SpriteSet *inputSet, ObjectController *ObjectList)
{
	if (inputSet == NULL)
	{
		return MISSING_DATA;
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
	else if (ObjectList != NULL)
	{
		ObjectList->startSpriteSetPtr = nextSet;
	}

	free(inputSet);

	return LEMON_SUCCESS;
}


int deleteAllSpriteSets(ObjectController *ObjectList)
{
	int i = 0;

	while (ObjectList->startSpriteSetPtr != NULL && i < 9999)
	{
		deleteSpriteSet(ObjectList->startSpriteSetPtr, ObjectList);
		i++;
	}

	return LEMON_SUCCESS;
}


int deleteExcessSpriteSets(ObjectController *ObjectList, int numberToKeep)
{
	if (ObjectList == NULL || ObjectList->startSpriteSetPtr == NULL)
	{
		return MISSING_DATA;
	}

	if (numberToKeep < 1)
	{
		return deleteAllSpriteSets(ObjectList);
	}

	SpriteSet *currentSet = ObjectList->startSpriteSetPtr;
	int i = 1;

	while (currentSet->nextSet != NULL && i < numberToKeep)
	{
		currentSet = currentSet->nextSet;
		i++;
	}
	
	while (currentSet->nextSet != NULL && i < 9999)
	{
		i++;
		deleteSpriteSet(currentSet->nextSet, ObjectList);
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