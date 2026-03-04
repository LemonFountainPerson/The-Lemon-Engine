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
	loadLevel(GameWorld, 1);

	//PositionSound(PlaySound("StartUp", "Music", MUSIC_CHANNEL, 0.6), 1000.0, 200.0, 15.0, GameWorld->MainCamera);

	return LEMON_SUCCESS;
}


// Its unrecommended to use the change_screen_size, enable_fullscreen, etc commands interchangably with their SCALE counterparts, but it is possible
int HandleGameWorldEvents(World *GameWorld, RenderFrame *ScreenData)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (keyboard[LMN_ESCAPE] == 1)
	{
		AcknowledgeButton(LMN_ESCAPE);

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
	int i = 0;

	while (i < EngineSettings.MaxGameEvents)
	{
		ExecuteGameEvent(&eventList[i], GameWorld, ScreenData);

		i++;
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

	GameEventData *EventData = &inputEvent->EventData;

	switch (inputEvent->EventID)
	{
		case SWITCH_LEVEL:
			if (EventData->newLevelID > -1)
			{
				loadLevel(GameWorld, EventData->newLevelID);
			}
			break;

		case SET_CAMERA_ZOOM:
			{
				applyCameraZoom(EventData->zoomScales[0], EventData->zoomScales[1], &GameWorld->MainCamera, ScreenData);
			} break;

		case CHANGE_CAMERA_ZOOM:
			{
				applyCameraZoom((GameWorld->MainCamera.zoomX + EventData->zoomScales[0]), (GameWorld->MainCamera.zoomY + EventData->zoomScales[1]), &GameWorld->MainCamera, ScreenData);
			} break;

		case SET_SCREEN_AND_RENDERER_SIZE:
			{
				applyScreenAndRendererSize(EventData->screenDimensions[0], EventData->screenDimensions[1], ScreenData);
			} break;

		case CHANGE_SCREEN_SIZE:
			{	
				applyScreenSize(EventData->screenDimensions[0], EventData->screenDimensions[1], ScreenData);
			} break;

		case CHANGE_SCREEN_SIZE_SCALE:
			{
				applyScreenSizeScale(EventData->screenDimensions[0], EventData->screenDimensions[1], ScreenData);
			} break;

		case ENABLE_FULLSCREEN:
			{
				applyEnableFullscreen(ScreenData);
			} break;

		case DISABLE_FULLSCREEN:
			{
				applyDisableFullscreen(ScreenData);
			} break;

		case ENABLE_FULLSCREEN_SCALE:
			{
				if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
				{
					break;
				}

				if (ScreenData->Fullscreen == true)
				{
					break;
				}

				validateScreenDimensions(ScreenData);

				SDL_SetWindowFullscreen(ScreenData->Window, true);
				SDL_SyncWindow(ScreenData->Window);
				SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);

				ScreenData->Fullscreen = true;
			} break;

		case STREAM_LEVEL_PARTITION:
			{
				int result = loadLevelDataChunk(GameWorld, EventData->loadedFile, 10);
				if (result != LEMON_SUCCESS)
				{
					fclose(EventData->loadedFile);
					EventData->loadedFile = NULL;
					inputEvent->loadingAFile = false;
					inputEvent->canDelete = true;
				}
			} break;

		case DELETE_ENVIRONMENT_OBJECTS:
			{
				deleteAllEnvironmentObjects(GameWorld->ObjectList);
			} break;

		default:
			break;
	}

	// just in case
	if (inputEvent->loadingAFile && inputEvent->canDelete && EventData->loadedFile)
	{
		fclose(EventData->loadedFile);
		EventData->loadedFile = NULL;
		inputEvent->loadingAFile = false;
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
		if (!eventList[i].canDelete && eventList[i].loadingAFile)
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
		if (eventList[i].canDelete && eventList[i].EventID != NO_EVENT)
		{
			eventList[i].EventID = NO_EVENT;
			GameWorld->GameEvents.eventsPending--;
		}

		i++;
	}

	if (GameWorld->GameEvents.eventsPending < 1)
	{
		GameWorld->GameEvents.nextAvailable = 0;
	}

	return LEMON_SUCCESS;
}


GameEvent* findAvailableEvent(GameEvent *gameEvents, int nextAvail)
{
	int i = nextAvail;
	int k = 0;

	while (k < EngineSettings.MaxGameEvents)
	{
		if (gameEvents[i].EventID == NO_EVENT)
		{
			return &gameEvents[i];
		}

		i = (i + 1) % EngineSettings.MaxGameEvents;
		k++;
	}

	return NULL;
}

GameEvent* addNewGameEvent(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->GameEvents.eventsPending >= EngineSettings.MaxGameEvents)
	{
		return NULL;
	}

	GameEvent *eventPtr = findAvailableEvent(GameWorld->GameEvents.Events, GameWorld->GameEvents.nextAvailable);

	if (eventPtr == NULL)
	{
		GameWorld->GameEvents.eventsPending = EngineSettings.MaxGameEvents;
		return NULL;
	}

	memset(eventPtr, 0, sizeof(GameEvent));
	eventPtr->canDelete = true;
	eventPtr->loadingAFile = false;

	GameWorld->GameEvents.eventsPending++;
	GameWorld->GameEvents.nextAvailable++;

	return eventPtr;
}

