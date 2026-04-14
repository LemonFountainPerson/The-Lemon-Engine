#include "LemonEngine.h"


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
    GameWorld->level = level;

    // Debug
	if (DebugSettings.ConsoleTextEnabled == CONSOLE_ALL_EVENTS)
	{
		putConsoleString("\nLoading into level %d...", level);
	}

	// load data
	if (loadLevelData(GameWorld, fPtr) == INVALID_DATA)
	{
		putConsoleString("\nError: Failed to load level %d", level);
        GameWorld->GameState = ENCOUNTERED_FATAL_ERROR;
		return LEMON_ERROR;
	}

	restoreAllCachedObjects(GameWorld->ObjectList);

	GameWorld->GameState = GAMEPLAY;
	GameWorld->GamePaused = 0;


	return LEMON_SUCCESS;
}


int loadPartition(World *GameWorld, int partID)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (partID < 0)
	{
		return INVALID_DATA;
	}


	// load file
	char fileName[MAX_LEN] = {0};
	snprintf(fileName, MAX_LEN, "Level%d_Part%d", GameWorld->level, partID);

	FILE *fPtr = openFile(fileName, LEVELDATA_ROOT, "--PARTITION_DATA--");

	if (fPtr == NULL)
	{
		return INVALID_DATA;
	}

	// load data
	if (loadLevelData(GameWorld, fPtr) == INVALID_DATA)
	{
		return LEMON_ERROR;
	}

	return LEMON_SUCCESS;
}


int loadSave(int saveFile, int flags[GAME_FLAG_COUNT], World *GameWorld)
{
	if (flags == NULL || GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	char fileName[MAX_LEN] = {0};
	snprintf(fileName, MAX_LEN, "SaveFile%d", saveFile);

	return loadSaveData(fileName, flags, GameWorld);
}

int loadSettings(int settingsFile, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	char fileName[MAX_LEN] = {0};
	snprintf(fileName, MAX_LEN, "SettingsFile%d", settingsFile);

	return loadSaveData(fileName, NULL, GameWorld);
}


int loadSaveData(const char *fileName, int flags[GAME_FLAG_COUNT], World *GameWorld)
{
	FILE *fPtr = openFile(fileName, SAVEDATA_ROOT, "--SAVE_DATA--");

	if (fPtr == NULL)
	{
		return INVALID_DATA;
	}

	char readPhrase[MAX_LEN] = {0};
	while (!endOfFile(fPtr))
	{
		Next_Save_Instruction:

		getNextArg(fPtr, readPhrase, MAX_LEN);
		stringToUpper(readPhrase);

		if (!strcmp(readPhrase, "FULLSCREEN:"))
		{
			getNextArg(fPtr, readPhrase, MAX_LEN);
			stringToUpper(readPhrase);

			if (!strcmp(readPhrase, "TRUE") || !strcmp(readPhrase, "YES"))
			{
				enableFullscreenScaled(GameWorld);
			}
			else if (!strcmp(readPhrase, "EXPAND") || !strcmp(readPhrase, "TRUE/EXPAND"))
			{
				enableFullscreen(GameWorld);
			}
			else
			{
				disableFullscreen(GameWorld);
			}
		}
		if (!strcmp(readPhrase, "VSYNC:"))
		{
			getNextArg(fPtr, readPhrase, MAX_LEN);
			stringToUpper(readPhrase);

			if (!strcmp(readPhrase, "TRUE") || !strcmp(readPhrase, "YES") || !strcmp(readPhrase, "ON"))
			{
				setVsync(true);
			}
			else
			{
				setVsync(false);
			}
		}
		else if (!strcmp(readPhrase, "WINDOWRESOLUTION:"))
		{
			int width = getNextArgInt(fPtr);
			int height = getNextArgInt(fPtr);

			changeScreenSize(width, height, GameWorld);
		}
		else if (!strcmp(readPhrase, "SCREENRESOLUTION:"))
		{
			int width = getNextArgInt(fPtr);
			int height = getNextArgInt(fPtr);

			if (width >= MINIMUM_SCREEN_WIDTH)
			{
				screenWidth = width;
			}

			if (height >= MINIMUM_SCREEN_HEIGHT)
			{
				screenHeight = height;
			}
		}
		else if (!strcmp(readPhrase, "RESOLUTION:"))
		{
			int width = getNextArgInt(fPtr);
			int height = getNextArgInt(fPtr);

			setScreenAndRendererSize(width, height, GameWorld);
		}
		else if (!strcmp(readPhrase, "GAMEFLAGS:"))
		{
			if (!bracketedStatementPresent(fPtr, NULL))
			{
				goto Next_Save_Instruction;
			}

			consumeStatement(fPtr, '{');

			if (flags == NULL)
			{
				consumeStatement(fPtr, '}');
				goto Next_Save_Instruction;
			}

			int i = 0;
			while (hasNextArgInt(fPtr) && i < GAME_FLAG_COUNT)
			{
				flags[i] = getNextArgInt(fPtr);
				i++;
			}

			consumeStatement(fPtr, '}');
		}
		else if (!strcmp(readPhrase, "LEVEL:"))
		{
			switchLevel(getNextArgInt(fPtr), GameWorld);
		}	
	}

	closeFile(fPtr);

	return LEMON_SUCCESS;
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
		putConsoleString("\nCould not save level %d", GameWorld->level);
		return LEMON_ERROR;
	}

	char Header[25] = LEMON_VERSION;
	strcat(Header, "--LOG_DATA--");

	fwrite(Header, sizeof(char), 20, fPtr);

	fwrite("\n", sizeof(char), 2, fPtr);


	Object *currentObject = GameWorld->ObjectList->firstObject;
	PhysicsBox *currentBox;

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

		currentBox = currentObject->ObjectBox;

		size = convertIntToStr(buffer, currentBox->xPos);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite(", ", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentBox->yPos);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite(", ", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentBox->xSize);

		fwrite(buffer, sizeof(char), size, fPtr);

		fwrite(", ", sizeof(char), 2, fPtr);

		size = convertIntToStr(buffer, currentBox->ySize);

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


		fwrite("\n", sizeof(char), 5, fPtr);

		currentObject = currentObject->nextObject;
	}

	fwrite("ENDFILE", sizeof(char), 8, fPtr);

	closeFile(fPtr);

	return LEMON_SUCCESS;
}


