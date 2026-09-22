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

	// char counter[30];
	// snprintf(counter, 30, "CoinCount: %d", GameWorld->Player.coinCount);

	// attachTextToObject(addTextWithName(counter, "CoinCounter", -360.0, 260.0, GameWorld), GameWorld->Player.PlayerPtr);


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

	deleteAllGameEvents(&GameWorld->GameEvents);

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

	if (Networking.connectionStatus == CONNECT_STATE_CONNECTED && (Networking.clientID == SERVER_CLIENT_ID || inputEvent->clientID == Networking.clientID))	// if we are not the server, don't send events you don't own
	{
		saveEventForNetworkTransmission(inputEvent);

		if (Networking.connectMode == CLIENT)
		{
			return LEMON_SUCCESS;
		}
	}

	if (DebugSettings.showEvents)
	{
		putConsoleTS("Executing event %d (%s)...", inputEvent->EventID, getEventName(inputEvent->EventID));
	}

	switch (inputEvent->EventID)
	{
		case EVENT_SWITCH_LEVEL:
		//	loadLevel(GameWorld, getGameEventInt(inputEvent, "level"));		Using NULL will simply pick the first instance of an Integer argument
			loadLevel(GameWorld, getGameEventInt(inputEvent, NULL));
			break;

		case EVENT_PLAY_CUTSCENE:
			{
				initialiseCutscene(getGameEventInt(inputEvent, NULL), GameWorld);
			} break;

		case EVENT_PLAY_CUTSCENE_FROM_FILE:
			{
				initialiseCutsceneFromFile(getGameEventString(inputEvent, NULL), GameWorld);
			} break;

		case EVENT_SET_GAME_FLAG:
			{
				char *flagName = getGameEventString(inputEvent, NULL);
				int newValue = getGameEventInt(inputEvent, NULL);
				setGameFlag(flagName, newValue);
			} break;

		case EVENT_CHANGE_GAME_FLAG:
			{
				char *flagName = getGameEventString(inputEvent, NULL);
				int changeValue = getGameEventInt(inputEvent, NULL);
				changeGameFlagBy(flagName, changeValue);
			} break;

		case EVENT_PLAY_SOUND:
			{
				char *path = getGameEventString(inputEvent, NULL);

				float volume = getGameEventFloat(inputEvent, "volume");
				if (volume < 0.01)
				{
					volume = 1.0;
				}

				int channel = getGameEventInt(inputEvent, "channel");

				PlaySound(path, volume, channel);
			} break;

		case EVENT_CHAT_MESSAGE:
			{
				int speakerID = getGameEventInt(inputEvent, "speaker");
				if (inputEvent->clientID != speakerID && inputEvent->clientID > SERVER_CLIENT_ID)
				{
					break;	// should we accept and correct or discard?
				}
				char *msg = getGameEventString(inputEvent, "message");
				Uint64 tickSent = getGameEventTick(inputEvent, NULL);

				addMessageToChatLog(msg, speakerID, tickSent);
			} break;

		case EVENT_MOVE_PLAYER:
			{
				float xPos = getGameEventFloat(inputEvent, "xPos");
				float yPos = getGameEventFloat(inputEvent, "yPos");

				GoTo(GameWorld->Player.PlayerPtr, xPos, yPos);
			} break;

		case EVENT_TELEPORT_PLAYER_TO_EXIT_DOOR:
			{
				float xPos = getGameEventFloat(inputEvent, "xPos");
				float yPos = getGameEventFloat(inputEvent, "yPos");

				centerOnXY(GameWorld->Player.PlayerPtr, xPos, yPos);
				ResetPlayer(&GameWorld->Player);
				PlaySound("Objects/DoorOpen", 1.0, OBJECT_SFX);
			} break;

		case EVENT_SET_BRIGHTNESS:
			{
				SDL_SetRenderColorScale(ScreenData->Renderer, getGameEventFloat(inputEvent, NULL));
			} break;

		case EVENT_SET_SCREEN_SIZE:
			{	
				int width = getGameEventInt(inputEvent, "width");
				int height = getGameEventInt(inputEvent, "height");
				applyScreenSize(width, height, ScreenData, GameWorld);
			} break;

		case EVENT_SET_SCREEN_SIZE_SCALE:
			{
				int width = getGameEventInt(inputEvent, "width");
				int height = getGameEventInt(inputEvent, "height");
				applyScreenSizeScale(width, height, &GameWorld->MainCamera, ScreenData);
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

		case EVENT_LOAD_LEVEL_PARTITION:
			{
				char fileName[MAX_LEN] = {0};
				int partID = getGameEventInt(inputEvent, NULL);
				snprintf(fileName, MAX_LEN, "Level%d_Part%d", GameWorld->level, partID);

				FILE *file = openFile(fileName, LEVELDATA_ROOT, "--PARTITION_DATA--");

				if (file == NULL)
				{
					break;
				}
				
				loadLevelData(GameWorld, file, true);
			} break;

		case EVENT_CONSOLE_COMMAND:
			{
				consoleInput(getGameEventString(inputEvent, "command"), GameWorld);
			} break;

		case EVENT_SET_TICKRATE:
			{
				EngineSettings.GameTicksPerSecond = getGameEventInt(inputEvent, "tickrate");

				EngineSettings.TickDelta = (Uint64)(NS_PER_SECOND/(double)EngineSettings.GameTicksPerSecond);

				if (EngineSettings.GameTicksPerSecond == RenderSettings.RendersPerSecond)
				{
			        RenderSettings.RenderDelta = EngineSettings.TickDelta;
				}
			} break;

		default:
			#ifdef LEMON_USE_CUSTOM_CALLBACKS
			ExecuteCustomGameEvent(inputEvent, GameWorld, ScreenData);
			#endif
			break;
	}


	return LEMON_SUCCESS;
}

