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

	addTextWithName("CoinCount: 0", "CoinCounter", -600.0, 300.0);

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

	updateTypedCommand(ScreenData->Window, GameWorld);

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
			if (EventData->newLevelID > -1)
			{
				loadLevel(GameWorld, EventData->newLevelID);
			} break;

		case EVENT_PLAY_CUTSCENE:
			{
				initialiseCutscene(EventData->sceneID, GameWorld);
			} break;

		case EVENT_PLAY_CUTSCENE_FROM_FILE:
			{
				putConsoleStringTS("Bruh");
				initialiseCutsceneFromFile(EventData->sceneName, GameWorld);
			} break;

		case EVENT_MOVE_PLAYER:
				GoTo(GameWorld->Player.PlayerPtr, EventData->ObjectGoTo[0], EventData->ObjectGoTo[1]);
			break;

		case EVENT_MOVE_OBJECT:
			{
				// need a better method than this
				int index = (int)EventData->ObjectGoTo[2];
				ObjectController *ObjectList = GameWorld->ObjectList;
				if (index < 0 || index >= EngineSettings.MaxObjects || ObjectList == NULL)
				{
					break;
				}

				Object *object = &ObjectList->objectComponents.Objects[index];
				GoTo(object, EventData->ObjectGoTo[0], EventData->ObjectGoTo[1]);
			} break;

		case EVENT_TELEPORT_PLAYER_TO_EXIT_DOOR:
			{
				if (EventData->object == NULL)
				{
					break;
				}

				centerOnObject(GameWorld->Player.PlayerPtr, EventData->object);
				ResetPlayer(&GameWorld->Player);
				PlaySound("Objects/DoorOpen", OBJECT_SFX, 1.0);
			} break;

		case EVENT_SET_BRIGHTNESS:
			{
				SDL_SetRenderColorScale(ScreenData->Renderer, EventData->colourScale);
			} break;

		case EVENT_CHANGE_SCREEN_SIZE:
			{	
				applyScreenSize(EventData->screenDimensions[0], EventData->screenDimensions[1], ScreenData);
			} break;

		case EVENT_CHANGE_SCREEN_SIZE_SCALE:
			{
				applyScreenSizeScale(EventData->screenDimensions[0], EventData->screenDimensions[1], &GameWorld->MainCamera, ScreenData);
			} break;

		case EVENT_ENABLE_FULLSCREEN:
			{
				applyEnableFullscreen(ScreenData);
			} break;

		case EVENT_DISABLE_FULLSCREEN:
			{
				applyDisableFullscreen(ScreenData, &GameWorld->MainCamera);
			} break;

		case EVENT_ENABLE_FULLSCREEN_SCALE:
			{
				applyEnableFullscreenScaled(ScreenData, &GameWorld->MainCamera);
			} break;

		case EVENT_STREAM_LEVEL_PARTITION:
			{
				if (!inputEvent->loadingAFile)
				{
					char fileName[MAX_LEN] = {0};
					snprintf(fileName, MAX_LEN, "Level%d_Part%d", GameWorld->level, EventData->newLevelID);

					EventData->loadedFile = openFile(fileName, LEVELDATA_ROOT, "--PARTITION_DATA--");

					if (EventData->loadedFile == NULL)
					{
						break;
					}

					inputEvent->loadingAFile = true;
				}

				int result = loadLevelDataChunk(GameWorld, EventData->loadedFile, 10);
				if (result != LEMON_SUCCESS)
				{
					fclose(EventData->loadedFile);
					EventData->loadedFile = NULL;
					inputEvent->loadingAFile = false;
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
		if (eventList[i].loadingAFile)
		{
			fclose(eventList[i].EventData.loadedFile);
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
		if (!eventList[i].loadingAFile && eventList[i].EventID != NO_EVENT)
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

	if (events[index].EventID != NO_EVENT && events[index].loadingAFile)
	{
		fclose(events[index].EventData.loadedFile);
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

	memset(eventPtr, 0, sizeof(GameEvent));
	eventPtr->loadingAFile = false;

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
	newEvent->EventData.newLevelID = level;

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
	newEvent->EventData.sceneID = scene;

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
	strcpy(newEvent->EventData.sceneName, name);

	putConsoleStringTS("Putting new event");

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
	newEvent->EventData.ObjectGoTo[0] = xPos;
	newEvent->EventData.ObjectGoTo[1] = yPos;

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
	newEvent->EventData.ObjectGoTo[0] = xPos;
	newEvent->EventData.ObjectGoTo[1] = yPos;
	newEvent->EventData.ObjectGoTo[2] = (float)input->index;

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
	newEvent->EventData.object = dest;

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
	newEvent->EventData.colourScale = brightness;

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
	newEvent->EventData.newLevelID = partID;

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
	newEvent->EventData.screenDimensions[0] = newWidth;
	newEvent->EventData.screenDimensions[1] = newHeight;

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
	newEvent->EventData.screenDimensions[0] = newWidth;
	newEvent->EventData.screenDimensions[1] = newHeight;

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


int applyScreenSize(int newWidth, int newHeight, RenderFrame *ScreenData)
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

	SDL_SetWindowSize(ScreenData->Window, newWidth, newHeight);
    SDL_SyncWindow(ScreenData->Window);
	SDL_GetWindowSize(ScreenData->Window, &ScreenData->screenWidth, &ScreenData->screenHeight);

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


int applyEnableFullscreen(RenderFrame *ScreenData)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	if (ScreenData->Fullscreen == true)
	{
		return ACTION_DISABLED;
	}
	SDL_SetWindowFullscreen(ScreenData->Window, true);
	SDL_SyncWindow(ScreenData->Window);

	SDL_GetWindowSize(ScreenData->Window, &ScreenData->screenWidth, &ScreenData->screenHeight);

	ScreenData->Fullscreen = true;
	ScreenData->Scaled = false;

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


int applyDisableFullscreen(RenderFrame *ScreenData, Camera *inputCamera)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	float ScaleX = 1.0;
	float ScaleY = 1.0;

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


const static char EventNames[EVENT_COUNT][64] = {
	[NO_EVENT] = "No Event",
 	[EVENT_SWITCH_LEVEL] = "Switch Level",
 	[EVENT_MOVE_OBJECT] = "Move Object",
 	[EVENT_MOVE_PLAYER] = "Move Player",
 	[EVENT_TELEPORT_PLAYER_TO_EXIT_DOOR] = "Teleport Player to exit door",
 	[EVENT_PLAY_CUTSCENE] = "Play Cutscene",
 	[EVENT_PLAY_CUTSCENE_FROM_FILE] = "Play Cutscene from file",
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
	for (int i = 0; i < EVENT_COUNT; i++)
	{
		if (strcmp(input, EventNames[i]) == 0)
		{
			return i;
		}
	}
	
	return UNDEFINED_EVENT;
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

int UpdateFlagObject(Object* inputObject, World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL || inputObject == NULL)
	{
		return MISSING_DATA;
	}

	PlayerData *Player = &GameWorld->Player;

	if (GameWorld->GameState != GAMEPLAY || GameWorld->Player.PlayerPtr == NULL)
	{
		return ACTION_DISABLED;
	}


	switch (getSubType(inputObject))
	{
		case CACHE_TRIGGER:
		if (detectCamera(inputObject, GameWorld->MainCamera))
		{
			PhysicsBox boundingBox;
			mapPhysicsBoxToCamera(&boundingBox, GameWorld->MainCamera);

			cacheObjects(GameWorld->ObjectList, boundingBox);
			inputObject->Action = 2;
		} break;


		case CUTSCENE_TRIGGER:
		if (detectPlayer(inputObject, Player))
		{
			playCutscene(inputObject->arg2, GameWorld);
			MarkObjectForDeletion(inputObject);
		} break;

		case LEVEL_TRIGGER:
		if (detectPlayer(inputObject, Player))
		{
			switchLevel(inputObject->arg2, GameWorld);
		}
		break;

		case LEVEL_TRIGGER_SEAMLESS:
		if (detectPlayer(inputObject, Player))
		{
			Player->PlayerPtr->reserved |= RFLAG_PRESERVE_ONCE;
			switchLevel(inputObject->arg2, GameWorld);
		}
		break;

		case SET_BACKGROUND_TRIGGER:
		if (detectPlayer(inputObject, Player))
		{
			switchBackGroundSprite(inputObject->arg2, inputObject->arg3, &GameWorld->WorldBackground);
		} break;


		case FALSE_CAMERA_BOUNDARY:
		{
			if (detectPlayer(inputObject, Player))
			{
				MarkObjectForDeletion(inputObject);
			}
		}

		case CAMERA_BOUNDARY:
		{
			Camera *cam = &GameWorld->MainCamera;
			PhysicsBox *box = inputObject->ObjectBox;
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
		if (detectPlayer(inputObject, Player))
		{
			streamPartition(inputObject->arg2, GameWorld);
		}
		break;

		case SWITCH_TO_NEW_PART_TRIGGER:
		if (detectPlayer(inputObject, Player))
		{
			switchToNewPartition(inputObject->arg2, GameWorld);
		} break;

		case SET_PLAYER_LAYER:
		if (detectPlayer(inputObject, Player))
		{
			setDisplayLayer(Player->PlayerPtr, getDisplayLayer(inputObject));
		} break;

		case PLAY_SOUND_TRIGGER:
		if (detectPlayer(inputObject, Player))
		{
			MarkObjectForDeletion(inputObject);
			//Entity *triggerEntity = getEntity(inputObject);
			//if (triggerEntity == NULL) { return TASK_FAILED; }
			//PlaySound(triggerEntity->SoundMeta.name, triggerEntity->SoundMeta.folder, triggerEntity->SoundMeta.channel, triggerEntity->SoundMeta.volume);
		} break;

		default:
		#ifndef LEMON_USE_CUSTOM_CALLBACKS
		MarkObjectForDeletion(inputObject);
		#endif

		#ifdef LEMON_USE_CUSTOM_CALLBACKS
		UpdateCustomLevelFlag(inputObject, GameWorld);
		#endif
		break;
	}


	return LEMON_SUCCESS;
}
