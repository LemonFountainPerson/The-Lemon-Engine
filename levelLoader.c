#ifndef IS_DEFINED
#include "gameObjects.h"
#include "spriteLoader.h"
#endif
#include "levelLoader.h"


void insertHLine(int level[GRID_WIDTH][GRID_HEIGHT], int i, int j, int itemID, int length)
{
	if (i < 0 || j < 0|| i >= GRID_WIDTH || j >= GRID_HEIGHT)
	{
		return;
	}

	for (int k = 0; k < length && k + i < GRID_WIDTH; k++)
	{
		level[i + k][j] = itemID;
	}

	return;
}


void insertVLine(int level[GRID_WIDTH][GRID_HEIGHT], int i, int j, int itemID, int length)
{
	if (i < 0 || j < 0 || i >= GRID_WIDTH || j >= GRID_HEIGHT)
	{
		return;
	}

	for (int k = 0; k < length && j + k < GRID_HEIGHT; k++)
	{
		level[i][j + k] = itemID;
	}

	return;
}


int switchLevel(World *gameWorld, int level)
{
	deleteAllObjects(gameWorld->objectList);

	loadLevel(gameWorld, level);

	gameWorld->level = level;

	return 0;
}


int loadDefault(World *gameWorld)
{
	addObject(gameWorld->objectList, 2900, 110, 0, 1, 1);
	addObject(gameWorld->objectList, 2940, 110, 0, 1, 1);
	addObject(gameWorld->objectList, 2970, 110, 0, 1, 1);

	addObject(gameWorld->objectList, 200, 100, 5, 1, 1);
	addObject(gameWorld->objectList, 300, 130, 5, 1, 1);
	addObject(gameWorld->objectList, 400, 160, 5, 1, 1);
	addObject(gameWorld->objectList, 450, 200, 5, 1, 1);

	addMovingPlatform(gameWorld->objectList, 1800, 200, 7, 1800, 2800, 6, 90);
	addMovingPlatform(gameWorld->objectList, 2600, 350, 7, 2600, 3600, 6, 60);
	addMovingPlatform(gameWorld->objectList, 8000, 128, 8, 176, 960, 6, 180);
	addMovingPlatform(gameWorld->objectList, 5700, 128, 8, 176, 960, 6, 180);
	addMovingPlatform(gameWorld->objectList, 3200, 100, 7, 3300, 4200, 6, 90);

	addObject(gameWorld->objectList, 3200, 70, 9, 26, 0);

	switchBackGroundSprite(0, 0, gameWorld);

	return 0;
}


int convertTxtToLem(char FileName[MAX_LEN], FILE *txtPtr)
{
	char newFileName[MAX_LEN + 4];
	strcpy(newFileName, FileName);
	strcat(newFileName, ".lem");

	FILE *newLemFile;
	newLemFile= fopen(newFileName, "wb");

	if (newLemFile == NULL)
	{
		printf("Couldn't open file (%s)! \n", newFileName);
		return -1;
	}
	printf("Couldn't open file (%s)! \n", newFileName);


	fclose(newLemFile);

	return 0;
}


int saveLevel(World *gameWorld)
{
	// BROKEN
	FILE *fPtr;

	char path[96] = "LemonData/LevelData/Level0Data.txt";
	path[25] = gameWorld->level + 48;

	fPtr = fopen(path, "wb");

	if (fPtr == NULL)
	{
		printf("Could not save level %d\n", gameWorld->level);
		return -1;
	}


	fwrite("V1____", sizeof(char), 6, fPtr);

	Object *currentObject;
	currentObject = gameWorld->objectList->firstObject;

	char buffer[16] = {0};

	while (currentObject != NULL)
	{
		switch(currentObject->objectID)
		{
			case 7:
			case 8:
				fwrite("MOV-", sizeof(char), 4, fPtr);
				break;

			default:
				fwrite("OBJ-", sizeof(char), 4, fPtr);
				break;
		}

		fwrite("__", sizeof(char), 2, fPtr);

		int size = convertIntToStr(buffer, currentObject->objectID);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite("__", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentObject->xPos);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite("__", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentObject->yPos);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite("__", sizeof(char), 2, fPtr);
		

		switch(currentObject->objectID)
		{
			case 1:
				size = convertIntToStr(buffer, (int)(currentObject->xSize/X_TILESCALE));

				fwrite(buffer, sizeof(char), size, fPtr);

				fwrite("__", sizeof(char), 2, fPtr);

				size = convertIntToStr(buffer, (int)(currentObject->ySize/Y_TILESCALE));

				fwrite(buffer, sizeof(char), size, fPtr);
				break;

			default:
				
				break;
		}

		fwrite("____", sizeof(char), 4, fPtr);

		currentObject = currentObject->nextObject;
	}

	fwrite("ENDF", sizeof(char), 4, fPtr);

	fclose(fPtr);

	return 0;
}


