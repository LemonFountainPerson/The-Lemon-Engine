#include "LemonEngine.h"


#ifndef LEMON_USE_CUSTOM_CALLBACKS
int StartGame(World *GameWorld)
{
	if (GameWorld->GameState == CLOSE_GAME)
	{
		return ACTION_DISABLED;
	}

	// Logic for handle flow of menus and levels, etc can go here for game start
	loadLevel(GameWorld, 1);

	return LEMON_SUCCESS;
}
#endif


// Game events
int HandleGameEvents(World *GameWorld, RenderFrame *ScreenData)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	updateConsole(ScreenData->Window, GameWorld);

	updateTyping(ScreenData->Window, GameWorld);

	if (DebugSettings.PauseEngine == ENGINE_PAUSED)
	{
		return ACTION_DISABLED;
	}


	if (buttons[LMN_MENU_OPEN] == BUTTON_PRESSED)
	{
		AcknowledgeButton(LMN_MENU_OPEN);

		if (GameWorld->GamePaused == 0)
		{
			PauseGame(GameWorld);
		}
		else
		{
			ResumeGame(GameWorld);
		}
	}

	if (GameWorld->GameEvents.eventsPending < 1)
	{
		return EXECUTION_UNNECESSARY;
	}

	GameEvent *eventList = GameWorld->GameEvents.Events;
	int i = GameWorld->GameEvents.nextAvailable;
	int count = EngineSettings.MaxGameEvents;

	while (count > 0)
	{
		ExecuteGameEvent(&eventList[i], GameWorld, ScreenData);

		i = (i + 1) % EngineSettings.MaxGameEvents;
		count--;
	}

	clearGameEvents(GameWorld);

	return LEMON_SUCCESS;
}


