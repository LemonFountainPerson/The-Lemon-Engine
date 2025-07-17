#include "levelLoader.h"


int loadLevel(World *GameWorld, int level)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameWorld->drawPlayer = 1;
	GameWorld->drawBackGround = 1;
	GameWorld->drawObjects = 1;
	GameWorld->drawParticles = 1;
	GameWorld->drawUI = 1;
	GameWorld->drawHitboxes = 0;
	GameWorld->drawSprites = 1;


	if (loadLevelData(GameWorld, level) != LEMON_SUCCESS)
	{
		if (GameWorld->ObjectList != NULL && GameWorld->ObjectList->firstObject != NULL)
		{
			GameWorld->GameState = GAMEPLAY;
		}

		return INVALID_DATA;
	}

	GameWorld->GameState = GAMEPLAY;
	GameWorld->level = level;

	// Create a default particle and UIElement object to initilise their sprite sets to avoid lag during gameplay 
	AddObject(GameWorld, PARTICLE, 0, 0, 0, 0, EMPTY_PARTICLE, 0, 0, 0, 0);
	AddObject(GameWorld, UI_ELEMENT, 0, 0, UNDEFINED_UI_ELEMENT, 0, 0, 0, 0, 0, 0);
	AddObject(GameWorld, UI_TEXT, 0, 0, UNDEFINED_UI_ELEMENT, 0, 0, 0, 0, 0, 0);

	SpawnHUD(GameWorld);

	return 0;
}


int loadDefault(World *GameWorld)
{
	AddObject(GameWorld, SOLID_BLOCK, 0, 64, 512, 1, 0, 0, 0, 0, 0);
	AddObject(GameWorld, SOLID_BLOCK, 2900, 110, 1, 1, 0, 0, 0, 0, 0);
	AddObject(GameWorld, SOLID_BLOCK, 2940, 110, 1, 1, 0, 0, 0, 0, 0);
	AddObject(GameWorld, SOLID_BLOCK, 2970, 110, 1, 1, 0, 0, 0, 0, 0);

	AddObject(GameWorld, COIN, 200, 100, 1, 1, 0, 0, 0, 0, 0);
	AddObject(GameWorld, COIN, 300, 130, 1, 1, 0, 0, 0, 0, 0);
	AddObject(GameWorld, COIN, 400, 160, 1, 1, 0, 0, 0, 0, 0);
	AddObject(GameWorld, COIN, 450, 200, 1, 1, 0, 0, 0, 0, 0);

	AddObject(GameWorld, MOVING_PLATFORM_HOR, 1800, 200, 0, 0, 1800, 2800, 6, 90, 0);
	AddObject(GameWorld, MOVING_PLATFORM_HOR, 2600, 350, 0, 0, 2600, 3600, 6, 60, 0);
	AddObject(GameWorld, MOVING_PLATFORM_VER, 8000, 128, 0, 0, 176, 960, 6, 180, 0);
	AddObject(GameWorld, MOVING_PLATFORM_VER, 5700, 128, 0, 0, 176, 960, 6, 180, 0);
	AddObject(GameWorld, MOVING_PLATFORM_HOR, 3200, 100, 0, 0, 3300, 4200, 6, 90, 0);

	AddObject(GameWorld, SPRING, 3200, 70, 0, 0, 26, 0, 0, 0, 0);

	switchBackGroundSprite(0, 0, GameWorld);


	return 0;
}


int saveLevel(World *GameWorld)
{
	// BROKEN
	FILE *fPtr;

	char path[96] = "LemonData/LevelData/Level0Data.txt";
	path[25] = GameWorld->level + 48;

	fPtr = fopen(path, "wb");

	if (fPtr == NULL)
	{
		printf("Could not save level %d\n", GameWorld->level);
		return -1;
	}


	fwrite("V1____", sizeof(char), 6, fPtr);

	Object *currentObject;
	currentObject = GameWorld->ObjectList->firstObject;

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
		fread(currentBlock, sizeof(char), 1, txtPtr);

		if (feof(txtPtr))
		{
			i = 9999;
			continue;
		}


		int offset = 1;

		currentBlock[0] = currentBlock[0] + offset;	// 100, 4
		//currentBlock[1] = lemon_Modulo(currentBlock[1] - offset, 128);

		printf("Decoded: %d %d %d %c %c \n\n", i, currentBlock[0], currentBlock[1], currentBlock[0], currentBlock[1]);

		fwrite(currentBlock, sizeof(char), 1, newLemFile);

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
		fread(currentBlock, sizeof(char), 1, lemPtr);

		if (feof(lemPtr))
		{
			i = 9999;
			continue;
		}

		int offset = 1;

		currentBlock[0] = lemon_Modulo(currentBlock[0] - offset, 128);   
		//currentBlock[1] = lemon_Modulo(currentBlock[1] + offset, 128);

		//printf("0 and 1: %d %d %c %c \n\n", currentBlock[0], currentBlock[1], currentBlock[0], currentBlock[1]);

		fwrite(currentBlock, sizeof(char), 1, newTextFile);

		i++;
	}

	fclose(lemPtr);

	return newTextFile;
}


