#define STB_IMAGE_IMPLEMENTATION
#define STBI_MAX_DIMENSIONS 1 << 20
#include "stb_image.h"

#include "spriteLoader.h"


int switchBackGroundSprite(int spriteID, int desiredSetID, World *gameWorld)
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
		printf("Could not find sprite %d for Background\n", spriteID);
		fflush(stdout);
		return -1;
	}

	gameWorld->bgSpriteBuffer = currentSprite;

	return 0;
}


int switchBackGroundSpriteName(char spriteName[], int desiredSetID, World *gameWorld)
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


int loadBackGroundSprite(char spriteName[], int desiredSetID, int tileMode, World *gameWorld)
{
	if (gameWorld == NULL)
	{
		printf("Gameworld is NULL\n");
		fflush(stdout);
		return -1;
	}


	SpriteSet *currentSet;
	currentSet = gameWorld->BackGrounds;

	if (currentSet == NULL)
	{
		SpriteSet *newSet = malloc(sizeof(SpriteSet));

		if (newSet == NULL)
		{
			printf("Could not allocate memory for new Background sprite set! (%d)\n", desiredSetID);
			fflush(stdout);
			return -1;
		}

		newSet->setID = desiredSetID;
		newSet->spriteCount = 0;
		newSet->nextSet = NULL;
		newSet->prevSet = NULL;
		newSet->lastSprite = NULL; 
		newSet->firstSprite = NULL;

		gameWorld->BackGrounds = newSet;
		currentSet = newSet;
	}
	else
	{
		while (currentSet->nextSet != NULL && currentSet->setID != desiredSetID)
		{
			currentSet = currentSet->nextSet;
		}


		if (currentSet->setID != desiredSetID)
		{
			SpriteSet *newSet = malloc(sizeof(SpriteSet));

			if (newSet == NULL)
			{
				printf("Could not allocate memory for new Background sprite set! (%d)\n", desiredSetID);
				fflush(stdout);
				return -1;
			}

			newSet->setID = desiredSetID;
			newSet->spriteCount = 0;
			newSet->nextSet = NULL;
			newSet->prevSet = currentSet;
			newSet->lastSprite = NULL;
			newSet->firstSprite = NULL;

			currentSet->nextSet = newSet;
			currentSet = newSet;

		}
	}

	

	Sprite *currentSpritePtr;
	currentSpritePtr = currentSet->lastSprite;


	Sprite *newSprite = malloc(sizeof(Sprite));

	if (newSprite == NULL)
	{
		printf("Could not allocate memory for new Background sprite! (%s)\n", spriteName);
		fflush(stdout);
		return -1;
	}

	int spriteWidth, spriteHeight, n;
	char path[100] = "LemonData/sprites/Backgrounds/";
	strcat(path, spriteName);

	newSprite->spriteData = stbi_load(path, &spriteWidth, &spriteHeight, &n, 4);

	// Load image
	if (newSprite->spriteData == NULL)
	{
		strcat(path, ".png");

		newSprite->spriteData = stbi_load(path, &spriteWidth, &spriteHeight, &n, 4);

		if (newSprite->spriteData == NULL)
		{
			printf("BackGround %d Image load failed! (%s)\n", currentSet->setID, path);
			fflush(stdout);

			newSprite->spriteData = stbi_load("LemonData/sprites/Backgrounds/BG_Missing.png", &spriteWidth, &spriteHeight, &n, 4);

			if (newSprite->spriteData == NULL)
			{
				printf("Error: BackGround Sprite data missing.\n");

				free(newSprite);

				if (currentSpritePtr != NULL)
				{
					currentSpritePtr->nextSprite = NULL;
				}
				else
				{
					currentSet->firstSprite = NULL;
					printf("Set to NULL\n\n\n");
				}

				return -1;
			}
		}
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

	newSprite->height = spriteHeight;
	newSprite->width = spriteWidth;
	newSprite->nextSprite = NULL;
	newSprite->prevSprite = currentSpritePtr;
	newSprite->RenderMode = tileMode;


	if (currentSpritePtr == NULL)
	{
		currentSet->firstSprite = newSprite;
		newSprite->spriteID = 1;
		currentSet->spriteCount = 1;
	}
	else
	{
		currentSpritePtr->nextSprite = newSprite;
		newSprite->spriteID = currentSet->spriteCount + 1;
		currentSet->spriteCount++;
	}

	currentSet->lastSprite = newSprite;


	// Flip R and B channels
	for (int j = 0; j < spriteHeight; j++)
	{
		for (int i = 0; i < spriteWidth; i++)
		{
			unsigned char temp = newSprite->spriteData[((j << 2) * spriteWidth) + (i << 2) + 2];
			newSprite->spriteData[((j << 2) * spriteWidth) + (i << 2) + 2] = newSprite->spriteData[((j << 2) * spriteWidth) + (i << 2)];
			newSprite->spriteData[((j << 2) * spriteWidth) + (i << 2)] = temp;
		}
	}

	return 0;
}



int loadObjectSprite(char spriteName[], SpriteSet *inputSet, int tileMode)
{
	Sprite *currentSpritePtr;
	currentSpritePtr = inputSet->lastSprite;


	Sprite *newSprite = malloc(sizeof(Sprite));

	if (newSprite == NULL)
	{
		printf("Could not allocate memory for new sprite! (%s)\n", spriteName);
		fflush(stdout);
		return -1;
	}

	int spriteWidth, spriteHeight, n;
	char path[100] = "LemonData/sprites/Objects/";
	strcat(path, spriteName);

	newSprite->spriteData = stbi_load(path, &spriteWidth, &spriteHeight, &n, 4);

	if (newSprite->spriteData == NULL)
	{
		strcat(path, ".png");

		newSprite->spriteData = stbi_load(path, &spriteWidth, &spriteHeight, &n, 4);

		if (newSprite->spriteData == NULL)
		{
			printf("Object %d Image load failed! (%s)\n", inputSet->setID, path);
			fflush(stdout);

			newSprite->spriteData = stbi_load("LemonData/sprites/Objects/OBJ_Missing.png", &spriteWidth, &spriteHeight, &n, 4);

			if (newSprite->spriteData == NULL)
			{
				printf("Error: Object Sprite data missing.\n");

				free(newSprite);

				if (currentSpritePtr != NULL)
				{
					currentSpritePtr->nextSprite = NULL;
				}
				else
				{
					inputSet->firstSprite = NULL;
					printf("Set to NULL\n\n\n");
				}

				return -1;
			}
		}
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

	newSprite->height = spriteHeight;
	newSprite->width = spriteWidth;
	newSprite->nextSprite = NULL;
	newSprite->prevSprite = currentSpritePtr;
	newSprite->RenderMode = tileMode;

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


	// Flip R and B channels
	for (int j = 0; j < spriteHeight; j++)
	{
		for (int i = 0; i < spriteWidth; i++)
		{
			unsigned char temp = newSprite->spriteData[((j << 2) * spriteWidth) + (i << 2) + 2];
			newSprite->spriteData[((j << 2) * spriteWidth) + (i << 2) + 2] = newSprite->spriteData[((j << 2) * spriteWidth) + (i << 2)];
			newSprite->spriteData[((j << 2) * spriteWidth) + (i << 2)] = temp;
		}
	}
	
	return 0;
}



int loadPlayerSprite(char spriteName[], int desiredSetID, int tileMode, PlayerData *player)
{
	if (player == NULL || spriteName[0] == 0)
	{
		return -1;
	}

	// Search for desired sprite set or create new sprite set if needed
	SpriteSet *currentSpriteSet;
	currentSpriteSet = player->spriteSetPtr;

	if (currentSpriteSet != NULL)
	{
		while (currentSpriteSet->nextSet != NULL && currentSpriteSet->setID != desiredSetID)
		{
			currentSpriteSet = currentSpriteSet->nextSet;
		}

		if (currentSpriteSet->setID != desiredSetID)
		{
			SpriteSet *newSetPtr = malloc(sizeof(SpriteSet));
			currentSpriteSet->nextSet = newSetPtr;

			if (newSetPtr == NULL)
			{
				printf("Error: Could not allocate space for player sprite set %d.\n", desiredSetID);
				fflush(stdout);
				free(newSetPtr);
				return -1;
			}

			newSetPtr->setID = desiredSetID;
			newSetPtr->spriteCount = 0;
			newSetPtr->prevSet = NULL;
			newSetPtr->nextSet = NULL;
			newSetPtr->firstSprite = NULL;
			newSetPtr->lastSprite = NULL;

			currentSpriteSet = newSetPtr;
		}
	}
	else
	{
		SpriteSet *newSetPtr = malloc(sizeof(SpriteSet));
		player->spriteSetPtr = newSetPtr;

		if (newSetPtr == NULL)
		{
			printf("Error: Could not allocate space for player sprite set %d.\n", desiredSetID);
			fflush(stdout);
			free(newSetPtr);
			return -1;
		}

		newSetPtr->setID = desiredSetID;
		newSetPtr->spriteCount = 0;
		newSetPtr->prevSet = NULL;
		newSetPtr->nextSet = NULL;
		newSetPtr->firstSprite = NULL;
		newSetPtr->lastSprite = NULL;

		currentSpriteSet = newSetPtr;
	}

	// Go to end of sprite set
	Sprite *currentPtr;
	currentPtr = currentSpriteSet->firstSprite;

	int i = 1;

	if (currentPtr != NULL)
	{
		while (currentPtr->nextSprite != NULL && i < currentSpriteSet->spriteCount)
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

	int spriteWidth, spriteHeight, n;
	char path[120] = "LemonData/sprites/Player/";

	strcat(path, spriteName);

	newSprite->spriteData = stbi_load(path, &spriteWidth, &spriteHeight, &n, 4);

	if (newSprite->spriteData == NULL)
	{
		strcat(path, ".png");

		newSprite->spriteData = stbi_load(path, &spriteWidth, &spriteHeight, &n, 4);

		if (newSprite->spriteData == NULL)
		{
			printf("Player Image load failed! (%s)\n", path);
			fflush(stdout);

			newSprite->spriteData = stbi_load("LemonData/sprites/Player/PLAYER_Missing.png", &spriteWidth, &spriteHeight, &n, 4);

			strcpy(newSprite->spriteName, "Missing");


			if (newSprite->spriteData == NULL)
			{
				printf("Error: Player Sprite data missing.\n");

				free(newSprite);

				if (currentPtr != NULL)
				{
					currentPtr->nextSprite = NULL;
				}
				else
				{
					player->spriteSetPtr->firstSprite = NULL;
				}

				return -1;
			}
		}

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

	newSprite->height = spriteHeight;
	newSprite->width = spriteWidth;
	newSprite->nextSprite = NULL;
	newSprite->prevSprite = currentPtr;
	newSprite->RenderMode = tileMode;

	if (currentPtr == NULL)
	{
		currentSpriteSet->firstSprite = newSprite;
		newSprite->spriteID = 1;
	}
	else
	{
		currentPtr->nextSprite = newSprite;
		newSprite->spriteID = i + 1;
	}

	currentSpriteSet->lastSprite = newSprite;

	currentSpriteSet->spriteCount = i + 1;


	// Flip R and B channels
	for (int j = 0; j < spriteHeight; j++)
	{
		for (int i = 0; i < spriteWidth; i++)
		{
			unsigned char temp = newSprite->spriteData[((j << 2) * spriteWidth) + (i << 2) + 2];
			newSprite->spriteData[((j << 2) * spriteWidth) + (i << 2) + 2] = newSprite->spriteData[((j << 2) * spriteWidth) + (i << 2)];
			newSprite->spriteData[((j << 2) * spriteWidth) + (i << 2)] = temp;
		}
	}

	return 0;
}


void deleteSprite(SpriteSet *spriteSet, Sprite **input)
{
	// input is a double pointer so that the pointer value of the input sprite ptr gets incremented to next sprite in chain
	Sprite *spriteToDelete;
	spriteToDelete = (*input);

	if ((*input) == NULL)
	{
		return;
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

	return;
}