int ExecuteGameEvent(GameEvent *inputEvent, World *GameWorld, RenderFrame *ScreenData)
{
	if (inputEvent == NULL || GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (inputEvent->EventID == NO_EVENT)
	{
		return EXECUTION_UNNECESSARY;
	}

	if (DebugSettings.showEvents)
	{
		putConsoleStringTS("Executing event %d (%s)...", inputEvent->EventID, getEventName(inputEvent->EventID));
	}

	GameEventData *EventData = &inputEvent->EventData;

	switch (inputEvent->EventID)
	{
		case EVENT_SWITCH_LEVEL:
			loadLevel(GameWorld, (int)EventData->vars[0]);
			break;

		case EVENT_PLAY_CUTSCENE:
			{
				initialiseCutscene((int)EventData->vars[0], GameWorld);
			} break;

		case EVENT_PLAY_CUTSCENE_FROM_FILE:
			{
				initialiseCutsceneFromFile(EventData->string, GameWorld);
			} break;

		case EVENT_PLAY_SOUND:
			{
				if (EventData->vars[0] < 0.01)
				{
					EventData->vars[0] = 1.0;
				}

				PlaySound(EventData->string, EventData->vars[0], (int)EventData->vars[1]);
			} break;

		case EVENT_MOVE_PLAYER:
				GoTo(GameWorld->Player.PlayerPtr, EventData->vars[0], EventData->vars[1]);
			break;

		case EVENT_MOVE_OBJECT:
			{
				GoTo(EventData->objReference, EventData->vars[0], EventData->vars[1]);
			} break;

		case EVENT_TELEPORT_PLAYER_TO_EXIT_DOOR:
			{
				if (EventData->objReference == NULL)
				{
					break;
				}

				centerOnObject(GameWorld->Player.PlayerPtr, EventData->objReference);
				ResetPlayer(&GameWorld->Player);
				PlaySound("Objects/DoorOpen", 1.0, OBJECT_SFX);
			} break;

		case EVENT_SET_BRIGHTNESS:
			{
				SDL_SetRenderColorScale(ScreenData->Renderer, EventData->vars[0]);
			} break;

		case EVENT_CHANGE_SCREEN_SIZE:
			{	
				applyScreenSize((int)EventData->vars[0], (int)EventData->vars[1], ScreenData, GameWorld);
			} break;

		case EVENT_CHANGE_SCREEN_SIZE_SCALE:
			{
				applyScreenSizeScale((int)EventData->vars[0], (int)EventData->vars[1], &GameWorld->MainCamera, ScreenData);
			} break;

		case EVENT_ENABLE_FULLSCREEN:
			{
				applyEnableFullscreen(ScreenData, GameWorld);
			} break;

		case EVENT_DISABLE_FULLSCREEN:
			{	
				applyDisableFullscreen(ScreenData, &GameWorld->MainCamera, GameWorld);
			} break;

		case EVENT_ENABLE_FULLSCREEN_SCALE:
			{
				applyEnableFullscreenScaled(ScreenData, &GameWorld->MainCamera);
			} break;

		case EVENT_STREAM_LEVEL_PARTITION:
			{
				if (inputEvent->loadedFile == NULL)
				{
					char fileName[MAX_LEN] = {0};
					snprintf(fileName, MAX_LEN, "Level%d_Part%d", GameWorld->level, (int)EventData->vars[0]);

					inputEvent->loadedFile = openFile(fileName, LEVELDATA_ROOT, "--PARTITION_DATA--");

					if (inputEvent->loadedFile == NULL)
					{
						break;
					}
				}

				int result = loadLevelData(GameWorld, inputEvent->loadedFile, 10);
				if (result != LEMON_SUCCESS)
				{
					fclose(inputEvent->loadedFile);
					inputEvent->loadedFile = NULL;
				}
			} break;

		case EVENT_DELETE_ENVIRONMENT_OBJECTS:
			{
				deleteAllEnvironmentObjects(GameWorld->ObjectList);
			} break;

		default:
			#ifdef LEMON_USE_CUSTOM_CALLBACKS
			ExecuteCustomGameEvent(inputEvent, GameWorld, ScreenData);
			#endif
			break;
	}


	return LEMON_SUCCESS;
}


void startTyping(SDL_Window *window, Text *inputTypingText)
{
	if (SDL_TextInputActive(window))
	{
		return;
	}

	SDL_StartTextInput(window);
	
	TextSettings.userInputIndex = 0;
	memset(TextSettings.userInputString, 0, USER_INPUT_MAX_LEN);

	TextSettings.cursorXPos = 0.0;

	TextSettings.Typing = true;

	TextSettings.typingText = inputTypingText;

	return;
}

void stopTyping(SDL_Window *window)
{
	if (!TextSettings.Typing)
	{
		return;
	}

	SDL_StopTextInput(window);

	TextSettings.Typing = false;
	TextSettings.userInputIndex = 0;
	TextSettings.cursorXPos = 0.0;
	TextSettings.typingText = NULL;

	return;
}

void updateTyping(SDL_Window *window, World *GameWorld)
{
	if (!TextSettings.Typing)
	{
		return;
	}

	if (buttonPressed(LMN_BACKSPACE) && TextSettings.userInputIndex > 0)
	{
		TextSettings.userInputIndex--;				

		char buffer[USER_INPUT_MAX_LEN] = {0};
		if (TextSettings.userInputIndex < USER_INPUT_MAX_LEN - 1)
		{
			strcpy(buffer, TextSettings.userInputString + TextSettings.userInputIndex + 1);
		}
			
		TextSettings.userInputString[TextSettings.userInputIndex] = 0;
		strcat(TextSettings.userInputString, buffer);
		setCursorPos();
	}

	if (buttonPressed(LMN_LEFTARROW))
	{
		TextSettings.userInputIndex = clamp(TextSettings.userInputIndex - 1, 0, USER_INPUT_MAX_LEN);
		setCursorPos();
	}

	if (buttonPressed(LMN_RIGHTARROW))
	{
		TextSettings.userInputIndex = clamp(TextSettings.userInputIndex + 1, 0, strlen(TextSettings.userInputString));
		setCursorPos();
	}

	if (TextSettings.typingText != NULL)
	{
		updateText(TextSettings.typingText, TextSettings.userInputString);
	}

	if (buttonPressed(LMN_TYPING_END) && TextSettings.userInputString[0] != '\0')
	{
		stopTyping(window);
	}

	
	ClearInput();

	return;
}

void inputTyping(const char input[])
{
	int prevLength = strlen(TextSettings.userInputString);
	TextSettings.userInputIndex = clamp(TextSettings.userInputIndex, 0, USER_INPUT_MAX_LEN - 1);

	if (TextSettings.userInputIndex >= USER_INPUT_MAX_LEN - 1 || prevLength >= USER_INPUT_MAX_LEN - 1 || input == NULL)
	{
		return;
	}

	char buffer[USER_INPUT_MAX_LEN] = {0};

	if (TextSettings.userInputString[TextSettings.userInputIndex] != '\0')
	{
		strcpy(buffer, TextSettings.userInputString + TextSettings.userInputIndex);
	}
	
	int bytesAvailable = USER_INPUT_MAX_LEN - prevLength - 1;

	LemonStrncpy(TextSettings.userInputString + TextSettings.userInputIndex, input, bytesAvailable);

	strcat(TextSettings.userInputString, buffer);

	TextSettings.userInputIndex += strlen(TextSettings.userInputString) - prevLength;

	TextSettings.userInputString[USER_INPUT_MAX_LEN - 1] = 0;

	setCursorPos();

	return;
}

void setCursorPos(void)
{
	if (TextSettings.userInputIndex < 1)
	{
		TextSettings.cursorXPos = 0.0;
		return;
	}

	TTF_Font *font = NULL;


	int width = 0;
	int height = 0;
	int wrapWidth = 0;

	Text *typingText = TextSettings.typingText;

	if (typingText == NULL)
	{
		font = TextSettings.DebugFont;
	}
	else
	{
		font = TTF_GetTextFont(typingText->text);

		TTF_GetTextWrapWidth(typingText->text, &wrapWidth);
	}
	
	if (font == NULL)
	{
		return;
	}
	
	TTF_GetStringSizeWrapped(font, TextSettings.userInputString, TextSettings.userInputIndex, wrapWidth, &width, &height);

	if (wrapWidth > 0)
	{
		int lineSkip = TTF_GetFontLineSkip(font);

		TTF_SubString lastLine = {0};
		TTF_GetTextSubStringForLine(typingText->text, (height / lineSkip) - 1, &lastLine);

		TTF_GetStringSize(font, TextSettings.userInputString + lastLine.offset, TextSettings.userInputIndex - lastLine.offset, &width, &height);
		TextSettings.cursorYPos = (float)(height - lineSkip);
	}
	else
	{
		TextSettings.cursorYPos = 0.0;
	}

	TextSettings.cursorXPos = (float)width;

    return;
}

int deleteAllGameEvents(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameEvent *eventList = GameWorld->GameEvents.Events;

	int i = 0;
	while (i < EngineSettings.MaxGameEvents)
	{
		if (eventList[i].loadedFile != NULL)
		{
			fclose(eventList[i].loadedFile);
			eventList[i].loadedFile = NULL;
		}
		eventList[i].EventID = NO_EVENT;

		i++;
	}

	GameWorld->GameEvents.nextAvailable = 0;
	GameWorld->GameEvents.eventsPending = 0;

	return LEMON_SUCCESS;
}

int clearGameEvents(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameEvent *eventList = GameWorld->GameEvents.Events;

	int i = 0;
	while (i < EngineSettings.MaxGameEvents)
	{
		if (eventList[i].loadedFile == NULL && eventList[i].EventID != NO_EVENT)
		{
			eventList[i].EventID = NO_EVENT;
			GameWorld->GameEvents.eventsPending--;
		}

		i++;
	}

	return LEMON_SUCCESS;
}


GameEvent* findAvailableEvent(GameEventManager *Manager)
{
	GameEvent *events = Manager->Events;
	int index = Manager->nextAvailable;
	Manager->nextAvailable = (Manager->nextAvailable + 1) % EngineSettings.MaxGameEvents;

	if (events[index].EventID != NO_EVENT && events[index].loadedFile != NULL)
	{
		fclose(events[index].loadedFile);
		events[index].loadedFile = NULL;
	}

	return &events[index];
}

GameEvent* addNewGameEvent(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *eventPtr = findAvailableEvent(&GameWorld->GameEvents);

	if (eventPtr == NULL)
	{
		return NULL;
	}

	memset(&eventPtr->EventData, 0, sizeof(GameEventData));
	eventPtr->EventData.objReference = NULL;
	eventPtr->loadedFile = NULL;

	if (GameWorld->GameEvents.eventsPending < EngineSettings.MaxGameEvents)
	{
		GameWorld->GameEvents.eventsPending++;
	}

	return eventPtr;
}

int triggerGameEvent(GameEvent *inputEvent, World *GameWorld)
{
	if (inputEvent == NULL || inputEvent->EventID == NO_EVENT)
	{
		return MISSING_DATA;
	}

	GameEvent *eventPtr = addNewGameEvent(GameWorld);

	if (eventPtr == NULL)
	{
		return LEMON_ERROR;
	}

	memcpy(eventPtr, inputEvent, sizeof(GameEvent));

	return LEMON_SUCCESS;
}

// slightly cleaner than using a preprocessor define
inline void removeEventToTriggerLater(GameEvent *inputEvent, GameEvent *storage, World *GameWorld)	
{
	memcpy(storage, inputEvent, sizeof(GameEvent));
	GameWorld->GameEvents.eventsPending--;
	inputEvent->EventID = NO_EVENT;
}


GameEvent* switchLevel(int level, World *GameWorld)
{
	if (GameWorld == NULL || level < 0)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_SWITCH_LEVEL;
	newEvent->EventData.vars[0] = (float)level;

	return newEvent;
}

GameEvent* playCutscene(int scene, World *GameWorld)
{
	if (GameWorld == NULL || scene <= NO_CUTSCENE)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_PLAY_CUTSCENE;
	newEvent->EventData.vars[0] = (float)scene;

	return newEvent;
}

GameEvent* playCutsceneFromFile(const char name[], World *GameWorld)
{
	if (GameWorld == NULL || strlen(name) <= 0)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_PLAY_CUTSCENE_FROM_FILE;
	strcpy(newEvent->EventData.string, name);

	return newEvent;
}

GameEvent* Event_MovePlayer(float xPos, float yPos, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_MOVE_PLAYER;
	newEvent->EventData.vars[0] = xPos;
	newEvent->EventData.vars[1] = yPos;

	return newEvent;
}

GameEvent* Event_MoveObject(Object *input, float xPos, float yPos, World *GameWorld)
{
	if (GameWorld == NULL || input == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_MOVE_OBJECT;
	newEvent->EventData.vars[0] = xPos;
	newEvent->EventData.vars[1] = yPos;
	newEvent->EventData.objReference = input;

	return newEvent;
}

GameEvent* Event_TeleportPlayerToExitDoor(Object *dest, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_TELEPORT_PLAYER_TO_EXIT_DOOR;
	newEvent->EventData.objReference = dest;

	return newEvent;
}

GameEvent* Event_SetScreenBrightness(float brightness, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_SET_BRIGHTNESS;
	newEvent->EventData.vars[0] = brightness;

	return newEvent;
}

GameEvent* streamPartition(int partID, World *GameWorld)
{
	if (GameWorld == NULL || partID < 0)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_STREAM_LEVEL_PARTITION;
	newEvent->EventData.vars[0] = (float)partID;

	return newEvent;
}

GameEvent* scheduleEnvironmentDeletion(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_DELETE_ENVIRONMENT_OBJECTS;

	return newEvent;
}

GameEvent* switchToNewPartition(int partID, World *GameWorld)
{
	scheduleEnvironmentDeletion(GameWorld);
	return streamPartition(partID, GameWorld);
}


GameEvent* changeScreenSizeScaled(int newWidth, int newHeight, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_CHANGE_SCREEN_SIZE_SCALE;
	newEvent->EventData.vars[0] = (float)newWidth;
	newEvent->EventData.vars[1] = (float)newHeight;

	return newEvent;
}

GameEvent* changeScreenSize(int newWidth, int newHeight, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_CHANGE_SCREEN_SIZE;
	newEvent->EventData.vars[0] = (float)newWidth;
	newEvent->EventData.vars[1] = (float)newHeight;

	return newEvent;
}

GameEvent* enableFullscreen(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_ENABLE_FULLSCREEN;

	return newEvent;
}

GameEvent* enableFullscreenScaled(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_ENABLE_FULLSCREEN_SCALE;

	return newEvent;
}

GameEvent* disableFullscreen(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = EVENT_DISABLE_FULLSCREEN;

	return newEvent;
}


int applyScreenSize(int newWidth, int newHeight, RenderFrame *ScreenData, World *GameWorld)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	if (ScreenData->Fullscreen == true)
	{ 
		return ACTION_DISABLED;
	}

	if (newWidth < MINIMUM_SCREEN_WIDTH || newHeight < MINIMUM_SCREEN_HEIGHT)
	{
		return INVALID_DATA;
	}

	int prevWidth = ScreenData->screenWidth;
	int prevHeight = ScreenData->screenHeight;

	SDL_SetWindowSize(ScreenData->Window, newWidth, newHeight);
    SDL_SyncWindow(ScreenData->Window);
	SDL_GetWindowSize(ScreenData->Window, &ScreenData->screenWidth, &ScreenData->screenHeight);

	adjustHUD(prevWidth, prevHeight, ScreenData, GameWorld);

	return LEMON_SUCCESS;
}


int applyScreenSizeScale(int newWidth, int newHeight, Camera *inputCamera, RenderFrame *ScreenData)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	if (ScreenData->Fullscreen == true)
	{
		return ACTION_DISABLED;
	}

	if (newWidth < MINIMUM_SCREEN_WIDTH || newHeight < MINIMUM_SCREEN_HEIGHT)
	{
		return INVALID_DATA;
	}

	float ScaleX = ((float)inputCamera->width/(float)ScreenData->screenWidth);
	float ScaleY = ((float)inputCamera->height/(float)ScreenData->screenHeight);

	SDL_SetWindowSize(ScreenData->Window, newWidth, newHeight);
    SDL_SyncWindow(ScreenData->Window);
	SDL_GetWindowSize(ScreenData->Window, &ScreenData->screenWidth, &ScreenData->screenHeight);

	inputCamera->width = ScreenData->screenWidth * ScaleX;
	inputCamera->height = ScreenData->screenHeight * ScaleY;

	return LEMON_SUCCESS;
}