int deleteAllGameEvents(GameEventManager *manager)
{
	if (manager == NULL)
	{
		return MISSING_DATA;
	}

	GameEvent *eventList = manager->Events;

	int i = 0;
	while (i < EngineSettings.MaxGameEvents)
	{
		eventList[i].EventID = NO_EVENT;

		i++;
	}

	manager->nextAvailable = 0;
	manager->eventsPending = 0;

	return LEMON_SUCCESS;
}


GameEvent* findAvailableEvent(GameEventManager *Manager)
{
	GameEvent *events = Manager->Events;
	int index = Manager->nextAvailable % EngineSettings.MaxGameEvents;
	Manager->nextAvailable = (Manager->nextAvailable + 1) % EngineSettings.MaxGameEvents;

	return &events[index];
}

int eventNetworkingPermission(GameEventID input)
{
	switch(input)
	{
	case EVENT_SWITCH_LEVEL:
	case EVENT_PLAY_CUTSCENE:
	case EVENT_PLAY_CUTSCENE_FROM_FILE:
	case EVENT_PLAY_SOUND:
	case EVENT_SET_GAME_FLAG:
	case EVENT_CHANGE_GAME_FLAG:
	case EVENT_CHAT_MESSAGE:
	case EVENT_SET_TICKRATE:
		return Networking.clientID;

		// events that should not be broadcasted
	default:
		return LOCAL_CLIENT_ID;
	}
}

GameEvent* addNewGameEvent(GameEventID eventID, World *GameWorld)
{
	if (GameWorld == NULL || eventID <= NO_EVENT)
	{
		return NULL;
	}

	GameEvent *eventPtr = findAvailableEvent(&GameWorld->GameEvents);

	if (eventPtr == NULL)
	{
		return NULL;
	}

	GameEventArg *args = eventPtr->args;
	for (int index = 0; index < EVENT_VAR_COUNT; index++)
	{	
		args[index].type = ARG_UNUSED;		
	}
	
	if (GameWorld->GameEvents.eventsPending < EngineSettings.MaxGameEvents)
	{
		GameWorld->GameEvents.eventsPending++;
	}

	eventPtr->EventID = eventID;

	eventPtr->clientID = eventNetworkingPermission(eventID);

	return eventPtr;
}

