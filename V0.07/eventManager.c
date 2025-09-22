#include "eventManager.h"


int StartGame(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	// Logic for handle flow of menus and levels, etc can go here for game start
	loadLevel(GameWorld, 1);


	return LEMON_SUCCESS;
}


int HandleGameWorldEvents(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	switch (GameWorld->GameEvent)
	{
		case SWITCH_LEVEL:
			loadLevel(GameWorld, GameWorld->level);
			break;


		case CHANGE_SCREEN_SIZE:
			{	
				if (screenBuffer == NULL)
				{
					break;
				}

			}
			break;


		default:
			break;
	}


	if (keyboard[LMN_ESCAPE] == 1)
	{
		keyboard[LMN_ESCAPE] = 2;

		if (GameWorld->GamePaused == 0)
		{
			PauseGame(GameWorld);
		}
		else
		{
			ResumeGame(GameWorld);
		}
	}


	GameWorld->GameEvent = NO_EVENT;


	return LEMON_SUCCESS;
}


int PauseGame(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState == EMPTY_GAME || GameWorld->GameState == LOADING || GameWorld->PlayingText == 1)
	{
		return ACTION_DISABLED;
	}

	GameWorld->GamePaused = 1;
	RenderSettings.drawBackGround = 0;
	GameWorld->MainCamera.CameraMode = MENU_CAMERA;
	ClearKeyboardInput();

	// In order to have particles be visible in the pause menu while hiding normal particles, the camera is moved elsewhere
	// and is restored to its previous position when unpaused  (There should not be any level geometry before X pos 0)
	// When game is resumed, even if x pos is not reset to original value, it will be corrected to 0 by WorldCameraControl
	// X pos is saved in cameraXBuffer


	if (GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	AddObject(GameWorld, UI_ELEMENT, 0, 0, 0, 0, PAUSE_MENU_CONTROLLER, 0, 0, 0, 0);


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
	RenderSettings.drawBackGround = 1;
	GameWorld->MainCamera.CameraMode = FOLLOW_PLAYER;

	return LEMON_SUCCESS;
}


int OpenSettings(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	RenderSettings.drawBackGround = 0;
	ClearKeyboardInput();

	if (GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	AddObject(GameWorld, UI_ELEMENT, 0, 0, 0, 0, SETTINGS_MENU_CONTROLLER, 0, 0, 0, 0);

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

	switch (inputObject->arg1)
	{
		default:
		break;
	}


	return LEMON_SUCCESS;
}


int UpdateFlagObject(World *GameWorld, Object* inputObject)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL || GameWorld->Player == NULL || GameWorld->Player->PlayerBox == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState != GAMEPLAY)
	{
		return ACTION_DISABLED;
	}

	PlayerData *Player = GameWorld->Player;


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

				cacheObjects(GameWorld->ObjectList, &boundingBox, GameWorld->Player);
				inputObject->Action = 2;
			}
			break;


		case CUTSCENE_TRIGGER:
		if (inputObject->Action == 1)
		{
			PlayCutscene(inputObject->arg2, GameWorld);
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