int saveGameState(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	char title[MAX_LEN];
	snprintf(title, MAX_LEN, "SaveState%d", GameWorld->level);
	FILE *file = fopen(title, "wb");

	//write file version
	fwrite(LEMON_VERSION, sizeof(char), strlen(LEMON_VERSION), file);

	//write Gameworld data
	int emptyValue = -1;
	fwrite(GameWorld, sizeof(World), 1, file);

	BackgroundData *gameBG = &GameWorld->WorldBackground;
	if (gameBG->BackgroundSpriteBuffer != NULL)
	{
		int bgIndex = getSpriteIndexSpriteSet(gameBG->BackgroundSpriteBuffer->name, gameBG->bgSpriteSets.start);
		fwrite(&bgIndex, 4, 1, file);
	}
	else
	{
		fwrite(&emptyValue, 4, 1, file);
	}

	if (gameBG->bgSpriteSets.start != NULL)
	{
		int bgSetIndex = gameBG->bgSpriteSets.start->setID;
		fwrite(&bgSetIndex, 4, 1, file);
	}
	else
	{
		fwrite(&emptyValue, 4, 1, file);
	}
	

	//write indexes for player pointer
	if (GameWorld->Player.PlayerPtr == NULL)
	{
		fwrite(&emptyValue, 4, 1, file);
	}
	else
	{
		fwrite(&GameWorld->Player.PlayerPtr->index, 4, 1, file);
	}

	// write object controller data
	fwrite(GameWorld->ObjectList, sizeof(ObjectController), 1, file);

	// write object controller indices to replace pointers
	Object *list = GameWorld->ObjectList->objectComponents.Objects;
	int i = 0;
	while(i < EngineSettings.MaxObjects)
	{
		writeObjectIndices(&list[i], file);

		i++;
	}

	Object *current = GameWorld->ObjectList->firstObject;
	if (current != NULL)
	{
		fwrite(&current->index, 4, 1, file);
	}
	else
	{
		fwrite(&emptyValue, 4, 1, file);
	}

	current = GameWorld->ObjectList->lastObject;
	if (current != NULL)
	{
		fwrite(&current->index, 4, 1, file);
	}
	else
	{
		fwrite(&emptyValue, 4, 1, file);
	}

	current = GameWorld->ObjectList->availableSlots;
	if (current != NULL)
	{
		fwrite(&current->index, 4, 1, file);
	}
	else
	{
		fwrite(&emptyValue, 4, 1, file);
	}

	current = GameWorld->ObjectList->cachedFirstObject;
	if (current != NULL)
	{
		fwrite(&current->index, 4, 1, file);
	}
	else
	{
		fwrite(&emptyValue, 4, 1, file);
	}

	current = GameWorld->ObjectList->cachedLastObject;
	if (current != NULL)
	{
		fwrite(&current->index, 4, 1, file);
	}
	else
	{
		fwrite(&emptyValue, 4, 1, file);
	}


	int var = GameWorld->SceneActionCount;
	fwrite(&var, 4, 1, file);

	SceneAction *action = GameWorld->SceneActionQueue;
	while (var > 0 && action != NULL)
	{
		fwrite(action, sizeof(SceneAction), 1, file);

		int index = -1;
		if (action->ActorObject != NULL)
		{
			index = action->ActorObject->index;
		}

		fwrite(&index, 4, 1, file);
		
		var--;
		action = action->nextSceneAction;
	}


	fwrite("ENDFILE", 8, 1, file);

	closeFile(file);

	putConsoleString("\nGame State Saved!\n");


	return LEMON_SUCCESS;
}


int writeObjectIndices(Object *input, FILE *file)
{
	if (file == NULL || input == NULL)
	{
		return MISSING_DATA;
	}

	int negative = -1;

	if (input->prevObject != NULL)
	{
		fwrite(&input->prevObject->index, 4, 1, file);
	}
	else
	{
		fwrite(&negative, 4, 1, file);
	}

	if (input->nextObject != NULL)
	{
		fwrite(&input->nextObject->index, 4, 1, file);
	}
	else
	{
		fwrite(&negative, 4, 1, file);
	}

	if (input->Parent != NULL)
	{
		fwrite(&input->Parent->index, 4, 1, file);
	}
	else
	{
		fwrite(&negative, 4, 1, file);
	}

	return LEMON_SUCCESS;
}