int checkFileHeader(FILE *fPtr, const char FileType[])
{
	if (fPtr == NULL)
	{
		return MISSING_DATA;
	}

	char charBuffer[11];

	// Read version number
	fread(charBuffer, sizeof(char), 5, fPtr);
	charBuffer[5] = 0;

	if (strcmp(charBuffer, LEMON_VERSION) != 0)
	{
		printf("File load failed: Incompatible version number! %s\n", charBuffer);
		return INVALID_DATA;
	}


	// Read data type
	fread(charBuffer, sizeof(char), 10, fPtr);
	charBuffer[10] = 0;

	if (strcmp(charBuffer, FileType) != 0)
	{
		printf("File load failed: This file does not contain expected data type!\n");
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}


int loadLevelData(World *GameWorld, int level)
{
	GameWorld->GameState = LOADING;

	char charBuffer[MAX_LEN + 4] = "Level0Data.lem";
	charBuffer[5] = level + 48;

	char path[96] = "LemonData/LevelData/";
	strcat(path, charBuffer);

	FILE *fPtr = fopen(path, "rb");

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
			return MISSING_DATA;
		}
	}
	else
	{
		fPtr = convertLemToTxt(charBuffer, fPtr);

		if (fPtr == NULL)
		{
			return ERROR;
		}
	}


	int result = checkFileHeader(fPtr, "-LEVELDATA");

	if (result != LEMON_SUCCESS)
	{
		fclose(fPtr);
		return INVALID_DATA;
	}


	// Erase existing data
	deleteAllObjects(GameWorld->ObjectList);

	deleteExcessSpriteSets(GameWorld->ObjectList, PRESERVED_SPRITESETS);

	GameWorld->MainCamera.CameraMode = FOLLOW_PLAYER;


	int endOfFile = 0;
	int i = 0;


	while (endOfFile == 0 && feof(fPtr) == 0)
	{
		getNextArg(fPtr, charBuffer, 4);
		charBuffer[4] = 0;

		if (strcmp(charBuffer, "////") != 0)
		{
			printf("Level %d load failed: Data formatted incorrectly! %c %c %c %c\n", level, charBuffer[0], charBuffer[1], charBuffer[2], charBuffer[3]);
			clearCurrentlyLoadedLevelData(GameWorld);
			GameWorld->GameState = EMPTY_GAME;
			fclose(fPtr);
			return INVALID_DATA;
		}

		fread(charBuffer, sizeof(char), 7, fPtr);
		charBuffer[7] = 0;

		if (strcmp(charBuffer, "ENDFILE") == 0)
		{
			endOfFile = 1;
			continue;
		}
		else if (strcmp(charBuffer, "OBJECT-") == 0)
		{
			loadObject(GameWorld, fPtr, 0, 0);
		}	
		else if (strcmp(charBuffer, "OBJREP-") == 0)
		{
			loadRepeatingObject(GameWorld, fPtr);
		}	
		else if (strcmp(charBuffer, "LVFLAG-") == 0)
		{
			loadLevelFlag(GameWorld, fPtr);
		}
		else if (charBuffer[0] == '>')
		{
			int commentLength = 0;

			unsigned long objectPosition = ftell(fPtr);

			while (commentLength < 200 && charBuffer[0] != '/')
			{
				objectPosition = ftell(fPtr);
				fread(charBuffer, sizeof(char), 1, fPtr);
				commentLength++;
			}

			fseek(fPtr, objectPosition, SEEK_SET);
		}
		else
		{
			printf("Unrecognised data!\n");
			clearCurrentlyLoadedLevelData(GameWorld);
			GameWorld->GameState = EMPTY_GAME;
			fclose(fPtr);
			return INVALID_DATA;
		}
		
		// This is incredibly hacky but basically I do this in case only one entry is malformed, stops at '////'
		int temp[16];

		readIntArgs(fPtr, temp, 16);

		i++;

		if (i > 10000)
		{
			printf("\nFile limit reached! (%s)!\n", path);
			endOfFile = 1;
		}
	}


	//strcpy(charBuffer, "Level0Data");
	//charBuffer[5] = level + 48;
	//charBuffer[10] = 0;
	//convertTxtToLem(charBuffer, fPtr);

	printf("\nSuccessfully read file (%s)!\n\n", path);
	fclose(fPtr);


	return LEMON_SUCCESS;
}


