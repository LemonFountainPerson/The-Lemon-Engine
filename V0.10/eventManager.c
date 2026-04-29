#include "LemonEngine.h"


int StartGame(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState == CLOSE_GAME)
	{
		return ACTION_DISABLED;
	}

	// Logic for handle flow of menus and levels, etc can go here for game start
	loadLevel(GameWorld, 0);


	return LEMON_SUCCESS;
}


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


	if (keyboard[LMN_MENU_OPEN] == 1)
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
			}
			break;

		case EVENT_PLAY_CUTSCENE:
			{
				initialiseCutscene(EventData->sceneID, GameWorld);
			}
			break;

		case EVENT_PLAY_CUTSCENE_FROM_FILE:
			{
				initialiseCutsceneFromFile(EventData->sceneName, GameWorld);
			}
			break;

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
				PlaySound("DoorOpen", "Objects", OBJECT_SFX, 1.0);
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

	return newEvent;
}

GameEvent* Event_movePlayer(float xPos, float yPos, World *GameWorld)
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

GameEvent* Event_moveObject(Object *input, float xPos, float yPos, World *GameWorld)
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

GameEvent* Event_teleportPlayerToExitDoor(Object *dest, World *GameWorld)
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

GameEvent* Event_setScreenBrightness(float brightness, World *GameWorld)
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


const char* getEventName(GameEventID input)
{
	switch(input)
	{
	case EVENT_SWITCH_LEVEL:
		return "Switch Level";

	case EVENT_MOVE_OBJECT:
		return "Move Object";

	case EVENT_MOVE_PLAYER:
		return "Move Player";

	case EVENT_TELEPORT_PLAYER_TO_EXIT_DOOR:
		return "Teleport Player to exit door";

	case EVENT_PLAY_CUTSCENE:
		return "Play Cutscene";

	case EVENT_PLAY_CUTSCENE_FROM_FILE:
		return "Play Cutscene from file";

	case EVENT_ENABLE_FULLSCREEN:
		return "Enable fullscreen";

	case EVENT_DISABLE_FULLSCREEN:
		return "Disable fullscreen";

	case EVENT_ENABLE_FULLSCREEN_SCALE:
		return "Enable fullscreen scaled";

	case EVENT_CHANGE_SCREEN_SIZE:
		return "Change screen size";

	case EVENT_CHANGE_SCREEN_SIZE_SCALE:
		return "Change screen size scaled";

	case EVENT_STREAM_LEVEL_PARTITION:
		return "Stream Level Partition";

	case EVENT_DELETE_ENVIRONMENT_OBJECTS:
		return "Delete environment Objects";

	default:
		return "UnmappedEventID";
	}
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

	switch (getSubType(inputObject))
	{
		default:
		break;
	}


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


int UpdateFlagObject(Object* inputObject, PlayerData *Player, World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL || Player == NULL || Player->PlayerPtr == NULL || inputObject == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState != GAMEPLAY)
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


		case SET_CAMBOX_TRIGGER:
		if (detectPlayer(inputObject, Player))
		{

			MarkObjectForDeletion(inputObject);
		} break;

		case FALSE_CAMERA_BOUNDARY:
		{
			if (detectPlayer(inputObject, Player))
			{
				MarkObjectForDeletion(inputObject);
			}

			Camera *cam = &GameWorld->MainCamera;
			PhysicsBox *box = inputObject->ObjectBox;
			float halfWidth = (float)(cam->width / 2);
			float playerPrevX = Player->PlayerBox->prevXPos + (Player->PlayerBox->xSize >> 1);

			if (playerPrevX > box->xPos + box->xSize && cam->CameraX - halfWidth < box->xPos + box->xSize)
			{
				cam->CameraX = box->xPos + box->xSize + halfWidth;
			}

			if (playerPrevX < box->xPos && cam->CameraX + halfWidth > box->xPos)
			{
				cam->CameraX = box->xPos - halfWidth;
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

		default:
		MarkObjectForDeletion(inputObject);
		break;
	}


	return LEMON_SUCCESS;
}

// command console
void executeCommand(char inputSource[], World *GameWorld)
{
	char input[USER_INPUT_MAX_LEN] = {0};
	strcpy(input, inputSource);
	memset(inputSource, 0, USER_INPUT_MAX_LEN);
	DebugSettings.userInputIndex = 0;
	DebugSettings.cursorXPos = 0.0;
	DebugSettings.scrollVal = 0;

	putConsoleString("> %s", input);
	addInputHistory(input, &DebugSettings.userInputHistory);

	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return;
	}

	char arg[USER_INPUT_MAX_LEN] = {0};
	DebugSettings.argIndex = 0;

	parseArgument(input, arg);
	stringToLower(arg);

	// crashing when typing many commands?
	for (int i = 0; i < MAX_CONSOLE_COMMANDS; i++)
	{
		ConsoleCommandFunction command = DebugSettings.commands[i].function;
		
		if (command != NULL && strcmp(DebugSettings.commands[i].name, arg) == 0)
		{
			if (command(input, GameWorld) != LEMON_SUCCESS)
			{
				putConsoleString("command unrecognised");
			}
		
			return;	
		}
	}

	putConsoleString("'%s' command unrecognised", arg);
	return;
}


void parseArgument(const char input[USER_INPUT_MAX_LEN], char argDest[USER_INPUT_MAX_LEN])
{
	while (DebugSettings.argIndex < USER_INPUT_MAX_LEN - 1 && input[DebugSettings.argIndex] < 33)
	{
		DebugSettings.argIndex++;
	}

	bool enclosedCommand = false;
	if (input[DebugSettings.argIndex] =='"')
	{
		enclosedCommand = true;
		DebugSettings.argIndex++;
	}

	int i = 0;
	while (DebugSettings.argIndex < USER_INPUT_MAX_LEN - 1 && input[DebugSettings.argIndex] != '\0')
	{
		argDest[i] = input[DebugSettings.argIndex];
		DebugSettings.argIndex++;

		i++;

		if (input[DebugSettings.argIndex] == '"' || (!enclosedCommand && input[DebugSettings.argIndex] < 33) )
		{
			DebugSettings.argIndex++;
			break;
		}
	}

	argDest[i] = 0;

	return;
}