int loadLevel(World *gameWorld, int level)
{
	FILE *fPtr;

	char charBuffer[16] = "Level0Data.txt";
	charBuffer[5] = level + 48;

	char path[96] = "LemonData/LevelData/";
	strcat(path, charBuffer);

	fPtr = fopen(path, "rb");

	if (fPtr == NULL)
	{
		printf("Level %d not found.\n", level);
		return -1;
	}

	// Read version number
	fread(charBuffer, sizeof(char), 2, fPtr);
	charBuffer[2] = 0;

	if (strcmp(charBuffer, "V1") != 0)
	{
		printf("Level %d load failed: Incompatible version number!\n");
		return -1;
	}

	int endOfFile = 0;
	int i = 0;

	while (endOfFile == 0 && i < 8000)
	{
		fread(charBuffer, sizeof(char), 4, fPtr);
		charBuffer[4] = 0;

		if (strcmp(charBuffer, "////") != 0)
		{
			printf("Level %d load failed: Data formatted incorrectly! %c %c %c %c\n", level, charBuffer[0], charBuffer[1], charBuffer[2], charBuffer[3]);
			return -1;
		}

		fread(charBuffer, sizeof(char), 7, fPtr);
		charBuffer[7] = 0;

		if (strcmp(charBuffer, "ENDFILE") == 0)
		{
			endOfFile = 1;
			printf("\nSuccessfully read file (%s)!\n\n", path);
		}
		else if (strcmp(charBuffer, "OBJECT-") == 0)
		{
			loadObject(gameWorld, fPtr);
		}
		else if (strcmp(charBuffer, "OBJMOV-") == 0)
		{
			loadMovingPlatform(gameWorld, fPtr);
		}	
		else if (strcmp(charBuffer, "OBJREP-") == 0)
		{
			loadRepeatingObject(gameWorld, fPtr);
		}	
		else if (strcmp(charBuffer, "LVFLAG-") == 0)
		{
			loadLevelFlag(gameWorld, fPtr);
		}
		else
		{
			printf("Unrecognised data!\n");
			return -1;
		}
		

		i++;
	}


	fclose(fPtr);

	return 0;
}


int loadLevelFlag(World *gameWorld, FILE *fPtr)
{
	char buffer[16] = {0};

	getNextArg(fPtr, buffer, 15, 0);

	// Flag Decoded
	if (strcmp(buffer, "BGSET") == 0)
	{
		int spriteValue = getNextArg(fPtr, buffer, 4, 1);

		int setValue = getNextArg(fPtr, buffer, 4, 1);
	
		switchBackGroundSprite(spriteValue, setValue, gameWorld);

		return 0;
	}

	if (strcmp(buffer, "BGSET_TRIGGER") == 0)
	{
		int spriteValue = getNextArg(fPtr, buffer, 4, 1);

		int setValue = getNextArg(fPtr, buffer, 4, 1);

		int xValue = getNextArg(fPtr, buffer, 5, 1);

		int yValue = getNextArg(fPtr, buffer, 5, 1);
	
		addFlagObject(gameWorld->objectList, BACKGROUND_SET_TRIGGER, xValue, yValue, spriteValue, setValue, 0, 0, 0);

		return 0;
	}



	return -1;
}


int loadRepeatingObject(World *gameWorld, FILE *fPtr)
{
	char buffer[8] = {0};


	int xCount = getNextArg(fPtr, buffer, 3, 1);

	int yCount = getNextArg(fPtr, buffer, 3, 1);

	int xOffset = getNextArg(fPtr, buffer, 4, 1);

	int yOffset = getNextArg(fPtr, buffer, 4, 1);

	if (xCount == -1 || yCount == -1 || xOffset == -1 || yOffset == -1)
	{
		return -1;
	}


	//fflush(fPtr);
	unsigned long objectPosition = ftell(fPtr);
			
	for (int yIter = 0; yIter < yCount; yIter++)
	{
		for (int xIter = 0; xIter < xCount; xIter++)
		{		
			fseek(fPtr, objectPosition, SEEK_SET);

			loadObjectOffset(gameWorld, fPtr, xOffset * xIter, yOffset * yIter);
		}
	}

	return 0;
}


int loadObject(World *gameWorld, FILE *fPtr)
{
	char buffer[8] = {0};

	// ID
	int objectID = getNextArg(fPtr, buffer, 4, 1);

	if (objectID == -1)
	{
		return -1;
	}

	// X and Y co-ords
	int XPos = getNextArg(fPtr, buffer, 6, 1);

	if (XPos == -1)
	{
		return -1;
	}

	int YPos = getNextArg(fPtr, buffer, 6, 1);

	if (YPos == -1)
	{
		return -1;
	}


	// args
	unsigned long objectPosition = ftell(fPtr);

	int arg1 = getNextArg(fPtr, buffer, 6, 1);

	if (arg1 == -1)
	{
		return -1;
	}

	if (strcmp(buffer, "////") == 0)
	{
		addObject(gameWorld->objectList, objectID, XPos, YPos, 0, 0);
		fseek(fPtr, objectPosition, SEEK_SET);
		return 0;
	}

	objectPosition = ftell(fPtr);

	int arg2 = getNextArg(fPtr, buffer, 6, 1);

	if (arg2 == -1)
	{
		return -1;
	}

	if (strcmp(buffer, "////") == 0)
	{
		addObject(gameWorld->objectList, objectID, XPos, YPos, 0, 0);
		fseek(fPtr, objectPosition, SEEK_SET);
		return 0;
	}

	addObject(gameWorld->objectList, objectID, XPos, YPos, arg1, arg2);	

	return 0;
}