int applyEnableFullscreen(RenderFrame *ScreenData, World *GameWorld)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	if (ScreenData->Fullscreen == true)
	{
		return ACTION_DISABLED;
	}


	int prevWidth = ScreenData->screenWidth;
	int prevHeight = ScreenData->screenHeight;

	SDL_SetWindowFullscreen(ScreenData->Window, true);
	SDL_SyncWindow(ScreenData->Window);

	SDL_GetWindowSize(ScreenData->Window, &ScreenData->screenWidth, &ScreenData->screenHeight);

	ScreenData->Fullscreen = true;
	ScreenData->Scaled = false;

	adjustHUD(prevWidth, prevHeight, ScreenData, GameWorld);


	return LEMON_SUCCESS;
}

int applyEnableFullscreenScaled(RenderFrame *ScreenData, Camera *inputCamera)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	if (ScreenData->Fullscreen == true)
	{
		return ACTION_DISABLED;
	}

	validateScreenDimensions(ScreenData);

	float ScaleX = ((float)inputCamera->width/(float)ScreenData->screenWidth);
	float ScaleY = ((float)inputCamera->height/(float)ScreenData->screenHeight);

	SDL_SetWindowFullscreen(ScreenData->Window, true);
	SDL_SyncWindow(ScreenData->Window);
	SDL_GetWindowSize(ScreenData->Window, &ScreenData->screenWidth, &ScreenData->screenHeight);

	inputCamera->width = ScreenData->screenWidth * ScaleX;
	inputCamera->height = ScreenData->screenHeight * ScaleY;

	ScreenData->Fullscreen = true;
	ScreenData->Scaled = true;

	return LEMON_SUCCESS;
}