int parseArgumentAsInt(const char input[USER_INPUT_MAX_LEN])
{
	int prevPos = DebugSettings.argIndex;

	char buffer[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, buffer);

	if (!inRange(buffer[0], '0', '9'))
	{
		DebugSettings.argIndex = prevPos;
		return 0;
	}

	return atoi(buffer);
}

bool nextArgumentIsNumber(const char input[USER_INPUT_MAX_LEN])
{
	int prevPos = DebugSettings.argIndex;

	char buffer[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, buffer);

	DebugSettings.argIndex = prevPos;

	return inRange(buffer[0], '0', '9');
}

float parseArgumentAsFloat(const char input[USER_INPUT_MAX_LEN])
{
	int prevPos = DebugSettings.argIndex;

	char buffer[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, buffer);

	if (!inRange(buffer[0], '0', '9'))
	{
		DebugSettings.argIndex = prevPos;
		return 0;
	}

	return atof(buffer);
}

void parseArgumentFlag(char input[USER_INPUT_MAX_LEN], char argDest[USER_INPUT_MAX_LEN])
{
	int prevPos = DebugSettings.argIndex;

	parseArgument(input, argDest);

	while (DebugSettings.argIndex < USER_INPUT_MAX_LEN && input[DebugSettings.argIndex] != '-')
	{
		DebugSettings.argIndex++;
	}

	int i = 0;
	while (DebugSettings.argIndex < USER_INPUT_MAX_LEN && input[DebugSettings.argIndex] > 32)
	{
		argDest[i] = input[DebugSettings.argIndex];
		input[DebugSettings.argIndex] = ' ';
		DebugSettings.argIndex++;
		i++;
	}

	argDest[USER_INPUT_MAX_LEN - 1] = '\0';

	DebugSettings.argIndex = prevPos;

	stringToLower(argDest);

	return;
}

bool parseArgumentAsBoolean(const char input[USER_INPUT_MAX_LEN])
{
	char buffer[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, buffer);

	if (strcmp(buffer, "true") == 0 || buffer[0] == '1')
	{
		return true;
	}
	else
	{
		return false;
	}
}



Object* parseArgumentToFindObject(const char input[USER_INPUT_MAX_LEN], ObjectController *ObjectList)
{
	char buffer[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, buffer);

	if (inRange(buffer[0], '0', '9'))
	{
		int index = atoi(buffer);

		if (index >= MAX_OBJECTS)
		{
			putConsoleString("'%d' index out of bounds. Valid range 0 <-> %d", index, MAX_OBJECTS - 1);
			return NULL;
		}

		return &ObjectList->objectComponents.Objects[index];
	}
	else
	{
		Object *object = FindObject(buffer, ObjectList);
		if (object == NULL)
		{
			putConsoleString("Cannot find '%s' from objectlist.", buffer);
		}

		return object;
	}
}

// macro used to create new command; name and function are derived from cName
#define NEWCOMMAND(cName, cHelp, cFormat) 	strcpy(commandList[i].name, #cName);\
											stringToLower(commandList[i].name);\
											strcpy(commandList[i].helpString, cHelp);\
											strcpy(commandList[i].formatString, cFormat);\
											commandList[i].function = &ConsoleCommand_##cName; i++;

