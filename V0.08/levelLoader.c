#include "levelLoader.h"


int loadLevel(World *GameWorld, int level)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}


	// load file
	char fileName[20] = {0};
	snprintf(fileName, 20, "Level%d", level);

	FILE *fPtr = openFile(fileName, LEVELDATA_ROOT, "--LEVEL_DATA--");

	if (fPtr == NULL)
	{
		return INVALID_DATA;
	}

	// Erase existing data
	GameWorld->GameState = LOADING;
	clearLevelData(GameWorld);
	SetDebugSettingsToDefault();

	// load data
	if (loadLevelData(GameWorld, fPtr) != LEMON_SUCCESS)
	{
		putConsoleStrInt("\nError: Failed to load level ", level);
		return LEMON_ERROR;
	}

	GameWorld->GameState = GAMEPLAY;
	GameWorld->level = level;


	SpawnHUD(GameWorld);


	return LEMON_SUCCESS;
}


int loadScene(World *GameWorld, int sceneID)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}


	// load file
	char fileName[MAX_LEN] = {0};
	snprintf(fileName, MAX_LEN, "Level%d_Part%d", GameWorld->level, sceneID);

	FILE *fPtr = openFile(fileName, LEVELDATA_ROOT, "--PARTITION_DATA--");

	if (fPtr == NULL)
	{
		return INVALID_DATA;
	}

	// load data
	if (loadLevelData(GameWorld, fPtr) != LEMON_SUCCESS)
	{
		return LEMON_ERROR;
	}

	return LEMON_SUCCESS;
}


int loadTestScene(World *GameWorld)
{
	GameWorld->GameState = GAMEPLAY;

	switchBackGroundSprite(2, 1, &GameWorld->WorldBackground);

	AddObject(GameWorld, PLAYER_OBJECT, 3200, 70, 0, 0, 26, 0, 0, 0, 0);

	AddObject(GameWorld, SOLID_BLOCK, 0, 64, 512, 1, 1, 0, 0, 0, 0);

	AddObject(GameWorld, SOLID_BLOCK, 2900, 128, 3, 1, 1, 0, 0, 0, 0);

	AddObject(GameWorld, COIN, 200, 100, 0, 0, 0, 0, 0, 0, 0);
	AddObject(GameWorld, COIN, 300, 130, 0, 0, 0, 0, 0, 0, 0);
	AddObject(GameWorld, COIN, 400, 160, 0, 0, 0, 0, 0, 0, 0);
	AddObject(GameWorld, COIN, 450, 200, 0, 0, 0, 0, 0, 0, 0);

	AddObject(GameWorld, MOVING_PLATFORM_HOR, 1800, 200, 0, 0, 1800, 2800, 6, 90, 0);
	AddObject(GameWorld, MOVING_PLATFORM_HOR, 2600, 350, 0, 0, 2600, 3600, 6, 60, 0);
	AddObject(GameWorld, MOVING_PLATFORM_VER, 8000, 128, 0, 0, 176, 960, 6, 180, 0);
	AddObject(GameWorld, MOVING_PLATFORM_VER, 5700, 128, 0, 0, 176, 960, 6, 180, 0);
	AddObject(GameWorld, MOVING_PLATFORM_HOR, 3200, 100, 0, 0, 3300, 4200, 6, 90, 0);

	AddObject(GameWorld, SPRING, 3200, 96, 0, 0, 26, 0, 0, 0, 0);

	return LEMON_SUCCESS;
}