int applyDisableFullscreen(RenderFrame *ScreenData, Camera *inputCamera, World *GameWorld)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	float ScaleX = 1.0;
	float ScaleY = 1.0;
	int prevWidth = ScreenData->screenWidth;
	int prevHeight = ScreenData->screenHeight;

	if (ScreenData->Scaled)
	{
		ScaleX = ((float)inputCamera->width/(float)ScreenData->screenWidth);
		ScaleY = ((float)inputCamera->height/(float)ScreenData->screenHeight);
	}

	SDL_SetWindowFullscreen(ScreenData->Window, false);
	SDL_SyncWindow(ScreenData->Window);
	SDL_GetWindowSize(ScreenData->Window, &ScreenData->screenWidth, &ScreenData->screenHeight);

	if (ScreenData->Scaled)
	{
		inputCamera->width = ScreenData->screenWidth * ScaleX;
		inputCamera->height = ScreenData->screenHeight * ScaleY;
	}
	else
	{
		adjustHUD(prevWidth, prevHeight, ScreenData, GameWorld);
	}

	ScreenData->Fullscreen = false;
	ScreenData->Scaled = false;

	return LEMON_SUCCESS;
}


int validateScreenDimensions(RenderFrame *ScreenData)
{
	if (ScreenData->screenWidth < MINIMUM_SCREEN_WIDTH || ScreenData->screenHeight < MINIMUM_SCREEN_HEIGHT)
	{
		ScreenData->screenWidth = H_RESOLUTION;
		ScreenData->screenHeight = V_RESOLUTION;

		SDL_SetWindowSize(ScreenData->Window, H_RESOLUTION, V_RESOLUTION);

		return LEMON_ERROR;
	}

	return LEMON_SUCCESS;
}


