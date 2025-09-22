#include "spriteLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MAX_DIMENSIONS 1 << 20
#include "stb_image.h"


int switchBackGroundSprite(int spriteID, int desiredSetID, World *gameWorld)
{
	SpriteSet *currentSet = gameWorld->BackGrounds;

	if (currentSet == NULL)
	{
		printf("No sprite set for Backgrounds found\n");
		return MISSING_DATA;
	}

	while (currentSet->nextSet != NULL && currentSet->setID != desiredSetID)
	{
		currentSet = currentSet->nextSet;
	}

	if (currentSet->setID != desiredSetID)
	{
		printf("Sprite Set %d not found\n", desiredSetID);
		return MISSING_DATA;
	}

	if (currentSet->spriteCount < 1 || currentSet->firstSprite == NULL)
	{
		printf("Background Sprite set does not contain sprites\n");
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
		printf("Could not find sprite %d for Background set %d\n", spriteID, desiredSetID);
		return MISSING_DATA;
	}


	gameWorld->bgSpriteBuffer = currentSprite;

	return 0;
}


int switchBackGroundSpriteName(const char spriteName[], int desiredSetID, World *gameWorld)
{
	SpriteSet *currentSet;
	currentSet = gameWorld->BackGrounds;

	if (currentSet == NULL)
	{
		printf("No sprite set for Backgrounds found\n");
		fflush(stdout);
		return -1;
	}

	while (currentSet->nextSet != NULL && currentSet->setID != desiredSetID)
	{
		currentSet = currentSet->nextSet;
	}

	if (currentSet->setID != desiredSetID)
	{
		printf("Sprite Set %d not found\n", desiredSetID);
		fflush(stdout);
		return -1;
	}

	if (currentSet->spriteCount < 1 || currentSet->firstSprite == NULL)
	{
		printf("Background Sprite set does not contain sprites\n");
		fflush(stdout);
		currentSet->spriteCount = 0;
		return -1;
	}


	// Find correct sprite from sprite set
	Sprite *currentSprite;
	int i;


	currentSprite = currentSet->lastSprite;
	i = currentSet->spriteCount;

	while (i > 0 && currentSprite != NULL && strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		currentSprite = currentSprite->prevSprite;
		i--;
	}
	

	if (currentSprite == NULL || strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		printf("Could not find sprite %s for Background\n", spriteName);
		fflush(stdout);
		return -1;
	}

	gameWorld->bgSpriteBuffer = currentSprite;

	return 0;
}


int loadSpriteFromPath(Sprite *inputSprite, char path[])
{
	int n = 0;

	inputSprite->spriteData = stbi_load(path, &inputSprite->width, &inputSprite->height, &n, 4);

	if (inputSprite->spriteData != NULL)
	{
		return LEMON_SUCCESS;
	}

	strcat(path, ".png");

	inputSprite->spriteData = stbi_load(path, &inputSprite->width, &inputSprite->height, &n, 4);

	if (inputSprite->spriteData != NULL)
	{
		return LEMON_SUCCESS;
	}

	printf("Image load failed! (%s)\n", path, n);
	fflush(stdout);

	inputSprite->spriteData = stbi_load("LemonData/sprites/Missing.png", &inputSprite->width, &inputSprite->height, &n, 4);


	if (inputSprite->spriteData != NULL)
	{
		return INVALID_DATA;
	}
				
	
	printf("Error: Object Sprite data missing.\n");

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
			printf("Could not allocate memory for new Background sprite set! (%d)\n", desiredSetID);
			fflush(stdout);
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
	

	while (currentSet->prevSet != NULL)
	{
		currentSet = currentSet->prevSet;
	}
	

	while (currentSet->nextSet != NULL && currentSet->setID != desiredSetID)
	{
		currentSet = currentSet->nextSet;
	}


	if (currentSet->setID == desiredSetID)
	{
		(*sourceSet) = currentSet;
		return (*sourceSet);
	}


	// Create sprite set if desired set does not exist
	SpriteSet *newSet = malloc(sizeof(SpriteSet));

	if (newSet == NULL)
	{
		printf("Could not allocate memory for new Background sprite set! (%d)\n", desiredSetID);
		fflush(stdout);
		return NULL;
	}


	if (currentSet->setID > desiredSetID)
	{
		while (currentSet->prevSet != NULL && currentSet->setID > desiredSetID)
		{
			currentSet = currentSet->prevSet;
		}

		if (currentSet->setID > desiredSetID)
		{
			currentSet->prevSet = newSet;
			newSet->nextSet = currentSet;
			newSet->prevSet = NULL;
			*sourceSet = newSet;
		}
		else
		{
			newSet->nextSet = currentSet->nextSet;
			newSet->prevSet = currentSet;
			currentSet->nextSet->prevSet = newSet;
			currentSet->nextSet = newSet;
		}
	}
	else
	{
		currentSet->nextSet = newSet;
		newSet->prevSet = currentSet;
		newSet->nextSet = NULL;
	}

	newSet->setID = desiredSetID;
	newSet->spriteCount = 0;
	newSet->lastSprite = NULL;
	newSet->firstSprite = NULL;

	currentSet = newSet;
	
	(*sourceSet) = currentSet;
	return (*sourceSet);
}