int triggerGameEvent(GameEvent *inputEvent, World *GameWorld)
{
	if (inputEvent == NULL || inputEvent->EventID == NO_EVENT)
	{
		return MISSING_DATA;
	}

	GameEvent *eventPtr = addNewGameEvent(inputEvent->EventID, GameWorld);

	if (eventPtr == NULL)
	{
		return LEMON_ERROR;
	}
	
	int clientID = eventPtr->clientID;
	memcpy(eventPtr, inputEvent, sizeof(GameEvent));
	eventPtr->clientID = clientID;
	
	return LEMON_SUCCESS;
}

// slightly cleaner than using a preprocessor define
inline void removeEventToTriggerLater(GameEvent *inputEvent, GameEvent *storage, World *GameWorld)	
{
	memcpy(storage, inputEvent, sizeof(GameEvent));
	GameWorld->GameEvents.eventsPending--;

	memset(inputEvent, 0, sizeof(GameEvent));
	inputEvent->EventID = NO_EVENT;
}


GameEvent* switchLevel(int level, World *GameWorld)
{
	if (GameWorld == NULL || level < 0)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(EVENT_SWITCH_LEVEL, GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	addGameEventInt(newEvent, "level", level);

	return newEvent;
}

GameEvent* playCutscene(int scene, World *GameWorld)
{
	if (GameWorld == NULL || scene <= NO_CUTSCENE)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(EVENT_PLAY_CUTSCENE, GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	addGameEventInt(newEvent, "cutsceneID", scene);

	return newEvent;
}

GameEvent* playCutsceneFromFile(const char name[], World *GameWorld)
{
	if (GameWorld == NULL || name == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(EVENT_PLAY_CUTSCENE_FROM_FILE, GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	addGameEventString(newEvent, "cutsceneName", name);

	return newEvent;
}

GameEvent* Message(char msg[], int speakerID, World *GameWorld)
{
	if (GameWorld == NULL || msg == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(EVENT_CHAT_MESSAGE, GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	addGameEventString(newEvent, "message", msg);
	addGameEventInt(newEvent, "speaker", speakerID);
	addGameEventTick(newEvent, "tickSent", TickNumber());

	if (speakerID == LOCAL_CLIENT_ID)
	{
		newEvent->clientID = LOCAL_CLIENT_ID;
	}

	return newEvent;
}

GameEvent* Event_MovePlayer(float xPos, float yPos, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(EVENT_MOVE_PLAYER, GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	addGameEventFloat(newEvent, "xPos", xPos);
	addGameEventFloat(newEvent, "yPos", yPos);

	return newEvent;
}

GameEvent* Event_TeleportPlayerToExitDoor(Object *dest, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(EVENT_TELEPORT_PLAYER_TO_EXIT_DOOR, GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	addGameEventFloat(newEvent, "xPos", dest->ObjectBox->xPos + (dest->ObjectBox->xSize / 2));
	addGameEventFloat(newEvent, "yPos", dest->ObjectBox->yPos + (dest->ObjectBox->ySize / 2));

	return newEvent;
}

GameEvent* streamPartition(int partID, World *GameWorld)
{
	if (GameWorld == NULL || partID < 0)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(EVENT_LOAD_LEVEL_PARTITION, GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	addGameEventFloat(newEvent, "partID", partID);

	return newEvent;
}

GameEvent* Event_SetScreenBrightness(float brightness, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(EVENT_SET_BRIGHTNESS, GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	addGameEventFloat(newEvent, "brightness", brightness);

	return newEvent;
}

GameEvent* changeScreenSizeScaled(int newWidth, int newHeight, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(EVENT_SET_SCREEN_SIZE_SCALE, GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	addGameEventInt(newEvent, "width", newWidth);
	addGameEventInt(newEvent, "height", newHeight);

	return newEvent;
}

GameEvent* changeScreenSize(int newWidth, int newHeight, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(EVENT_SET_SCREEN_SIZE, GameWorld);
	if (newEvent == NULL)
	{
		return NULL;
	}

	addGameEventInt(newEvent, "width", newWidth);
	addGameEventInt(newEvent, "height", newHeight);

	return newEvent;
}

GameEvent* enableFullscreen(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(EVENT_ENABLE_FULLSCREEN, GameWorld);

	return newEvent;
}

GameEvent* enableFullscreenScaled(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(EVENT_ENABLE_FULLSCREEN_SCALE, GameWorld);

	return newEvent;
}

GameEvent* disableFullscreen(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	GameEvent *newEvent = addNewGameEvent(EVENT_DISABLE_FULLSCREEN, GameWorld);

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
 	[EVENT_PLAY_CUTSCENE] = "Play Cutscene",
 	[EVENT_PLAY_CUTSCENE_FROM_FILE] = "Play Cutscene from file",
 	[EVENT_SET_GAME_FLAG] = "Set Game Flag",
 	[EVENT_CHANGE_GAME_FLAG] = "Change Game Flag",
 	[EVENT_PLAY_SOUND] = "Play Sound",
 	[EVENT_CHAT_MESSAGE] = "Chat Message",
 	[EVENT_MOVE_PLAYER] = "Move Player",
 	[EVENT_TELEPORT_PLAYER_TO_EXIT_DOOR] = "Teleport Player to exit door",
 	[EVENT_LOAD_LEVEL_PARTITION] = "Load Level Partition",
 	[EVENT_CONSOLE_COMMAND] = "Console Command",
 	[EVENT_SET_TICKRATE] = "Set Tickrate",
 	[EVENT_SET_BRIGHTNESS] = "Set Brightness",
 	[EVENT_ENABLE_FULLSCREEN] = "Enable fullscreen",
 	[EVENT_DISABLE_FULLSCREEN] = "Disable fullscreen",
 	[EVENT_ENABLE_FULLSCREEN_SCALE] = "Enable fullscreen scaled",
 	[EVENT_SET_SCREEN_SIZE] = "Set screen size",
 	[EVENT_SET_SCREEN_SIZE_SCALE] = "Set screen size scaled"
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


bool getNextArgGameEvent(FILE *file, GameEvent *newEvent, World *GameWorld)
{
	if (file == NULL)
	{
		return false;
	}

	if (GameWorld == NULL || newEvent == NULL)
	{
		return false;
	}

	long filePos = ftell(file);
	char buffer[MAX_LEN] = {0};

	getNextArg(file, buffer, MAX_LEN);

	GameEventID inputEvent = getEventID(buffer);	// find ID of event via name

	if (inputEvent == NO_EVENT || inputEvent == UNDEFINED_EVENT)
	{
		fseek(file, filePos, SEEK_SET);
		return false;
	}

	memset(newEvent, 0, sizeof(GameEvent));
	newEvent->EventID = inputEvent;			// if succesful, set ID of empty slot

	filePos = ftell(file);
	getNextArg(file, buffer, MAX_LEN);

	if (buffer[0] != '{')		// if there are arguments supplied (via { String: "StringInput", Vars: 1.0 2.0 3.0, ... etc. }), read them in
	{
		fseek(file, filePos, SEEK_SET);
		return true;
	}

	getNextArg(file, buffer, MAX_LEN);

	while (!endOfFile(file) && buffer[0] != '}')
	{
		char type[MAX_LEN] = {0};
		strcpy(type, buffer);
		stringToLower(type);

		if (strcmp(type, "string") == 0 || strcmp(type, "text") == 0)
		{
			char name[EVENT_ARG_NAME_MAX_LEN] = {0};
			getNextArg(file, name, EVENT_ARG_NAME_MAX_LEN);
			consumeStatement(file, '=');

			char text[MESSAGE_LENGTH] = {0};
			getNextArg(file, text, MESSAGE_LENGTH);

			addGameEventString(newEvent, name, text);
		}
		else if (strcmp(type, "int") == 0 || strcmp(type, "integer") == 0)
		{
			char name[EVENT_ARG_NAME_MAX_LEN] = {0};
			getNextArg(file, name, EVENT_ARG_NAME_MAX_LEN);
			consumeStatement(file, '=');

			int val = getNextArgInt(file);

			addGameEventInt(newEvent, name, val);
		}
		else if (strcmp(type, "float") == 0 || strcmp(type, "number") == 0)
		{
			char name[EVENT_ARG_NAME_MAX_LEN] = {0};
			getNextArg(file, name, EVENT_ARG_NAME_MAX_LEN);
			consumeStatement(file, '=');

			float val = getNextArgFloat(file);

			addGameEventFloat(newEvent, name, val);
		}
		else
		{
			// if type is not specified, assume it is a float if next arg is a number, otherwise a string
			char equals[3] = {0};
			getNextArg(file, equals, 3);

			if (strcmp(equals, "=") == 0)
			{
				if (hasNextArgNumber(file))
				{
					float val = getNextArgFloat(file);

					addGameEventFloat(newEvent, buffer, val);
				}
				else
				{
					char text[MESSAGE_LENGTH] = {0};
					getNextArg(file, text, MESSAGE_LENGTH);

					addGameEventString(newEvent, buffer, text);
				}
			}
		}
		

		getNextArg(file, buffer, MAX_LEN);
	}

	return true;
}


void addGameEventInt(GameEvent *input, const char name[], int val)
{
	if (input == NULL)
	{
		return;
	}

	// find available arg slot
	int i = 0;
	while (i < EVENT_VAR_COUNT && input->args[i].type != ARG_UNUSED)
	{
		i++;
	}

	if (i >= EVENT_VAR_COUNT)
	{
		return;
	}

	GameEventArg *newArg = &input->args[i];
	
	LemonStrncpy(newArg->name, name, EVENT_ARG_NAME_MAX_LEN);

	newArg->type = ARG_INTEGER;
	newArg->data.iNumber = val;

	return;
}

int getGameEventInt(GameEvent *input, const char name[])
{
	if (input == NULL)
	{
		return 0;
	}

	GameEventArg *args = input->args;

	for (int i = 0; i < EVENT_VAR_COUNT; i++)
	{
		if (args[i].type == ARG_INTEGER && (name == NULL || strcmp(args[i].name, name) == 0))
		{
			return args[i].data.iNumber;
		}
	}

	return 0;
}


void addGameEventFloat(GameEvent *input, const char name[], float val)
{
	if (input == NULL)
	{
		return;
	}

	// find available arg slot
	int i = 0;
	while (i < EVENT_VAR_COUNT && input->args[i].type != ARG_UNUSED)
	{
		i++;
	}

	if (i >= EVENT_VAR_COUNT)
	{
		return;
	}

	GameEventArg *newArg = &input->args[i];
	
	LemonStrncpy(newArg->name, name, EVENT_ARG_NAME_MAX_LEN);

	newArg->type = ARG_FLOAT;
	newArg->data.fNumber = val;

	return;
}

int getGameEventFloat(GameEvent *input, const char name[])
{
	if (input == NULL)
	{
		return 0.0;
	}

	GameEventArg *args = input->args;

	for (int i = 0; i < EVENT_VAR_COUNT; i++)
	{
		if (args[i].type == ARG_FLOAT && (name == NULL || strcmp(args[i].name, name) == 0))
		{
			return args[i].data.fNumber;
		}
	}

	return 0.0;
}

void addGameEventString(GameEvent *input, const char name[], const char val[])
{
	if (input == NULL)
	{
		return;
	}

	int length = strlen(val);

	if (length >= MESSAGE_LENGTH)
	{
		return;
	}

	// find available arg slot
	int i = 0;
	while (i < EVENT_VAR_COUNT && input->args[i].type != ARG_UNUSED)
	{
		i++;
	}

	if (i >= EVENT_VAR_COUNT)
	{
		return;
	}

	GameEventArg *newArg = &input->args[i];

	LemonStrncpy(newArg->name, name, EVENT_ARG_NAME_MAX_LEN);
	newArg->type = ARG_STRING;

	LemonStrncpy(newArg->data.string, val, MESSAGE_LENGTH);

	return;
}

char* getGameEventString(GameEvent *input, const char name[])
{
	if (input == NULL)
	{
		return NULL;
	}

	GameEventArg *args = input->args;

	for (int i = 0; i < EVENT_VAR_COUNT; i++)
	{
		if (args[i].type == ARG_STRING && (name == NULL || strcmp(args[i].name, name) == 0))
		{
			return args[i].data.string;
		}
	}

	return NULL;
}


void addGameEventTick(GameEvent *input, const char name[], Uint64 val)
{
	if (input == NULL)
	{
		return;
	}

	// find available arg slot
	int i = 0;
	while (i < EVENT_VAR_COUNT && input->args[i].type != ARG_UNUSED)
	{
		i++;
	}

	if (i >= EVENT_VAR_COUNT)
	{
		return;
	}

	GameEventArg *newArg = &input->args[i];
	
	LemonStrncpy(newArg->name, name, EVENT_ARG_NAME_MAX_LEN);

	newArg->type = ARG_TICK;
	newArg->data.tickValue = val;

	return;
}

Uint64 getGameEventTick(GameEvent *input, const char name[])
{
	if (input == NULL)
	{
		return 0;
	}

	GameEventArg *args = input->args;

	for (int i = 0; i < EVENT_VAR_COUNT; i++)
	{
		if (args[i].type == ARG_TICK && (name == NULL || strcmp(args[i].name, name) == 0))
		{
			return args[i].data.tickValue;
		}
	}

	return 0;
}

// specifically just to clean up any strings that were allocated
// void cleanUpGameEventArgs(GameEvent *input)
// {
// 	if (input == NULL || input->EventID == NO_EVENT || !input->deleteArgs)
// 	{
// 		return;
// 	}

// 	GameEventArg *args = input->args;

// 	for (int index = 0; index < EVENT_VAR_COUNT; index++)
// 	{
// 		if (args[index].type == ARG_STRING && args[index].data.string != NULL)
// 		{
// 			free(args[index].data.string);
// 			args[index].data.string = NULL;
// 		}	

// 		args[index].type = ARG_UNUSED;
// 	}

// 	return;
// }

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
	enableMenuCamera(&GameWorld->MainCamera, -3200.0, 0.0);
	HideHUD(&GameWorld->ObjectList);
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
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GamePaused == 0)
	{
		return EXECUTION_UNNECESSARY;
	}


	GameWorld->GamePaused = 0;
	disableMenuCamera(&GameWorld->MainCamera, FOLLOW_PLAYER);
	ShowHUD(&GameWorld->ObjectList);

	if (Networking.connectMode == OFFLINE)
	{
		setTickNumber(prevTickVal);
	}
	
	return LEMON_SUCCESS;
}


bool detectPlayer(Object* inputObject, PlayerData *Player)
{
	if (Player == NULL || Player->PlayerPtr == NULL || inputObject == NULL)
	{
		return false;
	}

	int touchingPlayer = checkBoxOverlapsBoxBroad(Player->PlayerPtr->ObjectBox, inputObject->ObjectBox);

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
	if (GameWorld == NULL || flag == NULL)
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
		case GAME_EVENT_TRIGGER:
		{
			if (detectPlayer(flag, Player))
			{
				triggerObjectEvent(flag, GameWorld);

				if (!hasObjectEvent(flag, GameWorld))
				{
					MarkObjectForDeletion(flag);
				}
			}
		} break;

		case CACHE_TRIGGER:
		if (detectCamera(flag, GameWorld->MainCamera))
		{
			PhysicsBox boundingBox;
			mapPhysicsBoxToCamera(&boundingBox, GameWorld->MainCamera);

			cacheObjects(&GameWorld->ObjectList, boundingBox);
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
			Object *cursor = GameWorld->ObjectList.firstObject;
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
			Object *cursor = GameWorld->ObjectList.firstObject;
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

		case SET_PLAYER_LAYER:
		if (detectPlayer(flag, Player))
		{
			setDisplayLayer(Player->PlayerPtr, getDisplayLayer(flag));
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