const static char EventNames[EVENT_COUNT][EVENT_NAME_MAX_LEN] = {
	[NO_EVENT] = "No Event",
 	[EVENT_SWITCH_LEVEL] = "Switch Level",
 	[EVENT_MOVE_OBJECT] = "Move Object",
 	[EVENT_MOVE_PLAYER] = "Move Player",
 	[EVENT_TELEPORT_PLAYER_TO_EXIT_DOOR] = "Teleport Player to exit door",
 	[EVENT_PLAY_CUTSCENE] = "Play Cutscene",
 	[EVENT_PLAY_CUTSCENE_FROM_FILE] = "Play Cutscene from file",
 	[EVENT_SET_BRIGHTNESS] = "Set Brightness",
 	[EVENT_ENABLE_FULLSCREEN] = "Enable fullscreen",
 	[EVENT_DISABLE_FULLSCREEN] = "Disable fullscreen",
 	[EVENT_ENABLE_FULLSCREEN_SCALE] = "Enable fullscreen scaled",
 	[EVENT_CHANGE_SCREEN_SIZE] = "Change screen size",
 	[EVENT_CHANGE_SCREEN_SIZE_SCALE] = "Change screen size scaled",
 	[EVENT_STREAM_LEVEL_PARTITION] = "Stream Level Partition",
 	[EVENT_DELETE_ENVIRONMENT_OBJECTS] = "Delete environment Objects"
};