int clearCurrentlyLoadedLevelData(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	GameWorld->GameState = LOADING;

	deleteAllObjects(GameWorld->ObjectList);

	GameWorld->level = 0;

	GameWorld->GameState = EMPTY_GAME;

	return 0;
}


int loadLevelFlag(World *GameWorld, FILE *fPtr)
{
	char buffer[20] = {0};

	getNextArg(fPtr, buffer, 19);

	// Flag Decoded
	if (strcmp(buffer, "SET-BG") == 0)
	{
		int args[3] = {0};

		int returnMsg = readIntArgs(fPtr, args, 2);

		if (returnMsg != 0)
		{
			return returnMsg;
		}
	
		switchBackGroundSprite(args[0], args[1], GameWorld);

		return 0;
	}
	else if (strcmp(buffer, "SET-BG-TRIGGER") == 0)
	{
		int args[8] = {0};

		int returnMsg = readIntArgs(fPtr, args, 8);

		if (returnMsg != 0)
		{
			return returnMsg;
		}
	
		AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], args[2], args[3], SET_BACKGROUND_TRIGGER, args[4], args[5], args[6], args[7]);

		return 0;
	}
	else if (strcmp(buffer, "SET-CAMBOX") == 0)
	{
		int args[4] = {0};

		int returnMsg = readIntArgs(fPtr, args, 4);

		if (returnMsg != 0)
		{
			return returnMsg;
		}
	
		if (args[0] >= 0)
		{
			GameWorld->MainCamera.minCameraX = args[0];
		}

		if (args[1] >= 0)
		{
			GameWorld->MainCamera.maxCameraX = args[1];
		}

		if (args[2] >= 0)
		{
			GameWorld->MainCamera.minCameraY = args[2];
		}

		if (args[3] >= 0)
		{
			GameWorld->MainCamera.maxCameraY = args[3];
		}
		
		return 0;
	}
	else if (strcmp(buffer, "SET-CAMBOX-TRIGGER") == 0)
	{
		int args[8] = {0};

		int returnMsg = readIntArgs(fPtr, args, 8);

		if (returnMsg != 0)
		{
			return returnMsg;
		}
	
		AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], args[2], args[3], SET_CAMBOX_TRIGGER, args[4], args[5], args[6], args[7]);

		return 0;
	}
	else if (strcmp(buffer, "START-CAMPOS") == 0)
	{
		int args[2] = {0};

		int returnMsg = readIntArgs(fPtr, args, 2);

		if (returnMsg != 0)
		{
			return returnMsg;
		}

		GameWorld->MainCamera.CameraX = args[0];
		GameWorld->MainCamera.CameraY = args[1];
	}
	else if (strcmp(buffer, "START-CAMMODE") == 0)
	{
		int args[1] = {0};

		int returnMsg = readIntArgs(fPtr, args, 1);

		if (returnMsg != 0)
		{
			return returnMsg;
		}

		GameWorld->MainCamera.CameraMode = args[0];
	}
	else if (strcmp(buffer, "START-PLAYERPOS") == 0)
	{
		int args[2] = {0};

		int returnMsg = readIntArgs(fPtr, args, 2);

		if (returnMsg != 0)
		{
			return returnMsg;
		}

		if (GameWorld->Player == NULL || GameWorld->Player->PlayerBox == NULL)
		{
			return MISSING_DATA;
		}

		GameWorld->Player->PlayerBox->xPos = (double)args[0];
		GameWorld->Player->PlayerBox->yPos = (double)args[1];

		return 0;
	}
	else if (strcmp(buffer, "START-MUSIC") == 0)
	{
		char nameBuffer[80] = {0};

		getNextArg(fPtr, nameBuffer, 80);

		nameBuffer[79] = 0;

		char volumeBuffer[5] = {0};

		getNextArg(fPtr, volumeBuffer, 4);

		double volume = (float)convertStrToInt(volumeBuffer, 4) / 100.0;

		LemonPlaySound(nameBuffer, "Music", LOOP_CHANNEL, volume);

		return 0;
	}
	else if (strcmp(buffer, "CACHE-TRIGGER") == 0)
	{
		int args[8] = {0};

		int returnMsg = readIntArgs(fPtr, args, 8);

		if (returnMsg != 0)
		{
			return returnMsg;
		}

		AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], args[2], args[3], CACHE_TRIGGER, args[4], args[5], args[6], args[7]);

		return 0;
	}

	return INVALID_DATA;
}


