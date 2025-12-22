#include "eventManager.h"


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


	if (GameWorld->GameEvent == NO_EVENT)
	{
		return EXECUTION_UNNECESSARY;
	}

	LemonGameEventData *EventData = &GameWorld->GameEventData;

	switch (GameWorld->GameEvent)
	{
		case SWITCH_LEVEL:
			if (EventData->newLevelID > -1)
			{
				loadLevel(GameWorld, EventData->newLevelID);
			}
			break;

		case SET_SCREEN_ZOOM:
			{
				setCameraZoom(EventData->zoomScales[0], EventData->zoomScales[1], &GameWorld->MainCamera, ScreenData);
			} break;

		case CHANGE_SCREEN_ZOOM:
			{
				setCameraZoom((GameWorld->MainCamera.zoomX + EventData->zoomScales[0]), (GameWorld->MainCamera.zoomY + EventData->zoomScales[1]), &GameWorld->MainCamera, ScreenData);
			} break;

		case SET_SCREEN_AND_RENDERER_SIZE:
			{
				setScreenAndRendererSize(EventData->screenDimensions[0], EventData->screenDimensions[1], ScreenData);
			} break;

		case CHANGE_SCREEN_SIZE:
			{	
				setScreenSize(EventData->screenDimensions[0], EventData->screenDimensions[1], ScreenData);
			} break;

		case CHANGE_SCREEN_SIZE_SCALE:
			{
				setScreenSizeScale(EventData->screenDimensions[0], EventData->screenDimensions[1], ScreenData);
			} break;

		case ENABLE_FULLSCREEN:
			{
				enableFullscreen(ScreenData);
			} break;

		case DISABLE_FULLSCREEN:
			{
				disableFullscreen(ScreenData);
			} break;

		case ENABLE_FULLSCREEN_SCALE:
			{
				if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
				{
					break;
				}

				if (Running_In_Windows_Mode == 1 || ScreenData->Fullscreen == true)
				{
					break;
				}

				validateScreenDimensions(ScreenData);

				SDL_SetWindowFullscreen(ScreenData->Window, true);
				SDL_SyncWindow(ScreenData->Window);
				SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);

				ScreenData->Fullscreen = true;
			} break;

		default:
			break;
	}


	GameWorld->GameEvent = NO_EVENT;
	memset(EventData, 0, sizeof(LemonGameEventData));


	return LEMON_SUCCESS;
}


int setCameraZoom(float newZoomX, float newZoomY, Camera *inputCamera, RenderFrame *ScreenData)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL || inputCamera == NULL)
	{
		return MISSING_DATA;
	}

	if (Running_In_Windows_Mode == 1)
	{
		return ACTION_DISABLED;
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


int setScreenAndRendererSize(int newWidth, int newHeight, RenderFrame *ScreenData)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	if (ScreenData->Fullscreen == true || Running_In_Windows_Mode == 1)
	{
		return ACTION_DISABLED;
	}

	if (newWidth < MINIMUM_SCREEN_WIDTH || newHeight < MINIMUM_SCREEN_HEIGHT)
	{
		return INVALID_DATA;
	}

	validateScreenDimensions(ScreenData);

	SDL_SetWindowSize(ScreenData->Window, newWidth, newHeight);
	SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);

	screenWidth = ScreenData->windowWidth;
	screenHeight = ScreenData->windowHeight;

	SDL_SetRenderLogicalPresentation(ScreenData->Renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_STRETCH);
	
	return LEMON_SUCCESS;
}


int setScreenSize(int newWidth, int newHeight, RenderFrame *ScreenData)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	if (ScreenData->Fullscreen == true || Running_In_Windows_Mode == 1)
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
	SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);

	screenWidth = ScreenData->windowWidth * ScaleX;
	screenHeight = ScreenData->windowHeight * ScaleY;

	SDL_SetRenderLogicalPresentation(ScreenData->Renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_STRETCH);
	
	return LEMON_SUCCESS;
}