int ConvertEntryToObjectID(char entry[MAX_LEN])
{
	entry[MAX_LEN - 1] = 0;

	if (entry[0] > 47 && entry[0] < 58)
	{
		return convertStrToInt(entry, 6);
	}

	stringToLower(entry);

	if (strcmp(entry, "solidblock") == 0)
	{
		return SOLID_BLOCK;
	}
	else if (strcmp(entry, "flatslopefloor") == 0)
	{
		return FLAT_SLOPE_FLOOR;
	}
	else if (strcmp(entry, "jumpthrublock") == 0)
	{
		return JUMP_THRU_BLOCK;
	}
	else if (strcmp(entry, "coin") == 0)
	{
		return COIN;
	}
	else if (strcmp(entry, "playerobject") == 0)
	{
		return PLAYER_OBJECT;
	}
	else if (strcmp(entry, "uielement") == 0)
	{
		return UI_ELEMENT;
	}
	else if (strcmp(entry, "uitext") == 0)
	{
		return UI_TEXT;
	}
	else if (strcmp(entry, "particle") == 0)
	{
		return PARTICLE;
	}
	else if (strcmp(entry, "movingplatformhor") == 0)
	{
		return MOVING_PLATFORM_HOR;
	}
	else if (strcmp(entry, "movingplatformver") == 0)
	{
		return MOVING_PLATFORM_VER;
	}
	else if (strcmp(entry, "spring") == 0)
	{
		return SPRING;
	}
	else if (strcmp(entry, "gateswitch") == 0)
	{
		return GATE_SWITCH;
	}
	else if (strcmp(entry, "gateswitchtimed") == 0)
	{
		return GATE_SWITCH_TIMED;
	}
	else if (strcmp(entry, "verticalgate") == 0)
	{
		return VERTICAL_GATE;
	}
	else if (strcmp(entry, "horizontalgate") == 0)
	{
		return HORIZONTAL_GATE;
	}
	else if (strcmp(entry, "door") == 0)
	{
		return DOOR;
	}
	else if (strcmp(entry, "leveldoor") == 0)
	{
		return LEVEL_DOOR;
	}
	else if (strcmp(entry, "pushablebox") == 0)
	{
		return PUSHABLE_BOX;
	}
	else if (strcmp(entry, "basicenemy") == 0)
	{
		return BASIC_ENEMY;
	}	


	return -1;
}


int logLevel(World *GameWorld)
{
	// BROKEN
	FILE *fPtr;

	char path[strlen(LEVELDATA_ROOT) + 18];
	strcpy(path, LEVELDATA_ROOT);
	strcat(path, "Level0LOG.txt");
	path[strlen(LEVELDATA_ROOT) + 5] = GameWorld->level + 48;

	fPtr = fopen(path, "wb");

	if (fPtr == NULL)
	{
		putConsoleStrInt("\nCould not save level ", GameWorld->level);
		return LEMON_ERROR;
	}

	char Header[25] = LEMON_VERSION;
	strcat(Header, "--LEVEL_DATA--");

	fwrite(Header, sizeof(char), 20, fPtr);

	fwrite("\n", sizeof(char), 2, fPtr);


	Object *currentObject;
	currentObject = GameWorld->ObjectList->firstObject;

	char buffer[INT_MAX_LEN] = {0};

	while (currentObject != NULL)
	{
		switch(currentObject->ObjectID)
		{
			case LEVEL_FLAG_OBJ:
				fwrite("LVFLAG: ", sizeof(char), 8, fPtr);
				break;

			default:
				fwrite("OBJECT: ", sizeof(char), 8, fPtr);
				break;
		}


		int size = convertIntToStr(buffer, currentObject->ObjectID);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite(", ", sizeof(char), 2, fPtr);

		
		size = convertIntToStr(buffer, currentObject->ObjectBox->xPos);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite(", ", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentObject->ObjectBox->yPos);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite(", ", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentObject->ObjectBox->xSize);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite(", ", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentObject->ObjectBox->ySize);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite(", ", sizeof(char), 2, fPtr);


		size = convertIntToStr(buffer, currentObject->arg1);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite(", ", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentObject->arg2);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite(", ", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentObject->arg4);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite(", ", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentObject->arg4);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite(", ", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentObject->arg5);

		fwrite(buffer, sizeof(char), size, fPtr);


		fwrite("\n", sizeof(char), 5, fPtr);

		currentObject = currentObject->nextObject;
	}

	fwrite("ENDFILE", sizeof(char), 8, fPtr);

	fclose(fPtr);

	return LEMON_SUCCESS;
}


int lemon_Modulo(int x, int N)
{
	if (N < 1)
	{
		return 0;
	}

	return ((x % N) + N) % N;
}


/*
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
		putConsoleStrStr("\nCouldn't open file ", newFileName);
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
*/

