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
				if (ScreenData == NULL || ScreenData->Window == NULL || Running_In_Windows_Mode == 1)
				{
					break;
				}

				if (EventData->zoomScales[1] < 0.01 || EventData->zoomScales[0] < 0.01 || screenWidth < MINIMUM_SCREEN_WIDTH || screenHeight < MINIMUM_SCREEN_HEIGHT)
				{
					break;
				}

				float oldScaleX = ((float)ScreenData->windowWidth/(float)screenWidth);
				float oldScaleY = ((float)ScreenData->windowHeight/(float)screenHeight);

				float newZoomX = EventData->zoomScales[0];
				float newZoomY = EventData->zoomScales[1];

				if (newZoomX < MINIMUM_ZOOM || newZoomY < MINIMUM_ZOOM)
				{
					break;
				}

				ScreenData->zoomX = newZoomX;
				ScreenData->zoomY = newZoomY;
				
				SDL_SetRenderScale(ScreenData->Renderer, newZoomX * oldScaleX, newZoomY * oldScaleY);

				validateZoom(ScreenData);
			}
			break;


		case CHANGE_SCREEN_ZOOM:
			{
				if (ScreenData == NULL || ScreenData->Window == NULL || Running_In_Windows_Mode == 1)
				{
					break;
				}

				if (screenWidth < MINIMUM_SCREEN_WIDTH || screenHeight < MINIMUM_SCREEN_HEIGHT)
				{
					break;
				}

				float oldScaleX = ((float)ScreenData->windowWidth/(float)screenWidth);
				float oldScaleY = ((float)ScreenData->windowHeight/(float)screenHeight);

				float newZoomX = (ScreenData->zoomX + EventData->zoomScales[0]);
				float newZoomY = (ScreenData->zoomY + EventData->zoomScales[1]);

				if (newZoomX < MINIMUM_ZOOM || newZoomY < MINIMUM_ZOOM)
				{
					break;
				}

				ScreenData->zoomX = newZoomX;
				ScreenData->zoomY = newZoomY;
				
				SDL_SetRenderScale(ScreenData->Renderer, newZoomX * oldScaleX, newZoomY * oldScaleY);

				validateZoom(ScreenData);
			}
			break;

		case CHANGE_SCREEN_SIZE:
			{	
				if (ScreenData == NULL || ScreenData->Window == NULL || Running_In_Windows_Mode == 1)
				{
					break;
				}

				float newWidth = EventData->screenDimensions[0];
				float newHeight = EventData->screenDimensions[1];

				if (newWidth < MINIMUM_SCREEN_WIDTH || newHeight < MINIMUM_SCREEN_HEIGHT)
				{
					break;
				}

				float ScaleX = ((float)screenWidth/(float)ScreenData->windowWidth);
				float ScaleY = ((float)screenHeight/(float)ScreenData->windowHeight);

				SDL_SetWindowSize(ScreenData->Window, newWidth, newHeight);
				SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);

				screenWidth = ScreenData->windowWidth * ScaleX;
				screenHeight = ScreenData->windowHeight * ScaleY;
			}
			break;

		case CHANGE_SCREEN_SIZE_SCALE:
			{
				if (ScreenData == NULL || ScreenData->Window == NULL || Running_In_Windows_Mode == 1)
				{
					break;
				}

				if (screenWidth < MINIMUM_SCREEN_WIDTH || screenHeight < MINIMUM_SCREEN_HEIGHT)
				{
					break;
				}

				if (EventData->screenDimensions[0] < MINIMUM_SCREEN_WIDTH || EventData->screenDimensions[1] < MINIMUM_SCREEN_HEIGHT)
				{
					break;
				}

				SDL_SetWindowSize(ScreenData->Window, EventData->screenDimensions[0], EventData->screenDimensions[1]);
				SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);

				float newWidth = ScreenData->zoomX * (float)ScreenData->windowWidth/(float)screenWidth;
				float newHeight = ScreenData->zoomY * (float)ScreenData->windowHeight/(float)screenHeight;

				SDL_SetRenderScale(ScreenData->Renderer, newWidth, newHeight);
			}
			break;

		case ENABLE_FULLSCREEN:
			{
				if (ScreenData == NULL || ScreenData->Window == NULL || Running_In_Windows_Mode == 1)
				{
					break;
				}

				SDL_SetWindowFullscreen(ScreenData->Window, true);
				SDL_MaximizeWindow(ScreenData->Window);
				SDL_SyncWindow(ScreenData->Window);

				SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);
				screenWidth = ScreenData->windowWidth;
				screenHeight = ScreenData->windowHeight;
				ScreenData->Fullscreen = true;
			}
			break;

		case DISABLE_FULLSCREEN:
			{
				if (ScreenData == NULL || ScreenData->Window == NULL || Running_In_Windows_Mode == 1)
				{
					break;
				}

				SDL_SetWindowFullscreen(ScreenData->Window, false);
				SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);
				screenWidth = ScreenData->windowWidth;
				screenHeight = ScreenData->windowHeight;

				SDL_SetRenderScale(ScreenData->Renderer, ScreenData->zoomX, ScreenData->zoomY);
				ScreenData->Fullscreen = false;
			} 
			break;

		case ENABLE_FULLSCREEN_SCALE:
			{
				if (ScreenData == NULL || ScreenData->Window == NULL || Running_In_Windows_Mode == 1)
				{
					break;
				}

				if (screenWidth < MINIMUM_SCREEN_WIDTH || screenHeight < MINIMUM_SCREEN_HEIGHT || ScreenData->Fullscreen == true)
				{
					break;
				}

				SDL_SetWindowFullscreen(ScreenData->Window, true);
				SDL_SyncWindow(ScreenData->Window);

				SDL_GetWindowSize(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);
				float newWidth = ScreenData->zoomX * (float)ScreenData->windowWidth/(float)screenWidth;
				float newHeight = ScreenData->zoomY * (float)ScreenData->windowHeight/(float)screenHeight;

				SDL_SetRenderScale(ScreenData->Renderer, newWidth, newHeight);
				ScreenData->Fullscreen = true;
			}
			break;

		default:
			break;
	}


	GameWorld->GameEvent = NO_EVENT;
	memset(EventData, 0, sizeof(LemonGameEventData));


	return LEMON_SUCCESS;
}


