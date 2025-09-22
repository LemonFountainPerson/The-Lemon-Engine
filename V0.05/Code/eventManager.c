#include "eventManager.h"


int UpdateFlagObject(World *GameWorld, Object* inputObject)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	PlayerData *Player = GameWorld->Player;


	int touchingPlayer = checkBoxOverlapsBox(Player->PlayerBox, inputObject->ObjectBox);

	if (touchingPlayer == 1 && inputObject->animationTick == 0)
	{
		inputObject->animationTick = 1;
	}

	if (touchingPlayer == 0)
	{
		inputObject->animationTick = 0;
	}


	switch (inputObject->arg1)
	{
		case SET_BACKGROUND_TRIGGER:
		if (inputObject->animationTick == 1)
		{
			switchBackGroundSprite(inputObject->arg2, inputObject->arg3, GameWorld);
			inputObject->animationTick = 2;
		}
		break;


		case SET_CAMBOX_TRIGGER:
		if (inputObject->animationTick == 1)
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

			inputObject->animationTick = 2;
		}
		break;


		default:
		MarkObjectForDeletion(inputObject, GameWorld->ObjectList);
		break;
	}


	return 0;
}


int PauseGame(World *GameWorld, int keyboard[256])
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameWorld->GamePaused = 1;
	GameWorld->drawBackGround = 0;
	GameWorld->MainCamera.CameraMode = MENU_CAMERA;
	ClearKeyboardInput(keyboard);

	// In order to have particles be visible in the pause menu while hiding normal particles, the camera is moved elsewhere
	// and is restored to its previous position when unpaused  (There should not be any level geometry before X pos 0)
	// When game is resumed, even if x pos is not reset to original value, it will be corrected to 0 by WorldCameraControl
	// X pos is saved in cameraXBuffer


	if (GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	AddObject(GameWorld, UI_ELEMENT, 0, 0, 0, 0, PAUSE_MENU_CONTROLLER, 0, 0, 0, 0);


	return 0;
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
	GameWorld->drawBackGround = 1;
	GameWorld->MainCamera.CameraMode = FOLLOW_PLAYER;

	return 0;
}


int OpenSettings(World *GameWorld, int keyboard[256])
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	GameWorld->drawBackGround = 0;
	ClearKeyboardInput(keyboard);

	if (GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	AddObject(GameWorld, UI_ELEMENT, 0, 0, 0, 0, SETTINGS_MENU_CONTROLLER, 0, 0, 0, 0);

	return 0;
}


int PlayCutscene(World *GameWorld, int keyboard[256])
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState != CUTSCENE)
	{
		GameWorld->SceneTick = 0;
		return EXECUTION_UNNECESSARY;
	}

	if (GameWorld->SceneTick == 0)
	{
		StartCutscene(GameWorld);
		GameWorld->SceneTick = 1; 
	}

	return 0;
}


int StartCutscene(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState != CUTSCENE)
	{
		return EXECUTION_UNNECESSARY;
	}

	return 0;
}