int checkFileHeader(FILE *fPtr, const char FileType[])
{
	if (fPtr == NULL)
	{
		return MISSING_DATA;
	}

	size_t readData = 0;
	char charBuffer[20] = {0};

	// Read version number
	readData = fread(charBuffer, sizeof(char), 5, fPtr);
	if (readData < 5)
	{
		return MISSING_DATA;
	}

	charBuffer[5] = 0;

	if (strcmp(charBuffer, LEMON_VERSION) != 0)
	{
		putConsoleStrStr("\nFile load failed: Incompatible version number! Got: ", charBuffer);
		return INVALID_DATA;
	}


	// Read data type
	getNextArg(fPtr, charBuffer, 20);

	if (strcmp(charBuffer, FileType) != 0)
	{
		putConsoleString("\nFile load failed: This file does not contain expected data type!");
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}


int getCurrentLineNumber(FILE *fPtr)
{
	long filePosition = ftell(fPtr);

	fseek(fPtr, 0, SEEK_SET);

	int lineCount = 1;
	char fileCharacter = fgetc(fPtr);

	while (ftell(fPtr) != filePosition && fileCharacter != EOF)
	{
		fileCharacter = fgetc(fPtr);

		if (fileCharacter == '\n')
		{
			lineCount++;
		}
	}

	return lineCount;
}


int skipCommentInFile(FILE *fPtr, int maxLength)
{
	if (fPtr == NULL)
	{
		return MISSING_DATA;
	}

	char buffer[2] = {0};
	int commentLength = 0;
	size_t readData = 0;

	long objectPosition = ftell(fPtr);

	while (commentLength < maxLength && buffer[0] != '/' && buffer[0] != '\n')
	{
		objectPosition = ftell(fPtr);
		readData = fread(buffer, sizeof(char), 1, fPtr);
		commentLength++;

		if (readData != 1)
		{
			return END_OF_FILE;
		}
	}

	fseek(fPtr, objectPosition, SEEK_SET);

	return LEMON_SUCCESS;
}


FILE* openFile(const char fileName[], const char rootPath[], const char header[])
{
	if (rootPath == NULL || fileName == NULL || header == NULL)
	{
		return NULL;
	}

	int rootPathLength = strlen(rootPath);
	int fileNameLength = strlen(fileName);

	if (fileNameLength >= MAX_LEN || rootPathLength >= MAX_LEN)
	{
		return NULL;
	}

	char path[MAX_LEN + MAX_LEN + 5] = {0};
	strcpy(path, rootPath);
	strcat(path, fileName);
	strcat(path, ".lem");

	FILE *fPtr = fopen(path, "rb");

	if (fPtr == NULL)
	{
		path[fileNameLength + rootPathLength + 1] = 't';
		path[fileNameLength + rootPathLength + 2] = 'x';
		path[fileNameLength + rootPathLength + 3] = 't';
		
		fPtr = fopen(path, "rb");

		if (fPtr == NULL)
		{
			putConsoleStrStr("\nCould not find file '", fileName);
			putConsoleStrStr("' from root path: '", rootPath);
			putConsoleString("'");
			
			return NULL;
		}
	}
	else
	{
		//fPtr = convertLemToTxt(charBuffer, fPtr);

		if (fPtr == NULL)
		{
			return NULL;
		}
	}

	if (checkFileHeader(fPtr, header) != LEMON_SUCCESS)
	{
		fclose(fPtr);
		return NULL;
	}

	return fPtr;
}


int loadLevelData(World *GameWorld, FILE *fPtr)
{
	loadLevelDataChunk(GameWorld, fPtr, 10000);

	fclose(fPtr);


	return LEMON_SUCCESS;
}


int loadLevelDataChunk(World *GameWorld, FILE *fPtr, int lineLimit)
{
	char charBuffer[MAX_LEN] = {0};
	int i = 0;


	while (i < lineLimit)
	{
		if (feof(fPtr))
		{
			return END_OF_FILE;
		}

		getNextArg(fPtr, charBuffer, MAX_LEN);

		if (strcmp(charBuffer, "ENDFILE") == 0)
		{
			return END_OF_FILE;
		}
		else if (strcmp(charBuffer, "OBJECT:") == 0)
		{
			loadObject(GameWorld, fPtr, 0, 0);
		}	
		else if (strcmp(charBuffer, "OBJREP:") == 0)
		{
			loadRepeatingObject(GameWorld, fPtr, &i);
		}	
		else if (strcmp(charBuffer, "LVFLAG:") == 0)
		{
			loadLevelFlag(GameWorld, fPtr);
		}
		else if (charBuffer[0] == '>')
		{
			skipCommentInFile(fPtr, 200);
		}
		else
		{
			int lineCount = getCurrentLineNumber(fPtr);
			putConsoleStrInt("\nLevelData load failed. Unrecognised data found at Line: ", lineCount);
			return INVALID_DATA;
		}

		// Display level loading progress
		printConsoleData();
		
		i++;

		if (feof(fPtr))
		{
			return END_OF_FILE;
		}
	}


	return LEMON_SUCCESS;
}


int clearLevelData(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	GameWorld->GameState = LOADING;

	clearTextQueue(GameWorld);
	deleteAllSceneActions(GameWorld);

	deleteAllObjects(GameWorld->ObjectList);

	deleteAllFrameUpdateFunctions(GameWorld->ObjectList);

	deleteExcessSpriteSets(GameWorld->ObjectList, EngineSettings.PreservedSpriteSets);

	ResetCamera(&GameWorld->MainCamera);

	GameWorld->GameState = EMPTY_GAME;

	return LEMON_SUCCESS;
}


int loadLevelFlag(World *GameWorld, FILE *fPtr)
{
	char buffer[20] = {0};

	getNextArg(fPtr, buffer, 20);

	// Flag Decoded
	if (strcmp(buffer, "SET_BG") == 0)
	{
		int args[3] = {0};

		int returnMsg = readIntArgs(fPtr, args, 2);

		if (returnMsg != 0)
		{
			return returnMsg;
		}
	
		switchBackGroundSprite(args[0], args[1], &GameWorld->WorldBackground);
	}
	else if (strcmp(buffer, "SET_BG_TRIGGER") == 0)
	{
		int args[8] = {0};

		int returnMsg = readIntArgs(fPtr, args, 8);

		if (returnMsg != 0)
		{
			return returnMsg;
		}
	
		AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], args[2], args[3], SET_BACKGROUND_TRIGGER, args[4], args[5], args[6], args[7]);
	}
	else if (strcmp(buffer, "CUTSCENE_TRIGGER") == 0)
	{
		int args[5] = {0};

		int returnMsg = readIntArgs(fPtr, args, 5);

		if (returnMsg != 0)
		{
			return returnMsg;
		}
	
		AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], args[2], args[3], CUTSCENE_TRIGGER, args[4], 0, 0, 0);
	}
	else if (strcmp(buffer, "SET_CAMBOX") == 0)
	{
		int args[4] = {0};

		int returnMsg = readIntArgs(fPtr, args, 4);

		if (returnMsg != 0)
		{
			return returnMsg;
		}

		GameWorld->MainCamera.minCameraX = args[0];
		GameWorld->MainCamera.maxCameraX = args[1];
		GameWorld->MainCamera.minCameraY = args[2];
		GameWorld->MainCamera.maxCameraY = args[3];
	}
	else if (strcmp(buffer, "SET_CAMBOX_TRIGGER") == 0)
	{
		int args[8] = {0};

		int returnMsg = readIntArgs(fPtr, args, 8);

		if (returnMsg != 0)
		{
			return returnMsg;
		}
	
		AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], args[2], args[3], SET_CAMBOX_TRIGGER, args[4], args[5], args[6], args[7]);
	}
	else if (strcmp(buffer, "START_CAMPOS") == 0)
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
	else if (strcmp(buffer, "START_CAMMODE") == 0)
	{
		int args[1] = {0};

		int returnMsg = readIntArgs(fPtr, args, 1);

		if (returnMsg != 0)
		{
			return returnMsg;
		}

		GameWorld->MainCamera.CameraMode = args[0];
	}
	else if (strcmp(buffer, "START_PLAYERPOS") == 0)
	{
		int args[2] = {0};

		int returnMsg = readIntArgs(fPtr, args, 2);

		if (returnMsg != 0)
		{
			return returnMsg;
		}

		if (GameWorld->Player.PlayerBox == NULL)
		{
			return MISSING_DATA;
		}

		GameWorld->Player.PlayerBox->xPos = (double)args[0];
		GameWorld->Player.PlayerBox->yPos = (double)args[1];
	}
	else if (strcmp(buffer, "START_MUSIC") == 0)
	{
		char nameBuffer[80] = {0};

		getNextArg(fPtr, nameBuffer, MAX_LEN);

		char volumeBuffer[5] = {0};

		getNextArg(fPtr, volumeBuffer, 5);

		float volume = (float)convertStrToInt(volumeBuffer, 4) / 100.0;

		if (getSoundInstance(nameBuffer, LOOP_CHANNEL) == NULL)
		{
			Lemon_PlaySound(nameBuffer, "Music", LOOP_CHANNEL, volume);
		}
	}
	else if (strcmp(buffer, "CACHE_TRIGGER") == 0)
	{
		int args[8] = {0};

		int returnMsg = readIntArgs(fPtr, args, 8);

		if (returnMsg != 0)
		{
			return returnMsg;
		}

		AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], args[2], args[3], CACHE_TRIGGER, args[4], args[5], args[6], args[7]);
	}
	else if (strcmp(buffer, "STREAM_PARTITION_TRIGGER") == 0)
	{
		int args[5] = {0};

		int returnMsg = readIntArgs(fPtr, args, 5);

		if (returnMsg != 0)
		{
			return returnMsg;
		}

		AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], args[2], args[3], STREAM_PARTITION_TRIGGER, args[4], 0, 0, 0);
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}