int validateZoom(RenderFrame *ScreenData)
{
	// panic script if zoom is invalid for whatever reason
	if (ScreenData->zoomX < 0.01 || ScreenData->zoomY < 0.01)
	{
		ScreenData->zoomX = 1.0;
		ScreenData->zoomY = 1.0;

		if (ScreenData->windowWidth > MINIMUM_SCREEN_WIDTH && ScreenData->windowHeight > MINIMUM_SCREEN_HEIGHT && screenWidth > 1 && screenHeight > 1)
		{
			SDL_SetRenderScale(ScreenData->Renderer, (float)ScreenData->windowWidth/(float)screenWidth, (float)ScreenData->windowHeight/(float)screenHeight);
		}
		else
		{
			putConsoleString("\nERROR: Screen/window set as invalid sizes. (Something's gone wrong!)");
			SDL_SetRenderScale(ScreenData->Renderer, 1.0, 1.0);
		}
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

	int touchingPlayer = checkBoxOverlapsBoxBroad(Player->PlayerBox, inputObject->ObjectBox);

	if (touchingPlayer == 1 && inputObject->Action == 0)
	{
		inputObject->Action = 1;
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

	switch (inputObject->arg1)
	{
		case CACHE_TRIGGER:
			if (inputObject->Action == 1)
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
			}
			break;


		case CUTSCENE_TRIGGER:
		if (inputObject->Action == 1)
		{
			StartCutscene(inputObject->arg2, GameWorld);
			MarkObjectForDeletion(inputObject);
			inputObject->Action = 2;
		}
		break;


		case SET_BACKGROUND_TRIGGER:
		if (inputObject->Action == 1)
		{
			switchBackGroundSprite(inputObject->arg2, inputObject->arg3, &GameWorld->WorldBackground);
			inputObject->Action = 2;
		}
		break;


		case SET_CAMBOX_TRIGGER:
		if (inputObject->Action == 1)
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
		}
		break;


		default:
		MarkObjectForDeletion(inputObject);
		break;
	}


	return LEMON_SUCCESS;
}