int loadObjectIndices(Object *input, ObjectController *ObjectList, FILE *file)
{
	if (file == NULL || input == NULL)
	{
		return MISSING_DATA;
	}

	input->prevObject = NULL;
	input->nextObject = NULL;
	input->Parent = NULL;

	Object *objects = ObjectList->objectComponents.Objects;

    size_t bytesRead = 0;
	int readVal = 0;
	bytesRead = fread(&readVal, 4, 1, file);

	if (readVal > -1 && readVal < EngineSettings.MaxObjects && bytesRead > 0)
	{
		input->prevObject = &objects[readVal];
	}

	bytesRead = fread(&readVal, 4, 1, file);

	if (readVal > -1 && readVal < EngineSettings.MaxObjects && bytesRead > 0)
	{
		input->nextObject = &objects[readVal];
	}

	bytesRead = fread(&readVal, 4, 1, file);

	if (readVal > -1 && readVal < EngineSettings.MaxObjects && bytesRead > 0)
	{
		input->Parent = &objects[readVal];
	}

	input->ObjectBox->GroundBox = NULL;
	
	DisplayData *display = getDisplay(input);

	if (display->currentSprite < 0)
	{
		display->spriteBuffer = EngineSettings.DefaultTexture;
	}
	else
	{
		display->spriteBuffer = NULL;
	}


	display->spriteSetSource = loadSpriteSet(ObjectList, input->ObjectID);	

	// TO DO: restore animation/frame here
	display->frameBuffer = NULL;
	display->animationBuffer = NULL;

	return LEMON_SUCCESS;
}


int loadGameState(World *GameWorld)
{
	// This function is not complete, and is not memory safe. Use at your own risk!
    size_t readData = 0;

	char title[MAX_LEN] = {0};
	snprintf(title, MAX_LEN, "SaveState%d", GameWorld->level);
	FILE *file = fopen(title, "rb");

	if (!file)
	{
		return MISSING_DATA;
	}

	EngineSettings.PreservedSpriteSets = 0;
	clearLevelData(GameWorld);

	char buffer[MAX_LEN] = {0};
	readData = fread(buffer, sizeof(char), strlen(LEMON_VERSION), file);

	if (strcmp(buffer, LEMON_VERSION) || readData != strlen(LEMON_VERSION))
	{
		closeFile(file);
		return INVALID_DATA;
	}

	//write Gameworld data
	ObjectController *list = GameWorld->ObjectList;
	SpriteSetList *bgs = &GameWorld->WorldBackground.bgSpriteSets;
	deleteAllSpriteSets(bgs);

	memset(GameWorld, 0, sizeof(World));
	memset(list, 0, sizeof(ObjectController));

	readData = fread(GameWorld, sizeof(World), 1, file);
	GameWorld->ObjectList = list;

	GameWorld->TextQueue = NULL;
	GameWorld->SceneActionQueue = NULL;
	GameWorld->WorldBackground.BackgroundSpriteBuffer = NULL;

	// restore backgrounds
	initialiseBackGround(&GameWorld->WorldBackground);

	int bgIndex;
	readData = fread(&bgIndex, 4, 1, file);

	int bgSetIndex;
	readData = fread(&bgSetIndex, 4, 1, file);

	switchBackGroundSprite(bgIndex, bgSetIndex, &GameWorld->WorldBackground);


	int index = -1;
	readData = fread(&index, 4, 1, file);
	
	if (index > -1)
	{
		GameWorld->Player.PlayerPtr = &GameWorld->ObjectList->objectComponents.Objects[index];
		GameWorld->Player.PlayerBox = &GameWorld->ObjectList->objectComponents.PhysicsBoxes[index];
		GameWorld->Player.PlayerDisplay = getDisplay(GameWorld->Player.PlayerPtr);
	}
	else
	{
		GameWorld->Player.PlayerPtr = NULL;
		GameWorld->Player.PlayerBox = NULL;
		GameWorld->Player.PlayerDisplay = NULL;
	}


	// read objectlist
	readData = fread(GameWorld->ObjectList, sizeof(ObjectController), 1, file);



	list->FrameUpdates = NULL;
	initialiseSpriteSetList(&list->spriteSets);

	Object *objects = list->objectComponents.Objects;
	int i = 0;
	while(i < EngineSettings.MaxObjects)
	{
		loadObjectIndices(&objects[i], list, file);

		i++;
	}

	readData = fread(&i, 4, 1, file);
	if (i == -1)
	{
		list->firstObject = NULL;
	}
	else
	{
		list->firstObject = &objects[i];
	}

	readData = fread(&i, 4, 1, file);
	if (i == -1)
	{
		list->lastObject = NULL;
	}
	else
	{
		list->lastObject = &objects[i];
	}

	readData = fread(&i, 4, 1, file);
	if (i == -1)
	{
		list->availableSlots = NULL;
	}
	else
	{
		list->availableSlots = &objects[i];
	}

	readData = fread(&i, 4, 1, file);
	if (i == -1)
	{
		list->cachedFirstObject = NULL;
	}
	else
	{
		list->cachedFirstObject = &objects[i];
	}

	readData = fread(&i, 4, 1, file);
	if (i == -1)
	{
		list->cachedLastObject = NULL;
	}
	else
	{
		list->cachedLastObject = &objects[i];
	}


	// # of scene actions
	readData = fread(&i, 4, 1, file);

	while (i > 0)
	{
		i--;

		SceneAction *action = createSceneAction(SCENE_END, GameWorld);
		if (action == NULL)
		{
			i = 0;
			continue;
		}

		SceneAction *prev = action->prevSceneAction;
		readData = fread(action, sizeof(SceneAction), 1, file);
		action->prevSceneAction = prev;
		action->nextSceneAction = NULL;

		int index = -1;
		readData = fread(&index, 4, 1, file);

		if (index == -1)
		{
			action->ActorObject = NULL;
		}
		else
		{
			action->ActorObject = &objects[index];
		}

		if (action->ActionID == SCENE_SAY_TEXT)
		{
			deleteSceneAction(action, GameWorld);
		}
	}


	closeFile(file);


	putConsoleString("\nGame State Loaded!\n");

	return LEMON_SUCCESS;
}


