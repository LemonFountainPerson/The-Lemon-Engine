#include "levelLoader.h"


int switchLevel(World *gameWorld, int level)
{
	if (gameWorld == NULL)
	{
		return MISSING_DATA;
	}

	gameWorld->GameState = LOADING;

	gameWorld->drawPlayer = 1;
	gameWorld->drawBackGround = 1;
	gameWorld->drawObjects = 1;
	gameWorld->drawParticles = 1;
	gameWorld->drawHud = 1;

	deleteAllObjects(gameWorld->ObjectList);

	if (loadLevel(gameWorld, level) != LEMON_SUCCESS)
	{
		gameWorld->drawPlayer = 0;
		gameWorld->drawBackGround = 0;
		gameWorld->GameState = EMPTY_GAME;
		return INVALID_DATA;
	}

	gameWorld->level = level;

	gameWorld->GameState = GAMEPLAY;

	ResetPlayer(gameWorld->Player);

	return 0;
}


int loadDefault(World *gameWorld)
{
	AddObject(gameWorld, 2900, 110, 0, 1, 1, 0, 0, 0);
	AddObject(gameWorld, 2940, 110, 0, 1, 1, 0, 0, 0);
	AddObject(gameWorld, 2970, 110, 0, 1, 1, 0, 0, 0);

	AddObject(gameWorld, 200, 100, 5, 1, 1, 0, 0, 0);
	AddObject(gameWorld, 300, 130, 5, 1, 1, 0, 0, 0);
	AddObject(gameWorld, 400, 160, 5, 1, 1, 0, 0, 0);
	AddObject(gameWorld, 450, 200, 5, 1, 1, 0, 0, 0);

	AddObject(gameWorld, 1800, 200, 7, 1800, 2800, 6, 90, 0);
	AddObject(gameWorld, 2600, 350, 7, 2600, 3600, 6, 60, 0);
	AddObject(gameWorld, 8000, 128, 8, 176, 960, 6, 180, 0);
	AddObject(gameWorld, 5700, 128, 8, 176, 960, 6, 180, 0);
	AddObject(gameWorld, 3200, 100, 7, 3300, 4200, 6, 90, 0);

	AddObject(gameWorld, 3200, 70, 9, 26, 0, 0, 0, 0);

	switchBackGroundSprite(0, 0, gameWorld);


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
	currentObject = gameWorld->ObjectList->firstObject;

	char buffer[16] = {0};

	while (currentObject != NULL)
	{
		switch(currentObject->ObjectID)
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

		int size = convertIntToStr(buffer, currentObject->ObjectID);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite("__", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentObject->ObjectBox->xPos);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite("__", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentObject->ObjectBox->yPos);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite("__", sizeof(char), 2, fPtr);
		

		switch(currentObject->ObjectID)
		{
			case 1:
				size = convertIntToStr(buffer, (int)(currentObject->ObjectBox->xSize/X_TILESCALE));

				fwrite(buffer, sizeof(char), size, fPtr);

				fwrite("__", sizeof(char), 2, fPtr);

				size = convertIntToStr(buffer, (int)(currentObject->ObjectBox->ySize/Y_TILESCALE));

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


int lemon_Modulo(int x, int N)
{
	if (N < 1)
	{
		return 0;
	}

	return ((x % N) + N) % N;
}


FILE* convertTxtToLem(char FileName[MAX_LEN], FILE *txtPtr)
{
	if (txtPtr == NULL)
	{
		return NULL;
	}

	fseek(txtPtr, 0, SEEK_SET);

	char newFileName[MAX_LEN + 4];
	strcpy(newFileName, FileName);
	strcat(newFileName, ".lem");

	FILE *newLemFile;
	newLemFile = fopen(newFileName, "wb");

	if (newLemFile == NULL)
	{
		printf("Couldn't open file (%s)! \n", newFileName);
		return NULL;
	}


	int i = 0;
	char currentBlock[8] = {0};

	while(i < 800000 && feof(txtPtr) == 0)
	{
		fread(currentBlock, sizeof(char), 2, txtPtr);

		if (feof(txtPtr))
		{
			i = 9999;
			continue;
		}


		int offset = (i + 256) % ENCRYPT_OFFSET;

		currentBlock[0] = lemon_Modulo(currentBlock[0] + offset, 128);	// 100, 4
		currentBlock[1] = lemon_Modulo(currentBlock[1] - offset, 128);

		printf("Decoded: %d %d %d %c %c \n\n", i, currentBlock[0], currentBlock[1], currentBlock[0], currentBlock[1]);

		fwrite(currentBlock, sizeof(char), 2, newLemFile);

		i++;
	}

	fclose(newLemFile);

	return txtPtr;
}


FILE* convertLemToTxt(char fileName[MAX_LEN + 4], FILE *lemPtr)
{
	if (lemPtr == NULL)
	{
		return NULL;
	}

	fseek(lemPtr, 0, SEEK_SET);

	int fileExt;
	for (int i = 0; i < MAX_LEN - 1; i++)
	{
		if (fileName[i] == '.')
		{
			fileExt = i;
			i = MAX_LEN;
		}
	}

	if (fileName[fileExt] != '.')
	{
		return NULL;
	}

	if (fileName[fileExt + 1] != 'l' || fileName[fileExt + 2] != 'e' || fileName[fileExt + 3] != 'm' || fileName[fileExt + 4] != 0)
	{
		return NULL;
	}


	FILE *newTextFile;

	newTextFile = fopen(fileName, "wb");

	if (newTextFile == NULL)
	{
		return NULL;
	}


	int i = 0;
	char currentBlock[8] = {0};

	while(i < 800000 && feof(lemPtr) == 0)
	{
		fread(currentBlock, sizeof(char), 2, lemPtr);

		if (feof(lemPtr))
		{
			i = 9999;
			continue;
		}

		int offset = (i + 256) % ENCRYPT_OFFSET;

		currentBlock[0] = lemon_Modulo(currentBlock[0] - offset, 128);   
		currentBlock[1] = lemon_Modulo(currentBlock[1] + offset, 128);

		//printf("0 and 1: %d %d %c %c \n\n", currentBlock[0], currentBlock[1], currentBlock[0], currentBlock[1]);

		fwrite(currentBlock, sizeof(char), 2, newTextFile);

		i++;
	}

	fclose(lemPtr);

	return newTextFile;
}


int loadLevel(World *gameWorld, int level)
{
	gameWorld->GameState = LOADING;

	FILE *fPtr;

	char charBuffer[MAX_LEN + 4] = "Level0Data.lem";
	charBuffer[5] = level + 48;

	char path[96] = "LemonData/LevelData/";
	strcat(path, charBuffer);

	fPtr = fopen(path, "rb");

	if (fPtr == NULL)
	{
		strcpy(charBuffer, "Level0Data.txt");
		charBuffer[5] = level + 48;

		strcpy(path, "LemonData/LevelData/");
		strcat(path, charBuffer);

		fPtr = fopen(path, "rb");

		if (fPtr == NULL)
		{
			printf("Level %d not found.\n", level);
			gameWorld->GameState = GAMEPLAY;
			return MISSING_DATA;
		}
	}
	else
	{
		fPtr = convertLemToTxt(charBuffer, fPtr);

		if (fPtr == NULL)
		{
			printf("Could not convert Lem to txt!\n");
			return ERROR;
		}
	}


	// Read version number
	fread(charBuffer, sizeof(char), 5, fPtr);
	charBuffer[5] = 0;

	if (strcmp(charBuffer, LEMON_VERSION) != 0)
	{
		printf("Level %d load failed: Incompatible version number! %s\n", charBuffer);
		fclose(fPtr);
		gameWorld->GameState = GAMEPLAY;
		return INVALID_DATA;
	}


	// Read data type
	fread(charBuffer, sizeof(char), 10, fPtr);
	charBuffer[10] = 0;

	if (strcmp(charBuffer, "-LEVELDATA") != 0)
	{
		printf("Level %d load failed: This file does not contain level data!\n");
		fclose(fPtr);
		gameWorld->GameState = GAMEPLAY;
		return INVALID_DATA;
	}


	if (gameWorld->Player != NULL)
	{
		ResetPlayer(gameWorld->Player);
	}

	int endOfFile = 0;
	int i = 0;


	while (endOfFile == 0 && i < 8000)
	{
		getNextArg(fPtr, charBuffer, 4);
		charBuffer[4] = 0;

		if (strcmp(charBuffer, "////") != 0)
		{
			printf("Level %d load failed: Data formatted incorrectly! %c %c %c %c\n", level, charBuffer[0], charBuffer[1], charBuffer[2], charBuffer[3]);
			clearCurrentlyLoadedLevelData(gameWorld);
			fclose(fPtr);
			return INVALID_DATA;
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
			clearCurrentlyLoadedLevelData(gameWorld);
			fclose(fPtr);
			return INVALID_DATA;
		}
		

		i++;
	}


	strcpy(charBuffer, "Level0Data");
	charBuffer[5] = level + 48;
	charBuffer[10] = 0;
	//convertTxtToLem(charBuffer, fPtr);

	fclose(fPtr);


	// Create a default particle object to initilise its sprite set to avoid lag during gameplay 
	AddObject(gameWorld, PARTICLE, 0, 0, SPARKLE, 0, 0, 0, 0);

	gameWorld->GameState = GAMEPLAY;

	return LEMON_SUCCESS;
}


int clearCurrentlyLoadedLevelData(World *gameWorld)
{
	if (gameWorld == NULL || gameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	deleteAllObjects(gameWorld->ObjectList);

	gameWorld->level = 0;
	gameWorld->GameState = IN_MENU;

	return 0;
}


int loadLevelFlag(World *gameWorld, FILE *fPtr)
{
	char buffer[20] = {0};

	getNextArg(fPtr, buffer, 19);

	// Flag Decoded
	if (strcmp(buffer, "SET-BG") == 0)
	{
		int args[3] = {0};

		int returnMsg = readMultipleIntArgs(fPtr, args, 2, 4);

		if (returnMsg != 0)
		{
			return returnMsg;
		}
	
		switchBackGroundSprite(args[0], args[1], gameWorld);

		return 0;
	}

	if (strcmp(buffer, "SET-BG-TRIGGER") == 0)
	{
		int args[5] = {0};

		int returnMsg = readMultipleIntArgs(fPtr, args, 4, 6);

		if (returnMsg != 0)
		{
			return returnMsg;
		}
	
		AddObject(gameWorld, LEVEL_FLAG_OBJ, args[0], args[1], SET_BACKGROUND_TRIGGER, args[2], args[3], args[4], 0);

		return 0;
	}

	if (strcmp(buffer, "SET-CAMERA-EDGES") == 0)
	{
		int args[4] = {0};

		int returnMsg = readMultipleIntArgs(fPtr, args, 4, 4);

		if (returnMsg != 0)
		{
			return returnMsg;
		}
	
		if (args[0] >= 0)
		{
			gameWorld->minCameraX = args[0];
		}

		if (args[1] >= 0)
		{
			gameWorld->maxCameraX = args[1];
		}

		if (args[2] >= 0)
		{
			gameWorld->minCameraY = args[2];
		}

		if (args[3] >= 0)
		{
			gameWorld->maxCameraY = args[3];
		}
		
		return 0;
	}



	return -1;
}


int loadRepeatingObject(World *gameWorld, FILE *fPtr)
{
	int args[5] = {0};

	int returnMsg = readMultipleIntArgs(fPtr, args, 4, 6);

	if (returnMsg != 0)
	{
		return returnMsg;
	}


	//fflush(fPtr);
	unsigned long objectPosition = ftell(fPtr);
			
	for (int yIter = 0; yIter < args[1]; yIter++)
	{
		for (int xIter = 0; xIter < args[0]; xIter++)
		{		
			fseek(fPtr, objectPosition, SEEK_SET);

			loadObjectOffset(gameWorld, fPtr, args[2] * xIter, args[3] * yIter);
		}
	}

	return 0;
}


int loadObject(World *gameWorld, FILE *fPtr)
{
	// ID and X/Y pos
	int coreArgs[5] = {0};
	
	int returnMsg = readMultipleIntArgs(fPtr, coreArgs, 3, 6);

	if (returnMsg != 0)
	{
		return returnMsg;
	}


	// args
	int args[5] = {0};

	if (readMultipleIntArgs(fPtr, args, 5, 5) == -1)
	{
		return -1;
	}


	AddObject(gameWorld, coreArgs[0], coreArgs[1], coreArgs[2], args[0], args[1], args[2], args[3], args[4]);	

	return 0;
}


int loadObjectOffset(World *gameWorld, FILE *fPtr, int xOffset, int yOffset)
{
	// ID and X/Y pos
	int coreArgs[5] = {0};

	if (readMultipleIntArgs(fPtr, coreArgs, 3, 6) != 0)
	{
		return -1;
	}

	// args
	int args[5] = {0};

	if (readMultipleIntArgs(fPtr, args, 5, 5) == -1)
	{
		return -1;
	}

	AddObject(gameWorld, coreArgs[0], coreArgs[1] + xOffset, coreArgs[2] + yOffset, args[0], args[1], args[2], args[3], args[4]);	

	return 0;
}


int readMultipleIntArgs(FILE *fPtr, int argsBuffer[], int number, int argMaxLength)
{
	unsigned long objectPosition;

	char inputBuffer[8] = {0};

	for (int i = 0; i < number; i++)
	{
		objectPosition = ftell(fPtr);

		int returnMsg = getNextArg(fPtr, inputBuffer, argMaxLength);

		if (returnMsg == -1)
		{
			return -1;
		}

		if (strcmp(inputBuffer, "////") == 0)
		{
			fseek(fPtr, objectPosition, SEEK_SET);
			return 0;
		}
		else
		{
			argsBuffer[i] = convertStrToInt(inputBuffer, argMaxLength);
		}
	
	}

	return 0;
}


int getNextArg(FILE *fPtr, char buffer[], int max)
{
	int i = 0;

	do
	{
		fread(buffer, sizeof(char), 1, fPtr);
	
		if (buffer[0] > 32)
		{
			i++;
		}
	} while ((buffer[0] < 33 || buffer[0] == '_') && i < max);


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

		buffer[i + 1] = 0;

		if (strcmp(buffer, "////") == 0)
		{
			return 0;
		}
	}

	fread(buffer + i, sizeof(char), 1, fPtr);

	if (buffer[i] != '_')
	{
		printf("Object data badly formatted!\n");
		return -1;
	}

	buffer[i] = 0;

	return 0;
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