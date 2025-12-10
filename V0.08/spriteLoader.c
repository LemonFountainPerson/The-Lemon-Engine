#include "spriteLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MAX_DIMENSIONS MAX_SPRITE_SIZE
#include "stb_image.h"


int switchBackGroundSprite(int spriteID, int desiredSetID, BackgroundData *inputData)
{
	SpriteSet *currentSet = inputData->BackgroundSpriteSet;

	if (currentSet == NULL)
	{
		putConsoleString("\nNo sprite set for Backgrounds found");
		return MISSING_DATA;
	}

	while (currentSet->nextSet != NULL && currentSet->setID != desiredSetID)
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
	SpriteSet *currentSet = inputData->BackgroundSpriteSet;

	if (currentSet == NULL)
	{
		putConsoleString("\nNo sprite set for Backgrounds found");
		return MISSING_DATA;
	}

	while (currentSet->nextSet != NULL && currentSet->setID != desiredSetID)
	{
		currentSet = currentSet->nextSet;
	}

	if (currentSet->setID != desiredSetID)
	{
		putConsoleStrInt("\nCouldn't find Backgrounds Sprite Set ", desiredSetID);
		return MISSING_DATA;
	}

	Sprite *currentSprite = currentSet->firstSprite;
	int count = 1;

	while (count < currentSet->spriteCount && currentSprite != NULL && strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		currentSprite = currentSprite->nextSprite;
		count++;
	}

	if (currentSprite == NULL || strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		putConsoleStrStr("\nCould not find sprite '", spriteName);
		putConsoleStrInt("' for Background set ", desiredSetID);
		return MISSING_DATA;
	}


	inputData->BackgroundSpriteBuffer = currentSprite;
	
	return LEMON_SUCCESS;
}


int loadSpriteFromPath(Sprite *inputSprite, char path[])
{
	if (path == NULL || inputSprite == NULL || strlen(path) < 5)
	{
		return MISSING_DATA;
	}

	char newPath[strlen(SPRITE_ROOT) + (MAX_LEN << 1) + 6];
	strcpy(newPath, path);
	path = newPath;

	if (strcmp(path + strlen(path) - 4, ".png") != 0)
	{
		strcat(path, ".png");		
	}

	inputSprite->texture = IMG_LoadTexture(ScreenData.Renderer, path);
	SDL_SetTextureScaleMode(inputSprite->texture, SDL_SCALEMODE_NEAREST);

	if (inputSprite->texture != NULL)
	{
		inputSprite->width = inputSprite->texture->w;
		inputSprite->height = inputSprite->texture->h;

		if (inputSprite->width > MAX_SPRITE_SIZE || inputSprite->height > MAX_SPRITE_SIZE)
		{
			putConsoleString("\nError: Object Sprite too large.\n");
		}
		else
		{
			return LEMON_SUCCESS;
		}
	}


	putConsoleStrStr("\nImage load failed! ", path);

	strcpy(path, SPRITE_ROOT);
	strcat(path, DEFAULT_TEXTURE);

	inputSprite->texture = IMG_LoadTexture(ScreenData.Renderer, path);
	SDL_SetTextureScaleMode(inputSprite->texture, SDL_SCALEMODE_NEAREST);

	if (inputSprite->texture != NULL)
	{
		return FILE_NOT_FOUND;
	}
				
	putConsoleString("\nError: Object Sprite data missing.\n");

	free(inputSprite);

	return LEMON_ERROR;
}