int checkFileHeader(FILE *fPtr, const char FileType[])
{
	if (fPtr == NULL)
	{
		return MISSING_DATA;
	}

	size_t readData = 0;
	char charBuffer[32] = {0};

	if (DEBUG_MODE)
	{
		readData = getNextArg(fPtr, charBuffer, 32);
		return LEMON_SUCCESS;
	}

	// Read version number
	int LemVerLength = strlen(LEMON_VERSION);
	readData = fread(charBuffer, sizeof(char), LemVerLength, fPtr);
	if (readData < LemVerLength)
	{
		return MISSING_DATA;
	}

	charBuffer[LemVerLength] = 0;

	if (charBuffer[0] == 'V')
	{
		if (strcmp(charBuffer, LEMON_VERSION) != 0)
		{
			putConsoleString("\nFile load failed: Incompatible version number! Got: %s", charBuffer);
			return INVALID_DATA;
		}
	}
	else
	{
		fseek(fPtr, 0, SEEK_SET);

		int FileVerLength = strlen(FILE_READER_VERSION);
		readData = fread(charBuffer, sizeof(char), FileVerLength, fPtr);
		if (readData < FileVerLength)
		{
			return MISSING_DATA;
		}

		if (strcmp(charBuffer, FILE_READER_VERSION) != 0)
		{
			putConsoleString("\nFile load failed: Incompatible version number! Got: %s", charBuffer);
			return INVALID_DATA;
		}
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


FILE* encodeLEMFile(FILE *file)
{



	return file;
}


FILE* decodeLEMFile(FILE *file)
{

	

	return file;
}


void closeFile(FILE *file)
{
    if (file)
    {
        fclose(file);
    }
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


	char path[MAX_LEN + MAX_LEN + 10] = {0};
	int pathLength = rootPathLength + fileNameLength;
	strcpy(path, rootPath);
	strcat(path, fileName);

	FILE *fPtr;
	char extensions[][10] = {"", ".lem", ".txt"};		// changing the order of this list modifies its priority; eg first it checks without ext, then with .lem, etc.

	for (int attempt = 0; attempt < 3; attempt++)
	{
		strcpy(path + pathLength, extensions[attempt]);
	 	fPtr = fopen(path, "rb");

	 	if (fPtr != NULL)
	 	{
	 		goto File_Loaded;
	 	}
	}

	putConsoleString("Could not find file '%s' from path: '%s'", fileName, rootPath);
	return NULL;


	File_Loaded:

	if (checkFileHeader(fPtr, header) != LEMON_SUCCESS)
	{
		closeFile(fPtr);
		return NULL;
	}

	return fPtr;
}


int loadLevelData(World *GameWorld, FILE *fPtr)
{
	FuncResult result = loadLevelDataChunk(GameWorld, fPtr, 10000);

	closeFile(fPtr);
	
	return result;
}


int loadLevelDataChunk(World *GameWorld, FILE *fPtr, int lineLimit)
{
	char buffer[MAX_LEN] = {0};
	int i = 0;
	int returnMsg = LEMON_SUCCESS;

	while (i < lineLimit || lineLimit < 1)
	{
		if (endOfFile(fPtr))
		{
			return END_OF_FILE;
		}

		if (returnMsg != LEMON_SUCCESS)
		{
			return returnMsg;
		}

		returnMsg = getNextArg(fPtr, buffer, MAX_LEN);
		stringToUpper(buffer);

		if (strcmp(buffer, "{") == 0)
		{
			i--;			// i is decremented as this is not an instruction, only a start of another block, so 
		}
		else if (strcmp(buffer, "}") == 0)
		{
			return LEMON_SUCCESS;
		}
		else if (strcmp(buffer, "OBJREP:") == 0)
		{
			loadRepeatingObject(GameWorld, fPtr, &i);
		}
		else if (strcmp(buffer, "REPEAT:") == 0)
		{
			loadObjectRepeated(GameWorld, fPtr);
		}
		else if (strcmp(buffer, "OBJECT:") == 0)
		{
			loadObject(GameWorld, fPtr, 0, 0);
		}	
		else if (strcmp(buffer, "LVFLAG:") == 0)
		{
			loadLevelFlag(GameWorld, fPtr);
		}
		else if (strcmp(buffer, "IFVARIABLE:") == 0)
		{
			loadConditionalStatement(GameWorld, fPtr);
		}
		else if (!strcmp(buffer, "PRESET:"))
		{
			getNextArg(fPtr, buffer, MAX_LEN);
			// check if 'level' is contained at position 'buffer'; i.e: check if buffer is in the form 'Level...'
			if (strstr(buffer, "Level") != buffer)
			{
				FILE *preset = openFile(buffer, LEVELDATA_ROOT, "--LEVEL_DATA--");
				loadLevelData(GameWorld, preset);
			}
			else
			{
				putConsoleString("Tried to load preset from file: %s", buffer);
			}
		}
		else
		{
			int lineCount = getCurrentLineNumber(fPtr);
			putConsoleString("\nLevelData load failed. Unrecognised data found at Line: %d", lineCount);
			if (DEBUG_MODE)
			{
				putConsoleString("\nRead: %s", buffer);
			}

			//return INVALID_DATA;
		}
		
		i++;
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


int skipCommentInFile(FILE *fPtr)
{
	if (fPtr == NULL)
	{
		return MISSING_DATA;
	}

	char buffer[2] = {0};
	size_t readData = 0;

	long objectPosition;

	do
	{
		objectPosition = ftell(fPtr);
		readData = fread(buffer, sizeof(char), 1, fPtr);

		if (readData != 1 || feof(fPtr))
		{
			return END_OF_FILE;
		}
	} while (buffer[0] != '/' && buffer[0] != '\n');

	fseek(fPtr, objectPosition, SEEK_SET);

	return LEMON_SUCCESS;
}

int loadConditionalStatement(World *GameWorld, FILE *fPtr)
{
	int flagIndex = getNextArgInt(fPtr);

	if (!inRange(flagIndex, 0, GAME_FLAG_COUNT - 1))
	{
		// this will result in the level load being aborted
		putConsoleString("\nInvalid game flag index! Got: %d \nGame flags can only go from 0 to %d", flagIndex, GAME_FLAG_COUNT - 1);
		return INVALID_DATA;
	}

	long filePosition = ftell(fPtr);

	char buffer[12] = {0};

	int returnMsg = getNextArg(fPtr, buffer, 12);
	if (returnMsg != LEMON_SUCCESS)
	{
		fseek(fPtr, filePosition, SEEK_SET);
		return returnMsg;
	}

	int compareValue = getNextArgInt(fPtr);
	bool conditionMet = false;

	if (strcmp("=", buffer) == 0)
	{	
		conditionMet = (GameFlags[flagIndex] == compareValue);
	}
	else if (strcmp("!=", buffer) == 0)
	{
		conditionMet = (GameFlags[flagIndex] != compareValue);
	}
	else if (strcmp(">", buffer) == 0)
	{
		conditionMet = (GameFlags[flagIndex] > compareValue);
	}
	else if (strcmp("<", buffer) == 0)
	{
		conditionMet = (GameFlags[flagIndex] < compareValue);
	}
	else if (strcmp(">=", buffer) == 0)
	{
		conditionMet = (GameFlags[flagIndex] >= compareValue);
	}
	else if (strcmp("<=", buffer) == 0)
	{
		conditionMet = (GameFlags[flagIndex] <= compareValue);
	}
	else
	{
		return INVALID_DATA;
	}

	readBranch(GameWorld, fPtr, conditionMet);

	

	return LEMON_SUCCESS;
}


int readBranch(World *GameWorld, FILE *fPtr, bool conditionMet)
{
	if (bracketedStatementPresent(fPtr, "THEN"))
	{
		consumeStatement(fPtr, '{');

		if (conditionMet)
		{
			// execute commands
			loadLevelDataChunk(GameWorld, fPtr, 0);
		}
		else
		{
			consumeStatement(fPtr, '}');
		}
	}
	else 
	{
		return INVALID_DATA;
	}


	if (!bracketedStatementPresent(fPtr, "ELSE"))
	{
		return LEMON_SUCCESS;
	}

	consumeStatement(fPtr, '{');

	if (!conditionMet)
	{
		loadLevelDataChunk(GameWorld, fPtr, 0);
	}
	else
	{
		consumeStatement(fPtr, '}');
	}
	

	return LEMON_SUCCESS;
}

bool bracketedStatementPresent(FILE *fPtr, const char expectedPhrase[])
{
	if (fPtr == NULL)
	{
		return false;
	}

	long filePosition = ftell(fPtr);

	char buffer[20] = {0};
	getNextArg(fPtr, buffer, 20);

	if (expectedPhrase == NULL || expectedPhrase[0] < 33)
	{
		fseek(fPtr, filePosition, SEEK_SET);
		return (buffer[0] == '{');
	}

	// check phrase before hand
	if (strcmp(expectedPhrase, buffer) != 0)
	{
		fseek(fPtr, filePosition, SEEK_SET);
		return false;
	}

	// check next thing
	filePosition = ftell(fPtr);

	getNextArg(fPtr, buffer, 20);

	fseek(fPtr, filePosition, SEEK_SET);

	if (buffer[0] != '{')
	{
		return false;
	}

	return true;
}

int consumeStatement(FILE *fPtr, char stopCharacter)
{
	char buffer[2] = {0};
	size_t readData = 0;

	while (!feof(fPtr) && buffer[0] != stopCharacter)
	{
		readData = fread(buffer, sizeof(char), 1, fPtr);
        
        if (readData < 1)
        {   
            return MISSING_DATA;
        }

		if ((buffer[0] & 0b10000000) > 0)
		{
			// unicode! skip until an ascii char is found
			buffer[0] = 0;
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

	deleteLevelObjects(GameWorld->ObjectList);

	deleteExcessSpriteSets(GameWorld->ObjectList, EngineSettings.PreservedSpriteSets);

	ResetCamera(&GameWorld->MainCamera);

	GameWorld->GameState = EMPTY_GAME;

	return LEMON_SUCCESS;
}


int loadLevelFlag(World *GameWorld, FILE *fPtr)
{
	char buffer[MAX_LEN] = {0};

	getNextArg(fPtr, buffer, MAX_LEN);
	stringToUpper(buffer);

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
		int args[6] = {0};

		int returnMsg = readIntArgs(fPtr, args, 6);

		if (returnMsg != 0)
		{
			return returnMsg;
		}
	
		setSize(AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], SET_BACKGROUND_TRIGGER, args[4], args[5], 0, 0), args[2], args[3]);
	}
	else if (strcmp(buffer, "START_CUTSCENE") == 0)		// START_LVL_WITH_CUTSCENE
	{
		int sceneID = getNextArgInt(fPtr);
		playCutscene(sceneID, GameWorld);
	}
	else if (strcmp(buffer, "CUTSCENE_TRIGGER") == 0)
	{
		int args[5] = {0};

		int returnMsg = readIntArgs(fPtr, args, 5);

		if (returnMsg != 0)
		{
			return returnMsg;
		}
	
		setSize(AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], CUTSCENE_TRIGGER, args[4], 0, 0, 0), args[2], args[3]);
	}
	else if (strcmp(buffer, "LEVEL_TRIGGER") == 0)
	{
		int args[5] = {0};

		readIntArgs(fPtr, args, 5);
	
		setSize(AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], LEVEL_TRIGGER, args[4], 0, 0, 0), args[2], args[3]);
	}
	else if (strcmp(buffer, "LEVEL_TRIGGER_SEAMLESS") == 0)
	{
		int args[5] = {0};

		readIntArgs(fPtr, args, 5);
	
		setSize(AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], LEVEL_TRIGGER_SEAMLESS, args[4], 0, 0, 0), args[2], args[3]);
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
	
		setSize(AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], SET_CAMBOX_TRIGGER, args[4], args[5], args[6], args[7]), args[2], args[3]);
	}
	else if (strcmp(buffer, "START_CAMPOS") == 0 || !strcmp(buffer, "CAMPOS"))
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
	else if (strcmp(buffer, "START_CAMMODE") == 0 || !strcmp(buffer, "CAMMODE"))
	{
		int args[1] = {0};

		int returnMsg = readIntArgs(fPtr, args, 1);

		if (returnMsg != 0)
		{
			return returnMsg;
		}

		GameWorld->MainCamera.CameraMode = args[0];
	}
	else if (strcmp(buffer, "PLAYER_POSITION") == 0)
	{
		if (GameWorld->Player.PlayerBox == NULL)
		{
			return MISSING_DATA;
		}

		GameWorld->Player.PlayerBox->xPos = getNextArgFloat(fPtr);
		GameWorld->Player.PlayerBox->yPos = getNextArgFloat(fPtr);
	}
	else if (strcmp(buffer, "CAMPOS_TO_PLAYER") == 0)
	{
		if (GameWorld->Player.PlayerBox == NULL)
		{
			return MISSING_DATA;
		}
		
		GameWorld->MainCamera.CameraX = GameWorld->Player.PlayerBox->xPos;
		GameWorld->MainCamera.CameraY = GameWorld->Player.PlayerBox->yPos;
	}
	else if (strcmp(buffer, "START_MUSIC") == 0)
	{
		char nameBuffer[MAX_LEN] = {0};

		getNextArg(fPtr, nameBuffer, MAX_LEN);

		float volume = getNextArgFloat(fPtr);

		if (getSoundInstance(nameBuffer, MUSIC_CHANNEL) == NULL)
		{
			PlaySoundRepeat(nameBuffer, "Music", MUSIC_CHANNEL, volume, -1);
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

		setSize(AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], CACHE_TRIGGER, args[4], args[5], args[6], args[7]), args[2], args[3]);
	}
	else if (strcmp(buffer, "LOAD_PART_TRIGGER") == 0)
	{
		int args[5] = {0};

		int returnMsg = readIntArgs(fPtr, args, 5);

		if (returnMsg != 0)
		{
			return returnMsg;
		}

		setSize(AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], LOAD_PART_TRIGGER, args[4], 0, 0, 0), args[2], args[3]);
	}
	else if (strcmp(buffer, "SWITCH_TO_NEW_PART_TRIGGER") == 0)
	{
		int args[5] = {0};

		int returnMsg = readIntArgs(fPtr, args, 5);

		if (returnMsg != 0)
		{
			return returnMsg;
		}

		setSize(AddObject(GameWorld, LEVEL_FLAG_OBJ, args[0], args[1], SWITCH_TO_NEW_PART_TRIGGER, args[4], 0, 0, 0), args[2], args[3]);
	}
	else if (strcmp(buffer, "LOAD_PART") == 0)
	{
		int partID = getNextArgInt(fPtr);

		loadPartition(GameWorld, partID);
	}
	else if (strcmp(buffer, "FALSE_CAM_BOUNDARY") == 0 || strcmp(buffer, "FALSE_CAMERA_BOUNDARY") == 0 || strcmp(buffer, "FALSE_CAM_BOUND") == 0 )
	{
		float xPos = getNextArgFloat(fPtr);
		int xSize = (int)(getNextArgFloat(fPtr) - xPos);

		float yPos = getNextArgFloat(fPtr);
		int ySize = (int)(getNextArgFloat(fPtr) - yPos);

		AddObject(GameWorld, LEVEL_FLAG_OBJ, xPos, yPos, FALSE_CAMERA_BOUNDARY, xSize, ySize, 0, 0);
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}


