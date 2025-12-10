#include "cutsceneManager.h"
#define END_SCENE_HERE createSceneAction(SCENE_END, GameWorld)


// Define cutscene playback here
int StartCutscene(CutsceneID inputID, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (inputID <= NO_CUTSCENE || inputID >= UNDEFINED_CUTSCENE || !(GameWorld->GameState == GAMEPLAY || GameWorld->GameState == CUTSCENE))
	{
		return INVALID_DATA;
	}

	// end current cutscene if one is already playing
	if (GameWorld->CurrentCutscene != NO_CUTSCENE)
	{
		EndCutscene(GameWorld);
	}

	GameWorld->CurrentCutscene = inputID;
	GameWorld->GameState = CUTSCENE;
	

	// Set-up cutscene
	switch (GameWorld->CurrentCutscene)
	{
	case TEST_SCENE:
		GameWorld->MainCamera.CameraMode = FREE_ROAM_RESTRICTED;

		SayText("A test cutscene, huh?", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		Wait(1.5, GameWorld);
		SayText("How fantastic.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		AnimateActor("MainPlayer", "Jump", 10, GameWorld);
		SayText("The player should animate now.", NO_PORTRAIT, BASIC_TOP, GameWorld);

		SwitchActorSprite("MainPlayer", "StickMan", GameWorld);
		SayText("The player should be standing now.", NO_PORTRAIT, BASIC_TOP, GameWorld);

		MoveActor("MainPlayer", 5.0, 2.0, 120, GameWorld);
		SceneAction_MoveCameraSmooth(3200.0, 300.0, 8.0, GameWorld);
		WaitUntil(RotateActor("MainPlayer", 10.0, 120, GameWorld));
	
		SayText("The player should have moved now.", NO_PORTRAIT, BASIC_TOP, GameWorld);
		SetActorDirection("MainPlayer", 90.0, GameWorld);

		SceneAction_ChangeZoom(0.005, 0.025, 100, GameWorld);
		SayTextOption("Play new cutscene?", NO_PORTRAIT, BASIC_FADE, GameWorld, 3, 
				"Test scene", 	&StartCutscene, TEST_SCENE, 
				"Test scene 2", &StartCutscene, TEST_SCENE_2,
				"No", NULL, NULL);
		SceneAction_SetZoom(1.0, 1.0, GameWorld);
		break;

	case TEST_SCENE_2:
		SayText("Dialogue test", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		Wait(1.0, GameWorld);
		SayText("A small tomato is really just a \ncherry.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		break;

	default:
		break;
	}

	ResetPlayer(&GameWorld->Player);

	return LEMON_SUCCESS;
}


int UpdateCutscene(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->CurrentCutscene == NO_CUTSCENE)
	{
		return EXECUTION_UNNECESSARY;
	}

	// Play cutscene
	switch (GameWorld->CurrentCutscene)
	{
		default:
		break;
	}

	updateSceneActions(GameWorld);


	if (GameWorld->SceneActionList == NULL || GameWorld->CurrentCutscene == END_CUTSCENE)
	{
		EndCutscene(GameWorld);
	}
	

	return LEMON_SUCCESS;
}


int updateSceneActions(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->SceneActionList == NULL)
	{
		return MISSING_DATA;
	}

	SceneAction *ActionPtr = GameWorld->SceneActionList;

	while (ActionPtr != NULL)
	{
		// execute code
		RunSceneAction(ActionPtr, GameWorld);

		if (ActionPtr->repeatTimes < 1)
		{
			ActionPtr = deleteSceneAction(ActionPtr, GameWorld);
		}
		else
		{
			if (ActionPtr->parallelAction == false)
			{
				return LEMON_SUCCESS;
			}
		
			ActionPtr = ActionPtr->nextSceneAction;
		}
		
	}


	return LEMON_SUCCESS;
}


int RunSceneAction(SceneAction *inputAction, World *GameWorld)
{
	if (inputAction == NULL || inputAction->ActionID == UNDEFINED_SCENE_ACTION)
	{
		return INVALID_DATA;
	}

	inputAction->repeatTimes--;

	switch (inputAction->ActionID)
	{
		case SCENE_SAY_TEXT:
		inputAction->repeatTimes = 1;
		break;

		case SCENE_END:
		GameWorld->CurrentCutscene = END_CUTSCENE;
		break;

		case SCENE_ANIMATE_ACTOR:
		{
			if (inputAction->ActorObject == NULL || inputAction->ActorObject->ObjectDisplay == NULL)
			{
				break;
			}

			DisplayData *actorDisplay = inputAction->ActorObject->ObjectDisplay;

			int animID = inputAction->ActionData.animationDetails[0];
			int loopCount = inputAction->ActionData.animationDetails[1];

			if (inputAction->repeatTimes > 0)
			{
				PlayAnimationByIndex(animID, loopCount, actorDisplay);
			}
			
			// Wait until animation is complete if set as non-parallel
			if (inputAction->parallelAction == false && actorDisplay->currentAnimation == animID)
			{
				inputAction->repeatTimes = 1;
			}
		} break;

		case SCENE_SET_ACTOR_SPRITE:
		{
			if (inputAction->ActorObject == NULL || inputAction->ActorObject->ObjectDisplay == NULL)
			{
				break;
			}

			DisplayData *actorDisplay = inputAction->ActorObject->ObjectDisplay;
			int spriteID = inputAction->ActionData.animationDetails[0];

			actorDisplay->currentAnimation = 0;
			switchSprite(spriteID, USE_CURRENT_SPRITESET, actorDisplay);
		} break;

		case SCENE_SET_ACTOR_POS:
		{
			if (inputAction->ActorObject == NULL || inputAction->ActorObject->ObjectBox == NULL)
			{
				break;
			}

			PhysicsRect *actorBox = inputAction->ActorObject->ObjectBox;

			actorBox->xPos = inputAction->ActionData.positions[0];
			actorBox->yPos = inputAction->ActionData.positions[1];
		} break;

		case SCENE_MOVE_ACTOR:
		case SCENE_MOVE_ACTOR_X:
		case SCENE_MOVE_ACTOR_Y:
		{
			if (inputAction->ActorObject == NULL || inputAction->ActorObject->ObjectBox == NULL)
			{
				break;
			}

			PhysicsRect *actorBox = inputAction->ActorObject->ObjectBox;
			double xMove = inputAction->ActionData.positions[0];
			double yMove = inputAction->ActionData.positions[1];

			if (fabs(xMove) > 0.01)
			{
				actorBox->xPos += xMove;
				actorBox->xPosRight += xMove;
			}

			if (fabs(yMove) > 0.01)
			{
				actorBox->yPos += yMove;
				actorBox->yPosTop += yMove;
			}
		} break;

		case SCENE_SET_ACTOR_DIRECTION:
		{
			double direction = inputAction->ActionData.positions[0];

			SetObjectDirection(inputAction->ActorObject, direction, ROTATE_ALL);
		} break;

		case SCENE_ROTATE_ACTOR:
		{
			double rotate = inputAction->ActionData.positions[0];

			RotateObject(inputAction->ActorObject, rotate, ROTATE_ALL);
		} break;

		case SCENE_HIDE_ACTOR:
		{
			if (inputAction->ActorObject == NULL || inputAction->ActorObject->ObjectDisplay == NULL)
			{
				break;
			}

			inputAction->ActorObject->ObjectDisplay->hidden = true;
		} break;

		case SCENE_SHOW_ACTOR:
		{
			if (inputAction->ActorObject == NULL || inputAction->ActorObject->ObjectDisplay == NULL)
			{
				break;
			}

			inputAction->ActorObject->ObjectDisplay->hidden = false;
		} break;

		case SCENE_CREATE_ACTOR:
		{
			struct ObjectMeta *loadedData = &inputAction->ActionData.objectInfo;
			Object *newActor = AddNamedObject(GameWorld, loadedData->name, loadedData->objectID, loadedData->xPos, loadedData->yPos);
			if (newActor != NULL)
			{
				newActor->State = ACTOR;
			}

			inputAction->repeatTimes = 0;
		} break;

		case SCENE_PLAY_SOUND:
			struct SoundMeta *loadedData = &inputAction->ActionData.soundData;

			Lemon_PlaySound(loadedData->soundName, loadedData->folderName, loadedData->channel, loadedData->volume);
			inputAction->repeatTimes = 0;
			break;

		case SCENE_SET_CAMERA_POS:
			GameWorld->MainCamera.CameraX = inputAction->ActionData.CameraData[0];
			GameWorld->MainCamera.CameraY = inputAction->ActionData.CameraData[1];
			break;

		case SCENE_MOVE_CAMERA:
			GameWorld->MainCamera.CameraX += inputAction->ActionData.CameraData[0];
			GameWorld->MainCamera.CameraY += inputAction->ActionData.CameraData[1];
			break;

		case SCENE_MOVE_CAMERA_SMOOTH:
		{
			float xDest = inputAction->ActionData.CameraData[0];
			float yDest = inputAction->ActionData.CameraData[1];
			float speedCoefficient = inputAction->ActionData.CameraData[2];

			float xDifference = xDest - GameWorld->MainCamera.CameraX;
			float yDifference = yDest - GameWorld->MainCamera.CameraY;

			if (fabs(xDifference) < 1.0)
			{
				GameWorld->MainCamera.CameraX = xDest;
			}
			else
			{
				GameWorld->MainCamera.CameraX += xDifference / speedCoefficient;
			}

			if (fabs(yDifference) < 1.0)
			{
				GameWorld->MainCamera.CameraY = yDest;
			}
			else
			{
				GameWorld->MainCamera.CameraY += yDifference / speedCoefficient;
			}

			if (fabs(xDifference) < 0.1 && fabs(yDifference) < 0.1)
			{
				inputAction->repeatTimes = 0;
			}
		} break;

		case SCENE_SET_SCREEN_ZOOM:
		{
			float zoomX = inputAction->ActionData.zoomScales[0];
			float zoomY = inputAction->ActionData.zoomScales[1];
			
			GameWorld->GameEvent = SET_SCREEN_ZOOM;
			GameWorld->GameEventData.zoomScales[0] = zoomX;
			GameWorld->GameEventData.zoomScales[1] = zoomY;
		} break;

		case SCENE_CHANGE_SCREEN_ZOOM:
		{
			float zoomX = inputAction->ActionData.zoomScales[0];
			float zoomY = inputAction->ActionData.zoomScales[1];

			GameWorld->GameEvent = CHANGE_SCREEN_ZOOM;
			GameWorld->GameEventData.zoomScales[0] = zoomX;
			GameWorld->GameEventData.zoomScales[1] = zoomY;
		} break;

		default:
		break;
	}


	return LEMON_SUCCESS;
}


int EndCutscene(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->CurrentCutscene == NO_CUTSCENE || GameWorld->TextQueue != NULL)
	{
		return EXECUTION_UNNECESSARY;
	}

	Object *PlayerObject = GameWorld->Player.PlayerPtr;

	if (PlayerObject != NULL && PlayerObject->State == ACTOR)
	{
		PlayerObject->State = DEFAULT;
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
			MarkObjectForDeletion(currentObject);
		}

		currentObject = currentObject->nextObject;
	}

	GameWorld->CurrentCutscene = NO_CUTSCENE;

	if (GameWorld->GameState == CUTSCENE)
	{
		GameWorld->GameState = GAMEPLAY;
		// Temporary; in future cutscenes will reset to previous cammode
		GameWorld->MainCamera.CameraMode = FOLLOW_PLAYER;
	}

	return LEMON_SUCCESS;
}