int setScreenSizeScale(int newWidth, int newHeight, RenderFrame *ScreenData)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	if (ScreenData->Fullscreen == true || Running_In_Windows_Mode == 1)
	{
		return ACTION_DISABLED;
	}

	if (newWidth < MINIMUM_SCREEN_WIDTH || newHeight < MINIMUM_SCREEN_HEIGHT)
	{
		return INVALID_DATA;
	}

	validateScreenDimensions(ScreenData);

	SDL_SetWindowSize(ScreenData->Window, newWidth, newHeight);
	SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);

	return LEMON_SUCCESS;
}


int enableFullscreen(RenderFrame *ScreenData)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	if (Running_In_Windows_Mode == 1 || ScreenData->Fullscreen == true)
	{
		return ACTION_DISABLED;
	}

	SDL_SetWindowFullscreen(ScreenData->Window, true);
	SDL_SyncWindow(ScreenData->Window);

	SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);
	screenWidth = ScreenData->windowWidth;
	screenHeight = ScreenData->windowHeight;
	SDL_SetRenderLogicalPresentation(ScreenData->Renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_STRETCH);

	ScreenData->Fullscreen = true;

	return LEMON_SUCCESS;
}


int disableFullscreen(RenderFrame *ScreenData)
{
	if (ScreenData == NULL || ScreenData->Window == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	if (Running_In_Windows_Mode == 1)
	{
		return ACTION_DISABLED;
	}

	SDL_SetWindowFullscreen(ScreenData->Window, false);
	SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);
	screenWidth = ScreenData->windowWidth;
	screenHeight = ScreenData->windowHeight;

	SDL_SetRenderLogicalPresentation(ScreenData->Renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_STRETCH);
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

	AddObject(GameWorld, UI_ELEMENT, 0, 0, 0, 0, PAUSE_MENU_CONTROLLER, 0, 0, 0, 0);

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

	switch (inputObject->arg1)
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


bool detectCamera(Object* inputObject, Camera inputCamera)
{
	if (inputObject == NULL)
	{
		return false;
	}

	PhysicsRect camBox = {0};
	camBox.xPos = inputCamera.CameraX;
	camBox.yPos = inputCamera.CameraY;
	camBox.xSize = 1;
	camBox.ySize = 1;

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


	switch (inputObject->arg1)
	{
		case CACHE_TRIGGER:
		if (detectCamera(inputObject, GameWorld->MainCamera))
		{
			PhysicsRect boundingBox;
			boundingBox.xPos = inputObject->arg2;
			boundingBox.xSize = inputObject->arg3 - inputObject->arg2;
			boundingBox.xPosRight = inputObject->arg3;
			boundingBox.yPos = inputObject->arg4;
			boundingBox.ySize = inputObject->arg5 - inputObject->arg4;
			boundingBox.yPosTop = inputObject->arg5;

			cacheObjects(GameWorld->ObjectList, boundingBox);
			inputObject->Action = 2;
		} break;


		case CUTSCENE_TRIGGER:
		if (detectPlayer(inputObject, Player))
		{
			StartCutscene(inputObject->arg2, GameWorld);
			MarkObjectForDeletion(inputObject);
			inputObject->Action = 2;
		} break;


		case SET_BACKGROUND_TRIGGER:
		if (detectCamera(inputObject, GameWorld->MainCamera))
		{
			switchBackGroundSprite(inputObject->arg2, inputObject->arg3, &GameWorld->WorldBackground);
			inputObject->Action = 2;
		} break;


		case SET_CAMBOX_TRIGGER:
		if (detectCamera(inputObject, GameWorld->MainCamera))
		{
			if (inputObject->arg2 > -1)
			{
				GameWorld->MainCamera.minCameraX = inputObject->arg2;
			}

			if (inputObject->arg3 > -1)
			{
				GameWorld->MainCamera.maxCameraX = inputObject->arg3;
			}

			if (inputObject->arg4 > -1)
			{
				GameWorld->MainCamera.minCameraY = inputObject->arg4;
			}

			if (inputObject->arg5 > -1)
			{
				GameWorld->MainCamera.maxCameraY = inputObject->arg5;
			}	

			inputObject->Action = 2;
		} break;


		default:
		MarkObjectForDeletion(inputObject);
		break;
	}


	return LEMON_SUCCESS;
}