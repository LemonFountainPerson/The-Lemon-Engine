#include "cutsceneManager.h"



int PlayCutscene(CutsceneID inputID, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (inputID < 0 || inputID >= UNDEFINED_CUTSCENE)
	{
		return INVALID_DATA;
	}

	GameWorld->CurrentCutscene = inputID;
	GameWorld->GameState = CUTSCENE;
	GameWorld->SceneTick = 0;

	StartCutscene(GameWorld);

	return 0;
}


int UpdateCutscene(World *GameWorld, int keyboard[256])
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState != CUTSCENE)
	{
		return EXECUTION_UNNECESSARY;
	}

	// Play cutscene
	switch (GameWorld->CurrentCutscene)
	{
		default:
		GameWorld->SceneTick--;
		break;
	}

	// < 0 means cutscene has ended (e.g: -1)
	if (GameWorld->SceneTick < 0)
	{
		EndCutscene(GameWorld);
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

	// Set-up cutscene
	switch (GameWorld->CurrentCutscene)
	{
		default:
		GameWorld->SceneTick = 0;
		break;
	}

	ResetPlayer(GameWorld->Player);

	return 0;
}


int EndCutscene(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState != CUTSCENE)
	{
		return EXECUTION_UNNECESSARY;
	}

	GameWorld->GameState = GAMEPLAY;

	PlayerData *Player = GameWorld->Player;

	if (Player != NULL && Player->PlayerPtr != NULL && Player->PlayerPtr->State == ACTOR)
	{
		GameWorld->Player->PlayerPtr->State = DEFAULT;
	}

	int result = LEMON_SUCCESS;
	while (result == LEMON_SUCCESS && GameWorld->TextQueue != NULL)
	{
		result = endTextInstance(GameWorld);
	}

	if (GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	// By default, any objects that were not manually restored from Actor state will be deleted
	Object *currentObject = GameWorld->ObjectList->firstObject;

	while (currentObject != NULL)
	{
		if (currentObject->State == ACTOR)
		{
			MarkObjectForDeletion(currentObject, GameWorld->ObjectList);
		}

		currentObject = currentObject->nextObject;
	}

	return 0;
}