int loadRepeatingObject(World *GameWorld, FILE *fPtr, int *objectsLoaded)
{
	int args[5] = {0};

	int returnMsg = readIntArgs(fPtr, args, 4);

	if (returnMsg != LEMON_SUCCESS)
	{
		return returnMsg;
	}

	args[0] = clamp(args[0], 1, 128);
	args[1] = clamp(args[1], 1, 128);


	unsigned long objectPosition = ftell(fPtr);
			
	for (int yIter = 0; yIter < args[1]; yIter++)
	{
		for (int xIter = 0; xIter < args[0]; xIter++)
		{		
			fseek(fPtr, objectPosition, SEEK_SET);

			loadObject(GameWorld, fPtr, args[2] * xIter, args[3] * yIter);
		}
	}

	if (objectsLoaded != NULL)
	{
		objectsLoaded += args[0] * args[1];
		objectsLoaded--;
	}

	return LEMON_SUCCESS;
}


int ApplyObjectLoadCommands(Object *inputObject, char command[MAX_LEN])
{
	if (inputObject == NULL)
	{
		return MISSING_DATA;
	}

	command[MAX_LEN - 1] = 0;

	if (strcmp(command, "UNSOLID") == 0)
	{
		inputObject->ObjectBox->solid = UNSOLID;
	}
	if (strcmp(command, "SOLID") == 0)
	{
		inputObject->ObjectBox->solid = SOLID;
	}
	else if (strcmp(command, "TOBACKGROUND") == 0)
	{
		inputObject->layer = BACKGROUND;
	}
	else if (strcmp(command, "TOMIDDLEGROUND") == 0)
	{
		inputObject->layer = MIDDLEGROUND;
	}
	else if (strcmp(command, "TOFOREGROUND") == 0)
	{
		inputObject->layer = FOREGROUND;
	}
	else if (strcmp(command, "COLLIDETOFOREGROUND") == 0)
	{
		inputObject->ObjectBox->collideLayer = FOREGROUND;
	}
	else if (strcmp(command, "COLLIDETOMIDDLEGROUND") == 0)
	{
		inputObject->ObjectBox->collideLayer = MIDDLEGROUND;
	}
	else if (strcmp(command, "COLLIDETOBACKGROUND") == 0)
	{
		inputObject->ObjectBox->collideLayer = BACKGROUND;
	}
	else if (strcmp(command, "ALLTOBACKGROUND") == 0)
	{
		inputObject->ObjectBox->collideLayer = BACKGROUND;
		inputObject->layer = BACKGROUND;
	}
	else if (strcmp(command, "ALLTOMIDDLEGROUND") == 0)
	{
		inputObject->ObjectBox->collideLayer = MIDDLEGROUND;
		inputObject->layer = MIDDLEGROUND;
	}
	else if (strcmp(command, "ALLTOFOREGROUND") == 0)
	{
		inputObject->ObjectBox->collideLayer = FOREGROUND;
		inputObject->layer = FOREGROUND;
	}


	return LEMON_SUCCESS;
}