int WaitUntil(SceneAction *inputAction)
{
	if (inputAction == NULL)
	{
		return MISSING_DATA;
	}

	inputAction->parallelAction = false;

	return LEMON_SUCCESS;
}


// helper function to create a loop that that halts on the last instruction so that all instructions in the loop finish before continuing
int Repeat(int repeatCount, int instructionCount, ...)
{
	if (repeatCount < 1 || instructionCount < 1)
	{
		return EXECUTION_UNNECESSARY;
	}

	va_list args;
    va_start(args, instructionCount);

    SceneAction *instruction = NULL;

    for (int i = 0; i < instructionCount; i++)
    {
    	instruction = va_arg(args, SceneAction*);
    	
    	if (instruction == NULL)
    	{
    		continue;
    	}

    	instruction->repeatTimes = repeatCount;
    }

    instruction->parallelAction = false;

    va_end(args);

    return LEMON_SUCCESS;
}


SceneAction* Wait(float seconds, World *GameWorld)
{
	if (seconds < 0.001 || GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_WAIT, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = (int)(seconds * EngineSettings.GameTicksPerSecond);
	newAction->parallelAction = false;

	return newAction;
}


SceneAction* SceneAction_SayText(TextInstance *text, World *GameWorld)
{
	if (GameWorld == NULL || text == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_SAY_TEXT, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.sceneText = text;
	newAction->parallelAction = false;

	return newAction;
}


SceneAction* AnimateActor(char objName[], const char animName[], int loopCount, World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL || animName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH || strlen(animName) >= MAX_LEN)
	{
		return NULL;
	}

	Object *actorObj = FindObject(objName, *(GameWorld->ObjectList));
	if (actorObj == NULL)
	{
		return NULL;
	}

	int animIndex = getAnimationIndex(animName, actorObj->ObjectDisplay);
	if (animIndex < 0)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_ANIMATE_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActorObject = actorObj;
	newAction->ActionData.animationDetails[0] = animIndex;
	newAction->ActionData.animationDetails[1] = loopCount;
	newAction->repeatTimes = 2;

	return newAction;
}


SceneAction* SwitchActorSprite(char objName[], const char spriteName[], World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL || spriteName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *actorObj = FindObject(objName, *(GameWorld->ObjectList));
	if (actorObj == NULL)
	{
		return NULL;
	}

	int spriteIndex = getSpriteIndex(spriteName, actorObj->ObjectDisplay);
	if (spriteIndex < 0)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_SET_ACTOR_SPRITE, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActorObject = actorObj;
	newAction->ActionData.animationDetails[0] = spriteIndex;
	newAction->repeatTimes = 1;

	return newAction;
}


SceneAction* SetActorPosition(char objName[], double xPosition, double yPosition, World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *actorObj = FindObject(objName, *(GameWorld->ObjectList));
	if (actorObj == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_SET_ACTOR_POS, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActorObject = actorObj;
	newAction->ActionData.positions[0] = xPosition;
	newAction->ActionData.positions[1] = yPosition;
	newAction->repeatTimes = 1;

	return newAction;
}


SceneAction* MoveActor(char objName[], double xMovement, double yMovement, short repeatTimes, World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *actorObj = FindObject(objName, *(GameWorld->ObjectList));
	if (actorObj == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_MOVE_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActorObject = actorObj;
	newAction->ActionData.positions[0] = xMovement;
	newAction->ActionData.positions[1] = yMovement;
	newAction->repeatTimes = repeatTimes;

	return newAction;
}


SceneAction* MoveActorX(char objName[], double xMovement, short repeatTimes, World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *actorObj = FindObject(objName, *(GameWorld->ObjectList));
	if (actorObj == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_MOVE_ACTOR_X, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActorObject = actorObj;
	newAction->ActionData.positions[0] = xMovement;
	newAction->ActionData.positions[1] = 0.0;
	newAction->repeatTimes = repeatTimes;

	return newAction;
}


SceneAction* MoveActorY(char objName[], double yMovement, short repeatTimes, World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *actorObj = FindObject(objName, *(GameWorld->ObjectList));
	if (actorObj == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_MOVE_ACTOR_Y, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActorObject = actorObj;
	newAction->ActionData.positions[0] = yMovement;
	newAction->ActionData.positions[1] = 0.0;
	newAction->repeatTimes = repeatTimes;

	return newAction;
}


SceneAction* SetActorDirection(char objName[], double rotation, World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *actorObj = FindObject(objName, *(GameWorld->ObjectList));
	if (actorObj == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_SET_ACTOR_DIRECTION, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActorObject = actorObj;
	newAction->ActionData.positions[0] = rotation;
	newAction->ActionData.positions[1] = 0.0;

	return newAction;
}


SceneAction* RotateActor(char objName[], double rotation, short repeatTimes, World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *actorObj = FindObject(objName, *(GameWorld->ObjectList));
	if (actorObj == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_ROTATE_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActorObject = actorObj;
	newAction->ActionData.positions[0] = rotation;
	newAction->ActionData.positions[1] = 0.0;
	newAction->repeatTimes = repeatTimes;

	return newAction;
}


SceneAction* HideActor(char objName[], World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *actorObj = FindObject(objName, *(GameWorld->ObjectList));
	if (actorObj == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_HIDE_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	return newAction;
}


SceneAction* ShowActor(char objName[], World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	Object *actorObj = FindObject(objName, *(GameWorld->ObjectList));
	if (actorObj == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_SHOW_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	return newAction;
}


SceneAction* CreateActor(char objName[], ObjectType actorID, int xPos, int yPos, World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_CREATE_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.objectInfo.name, objName);
	newAction->ActionData.objectInfo.objectID = actorID;
	newAction->ActionData.objectInfo.xPos = xPos;
	newAction->ActionData.objectInfo.xPos = yPos;

	return newAction;
}


SceneAction* SceneAction_PlaySound(char soundName[], char folderName[], ChannelName soundChannel, float volume, World *GameWorld)
{
	if (GameWorld == NULL || soundName == NULL || folderName == NULL)
	{
		return NULL;
	}

	if (strlen(soundName) >= MAX_LEN || strlen(folderName) >= MAX_LEN)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_PLAY_SOUND, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.soundData.soundName, soundName);
	strcpy(newAction->ActionData.soundData.folderName, folderName);
	newAction->ActionData.soundData.channel = soundChannel;
	newAction->ActionData.soundData.volume = volume;
	newAction->parallelAction = true;
	newAction->repeatTimes = 1;

	return newAction;
}


SceneAction* SceneAction_SetCameraPosition(float xPos, float yPos, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_SET_CAMERA_POS, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.CameraData[0] = xPos;
	newAction->ActionData.CameraData[1] = yPos;

	return newAction;
}


SceneAction* SceneAction_MoveCamera(float xVel, float yVel, int repeatTimes, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_MOVE_CAMERA, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.CameraData[0] = xVel;
	newAction->ActionData.CameraData[1] = yVel;
	newAction->repeatTimes = repeatTimes;

	return newAction;
}


SceneAction* SceneAction_MoveCameraSmooth(float xPos, float yPos, float coefficient, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_MOVE_CAMERA_SMOOTH, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.CameraData[0] = xPos;
	newAction->ActionData.CameraData[1] = yPos;
	newAction->ActionData.CameraData[2] = coefficient;
	newAction->repeatTimes = 999;

	return newAction;
}


SceneAction* SceneAction_SetZoom(float zoomX, float zoomY, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_SET_SCREEN_ZOOM, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.zoomScales[0] = zoomX;
	newAction->ActionData.zoomScales[1] = zoomY;

	return newAction;
}


SceneAction* SceneAction_ChangeZoom(float zoomX, float zoomY, int repeatTimes, World *GameWorld)
{
	if (GameWorld == NULL || repeatTimes < 1)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_CHANGE_SCREEN_ZOOM, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.zoomScales[0] = zoomX;
	newAction->ActionData.zoomScales[1] = zoomY;
	newAction->repeatTimes = repeatTimes;

	return newAction;
}


SceneAction* createSceneAction(SceneActionID newActionID, World *GameWorld)
{
	if (GameWorld == NULL || newActionID >= UNDEFINED_SCENE_ACTION || newActionID < 0)
	{
		return NULL;
	}

	SceneAction *newAction = malloc(sizeof(SceneAction));
	memset(newAction, 0, sizeof(SceneAction));

	if (newAction == NULL)
	{
		return NULL;
	}

	if (GameWorld->SceneActionList == NULL)
	{
		GameWorld->SceneActionList = newAction;
		newAction->prevSceneAction = NULL;
	}
	else
	{
		SceneAction *actionPtr = GameWorld->SceneActionList;

		while (actionPtr->nextSceneAction != NULL)
		{
			actionPtr = actionPtr->nextSceneAction;
		}

		actionPtr->nextSceneAction = newAction;
		newAction->prevSceneAction = actionPtr;
	}

	newAction->nextSceneAction = NULL;
	newAction->ActorObject = NULL;

	newAction->ActionID = newActionID;
	newAction->parallelAction = true;
	newAction->repeatTimes = 1;

	return newAction;
}


SceneAction* deleteSceneAction(SceneAction *deleteAction, World *GameWorld)
{
	if (deleteAction == NULL || GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *prevAction = deleteAction->prevSceneAction;
	SceneAction *nextAction = deleteAction->nextSceneAction;

	if (prevAction != NULL)
	{
		prevAction->nextSceneAction = nextAction;
	}
	else
	{
		GameWorld->SceneActionList = nextAction;
	}

	if (nextAction != NULL)
	{
		nextAction->prevSceneAction = prevAction;
	}


	free(deleteAction);


	return nextAction;
}


int deleteAllSceneActions(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	while (GameWorld->SceneActionList != NULL)
	{
		deleteSceneAction(GameWorld->SceneActionList, GameWorld);
	}

	return LEMON_SUCCESS;
}