int loadRepeatingObject(World *GameWorld, FILE *fPtr, int *objectsLoaded)	// legacy method
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

int loadObjectRepeated(World *GameWorld, FILE *fPtr)
{
	int args[4] = {0};

	char string[16] = {0};

	for (int i = 0; i < 4; i++)
	{
		getNextArg(fPtr, string, 16);
		if (string[0] == '{')
		{
			goto Skip_Repeated_Args;
		}
		consumeStatement(fPtr, '=');
		args[i] = getNextArgInt(fPtr);
	}
	
	consumeStatement(fPtr, '{');

	Skip_Repeated_Args:

	long filePos = ftell(fPtr);
	
	int xVal = 0;
	int yVal = 0;
	int xTimes = 0;
	int xChange = args[1];
	int yTimes = args[2];
	int yChange = args[3];

	while (yTimes > 0)
	{
		yTimes--;
		xTimes = args[0];
		xVal = 0;

		while (xTimes > 0)
		{
			xTimes--;

			fseek(fPtr, filePos, SEEK_SET);

			if (!hasNextArgInt(fPtr))
			{
				getNextArg(fPtr, string, 16);
			}

			loadObject(GameWorld, fPtr, xVal, yVal);

			xVal += xChange;
		}

		yVal += yChange;
	}

	consumeStatement(fPtr, '}');

	return LEMON_SUCCESS;
}