void createConsoleCommands(ConsoleCommand commandList[MAX_CONSOLE_COMMANDS])
{
	memset(commandList, 0, MAX_CONSOLE_COMMANDS * sizeof(ConsoleCommand));

	int i = 0;

	NEWCOMMAND(Version, "check engine version information", "version");

	NEWCOMMAND(Quit, "quit the game", "quit");

	NEWCOMMAND(Restart, "restart the game", "restart");

	NEWCOMMAND(Tick, "check current tick number", "tick");
	
	NEWCOMMAND(Fullscreen, "toggle fullscreen (equivalent to calling 'event enablefullscreen' or 'event disablefullscreen')", "fullscreen");

	NEWCOMMAND(Show, "show when a new event/spriteset/etc. is created", "show [events/spritesets/sceneactions/errors/...] [true/false]");

	NEWCOMMAND(Vsync, "toggle screen vertical sync", "vsync [true/false]");

	NEWCOMMAND(Debug, "set the debug mode", "debug [0/1/2/...]");

	NEWCOMMAND(Fps, "show the engine's current frames per second", "fps");

	NEWCOMMAND(Draw, "toggle whether a certain element is drawn or not", 
		"draw [sprites/hitboxes/backgrounds/camviews/hud/particles] [true/false]");

	NEWCOMMAND(HitboxThickness, "set the hitbox thickness in pixels for when hitboxes are being drawn", "hitboxthickness [THICKNESS]");

	NEWCOMMAND(List, "lists current instances of requested data", "list [objects/text/fonts/spritesets/...]");

	NEWCOMMAND(AddObject, "create a new object, equivalent to 'object_add [OBJECTID] ...'", 
		"addobject [OBJECTID] [XPOS] [YPOS] [ARG1] [ARG2] [ARG3] [ARG4] [ARG5]");

	NEWCOMMAND(Object, "perform various actions on a specific object, identified by their index number or name", 
		"object [INDEX/NAME] [info/setpos/setname/add/delete/...]");

	NEWCOMMAND(UsedMemory, "check how much memory in kilobytes is currently being used for specific data", 
		"usedmemory [objects/animations/text/...]");

	NEWCOMMAND(BackGround, "change the background sprite", "background [backGroundID]");

	NEWCOMMAND(Level, "switch to a new level", "level [LEVELID]");

	NEWCOMMAND(Event, "trigger a specific GameEvent", "event [setscreensize/enablefullscreen/switchlevel/...]");

	NEWCOMMAND(CamView, "add or modify camera views in the gameworld", "camview [add/clear/attach/...]");

	NEWCOMMAND(Sound, "play or modify sounds", "sound [play/...]");

	NEWCOMMAND(Cutscene, "play or manipulate cutscenes", "cutscene [play/start/...]");

	NEWCOMMAND(Load, "load some data type into the engine to be used later", "load [spriteset/audio/...]");

	NEWCOMMAND(DebugText, "show info on debug text or manipulate them", "DebugText [info/...]");

	NEWCOMMAND(Pause, "toggle engine pause state", "pause");

	NEWCOMMAND(SetPos, "set the player X and Y position", "setPos [x] [y]");

	NEWCOMMAND(SetCamPos, "set the main camera X and Y position", "setcampos [x] [y]");

	NEWCOMMAND(SetCamZoom, "set the main camera X and Y zoom, default: 1.0, 1.0", "setcamzoom [xZoom] [yZoom]");

	NEWCOMMAND(SetTickRate, "set a new tickrate (GameTicks per second)", "settickrate [newTickRate]");

	NEWCOMMAND(Save, "save the game to a slot", "save [saveID]");

	NEWCOMMAND(LoadSave, "load a save file", "loadsave [saveID]");

	NEWCOMMAND(SaveSettings, "save current settings to a slot", "savesettings [settingsID]");

	NEWCOMMAND(LoadSettings, "load a settings file", "loadsettings [settingsID]");

	NEWCOMMAND(AddGameFlag, "add a new gameflag with a unique identifying name", "addgameflag [name] [startValue]");

	NEWCOMMAND(SetGameFlag, "set a gameflag to a new value", "setgameflag [NAME/INDEX] [newValue]");

	NEWCOMMAND(CheckGameFlag, "check the current value of a GameFlag", "checkgameflag [NAME/INDEX]");

	NEWCOMMAND(Help, "see information on a specific command or just type 'help' to see all help info", "help [command]");

	NEWCOMMAND(DoABarrelRoll, "does a barrel roll.", "barrelroll");

	NEWCOMMAND(Noclip, "toggles noclip.", "noclip");


	if (DEBUG_MODE)
	{
		putConsoleString("Loaded %d commands.", i);
	}
	
	return;
}

