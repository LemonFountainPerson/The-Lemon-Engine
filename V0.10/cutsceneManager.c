#include "LemonEngine.h"
#define END_SCENE_HERE createSceneAction(SCENE_END, GameWorld)


void prepareCutsceneEnvironment(World *GameWorld)
{
	// I have to do this terribleness because deleting the text associated with a scene action when the
	// scene action is deleted was too much of a hassle and could cause bad pointer behaviour;
	// was easier to just clear the whole queue 
	// Ideally this shouldn't matter, but if for some reason theres a text box you want to persist across a cutscene 
	// you cannot because it gets deleted here
	deleteAllSceneActions(GameWorld);
	clearTextQueue(GameWorld);		

	GameWorld->GameState = CUTSCENE;

	return;
}

int initialiseCutscene(CutsceneID inputID, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (inputID <= NO_CUTSCENE || inputID >= UNDEFINED_CUTSCENE || !(GameWorld->GameState == GAMEPLAY || GameWorld->GameState == CUTSCENE))
	{
		return INVALID_DATA;
	}

	putConsoleStringTS("Starting Cutscene... ID: %d", inputID);

	prepareCutsceneEnvironment(GameWorld);
	GameWorld->CurrentCutscene = inputID;


	// Set-up cutscene
	switch (inputID)
	{
	case TEST_SCENE:
		GameWorld->MainCamera.CameraMode = FREE_ROAM_RESTRICTED;

		SayText("A test cutscene, huh?", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		SayText("How fantastic.", NO_PORTRAIT, COMIC_TEXT, GameWorld);
	
		SayTextOption("Play new cutscene?", NO_PORTRAIT, BASIC_FADE, GameWorld, 3, 
					"Test scene", 	playCutscene(TEST_SCENE, GameWorld), 
					"Test scene 2", playCutscene(TEST_SCENE_2, GameWorld),
					"No", NO_ACTION);
		break;

	case TEST_SCENE_2:
		GameFlags[0].value++;

		ifElse(ifGreaterThan(0, 1, TEST_SCENE_2_AGAIN, GameWorld), NO_CUTSCENE);	// equivalent to 'ifGreaterThan(0, 1, TEST_SCENE_2_AGAIN, GameWorld)'
		SayText("A small tomato is really just a cherry.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		break;

	case TEST_SCENE_2_AGAIN:
		SayText("Wait a second..... \nyou've been here before.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		SayTextOption("Do you remember the thing about the cherries?", NO_PORTRAIT, BASIC_FADE, GameWorld, 3, 
				"No", 	NO_ACTION, 
				"Yes, they're ugly", playCutscene(TEST_SCENE_2_WRONG, GameWorld),
				"Yes, they're small tomatoes", playCutscene(TEST_SCENE_2_CORRECT, GameWorld));
		SayText("Oh... ok nevermind then.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		break;

	case TEST_SCENE_2_CORRECT:
		SayText("You're right! Nice job.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		break;

	case TEST_SCENE_2_WRONG:
		SayText("You're wrong! hmmmm....", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		SayText("Get lost.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		break;

	default:
		char fileName[CUTSCENE_FILE_NAME_MAX] = {0};
		snprintf(fileName, CUTSCENE_FILE_NAME_MAX, "Scene%d", inputID);

		if (LoadCutsceneFromFile(fileName, GameWorld) != LEMON_SUCCESS)
		{
			return LEMON_ERROR;
		}

		GameWorld->CurrentCutscene = inputID;
		break;
	}

	if (GameWorld->GameState == CUTSCENE)
	{
		if (GameWorld->Player.PlayerPtr != NULL)
		{
			GameWorld->Player.PlayerPtr->State = ACTOR_STATE;
		}
	}

	return LEMON_SUCCESS;
}

int initialiseCutsceneFromFile(const char sceneName[], World *GameWorld)
{
	putConsoleString("Starting Cutscene... Name: %s", sceneName);

	prepareCutsceneEnvironment(GameWorld);

	GameWorld->CurrentCutscene = CUTSCENE_FROM_FILE;

	return LoadCutsceneFromFile(sceneName, GameWorld);
}



int LoadCutsceneFromFile(const char sceneName[], World *GameWorld)
{
	if (!(GameWorld->GameState == GAMEPLAY || GameWorld->GameState == CUTSCENE))
	{
		return INVALID_DATA;
	}

	FILE *fPtr = openFile(sceneName, CUTSCENE_ROOT, "--CUTSCENE_DATA--");

	if (fPtr == NULL)
	{
		return LEMON_ERROR;
	}

	char readString[MAX_LEN] = {0};
	char textBoxString[MAX_TEXT_LENGTH] = {0};

	while (!endOfFile(fPtr))
	{
		getNextArg(fPtr, readString, MAX_LEN);
		
		loadSceneAction(readString, textBoxString, GameWorld, fPtr);
	}

	closeFile(fPtr);

	if (GameWorld->GameState != CUTSCENE)
	{
		if (GameWorld->Player.PlayerPtr != NULL)
		{
			GameWorld->Player.PlayerPtr->State = ACTOR_STATE;
		}
	}

	return LEMON_SUCCESS;
}


int UpdateCutscene(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if ((GameWorld->SceneActionQueue == NULL && GameWorld->CurrentCutscene == NO_CUTSCENE) || GameWorld->GamePaused == 1)
	{
		return EXECUTION_UNNECESSARY;
	}

	// Play cutscene
	updateSceneActions(GameWorld);

	if (GameWorld->SceneActionQueue == NULL || GameWorld->CurrentCutscene == END_CUTSCENE)
	{
		EndCutscene(GameWorld);
	}
	

	return LEMON_SUCCESS;
}


int updateSceneActions(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->SceneActionQueue == NULL)
	{
		return MISSING_DATA;
	}

	SceneAction *ActionPtr = GameWorld->SceneActionQueue;
	int i = EngineSettings.MaxSceneActions;

	while (ActionPtr != NULL && i >= 0)
	{
		RunSceneAction(ActionPtr, GameWorld);
		if (GameWorld->SceneActionQueue == NULL)
		{
			return DATA_CLEARED;
		}

		if (ActionPtr->repeatTimes < 1)
		{
			ActionPtr = deleteSceneAction(ActionPtr, GameWorld);
		}
		else if (ActionPtr->parallelAction == false)
		{
			return LEMON_SUCCESS;
		}
		else
		{
			ActionPtr = ActionPtr->nextSceneAction;
		}

		i--;
	}


	return LEMON_SUCCESS;
}


int RunSceneAction(SceneAction *inputAction, World *GameWorld)
{
	if (GameWorld == NULL || inputAction == NULL)
	{
		return MISSING_DATA;
	}

	if (inputAction->ActionID == UNDEFINED_SCENE_ACTION)
	{
		return INVALID_DATA;
	}

	inputAction->repeatTimes--;

	union SceneActionArguments currentData = inputAction->ActionData;

	if (inputAction->ActorObject != NULL && inputAction->ActorObject->State == EMPTY_OBJECT)
	{
		return MISSING_DATA;
	}

	switch (inputAction->ActionID)
	{
	case SCENE_SAY_TEXT:
		inputAction->repeatTimes = 1;
		break;

	case SCENE_END:
		GameWorld->CurrentCutscene = END_CUTSCENE;
		break;

	case SCENE_TRIGGER_GAME_EVENT:
		{
			triggerGameEvent(&currentData.TriggerEvent, GameWorld);
		} break;

	case SCENE_DISABLE_PLAYER:
		if (GameWorld->Player.PlayerPtr != NULL)
		{
			GameWorld->Player.PlayerPtr->State = ACTOR_STATE;
		}
		break;

	case SCENE_ENABLE_PLAYER:
		if (GameWorld->Player.PlayerPtr != NULL)
		{
			GameWorld->Player.PlayerPtr->State = DEFAULT_STATE;
		}
		break;

	case SCENE_CHANGE_VARIABLE_BY:
		{
			GameFlags[currentData.variableArgs[0]].value += currentData.variableArgs[1];
		} break;

	case SCENE_SET_VARIABLE_TO:
		{
			GameFlags[currentData.variableArgs[0]].value = currentData.variableArgs[1];
		} break;

	case SCENE_IF_EQUALS:
		{
			SceneBranchData branchData = currentData.branchData;

			if (GameFlags[branchData.variableIndex].value == branchData.comparisonValue)
			{
				if (branchData.ifTrueString[0] != 0)
				{
					playCutsceneFromFile(branchData.ifTrueString, GameWorld);
				}	
				else
				{
					playCutscene(branchData.ifTrue, GameWorld);
				}
			}
			else if (branchData.elseBranchPresent)
			{
				if (branchData.ifFalseString[0] != 0)
				{
					playCutsceneFromFile(branchData.ifFalseString, GameWorld);
				}	
				else
				{
					playCutscene(branchData.ifFalse, GameWorld);
				}
			}
		} break;

	case SCENE_IF_LESS_THAN:
		{
			SceneBranchData branchData = currentData.branchData;

			if (GameFlags[branchData.variableIndex].value < branchData.comparisonValue)
			{
				if (branchData.ifTrueString[0] != 0)
				{
					playCutsceneFromFile(branchData.ifTrueString, GameWorld);
				}	
				else
				{
					playCutscene(branchData.ifTrue, GameWorld);
				}
			}
			else if (branchData.elseBranchPresent)
			{
				if (branchData.ifFalseString[0] != 0)
				{
					playCutsceneFromFile(branchData.ifFalseString, GameWorld);
				}	
				else
				{
					playCutscene(branchData.ifFalse, GameWorld);
				}
			}
		} break;

	case SCENE_IF_GREATER_THAN:
		{
			SceneBranchData branchData = currentData.branchData;
	
			if (GameFlags[branchData.variableIndex].value > branchData.comparisonValue)
			{
				if (branchData.ifTrueString[0] != 0)
				{
					playCutsceneFromFile(branchData.ifTrueString, GameWorld);
				}	
				else
				{
					playCutscene(branchData.ifTrue, GameWorld);
				}
			}
			else if (branchData.elseBranchPresent)
			{
				if (branchData.ifFalseString[0] != 0)
				{
					playCutsceneFromFile(branchData.ifFalseString, GameWorld);
				}	
				else
				{
					playCutscene(branchData.ifFalse, GameWorld);
				}
			}
		} break;


	case SCENE_ANIMATE_ACTOR:
		{
			if (inputAction->ActorObject == NULL)
			{
				break;
			}

			DisplayData *actorDisplay = getDisplay(inputAction->ActorObject);
			int animID = currentData.animationDetails[0];
			int loopCount = currentData.animationDetails[1];

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
			if (inputAction->ActorObject == NULL)
			{
				break;
			}

			DisplayData *actorDisplay = getDisplay(inputAction->ActorObject);
			int spriteID = currentData.animationDetails[0];

			actorDisplay->currentAnimation = 0;
			switchSprite(spriteID, USE_CURRENT_SPRITESET, actorDisplay);
		} break;

	case SCENE_SET_ACTOR_POS:
		{
			GoTo(inputAction->ActorObject, currentData.positions[0], currentData.positions[1]);
		} break;

	case SCENE_MOVE_ACTOR:
	case SCENE_MOVE_ACTOR_X:
	case SCENE_MOVE_ACTOR_Y:
		{
			if (inputAction->ActorObject == NULL || inputAction->ActorObject->ObjectBox == NULL)
			{
				break;
			}

			PhysicsBox *actorBox = inputAction->ActorObject->ObjectBox;
			float xMove = currentData.positions[0];
			float yMove = currentData.positions[1];

			if (fabs(xMove) > 0.01)
			{
				actorBox->xPos += xMove;
			}

			if (fabs(yMove) > 0.01)
			{
				actorBox->yPos += yMove;
			}
		} break;

	case SCENE_SET_ACTOR_DIRECTION:
		{
			double direction = currentData.positions[0];

			SetObjectDirection(inputAction->ActorObject, direction);
		} break;

	case SCENE_ROTATE_ACTOR:
		{
			double rotate = currentData.positions[0];

			RotateObject(inputAction->ActorObject, rotate);
		} break;

	case SCENE_HIDE_ACTOR:
		{
			if (inputAction->ActorObject == NULL)
			{
				break;
			}

			hideObject(inputAction->ActorObject);
		} break;

	case SCENE_SHOW_ACTOR:
		{
			if (inputAction->ActorObject == NULL)
			{
				break;
			}

			showObject(inputAction->ActorObject);
		} break;

	case SCENE_SET_ACTOR_LAYER:
		{
			inputAction->repeatTimes = 0;

			if (inputAction->ActorObject == NULL)
			{
				break;
			}

			setDisplayLayer(inputAction->ActorObject, currentData.layer);
		} break;

	case SCENE_CREATE_ACTOR:
		{
			inputAction->repeatTimes = 0;
			if (inputAction->ActorObject == NULL)
			{
				break;
			}

			Object *actor = inputAction->ActorObject;
			actor->State = ACTOR_STATE;
			showObject(actor);
			GoTo(actor, currentData.positions[0], currentData.positions[1]);
		} break;

	case SCENE_RELEASE_ACTOR:
		{
			inputAction->repeatTimes = 0;
			putConsoleStringTS("Hello");
			if (inputAction->ActorObject != NULL && inputAction->ActorObject->State == ACTOR_STATE)
			{
				putConsoleStringTS("Hello again");
				inputAction->ActorObject->State = DEFAULT_STATE;
			}
		} break;

	case SCENE_PLACE_INVISIBLE_WALL:
		{
			Object *wall = AddObject(GameWorld, SOLID_BLOCK, currentData.invisWall[0], currentData.invisWall[1], currentData.invisWall[2], currentData.invisWall[3], -1, 0, 0);
			setObjectName(wall,	"InvisibleWall");
			wall->State = ACTOR_STATE;
		} break;

	case SCENE_PLAY_SOUND:
		{
			RepeatSound(PlaySound(
					currentData.soundData.soundName, currentData.soundData.folderName, 
					currentData.soundData.channel, currentData.soundData.volume), 
			inputAction->repeatTimes + 1);
			inputAction->repeatTimes = 0;
		} break;

	case SCENE_SET_CAMERA_POS:
			GameWorld->MainCamera.CameraX = currentData.CameraData[0];
			GameWorld->MainCamera.CameraY = currentData.CameraData[1];
			break;

	case SCENE_MOVE_CAMERA:
			GameWorld->MainCamera.CameraX += currentData.CameraData[0];
			GameWorld->MainCamera.CameraY += currentData.CameraData[1];
			break;

	case SCENE_MOVE_CAMERA_SMOOTH:
		{
			float xDest = currentData.CameraData[0];
			float yDest = currentData.CameraData[1];
			float speedCoefficient = currentData.CameraData[2];

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

	case SCENE_SET_CAMERA_ZOOM:
		{
			GameWorld->MainCamera.zoomX = currentData.zoomScales[0];
			GameWorld->MainCamera.zoomY = currentData.zoomScales[1];
		} break;

	case SCENE_CHANGE_CAMERA_ZOOM:
		{
			GameWorld->MainCamera.zoomX += currentData.zoomScales[0];
			GameWorld->MainCamera.zoomY += currentData.zoomScales[1];
		} break;

	case SCENE_SET_CHANNEL_VOL:
		{
			SetChannelVolume(currentData.soundData.channel, currentData.soundData.volume);
		} break;

	case SCENE_CHANGE_CHANNEL_VOL:
		{
			ChangeChannelVolume(currentData.soundData.channel, currentData.soundData.volume);
		} break;

	default:
		break;
	}


	return LEMON_SUCCESS;
}


SceneAction* loadSceneAction(char inputString[MAX_LEN], char textBoxString[MAX_LEN], World *GameWorld, FILE *fPtr)
{
	stringToUpper(inputString);

	if (strcmp(inputString, "SAYTEXT:") == 0)
	{
		getNextArg(fPtr, textBoxString, MAX_TEXT_LENGTH);
		getNextArg(fPtr, inputString, MAX_LEN);
		SayText(textBoxString, inputString, getNextArgInt(fPtr), GameWorld);
	}
	if (strcmp(inputString, "SAYTEXTOPTION:") == 0)
	{
		getNextArg(fPtr, textBoxString, MAX_TEXT_LENGTH);
		getNextArg(fPtr, inputString, MAX_LEN);
		int Preset = getNextArgInt(fPtr);
		int numberOfOptions = getNextArgInt(fPtr);

		char options[MAX_TEXT_OPTIONS][OPTION_TEXT_MAX_LEN] = {0};
		int sceneID[MAX_TEXT_OPTIONS] = {0};

		for (int i = 0; i < numberOfOptions && i < MAX_TEXT_OPTIONS; i++)
		{
			getNextArg(fPtr, options[i], OPTION_TEXT_MAX_LEN);
			sceneID[i] = getNextArgInt(fPtr);
		}

		switch (numberOfOptions)
		{
		case 1:
			{
				SayTextOption(textBoxString, inputString, Preset, GameWorld, 1, 
						options[0], playCutscene(sceneID[0], GameWorld));
			} break;

		case 2:
			{
				SayTextOption(textBoxString, inputString, Preset, GameWorld, 2, 
						options[0], playCutscene(sceneID[0], GameWorld), 
						options[1], playCutscene(sceneID[1], GameWorld));
			} break;

		case 3:
			{
				SayTextOption(textBoxString, inputString, Preset, GameWorld, 3, 
						options[0], playCutscene(sceneID[0], GameWorld), 
						options[1], playCutscene(sceneID[1], GameWorld),
						options[2], playCutscene(sceneID[2], GameWorld));
			} break;

		case 4:
			{
				SayTextOption(textBoxString, inputString, Preset, GameWorld, 4, 
						options[0], playCutscene(sceneID[0], GameWorld), 
						options[1], playCutscene(sceneID[1], GameWorld),
						options[2], playCutscene(sceneID[2], GameWorld),
						options[3], playCutscene(sceneID[3], GameWorld));
			} break;
		 
		default:
			break;
		}

	}
	else if (strcmp(inputString, "WAIT:") == 0)
	{
		return Wait(getNextArgFloat(fPtr), GameWorld);
	}
	else if (strcmp(inputString, "ENDCUTSCENE") == 0)
	{
		END_SCENE_HERE;
	}
	else if (strcmp(inputString, "CHANGEGAMEFLAG:") == 0 || strcmp(inputString, "INCREMENTGAMEFLAG:") == 0 || strcmp(inputString, "DECREMENTGAMEFLAG:") == 0)
	{
		int index = getNextArgGameFlag(fPtr);
		
		int value = getNextArgInt(fPtr);
		if (strcmp(inputString, "DECREMENTGAMEFLAG:") == 0)
		{
			value = -value;
		}

		return changeVariableBy(index, value, GameWorld);
	}
	else if (strcmp(inputString, "SETGAMEFLAG:") == 0 || strcmp(inputString, "SETFLAG:") == 0)
	{
		int index = getNextArgGameFlag(fPtr);

		int value = getNextArgInt(fPtr);
		return setVariableTo(index, value, GameWorld);
	}
	else if (strcmp(inputString, "IFVARIABLE:") == 0)
	{
		int index = getNextArgGameFlag(fPtr);

		getNextArg(fPtr, textBoxString, 3);
		int value = getNextArgInt(fPtr);

		getNextArg(fPtr, inputString, MAX_LEN);
		long filePos = ftell(fPtr);

		int ifTrue = NO_CUTSCENE;
		int ifFalse = NO_CUTSCENE;
		char ifTrueString[MAX_LEN] = {0};
		char ifFalseString[MAX_LEN] = {0};

		if (strcmp(inputString, "THEN:") == 0)
		{
			if (hasNextArgInt(fPtr))
			{
				ifTrue = getNextArgInt(fPtr);
			}
			else
			{
				getNextArg(fPtr, ifTrueString, CUTSCENE_FILE_NAME_MAX);
			}

			getNextArg(fPtr, inputString, MAX_LEN);
		}

		if (strcmp(inputString, "ELSE:") == 0)
		{
			if (hasNextArgInt(fPtr))
			{
				ifFalse = getNextArgInt(fPtr);
			}
			else
			{
				getNextArg(fPtr, ifFalseString, CUTSCENE_FILE_NAME_MAX);
			}
		}
		else
		{
			fseek(fPtr, filePos, SEEK_SET);
		}

		SceneAction *action = mapSymbolToIfAction(index, value, ifTrue, ifFalse, textBoxString, GameWorld);

		if (action != NULL)
		{
			// does not allow the scene currently playing to start itself, or a file with the name of "0" to play, so there is distinction between files and NO_CUTSCENE
 			if (ifTrueString[0] != 0 && strcmp(ifTrueString, "0"))
			{
				strcpy(action->ActionData.branchData.ifTrueString, ifTrueString);
			}
		
			if (ifFalseString[0] != 0 && strcmp(ifFalseString, "0"))
			{
				strcpy(action->ActionData.branchData.ifFalseString, ifFalseString);
			}
		}

		if (ifTrueString[0] == 0 && ifFalseString[0] == 0 && ifTrue == 0 && ifFalse == 0)
		{
			deleteSceneAction(action, GameWorld);
			return NULL;
		}

		return action;
	}
	else if (strcmp(inputString, "ANIMATEACTOR:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		getNextArg(fPtr, textBoxString, MAX_LEN);
		return AnimateActor(inputString, textBoxString, getNextArgInt(fPtr), GameWorld);
	}
	else if (strcmp(inputString, "SETACTORSPRITE:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		getNextArg(fPtr, textBoxString, MAX_LEN);
		return SwitchActorSprite(inputString, textBoxString, GameWorld);
	}
	else if (strcmp(inputString, "SETACTORPOS:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		float xPos =  getNextArgFloat(fPtr);
		return SetActorPosition(inputString, xPos, getNextArgFloat(fPtr), GameWorld);
	}
	else if (strcmp(inputString, "MOVEACTOR:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		float xMove = getNextArgFloat(fPtr);
		float yMove = getNextArgFloat(fPtr);
		int repeatTimes = getNextArgInt(fPtr);
		return MoveActor(inputString, xMove, yMove, repeatTimes, GameWorld);
	}
	else if (strcmp(inputString, "MOVEACTORX:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		float xMove = getNextArgFloat(fPtr);
		int repeatTimes = getNextArgInt(fPtr);
		return MoveActorX(inputString, xMove, repeatTimes, GameWorld);
	}
	else if (strcmp(inputString, "MOVEACTORY:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		float yMove = getNextArgFloat(fPtr);
		int repeatTimes = getNextArgInt(fPtr);
		return MoveActorY(inputString, yMove, repeatTimes, GameWorld);
	}
	else if (strcmp(inputString, "ROTATEACTOR:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		double rotation = (double)getNextArgFloat(fPtr);
		int repeat = getNextArgInt(fPtr);
		return RotateActor(inputString, rotation, repeat, GameWorld);
	}
	else if (strcmp(inputString, "SETACTORDIRECTION:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		return SetActorDirection(inputString, (double)getNextArgFloat(fPtr), GameWorld);
	}
	else if (strcmp(inputString, "HIDEACTOR:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		return HideActor(inputString, GameWorld);
	}
	else if (strcmp(inputString, "SHOWACTOR:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		return ShowActor(inputString, GameWorld);
	}
	else if (strcmp(inputString, "CREATEACTOR:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		int ID = getNextArgInt(fPtr);
		float xPos = getNextArgFloat(fPtr);
		float yPos = getNextArgFloat(fPtr);
		return CreateActor(inputString, ID, xPos, yPos, GameWorld);
	}
	else if (strcmp(inputString, "RELEASEACTOR:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		return ReleaseActor(inputString, GameWorld);
	}
	else if (strcmp(inputString, "SETACTORLAYER:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		return SetActorLayer(inputString, getNextArgInt(fPtr), GameWorld);
	}
	else if (strcmp(inputString, "PLAYSOUND:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		getNextArg(fPtr, textBoxString, MAX_LEN);
		int channel = getNextArgInt(fPtr);
		float volume = getNextArgFloat(fPtr);

		return SceneAction_PlaySound(inputString, textBoxString, channel, volume, GameWorld);
	}
	else if (strcmp(inputString, "SETCHANNELVOL:") == 0)
	{
		int channel = getNextArgFloat(fPtr);
		float volume = getNextArgFloat(fPtr);
		return SceneAction_SetSoundChannelVolume(channel, volume, GameWorld);
	}
	else if (strcmp(inputString, "CHANGECHANNELVOL:") == 0)
	{
		int channel = getNextArgInt(fPtr);
		float volume = getNextArgFloat(fPtr);
		return SceneAction_ChangeSoundChannelVolume(channel, volume, GameWorld);
	}
	else if (strcmp(inputString, "SETCAMERAPOS:") == 0)
	{
		float xPos = getNextArgFloat(fPtr);
		float yPos = getNextArgFloat(fPtr);
		return SceneAction_SetCameraPosition(xPos, yPos, GameWorld);
	}
	else if (strcmp(inputString, "MOVECAMERA:") == 0)
	{
		float xMove = getNextArgFloat(fPtr);
		float yMove = getNextArgFloat(fPtr);
		return SceneAction_MoveCamera(xMove, yMove, getNextArgInt(fPtr), GameWorld);
	}
	else if (!strcmp(inputString, "SMOOTHMOVECAMERATO:") || !strcmp(inputString, "MOVECAMERATO:"))
	{
		float xPos = getNextArgFloat(fPtr);
		float yPos = getNextArgFloat(fPtr);
		return SceneAction_MoveCameraSmooth(xPos, yPos, getNextArgFloat(fPtr), GameWorld);
	}
	else if (!strcmp(inputString, "SETCAMERAZOOM:") || !strcmp(inputString, "SETZOOM:"))
	{
		float xZoom = getNextArgFloat(fPtr);
		float yZoom = getNextArgFloat(fPtr);
		return SceneAction_SetZoom(xZoom, yZoom, GameWorld);
	}
	else if (!strcmp(inputString, "SETCAMERAZOOM:") || !strcmp(inputString, "SETZOOM:"))
	{
		float xZoom = getNextArgFloat(fPtr);
		float yZoom = getNextArgFloat(fPtr);
		return SceneAction_ChangeZoom(xZoom, yZoom, getNextArgInt(fPtr), GameWorld);
	}
	else if (!strcmp(inputString, "PLACEWALL:") || !strcmp(inputString, "PLACEINVISWALL:") || !strcmp(inputString, "PLACEINVISIBLEWALL"))
	{
		int xPos = getNextArgInt(fPtr);
		int yPos = getNextArgInt(fPtr);
		int xSize = getNextArgInt(fPtr);
		int ySize = getNextArgInt(fPtr);

		// add 'CamRelative' or 'OnScreen' after the command to place the wall relative to the camera position
		if (!atEndOfLine(fPtr))
		{
			getNextArg(fPtr, inputString, MAX_LEN); 
			stringToUpper(inputString);

			if (!strcmp(inputString, "CAMRELATIVE") || !strcmp(inputString, "ONSCREEN"))
			{
				return placeInvisibleWall(xPos + (int)GameWorld->MainCamera.CameraX, yPos + (int)GameWorld->MainCamera.CameraY, xSize, ySize, GameWorld);
			}
		}

		return placeInvisibleWall(xPos, yPos, xSize, ySize, GameWorld);
	}
	else if (!strcmp(inputString, "ENABLEPLAYER") || !strcmp(inputString, "ALLOWPLAYERCONTROL"))
	{
		return enablePlayer(GameWorld);
	}
	else if (!strcmp(inputString, "DISABLEPLAYER") || !strcmp(inputString, "REMOVEPLAYERCONTROL"))
	{
		return disablePlayer(GameWorld);
	}
	else if (!strcmp(inputString, "STATICSCENE:"))
	{
		bool staticScene = getNextArgBool(fPtr);

		if (!staticScene)
		{
			GameWorld->GameState = GAMEPLAY;
		}
	}
	else if (!strcmp(inputString, "WAITUNTIL:") && strcmp(textBoxString, "Wait_Until:"))
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		strcpy(textBoxString, "Wait_Until:");	// prevents 2 or more wait untils from happening in a row

		WaitUntil(loadSceneAction(inputString, textBoxString, GameWorld, fPtr));
	}
	else if (!strcmp(inputString, "REPEAT:") && strcmp(textBoxString, "Repeat_Instruction:"))
	{
		int repeatTimes = getNextArgInt(fPtr);

		getNextArg(fPtr, inputString, MAX_LEN);

		if (inputString[0] != '{')
		{
			return NULL;
		}

		SceneAction *instructions = NULL;
		int count = 0;

		while (inputString[0] != '}')
		{
			getNextArg(fPtr, inputString, MAX_LEN);

			if (inputString[0] == '}')
			{
				continue;
			}
		
			strcpy(textBoxString, "Repeat_Instruction:");	
			if (instructions == NULL)
			{
				instructions = loadSceneAction(inputString, textBoxString, GameWorld, fPtr);
			}
			else
			{
				loadSceneAction(inputString, textBoxString, GameWorld, fPtr);
			}
			count++;
		}
		
		RepeatArray(repeatTimes, count, instructions);
	}

	// add all other commands here

	return NULL;
}

const char* getSceneActionName(SceneActionID input)
{
	switch (input)
	{
	case SCENE_END:
		return "End cutscene";

	case SCENE_WAIT:
		return "Wait";

	case SCENE_SAY_TEXT:
		return "Say Text";

	case SCENE_CREATE_ACTOR:
		return "Create Actor";

	case SCENE_HIDE_ACTOR:
		return "Hide Actor";

	case SCENE_SHOW_ACTOR:
		return "Show Actor";

	case SCENE_RELEASE_ACTOR:
		return "Release Actor";

	case SCENE_IF_EQUALS:
	case SCENE_IF_LESS_THAN:
	case SCENE_IF_GREATER_THAN:
		return "Conditional Branch (If statement)";

	case SCENE_ROTATE_ACTOR:
		return "Rotate Actor";

	case SCENE_ANIMATE_ACTOR:
		return "Animate Actor";

	case SCENE_MOVE_ACTOR_X:
	case SCENE_MOVE_ACTOR_Y:
	case SCENE_MOVE_ACTOR:
		return "Move Actor";

	case SCENE_PLACE_INVISIBLE_WALL:
		return "Place invisible wall";

	case SCENE_MOVE_CAMERA:
		return "Move Camera";

	case SCENE_MOVE_CAMERA_SMOOTH:
		return "Move Camera smoothly";

	case SCENE_SET_CAMERA_POS:
		return "Set Camera Position";

	case SCENE_SET_CAMERA_ZOOM:
		return "Set Camera zoom";

	case SCENE_CHANGE_CAMERA_ZOOM:
		return "Change Camera zoom";

	case SCENE_SET_ACTOR_POS:
		return "Set Actor position";

	case SCENE_SET_ACTOR_LAYER:
		return "Set Actor layer";

	case SCENE_SET_ACTOR_SPRITE:
		return "Set Actor Sprite";

	case SCENE_SET_ACTOR_DIRECTION:
		return "Set Actor direction";

	case SCENE_ENABLE_PLAYER:
		return "Enable Player";

	case SCENE_DISABLE_PLAYER:
		return "Disable Player";
		
	case SCENE_TRIGGER_GAME_EVENT:
		return "Trigger GameEvent";

	case SCENE_CHANGE_VARIABLE_BY:
		return "Change GameFlag";

	case SCENE_SET_VARIABLE_TO:
		return "Set GameFlag";

	case SCENE_PLAY_SOUND:
		return "Play sound";

	case SCENE_SET_CHANNEL_VOL:
		return "Set channel volume";

	case SCENE_CHANGE_CHANNEL_VOL:
		return "Change channel volume";

	default:
		return "Unmapped SceneAction";
	}
}

int EndCutscene(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->CurrentCutscene == NO_CUTSCENE)
	{
		return EXECUTION_UNNECESSARY;
	}

	putConsoleStringTS("Ending cutscene, returning to gameplay.");

	GameWorld->CurrentCutscene = NO_CUTSCENE;
	deleteAllSceneActions(GameWorld);

	if (GameWorld->GameState == CUTSCENE)
	{
		GameWorld->GameState = GAMEPLAY;

		// Temporary; in future cutscenes will reset to previous cammode
		GameWorld->MainCamera.CameraMode = FOLLOW_PLAYER;
	}

	Object *PlayerObject = GameWorld->Player.PlayerPtr;

	if (PlayerObject != NULL && PlayerObject->State == ACTOR_STATE)
	{
		PlayerObject->State = DEFAULT_STATE;
	}

	if (GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	// By default, any objects that were not manually restored from Actor state will be deleted
	Object *currentObject = GameWorld->ObjectList->firstObject;

	while (currentObject != NULL)
	{
		if (currentObject->State == ACTOR_STATE)
		{
			MarkObjectForDeletion(currentObject);
		}

		currentObject = currentObject->nextObject;
	}

	return LEMON_SUCCESS;
}


SceneAction* enablePlayer(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	return createSceneAction(SCENE_ENABLE_PLAYER, GameWorld);
}

SceneAction* disablePlayer(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	return createSceneAction(SCENE_DISABLE_PLAYER, GameWorld);
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
	if (repeatCount < 1)
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

int RepeatArray(int repeatCount, int instructionCount, SceneAction *list)
{
	if (repeatCount < 1 || instructionCount < 1 || list == NULL)
	{
		return EXECUTION_UNNECESSARY;
	}

    SceneAction *instruction = list;

    for (int i = 0; i < instructionCount; i++)
    {
    	if (instruction == NULL)
    	{
    		return MISSING_DATA;
    	}

    	instruction->repeatTimes = repeatCount;
    	if (i == instructionCount - 1)
	    {
	    	instruction->parallelAction = false;
	    }

    	instruction = list->nextSceneAction;
    }


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

SceneAction* SceneAction_triggerGameEvent(GameEvent *inputEvent, World *GameWorld)
{
	if (inputEvent == NULL || GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_TRIGGER_GAME_EVENT, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	removeEventToTriggerLater(inputEvent, &newAction->ActionData.TriggerEvent, GameWorld);

	return newAction;
}

SceneAction* setVariableTo(int variableIndex, int value, World *GameWorld)
{
	if (!inRange(variableIndex, 0, GAME_FLAG_COUNT - 1) || GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_SET_VARIABLE_TO, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.variableArgs[0] = variableIndex;
	newAction->ActionData.variableArgs[1] = value;
	

	return newAction;
}

SceneAction* changeVariableBy(int variableIndex, int value, World *GameWorld)
{
	if (!inRange(variableIndex, 0, GAME_FLAG_COUNT - 1) || GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_CHANGE_VARIABLE_BY, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.variableArgs[0] = variableIndex;
	newAction->ActionData.variableArgs[1] = value;
	

	return newAction;
}


SceneAction* mapSymbolToIfAction(int variableIndex, int value, CutsceneID ifTrue, CutsceneID ifFalse, const char operator[], World *GameWorld)
{
	if (!strcmp(operator, "==") || !strcmp(operator, "="))
	{
		return ifElse(ifEquals(variableIndex, value, ifTrue, GameWorld), ifFalse);
	}
	else if (!strcmp(operator, "!="))
	{
		return ifElse(ifNotEquals(variableIndex, value, ifTrue, GameWorld), ifFalse);
	}
	else if (!strcmp(operator, ">"))
	{
		return ifElse(ifGreaterThan(variableIndex, value, ifTrue, GameWorld), ifFalse);
	}
	else if (!strcmp(operator, ">="))
	{
		return ifElse(ifGreaterThanEquals(variableIndex, value, ifTrue, GameWorld), ifFalse);
	}
	else if (!strcmp(operator, "<"))
	{
		return ifElse(ifLessThan(variableIndex, value, ifTrue, GameWorld), ifFalse);
	}
	else if (!strcmp(operator, "<="))
	{
		return ifElse(ifLessThanEquals(variableIndex, value, ifTrue, GameWorld), ifFalse);
	}

	return NULL;
}

SceneAction* ifEquals(int variableIndex, int value, CutsceneID cutsceneToTrigger, World *GameWorld)
{
	if (!inRange(variableIndex, 0, GAME_FLAG_COUNT - 1) || GameWorld == NULL ||  cutsceneToTrigger >= UNDEFINED_CUTSCENE)
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
	newAction->ActionData.branchData.elseBranchPresent = false;
	newAction->ActionData.branchData.ifTrue = cutsceneToTrigger;
	newAction->ActionData.branchData.ifFalse = NO_CUTSCENE;
	newAction->ActionData.branchData.variableIndex = variableIndex;
	newAction->ActionData.branchData.comparisonValue = value;

	return newAction;
}

SceneAction* ifNotEquals(int variableIndex, int value, CutsceneID cutsceneToTrigger, World *GameWorld)
{
	if (!inRange(variableIndex, 0, GAME_FLAG_COUNT - 1) || GameWorld == NULL || cutsceneToTrigger >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_IF_EQUALS, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;	
	newAction->ActionData.branchData.elseBranchPresent = true;
	newAction->ActionData.branchData.ifTrue = NO_CUTSCENE;
	newAction->ActionData.branchData.ifFalse = cutsceneToTrigger;
	newAction->ActionData.branchData.variableIndex = variableIndex;
	newAction->ActionData.branchData.comparisonValue = value;

	return newAction;
}

SceneAction* ifLessThan(int variableIndex, int value, CutsceneID cutsceneToTrigger, World *GameWorld)
{
	if (!inRange(variableIndex, 0, GAME_FLAG_COUNT - 1) || GameWorld == NULL || cutsceneToTrigger >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_IF_LESS_THAN, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;
	newAction->ActionData.branchData.elseBranchPresent = false;
	newAction->ActionData.branchData.ifTrue = cutsceneToTrigger;
	newAction->ActionData.branchData.ifFalse = NO_CUTSCENE;
	newAction->ActionData.branchData.variableIndex = variableIndex;
	newAction->ActionData.branchData.comparisonValue = value;

	return newAction;
}

SceneAction* ifLessThanEquals(int variableIndex, int value, CutsceneID cutsceneToTrigger, World *GameWorld)
{
	if (!inRange(variableIndex, 0, GAME_FLAG_COUNT - 1) || GameWorld == NULL ||  cutsceneToTrigger >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_IF_GREATER_THAN, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;
	newAction->ActionData.branchData.elseBranchPresent = true;
	newAction->ActionData.branchData.ifTrue = NO_CUTSCENE;
	newAction->ActionData.branchData.ifFalse = cutsceneToTrigger;
	newAction->ActionData.branchData.variableIndex = variableIndex;
	newAction->ActionData.branchData.comparisonValue = value;

	return newAction;
}

SceneAction* ifGreaterThan(int variableIndex, int value, CutsceneID cutsceneToTrigger, World *GameWorld)
{
	if (!inRange(variableIndex, 0, GAME_FLAG_COUNT - 1) || GameWorld == NULL || cutsceneToTrigger >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_IF_GREATER_THAN, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;	
	newAction->ActionData.branchData.elseBranchPresent = false;
	newAction->ActionData.branchData.ifTrue = cutsceneToTrigger;
	newAction->ActionData.branchData.ifFalse = NO_CUTSCENE;
	newAction->ActionData.branchData.variableIndex = variableIndex;
	newAction->ActionData.branchData.comparisonValue = value;
 
	return newAction;
}

SceneAction* ifGreaterThanEquals(int variableIndex, int value, CutsceneID cutsceneToTrigger, World *GameWorld)
{
	if (!inRange(variableIndex, 0, GAME_FLAG_COUNT - 1) || GameWorld == NULL || cutsceneToTrigger >= UNDEFINED_CUTSCENE)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_IF_LESS_THAN, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->repeatTimes = 1;
	newAction->parallelAction = false;	
	newAction->ActionData.branchData.elseBranchPresent = true;
	newAction->ActionData.branchData.ifTrue = NO_CUTSCENE;
	newAction->ActionData.branchData.ifFalse = cutsceneToTrigger;
	newAction->ActionData.branchData.variableIndex = variableIndex;
	newAction->ActionData.branchData.comparisonValue = value;
 
	return newAction;
}

SceneAction* ifElse(SceneAction *inputIfStatement, CutsceneID cutsceneIfElse)
{
	if (inputIfStatement == NULL || cutsceneIfElse >= UNDEFINED_CUTSCENE)
	{
		return inputIfStatement;
	}

	switch(inputIfStatement->ActionID)
	{
	case SCENE_IF_EQUALS:
	case SCENE_IF_LESS_THAN:
	case SCENE_IF_GREATER_THAN:
		// If it already has the else branch enabled, it has come from the if__Equals/ifNotEquals variation because it gets flipped
		if (inputIfStatement->ActionData.branchData.elseBranchPresent)
		{
			inputIfStatement->ActionData.branchData.ifTrue = cutsceneIfElse;
		}
		else
		{
			inputIfStatement->ActionData.branchData.ifFalse = cutsceneIfElse;
		}
		inputIfStatement->ActionData.branchData.elseBranchPresent = true;
		break;

	default:
		break;
	}

	return inputIfStatement;
}


SceneAction* SceneAction_SayText(TextBox *text, World *GameWorld)
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

	Object *actorObj = FindObject(objName, GameWorld->ObjectList);
	if (actorObj == NULL)
	{
		return NULL;
	}

	int animIndex = getAnimationIndex(animName, getDisplay(actorObj));
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

	Object *actorObj = FindObject(objName, GameWorld->ObjectList);
	if (actorObj == NULL)
	{
		return NULL;
	}

	int spriteIndex = getSpriteIndex(spriteName, getDisplay(actorObj));
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

	Object *actorObj = FindObject(objName, GameWorld->ObjectList);
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

	Object *actorObj = FindObject(objName, GameWorld->ObjectList);
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

	Object *actorObj = FindObject(objName, GameWorld->ObjectList);
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

	Object *actorObj = FindObject(objName, GameWorld->ObjectList);
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

	Object *actorObj = FindObject(objName, GameWorld->ObjectList);
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

	Object *actorObj = FindObject(objName, GameWorld->ObjectList);
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

	Object *actorObj = FindObject(objName, GameWorld->ObjectList);
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

	Object *actorObj = FindObject(objName, GameWorld->ObjectList);
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

	Object *actorObj = FindObject(objName, GameWorld->ObjectList);
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

SceneAction* CreateActor(char objName[], ObjectType actorID, float xPos, float yPos, World *GameWorld)
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

	newAction->ActionData.positions[0] = xPos;
	newAction->ActionData.positions[1] = yPos;

	newAction->ActorObject = FindObject(objName, GameWorld->ObjectList);
	if (newAction->ActorObject != NULL)
	{
		// If an object with this name already exists, just get a reference to it
		// to have its state set to 'ACTOR' and position set precisely when its scheduled to
		return newAction;
	}

	// otherwise create a new object with this name at (0,0), being static and invisible to enter the scene when its scheduled to
	newAction->ActorObject = AddNamedObject(GameWorld, objName, actorID, 0, 0);
	if (newAction->ActorObject != NULL)
	{
		newAction->ActorObject->State = STATIC;
		hideObject(newAction->ActorObject);
	}

	return newAction;
}


SceneAction* ReleaseActor(char objName[], World *GameWorld)		// use if you dont want an actor to be deleted when the cutscene ends
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}


	Object *actorObj = FindObject(objName, GameWorld->ObjectList);
	if (actorObj == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_RELEASE_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActorObject = actorObj;

	return newAction;
}

SceneAction* placeInvisibleWall(int xPos, int yPos, int xSize, int ySize, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}


	SceneAction *newAction = createSceneAction(SCENE_PLACE_INVISIBLE_WALL, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.invisWall[0] = xPos;
	newAction->ActionData.invisWall[1] = yPos;
	newAction->ActionData.invisWall[2] = xSize;
	newAction->ActionData.invisWall[3] = ySize;

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

SceneAction* SceneAction_ChangeSoundChannelVolume(ChannelName soundChannel, float change, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_CHANGE_CHANNEL_VOL, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.soundData.soundName, "noSound");
	strcpy(newAction->ActionData.soundData.folderName, "noFolder");
	newAction->ActionData.soundData.channel = soundChannel;
	newAction->ActionData.soundData.volume = change;
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

	SceneAction *newAction = createSceneAction(SCENE_CHANGE_CHANNEL_VOL, GameWorld);
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

	SceneAction *newAction = createSceneAction(SCENE_SET_CAMERA_ZOOM, GameWorld);
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

	SceneAction *newAction = createSceneAction(SCENE_CHANGE_CAMERA_ZOOM, GameWorld);
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
	if (GameWorld == NULL || GameWorld->SceneActionCount >= EngineSettings.MaxSceneActions)
	{
		return NULL;
	}

	if (newActionID >= UNDEFINED_SCENE_ACTION || newActionID < 0)
	{
		return NULL;
	}

	SceneAction *newAction = malloc(sizeof(SceneAction));
	memset(newAction, 0, sizeof(SceneAction));

	if (newAction == NULL)
	{
		return NULL;
	}

	if (GameWorld->SceneActionQueue == NULL)
	{
		GameWorld->SceneActionQueue = newAction;
		newAction->prevSceneAction = NULL;
	}
	else
	{
		SceneAction *actionPtr = GameWorld->SceneActionQueue;

		while (actionPtr->nextSceneAction != NULL)
		{
			actionPtr = actionPtr->nextSceneAction;
		}

		actionPtr->nextSceneAction = newAction;
		newAction->prevSceneAction = actionPtr;
	}

	GameWorld->SceneActionCount++;

	newAction->nextSceneAction = NULL;
	newAction->ActorObject = NULL;

	newAction->ActionID = newActionID;
	newAction->parallelAction = true;
	newAction->repeatTimes = 1;

	if (DebugSettings.showSceneActions)
	{
		putConsoleStringTS("Running scene action ID: %d (%s)", newActionID, getSceneActionName(newActionID));
	}

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
		GameWorld->SceneActionQueue = nextAction;
	}

	if (nextAction != NULL)
	{
		nextAction->prevSceneAction = prevAction;
	}

	GameWorld->SceneActionCount--;
	free(deleteAction);


	return nextAction;
}


int deleteAllSceneActions(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	while (GameWorld->SceneActionQueue != NULL)
	{
		deleteSceneAction(GameWorld->SceneActionQueue, GameWorld);
	}

	return LEMON_SUCCESS;
}