const char* getEventName(GameEventID input)
{
	if (input < 0 || input >= EVENT_COUNT)
	{
		return "Unmapped EventID";
	}

	return EventNames[input];
}

GameEventID getEventID(const char input[])
{
	// filled in once the first time this is called, then all modified names are cached for future file-reading use
	static char compareEventNames[EVENT_COUNT][EVENT_NAME_MAX_LEN] = {0};	

	if (compareEventNames[0][0] == '\0')
	{
		for (int i = 0; i < EVENT_COUNT; i++)
		{
			strcpy(compareEventNames[i], EventNames[i]);
			stringToLower(compareEventNames[i]);
			removeChar(compareEventNames[i], ' ', EVENT_NAME_MAX_LEN);
		}
	}

	char inputLower[MAX_LEN] = {0};

	LemonStrncpy(inputLower, input, MAX_LEN);
	stringToLower(inputLower);
	removeChar(inputLower, ' ', MAX_LEN);

	for (int i = 0; i < EVENT_COUNT; i++)
	{
		if (strcmp(inputLower, compareEventNames[i]) == 0)
		{
			return i;
		}
	}
	
	return UNDEFINED_EVENT;
}


GameEvent* getNextArgGameEvent(FILE *file, World *GameWorld)
{
	if (file == NULL || GameWorld == NULL)
	{
		return NULL;
	}

	long filePos = ftell(file);
	char buffer[MAX_LEN] = {0};

	getNextArg(file, buffer, MAX_LEN);

	GameEventID inputEvent = getEventID(buffer);	// find ID of event via name

	if (inputEvent == NO_EVENT || inputEvent == UNDEFINED_EVENT)
	{
		fseek(file, filePos, SEEK_SET);
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);	// create empty slot for new event

	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->EventID = inputEvent;			// if succesful, set ID of empty slot


	ObjectController *ObjectList = GameWorld->ObjectList;
	filePos = ftell(file);
	getNextArg(file, buffer, MAX_LEN);

	if (buffer[0] != '{')		// if there are arguments supplied (via { String: "StringInput", Vars: 1.0 2.0 3.0, ... etc. }), read them in
	{
		fseek(file, filePos, SEEK_SET);
		return newEvent;
	}

	getNextArg(file, buffer, MAX_LEN);

	while (!endOfFile(file) && buffer[0] != '}')
	{
		stringToLower(buffer);

		if (strcmp(buffer, "string:") == 0)
		{
			getNextArg(file, newEvent->EventData.string, MAX_LEN);
		}
		else if (strcmp(buffer, "vars:") == 0 || strcmp(buffer, "variables:") == 0)
		{
			for (int i = 0; i < GAME_EVENT_VAR_COUNT; i++)
			{
				newEvent->EventData.vars[i] = getNextArgFloat(file);
			}
		}
		else if (strcmp(buffer, "object:") == 0 && ObjectList != NULL)
		{
			if (hasNextArgNumber(file))
			{
				int index = getNextArgInt(file);
				if (inRange(index, 0, EngineSettings.MaxObjects - 1))
				{
					newEvent->EventData.objReference = &ObjectList->objectComponents.Objects[index];
				}
			}
			else
			{
				getNextArg(file, buffer, MAX_LEN);

				newEvent->EventData.objReference = FindObject(buffer, ObjectList);
			}
		}

		getNextArg(file, buffer, MAX_LEN);
	}

	return newEvent;
}


void setTickNumber(Uint64 input);
static Uint64 prevTickVal = 0;

int PauseGame(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState == EMPTY_GAME || GameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}

	GameWorld->GamePaused = 1;
	AcknowledgeHeldButtons();
	GameWorld->MainCamera.CameraMode = MENU_CAMERA;
	CameraControl(GameWorld, &GameWorld->MainCamera);
	HideHUD(GameWorld->ObjectList);
	prevTickVal = TickNumber();

	AddObject(GameWorld, UI_ELEMENT, 0, 0, PAUSE_MENU_CONTROLLER, 0, 0, 0, 0);

	// In order to have objects be visible in the pause menu while hiding objects from the previous scene, the camera is moved elsewhere
	// and is restored to its previous position when unpaused  (There should not be any level geometry before X pos 0)
	// When game is resumed, even if pos is not reset to original value, it will be corrected to 0 by WorldCameraControl
	// X pos/Y pos is saved in camera[X/Y]Buffer


	return LEMON_SUCCESS;
}