int loadSprite(const char spriteName[], const char folderName[], SpriteSet **spriteSetSource, int desiredSetID, RenderMode renderMode)
{
	if (spriteSetSource == NULL)
	{
		return MISSING_DATA;
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
		printf("Could not allocate memory for new sprite! (%s)\n", spriteName);
		return LEMON_ERROR;
	}

	char path[100];

	strcpy(path, "LemonData/sprites/");

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
	
	return 0;
}


int loadSpriteIntoSpriteSet(const char spriteName[], const char folderName[], SpriteSet *inputSet, RenderMode renderMode)
{
	if (inputSet == NULL)
	{
		return MISSING_DATA;
	}

	Sprite *currentSpritePtr;
	currentSpritePtr = inputSet->lastSprite;

	Sprite *newSprite = malloc(sizeof(Sprite));

	if (newSprite == NULL)
	{
		printf("Could not allocate memory for new sprite! (%s)\n", spriteName);
		fflush(stdout);
		return -1;
	}

	char path[100];

	strcpy(path, "LemonData/sprites/");

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

		printf("Could not find %s in %s\n", spriteName, path);

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
			printf("Couldn't find sprite set %d\n", spriteSet);
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
		//printf("Could not find sprite %d from display data  %d\n", spriteID, i);
		return MISSING_DATA;
	}

	inputData->spriteBuffer = currentSprite;
	inputData->currentSprite = spriteID;
	inputData->spriteSetSource = currentSet;

	return 0;
}


int switchSpriteByName(const char spriteName[MAX_LEN], int spriteSet, DisplayData *inputData)
{
	if (inputData == NULL || inputData->spriteSetSource == NULL)
	{
		return MISSING_DATA;
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
		printf("Could not find sprite '%s' from display data\n", spriteName);
		return MISSING_DATA;
	}

	inputData->spriteBuffer = currentSprite;
	inputData->currentSprite = currentSprite->spriteID;
	inputData->spriteSetSource = currentSet;

	return 0;
}



int loadBackGroundSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, World *gameWorld)
{
	if (gameWorld == NULL)
	{
		return MISSING_DATA;
	}


	setSourceToDesiredSpriteSet(&gameWorld->BackGrounds, desiredSetID);
	
	if (gameWorld->BackGrounds == NULL)
	{
		return MISSING_DATA;
	}

	Sprite *currentSpritePtr;
	currentSpritePtr = gameWorld->BackGrounds->lastSprite;


	Sprite *newSprite = malloc(sizeof(Sprite));

	if (newSprite == NULL)
	{
		printf("Could not allocate memory for new Background sprite! (%s)\n", spriteName);
		fflush(stdout);
		return LEMON_ERROR;
	}

	char path[100] = "LemonData/sprites/Backgrounds/";
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
			gameWorld->BackGrounds->firstSprite = NULL;
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
		gameWorld->BackGrounds->firstSprite = newSprite;
		newSprite->spriteID = 1;
		gameWorld->BackGrounds->spriteCount = 1;
	}
	else
	{
		currentSpritePtr->nextSprite = newSprite;
		newSprite->spriteID = gameWorld->BackGrounds->spriteCount + 1;
		gameWorld->BackGrounds->spriteCount++;
	}

	gameWorld->BackGrounds->lastSprite = newSprite;


	flipRAndBChannels(newSprite);

	return 0;
}



int loadObjectSprite(const char spriteName[], SpriteSet *inputSet, RenderMode renderMode)
{
	if (inputSet == NULL)
	{
		return MISSING_DATA;
	}

	Sprite *currentSpritePtr;
	currentSpritePtr = inputSet->lastSprite;

	Sprite *newSprite = malloc(sizeof(Sprite));

	if (newSprite == NULL)
	{
		printf("Could not allocate memory for new sprite! (%s)\n", spriteName);
		return LEMON_ERROR;
	}

	char path[100];

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
}



int loadPlayerSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, PlayerData *player)
{
	if (player == NULL || spriteName[0] == 0)
	{
		return -1;
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
		printf("Could not allocate memory for new sprite!\n");
		fflush(stdout);
		return -1;
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
	else if (strcmp(string, "BG_ROW_PARALLAX") == 0)
	{
		return BG_ROW_PARALLAX;
	}
	else 
	{
		return UNDEFINED_RENDERMODE;
	}

}