SpriteSet* setSourceToDesiredSpriteSet(SpriteSet **sourceSet, int desiredSetID)
{
	if (sourceSet == NULL)
	{
		return NULL;
	}

	if ((*sourceSet) != NULL && (*sourceSet)->setID == desiredSetID)
	{
		return (*sourceSet);
	}

	
	SpriteSet *currentSet = (*sourceSet);

	if ((*sourceSet) == NULL)
	{
		SpriteSet *newSet = malloc(sizeof(SpriteSet));

		if (newSet == NULL)
		{
			putConsoleStrInt("\nCould not allocate memory for new sprite set! ", desiredSetID);
			return NULL;
		}

		newSet->setID = desiredSetID;
		newSet->spriteCount = 0;
		newSet->nextSet = NULL;
		newSet->prevSet = NULL;
		newSet->lastSprite = NULL; 
		newSet->firstSprite = NULL;

		*sourceSet = newSet;
		return (*sourceSet);
	}

	while (currentSet->nextSet != NULL && currentSet->setID != desiredSetID)
	{
		currentSet = currentSet->nextSet;
	}

	if (currentSet->setID != desiredSetID)
	{
		currentSet = *sourceSet;

		while (currentSet->prevSet != NULL && currentSet->setID != desiredSetID)
		{
			currentSet = currentSet->prevSet;
		}
	}

	// If found desired set, return a pointer to it
	if (currentSet->setID == desiredSetID)
	{
		// Set the pointer contained in the displaydata to now point to this new set for faster access next time
		(*sourceSet) = currentSet;
		return (*sourceSet);
	}


	// Create sprite set if desired set does not exist - find correct slot
	while (currentSet->nextSet != NULL && currentSet->nextSet->setID < desiredSetID)
	{
		currentSet = currentSet->nextSet;
	}

	SpriteSet *newSet = malloc(sizeof(SpriteSet));

	if (newSet == NULL)
	{
		putConsoleStrInt("\nCould not allocate memory for new sprite set! ", desiredSetID);
		return NULL;
	}

	
	newSet->nextSet = currentSet->nextSet;
	newSet->prevSet = currentSet;

	if (currentSet->nextSet != NULL)
	{
		currentSet->nextSet->prevSet = newSet;
	}
	
	currentSet->nextSet = newSet;
		
	// initialise new set
	newSet->setID = desiredSetID;
	newSet->spriteCount = 0;
	newSet->lastSprite = NULL;
	newSet->firstSprite = NULL;
	
	// Set the pointer contained in the displaydata to now point to this new set for faster access next time
	(*sourceSet) = newSet;

	return (*sourceSet);
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

	strcpy(newSprite->spriteName, buffer);

	newSprite->RenderMode = renderMode;
	newSprite->nextSprite = NULL;
	newSprite->prevSprite = NULL;
	newSprite->spriteID = 0;


	return newSprite;
}