int loadObjectOffset(World *gameWorld, FILE *fPtr, int xOffset, int yOffset)
{
	char buffer[8] = {0};

	// ID
	int objectID = getNextArg(fPtr, buffer, 4, 1);

	if (objectID == -1)
	{
		return -1;
	}

	// X and Y co-ords
	int XPos = getNextArg(fPtr, buffer, 6, 1);

	if (XPos == -1)
	{
		return -1;
	}

	int YPos = getNextArg(fPtr, buffer, 6, 1);

	if (YPos == -1)
	{
		return -1;
	}

	// args
	unsigned long objectPosition = ftell(fPtr);

	int arg1 = getNextArg(fPtr, buffer, 6, 1);

	if (arg1 == -1)
	{
		return -1;
	}

	if (strcmp(buffer, "////") == 0)
	{
		addObject(gameWorld->objectList, objectID, XPos + xOffset, YPos + yOffset, 0, 0);
		fseek(fPtr, objectPosition, SEEK_SET);
		return 0;
	}

	objectPosition = ftell(fPtr);

	int arg2 = getNextArg(fPtr, buffer, 6, 1);

	if (arg2 == -1)
	{
		return -1;
	}

	if (strcmp(buffer, "////") == 0)
	{
		addObject(gameWorld->objectList, objectID, XPos + xOffset, YPos + yOffset, arg1, 0);
		fseek(fPtr, objectPosition, SEEK_SET);
		return 0;
	}

	addObject(gameWorld->objectList, objectID, XPos + xOffset, YPos + yOffset, arg1, arg2);	

	return 0;
}


int loadMovingPlatform(World *gameWorld, FILE *fPtr)
{
	char buffer[8] = {0};

	// ID
	int objectID =  getNextArg(fPtr, buffer, 4, 1);

	if (objectID == -1)
	{
		return -1;
	}

	// X and Y co-ords
	int XPos = getNextArg(fPtr, buffer, 6, 1);

	if (XPos == -1)
	{
		return -1;
	}

	int YPos = getNextArg(fPtr, buffer, 6, 1);

	if (YPos == -1)
	{
		return -1;
	}

	// args
	int bound1 = getNextArg(fPtr, buffer, 6, 1);

	if (bound1 == -1)
	{
		return -1;
	}

	int bound2 =  getNextArg(fPtr, buffer, 6, 1);

	if (bound2 == -1)
	{
		return -1;
	}

	int speed = getNextArg(fPtr, buffer, 6, 1);

	if (speed == -1)
	{
		return -1;
	}

	int timer = getNextArg(fPtr, buffer, 6, 1);

	if (timer == -1)
	{
		return -1;
	}

	addMovingPlatform(gameWorld->objectList, objectID, XPos, YPos, bound1, bound2, speed, timer);	

	return 0;
}


int getNextArg(FILE *fPtr, char buffer[], int max, int convertToInteger)
{
	int i = 0;

	do
	{
		fread(buffer, sizeof(char), 1, fPtr);

		i++;
	} while (buffer[0] == '_' && i < max);

	if (i > 3)
	{
		printf("Object data badly formatted! %c\n", buffer[0]);
		return -1;
	}


	i = 0;
	
	while (buffer[i] != '_' && i < max)
	{
		i++;
		fread(buffer + i, sizeof(char), 1, fPtr);
	}

	fread(buffer + i, sizeof(char), 1, fPtr);

	if (buffer[i] != '_')
	{
		printf("Object data badly formatted!\n");
		return -1;
	}

	buffer[i] = 0;

	printf("Buffer: (%s) Length of arg: %d\n", buffer, i);


	if (convertToInteger == 1)
	{
		return convertStrToInt(buffer, i);
	}

	return i;
}


int convertStrToInt(char str[], int size)
{
	int input = 0;
	int polarity = 1;
	
	int i = 0;
	

	while (i < size)
	{
		if (inRange(str[i], 48, 58) == 0)
		{
			input *= 10;
			input += (str[i] - 48);
		}
		else if (str[i] == 45 && input == 0)
		{
			polarity *= -1;
		}
		else
		{
			i = size;
		}
		
		i++;
	}

	return input * polarity;
}


int convertIntToStr(char str[], int input)
{
	int inputBuffer = input;
	int polarity = 1;
	
	int i = 0;

	int count = (int)((ceil(log10(abs(input))) + 1) * sizeof(char));

	if (input < 0)
	{
		i++;
		str[0] = '-';
	}

	sprintf(str + i, "%d", count);

	str[i + count] = 0;
	
	printf("%d - Converted str: %s\n", input, str);

	return count;
}


int inRange(int input, int low, int high)
{
	// printf("%d %d %d\n", input, low, high);
	if (input < low || input > high)
	{
		return -1;
	}
	else {
		return 0;
	}
	
}