int ResumeGame(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GamePaused == 0)
	{
		return EXECUTION_UNNECESSARY;
	}


	GameWorld->GamePaused = 0;
	GameWorld->MainCamera.CameraMode = FOLLOW_PLAYER;
	ShowHUD(GameWorld->ObjectList);
	setTickNumber(prevTickVal);

	return LEMON_SUCCESS;
}


bool detectPlayer(Object* inputObject, PlayerData *Player)
{
	if (Player == NULL || Player->PlayerBox == NULL || inputObject == NULL)
	{
		return false;
	}

	int touchingPlayer = checkBoxOverlapsBoxBroad(Player->PlayerBox, inputObject->ObjectBox);

	if (touchingPlayer == 1 && inputObject->Action == 0)
	{
		inputObject->Action = 1;
		return true;
	}

	if (touchingPlayer == 0)
	{
		if (inputObject->Action == 2)
		{
			inputObject->Action = -1;
		}
		else 
		{
			inputObject->Action = 0; 
		}
	}


	return false;
}


int mapPhysicsBoxToCamera(PhysicsBox *inputBox, Camera inputCam)
{
	if (inputBox == NULL)
	{
		return MISSING_DATA;
	}	

	resetPhysicsBox(inputBox);

	inputBox->xPos = inputCam.CameraX - (inputCam.width / 2);
	inputBox->yPos = inputCam.CameraY - (inputCam.height / 2);
	inputBox->xSize = inputCam.width;
	inputBox->ySize = inputCam.height;

	return LEMON_SUCCESS;
}

bool detectCamera(Object* inputObject, Camera inputCamera)
{
	if (inputObject == NULL)
	{
		return false;
	}

	PhysicsBox camBox = {0};
	mapPhysicsBoxToCamera(&camBox, inputCamera);

	int touchingCamBox = checkBoxOverlapsBoxBroad(&camBox, inputObject->ObjectBox);

	if (touchingCamBox == 1 && inputObject->Action == 0)
	{
		inputObject->Action = 1;
		return true;
	}

	if (touchingCamBox == 0)
	{
		if (inputObject->Action == 2)
		{
			inputObject->Action = -1;
		}
		else 
		{
			inputObject->Action = 0; 
		}
	}

	return false;
}


int InitialiseLevelFlag(Object *inputObject, ObjectController *ObjectList)
{
	if (inputObject == NULL || inputObject->ObjectID != LEVEL_FLAG_OBJ)
	{
		return INVALID_DATA;
	}

	SetDrawPriorityToFront(ObjectList, inputObject);

	inputObject->ObjectBox->solid = UNSOLID;
	setRenderModeOverride(inputObject, DO_NOT_RENDER);
	inputObject->ObjectBox->xSize = inputObject->arg2;
	inputObject->ObjectBox->ySize = inputObject->arg3;

	return LEMON_SUCCESS;
}

