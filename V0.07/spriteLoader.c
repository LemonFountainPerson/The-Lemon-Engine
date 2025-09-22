#include "spriteLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MAX_DIMENSIONS 1 << 20
#include "stb_image.h"


int switchBackGroundSprite(int spriteID, int desiredSetID, BackgroundData *inputData)
{
	SpriteSet *currentSet = inputData->BackgroundSpriteSet;

	if (currentSet == NULL)
	{
		putDebugString("\nNo sprite set for Backgrounds found");
		return MISSING_DATA;
	}

	while (currentSet->nextSet != NULL && currentSet->setID != desiredSetID)
	{
		currentSet = currentSet->nextSet;
	}

	if (currentSet->setID != desiredSetID)
	{
		putDebugStrInt("\nCouldn't find Sprite Set ", desiredSetID);
		return MISSING_DATA;
	}

	if (currentSet->spriteCount < 1 || currentSet->firstSprite == NULL)
	{
		putDebugString("\nBackground Sprite set does not contain sprites");
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
		putDebugStrInt("\nCould not find sprite ", spriteID);
		putDebugStrInt("for Background set ", desiredSetID);
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
		putDebugString("\nNo sprite set for Backgrounds found");
		return MISSING_DATA;
	}

	while (currentSet->nextSet != NULL && currentSet->setID != desiredSetID)
	{
		currentSet = currentSet->nextSet;
	}

	if (currentSet->setID != desiredSetID)
	{
		putDebugStrInt("\nCouldn't find Sprite Set ", desiredSetID);
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
		putDebugStrStr("\nCould not find sprite '", spriteName);
		putDebugStrInt("' for Background set ", desiredSetID);
		return MISSING_DATA;
	}


	inputData->BackgroundSpriteBuffer = currentSprite;
	
	return LEMON_SUCCESS;
}


int loadSpriteFromPath(Sprite *inputSprite, char path[])
{
	int n = 0;

	inputSprite->spriteData = stbi_load(path, &inputSprite->width, &inputSprite->height, &n, 4);

	if (inputSprite->spriteData != NULL)
	{
		if (inputSprite->width > MAX_SPRITE_SIZE || inputSprite->height > MAX_SPRITE_SIZE)
		{
			putDebugString("\nError: Object Sprite too large.");

			free(inputSprite);

			return LEMON_ERROR;
		}
		
		return LEMON_SUCCESS;
	}

	strcat(path, ".png");

	inputSprite->spriteData = stbi_load(path, &inputSprite->width, &inputSprite->height, &n, 4);

	if (inputSprite->spriteData != NULL)
	{
		if (inputSprite->width > MAX_SPRITE_SIZE || inputSprite->height > MAX_SPRITE_SIZE)
		{
			putDebugString("\nError: Object Sprite too large.");

			free(inputSprite);

			return LEMON_ERROR;
		}

		return LEMON_SUCCESS;
	}

	putDebugStrStr("\nImage load failed! ", path);

	inputSprite->spriteData = stbi_load("LemonData/sprites/Missing.png", &inputSprite->width, &inputSprite->height, &n, 4);


	if (inputSprite->spriteData != NULL)
	{
		return FILE_NOT_FOUND;
	}
				
	
	putDebugString("\nError: Object Sprite data missing.");

	free(inputSprite);

	return LEMON_ERROR;
}


int flipRAndBChannels(Sprite *inputSprite)
{
	if (inputSprite == NULL || inputSprite->width < 1 || inputSprite->height < 1)
	{
		return MISSING_DATA;
	}

	// Flip R and B channels
	for (int j = 0; j < inputSprite->height; j++)
	{
		for (int i = 0; i < inputSprite->width; i++)
		{
			unsigned char temp = inputSprite->spriteData[((j << 2) * inputSprite->width) + (i << 2) + 2];

			inputSprite->spriteData[((j << 2) * inputSprite->width) + (i << 2) + 2] = inputSprite->spriteData[((j << 2) * inputSprite->width) + (i << 2)];
			inputSprite->spriteData[((j << 2) * inputSprite->width) + (i << 2)] = temp;
		}
	}

	return 0;
}