int switchLevel(int level, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (level < 0)
	{
		return INVALID_DATA;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return LEMON_ERROR;
	}

	newEvent->EventID = SWITCH_LEVEL;
	newEvent->EventData.newLevelID = level;

	return LEMON_SUCCESS;
}

int streamPartition(int partID, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (partID < 0)
	{
		return INVALID_DATA;
	}

	char fileName[MAX_LEN] = {0};
	snprintf(fileName, MAX_LEN, "Level%d_Part%d", GameWorld->level, partID);

	FILE *fPtr = openFile(fileName, LEVELDATA_ROOT, "--PARTITION_DATA--");

	if (fPtr == NULL)
	{
		return INVALID_DATA;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		fclose(fPtr);
		return LEMON_ERROR;
	}

	newEvent->EventID = STREAM_LEVEL_PARTITION;
	newEvent->EventData.loadedFile = fPtr;
	newEvent->canDelete = false;
	newEvent->loadingAFile = true;


	return LEMON_SUCCESS;
}

int scheduleEnvironmentDeletion(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return LEMON_ERROR;
	}

	newEvent->EventID = DELETE_ENVIRONMENT_OBJECTS;

	return LEMON_SUCCESS;
}

int switchToNewPartition(int partID, World *GameWorld)
{
	scheduleEnvironmentDeletion(GameWorld);
	return streamPartition(partID, GameWorld);
}


int changeScreenSizeScaled(int newWidth, int newHeight, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return LEMON_ERROR;
	}

	newEvent->EventID = CHANGE_SCREEN_SIZE_SCALE;
	newEvent->EventData.screenDimensions[0] = newWidth;
	newEvent->EventData.screenDimensions[1] = newHeight;

	return LEMON_SUCCESS;
}

int changeScreenSize(int newWidth, int newHeight, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return LEMON_ERROR;
	}

	newEvent->EventID = CHANGE_SCREEN_SIZE;
	newEvent->EventData.screenDimensions[0] = newWidth;
	newEvent->EventData.screenDimensions[1] = newHeight;

	return LEMON_SUCCESS;
}

int setScreenAndRendererSize(int newWidth, int newHeight, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return LEMON_ERROR;
	}

	newEvent->EventID = SET_SCREEN_AND_RENDERER_SIZE;
	newEvent->EventData.screenDimensions[0] = newWidth;
	newEvent->EventData.screenDimensions[1] = newHeight;

	return LEMON_SUCCESS;
}

int enableFullscreen(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return LEMON_ERROR;
	}

	newEvent->EventID = ENABLE_FULLSCREEN;

	return LEMON_SUCCESS;
}

int enableFullscreenScaled(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return LEMON_ERROR;
	}

	newEvent->EventID = ENABLE_FULLSCREEN_SCALE;

	return LEMON_SUCCESS;
}

int disableFullscreen(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return LEMON_ERROR;
	}

	newEvent->EventID = DISABLE_FULLSCREEN;

	return LEMON_SUCCESS;
}

int toggleFullscreen(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return LEMON_ERROR;
	}

	if (ScreenData.Fullscreen == true)
	{
		newEvent->EventID = DISABLE_FULLSCREEN;
	}
	else
	{
		newEvent->EventID = DISABLE_FULLSCREEN;
	}


	return LEMON_SUCCESS;
}


int setCameraZoom(float zoomX, float zoomY, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return LEMON_ERROR;
	}

	newEvent->EventID = SET_CAMERA_ZOOM;
	newEvent->EventData.zoomScales[0] = zoomX;
	newEvent->EventData.zoomScales[1] = zoomY;

	return LEMON_SUCCESS;
}


int changeCameraZoom(float zoomX, float zoomY, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameEvent *newEvent = addNewGameEvent(GameWorld);
	if (newEvent == NULL)
	{
		return LEMON_ERROR;
	}

	newEvent->EventID = CHANGE_CAMERA_ZOOM;
	newEvent->EventData.zoomScales[0] = zoomX;
	newEvent->EventData.zoomScales[1] = zoomY;

	return LEMON_SUCCESS;
}


int applyCameraZoom(float newZoomX, float newZoomY, Camera *inputCamera, RenderFrame *ScreenData)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL || inputCamera == NULL)
	{
		return MISSING_DATA;
	}

	if (newZoomX < MINIMUM_ZOOM || newZoomY < MINIMUM_ZOOM || screenWidth < MINIMUM_SCREEN_WIDTH || screenHeight < MINIMUM_SCREEN_HEIGHT)
	{
		return INVALID_DATA;
	}

	inputCamera->zoomX = newZoomX;
	inputCamera->zoomY = newZoomY;
	inputCamera->zoomedWidth = screenWidth / newZoomX;
	inputCamera->zoomedHeight = screenHeight / newZoomY;
	
	SDL_SetRenderScale(ScreenData->Renderer, newZoomX, newZoomY);

	validateZoom(inputCamera, ScreenData);

	return LEMON_SUCCESS;
}