int loadRepeatingObject(World *GameWorld, FILE *fPtr)
{
	int args[5] = {0};

	int returnMsg = readIntArgs(fPtr, args, 4);

	if (returnMsg != 0)
	{
		return returnMsg;
	}


	unsigned long objectPosition = ftell(fPtr);
			
	for (int yIter = 0; yIter < args[1]; yIter++)
	{
		for (int xIter = 0; xIter < args[0]; xIter++)
		{		
			fseek(fPtr, objectPosition, SEEK_SET);

			loadObject(GameWorld, fPtr, args[2] * xIter, args[3] * yIter);
		}
	}

	return 0;
}


int ConvertEntryToObjectID(char entry[60])
{
	entry[59] = 0;

	if (entry[0] > 47 && entry[0] < 58 || entry[0] == 45)
	{
		return convertStrToInt(entry, 6);
	}




	return -1;
}


int ApplyObjectLoadCommands(Object *inputObject, char commands[32])
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	commands[31] = 0;

	if (strcmp(commands, "UNSOLID") == 0)
	{
		inputObject->ObjectBox->solid = UNSOLID;
	}

	if (strcmp(commands, "UNSOLID-TOBACKGROUND") == 0)
	{
		inputObject->ObjectBox->solid = UNSOLID;
		inputObject->layer = BACKGROUND;
	}

	if (strcmp(commands, "TOBACKGROUND") == 0)
	{
		inputObject->layer = BACKGROUND;
	}

	if (strcmp(commands, "TOFOREGROUND") == 0)
	{
		inputObject->layer = FOREGROUND;
	}

	if (strcmp(commands, "UNSOLID-TOFOREGROUND") == 0)
	{
		inputObject->ObjectBox->solid = UNSOLID;
		inputObject->layer = FOREGROUND;
	}


	return 0;
}


int loadObject(World *GameWorld, FILE *fPtr, int xOffset, int yOffset)
{
	char readArgs[60] = {0};
	int convertedArgs[9] = {0};

	// ID
	getNextArg(fPtr, readArgs, 60);

	int readID = ConvertEntryToObjectID(readArgs);

	// X/Y pos & args
	if (readIntArgs(fPtr, convertedArgs, 9) != 0)
	{
		return INVALID_DATA;
	}

	Object *addedObject;

	addedObject = AddObject(GameWorld, readID, convertedArgs[0] + xOffset, convertedArgs[1] + yOffset, convertedArgs[2], convertedArgs[3], convertedArgs[4], convertedArgs[5], convertedArgs[6], convertedArgs[7], convertedArgs[8]);	

	// Read extra commands
	char buffer[32] = {0};

	unsigned long objectPosition = ftell(fPtr);

	getNextArg(fPtr, buffer, 32);

	if (strcmp(buffer, "////") == 0)
	{
		fseek(fPtr, objectPosition, SEEK_SET);

		return 0;
	}
	
	ApplyObjectLoadCommands(addedObject, buffer);

	return 0;
}


int readIntArgs(FILE *fPtr, int argsDest[], int number)
{
	if (fPtr == NULL)
	{
		return MISSING_DATA;
	}


	unsigned long objectPosition;

	char inputBuffer[8] = {0};

	for (int i = 0; i < number; i++)
	{
		objectPosition = ftell(fPtr);

		int returnMsg = getNextArg(fPtr, inputBuffer, 8);

		if (returnMsg != LEMON_SUCCESS)
		{
			return LEMON_ERROR;
		}

		if (strcmp(inputBuffer, "////") == 0 || inputBuffer[0] > 64)
		{
			fseek(fPtr, objectPosition, SEEK_SET);
			return 0;
		}
		else
		{
			argsDest[i] = convertStrToInt(inputBuffer, 8);
		}
	
	}

	return 0;
}


int getNextArg(FILE *fPtr, char buffer[], int max)
{
	int i = 0;

	buffer[0] = '_';

	while ((buffer[0] < 33 || buffer[0] == '_') && i < max)
	{
		if (feof(fPtr) != 0)
		{
			return MISSING_DATA;
		}

		fread(buffer, sizeof(char), 1, fPtr);
	
		if (buffer[0] > 32)
		{
			i++;
		}
	}


	if (i > 3)
	{
		printf("Data badly formatted!\n");
		return INVALID_DATA;
	}

	i = 0;
	
	while (buffer[i] != '_' && buffer[i] > 32 && i < max)
	{
		if (feof(fPtr) != 0)
		{
			return MISSING_DATA;
		}

		i++;
		fread(buffer + i, sizeof(char), 1, fPtr);

		buffer[i + 1] = 0;

		if (strcmp(buffer, "////") == 0)
		{
			return LEMON_SUCCESS;
		}
	}

	buffer[i] = 0;

	return LEMON_SUCCESS;
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