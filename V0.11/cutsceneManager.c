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

	GameWorld->GameState = CUTSCENE;

	// by default, player is disabled during cutscenes
	if (GameWorld->Player.PlayerPtr != NULL)
	{
		GameWorld->Player.PlayerPtr->reserved &= ~RFLAG_CUTSCENE_IMMUNITY;
	}

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

	putConsoleTS("Starting Cutscene... ID: %d", inputID);

	prepareCutsceneEnvironment(GameWorld);
	
	// Set-up cutscene
	switch (inputID)
	{
	case TEST_SCENE:
		GameWorld->MainCamera.CameraMode = FREE_ROAM_RESTRICTED;

		SayText("A test cutscene, huh?", NO_PORTRAIT, BASIC_TEXT, GameWorld);
	
		SayTextOption("Play new cutscene?", NO_PORTRAIT, BASIC_TEXT, GameWorld, 3, 
					"Test scene", 	playCutscene(TEST_SCENE, GameWorld), 
					"Test scene 2", playCutscene(TEST_SCENE_2, GameWorld),
					"No", NO_ACTION);
		break;

	case TEST_SCENE_2:
		GameFlags[0].value++;

		SayText("A small tomato is really just a cherry.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		break;

	case TEST_SCENE_2_AGAIN:
		SayText("Wait a second..... \nyou've been here before.", NO_PORTRAIT, BASIC_TEXT, GameWorld);
		SayTextOption("Do you remember the thing about the cherries?", NO_PORTRAIT, BASIC_TEXT, GameWorld, 3, 
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
		break;
	}

	GameWorld->CurrentCutscene = inputID;


	return LEMON_SUCCESS;
}

int initialiseCutsceneFromFile(const char sceneName[], World *GameWorld)
{
	putConsole("Starting Cutscene... Name: %s", sceneName);

	prepareCutsceneEnvironment(GameWorld);

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

	while (!endOfFile(fPtr))
	{
		getNextArg(fPtr, readString, MAX_LEN);
		
		loadSceneAction(readString, GameWorld, fPtr);
	}

	closeFile(fPtr);

	GameWorld->CurrentCutscene = CUTSCENE_FROM_FILE;

	return LEMON_SUCCESS;
}


int updateCutscene(World *GameWorld)
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

	if (GameWorld->nextSceneAction == NULL || GameWorld->CurrentCutscene == END_CUTSCENE)
	{
		EndCutscene(GameWorld);
	}
	

	return LEMON_SUCCESS;
}


int updateSceneActions(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	int i = EngineSettings.MaxSceneActions;
	FuncResult response = LEMON_SUCCESS;

	while (GameWorld->nextSceneAction != NULL && i > 0)
	{
		response = RunSceneAction(GameWorld);

		if (response == ACTION_DISABLED)
		{
			return LEMON_SUCCESS;
		}
	}

	return LEMON_SUCCESS;
}

int SkipSceneActions(int skipCount, World *GameWorld)
{
	int skip = 0;
	SceneAction *current = GameWorld->nextSceneAction;

	while (skip < skipCount && current != NULL)
	{
		if (current->ActionID == SCENE_SAY_TEXT)
		{
			deleteTextBox(&current->ActionData.sceneText, GameWorld);
		}

		current = current->nextAction;
		skip++;
	}

	GameWorld->nextSceneAction = current;

	return LEMON_SUCCESS;
}