int loadObject(World *GameWorld, FILE *fPtr, int xOffset, int yOffset)
{
	char readArgs[MAX_LEN] = {0};
	int convertedArgs[9] = {0};

	// ID
	getNextArg(fPtr, readArgs, MAX_LEN);

	int readID = ConvertEntryToObjectID(readArgs);

	// X/Y pos & args
	readIntArgs(fPtr, convertedArgs, 9);

	Object *addedObject = AddObject(GameWorld, readID, convertedArgs[0] + xOffset, convertedArgs[1] + yOffset, convertedArgs[2], convertedArgs[3], convertedArgs[4], convertedArgs[5], convertedArgs[6], convertedArgs[7], convertedArgs[8]);	

	// Read extra commands
	if (atEndOfLine(fPtr) == 1)
	{
		return LEMON_SUCCESS;
	}

	char buffer[MAX_LEN] = {0};

	while (atEndOfLine(fPtr) == 0)
	{
		getNextArg(fPtr, buffer, MAX_LEN);

		ApplyObjectLoadCommands(addedObject, buffer);
	}

	return LEMON_SUCCESS;
}


int atEndOfLine(FILE *fPtr)
{
	if (fPtr == NULL)
	{
		return -1;
	}

	unsigned long filePosition = ftell(fPtr);

	char buffer[2] = {0};
	size_t readData = 1;

	while (buffer[0] != '\n' && buffer[0] < 33 && feof(fPtr) == 0 && readData > 0)
	{
		readData = fread(buffer, sizeof(char), 1, fPtr);
	}

	fseek(fPtr, filePosition, SEEK_SET);

	if (buffer[0] == '\n' || buffer[0] == '/')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


int readIntArgs(FILE *fPtr, int argsDest[], int number)
{
	if (fPtr == NULL)
	{
		return MISSING_DATA;
	}

	unsigned long objectPosition;

	char inputBuffer[12] = {0};

	for (int i = 0; i < number; i++)
	{
		objectPosition = ftell(fPtr);

		int returnMsg = getNextArg(fPtr, inputBuffer, 12);

		if (returnMsg != LEMON_SUCCESS)
		{
			fseek(fPtr, objectPosition, SEEK_SET);
			return LEMON_ERROR;
		}

		if (inputBuffer[0] == '-' || inRange(inputBuffer[0], '0', '9') )
		{
			argsDest[i] = convertStrToInt(inputBuffer, 12);			
		}
		else
		{
			fseek(fPtr, objectPosition, SEEK_SET);
			return LEMON_SUCCESS;
		}
	}

	return LEMON_SUCCESS;
}


int getNextArg(FILE *fPtr, char buffer[], int capacity)
{
	if (fPtr == NULL || feof(fPtr) != 0 || capacity < 2)
	{
		return MISSING_DATA;
	}

	memset(buffer, 0, capacity);

	while (buffer[0] < 33 || buffer[0] == '/')
	{
		fread(buffer, sizeof(char), 1, fPtr);

		if (feof(fPtr))
		{
			return LEMON_SUCCESS;
		}
	}

	int i = 1;
	
	while (i < capacity - 1)
	{
		fread(buffer + i, sizeof(char), 1, fPtr);

		if (buffer[i] < 33)
		{
			//fseek(fPtr, (int)-sizeof(char), SEEK_CUR);
			buffer[i] = 0;
			return LEMON_SUCCESS;
		}

		if (buffer[i] == ',') 
		{
			buffer[i] = 0;
			return LEMON_SUCCESS;
		}

		if (feof(fPtr))
		{
			buffer[i] = 0;
			return MISSING_DATA;
		}

		i++;
	}

	buffer[capacity - 1] = 0;

	return LEMON_SUCCESS;
}

int getNextArgInt(FILE *fPtr)
{
	if (fPtr == NULL)
	{
		return 0;
	}

	long filePos = ftell(fPtr);

	char buffer[41] = {0};
	getNextArg(fPtr, buffer, 40);
	buffer[40] = 0;

	if (inRange(buffer[0], '0', '9') || buffer[0] == '-')
	{
		return convertStrToInt(buffer, 41);
	}

	fseek(fPtr, filePos, SEEK_SET);
	return 0;
}

float getNextArgFloat(FILE *fPtr)
{
	if (fPtr == NULL)
	{
		return 0.0;
	}

	long filePos = ftell(fPtr);

	char buffer[41] = {0};
	getNextArg(fPtr, buffer, 40);
	buffer[40] = 0;

	if (inRange(buffer[0], '0', '9') || buffer[0] == '-')
	{
		return ((float)atof(buffer));
	}

	fseek(fPtr, filePos, SEEK_SET);
	return 0.0;
}


int convertStrToInt(char str[], int size)
{
	int input = 0;
	int polarity = 1;
	
	int i = 0;
	

	while (i < size)
	{
		if (inRange(str[i], 48, 58))
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
	if (input == 0)
	{
		str[0] = 48;
		str[1] = 0;
		return 1;
	}

	int i = 0;

	if (input < 0)
	{
		i = 1;
		str[0] = '-';
		input = abs(input);
	}


	int j = floor( log10(input) );

	while (j > 0)
	{
		if (i >= INT_MAX_LEN)
		{
			str[INT_MAX_LEN - 1] = 0;
			return i;
		}

		int power = pow(10, j);
		str[i] = ((input / power) % 10) + 48;
		j--;
		i++;
	}

	str[i] = (input % 10) + 48;
	i++;
	str[i] = 0;
	
	return i;
}


bool inRange(int input, int low, int high)
{
	if (input < low || input > high)
	{
		return false;
	}
	else 
	{
		return true;
	}
	
}