int applyScreenAndRendererSize(int newWidth, int newHeight, RenderFrame *ScreenData)
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

	validateScreenDimensions(ScreenData);

	SDL_SetWindowSize(ScreenData->Window, newWidth, newHeight);
    SDL_SyncWindow(ScreenData->Window);
	SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);

	screenWidth = ScreenData->windowWidth;
	screenHeight = ScreenData->windowHeight;

	SDL_SetRenderLogicalPresentation(ScreenData->Renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_STRETCH);
	
	return LEMON_SUCCESS;
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

	validateScreenDimensions(ScreenData);

	float ScaleX = ((float)screenWidth/(float)ScreenData->windowWidth);
	float ScaleY = ((float)screenHeight/(float)ScreenData->windowHeight);

	SDL_SetWindowSize(ScreenData->Window, newWidth, newHeight);
    SDL_SyncWindow(ScreenData->Window);
	SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);

	screenWidth = ScreenData->windowWidth * ScaleX;
	screenHeight = ScreenData->windowHeight * ScaleY;

	SDL_SetRenderLogicalPresentation(ScreenData->Renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_STRETCH);
	
	return LEMON_SUCCESS;
}


int applyScreenSizeScale(int newWidth, int newHeight, RenderFrame *ScreenData)
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

	validateScreenDimensions(ScreenData);

	SDL_SetWindowSize(ScreenData->Window, newWidth, newHeight);
    SDL_SyncWindow(ScreenData->Window);
	SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);


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

	float ScaleX = ((float)screenWidth/(float)ScreenData->windowWidth);
	float ScaleY = ((float)screenHeight/(float)ScreenData->windowHeight);

	SDL_SetWindowFullscreen(ScreenData->Window, true);
	SDL_SyncWindow(ScreenData->Window);

	SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);
	screenWidth = ScreenData->windowWidth * ScaleX;
	screenHeight = ScreenData->windowHeight * ScaleY;
	SDL_SetRenderLogicalPresentation(ScreenData->Renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_STRETCH);

	ScreenData->Fullscreen = true;

	return LEMON_SUCCESS;
}


int applyDisableFullscreen(RenderFrame *ScreenData)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	SDL_SetWindowFullscreen(ScreenData->Window, false);
	SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);

	ScreenData->Fullscreen = false;

	return LEMON_SUCCESS;
}


int validateScreenDimensions(RenderFrame *ScreenData)
{
	if (ScreenData->windowWidth < MINIMUM_SCREEN_WIDTH || ScreenData->windowHeight < MINIMUM_SCREEN_HEIGHT || screenWidth < MINIMUM_SCREEN_WIDTH || screenHeight < MINIMUM_SCREEN_HEIGHT)
	{
		ScreenData->windowWidth = H_RESOLUTION;
		ScreenData->windowHeight = V_RESOLUTION;
		screenWidth = H_RESOLUTION;
		screenHeight = V_RESOLUTION;

		SDL_SetWindowSize(ScreenData->Window, screenWidth, screenHeight);

		SDL_SetRenderLogicalPresentation(ScreenData->Renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_STRETCH);

		return LEMON_ERROR;
	}

	return LEMON_SUCCESS;
}


int validateZoom(Camera *inputCamera, RenderFrame *ScreenData)
{
	// panic script if zoom is invalid for whatever reason
	if (inputCamera->zoomX < MINIMUM_ZOOM || inputCamera->zoomY < MINIMUM_ZOOM)
	{
		inputCamera->zoomX = 1.0;
		inputCamera->zoomY = 1.0;

		if (ScreenData->windowWidth < MINIMUM_SCREEN_WIDTH || ScreenData->windowHeight < MINIMUM_SCREEN_HEIGHT || screenWidth < 1 || screenHeight < 1)
		{
			putConsoleString("\nERROR: Screen/window set as invalid sizes. (Something's gone wrong!)");
			SDL_SetRenderScale(ScreenData->Renderer, 1.0, 1.0);
		}

		return LEMON_ERROR;
	}

	return LEMON_SUCCESS;
}


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
	WorldCameraControl(GameWorld, &GameWorld->MainCamera);
	HideHUD(GameWorld->ObjectList);

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
	inputObject->ObjectDisplay->RenderModeOverride = DO_NOT_RENDER;
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

	inputBox->xPos = inputCam.CameraX;
	inputBox->yPos = inputCam.CameraY;
	inputBox->xSize = screenWidth;
	inputBox->ySize = screenHeight;

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
	if (GameWorld == NULL || GameWorld->ObjectList == NULL || Player == NULL || Player->PlayerBox == NULL || inputObject == NULL)
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
			StartCutscene(inputObject->arg2, GameWorld);
			MarkObjectForDeletion(inputObject);
		} break;


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
		}
		break;

		default:
		MarkObjectForDeletion(inputObject);
		break;
	}


	return LEMON_SUCCESS;
}