FuncResult RunSceneAction(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->nextSceneAction == NULL)
	{
		return MISSING_DATA;
	}

	SceneAction *action = GameWorld->nextSceneAction;

	SceneActionArguments *data = &action->ActionData;

	switch (action->ActionID)
	{
	case SCENE_IF_STATEMENT:
		{
			if (!ConditionIsTrue(&data->sceneIfStatement.condition))
			{
				return SkipSceneActions(data->sceneIfStatement.branchDistanceIfFalse, GameWorld);
			}
		} break;

	case SCENE_SKIP_INSTRUCTIONS:
		{
			return SkipSceneActions(data->instructionsToSkip, GameWorld);
		} break;

	case SCENE_REPEAT:
		{
			LoopData *loop = &action->ActionData.loop; 

			loop->currentLoop++;
			if (loop->currentLoop < loop->repeatTimes)
			{
				int instructions = loop->instructionCount;
				while (instructions > 0 && GameWorld->nextSceneAction->prevAction != NULL)
				{
					instructions--;
					GameWorld->nextSceneAction = GameWorld->nextSceneAction->prevAction;
				}

				return ACTION_DISABLED;
			}	
			else
			{
				// reset this loop in case its revisited later
				loop->currentLoop = 0;
			}
		} break;

	case SCENE_REPEAT_UNTIL:
		{
			ConditionalLoopData *loop = &action->ActionData.loopUntil; 

			if (!ConditionIsTrue(&loop->condition))
			{
				int instructions = loop->instructionCount;
				while (instructions > 0 && GameWorld->nextSceneAction->prevAction != NULL)
				{
					instructions--;
					GameWorld->nextSceneAction = GameWorld->nextSceneAction->prevAction;
				}

				return ACTION_DISABLED;
			}	
		} break;

	case SCENE_REPEAT_WHILE:
		{
			ConditionalLoopData *loop = &action->ActionData.loopUntil; 

			if (ConditionIsTrue(&loop->condition))
			{
				int instructions = loop->instructionCount;
				while (instructions > 0 && GameWorld->nextSceneAction->prevAction != NULL)
				{
					instructions--;
					GameWorld->nextSceneAction = GameWorld->nextSceneAction->prevAction;
				}

				return ACTION_DISABLED;
			}	
		} break;

	case SCENE_WAIT:
		action->ActionData.WaitTicks[1]--;
		if (action->ActionData.WaitTicks[1] < 1)
		{
			action->ActionData.WaitTicks[1] = action->ActionData.WaitTicks[0];
		}
		else
		{
			return ACTION_DISABLED;
		}
		break;

	case SCENE_END:
		GameWorld->CurrentCutscene = END_CUTSCENE;
		GameWorld->nextSceneAction = NULL;
		break;

	case SCENE_SWITCH_CUTSCENE:
		{
			playCutscene(data->SceneID, GameWorld);
			return ACTION_DISABLED;
		} break;

	case SCENE_TRIGGER_GAME_EVENT:
		{
			triggerGameEvent(&action->ActionData.TriggerEvent, GameWorld);
		} break;

	case SCENE_SAY_TEXT:
		{
			TextBox *box = &action->ActionData.sceneText;
			if (box->currentIndex < 0)
			{
				TextInteraction(box, GameWorld);
			}
			else
			{
				displayText(box, GameWorld);
			}

			if (box->boxPtr == NULL)
			{
				GameWorld->nextSceneAction = action->nextAction;
			}

			return ACTION_DISABLED;
		} break;

	case SCENE_DISABLE_PLAYER:
		if (GameWorld->Player.PlayerPtr != NULL)
		{
			GameWorld->Player.PlayerPtr->reserved &= ~RFLAG_CUTSCENE_IMMUNITY;
		}
		break;

	case SCENE_ENABLE_PLAYER:
		if (GameWorld->Player.PlayerPtr != NULL)
		{
			GameWorld->Player.PlayerPtr->reserved |= RFLAG_CUTSCENE_IMMUNITY;
		}
		break;

	case SCENE_CHANGE_VARIABLE_BY:
		{
			GameFlags[data->variableArgs[0]].value += data->variableArgs[1];
		} break;

	case SCENE_SET_VARIABLE_TO:
		{
			GameFlags[data->variableArgs[0]].value = data->variableArgs[1];
		} break;

	case SCENE_ANIMATE_ACTOR:
		{
			Object *actor = FindObject(data->actor.name, &GameWorld->ObjectList);
			if (actor == NULL)
			{
				break;
			}

			DisplayData *actorDisplay = getDisplay(actor);
			char *animName = data->actor.animationName;
			int loopCount = data->actor.loopCount;

			PlayAnimation(animName, loopCount, actorDisplay);
		} break;

	case SCENE_ANIMATE_ACTOR_WAIT:
		{
			Object *actor = FindObject(data->actor.name, &GameWorld->ObjectList);
			if (actor == NULL)
			{
				break;
			}

			DisplayData *actorDisplay = getDisplay(actor);
			char *animName = data->actor.animationName;
			int loopCount = data->actor.loopCount;

			if (data->actor.animationTriggered == 0)
			{
				PlayAnimation(animName, loopCount, actorDisplay);
				data->actor.animationTriggered = 1;
			}

			if (actorDisplay->currentAnimation != 0)
			{
				return ACTION_DISABLED;
			}
			else
			{
				data->actor.animationTriggered = 0;
			}
		} break;

	case SCENE_SET_ACTOR_SPRITE:
		{
			Object *actor = FindObject(data->actor.name, &GameWorld->ObjectList);
			if (actor == NULL)
			{
				break;
			}

			DisplayData *actorDisplay = getDisplay(actor);
			char *spriteName = data->actor.spriteName;

			actorDisplay->currentAnimation = 0;
			switchSpriteByName(spriteName, USE_CURRENT_SPRITESET, actorDisplay);
		} break;

	case SCENE_SET_ACTOR_POS:
		{
			Object *actor = FindObject(data->actor.name, &GameWorld->ObjectList);
			if (actor == NULL)
			{
				break;
			}

			GoTo(actor, data->actor.xPos, data->actor.yPos);
		} break;

	case SCENE_MOVE_ACTOR:
	case SCENE_MOVE_ACTOR_X:
	case SCENE_MOVE_ACTOR_Y:
		{
			Object *actor = FindObject(data->actor.name, &GameWorld->ObjectList);
			if (actor == NULL)
			{
				break;
			}

			PhysicsBox *actorBox = actor->ObjectBox;
			float xMove = data->actor.xPos;
			float yMove = data->actor.yPos;
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
			Object *actor = FindObject(data->actor.name, &GameWorld->ObjectList);
			if (actor == NULL)
			{
				break;
			}

			double direction = data->actor.direction;

			SetObjectDirection(actor, direction);
		} break;

	case SCENE_ROTATE_ACTOR:
		{
			Object *actor = FindObject(data->actor.name, &GameWorld->ObjectList);
			if (actor == NULL)
			{
				break;
			}

			double rotate = data->actor.direction;

			RotateObject(actor, rotate);
		} break;

	case SCENE_HIDE_ACTOR:
		{
			hideObject(FindObject(data->actor.name, &GameWorld->ObjectList));
		} break;

	case SCENE_SHOW_ACTOR:
		{
			showObject(FindObject(data->actor.name, &GameWorld->ObjectList));
		} break;

	case SCENE_SET_ACTOR_LAYER:
		{
			setDisplayLayer(FindObject(data->actor.name, &GameWorld->ObjectList), data->actor.layer);
		} break;

	case SCENE_CREATE_ACTOR:
		{
			Object *actor = FindObject(data->actor.name, &GameWorld->ObjectList);
			if (actor == NULL)
			{
				actor = AddNamedObject(GameWorld, data->actor.name, data->actor.objectID, data->actor.xPos, data->actor.yPos);			
			}

			if (actor == NULL)
			{
				break;
			}

			actor->State = ACTOR_STATE;
		} break;

	case SCENE_RELEASE_ACTOR:
		{
			Object *actor = FindObject(data->actor.name, &GameWorld->ObjectList);
			if (actor != NULL && actor->State == ACTOR_STATE)
			{
				actor->State = DEFAULT_STATE;
			}
		} break;

	case SCENE_PLACE_INVISIBLE_WALL:
		{
			ObjectMeta *meta = &data->actor;
			Object *wall = AddObject(GameWorld, SOLID_BLOCK, meta->xPos, meta->yPos, meta->xSize, meta->ySize, -1, 0);

			if (wall != NULL)
			{
				SetObjectName(wall,	"InvisibleWall");
				wall->State = ACTOR_STATE;
			}
		} break;

	case SCENE_PLAY_SOUND:
		{
			PlaySound(data->soundData.soundName, data->soundData.volume, data->soundData.channel);
		} break;

	case SCENE_SET_CAMERA_POS:
			GameWorld->MainCamera.CameraX = data->CameraData[0];
			GameWorld->MainCamera.CameraY = data->CameraData[1];
			break;

	case SCENE_MOVE_CAMERA_TO_OBJECT:
		{
			Object *actor = FindObject(data->actor.name, &GameWorld->ObjectList);
			if (actor == NULL)
			{
				break;
			}

			PhysicsBox *objBox = actor->ObjectBox;
			float xDest = objBox->xPos + (objBox->xSize / 2);
			float yDest = objBox->yPos + (objBox->ySize / 2);
			float speedCoefficient = data->actor.speed;

			float xDifference = xDest - GameWorld->MainCamera.CameraX;
			float yDifference = yDest - GameWorld->MainCamera.CameraY;

			if (fabs(xDifference) < 1.0 || speedCoefficient < 0.1)
			{
				GameWorld->MainCamera.CameraX = xDest;
			}
			else
			{
				GameWorld->MainCamera.CameraX += xDifference / speedCoefficient;
			}

			if (fabs(yDifference) < 1.0 || speedCoefficient < 0.1)
			{
				GameWorld->MainCamera.CameraY = yDest;
			}
			else
			{
				GameWorld->MainCamera.CameraY += yDifference / speedCoefficient;
			}
		} break;

	case SCENE_MOVE_CAMERA:
			GameWorld->MainCamera.CameraX += data->CameraData[0];
			GameWorld->MainCamera.CameraY += data->CameraData[1];
			break;

	case SCENE_MOVE_CAMERA_TO:
		{
			float xDest = data->CameraData[0];
			float yDest = data->CameraData[1];
			float speedCoefficient = data->CameraData[2];

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
		} break;

	case SCENE_SET_CAMERA_ZOOM:
		{
			GameWorld->MainCamera.zoomX = data->zoomScales[0];
			GameWorld->MainCamera.zoomY = data->zoomScales[1];
		} break;

	case SCENE_CHANGE_CAMERA_ZOOM:
		{
			GameWorld->MainCamera.zoomX += data->zoomScales[0];
			GameWorld->MainCamera.zoomY += data->zoomScales[1];
		} break;

	case SCENE_CHANGE_CAMERA_ZOOM_TO:
		{
			float xDest = data->zoomScales[0];
			float yDest = data->zoomScales[1];
			float speedCoefficient = data->zoomScales[2];

			float xDifference = xDest - GameWorld->MainCamera.zoomX;
			float yDifference = yDest - GameWorld->MainCamera.zoomY;

			if (fabs(xDifference) < 0.01)
			{
				GameWorld->MainCamera.zoomX = xDest;
			}
			else
			{
				GameWorld->MainCamera.zoomX += xDifference / speedCoefficient;
			}

			if (fabs(yDifference) < 0.01)
			{
				GameWorld->MainCamera.zoomY = yDest;
			}
			else
			{
				GameWorld->MainCamera.zoomY += yDifference / speedCoefficient;
			}
		} break;

	case SCENE_SET_CAMERA_MODE:
		{
			GameWorld->MainCamera.CameraMode = data->cameraMode;
		} break;

	case SCENE_SET_CHANNEL_VOL:
		{
			SetChannelVolume(data->soundData.channel, data->soundData.volume);
		} break;

	case SCENE_CHANGE_CHANNEL_VOL:
		{
			ChangeChannelVolume(data->soundData.channel, data->soundData.volume);
		} break;

	default:
		break;
	}

	if (GameWorld->nextSceneAction != NULL)
	{
		GameWorld->nextSceneAction = GameWorld->nextSceneAction->nextAction;
	}
	
	return LEMON_SUCCESS;
}


