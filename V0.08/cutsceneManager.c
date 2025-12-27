#include "cutsceneManager.h"
#define END_SCENE_HERE createSceneAction(SCENE_END, GameWorld)


// global variables for cutscenes
static int flags[16] = {0};


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

	deleteAllSceneActions(GameWorld);
	clearTextQueue(GameWorld);		
	// I have to do this terribleness because deleting the text associated with a scene action when the
	// scene action is deleted was too much of a hassle and could cause bad pointer behaviour;
	// was easier to just clear the whole queue 
	// Ideally this shouldn't matter, but if for some reason theres a text box you want to persist across a cutscene 
	// you cannot because it gets deleted here


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
	
		SayTextOption("Play new cutscene?", NO_PORTRAIT, BASIC_FADE, GameWorld, 3, 
				"Test scene", 	&StartCutscene, TEST_SCENE, 
				"Test scene 2", &StartCutscene, TEST_SCENE_2,
				"No", NULL, NULL);
		break;

	case TEST_SCENE_2:
		// always happens at the start
		flags[0]++;

		ifGreaterThan(&flags[0], 1, TEST_SCENE_2_AGAIN, GameWorld);
		SayText("A small tomato is really just a \ncherry.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		break;

	case TEST_SCENE_2_AGAIN:
		SayText("Wait a second..... \nyou've been here before.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		SayTextOption("Do you remember the thing about the cherries?", NO_PORTRAIT, BASIC_FADE, GameWorld, 3, 
				"No", 	NULL, NULL, 
				"Yes, they're ugly", &StartCutscene, TEST_SCENE_2_WRONG,
				"Yes, they're small tomatoes", &StartCutscene, TEST_SCENE_2_CORRECT);
		SayText("Oh... ok nevermind then.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		break;

	case TEST_SCENE_2_CORRECT:
		SayText("You're right! Nice job.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		break;

	case TEST_SCENE_2_WRONG:
		SayText("You're wrong! hmmmm....", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		SayText("Get lost.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		SetActorPosition("MainPlayer", 100000.0, 96.0, GameWorld);
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
	

	if (GameWorld->TextQueue != NULL)
	{
		//printf("\n%s", GameWorld->TextQueue->textPhrase);
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
		if (RunSceneAction(ActionPtr, GameWorld) == DATA_CLEARED)
		{
			return EXECUTION_UNNECESSARY;
		}

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

		case SCENE_IF_EQUALS:
		{
			struct ifIntData *branchData = &inputAction->ActionData.branchDataInt;

			if (branchData == NULL || branchData->variable == NULL)
			{
				break;
			}

			if (*(branchData->variable) == branchData->comparisonValue)
			{
				StartCutscene(branchData->ifTrue, GameWorld);
				return DATA_CLEARED;
			}
			else if (branchData->elseBranchPresent)
			{
				StartCutscene(branchData->ifFalse, GameWorld);
				return DATA_CLEARED;
			}

			// returning 'DATA_CLEARED' is neccessary here to avoid incorrect pointer access
		} break;

		case SCENE_IF_NOT_EQUALS:
		{
			struct ifIntData *branchData = &inputAction->ActionData.branchDataInt;

			if (branchData == NULL || branchData->variable == NULL)
			{
				break;
			}

			if (*(branchData->variable) != branchData->comparisonValue)
			{
				StartCutscene(branchData->ifTrue, GameWorld);
				return DATA_CLEARED;
			}
			else if (branchData->elseBranchPresent)
			{
				StartCutscene(branchData->ifFalse, GameWorld);
				return DATA_CLEARED;
			}

			// returning 'DATA_CLEARED' is neccessary here to avoid incorrect pointer access
		} break;

		case SCENE_IF_LESS_THAN:
		{
			struct ifIntData *branchData = &inputAction->ActionData.branchDataInt;

			if (branchData == NULL || branchData->variable == NULL)
			{
				break;
			}

			if (*(branchData->variable) < branchData->comparisonValue)
			{
				StartCutscene(branchData->ifTrue, GameWorld);
				return DATA_CLEARED;
			}
			else if (branchData->elseBranchPresent)
			{
				StartCutscene(branchData->ifFalse, GameWorld);
				return DATA_CLEARED;
			}
		} break;

		case SCENE_IF_GREATER_THAN:
		{
			struct ifIntData *branchData = &inputAction->ActionData.branchDataInt;

			if (branchData == NULL || branchData->variable == NULL)
			{
				break;
			}

			if (*(branchData->variable) > branchData->comparisonValue)
			{
				StartCutscene(branchData->ifTrue, GameWorld);
				return DATA_CLEARED;
			}
			else if (branchData->elseBranchPresent)
			{
				StartCutscene(branchData->ifFalse, GameWorld);
				return DATA_CLEARED;
			}
		} break;

		case SCENE_FLOAT_IF_EQUALS:
		{
			struct ifFloatData *branchData = &inputAction->ActionData.branchDataFloat;

			if (branchData == NULL || branchData->variable == NULL)
			{
				break;
			}

			if (fabs(*(branchData->variable) - branchData->comparisonValue) < 0.0001)
			{
				StartCutscene(branchData->ifTrue, GameWorld);
				return DATA_CLEARED;
			}
			else if (branchData->elseBranchPresent)
			{
				StartCutscene(branchData->ifFalse, GameWorld);
				return DATA_CLEARED;
			}

			// returning 'DATA_CLEARED' is neccessary here to avoid incorrect pointer access
		} break;

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
			inputAction->repeatTimes = 0;
			if (inputAction->ActorObject == NULL || inputAction->ActorObject->ObjectDisplay == NULL)
			{
				break;
			}
			
			inputAction->ActorObject->State = ACTOR;
			inputAction->ActorObject->ObjectDisplay->RenderModeOverride = DEFAULT_TO_SPRITE;
		} break;

		case SCENE_SET_ACTOR_LAYER:
		{
			inputAction->repeatTimes = 0;
			printf("Layer: %d", inputAction->ActionData.layer);

			if (inputAction->ActorObject == NULL)
			{
				break;
			}

			inputAction->ActorObject->layer = inputAction->ActionData.layer;
		} break;

		case SCENE_PLAY_SOUND:
		{
			struct SoundMeta *loadedData = &inputAction->ActionData.soundData;

			Lemon_PlaySound(loadedData->soundName, loadedData->folderName, loadedData->channel, loadedData->volume);
			inputAction->repeatTimes = 0;
		} break;

		case SCENE_PLAY_SOUND_REPEAT:
		{
			struct SoundMeta *loadedData = &inputAction->ActionData.soundData;

			RepeatSound(Lemon_PlaySound(loadedData->soundName, loadedData->folderName, loadedData->channel, loadedData->volume), inputAction->repeatTimes);
			inputAction->repeatTimes = 0;
		} break;

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

		case SCENE_SET_CHANNEL_VOL:
		{
			SetChannelVolume(inputAction->ActionData.soundData.channel, inputAction->ActionData.soundData.volume);
		} break;

		case SCENE_FADE_CHANNEL_VOL:
		{
			ChangeChannelVolume(inputAction->ActionData.soundData.channel, inputAction->ActionData.soundData.volume);
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


SceneAction* ifEquals(int* variable, int value, CutsceneID cutsceneToTrigger, World *GameWorld)
{
	if (variable == NULL || GameWorld == NULL || cutsceneToTrigger <= NO_CUTSCENE || cutsceneToTrigger >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_IF_EQUALS, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;	// in case it doesnt immediately delete, it wont execute potentially incorrect actions after itself
	newAction->ActionData.branchDataInt.elseBranchPresent = false;
	newAction->ActionData.branchDataInt.ifTrue = cutsceneToTrigger;
	newAction->ActionData.branchDataInt.ifFalse = NO_CUTSCENE;
	newAction->ActionData.branchDataInt.variable = variable;
	newAction->ActionData.branchDataInt.comparisonValue = value;

	return newAction;
}


SceneAction* ifEqualsElse(int* variable, int value, CutsceneID cutsceneIfTrue, CutsceneID cutsceneIfElse, World *GameWorld)
{
	if (variable == NULL || GameWorld == NULL || cutsceneIfElse <= NO_CUTSCENE || cutsceneIfElse >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_IF_EQUALS, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;	// in case it doesnt immediately delete, it wont execute potentially incorrect actions after itself
	newAction->ActionData.branchDataInt.elseBranchPresent = true;
	newAction->ActionData.branchDataInt.ifTrue = cutsceneIfTrue;
	newAction->ActionData.branchDataInt.ifFalse = cutsceneIfElse;
	newAction->ActionData.branchDataInt.variable = variable;
	newAction->ActionData.branchDataInt.comparisonValue = value;

	return newAction;
}


SceneAction* ifNotEquals(int* variable, int value, CutsceneID cutsceneToTrigger, World *GameWorld)
{
	if (variable == NULL || GameWorld == NULL || cutsceneToTrigger <= NO_CUTSCENE || cutsceneToTrigger >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_IF_NOT_EQUALS, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;	// in case it doesnt immediately delete, it wont execute potentially incorrect actions after itself
	newAction->ActionData.branchDataInt.elseBranchPresent = false;
	newAction->ActionData.branchDataInt.ifTrue = cutsceneToTrigger;
	newAction->ActionData.branchDataInt.ifFalse = NO_CUTSCENE;
	newAction->ActionData.branchDataInt.variable = variable;
	newAction->ActionData.branchDataInt.comparisonValue = value;

	return newAction;
}


SceneAction* ifNotEqualsElse(int* variable, int value, CutsceneID cutsceneIfTrue, CutsceneID cutsceneIfElse, World *GameWorld)
{
	if (variable == NULL || GameWorld == NULL || cutsceneIfElse <= NO_CUTSCENE || cutsceneIfElse >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_IF_NOT_EQUALS, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;	// in case it doesnt immediately delete, it wont execute potentially incorrect actions after itself
	newAction->ActionData.branchDataInt.elseBranchPresent = true;
	newAction->ActionData.branchDataInt.ifTrue = cutsceneIfTrue;
	newAction->ActionData.branchDataInt.ifFalse = cutsceneIfElse;
	newAction->ActionData.branchDataInt.variable = variable;
	newAction->ActionData.branchDataInt.comparisonValue = value;

	return newAction;
}


SceneAction* ifLessThan(int* variable, int value, CutsceneID cutsceneToTrigger, World *GameWorld)
{
	if (variable == NULL || GameWorld == NULL || cutsceneToTrigger <= NO_CUTSCENE || cutsceneToTrigger >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_IF_LESS_THAN, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;	// in case it doesnt immediately delete, it wont execute potentially incorrect actions after itself
	newAction->ActionData.branchDataInt.elseBranchPresent = false;
	newAction->ActionData.branchDataInt.ifTrue = cutsceneToTrigger;
	newAction->ActionData.branchDataInt.ifFalse = NO_CUTSCENE;
	newAction->ActionData.branchDataInt.variable = variable;
	newAction->ActionData.branchDataInt.comparisonValue = value;

	return newAction;
}


SceneAction* ifLessThanElse(int* variable, int value, CutsceneID cutsceneIfTrue, CutsceneID cutsceneIfElse, World *GameWorld)
{
	if (variable == NULL || GameWorld == NULL || cutsceneIfElse <= NO_CUTSCENE || cutsceneIfElse >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_IF_LESS_THAN, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;	// in case it doesnt immediately delete, it wont execute potentially incorrect actions after itself
	newAction->ActionData.branchDataInt.elseBranchPresent = true;
	newAction->ActionData.branchDataInt.ifTrue = cutsceneIfTrue;
	newAction->ActionData.branchDataInt.ifFalse = cutsceneIfElse;
	newAction->ActionData.branchDataInt.variable = variable;
	newAction->ActionData.branchDataInt.comparisonValue = value;

	return newAction;
}


SceneAction* ifGreaterThan(int* variable, int value, CutsceneID cutsceneToTrigger, World *GameWorld)
{
	if (variable == NULL || GameWorld == NULL || cutsceneToTrigger <= NO_CUTSCENE || cutsceneToTrigger >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_IF_GREATER_THAN, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;	// in case it doesnt immediately delete, it wont execute potentially incorrect actions after itself
	newAction->ActionData.branchDataInt.elseBranchPresent = false;
	newAction->ActionData.branchDataInt.ifTrue = cutsceneToTrigger;
	newAction->ActionData.branchDataInt.ifFalse = NO_CUTSCENE;
	newAction->ActionData.branchDataInt.variable = variable;
	newAction->ActionData.branchDataInt.comparisonValue = value;

	return newAction;
}


SceneAction* ifGreaterThanElse(int* variable, int value, CutsceneID cutsceneIfTrue, CutsceneID cutsceneIfElse, World *GameWorld)
{
	if (variable == NULL || GameWorld == NULL || cutsceneIfElse <= NO_CUTSCENE || cutsceneIfElse >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_IF_GREATER_THAN, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;	// in case it doesnt immediately delete, it wont execute potentially incorrect actions after itself
	newAction->ActionData.branchDataInt.elseBranchPresent = true;
	newAction->ActionData.branchDataInt.ifTrue = cutsceneIfTrue;
	newAction->ActionData.branchDataInt.ifFalse = cutsceneIfElse;
	newAction->ActionData.branchDataInt.variable = variable;
	newAction->ActionData.branchDataInt.comparisonValue = value;

	return newAction;
}


SceneAction* ifFloatEquals(float* variable, float value, CutsceneID cutsceneToTrigger, World *GameWorld)
{
	if (variable == NULL || GameWorld == NULL || cutsceneToTrigger <= NO_CUTSCENE || cutsceneToTrigger >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_FLOAT_IF_EQUALS, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;	// in case it doesnt immediately delete, it wont execute potentially incorrect actions after itself
	newAction->ActionData.branchDataFloat.elseBranchPresent = false;
	newAction->ActionData.branchDataFloat.ifTrue = cutsceneToTrigger;
	newAction->ActionData.branchDataFloat.ifFalse = NO_CUTSCENE;
	newAction->ActionData.branchDataFloat.variable = variable;
	newAction->ActionData.branchDataFloat.comparisonValue = value;

	return newAction;
}


SceneAction* ifFloatEqualsElse(float* variable, float value, CutsceneID cutsceneIfTrue, CutsceneID cutsceneIfElse, World *GameWorld)
{
	if (variable == NULL || GameWorld == NULL || cutsceneIfElse <= NO_CUTSCENE || cutsceneIfElse >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_FLOAT_IF_EQUALS, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;	// in case it doesnt immediately delete, it wont execute potentially incorrect actions after itself
	newAction->ActionData.branchDataFloat.elseBranchPresent = true;
	newAction->ActionData.branchDataFloat.ifTrue = cutsceneIfTrue;
	newAction->ActionData.branchDataFloat.ifFalse = cutsceneIfElse;
	newAction->ActionData.branchDataFloat.variable = variable;
	newAction->ActionData.branchDataFloat.comparisonValue = value;

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


SceneAction* SetActorPosition(char objName[], float xPosition, float yPosition, World *GameWorld)
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


SceneAction* MoveActor(char objName[], float xMovement, float yMovement, short repeatTimes, World *GameWorld)
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


SceneAction* MoveActorX(char objName[], float xMovement, short repeatTimes, World *GameWorld)
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


SceneAction* MoveActorY(char objName[], float yMovement, short repeatTimes, World *GameWorld)
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

	newAction->ActorObject = actorObj;

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

	newAction->ActorObject = actorObj;

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

	newAction->ActorObject = AddNamedObject(GameWorld, objName, actorID, xPos, yPos);
	if (newAction->ActorObject != NULL)
	{
		newAction->ActorObject->State = STATIC;
		newAction->ActorObject->ObjectDisplay->RenderModeOverride = DO_NOT_RENDER;
	}

	return newAction;
}


SceneAction* SetActorLayer(char objName[], Layer destLayer, World *GameWorld)
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

	SceneAction *newAction = createSceneAction(SCENE_SET_ACTOR_LAYER, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.layer = destLayer;
	newAction->ActorObject = actorObj;

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


SceneAction* SceneAction_PlaySoundRepeat(char soundName[], char folderName[], ChannelName soundChannel, float volume, int repeatTimes, World *GameWorld)
{
	if (GameWorld == NULL || soundName == NULL || folderName == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = SceneAction_PlaySound(soundName, folderName, soundChannel, volume, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = repeatTimes;

	return newAction;
}


SceneAction* SceneAction_SetSoundChannelVolume(ChannelName soundChannel, float newVolume, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_SET_CHANNEL_VOL, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.soundData.soundName, "noSound");
	strcpy(newAction->ActionData.soundData.folderName, "noFolder");
	newAction->ActionData.soundData.channel = soundChannel;
	newAction->ActionData.soundData.volume = newVolume;
	newAction->parallelAction = true;
	newAction->repeatTimes = 1;

	return newAction;
}

SceneAction* SceneAction_FadeSoundChannel(ChannelName soundChannel, float fadeValue, int repeatTimes, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_FADE_CHANNEL_VOL, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.soundData.soundName, "noSound");
	strcpy(newAction->ActionData.soundData.folderName, "noFolder");
	newAction->ActionData.soundData.channel = soundChannel;
	newAction->ActionData.soundData.volume = fadeValue;
	newAction->parallelAction = true;
	newAction->repeatTimes = repeatTimes;

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