int ApplyObjectLoadCommands(FILE *fPtr, Object *inputObject, char command[MAX_LEN], ObjectController *ObjectList)
{
	if (inputObject == NULL || fPtr == NULL)
	{
		return MISSING_DATA;
	}

	stringToUpper(command);
	command[MAX_LEN - 1] = 0;
	PhysicsBox *inputBox = inputObject->ObjectBox;

	if (strcmp(command, "UNSOLID") == 0)
	{
		inputBox->solid = UNSOLID;
	}
	if (strcmp(command, "SOLID") == 0)
	{
		inputBox->solid = SOLID;
	}
	else if (strcmp(command, "TOBACKGROUND") == 0)
	{
		setDisplayLayer(inputObject, BACKGROUND);
	}
	else if (strcmp(command, "TOMIDDLEGROUND") == 0)
	{
		setDisplayLayer(inputObject, MIDDLEGROUND);
	}
	else if (strcmp(command, "TOMIDDLEGROUND_2") == 0)
	{
		setDisplayLayer(inputObject, MIDDLEGROUND_2);
	}
	else if (strcmp(command, "TOFOREGROUND") == 0)
	{
		setDisplayLayer(inputObject, FOREGROUND);
	}
	else if (strcmp(command, "COLLIDETOFOREGROUND") == 0)
	{
		inputBox->collideLayer = FOREGROUND;
	}
	else if (strcmp(command, "COLLIDETOMIDDLEGROUND") == 0)
	{
		inputBox->collideLayer = MIDDLEGROUND;
	}
	else if (strcmp(command, "COLLIDETOBACKGROUND") == 0)
	{
		inputBox->collideLayer = BACKGROUND;
	}
	else if (strcmp(command, "ALLTOBACKGROUND") == 0)
	{
		inputBox->collideLayer = BACKGROUND;
		setDisplayLayer(inputObject, BACKGROUND);
	}
	else if (strcmp(command, "ALLTOMIDDLEGROUND") == 0)
	{
		inputBox->collideLayer = MIDDLEGROUND;
		setDisplayLayer(inputObject, MIDDLEGROUND);
	}
	else if (strcmp(command, "ALLTOMIDDLEGROUND2") == 0)
	{
		inputBox->collideLayer = MIDDLEGROUND_2;
		setDisplayLayer(inputObject, MIDDLEGROUND_2);
	}
	else if (strcmp(command, "ALLTOFOREGROUND") == 0)
	{
		inputBox->collideLayer = FOREGROUND;
		setDisplayLayer(inputObject, FOREGROUND);
	}
	else if (strcmp(command, "SETNAME") == 0)
	{
		long filePos = ftell(fPtr);
		getNextArg(fPtr, command, MAX_LEN);
		fseek(fPtr, filePos, SEEK_SET);

		if (command[0] != '{')
		{
			return INVALID_DATA;
		}

		consumeStatement(fPtr, '{');
		getNextArg(fPtr, command, MAX_LEN);
		if (command[0] != '}')
		{
			consumeStatement(fPtr, '}');
		}
	
		setObjectName(inputObject, command);
	}
	else if (!strcmp(command, "SETPARENT"))
	{
		long filePos = ftell(fPtr);
		getNextArg(fPtr, command, MAX_LEN);
		fseek(fPtr, filePos, SEEK_SET);

		if (command[0] != '{')
		{
			return INVALID_DATA;
		}

		consumeStatement(fPtr, '{');
		getNextArg(fPtr, command, MAX_LEN);
		if (command[0] != '}')
		{
			consumeStatement(fPtr, '}');
		}
		Object *parent = FindObject(command, ObjectList);

		if (parent != NULL)
		{
			inputObject->Parent = parent;
		}
	}
	else if (!strcmp(command, "HIDE"))
	{
		hideObject(inputObject);
	}
	else if (!strcmp(command, "ALIGNTOGRID"))
	{
		snapPositionToTileGrid(inputObject, inputObject->ObjectBox->xPos, inputObject->ObjectBox->yPos);
	}


	return LEMON_SUCCESS;
}