bool ConditionIsTrue(ConditionalStatement *input)
{
	if (input->variableIndex >= 0 && input->variableIndex < GAME_FLAG_COUNT)
	{
		if (strcmp(input->expression, "=") == 0 || strcmp(input->expression, "==") == 0)
		{
			return (GameFlags[input->variableIndex].value == input->comparisonValue);
		}
		else if (strcmp(input->expression, ">") == 0)
		{
			return (GameFlags[input->variableIndex].value > input->comparisonValue);
		}
		else if (strcmp(input->expression, ">=") == 0)
		{
			return (GameFlags[input->variableIndex].value >= input->comparisonValue);
		}
		else if (strcmp(input->expression, "<") == 0)
		{
			return (GameFlags[input->variableIndex].value < input->comparisonValue);
		}
		else if (strcmp(input->expression, "<=") == 0)
		{
			return (GameFlags[input->variableIndex].value <= input->comparisonValue);
		}
		else if (strcmp(input->expression, "!=") == 0)
		{
			return (GameFlags[input->variableIndex].value != input->comparisonValue);
		}
	}

	return false;
}

SceneAction* loadSceneAction(char inputString[MAX_LEN], World *GameWorld, FILE *fPtr)
{
	removeChar(inputString, '_', MAX_LEN);
	stringToUpper(inputString);

	if (strcmp(inputString, "SAYTEXT:") == 0)
	{
		char textBoxString[MAX_TEXT_LENGTH] = {0};
		getNextArg(fPtr, textBoxString, MAX_TEXT_LENGTH);
		getNextArg(fPtr, inputString, MAX_LEN);
		SayText(textBoxString, inputString, getNextArgInt(fPtr), GameWorld);
	}
	else if (strcmp(inputString, "SAYTEXTEVENT:") == 0)
	{
		char textBoxString[MAX_TEXT_LENGTH] = {0};
		getNextArg(fPtr, textBoxString, MAX_TEXT_LENGTH);
		getNextArg(fPtr, inputString, MAX_LEN);
		TextBox *textEvent = SayText(textBoxString, inputString, getNextArgInt(fPtr), GameWorld);

		if (textEvent == NULL)
		{
			return NULL;
		}

		GameEvent *newEvent = malloc(sizeof(GameEvent));

		if (newEvent == NULL)
		{
			return NULL;
		}

		memset(newEvent, 0, sizeof(GameEvent));
		getNextArgGameEvent(fPtr, newEvent, GameWorld);

		textEvent->textTypeSetting = TEXTBOX_TRIGGER_EVENT;
		textEvent->textTypeData.TriggerEvent = newEvent;
	}
	else if (strcmp(inputString, "SAYTEXTOPTION:") == 0)
	{
		char textBoxString[MAX_TEXT_LENGTH] = {0};
		getNextArg(fPtr, textBoxString, MAX_TEXT_LENGTH);
		getNextArg(fPtr, inputString, MAX_LEN);
		int Preset = getNextArgInt(fPtr);
		int numberOfOptions = getNextArgInt(fPtr);

		TextBox *textOption = SayText(textBoxString, inputString, Preset, GameWorld);

		if (textOption == NULL)
		{
			return NULL;
		}

		textOption->textTypeSetting = TEXTBOX_OPTION_PROMPT;
		TextOptionPrompt *data = &textOption->textTypeData.OptionPrompt;
		data->numberOfOptions = numberOfOptions;
		data->SelectedOption = 0;
		data->setUpComplete = false;
		data->optionTriggers = malloc(numberOfOptions * sizeof(GameEvent));

		if (data->optionTriggers == NULL)
		{
			return NULL;
		}

		memset(data->optionTriggers, 0, numberOfOptions * sizeof(GameEvent));

		GameEvent newEvent; 

		for (int i = 0; i < numberOfOptions && i < MAX_TEXT_OPTIONS; i++)
		{
			getNextArg(fPtr, data->optionNames[i], OPTION_TEXT_MAX_LEN);

			memset(&newEvent, 0, sizeof(GameEvent));
			getNextArgGameEvent(fPtr, &newEvent, GameWorld);
			
			memcpy(&data->optionTriggers[i], &newEvent, sizeof(GameEvent));
		}
	}
	else if (strcmp(inputString, "WAIT:") == 0)
	{
		return Wait(getNextArgFloat(fPtr), GameWorld);
	}
	else if (strcmp(inputString, "SWITCHCUTSCENE:") == 0 || strcmp(inputString, "PLAYCUTSCENE:") == 0)
	{
		int sceneID = getNextArgInt(fPtr);

		return SceneAction_SwitchCutscene(sceneID, GameWorld);
	}
	else if (strcmp(inputString, "ENDCUTSCENE") == 0)
	{
		END_SCENE_HERE;
	}
	else if (strcmp(inputString, "TRIGGEREVENT:") == 0 || strcmp(inputString, "TRIGGERGAMEEVENT:") == 0)
	{
		GameEvent newEvent = {0}; 
		getNextArgGameEvent(fPtr, &newEvent, GameWorld);
		SceneAction *newAction = createSceneAction(SCENE_TRIGGER_GAME_EVENT, GameWorld);

		if (newAction != NULL)
		{
			memcpy(&newAction->ActionData.TriggerEvent, &newEvent, sizeof(GameEvent));
		}
	}
	else if (strcmp(inputString, "CHANGEGAMEFLAG:") == 0)
	{
		int index = getNextArgGameFlag(fPtr);
		
		int value = getNextArgInt(fPtr);

		return changeVariableBy(index, value, GameWorld);
	}
	else if (strcmp(inputString, "INCREMENTGAMEFLAG:") == 0 || strcmp(inputString, "INCGAMEFLAG:") == 0)
	{
		int index = getNextArgGameFlag(fPtr);

		return changeVariableBy(index, 1, GameWorld);
	}
	else if (strcmp(inputString, "DECREMENTGAMEFLAG:") == 0 || strcmp(inputString, "DECGAMEFLAG:") == 0)
	{
		int index = getNextArgGameFlag(fPtr);

		return changeVariableBy(index, -1, GameWorld);
	}
	else if (strcmp(inputString, "SETGAMEFLAG:") == 0 || strcmp(inputString, "SETFLAG:") == 0)
	{
		int index = getNextArgGameFlag(fPtr);

		int value = getNextArgInt(fPtr);
		return setVariableTo(index, value, GameWorld);
	}
	else if (strcmp(inputString, "IFVARIABLE:") == 0 || strcmp(inputString, "IF:") == 0)
	{
		SceneAction *ifStatement = createSceneAction(SCENE_IF_STATEMENT, GameWorld);

		if (ifStatement == NULL)
		{
			loadBracketedSceneActions(fPtr, GameWorld);
			return NULL;
		}

		IfStatementData *data = &ifStatement->ActionData.sceneIfStatement;
		LoadConditionalStatement(fPtr, &data->condition);

		data->branchDistanceIfFalse = loadBracketedSceneActions(fPtr, GameWorld);

		long filePos = ftell(fPtr);
		getNextArg(fPtr, inputString, MAX_LEN);

		if (strcmp(inputString, "ELSE") == 0)
		{
			SceneAction *elseStatement = createSceneAction(SCENE_SKIP_INSTRUCTIONS, GameWorld);

			if (elseStatement == NULL)
			{
				return ifStatement;
			}

			elseStatement->ActionData.instructionsToSkip = loadBracketedSceneActions(fPtr, GameWorld);
			data->elseBranchPresent = true;
			data->branchDistanceIfFalse++;	// +1 to account for 'skip instructions' scene action
		}
		else
		{
			data->elseBranchPresent = false;
			fseek(fPtr, filePos, SEEK_SET);
		}

		if (data->branchDistanceIfFalse < 1)
		{
			deleteSceneAction(ifStatement, GameWorld);

			return NULL;
		}

		return ifStatement;
	}
	else if (strcmp(inputString, "ANIMATEACTOR:") == 0)
	{
		char animName[MAX_LEN] = {0};
		getNextArg(fPtr, inputString, MAX_LEN);
		getNextArg(fPtr, animName, MAX_LEN);

		return AnimateActor(inputString, animName, getNextArgInt(fPtr), GameWorld);
	}
	else if (strcmp(inputString, "ANIMATEACTORANDWAIT:") == 0)
	{
		char animName[MAX_LEN] = {0};
		getNextArg(fPtr, inputString, MAX_LEN);
		getNextArg(fPtr, animName, MAX_LEN);

		return AnimateActorAndWait(inputString, animName, getNextArgInt(fPtr), GameWorld);
	}
	else if (strcmp(inputString, "SETACTORSPRITE:") == 0)
	{
		char spriteName[MAX_LEN] = {0};
		getNextArg(fPtr, inputString, MAX_LEN);
		getNextArg(fPtr, spriteName, MAX_LEN);

		return SwitchActorSprite(inputString, spriteName, GameWorld);
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
		return MoveActor(inputString, xMove, yMove, GameWorld);
	}
	else if (strcmp(inputString, "MOVEACTORX:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		float xMove = getNextArgFloat(fPtr);
		return MoveActorX(inputString, xMove, GameWorld);
	}
	else if (strcmp(inputString, "MOVEACTORY:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		float yMove = getNextArgFloat(fPtr);
		return MoveActorY(inputString, yMove, GameWorld);
	}
	else if (strcmp(inputString, "ROTATEACTOR:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		double rotation = (double)getNextArgFloat(fPtr);
		return RotateActor(inputString, rotation, GameWorld);
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
	else if (strcmp(inputString, "SETACTORSIZE:") == 0)
	{
		getNextArg(fPtr, inputString, MAX_LEN);
		int xSize = getNextArgInt(fPtr);
		int ySize = getNextArgInt(fPtr);
		Object *obj = FindObject(inputString, &GameWorld->ObjectList);

		if (obj != NULL)
		{
			obj->ObjectBox->xSize = xSize;
			obj->ObjectBox->ySize = ySize;
		}

		return NULL;
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
		float volume = getNextArgFloat(fPtr);
		int channel = getNextArgInt(fPtr);

		return SceneAction_PlaySound(inputString, channel, volume, GameWorld);
	}
	else if (strcmp(inputString, "SETCHANNELVOLUME:") == 0 || strcmp(inputString, "SETCHANNELVOL:") == 0)
	{
		int channel = getNextArgFloat(fPtr);
		float volume = getNextArgFloat(fPtr);
		return SceneAction_SetSoundChannelVolume(channel, volume, GameWorld);
	}
	else if (strcmp(inputString, "CHANGECHANNELVOLUME:") == 0 || strcmp(inputString, "CHANGECHANNELVOL:") == 0)
	{
		int channel = getNextArgInt(fPtr);
		float volume = getNextArgFloat(fPtr);
		return SceneAction_ChangeSoundChannelVolume(channel, volume, GameWorld);
	}
	else if (strcmp(inputString, "SETCAMERAPOS:") == 0)
	{
		if (hasNextArgNumber(fPtr))
		{
			float xPos = getNextArgFloat(fPtr);
			float yPos = getNextArgFloat(fPtr);
			return SceneAction_SetCameraPosition(xPos, yPos, GameWorld);
		}
		else
		{
			char name[OBJECT_NAME_LENGTH] = {0};
			getNextArg(fPtr, name, OBJECT_NAME_LENGTH);

			return SceneAction_MoveCameraToObject(name, 0.0, GameWorld);
		}
		
	}
	else if (strcmp(inputString, "MOVECAMERA:") == 0)
	{
		float xMove = getNextArgFloat(fPtr);
		float yMove = getNextArgFloat(fPtr);
		return SceneAction_MoveCamera(xMove, yMove, GameWorld);
	}
	else if (!strcmp(inputString, "SMOOTHMOVECAMERATO:") || strcmp(inputString, "MOVECAMERATO:") == 0)
	{
		if (hasNextArgNumber(fPtr))
		{
			float xPos = getNextArgFloat(fPtr);
			float yPos = getNextArgFloat(fPtr);

			return SceneAction_MoveCameraTo(xPos, yPos, getNextArgFloat(fPtr), GameWorld);
		}
		else
		{
			char name[OBJECT_NAME_LENGTH] = {0};
			getNextArg(fPtr, name, OBJECT_NAME_LENGTH);
			return SceneAction_MoveCameraToObject(name, getNextArgFloat(fPtr), GameWorld);
		}
	}
	else if (strcmp(inputString, "SETCAMERAMODE:") == 0 || strcmp(inputString, "SETCAMMODE:") == 0)
	{
		int mode = getNextArgInt(fPtr);
		return SceneAction_SetCameraMode(mode, GameWorld);
	}
	else if (!strcmp(inputString, "SETCAMERAZOOM:") || !strcmp(inputString, "SETZOOM:"))
	{
		float xZoom = getNextArgFloat(fPtr);
		float yZoom = getNextArgFloat(fPtr);
		return SceneAction_SetZoom(xZoom, yZoom, GameWorld);
	}
	else if (!strcmp(inputString, "CHANGECAMERAZOOM:") || !strcmp(inputString, "CHANGEZOOM:"))
	{
		float xZoom = getNextArgFloat(fPtr);
		float yZoom = getNextArgFloat(fPtr);
		return SceneAction_ChangeZoom(xZoom, yZoom, GameWorld);
	}
	else if (!strcmp(inputString, "CHANGEZOOMTOSMOOTH:") || !strcmp(inputString, "CHANGECAMERAZOOMTO:") || !strcmp(inputString, "CHANGEZOOMTO:"))
	{
		float xZoom = getNextArgFloat(fPtr);
		float yZoom = getNextArgFloat(fPtr);
		return SceneAction_ChangeZoomTo(xZoom, yZoom, getNextArgFloat(fPtr), GameWorld);
	}
	else if (!strcmp(inputString, "PLACEWALL:") || !strcmp(inputString, "PLACEINVISWALL:") || !strcmp(inputString, "PLACEINVISIBLEWALL:"))
	{
		int xPos = getNextArgInt(fPtr);
		int yPos = getNextArgInt(fPtr);
		int xSize = getNextArgInt(fPtr);
		int ySize = getNextArgInt(fPtr);

		// add 'CamRelative' or 'OnScreen' after the command to place the wall relative to the camera position
		long filePos = ftell(fPtr);

		getNextArg(fPtr, inputString, MAX_LEN); 
		stringToUpper(inputString);

		if (!strcmp(inputString, "CAMRELATIVE") || !strcmp(inputString, "ONSCREEN"))
		{
			return placeInvisibleWall(xPos + (int)GameWorld->MainCamera.CameraX, yPos + (int)GameWorld->MainCamera.CameraY, xSize, ySize, GameWorld);
		}
		else
		{
			fseek(fPtr, filePos, SEEK_SET);

			return placeInvisibleWall(xPos, yPos, xSize, ySize, GameWorld);
		}
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
	else if (!strcmp(inputString, "REPEAT:"))
	{
		int repeatTimes = getNextArgInt(fPtr);

		int count = loadBracketedSceneActions(fPtr, GameWorld);

		return Repeat(repeatTimes, count, GameWorld);
	}
	else if (!strcmp(inputString, "REPEATUNTIL:"))
	{
		ConditionalStatement condition = {0};
		LoadConditionalStatement(fPtr, &condition);

		int count = loadBracketedSceneActions(fPtr, GameWorld);

		return RepeatUntil(condition, count, GameWorld);
	}
	else if (!strcmp(inputString, "REPEATWHILE:"))
	{
		ConditionalStatement condition = {0};
		LoadConditionalStatement(fPtr, &condition);

		int count = loadBracketedSceneActions(fPtr, GameWorld);

		return RepeatWhile(condition, count, GameWorld);	
	}
	else 
	{
		putConsoleError("Couldn't identify '%s' as a scene action. Correct SceneAction syntax is: \n[Action name]: [Number], [String], \"[String with gaps]\"", inputString);
	}


	return NULL;
}

int loadBracketedSceneActions(FILE *fPtr, World *GameWorld)
{
	char buffer[MAX_LEN] = {0};
	getNextArg(fPtr, buffer, MAX_LEN);

	if (buffer[0] != '{')
	{
		return 0;
	}

	SceneAction *firstInstruction = GameWorld->SceneActionQueue;
	while (firstInstruction != NULL && firstInstruction->nextAction != NULL)
	{
		firstInstruction = firstInstruction->nextAction;
	}

	while (!endOfFile(fPtr))
	{
		getNextArg(fPtr, buffer, MAX_LEN);

		if (buffer[0] == '}')
		{
			break;
		}

		loadSceneAction(buffer, GameWorld, fPtr);
	}

	if (firstInstruction == NULL)
	{
		firstInstruction = GameWorld->SceneActionQueue;
	}

	int count = 0;
	while (firstInstruction != NULL && firstInstruction->nextAction != NULL)
	{
		firstInstruction = firstInstruction->nextAction;
		count++;
	}

	return count;
}

int LoadConditionalStatement(FILE *fPtr, ConditionalStatement *input)
{
	input->variableIndex = getNextArgGameFlag(fPtr);

	getNextArgIfExpression(input->expression, fPtr);

	input->comparisonValue = getNextArgInt(fPtr);

	return LEMON_SUCCESS;
}


const static char ActionNames[SCENE_ACTION_COUNT][EVENT_NAME_MAX_LEN] = {
	[SCENE_END] = "End Cutscene",
	[SCENE_REPEAT] = "Repeat",
	[SCENE_REPEAT_UNTIL] = "Repeat Until",
	[SCENE_REPEAT_WHILE] = "Repeat While",
	[SCENE_SKIP_INSTRUCTIONS] = "Skip Instructions",
	[SCENE_IF_STATEMENT] = "If Statement",
	[SCENE_SWITCH_CUTSCENE] = "Switch cutscene",
	[SCENE_TRIGGER_GAME_EVENT] = "Trigger Game Event",
	[SCENE_DISABLE_PLAYER] = "Disable player",
	[SCENE_ENABLE_PLAYER] = "Enable player",
	[SCENE_WAIT] = "Wait",
	[SCENE_CHANGE_VARIABLE_BY] = "Change variable by",
	[SCENE_SET_VARIABLE_TO] = "Set variable to",
	[SCENE_SAY_TEXT] = "Say Text",
	[SCENE_PLACE_INVISIBLE_WALL] = "Place invisible wall",
	[SCENE_CREATE_ACTOR] = "Create Actor",
	[SCENE_RELEASE_ACTOR] = "Release Actor",
	[SCENE_SHOW_ACTOR] = "Show Actor",
	[SCENE_HIDE_ACTOR] = "Hide Actor",
	[SCENE_ANIMATE_ACTOR] = "Animate Actor",
	[SCENE_ANIMATE_ACTOR_WAIT] = "Animate Actor And Wait",
	[SCENE_SET_ACTOR_SPRITE] = "Set actor sprite",
	[SCENE_SET_ACTOR_POS] = "Set Actor Position",
	[SCENE_MOVE_ACTOR] = "Move Actor",
	[SCENE_MOVE_ACTOR_X] = "Move Actor X",
	[SCENE_MOVE_ACTOR_Y] = "Move Actor Y",
	[SCENE_MOVE_ACTOR_TO] = "Move actor to",
	[SCENE_ROTATE_ACTOR] = "Rotate Actor",
	[SCENE_SET_ACTOR_DIRECTION] = "Set actor direction",
	[SCENE_SET_ACTOR_LAYER] = "Set actor layer",
	[SCENE_PLAY_SOUND] = "Play sound",
	[SCENE_SET_CHANNEL_VOL] = "Set channel volume",
	[SCENE_CHANGE_CHANNEL_VOL] = "Change channel volume",
	[SCENE_SET_ACTOR_LAYER] = "Set Actor Layer",
	[SCENE_SET_CAMERA_POS] = "Set Camera Position",
	[SCENE_SET_CAMERA_MODE] = "Set camera mode",
	[SCENE_MOVE_CAMERA] = "Move Camera",
	[SCENE_MOVE_CAMERA_TO] = "Move Camera To",
	[SCENE_MOVE_CAMERA_TO_OBJECT] = "Move Camera To Object",
	[SCENE_SET_CAMERA_ZOOM] = "Set Camera Zoom",
	[SCENE_CHANGE_CAMERA_ZOOM] = "Change Camera Zoom"
};

const char* GetSceneActionName(SceneActionID input)
{
	if (input < 0 || input >= SCENE_ACTION_COUNT)
	{
		return "Unmapped SceneAction";
	}

	return ActionNames[input];
}


int EndCutscene(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	deleteAllSceneActions(GameWorld);

	if (GameWorld->CurrentCutscene == NO_CUTSCENE)
	{
		return EXECUTION_UNNECESSARY;
	}

	GameWorld->CurrentCutscene = NO_CUTSCENE;
	
	if (GameWorld->GameState == CUTSCENE)
	{
		GameWorld->GameState = GAMEPLAY;
	}

	Object *PlayerObject = GameWorld->Player.PlayerPtr;

	if (PlayerObject != NULL)
	{
		PlayerObject->reserved &= ~RFLAG_CUTSCENE_IMMUNITY;
	
		if (PlayerObject->State == ACTOR_STATE)
		{
			PlayerObject->State = DEFAULT_STATE;
		}
	}

	// By default, any objects that were not manually released from Actor state will be deleted
	Object *currentObject = GameWorld->ObjectList.firstObject;

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


SceneAction* SceneAction_SwitchCutscene(int sceneID, World *GameWorld)
{
	if (sceneID < 1 || GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_SWITCH_CUTSCENE, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.SceneID = sceneID;

	return newAction;
}

SceneAction* SceneAction_TriggerGameEvent(GameEvent *inputEvent, World *GameWorld)
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

	newAction->ActionData.WaitTicks[0] = (int)(seconds * EngineSettings.GameTicksPerSecond);
	newAction->ActionData.WaitTicks[1] = newAction->ActionData.WaitTicks[0];

	return newAction;
}

SceneAction* Repeat(int repeatTimes, int instructions, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_REPEAT, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.loop.repeatTimes = repeatTimes;
	newAction->ActionData.loop.instructionCount = instructions;

	return newAction;
}

SceneAction* RepeatUntil(ConditionalStatement condition, int instructions, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_REPEAT_UNTIL, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.loopUntil.condition = condition;
	newAction->ActionData.loopUntil.instructionCount = instructions;

	return newAction;
}

SceneAction* RepeatWhile(ConditionalStatement condition, int instructions, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_REPEAT_WHILE, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.loopUntil.condition = condition;
	newAction->ActionData.loopUntil.instructionCount = instructions;

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

	SceneAction *newAction = createSceneAction(SCENE_ANIMATE_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.actor.name, objName);
	strcpy(newAction->ActionData.actor.animationName, animName);
	newAction->ActionData.actor.loopCount = loopCount;

	return newAction;
}

SceneAction* AnimateActorAndWait(char objName[], const char animName[], int loopCount, World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL || animName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_ANIMATE_ACTOR_WAIT, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	ObjectMeta *meta = &newAction->ActionData.actor;
	strcpy(meta->name, objName);
	LemonStrncpy(meta->animationName, animName, ANIMATION_NAME_LENGTH);
	meta->loopCount = loopCount;

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

	Object *actorObj = FindObject(objName, &GameWorld->ObjectList);
	if (actorObj == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_SET_ACTOR_SPRITE, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.actor.name, objName);
	LemonStrncpy(newAction->ActionData.actor.spriteName, spriteName, MAX_LEN);

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

	SceneAction *newAction = createSceneAction(SCENE_SET_ACTOR_POS, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.actor.name, objName);
	newAction->ActionData.actor.xPos = xPosition;
	newAction->ActionData.actor.yPos = yPosition;

	return newAction;
}


SceneAction* MoveActor(char objName[], float xMovement, float yMovement, World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_MOVE_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}
			
	strcpy(newAction->ActionData.actor.name, objName);
	newAction->ActionData.actor.xPos = xMovement;
	newAction->ActionData.actor.yPos = yMovement;

	return newAction;
}


SceneAction* MoveActorX(char objName[], float xMovement, World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_MOVE_ACTOR_X, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.actor.name, objName);
	newAction->ActionData.actor.xPos = xMovement;

	return newAction;
}


SceneAction* MoveActorY(char objName[], float yMovement, World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_MOVE_ACTOR_Y, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.actor.name, objName);
	newAction->ActionData.actor.yPos = yMovement;

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

	SceneAction *newAction = createSceneAction(SCENE_SET_ACTOR_DIRECTION, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.actor.name, objName);
	newAction->ActionData.actor.direction = rotation;

	return newAction;
}


SceneAction* RotateActor(char objName[], double rotation, World *GameWorld)
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_ROTATE_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.actor.name, objName);
	newAction->ActionData.actor.direction = rotation;

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

	SceneAction *newAction = createSceneAction(SCENE_HIDE_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.actor.name, objName);

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

	SceneAction *newAction = createSceneAction(SCENE_SHOW_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.actor.name, objName);


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

	SceneAction *newAction = createSceneAction(SCENE_SET_ACTOR_LAYER, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	ObjectMeta *meta = &newAction->ActionData.actor;
	strcpy(meta->name, objName);
	meta->layer = destLayer;

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

	if (FindObject(objName, &GameWorld->ObjectList) != NULL)
	{
		// If an object with this name already exists, just get a reference to it
		// to have its state set to 'ACTOR' and position set precisely when its scheduled to
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_CREATE_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	ObjectMeta *meta = &newAction->ActionData.actor;
	strcpy(meta->name, objName);
	meta->objectID = actorID;
	meta->xPos = xPos;
	meta->yPos = yPos;

	return newAction;
}

SceneAction* ReleaseActor(char objName[], World *GameWorld)		// use if you dont want an actor to be deleted when the cutscene ends
{
	if (GameWorld == NULL || objName == NULL)
	{
		return NULL;
	}

	if (strlen(objName) >= OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_RELEASE_ACTOR, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.actor.name, objName);

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

	newAction->ActionData.actor.xPos = xPos;
	newAction->ActionData.actor.yPos = yPos;
	newAction->ActionData.actor.xSize = xSize;
	newAction->ActionData.actor.ySize = ySize;

	return newAction;
}


SceneAction* SceneAction_PlaySound(char soundName[], ChannelName soundChannel, float volume, World *GameWorld)
{
	if (GameWorld == NULL || soundName == NULL)
	{
		return NULL;
	}

	if (strlen(soundName) >= MAX_LEN)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_PLAY_SOUND, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	strcpy(newAction->ActionData.soundData.soundName, soundName);
	newAction->ActionData.soundData.channel = soundChannel;
	newAction->ActionData.soundData.volume = volume;

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
	newAction->ActionData.soundData.channel = soundChannel;
	newAction->ActionData.soundData.volume = newVolume;

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
	newAction->ActionData.soundData.channel = soundChannel;
	newAction->ActionData.soundData.volume = change;

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

SceneAction* SceneAction_MoveCamera(float xVel, float yVel, World *GameWorld)
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

	return newAction;
}


SceneAction* SceneAction_MoveCameraTo(float xPos, float yPos, float coefficient, World *GameWorld)
{
	if (GameWorld == NULL || coefficient < 0.1)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_MOVE_CAMERA_TO, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.CameraData[0] = xPos;
	newAction->ActionData.CameraData[1] = yPos;
	newAction->ActionData.CameraData[2] = coefficient;

	return newAction;
}

SceneAction* SceneAction_MoveCameraToObject(char objectName[], float coefficient, World *GameWorld)
{
	if (GameWorld == NULL || objectName == NULL)
	{
		return NULL;
	}

	if (strlen(objectName) > OBJECT_NAME_LENGTH)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_MOVE_CAMERA_TO_OBJECT, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.CameraData[0] = 0.0;
	newAction->ActionData.CameraData[1] = 0.0;
	newAction->ActionData.CameraData[2] = coefficient;


	return newAction;
}

SceneAction* SceneAction_SetCameraMode(int mode, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_SET_CAMERA_MODE, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.cameraMode = mode;

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


SceneAction* SceneAction_ChangeZoom(float zoomX, float zoomY, World *GameWorld)
{
	if (GameWorld == NULL)
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

	return newAction;
}

SceneAction* SceneAction_ChangeZoomTo(float zoomX, float zoomY, float coefficient, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_CHANGE_CAMERA_ZOOM_TO, GameWorld);
	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->ActionData.zoomScales[0] = zoomX;
	newAction->ActionData.zoomScales[1] = zoomY;
	newAction->ActionData.zoomScales[2] = coefficient;

	return newAction;
}


SceneAction* createSceneAction(SceneActionID newActionID, World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->SceneActionCount >= EngineSettings.MaxSceneActions)
	{
		return NULL;
	}

	if (newActionID >= SCENE_ACTION_COUNT || newActionID < 0)
	{
		return NULL;
	}

	SceneAction *newAction = malloc(sizeof(SceneAction));

	if (newAction == NULL)
	{
		return NULL;
	}

	memset(newAction, 0, sizeof(SceneAction));

	if (GameWorld->SceneActionQueue == NULL)
	{
		GameWorld->SceneActionQueue = newAction;
		GameWorld->nextSceneAction = newAction;
		newAction->prevAction = NULL;
	}
	else
	{
		SceneAction *actionPtr = GameWorld->SceneActionQueue;

		while (actionPtr->nextAction != NULL)
		{
			actionPtr = actionPtr->nextAction;
		}

		actionPtr->nextAction = newAction;
		newAction->prevAction = actionPtr;
	}

	GameWorld->SceneActionCount++;

	newAction->nextAction = NULL;

	newAction->ActionID = newActionID;

	if (DebugSettings.showSceneActions)
	{
		putConsoleTS("Running scene action ID: %d (%s)", newActionID, GetSceneActionName(newActionID));
	}

	return newAction;
}


SceneAction* deleteSceneAction(SceneAction *deleteAction, World *GameWorld)
{
	if (deleteAction == NULL || GameWorld == NULL)
	{
		return NULL;
	}

	if (deleteAction->ActionID == SCENE_SAY_TEXT)
	{
		TextBox *text = &deleteAction->ActionData.sceneText;

		if (text->textTypeSetting == TEXTBOX_OPTION_PROMPT)
		{
			TextOptionPrompt *optionData = &text->textTypeData.OptionPrompt;
			if (optionData->optionTriggers != NULL)
			{
				free(optionData->optionTriggers);
				optionData->optionTriggers = NULL;
			}
		}
		else if (text->textTypeSetting == TEXTBOX_TRIGGER_EVENT)
		{
			if (text->textTypeData.TriggerEvent != NULL)
			{
				free(text->textTypeData.TriggerEvent);
				text->textTypeData.TriggerEvent = NULL;
			}
		}

		deleteTextBox(text, GameWorld);
	}

	SceneAction *prevAction = deleteAction->prevAction;
	SceneAction *nextAction = deleteAction->nextAction;

	if (prevAction != NULL)
	{
		prevAction->nextAction = nextAction;
	}
	else
	{
		GameWorld->SceneActionQueue = nextAction;
	}

	if (nextAction != NULL)
	{
		nextAction->prevAction = prevAction;
	}

	GameWorld->SceneActionCount--;
	free(deleteAction);


	return nextAction;
}


int deleteAllSceneActions(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->SceneActionQueue == NULL)
	{
		return MISSING_DATA;
	}

	while (GameWorld->SceneActionQueue != NULL)
	{
		deleteSceneAction(GameWorld->SceneActionQueue, GameWorld);
	}

	GameWorld->nextSceneAction = NULL;

	return LEMON_SUCCESS;
}