int ConsoleCommand_Version(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	putConsoleString("\n%s\n%s\nFile Reader: %s", LEMON_ENGINE_INFO, LEMON_VERSION, FILE_READER_VERSION);
	if (DEBUG_MODE)
	{
		putConsoleString("Running in Debug mode");
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_Quit(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	GameWorld->GameState = CLOSE_GAME;

	return LEMON_SUCCESS;
}

int ConsoleCommand_Restart(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	destroyWorld(GameWorld);

	initialiseWorld(GameWorld);

	StartGame(GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Tick(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	putConsoleStringTS("Tickrate: %d", EngineSettings.GameTicksPerSecond);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Fullscreen(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	if (ScreenData.Fullscreen)
	{
		disableFullscreen(GameWorld);
	}
	else
	{
		enableFullscreen(GameWorld);
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_Show(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, arg);
	stringToLower(arg);

	if (strcmp(arg, "events") == 0)
	{
		DebugSettings.showEvents = parseArgumentAsBoolean(input);
	}
	else if (strcmp(arg, "sceneactions") == 0)
	{
		DebugSettings.showSceneActions = parseArgumentAsBoolean(input);
	}
	else if (strcmp(arg, "spritesets") == 0)
	{
		DebugSettings.showSpriteset = parseArgumentAsBoolean(input);
	}
	else if (strcmp(arg, "errors") == 0)
	{
		DebugSettings.showErrors = parseArgumentAsBoolean(input);
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_Vsync(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	setVsync(parseArgumentAsBoolean(input));

	return LEMON_SUCCESS;
}

int ConsoleCommand_Debug(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	DebugSettings.DebugTextDisplayMode = parseArgumentAsInt(input);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Fps(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	DebugSettings.FPSCounter = parseArgumentAsBoolean(input);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Draw(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, arg);

	if (strcmp(arg, "hitboxes") == 0 || strcmp(arg, "hitbox") == 0)
	{
		RenderSettings.drawHitboxes = parseArgumentAsBoolean(input);
	}
	else if (strcmp(arg, "sprites") == 0)
	{
		RenderSettings.drawSprites = parseArgumentAsBoolean(input);
	}
	else if (strcmp(arg, "background") == 0)
	{
		RenderSettings.drawBackGround = parseArgumentAsBoolean(input);
	}
	else if (strcmp(arg, "camviews") == 0)
	{
		RenderSettings.drawCamViews = parseArgumentAsBoolean(input);
	}
	else if (strcmp(arg, "hud") == 0)
	{
		RenderSettings.drawHUD = parseArgumentAsBoolean(input);
	}
	else if (strcmp(arg, "particles") == 0)
	{
		RenderSettings.drawParticles = parseArgumentAsBoolean(input);
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_HitboxThickness(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	RenderSettings.HitboxOutlineThickness = parseArgumentAsInt(input);

	return LEMON_SUCCESS;
}


int ConsoleCommand_UsedMemory(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	ObjectController *ObjectList = GameWorld->ObjectList;
	char arg[USER_INPUT_MAX_LEN] = {0};

	parseArgument(input, arg);
	double total = 0.0;

	if (strcmp(arg, "text") == 0)
	{
		total = (double)sizeof(TextSettings) / 1000.0;
	}
	else if (strcmp(arg, "debug") == 0)
	{
		total = (double)sizeof(DebugSettings) / 1000.0;
	}
	else if (strcmp(arg, "world") == 0 || strcmp(arg, "gameworld") == 0)
	{
		total = (double)sizeof(World) / 1000.0;
	}
	else if (strcmp(arg, "objlist") == 0 || strcmp(arg, "objectlist") == 0)
	{
		total = (double)sizeof(ObjectController) / 1000.0;
	}
	else if (strcmp(arg, "components") == 0 || strcmp(arg, "comps") == 0)
	{
		total = (double)sizeof(ComponentData) / 1000.0;
	}
	else if (strcmp(arg, "objects") == 0)
	{
		total = (double)sizeof(ObjectList->objectComponents.Objects) / 1000.0;
	}
	else if (strcmp(arg, "displays") == 0)
	{
		total = (double)sizeof(ObjectList->objectComponents.Displays) / 1000.0;
	}
	else if (strcmp(arg, "physboxes") == 0 || strcmp(arg, "physicsboxes") == 0)
	{
		total = (double)sizeof(ObjectList->objectComponents.PhysicsBoxes) / 1000.0;
	}
	else if (strcmp(arg, "animations") == 0)
	{
		SpriteSet *set = ObjectList->spriteSets.start;
		int setCount = 0;
		int animCount = 0;
		int frameCount = 0;
		int spriteCount = 0;
		double textureData = 0;

		float width = 0;
		float height = 0;

		if (EngineSettings.DefaultTexture != NULL)
		{
			spriteCount++;
			SDL_GetTextureSize(EngineSettings.DefaultTexture->texture, &width, &height);
			textureData += width * height;
		}

		while (set != NULL)
		{
			Animation *anim = set->Animations;
			while (anim != NULL)
			{
				AnimationFrame *frame = anim->animationData;

				while (frame != NULL)
				{
					frame = frame->nextFrame;
					frameCount++;
				}

				anim = anim->nextAnimation;
				animCount++;
			}

			Sprite *sprite = set->firstSprite;
			while (sprite != NULL)
			{
				SDL_GetTextureSize(sprite->texture, &width, &height);
				textureData += width * height;
				sprite = sprite->nextSprite;
				spriteCount++;
			}


			set = set->nextSet;
			setCount++;
		}

		putConsoleString("Spritesets: %d  Animations: %d  AnimationFrames: %d  Sprites: %d \nTexture data estimate: %.2lfkb", 
			setCount, animCount, frameCount, spriteCount, textureData / 1000.0);
		total = (double)((sizeof(SpriteSet) * setCount) + (sizeof(Animation) * animCount) + (sizeof(AnimationFrame) * frameCount) + (sizeof(Sprite) * spriteCount)) + textureData;
		total /= 1000.0;
	}
	else
	{
		return INVALID_DATA;
	}
	
	putConsoleString("Total used: %.2lfkb", total);

	return LEMON_SUCCESS;
}

int ConsoleCommand_AddObject(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int ID = parseArgumentAsInt(input);
	int args[7] = {0};
	for (int i = 0; i < 7; i++)
	{
		args[i] = parseArgumentAsInt(input);
	}

	AddObject(GameWorld, ID, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Object(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	ObjectController *ObjectList = GameWorld->ObjectList;
	char arg[USER_INPUT_MAX_LEN] = {0};

	parseArgument(input, arg);

	if (strcmp(arg, "add") == 0)
	{
		return ConsoleCommand_AddObject(input, GameWorld);
	}

	Object *object = parseArgumentToFindObject(input, ObjectList);
	if (object == NULL)
	{
		return INVALID_DATA;
	}

	char flag[USER_INPUT_MAX_LEN] = {0};
	parseArgumentFlag(input, flag);

	if (strcmp(arg, "info") == 0)
	{
		displayObjectInfoConsole(object);
	}
	else if (strcmp(arg, "setpos") == 0)
	{
		float x = parseArgumentAsFloat(input);
		float y = parseArgumentAsFloat(input);

		if (strcmp(flag, "-snaptogrid") == 0 || strcmp(flag, "-grid") == 0)
		{
			snapPositionToTileGrid(object, x, y);
		}
		else
		{
			GoTo(object, x, y);
		}
	}
	else if (strcmp(arg, "setsize") == 0)
	{
		float width = parseArgumentAsInt(input);
		float height = parseArgumentAsInt(input);

		setSize(object, width, height);
	}
	else if (strcmp(arg, "changexsize") == 0)
	{
		float val = parseArgumentAsInt(input);

		ChangeXSizeBy(val, object, ObjectList);
	}
	else if (strcmp(arg, "changeysize") == 0)
	{
		float val = parseArgumentAsInt(input);

		ChangeYSizeBy(val, object, ObjectList);
	}
	else if (strcmp(arg, "setname") == 0)
	{
		parseArgument(input, arg);
		setObjectName(object, arg);
	}
	else if (strcmp(arg, "polygon") == 0)
	{
		Polygon *poly = getPolygon(object);

		if (poly == NULL)
		{
			putConsoleString("'%s' has no polygon component", object->name);
			return LEMON_SUCCESS;
		}

		if (poly->quad)
		{
			putConsoleString("Num of vertices: %d\nQuad polygon: Yes", poly->vertices);
		}
		else
		{
			putConsoleString("Num of vertices: %d\nQuad polygon: No", poly->vertices);
		}
	}
	else if (strcmp(arg, "physics") == 0)
	{
		PhysicsComponent *phys = getPhysicsComponent(object);

		if (phys == NULL)
		{
			putConsoleString("'%s' has no physics component", object->name);
			return LEMON_SUCCESS;
		}

		if (phys->gravity)
		{
			putConsoleString("Gravity: enabled");
		}
		else
		{
			putConsoleString("Gravity: disabled");
		}
	}
	else if (strcmp(arg, "timer") == 0)
	{
		Timer *timer = getTimer(object);

		if (timer == NULL)
		{
			putConsoleString("'%s' has no timer component", object->name);
			return LEMON_SUCCESS;
		}

		putConsoleString("Tick Started: %lld \nTimer length: %d \nTimer paused: %d", 
			timer->startTick, timer->timerLength, timer->pause);
	}
	else if (strcmp(arg, "removecomponents") == 0)
	{
		removeComponents(object, ObjectList);
	}
	else if (strcmp(arg, "delete") == 0)
	{
		object->State = TO_BE_DELETED;
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

void displayObjectInfoConsole(Object *input)
{
	if (input == NULL)
	{
		return;
	}

	putConsoleString("\nObject Information: \nName: '%s'\nID: %d (%s)", input->name, input->ObjectID, getObjectIDName(input->ObjectID));
	putConsoleString("Index: %d \nCurrent State: %d (%s)", input->index, input->State, getObjectStateName(input->State));

	if (input->Parent == NULL)
	{
		putConsoleString("Parent: \n    None");
	}
	else
	{
		Object *parent = input->Parent;
		putConsoleString("Parent: \n    Name: %s \n    ID: %d (%s)", parent->name, parent->ObjectID, getObjectIDName(parent->ObjectID));
		putConsoleString("    Index: %d \n    Current State: %d (%s)", parent->index, parent->State, getObjectStateName(parent->State));
	}

	Layer objLayer = getDisplayLayer(input);
	putConsoleString("XPos: %f  YPos: %f \nLayer: %d (%s)", input->ObjectBox->xPos, input->ObjectBox->yPos, objLayer, getLayerName(objLayer));
}

int ConsoleCommand_BackGround(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int ID = parseArgumentAsInt(input);
	int set = parseArgumentAsInt(input);

	switchBackGroundSprite(ID, set, &GameWorld->WorldBackground);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Level(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int level = parseArgumentAsInt(input);
	switchLevel(level, GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Event(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, arg);

	if (strcmp(arg, "changescreensize") == 0 || strcmp(arg, "setscreensize") == 0)
	{
		int width = parseArgumentAsInt(input);
		int height = parseArgumentAsInt(input);
		changeScreenSize(width, height, GameWorld);
	}
	else if (strcmp(arg, "changescreensizescaled") == 0)
	{
		int width = parseArgumentAsInt(input);
		int height = parseArgumentAsInt(input);
		changeScreenSizeScaled(width, height, GameWorld);
	}
	else if (strcmp(arg, "disablefullscreen") == 0 || strcmp(arg, "nofullscreen") == 0)
	{
		disableFullscreen(GameWorld);
	}
	else if (strcmp(arg, "enablefullscreen") == 0 || strcmp(arg, "fullscreen") == 0)
	{
		enableFullscreen(GameWorld);
	}
	else if (strcmp(arg, "enablefullscreenscaled") == 0 || strcmp(arg, "fullscreenscaled") == 0)
	{
		enableFullscreenScaled(GameWorld);
	}
	else if (strcmp(arg, "switchlevel") == 0)
	{
		int level = parseArgumentAsInt(input);
		switchLevel(level, GameWorld);
	}
	else if (strcmp(arg, "playcutscene") == 0)
	{
		int scene = parseArgumentAsInt(input);
		playCutscene(scene, GameWorld);
	}
	else if (strcmp(arg, "playcutscenefromfile") == 0)
	{
		parseArgument(input, arg);
		playCutsceneFromFile(arg, GameWorld);
	}
	else if (strcmp(arg, "deleteenv") == 0 || strcmp(arg, "deleteenvironment") == 0)
	{
		scheduleEnvironmentDeletion(GameWorld);
	}
	else if (strcmp(arg, "streampart") == 0 || strcmp(arg, "streampartition") == 0)
	{
		int partID = parseArgumentAsInt(input);
		streamPartition(partID, GameWorld);
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_List(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	ObjectController *ObjectList = GameWorld->ObjectList;
	char arg[USER_INPUT_MAX_LEN] = {0};

	parseArgument(input, arg);

	if (strcmp(arg, "object") == 0 || strcmp(arg, "objects") == 0)
	{
		Object *cursor = ObjectList->firstObject;

		while (cursor != NULL)
		{
			putConsoleString("%s - Index: %d  ID: %d (%s)  State: %d (%s)", 
				cursor->name, cursor->index, cursor->ObjectID, getObjectIDName(cursor->ObjectID), cursor->State, getObjectStateName(cursor->State));
			cursor = cursor->nextObject;
		}
	}
	else if (strcmp(arg, "text") == 0)
	{
		printTextsinfo(&TextSettings.TextList, "TextList");
	}
	else if (strcmp(arg, "fonts") == 0)
	{
		FontList *list = &TextSettings.FontList;

		for (int i = 0; i < MAX_LOADED_FONTS; i++)
		{
			if (list->font[i] != NULL)
			{
				putConsoleString("Slot %d '%s'  ", i, list->name[i]);
			}
			else
			{
				putConsoleString("Slot %d (Empty)", i);
			}
		}
	}
	else if (strcmp(arg, "debugtext") == 0)
	{
		printTextsinfo(&TextSettings.DebugTexts, "Debug Textlist");
	}
	else if (strcmp(arg, "spritesets") == 0)
	{
		SpriteSet *set = ObjectList->spriteSets.start;
		putConsoleString("Spritesets loaded:");

		while (set != NULL)
		{
			putConsoleString("Spriteset: %d (%s)", set->setID, getObjectIDName(set->setID));
			set = set->nextSet;
		}
	}
	else if (strcmp(arg, "camviews") == 0 || strcmp(arg, "cameraviews") == 0)
	{
		printCameraViewInfo(GameWorld->views);
	}
	else if (strcmp(arg, "layers") == 0)
	{
		for (int i = BACKGROUND; i < LAYER_COUNT; i++)
		{
			putConsoleString("%d: %s", i, getLayerName(i));
		}
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_CamView(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, arg);
		
	if (strcmp(arg, "clear") == 0)
	{
		removeAllCameraViews(GameWorld);
	}
	else if (strcmp(arg, "add") == 0)
	{
		char flag[USER_INPUT_MAX_LEN] = {0};
		parseArgumentFlag(input, flag);

		float camX = parseArgumentAsFloat(input);
		float camY = parseArgumentAsFloat(input);				

		float screenX = parseArgumentAsFloat(input);
		float screenY = parseArgumentAsFloat(input);
		float width = parseArgumentAsFloat(input);
		float height = parseArgumentAsFloat(input);
		Layer layer = parseArgumentAsInt(input);

		if (strcmp(flag, "-main") == 0)
		{
			addMainCameraView(screenX, screenY, width, height, layer, GameWorld);
		}
		else
		{
			addCameraView(camX, camY, GameWorld->MainCamera.width, GameWorld->MainCamera.height, screenX, screenY, width, height, layer, GameWorld);
		}
	}
	else if (strcmp(arg, "attach") == 0)
	{
		int index = parseArgumentAsInt(input);
		Object *attach = parseArgumentToFindObject(input, GameWorld->ObjectList);
		
		if (attach == NULL)
		{
			return INVALID_DATA;
		}

		attachCameraViewToObject(getCameraView(GameWorld, index), attach);
	}
	else if (strcmp(arg, "setrefresh") == 0)
	{
		int index = parseArgumentAsInt(input);
		CameraView *camView = getCameraView(GameWorld, index);

		if (camView == NULL)
		{
			return INVALID_DATA;
		}

		camView->ticksUntilRefresh = parseArgumentAsInt(input);
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_Sound(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, arg);

	if (strcmp(arg, "play") == 0)
	{
		char name[USER_INPUT_MAX_LEN] = {0};
		char folder[USER_INPUT_MAX_LEN] = {0};
		parseArgument(input, name);
		parseArgument(input, folder);
		float volume = parseArgumentAsFloat(input);
		ChannelName channel = parseArgumentAsInt(input);

		PlaySound(name, strcmp(folder, "NULL") ? folder : NULL, channel, volume);
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_Cutscene(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, arg);

	if (strcmp(arg, "play") == 0 || strcmp(arg, "start") == 0)
	{
		parseArgument(input, arg);

		if (inRange(arg[0], '0', '9'))
		{
			initialiseCutscene(atoi(arg), GameWorld);
		}
		else
		{
			initialiseCutsceneFromFile(arg, GameWorld);
		}
	}
	else if (strcmp(arg, "stop") == 0)
	{
		if (GameWorld->CurrentCutscene != NO_CUTSCENE)
		{
			GameWorld->CurrentCutscene = END_CUTSCENE;
			if (GameWorld->TextQueue != NULL)
			{
				clearTextQueue(GameWorld);
			}
		}
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_Load(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, arg);

	if (strcmp(arg, "spriteset") == 0)
	{
		int ID = parseArgumentAsInt(input);

		loadSpriteSet(GameWorld->ObjectList, ID);
	}
	else if (strcmp(arg, "audio") == 0)
	{
		parseArgument(input, arg);
		char folder[USER_INPUT_MAX_LEN] = {0};

		parseArgument(input, folder);

		loadAudio(arg, folder);
	}
	else
	{
		return INVALID_DATA;
	}

		return LEMON_SUCCESS;
}

int ConsoleCommand_DebugText(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, arg);

	if (strcmp(arg, "info") == 0)
	{
		printTextsinfo(&TextSettings.DebugTexts, "TextList");
	}
	else if (strcmp(arg, "clear") == 0)
	{
		RemoveAllTexts(&TextSettings.DebugTexts);
	}
	else 
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_Pause(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	DebugSettings.PauseEngine = (DebugSettings.PauseEngine + 1) % 2;

	if (DebugSettings.PauseEngine == 1)
	{
		putConsoleStringTS("Engine is now paused.");
	}
	else
	{
		putConsoleStringTS("Engine is now unpaused.");
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_SetPos(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	float x = parseArgumentAsFloat(input);
	float y = parseArgumentAsFloat(input);

	GoTo(GameWorld->Player.PlayerPtr, x, y);

	return LEMON_SUCCESS;
}

int ConsoleCommand_SetCamPos(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	float x = parseArgumentAsFloat(input);
	float y = parseArgumentAsFloat(input);

	setCameraPos(&GameWorld->MainCamera, x, y);

	return LEMON_SUCCESS;
}

int ConsoleCommand_SetCamZoom(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	if (input[DebugSettings.argIndex] == 0)
	{
		GameWorld->MainCamera.zoomX = 1.0;
		GameWorld->MainCamera.zoomY = 1.0;
		return LEMON_SUCCESS;
	}

	float zoomX =  parseArgumentAsFloat(input);
	float zoomY =  parseArgumentAsFloat(input);
	GameWorld->MainCamera.zoomX = zoomX;
	GameWorld->MainCamera.zoomY = zoomY;

	return LEMON_SUCCESS;
}

int ConsoleCommand_SetTickRate(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int rate = parseArgumentAsInt(input);
	setTickRate(rate);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Save(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int save = parseArgumentAsInt(input);
	saveGame(save, GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_SaveSettings(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int save = parseArgumentAsInt(input);
	saveSettings(save, GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_LoadSave(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int save = parseArgumentAsInt(input);
	loadSave(save, GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_LoadSettings(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	loadSettings(parseArgumentAsInt(input), GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_AddGameFlag(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char name[USER_INPUT_MAX_LEN] = {0};

	parseArgument(input, name);

	int startVal = parseArgumentAsInt(input);

	addGameFlag(name, startVal);

	return LEMON_SUCCESS;
}

int ConsoleCommand_SetGameFlag(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int index;

	if (nextArgumentIsNumber(input))
	{
		index = parseArgumentAsInt(input);
	}
	else
	{
		char name[USER_INPUT_MAX_LEN] = {0};
		parseArgument(input, name);
		index = getGameFlag(name);
	}

	int newValue = parseArgumentAsInt(input);

	if (index < 0 || index >= GAME_FLAG_COUNT)
	{
		putConsoleString("GameFlag does not exist");
		return LEMON_SUCCESS;
	}

	GameFlags[index].value = newValue;

	return LEMON_SUCCESS;
}

int ConsoleCommand_CheckGameFlag(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int index;

	if (nextArgumentIsNumber(input))
	{
		index = parseArgumentAsInt(input);
	}
	else
	{
		char name[USER_INPUT_MAX_LEN] = {0};
		parseArgument(input, name);
		index = getGameFlag(name);
	}

	if (index < 0 || index >= GAME_FLAG_COUNT)
	{
		putConsoleString("GameFlag does not exist");
	}
	else
	{
		putConsoleString("(%d)\"%s\": %d", index, GameFlags[index].name, GameFlags[index].value);
	}

	return LEMON_SUCCESS;
}


int ConsoleCommand_Help(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	parseArgument(input, arg);

	ConsoleCommand *commands = DebugSettings.commands;

	// find matching command to print help string for
	for (int i = 0; i < MAX_CONSOLE_COMMANDS; i++)
	{
		if (commands[i].name[0] == '\0')
		{
			return LEMON_SUCCESS;
		}

		// if arg is empty; print all help strings
		if (strcmp(commands[i].name, arg) == 0 || arg[0] == '\0')
		{
			putConsoleString("%s - %s", commands[i].formatString, commands[i].helpString);

			if (arg[0] != '\0')
			{
				return LEMON_SUCCESS;
			}
		}
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_DoABarrelRoll(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	Object *player = GameWorld->Player.PlayerPtr;
	if (player == NULL)
	{
		return LEMON_SUCCESS;
	}

	if (player->State == PAUSE_STATE)
	{
		if (objectPlayingThisAnimation(player, "DoABarrelRoll"))
		{
			player->State = DEFAULT_STATE;
			stopAnimation(player->ObjectDisplay);
		}
	}
	else if (player->State == DEFAULT_STATE)
	{
		player->State = PAUSE_STATE;
		PlayAnimation("BarrelRoll", 0, player->ObjectDisplay);
	}
	
	return LEMON_SUCCESS;
}

int ConsoleCommand_Noclip(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	DebugSettings.noclip = !DebugSettings.noclip;

	if (DebugSettings.noclip == true)
	{
		putConsoleString("NoClip is on");
	}
	else
	{
		putConsoleString("NoClip is off");

		if (GameWorld->Player.PlayerBox != NULL)
		{
			GameWorld->Player.PlayerBox->solid = BODY;
		}
	}
	
	return LEMON_SUCCESS;
}


static const float consoleWidth = 1024.0;
static const float consoleHeight = 600.0;

void updateTypedCommand(SDL_Window *window, World *GameWorld)
{
	if (DebugSettings.TypingInConsole == false)
	{
		if (keyboard[LMN_GRAVE] == 1)
		{
			AcknowledgeButton(LMN_GRAVE);
			startTypedCommand(window);
		}

		return;
	}

	if (buttonPressed(MOUSE_LEFT))
	{
		if (MouseInput.xPos > DebugSettings.consoleXPos && MouseInput.xPos < DebugSettings.consoleXPos + consoleWidth && MouseInput.yPos > DebugSettings.consoleYPos && MouseInput.yPos < DebugSettings.consoleYPos + consoleHeight)
		{
			DebugSettings.consoleFocus = true;
		}
		else
		{
			DebugSettings.consoleFocus = false;
		}
	}

	if (buttonPressed(LMN_ENTER))
	{
		executeCommand(DebugSettings.userInputString, GameWorld);
	}

	if (buttonPressed(LMN_GRAVE))
	{
		SDL_StopTextInput(window);
		DebugSettings.TypingInConsole = false;
	}

	if (buttonPressed(LMN_BACKSPACE) && DebugSettings.userInputIndex > 0)
	{
		DebugSettings.userInputIndex--;				

		char buffer[USER_INPUT_MAX_LEN] = {0};
		if (DebugSettings.userInputIndex < USER_INPUT_MAX_LEN - 1)
		{
			strcpy(buffer, DebugSettings.userInputString + DebugSettings.userInputIndex + 1);
		}
			
		DebugSettings.userInputString[DebugSettings.userInputIndex] = 0;
		strcat(DebugSettings.userInputString, buffer);

		DebugSettings.cursorXPos = getCursorPos();
	}

	if (buttonPressed(LMN_UPARROW) || MouseInput.wheelYDir > 0)
	{
		if (DebugSettings.consoleFocus)
		{
			DebugSettings.scrollVal = clamp(DebugSettings.scrollVal + 1, 0, USER_INPUT_HISTORY_LEN);
		}
		else
		{
			char *next = getNextInputHistory(&DebugSettings.userInputHistory);

			if (next != NULL && next[0] != '\0')
			{
				strcpy(DebugSettings.userInputString, next);
				DebugSettings.userInputIndex = strlen(DebugSettings.userInputString);

				DebugSettings.cursorXPos = getCursorPos();
			}
		}
	}

	if (buttonPressed(LMN_DOWNARROW) || MouseInput.wheelYDir < 0)
	{
		if (DebugSettings.consoleFocus)
		{
			DebugSettings.scrollVal = clamp(DebugSettings.scrollVal - 1, 0, USER_INPUT_HISTORY_LEN);
		}
		else
		{
			char *prev = getPreviousInputHistory(&DebugSettings.userInputHistory);

			if (prev != NULL && prev[0] != '\0')
			{
				strcpy(DebugSettings.userInputString, prev);
				DebugSettings.userInputIndex = strlen(DebugSettings.userInputString);

				DebugSettings.cursorXPos = getCursorPos();
			}
		}
	}

	if (buttonPressed(LMN_LEFTARROW))
	{
		DebugSettings.userInputIndex = clamp(DebugSettings.userInputIndex - 1, 0, USER_INPUT_MAX_LEN);
		DebugSettings.cursorXPos = getCursorPos();
	}

	if (buttonPressed(LMN_RIGHTARROW))
	{
		DebugSettings.userInputIndex = clamp(DebugSettings.userInputIndex + 1, 0, strlen(DebugSettings.userInputString));
		DebugSettings.cursorXPos = getCursorPos();
	}

	ClearInput();

	return;
}

void startTypedCommand(SDL_Window *window)
{
	if (SDL_TextInputActive(window) || DebugSettings.TypingInConsole)
	{
		return;
	}

	SDL_StartTextInput(window);
	DebugSettings.TypingInConsole = true;
	DebugSettings.consoleFocus = false;
	DebugSettings.scrollVal = 0;
	
	DebugSettings.userInputIndex = 0;
	memset(DebugSettings.userInputString, 0, USER_INPUT_MAX_LEN);

	DebugSettings.userInputString[0] = ' ';

	DebugSettings.cursorXPos = 0.0;
}

void addTypedCommand(const char input[])
{
	int prevLength = strlen(DebugSettings.userInputString);
	DebugSettings.userInputIndex = clamp(DebugSettings.userInputIndex, 0, USER_INPUT_MAX_LEN - 1);

	if (DebugSettings.userInputIndex >= USER_INPUT_MAX_LEN - 1 || prevLength >= USER_INPUT_MAX_LEN - 1 || input == NULL)
	{
		return;
	}

	char buffer[USER_INPUT_MAX_LEN] = {0};

	if (DebugSettings.userInputString[DebugSettings.userInputIndex] != '\0')
	{
		strcpy(buffer, DebugSettings.userInputString + DebugSettings.userInputIndex);
	}
	
	int bytesAvailable = USER_INPUT_MAX_LEN - prevLength - 1;

	strncpy(DebugSettings.userInputString + DebugSettings.userInputIndex, input, bytesAvailable);

	strcat(DebugSettings.userInputString, buffer);

	DebugSettings.userInputIndex += strlen(DebugSettings.userInputString) - prevLength;

	DebugSettings.userInputString[USER_INPUT_MAX_LEN - 1] = 0;

	DebugSettings.cursorXPos = getCursorPos();
}


void renderConsole(World *GameWorld, SDL_Renderer *Screen)
{
	float xCorrection = (float)(ScreenData.screenWidth >> 1);
	float yCorrection = (float)(ScreenData.screenHeight >> 1);

	static const float inputFieldHeight = 28.0;
	static const float insideSpacing = 8.0;
	static const float topSpacing = 3.0;
	DebugSettings.consoleXPos = -512.0;
	DebugSettings.consoleYPos = yCorrection - consoleHeight;


	SDL_FRect box = {0};

	// render input field
	box.x = xCorrection + DebugSettings.consoleXPos;
	box.y = -(DebugSettings.consoleYPos - yCorrection);
	box.w = consoleWidth;
	box.h = inputFieldHeight;

	if (DebugSettings.consoleFocus)
	{
		SDL_SetRenderDrawColor(Screen, 0x2D, 0x2A, 0x2A, 0xBB);
	}
	else
	{
		SDL_SetRenderDrawColor(Screen, 0x36, 0x32, 0x32, 0xBB);
	}
	
	SDL_RenderFillRect(Screen, &box);

	// render console
	box.h = consoleHeight;
	box.y -= consoleHeight;
	SDL_SetRenderDrawColor(Screen, 0x1D, 0x1A, 0x1A, 0xBB);
	SDL_RenderFillRect(Screen, &box);
	box.y += consoleHeight;


	// render cursor
	box.w = 2.0;
	box.h = inputFieldHeight - (topSpacing * 2.0);
	box.x += insideSpacing + DebugSettings.cursorXPos;
	box.y += topSpacing;
	SDL_SetRenderDrawColor(Screen, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(Screen, &box);

	// render user input
	box.x = DebugSettings.consoleXPos + insideSpacing;
	box.y = DebugSettings.consoleYPos;
	int textWidth = (int)(consoleWidth - (2.0 * insideSpacing));
	if (strlen(DebugSettings.userInputString) > 0)
	{
	    AddDebugText(DebugSettings.userInputString, box.x, box.y, textWidth, DTFORMAT_SCREEN_RELATIVE);
	}

	int index = modulo(DebugSettings.consoleHistory.head, USER_INPUT_HISTORY_LEN);
	char all[USER_INPUT_MAX_LEN * USER_INPUT_HISTORY_LEN] = {0};

	for (int i = USER_INPUT_HISTORY_LEN - DebugSettings.scrollVal; i > 0; i--)
	{
		strcat(all, DebugSettings.consoleHistory.inputs[index]);
		index = (index + 1) % USER_INPUT_HISTORY_LEN;

		if (i > 1)
		{
			all[strlen(all)] = '\n';
		}
	}

	// render text history
	box.y += insideSpacing;

    AddDebugText(all, box.x, box.y, textWidth, DTFORMAT_JUSTIFY_TOP);

	return;
}