SpriteSet* setSourceToDesiredSpriteSet(SpriteSet **sourceSet, int desiredSetID)
{
	SpriteSet *currentSet = *sourceSet;

	if ((*sourceSet) == NULL)
	{
		SpriteSet *newSet = malloc(sizeof(SpriteSet));

		if (newSet == NULL)
		{
			putDebugStrInt("\nCould not allocate memory for new Background sprite set! ", desiredSetID);
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
		putDebugStrInt("\nCould not allocate memory for new Background sprite set! ", desiredSetID);
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


int loadSprite(const char spriteName[], const char folderName[], SpriteSet **spriteSetSource, int desiredSetID, RenderMode renderMode)
{
	if (spriteSetSource == NULL || spriteName == NULL)
	{
		return MISSING_DATA;
	}

	if (strlen(spriteName) >= MAX_LEN + 4 || (folderName != NULL && strlen(folderName) >= MAX_LEN) )
	{
		return INVALID_DATA;
	}
	
	setSourceToDesiredSpriteSet(spriteSetSource, desiredSetID);

	if (*spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}


	Sprite *currentSpritePtr;
	currentSpritePtr = (*spriteSetSource)->lastSprite;

	Sprite *newSprite = malloc(sizeof(Sprite));

	if (newSprite == NULL)
	{
		putDebugStrStr("\nCould not allocate memory for new sprite! ", spriteName);
		return LEMON_ERROR;
	}

	char path[strlen(spriteName) + strlen(folderName) + strlen(SPRITE_ROOT) + 2];

	strcpy(path, SPRITE_ROOT);

	strcat(path, folderName);

	strcat(path, "/");

	strcat(path, spriteName);

	FunctionResult result = loadSpriteFromPath(newSprite, path);

	if (result == LEMON_ERROR)
	{
		if (currentSpritePtr != NULL)
		{
			currentSpritePtr->nextSprite = NULL;
		}
		else
		{
			(*spriteSetSource)->firstSprite = NULL;
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


	flipRAndBChannels(newSprite);
	
	return LEMON_SUCCESS;
}


int loadSpriteIntoSpriteSet(const char spriteName[], const char folderName[], SpriteSet *inputSet, RenderMode renderMode)
{
	if (inputSet == NULL || spriteName == NULL)
	{
		return MISSING_DATA;
	}

	int folderLength = 0;
	
	if (folderName != NULL)
	{
		folderLength = strlen(folderName);
	}

	if (strlen(spriteName) >= MAX_LEN + 4)
	{
		return INVALID_DATA;
	}

	Sprite *currentSpritePtr;
	currentSpritePtr = inputSet->lastSprite;

	Sprite *newSprite = malloc(sizeof(Sprite));

	if (newSprite == NULL)
	{
		putDebugStrStr("\nCould not allocate memory for new sprite! ", spriteName);
		return LEMON_ERROR;
	}

	char path[strlen(spriteName) + folderLength + strlen(SPRITE_ROOT) + 2];

	strcpy(path, SPRITE_ROOT);

	if (folderName != NULL)
	{
		strcat(path, folderName);

		strcat(path, "/");
	}

	strcat(path, spriteName);

	FunctionResult result = loadSpriteFromPath(newSprite, path);

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

		putDebugStrStr("\nCould not find ", spriteName);
		putDebugStrStr(" in ", path);

		return FILE_NOT_FOUND;
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


	flipRAndBChannels(newSprite);
	
	return LEMON_SUCCESS;
}


int switchSprite(int spriteID, int spriteSet, DisplayData *inputData)
{
	if (inputData == NULL || inputData->spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}


	if (inputData->spriteSetSource->setID == spriteSet && inputData->spriteBuffer != NULL && inputData->spriteBuffer->spriteID == spriteID)
	{
		return EXECUTION_UNNECESSARY;
	}

	// Find correct sprite set
	SpriteSet *currentSet = inputData->spriteSetSource;

	if (spriteSet > 0)
	{
		if (spriteSet > currentSet->setID)
		{
			while (currentSet->nextSet != NULL && currentSet->setID != spriteSet)
			{
				currentSet = currentSet->nextSet;
			}

		}
		else
		{
			while (currentSet->prevSet != NULL && currentSet->setID != spriteSet)
			{
				currentSet = currentSet->prevSet;
			}
		}

		if (currentSet->setID != spriteSet)
		{
			putDebugStrInt("\nCouldn't find sprite set ", spriteSet);
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
		putDebugStrInt("\nCould not find sprite ", spriteID);
		putDebugStrInt("from display data ", i);
		return MISSING_DATA;
	}

	inputData->spriteBuffer = currentSprite;
	inputData->currentSprite = spriteID;
	inputData->spriteSetSource = currentSet;

	return 0;
}


int switchSpriteByName(const char spriteName[], int spriteSet, DisplayData *inputData)
{
	if (inputData == NULL || inputData->spriteSetSource == NULL || spriteName == NULL)
	{
		return MISSING_DATA;
	}

	if (strlen(spriteName) >= MAX_LEN || strcmp(spriteName, "") == 0)
	{
		return INVALID_DATA;
	}

	if (inputData->spriteSetSource->setID == spriteSet && inputData->spriteBuffer != NULL && strcmp(inputData->spriteBuffer->spriteName, spriteName) == 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	// Find correct sprite set
	SpriteSet *currentSet = inputData->spriteSetSource;

	if (spriteSet > 0)
	{
		if (spriteSet > currentSet->setID)
		{
			while (currentSet->nextSet != NULL && currentSet->setID != spriteSet)
			{
				currentSet = currentSet->nextSet;
			}

		}
		else
		{
			while (currentSet->prevSet != NULL && currentSet->setID != spriteSet)
			{
				currentSet = currentSet->prevSet;
			}
		}

		if (currentSet->setID != spriteSet)
		{
			return MISSING_DATA;
		}
	}

	// Find correct sprite from sprite set
	Sprite *currentSprite;
	currentSprite = currentSet->firstSprite;
	int i = 1;

	while (i < currentSet->spriteCount && currentSprite != NULL && strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		currentSprite = currentSprite->nextSprite;
		i++;
	}

	
	if (currentSprite == NULL || strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		putDebugStrStr("\nCould not find sprite '", spriteName);
		putDebugString("' from display data");
		return MISSING_DATA;
	}

	inputData->spriteBuffer = currentSprite;
	inputData->currentSprite = currentSprite->spriteID;
	inputData->spriteSetSource = currentSet;

	return 0;
}



int loadBackGroundSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, BackgroundData *inputData)
{
	return loadSprite(spriteName, "BackGrounds", &inputData->BackgroundSpriteSet, desiredSetID, renderMode);

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
		putDebugStrStr("\nCould not allocate memory for new Background sprite! ", spriteName);
		return LEMON_ERROR;
	}

	char path[MAX_LEN + 31] = "LemonData/sprites/Backgrounds/";
	strcat(path, spriteName);


	FunctionResult result = loadSpriteFromPath(newSprite, path);

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


	flipRAndBChannels(newSprite);

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
		putDebugStrStr("\nCould not allocate memory for new sprite! ", spriteName);
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


	FunctionResult result = loadSpriteFromPath(newSprite, path);

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


	flipRAndBChannels(newSprite);
	
	return 0;
	*/
}



int loadPlayerSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, PlayerData *player)
{
	if (player == NULL || spriteName == NULL || player->PlayerDisplay == NULL)
	{
		return MISSING_DATA;
	}

	return loadSprite(spriteName, "Player", &player->PlayerDisplay->spriteSetSource, desiredSetID, renderMode);


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
		putDebugString("\nCould not allocate memory for new sprite!");
		return LEMON_ERROR;
	}

	char path[120] = "LemonData/sprites/Player/";

	strcat(path, spriteName);


	FunctionResult result = loadSpriteFromPath(newSprite, path);

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


	flipRAndBChannels(newSprite);

	return 0;
	*/
}


int deleteSprite(SpriteSet *spriteSet, Sprite **input)
{
	// input is a double pointer so that the pointer value of the input sprite ptr gets incremented to next sprite in chain
	Sprite *spriteToDelete;
	spriteToDelete = (*input);

	if ((*input) == NULL)
	{
		return MISSING_DATA;
	}

	Sprite *prevSprite;
	prevSprite = (*input)->prevSprite;

	(*input) = (*input)->nextSprite;


	if ((*input) != NULL)
	{
		(*input)->prevSprite = prevSprite;
	}
	else
	{
		spriteSet->lastSprite = prevSprite;
	}

	if (prevSprite != NULL)
	{
		prevSprite->nextSprite = (*input);
	}
	else
	{
		spriteSet->firstSprite = (*input);
	}

	stbi_image_free(spriteToDelete->spriteData);

	free(spriteToDelete);

	return 0;
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

	return 0;
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

	return 0;
}


RenderMode convertStringToRenderMode(char string[])
{
	int i = 0;
	while (string[i] != 0 && i < MAX_LEN)
	{
		i++;
	}

	if (i >= MAX_LEN)
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
	/*	UNIMPLEMENTED - See entries in data.h
	else if (strcmp(string, "TILE_SCALE") == 0)
	{
		return TILE_SCALE;
	}
	else if (strcmp(string, "TILE_SCALE_FULL_ALPHA") == 0)
	{
		return TILE_SCALE_FULL_ALPHA;
	}
	else if (strcmp(string, "SCALE_TILE") == 0)
	{
		return SCALE_TILE;
	}
	else if (strcmp(string, "SCALE_TILE_FULL_ALPHA") == 0)
	{
		return SCALE_TILE_FULL_ALPHA;
	}
	*/
	else if (strcmp(string, "BG_ROW_PARALLAX") == 0)
	{
		return BG_ROW_PARALLAX;
	}
	else 
	{
		return UNDEFINED_RENDERMODE;
	}

}