int loadSpriteIntoDesiredSet(const char spriteName[], const char folderName[], SpriteSet **spriteSetSource, int desiredSetID, RenderMode renderMode)
{
	Sprite *newSprite = loadSprite(spriteName, folderName, renderMode);

	if (newSprite == NULL)
	{
		return LEMON_ERROR;
	}

	setSourceToDesiredSpriteSet(spriteSetSource, desiredSetID);

	if (spriteSetSource == NULL || *spriteSetSource == NULL)
	{
		return MISSING_DATA;
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

	if (inputData->spriteBuffer != NULL && strcmp(inputData->spriteBuffer->spriteName, spriteName) == 0)
	{
		return inputData->spriteBuffer->spriteID;
	}

	SpriteSet *currentSet = inputData->spriteSetSource;

	// Find correct sprite from sprite set
	Sprite *currentSprite = currentSet->firstSprite;
	int i = 1;

	while (i < currentSet->spriteCount && currentSprite != NULL && strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		currentSprite = currentSprite->nextSprite;
		i++;
	}


	if (currentSprite == NULL || strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		putConsoleStrStr("\nCould not find sprite ", spriteName);

		return -1;
	}


	return currentSprite->spriteID;
}


int switchSprite(int spriteID, int spriteSet, DisplayData *inputData)
{
	if (inputData == NULL || inputData->spriteSetSource == NULL)
	{
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

		// Switch to Default if it exists
		inputData->spriteBuffer = EngineSettings.DefaultTexture;
		inputData->currentSprite = 0;

		return MISSING_DATA;
	}

	inputData->spriteBuffer = currentSprite;
	inputData->currentSprite = spriteID;
	inputData->spriteSetSource = currentSet;

	return LEMON_SUCCESS;
}


int switchSpriteByName(const char spriteName[], int spriteSet, DisplayData *inputData)
{
	if (inputData == NULL || inputData->spriteSetSource == NULL || spriteName == NULL)
	{
		return MISSING_DATA;
	}

	if (strlen(spriteName) > MAX_LEN + 3 || spriteName[0] == 0)
	{
		return INVALID_DATA;
	}

	if (inputData->spriteSetSource->setID == spriteSet && inputData->spriteBuffer != NULL && strcmp(inputData->spriteBuffer->spriteName, spriteName) == 0)
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

	while (i < currentSet->spriteCount && currentSprite != NULL && strcmp(currentSprite->spriteName, desiredSpriteName) != 0)
	{
		currentSprite = currentSprite->nextSprite;
		i++;
	}

	
	if (currentSprite == NULL || strcmp(currentSprite->spriteName, desiredSpriteName) != 0)
	{
		// Switch to Default if it exists
		putConsoleStrStr("\nCould not find sprite '", desiredSpriteName);
		putConsoleString("' from display data");

		inputData->spriteBuffer = EngineSettings.DefaultTexture;
		inputData->currentSprite = 0;
		
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
	return loadSpriteIntoDesiredSet(spriteName, "BackGrounds", &inputData->BackgroundSpriteSet, desiredSetID, renderMode);

	/*
	if (inputData == NULL || spriteName == NULL)
	{
		return MISSING_DATA;
	}

	if (strlen(spriteName) >= MAX_LEN)
	{
		return INVALID_DATA;
	}

	setSourceToDesiredSpriteSet(&inputData->BackgroundSpriteSet, desiredSetID);
	
	if (inputData->BackgroundSpriteSet == NULL)
	{
		return MISSING_DATA;
	}

	Sprite *currentSpritePtr;
	currentSpritePtr = inputData->BackgroundSpriteSet->lastSprite;


	Sprite *newSprite = malloc(sizeof(Sprite));

	if (newSprite == NULL)
	{
		putConsoleStrStr("\nCould not allocate memory for new Background sprite! ", spriteName);
		return LEMON_ERROR;
	}

	char path[MAX_LEN + 31] = "LemonData/sprites/Backgrounds/";
	strcat(path, spriteName);


	FuncResult result = loadSpriteFromPath(newSprite, path);

	if (result == LEMON_ERROR)
	{
		if (currentSpritePtr != NULL)
		{
			currentSpritePtr->nextSprite = NULL;
		}
		else
		{
			inputData->BackgroundSpriteSet->firstSprite = NULL;
		}

		return LEMON_ERROR;
	}


	// Assign sprite name
	char buffer[MAX_LEN + 4];
	strcpy(buffer, spriteName);

	for (int k = MAX_LEN + 3; k > 0; k--)
	{
		if (buffer[k] == '.')
		{
			buffer[k] = 0;
			k = 0;
		}
	}

	strcpy(newSprite->spriteName, buffer);

	newSprite->nextSprite = NULL;
	newSprite->prevSprite = currentSpritePtr;
	newSprite->RenderMode = renderMode;


	if (currentSpritePtr == NULL)
	{
		inputData->BackgroundSpriteSet->firstSprite = newSprite;
		newSprite->spriteID = 1;
		inputData->BackgroundSpriteSet->spriteCount = 1;
	}
	else
	{
		currentSpritePtr->nextSprite = newSprite;
		newSprite->spriteID = inputData->BackgroundSpriteSet->spriteCount + 1;
		inputData->BackgroundSpriteSet->spriteCount++;
	}

	inputData->BackgroundSpriteSet->lastSprite = newSprite;


	return 0;
	*/
}



int loadObjectSprite(const char spriteName[], SpriteSet *inputSet, RenderMode renderMode)
{
	if (inputSet->setID == PARTICLE)
	{
		return loadSpriteIntoSpriteSet(spriteName, "Particles", inputSet, renderMode);
	}
	else
	{
		return loadSpriteIntoSpriteSet(spriteName, "Objects", inputSet, renderMode);
	}
	
	/*
	if (inputSet == NULL || spriteName == NULL)
	{
		return MISSING_DATA;
	}

	if (strlen(spriteName) >= MAX_LEN)
	{
		return INVALID_DATA;
	}

	Sprite *currentSpritePtr;
	currentSpritePtr = inputSet->lastSprite;

	Sprite *newSprite = malloc(sizeof(Sprite));

	if (newSprite == NULL)
	{
		putConsoleStrStr("\nCould not allocate memory for new sprite! ", spriteName);
		return LEMON_ERROR;
	}

	char path[MAX_LEN + 29];

	// If spriteset given is for particles, search in particles folder instead of objects
	if (inputSet->setID == PARTICLE)
	{
		strcpy(path, "LemonData/sprites/Particles/");
	}
	else{
		strcpy(path, "LemonData/sprites/Objects/");
	}

	strcat(path, spriteName);


	FuncResult result = loadSpriteFromPath(newSprite, path);

	if (result == LEMON_ERROR)
	{
		if (currentSpritePtr != NULL)
		{
			currentSpritePtr->nextSprite = NULL;
		}
		else
		{
			inputSet->firstSprite = NULL;
		}

		return LEMON_ERROR;
	}


	// Assign sprite name
	char buffer[MAX_LEN + 4];
	strcpy(buffer, spriteName);

	for (int k = MAX_LEN + 3; k > 0; k--)
	{
		if (buffer[k] == '.')
		{
			buffer[k] = 0;
			k = 0;
		}
	}


	strcpy(newSprite->spriteName, buffer);
	newSprite->nextSprite = NULL;
	newSprite->prevSprite = currentSpritePtr;
	newSprite->RenderMode = renderMode;

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
	
	return 0;
	*/
}



int loadPlayerSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, PlayerData *player)
{
	if (player == NULL || spriteName == NULL || player->PlayerDisplay == NULL)
	{
		return MISSING_DATA;
	}

	return loadSpriteIntoDesiredSet(spriteName, "Player", &player->PlayerDisplay->spriteSetSource, desiredSetID, renderMode);


	/*
	if (strlen(spriteName) >= MAX_LEN)
	{
		return INVALID_DATA;
	}

	// Search for desired sprite set or create new sprite set if needed
	setSourceToDesiredSpriteSet(&player->PlayerDisplay->spriteSetSource, desiredSetID);

	if (player->PlayerDisplay->spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}

	SpriteSet *currentSet = player->PlayerDisplay->spriteSetSource;

	// Go to end of sprite set
	Sprite *currentPtr;
	currentPtr = currentSet->firstSprite;

	int i = 1;

	if (currentPtr != NULL)
	{
		while (currentPtr->nextSprite != NULL && i < currentSet->spriteCount)
		{
			currentPtr = currentPtr->nextSprite;
			i++;
		}
	}

	// Create new sprite
	Sprite *newSprite = malloc(sizeof(Sprite));

	if (newSprite == NULL)
	{
		putConsoleString("\nCould not allocate memory for new sprite!");
		return LEMON_ERROR;
	}

	char path[120] = "LemonData/sprites/Player/";

	strcat(path, spriteName);


	FuncResult result = loadSpriteFromPath(newSprite, path);

	if (result == LEMON_ERROR)
	{
		if (currentPtr != NULL)
		{
			currentPtr->nextSprite = NULL;
		}
		else
		{
			currentSet->firstSprite = NULL;
		}

		return LEMON_ERROR;
	}


	// Assign sprite name
	char buffer[MAX_LEN + 4];
	strcpy(buffer, spriteName);

	for (int k = MAX_LEN + 3; k > 0; k--)
	{
		if (buffer[k] == '.')
		{
			buffer[k] = 0;
			k = 0;
		}
	}

	strcpy(newSprite->spriteName, buffer);

	newSprite->nextSprite = NULL;
	newSprite->prevSprite = currentPtr;
	newSprite->RenderMode = renderMode;

	if (currentPtr == NULL)
	{
		currentSet->firstSprite = newSprite;
		newSprite->spriteID = 1;
	}
	else
	{
		currentPtr->nextSprite = newSprite;
		newSprite->spriteID = i + 1;
	}

	currentSet->lastSprite = newSprite;

	currentSet->spriteCount = i + 1;


	return 0;
	*/
}


int deleteSprite(SpriteSet *spriteSet, Sprite **input)
{
	// input is a double pointer so that the pointer value of the input sprite ptr gets incremented to next sprite in chain
	if ((*input) == NULL)
	{
		return MISSING_DATA;
	}

	Sprite *spriteToDelete = (*input);

	Sprite *prevSprite = (*input)->prevSprite;

	(*input) = (*input)->nextSprite;


	if ((*input) != NULL)
	{
		(*input)->prevSprite = prevSprite;
	}
	else if (spriteSet != NULL)
	{
		spriteSet->lastSprite = prevSprite;
	}

	if (prevSprite != NULL)
	{
		prevSprite->nextSprite = (*input);
	}
	else if (spriteSet != NULL)
	{
		spriteSet->firstSprite = (*input);
	}

	SDL_DestroyTexture(spriteToDelete->texture);

	free(spriteToDelete);

	return LEMON_SUCCESS;
}


int deleteSpriteSet(SpriteSet *inputSet, ObjectController *ObjectList)
{
	if (inputSet == NULL)
	{
		return MISSING_DATA;
	}

	Sprite *currentSprite = inputSet->firstSprite;

	while (currentSprite != NULL)
	{
		deleteSprite(inputSet, &currentSprite);
	}
	
	Animation *currentAnimation = inputSet->Animations;

	while (currentSprite != NULL)
	{
		Animation *deleteAnim = currentAnimation;
		currentAnimation = currentAnimation->nextAnimation;
		deleteAnimation(deleteAnim);
	}

	if (inputSet->nextSet != NULL)
	{
		inputSet->nextSet->prevSet = inputSet->prevSet;
	}

	if (inputSet->prevSet != NULL)
	{
		inputSet->prevSet->nextSet = inputSet->nextSet;
	}
	else if (ObjectList != NULL)
	{
		ObjectList->startSpriteSetPtr = inputSet->nextSet;
	}

	free(inputSet);

	return LEMON_SUCCESS;
}


int deleteExcessSpriteSets(ObjectController *ObjectList, int numberToKeep)
{
	if (ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	SpriteSet *currentSet = ObjectList->startSpriteSetPtr;
	int i = 0;

	while (currentSet != NULL && i < numberToKeep)
	{
		currentSet = currentSet->nextSet;
		i++;
	}

	SpriteSet *tempSet;
	
	while (currentSet != NULL)
	{
		tempSet = currentSet;
		currentSet = currentSet->nextSet;

		deleteSpriteSet(tempSet, ObjectList);
	}

	return LEMON_SUCCESS;
}


RenderMode convertStringToRenderMode(char string[])
{
	if (string == NULL || strlen(string) >= MAX_LEN)
	{
		return UNDEFINED_RENDERMODE;
	}

	if (strcmp(string, "TILE") == 0)
	{
		return TILE;
	}
	else if (strcmp(string, "SINGLE") == 0)
	{
		return SINGLE;
	}
	else if (strcmp(string, "SINGLE_FULL_ALPHA") == 0)
	{
		return SINGLE_FULL_ALPHA;
	}
	else if (strcmp(string, "SINGLE_FAST") == 0)
	{
		return SINGLE_FAST;
	}
	else if (strcmp(string, "TILE_FULL_ALPHA") == 0)
	{
		return TILE_FULL_ALPHA;
	}
	else if (strcmp(string, "TILE_FAST") == 0)
	{
		return TILE_FAST;
	}
	else if (strcmp(string, "SCALE") == 0)
	{
		return SCALE;
	}
	else if (strcmp(string, "SCALE_FULL_ALPHA") == 0)
	{
		return SCALE_FULL_ALPHA;
	}
	else if (strcmp(string, "STATIC_BACKGROUND") == 0)
	{
		return STATIC_BACKGROUND;
	}
	else if (strcmp(string, "SINGLE_BACKGROUND") == 0)
	{
		return SINGLE_BACKGROUND;
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

	case SINGLE_FAST:
		return "SINGLE_FAST";

	case SINGLE_FULL_ALPHA:
		return "SINGLE_FULL_ALPHA";

	case TILE:
		return "TILE";

	case TILE_FAST:
		return "TILE_FAST";

	case TILE_FULL_ALPHA:
		return "TILE_FULL_ALPHA";

	case SCALE:
		return "SCALE";

	case SCALE_FULL_ALPHA:
		return "SCALE_FULL_ALPHA";

	case STATIC_BACKGROUND:
		return "STATIC_BACKGROUND";

	case SINGLE_BACKGROUND:
		return "STATIC_BACKGROUND";

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