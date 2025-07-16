#include "eventManager.h"


int StartGame(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	// Logic for handle flow of menus and levels, etc can go here for game start
	loadLevel(GameWorld, 1);

	//SayText("Hello World!", NO_PORTRAIT, DEFAULT_TOP, GameWorld);
	//SayText("This is a test message on the top!", NO_PORTRAIT, DEFAULT_TOP, GameWorld);
	//SayText("And now i'm spookily inverted! oooooooooooo~! And a portrait!", "Test_Face", INVERTED_BOTTOM, GameWorld);

	return 0;
}


int HandleGameWorldEvents(World *GameWorld, int keyboard[256])
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState == SWITCHING_LEVEL)
	{
		loadLevel(GameWorld, GameWorld->level);
		return 0;
	}


	if (keyboard[LMN_ESCAPE] == 1)
	{
		keyboard[LMN_ESCAPE] = 2;

		if (GameWorld->GamePaused == 0)
		{
			PauseGame(GameWorld, keyboard);
		}
		else
		{
			ResumeGame(GameWorld);
		}
	}

	return 0;
}


int PauseGame(World *GameWorld, int keyboard[256])
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


	return 0;
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


	int touchingPlayer = checkBoxOverlapsBox(Player->PlayerBox, inputObject->ObjectBox);

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
			switchBackGroundSprite(inputObject->arg2, inputObject->arg3, GameWorld);
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


	return 0;
}