int UpdateFlagObject(Object* flag, World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL || flag == NULL)
	{
		return MISSING_DATA;
	}

	PlayerData *Player = &GameWorld->Player;

	if (GameWorld->GameState != GAMEPLAY)
	{
		return ACTION_DISABLED;
	}


	switch (getSubType(flag))
	{
		case CACHE_TRIGGER:
		if (detectCamera(flag, GameWorld->MainCamera))
		{
			PhysicsBox boundingBox;
			mapPhysicsBoxToCamera(&boundingBox, GameWorld->MainCamera);

			cacheObjects(GameWorld->ObjectList, boundingBox);
			flag->Action = 2;
		} break;


		case CUTSCENE_TRIGGER:
		if (detectPlayer(flag, Player))
		{
			playCutscene(flag->arg2, GameWorld);
			MarkObjectForDeletion(flag);
		} break;

		case LEVEL_TRIGGER:
		if (detectPlayer(flag, Player))
		{
			switchLevel(flag->arg2, GameWorld);
		}
		break;

		case LEVEL_TRIGGER_SEAMLESS:
		if (detectPlayer(flag, Player))
		{
			Player->PlayerPtr->reserved |= RFLAG_PRESERVE_ONCE;
			switchLevel(flag->arg2, GameWorld);
		}
		break;

		case DELETE_OBJECT_TRIGGER:
		{
			Object *cursor = GameWorld->ObjectList->firstObject;
			PhysicsBox *box;
			Object *current;

			while (cursor != NULL)
			{
				current = cursor;
				cursor = cursor->nextObject;

				if (getDisplayLayer(current) == HUD || current->State == STATIC_STATE)
				{
					continue;
				}

				box = current->ObjectBox;
				if (checkBoxOverlapsBoxBroad(box, flag->ObjectBox) && (flag->arg2 == 0 || box->solid != UNSOLID))
				{
					MarkObjectForDeletion(current);
				}
			}
		} break;

		case DELETE_BODY_TRIGGER:
		{
			Object *cursor = GameWorld->ObjectList->firstObject;
			PhysicsBox *box;
			Object *current;

			while (cursor != NULL)
			{
				current = cursor;
				cursor = cursor->nextObject;

				if (getDisplayLayer(current) == HUD || current->State == STATIC_STATE)
				{
					continue;
				}

				box = current->ObjectBox;
				if (box->solid == BODY && checkBoxOverlapsBoxBroad(box, flag->ObjectBox))
				{
					MarkObjectForDeletion(current);
				}
			}
		} break;

		case SET_BACKGROUND_TRIGGER:
		if (detectPlayer(flag, Player))
		{
			switchBackGroundSprite(flag->arg2, flag->arg3, &GameWorld->WorldBackground);
		} break;


		case FALSE_CAMERA_BOUNDARY:
		{
			if (detectPlayer(flag, Player))
			{
				MarkObjectForDeletion(flag);
			}
		}

		case CAMERA_BOUNDARY:
		{
			Camera *cam = &GameWorld->MainCamera;
			PhysicsBox *box = flag->ObjectBox;
			float halfWidth = (float)(cam->width / 2);
			float halfHeight = (float)(cam->height / 2);

			if (!(cam->CameraY - halfHeight > box->yPos + box->ySize || cam->CameraY + halfHeight < box->yPos))
			{
				if (cam->prevCameraX - halfWidth > box->xPos + box->xSize - 1.0 && cam->CameraX - halfWidth < box->xPos + box->xSize)
				{
					cam->CameraX = box->xPos + box->xSize + halfWidth;
					cam->prevCameraX = cam->CameraX;
				}
				
				if (cam->prevCameraX + halfWidth < box->xPos + 1.0 && cam->CameraX + halfWidth > box->xPos)
				{
					cam->CameraX = box->xPos - halfWidth;
					cam->prevCameraX = cam->CameraX;
				}
			}

			if (cam->CameraX - halfWidth > box->xPos + box->xSize || cam->CameraX + halfWidth < box->xPos)
			{
				return LEMON_SUCCESS;
			}

			if (cam->prevCameraY - halfHeight > box->yPos + box->ySize - 1.0 && cam->CameraY - halfHeight < box->yPos + box->ySize)
			{
				cam->CameraY = box->yPos + box->ySize + halfHeight;
				cam->prevCameraY = cam->CameraY;
			}
			
			if (cam->prevCameraY + halfHeight < box->yPos + 1.0 && cam->CameraY + halfHeight > box->yPos)
			{
				cam->CameraY = box->yPos - halfHeight;
				cam->prevCameraY = cam->CameraY;
			}
		} break;

		case LOAD_PART_TRIGGER:
		if (detectPlayer(flag, Player))
		{
			streamPartition(flag->arg2, GameWorld);
		}
		break;

		case SWITCH_TO_NEW_PART_TRIGGER:
		if (detectPlayer(flag, Player))
		{
			switchToNewPartition(flag->arg2, GameWorld);
		} break;

		case SET_PLAYER_LAYER:
		if (detectPlayer(flag, Player))
		{
			setDisplayLayer(Player->PlayerPtr, getDisplayLayer(flag));
		} break;

		case PLAY_SOUND_TRIGGER:
		if (detectPlayer(flag, Player))
		{
			MarkObjectForDeletion(flag);
			//Entity *triggerEntity = getEntity(flag);
			//if (triggerEntity == NULL) { return TASK_FAILED; }
			//PlaySound(triggerEntity->SoundMeta.name, triggerEntity->SoundMeta.folder, triggerEntity->SoundMeta.channel, triggerEntity->SoundMeta.volume);
		} break;

		default:
		#ifndef LEMON_USE_CUSTOM_CALLBACKS
		MarkObjectForDeletion(flag);
		#else 
		UpdateCustomLevelFlag(flag, GameWorld);
		#endif
		break;
	}


	return LEMON_SUCCESS;
}