int loadObject(World *GameWorld, FILE *fPtr, int xOffset, int yOffset)
{
	char readArgs[MAX_LEN] = {0};
	int convertedArgs[7] = {0};

	// ID
	getNextArg(fPtr, readArgs, MAX_LEN);

	int readID = getObjectID(readArgs);

	// X/Y pos & args
	readIntArgs(fPtr, convertedArgs, 7);

	if (readID == UI_ELEMENT && atEndOfLine(fPtr) == 0)
	{
		getNextArg(fPtr, readArgs, MAX_LEN);
		convertedArgs[2] = convertEntryToUIType(readArgs);
	}

	if (GameWorld->ObjectList->objectCount >= EngineSettings.MaxObjects - EngineSettings.ReservedObjects)
	{
		consumeStatement(fPtr, '\n');
		return ACTION_DISABLED;
	}

	Object *addedObject = AddObject(GameWorld, readID, convertedArgs[0] + xOffset, convertedArgs[1] + yOffset, convertedArgs[2], convertedArgs[3], convertedArgs[4], convertedArgs[5], convertedArgs[6]);	

	// Read extra commands
	if (atEndOfLine(fPtr) == 1)
	{
		return LEMON_SUCCESS;
	}

	char buffer[MAX_LEN] = {0};

	while (atEndOfLine(fPtr) == 0)
	{
		getNextArg(fPtr, buffer, MAX_LEN);

		ApplyObjectLoadCommands(fPtr, addedObject, buffer, GameWorld->ObjectList);
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

	if (buffer[0] == '\n' || buffer[0] == '/' || buffer[0] == '}')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


bool endOfFile(FILE *fPtr)
{
	if (feof(fPtr))
	{
		return true;
	}

	char checkBuffer[MAX_LEN] = {0};
	long filePos = ftell(fPtr);
	getNextArg(fPtr, checkBuffer, MAX_LEN);
	fseek(fPtr, filePos, SEEK_SET);

	if (strcmp(checkBuffer, "ENDFILE") == 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}



typedef struct FilePositions
{
	long positions[FILE_POSITION_HISTORY_LENGTH];
	FILE *file;
	int head;
	int stored;
} FilePositions;

static FilePositions prevPositions = {0};
void moveFilePos(FILE *file, int offset)
{
	if (!file)
	{
		return;
	}

	long position = ftell(file);
	fseek(file, offset, SEEK_CUR);

	if (prevPositions.file != file)
	{
		prevPositions.file = file;
		prevPositions.head = 0;
		prevPositions.stored = 0;
	}

	prevPositions.positions[prevPositions.head] = position;
	prevPositions.head = (prevPositions.head + 1) % FILE_POSITION_HISTORY_LENGTH;
	if (prevPositions.stored < FILE_POSITION_HISTORY_LENGTH)
	{
		prevPositions.stored++;
	}
}

void setFilePos(FILE *file, int pos)
{
	if (!file)
	{
		return;
	}

	long position = ftell(file);
	fseek(file, pos, SEEK_SET);

	if (prevPositions.file != file)
	{
		prevPositions.file = file;
		prevPositions.head = 0;
		prevPositions.stored = 0;
	}

	prevPositions.positions[prevPositions.head] = position;
	prevPositions.head = (prevPositions.head + 1) % FILE_POSITION_HISTORY_LENGTH;
	if (prevPositions.stored < FILE_POSITION_HISTORY_LENGTH)
	{
		prevPositions.stored++;
	}
}

void returnFilePos(FILE *file)
{
	if (file != prevPositions.file || prevPositions.stored < 1)
	{
		return;
	}

	prevPositions.head--;
	if (prevPositions.head < 0)
	{
		prevPositions.head = FILE_POSITION_HISTORY_LENGTH - 1;
	}
	fseek(file, prevPositions.positions[prevPositions.head], SEEK_SET);
	prevPositions.stored--;
}


int readIntArgs(FILE *fPtr, int argsDest[], int number)
{
	if (fPtr == NULL)
	{
		return MISSING_DATA;
	}

	for (int i = 0; i < number; i++)
	{
		argsDest[i] = getNextArgInt(fPtr);
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
    size_t readData = 0;
    bool enclosedCommand = false;

	while (buffer[0] < 33 || buffer[0] == '/' || buffer[0] == ',')
	{
		readData = fread(buffer, sizeof(char), 1, fPtr);
        
        if (readData < 1 || feof(fPtr))
        {   
            return MISSING_DATA;
        }

        if (buffer[0] == '>')
        {
        	skipCommentInFile(fPtr);
        	buffer[0] = 0;
        }
	}

	if (buffer[0] == '}' || buffer[0] == '{' || buffer[0] == '=')
	{
		return LEMON_SUCCESS;
	}


	int i = 1;

	if (buffer[0] == '"')
	{
		enclosedCommand = true;
		i = 0;
	}

	while (i < capacity - 1)
	{
		readData = fread(buffer + i, sizeof(char), 1, fPtr);

		if (feof(fPtr) || readData < 1)
		{
			return MISSING_DATA;
		}

		if ((buffer[i] & 0b10000000) > 0 && (i + 4) >= capacity)
		{
			// unicode! skip if there are less than 4 bytes available
			buffer[i] = 0;
		}

		if (enclosedCommand)
		{
			if (buffer[i] == '"')
			{
				buffer[i] = 0;
				return LEMON_SUCCESS;
			}

			i++;
			continue;
		}

		if (buffer[i] < 33 || buffer[i] == '}' || buffer[i] == '{' || buffer[i] == '=' || buffer[i] == '>')		// '{' '}' '=' etc. will only be read if it is the first character
		{
            // necessary because some commands expect at least one character gap before next argument
			fseek(fPtr, -1, SEEK_CUR);
			buffer[i] = 0; 
			return LEMON_SUCCESS;
		}

		if (buffer[i] == ',') 
		{
			buffer[i] = 0;
			return LEMON_SUCCESS;
		}			//  {  }   =  >  "  ,  /

		i++;
	}


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
	getNextArg(fPtr, buffer, 41);

	if (inRange(buffer[0], '0', '9') || (buffer[0] == '-' && inRange(buffer[1], '0', '9')))
	{
		return convertStrToInt(buffer, 41);
	}

	fseek(fPtr, filePos, SEEK_SET);
	return 0;
}

bool hasNextArgInt(FILE *fPtr)
{
	if (fPtr == NULL)
	{
		return false;
	}


	long filePos = ftell(fPtr);

	char buffer[41] = {0};
	getNextArg(fPtr, buffer, 41);
	fseek(fPtr, filePos, SEEK_SET);

	if (inRange(buffer[0], '0', '9') || (buffer[0] == '-' && inRange(buffer[1], '0', '9')))
	{
		return true;
	}

	return false;
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
		if (inRange(str[i], '0', '9'))
		{
			input *= 10;
			input += (str[i] - '0');
		}
		else if (str[i] == '-' && input == 0)
		{
			polarity = -polarity;
		}
		else
		{
			i = size;
		}
		
		i++;
	}

	if (polarity == 1)
	{
		return input;
	}
	else
	{
		return -input;
	}
}


int convertIntToStr(char str[], int input)
{
	if (input == 0)
	{
		str[0] = '0';
		str[1] = '\0';
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
		if (i >= INT_MAX_LEN - 1)
		{
			str[INT_MAX_LEN - 1] = '\0';
			return i;
		}

		int power = pow(10, j);
		str[i] = ((input / power) % 10) + '0';
		j--;
		i++;
	}

	str[i] = (input % 10) + '0';
	i++;
	str[i] = 0;
	
	return i;
}

