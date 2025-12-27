#include "UIObjects.h"


int SpawnHUD(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}


	// Create a default particle and UIElement object to initialise their sprite sets to avoid lag during gameplay 
	//AddParticle(GameWorld, 0, 0, EMPTY_PARTICLE, 0, 0);
	//AddObject(GameWorld, UI_ELEMENT, 0, 0, 0, 0, UNDEFINED_UI_ELEMENT, 0, 0, 0, 0);


	return LEMON_SUCCESS;
}


int HideHUD(ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	Object *currentObject = ObjectList->firstObject;

	while (currentObject != NULL)
	{
		if (currentObject->layer == HUD && currentObject->ObjectDisplay != NULL)
		{
			currentObject->ObjectDisplay->hidden = true;
		}

		currentObject = currentObject->nextObject;
	}


	return LEMON_SUCCESS;
}


int ShowHUD(ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	Object *currentObject = ObjectList->firstObject;

	while (currentObject != NULL)
	{
		if (currentObject->layer == HUD && currentObject->ObjectDisplay != NULL)
		{
			currentObject->ObjectDisplay->hidden = false;
		}

		currentObject = currentObject->nextObject;
	}


	return LEMON_SUCCESS;
}


int InitialiseUIElement(Object *UIElement, World *GameWorld)
{
	if (GameWorld == NULL || UIElement == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	UIElement->layer = HUD;
	UIElement->ObjectBox->collideLayer = HUD;
	UIElement->ObjectBox->solid = UNSOLID;


	switch(UIElement->arg1)
	{
	case UNDEFINED_UI_ELEMENT:
		MarkObjectForDeletion(UIElement);
		break;


	case FADEOUT:
		UIElement->ObjectDisplay->transparencyEffect = 1.0;
		UIElement->ObjectBox->xFlip = -1;
		UIElement->ObjectBox->yFlip = -1;
		UIElement->arg2 = 0;
		switchSpriteByName("FadeOut", 0, UIElement->ObjectDisplay);
		if (UIElement->prevObject != NULL && UIElement->prevObject->ObjectID == UI_ELEMENT && UIElement->prevObject->arg1 == FADEOUT)
		{
			MarkObjectForDeletion(UIElement);
		}
		break;


	case PAUSE_BACKGROUND:
		switchSpriteByName("PauseBackground", 0, UIElement->ObjectDisplay);
		break;


	case PAUSE_HEADER:
		switchSpriteByName("PauseHeader", 0, UIElement->ObjectDisplay);
		break;


	case SETTINGS_HEADER:
		switchSpriteByName("SettingsHeader", 0, UIElement->ObjectDisplay);
		break;


	case PAUSE_MENU_CONTROLLER:
	{
		double xPosOrigin = (3 * X_TILESCALE) - (screenWidth >> 1);
		double yPosOrigin = 0.0;
		UIElement->ObjectBox->xPos = xPosOrigin;
		UIElement->ObjectBox->yPos = yPosOrigin;
		UIElement->arg4 = (int)yPosOrigin;		// arg4 is y pos of first option

		//AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);

		AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin, yPosOrigin + 128, 200, 40, PAUSE_HEADER, 0, 0, 0, 0);

		yPosOrigin -= 5;
		xPosOrigin -= 130;
		AddOptionButton("ResumeGame", xPosOrigin, yPosOrigin, UIElement, GameWorld);

		yPosOrigin -= 128;
		AddOptionButton("Settings", xPosOrigin, yPosOrigin, UIElement, GameWorld);

		yPosOrigin -= 128;
		AddOptionButton("QuitGame", xPosOrigin, yPosOrigin, UIElement, GameWorld);
		
		switchObjectSpriteName("OptionCursor", UIElement);
		SetDrawPriorityToFront(GameWorld->ObjectList, UIElement);
	} break;


	case SETTINGS_MENU_CONTROLLER:
	{
		double xPosOrigin = (3 * X_TILESCALE) - (screenWidth >> 1);
		double yPosOrigin = (4 * Y_TILESCALE);
		UIElement->ObjectBox->xPos = xPosOrigin;
		UIElement->ObjectBox->yPos = yPosOrigin;
		UIElement->arg4 = (int)yPosOrigin;

		//AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);
		AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + X_TILESCALE, yPosOrigin + (Y_TILESCALE * 3), 100, 40, SETTINGS_HEADER, 0, 0, 0, 0);
		
		yPosOrigin -= 5;
		xPosOrigin -= 130;
		AddOptionButton("VideoSettings", xPosOrigin, yPosOrigin, UIElement, GameWorld);

		yPosOrigin -= 128;
		AddOptionButton("SoundSettings", xPosOrigin, yPosOrigin, UIElement, GameWorld);

		yPosOrigin -= 128;
		AddOptionButton("Settings", xPosOrigin, yPosOrigin, UIElement, GameWorld);

		yPosOrigin -= 128;
		AddOptionButton("BackOption", xPosOrigin, yPosOrigin, UIElement, GameWorld);
		
		switchObjectSpriteName("OptionCursor", UIElement);
		SetDrawPriorityToFront(GameWorld->ObjectList, UIElement);
	} break;


	case VIDEO_SETTINGS_CONTROLLER:
	{
		double xPosOrigin = (3 * X_TILESCALE) - (screenWidth >> 1);
		double yPosOrigin = (6 * Y_TILESCALE);
		UIElement->ObjectBox->xPos = xPosOrigin;
		UIElement->ObjectBox->yPos = yPosOrigin;
		UIElement->arg4 = (int)yPosOrigin;

		//AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);
		AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + X_TILESCALE, yPosOrigin + (Y_TILESCALE * 3), 100, 40, SETTINGS_HEADER, 0, 0, 0, 0);
		
		yPosOrigin -= 5;
		xPosOrigin -= 130;
		AddOptionButton("Settings", xPosOrigin, yPosOrigin, UIElement, GameWorld);
		
		yPosOrigin -= 128;
		AddOptionButton("Settings", xPosOrigin, yPosOrigin, UIElement, GameWorld);
		
		yPosOrigin -= 128;
		AddOptionButton("Settings", xPosOrigin, yPosOrigin, UIElement, GameWorld);

		yPosOrigin -= 128;
		AddOptionButton("Settings", xPosOrigin, yPosOrigin, UIElement, GameWorld);
		
		yPosOrigin -= 128;
		AddOptionButton("BackOption", xPosOrigin, yPosOrigin, UIElement, GameWorld);
		
		switchObjectSpriteName("OptionCursor", UIElement);
		SetDrawPriorityToFront(GameWorld->ObjectList, UIElement);
	} break;


	case SOUND_SETTINGS_CONTROLLER:
	{
		double xPosOrigin = (3 * X_TILESCALE) - (screenWidth >> 1);
		double yPosOrigin = (4 * Y_TILESCALE);
		UIElement->ObjectBox->xPos = xPosOrigin;
		UIElement->ObjectBox->yPos = yPosOrigin;
		UIElement->arg4 = (int)yPosOrigin;

		//AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);
		AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + X_TILESCALE, yPosOrigin + (Y_TILESCALE * 3), 100, 40, SETTINGS_HEADER, 0, 0, 0, 0);
		
		yPosOrigin -= 5;
		xPosOrigin -= 130;
		AddOptionButton("Settings", xPosOrigin, yPosOrigin, UIElement, GameWorld);
		
		yPosOrigin -= 128;
		AddOptionButton("Settings", xPosOrigin, yPosOrigin, UIElement, GameWorld);
		
		yPosOrigin -= 128;
		AddOptionButton("Settings", xPosOrigin, yPosOrigin, UIElement, GameWorld);
		
		yPosOrigin -= 128;
		AddOptionButton("BackOption", xPosOrigin, yPosOrigin, UIElement, GameWorld);
	
		switchObjectSpriteName("OptionCursor", UIElement);
		SetDrawPriorityToFront(GameWorld->ObjectList, UIElement);
	} break;


	case MOUSE_CURSOR:
		switchSpriteByName("MouseCursor", USE_CURRENT_SPRITESET, UIElement->ObjectDisplay);
		AddFrameUpdateFunction(&UpdateCursor, UIElement, GameWorld->ObjectList);
		UIElement->layer = FRONT_LAYER;
		break;

	default:
		break;
	}


	return LEMON_SUCCESS;
}


int LoadUISprites(SpriteSet *newSet)
{
	loadObjectSprite("OBJ_Missing", newSet, TILE);
	loadObjectSprite("OptionCursor", newSet, SINGLE);
	loadObjectSprite("BackButton", newSet, SINGLE);

	loadObjectSprite("GamePaused_Background", newSet, SINGLE);
	loadObjectSprite("GamePaused_Header", newSet, SINGLE);
	loadObjectSprite("ResumeGame", newSet, SINGLE);
	loadObjectSprite("ResumeGame_Highlighted", newSet, SINGLE);
	loadObjectSprite("Settings", newSet, SINGLE);
	loadObjectSprite("Settings_Highlighted", newSet, SINGLE);
	loadObjectSprite("QuitGame", newSet, SINGLE);
	loadObjectSprite("QuitGame_Highlighted", newSet, SINGLE);

	loadObjectSprite("Settings_Background", newSet, SINGLE);
	loadObjectSprite("Settings_Header", newSet, SINGLE);
	loadObjectSprite("RenderQuality_Option", newSet, SINGLE);
	loadObjectSprite("Volume_Option", newSet, SINGLE);
	loadObjectSprite("Test_Option", newSet, SINGLE);

	return LEMON_SUCCESS;
}


int LoadUITextSprites(SpriteSet *newSet)
{
	loadSpriteIntoSpriteSet("Test_Face", "TextDisplay", newSet, TILE);

	loadSpriteIntoSpriteSet("TextBox_Basic", "TextDisplay", newSet, TILE_FAST);
	loadSpriteIntoSpriteSet("TextBox_White", "TextDisplay", newSet, TILE_FAST);

	loadSpriteIntoSpriteSet("Text_0", "TextDisplay", newSet, TILE_FAST);
	loadSpriteIntoSpriteSet("Text_1", "TextDisplay", newSet, TILE);

	return LEMON_SUCCESS;
}


int UpdateUIElement(World *GameWorld, Object *UIElement)
{
	if (GameWorld == NULL || UIElement == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}


	switch (UIElement->arg1)
	{
	case PAUSE_MENU_CONTROLLER:
		PauseMenu(UIElement, GameWorld);
		break;

	case SETTINGS_MENU_CONTROLLER:
		SettingsMenuControl(UIElement, GameWorld);
		break;

	case VIDEO_SETTINGS_CONTROLLER:
		VideoSettingsControl(UIElement, GameWorld);
		break;

	case SOUND_SETTINGS_CONTROLLER:
		SoundSettingsControl(UIElement, GameWorld);
		break;

	case OPTION_BUTTON:
		UpdateOptionButton(UIElement);
		break;

	case FADEOUT:
		if (UIElement->arg2 == 0)
		{
			UIElement->ObjectDisplay->transparencyEffect -= 0.03;
			if (UIElement->ObjectDisplay->transparencyEffect < 0.01)
			{
				UIElement->ObjectDisplay->transparencyEffect = 0.0;
				UIElement->arg2 = 1;
			}
		}
		else if (UIElement->arg2 > 130)
		{
			UIElement->ObjectDisplay->transparencyEffect += 0.04;
			if (UIElement->ObjectDisplay->transparencyEffect > 0.99)
			{
				MarkObjectForDeletion(UIElement);
			}
		}
		else
		{
			UIElement->arg2++;
		}
		break;

	case MOUSE_CURSOR:
		UIElement->ObjectDisplay->pixelXOffset = 30;
		break;

	default:
		break;
	}


	return LEMON_SUCCESS;
}


int UpdateCursor(Object *Cursor, World *GameWorld)
{
	if (Cursor == NULL || GameWorld == NULL || Cursor->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	Cursor->ObjectBox->xPos = getMouseXZoom(GameWorld->MainCamera) - (Cursor->ObjectBox->xSize >> 1) + GameWorld->MainCamera.CameraX;
	Cursor->ObjectBox->yPos = getMouseYZoom(GameWorld->MainCamera) - (Cursor->ObjectBox->ySize >> 1) + GameWorld->MainCamera.CameraY;

	if (Cursor->nextObject != NULL)
	{
		SetDrawPriorityToFront(GameWorld->ObjectList, Cursor);
	}

	if (GameWorld->GamePaused == 1)
	{
		return ACTION_DISABLED;
	}

	PhysicsRect *PlayerBox = GameWorld->Player.PlayerBox;

	if (MouseInput.LeftButton == 1 && PlayerBox != NULL)
	{
		AcknowledgeButton(MOUSE_LEFT);

		int bulletXPos = (int)PlayerBox->xPos + (PlayerBox->xSize >> 1);
		int bulletYPos = (int)PlayerBox->yPos + (PlayerBox->ySize >> 1);

		Object *Bullet = AddNamedObject(GameWorld, "PlayerBullet", PROJECTILE, bulletXPos, bulletYPos);

		PointObjectToMouse(Bullet, GameWorld, ROTATE_ALL);

		//setScaleSize(Cursor, 0.75);
	}

	if (MouseInput.RightButton == 1)
	{
		MarkObjectForDeletion(Cursor);
	}

	return LEMON_SUCCESS;
}


int PauseMenu(Object *MenuController, World *GameWorld)
{
	if (MenuController == NULL || GameWorld == NULL || MenuController->ObjectID != UI_ELEMENT)
	{
		return MISSING_DATA;
	}


	if (MenuControl(MenuController, GameWorld))
	{
		switch (MenuController->arg2)
		{
			case 1:
				if (AddObject(GameWorld, UI_ELEMENT, 0, 0, 0, 0, SETTINGS_MENU_CONTROLLER, 0, 0, 0, 0) != NULL)
				{
					MarkObjectForDeletion(MenuController);
				}
				break;

			case 2:
				GameWorld->GameState = CLOSE_GAME;
				break;

			default:
				ResumeGame(GameWorld);
				break;
		}
	}


	if (GameWorld->GamePaused == 0 && GameWorld->GameState != IN_MENU)
	{
		MarkObjectForDeletion(MenuController);
	}

	return LEMON_SUCCESS;
}


int MenuControl(Object *MenuController, World *GameWorld)
{
	if (MenuController == NULL || GameWorld == NULL || MenuController->ObjectID != UI_ELEMENT)
	{
		return 0;
	}

	// arg2: Option selected
	// arg3: Number of options
	// arg4: Y Pos for first option
	// arg5: refresh flag

	if (MenuController->arg5 == 1)
	{
		MenuController->arg5 = 0;
		MarkObjectForDeletion(MenuController);
		Object *newMenu = AddObject(GameWorld, UI_ELEMENT, 0, 0, 0, 0, MenuController->arg1, 0, 0, 0, 0);

		if (newMenu != NULL)
		{
			newMenu->arg2 = MenuController->arg2;
			newMenu->ObjectBox->yPos = MenuController->ObjectBox->yPos;
			return 0;
		}
	}


	if (keyboard[LMN_DOWN])
	{
		keyboard[LMN_DOWN] = 0;

		MenuController->arg2++;
		MenuController->Interrupt = INTERACTION_INTERRUPT;

		if (MenuController->arg2 >= MenuController->arg3)
		{
			MenuController->arg2 = 0;
		}
	}

	if (keyboard[LMN_UP])
	{
		keyboard[LMN_UP] = 0;

		MenuController->arg2--;
		MenuController->Interrupt = INTERACTION_INTERRUPT;

		if (MenuController->arg2 < 0)
		{
			MenuController->arg2 = MenuController->arg3 - 1;
		}
	}

	if (MenuController->Interrupt == INTERACTION_INTERRUPT)
	{
		MenuController->ObjectBox->yPos = (double)MenuController->arg4 - (MenuController->arg2 * 128);
		MenuController->Interrupt = NO_INTERRUPT;
	}


	if (keyboard[LMN_MENU_CONFIRM] == 1 || MenuController->Interrupt == MOUSECLICK_INTERRUPT)
	{
		AcknowledgeHeldButtons();
		MenuController->Interrupt = NO_INTERRUPT;

		return 1;
	}


	return 0;
}


int SettingsMenuControl(Object *MenuController, World *GameWorld)
{
	if (MenuController == NULL || GameWorld == NULL || MenuController->ObjectID != UI_ELEMENT)
	{
		return MISSING_DATA;
	}


	if (MenuControl(MenuController, GameWorld))
	{
		switch (MenuController->arg2)
		{
			case 0:
				if (AddObject(GameWorld, UI_ELEMENT, 0, 0, 0, 0, VIDEO_SETTINGS_CONTROLLER, 0, 0, 0, 0) != NULL)
				{
					MarkObjectForDeletion(MenuController);
				}
				break;

			case 1:
				if (AddObject(GameWorld, UI_ELEMENT, 0, 0, 0, 0, SOUND_SETTINGS_CONTROLLER, 0, 0, 0, 0) != NULL)
				{
					MarkObjectForDeletion(MenuController);
				}
				break;

			case 2:

				break;


			default:
				PauseGame(GameWorld);
				MarkObjectForDeletion(MenuController);
				break;
		}
	}

	if (GameWorld->GamePaused == 0 && GameWorld->GameState != IN_MENU)
	{
		MarkObjectForDeletion(MenuController);
	}


	return LEMON_SUCCESS;
}


int VideoSettingsControl(Object *MenuController, World *GameWorld)
{
	if (MenuController == NULL || GameWorld == NULL || MenuController->ObjectID != UI_ELEMENT)
	{
		return MISSING_DATA;
	}

	if (MenuControl(MenuController, GameWorld))
	{
		switch (MenuController->arg2)
		{
			case 0:
				// THIS IS TEMPORARY - NEED TO IMPLEMENT SOME KIND OF SCREEN SIZE SELECTOR GUI
				GameWorld->GameEvent = CHANGE_SCREEN_SIZE_SCALE;
				GameWorld->GameEventData.screenDimensions[0] = 640;		// new width
				GameWorld->GameEventData.screenDimensions[1] = 360;		// new height
				break;

			case 1:
				GameWorld->GameEvent = CHANGE_SCREEN_SIZE_SCALE;
				GameWorld->GameEventData.screenDimensions[0] = 1280;	
				GameWorld->GameEventData.screenDimensions[1] = 720;	
				break;

			case 2:
				GameWorld->GameEvent = CHANGE_SCREEN_SIZE;
				GameWorld->GameEventData.screenDimensions[0] = 640;	
				GameWorld->GameEventData.screenDimensions[1] = 360;	
				break;

			case 3:
				GameWorld->GameEvent = CHANGE_SCREEN_SIZE;
				GameWorld->GameEventData.screenDimensions[0] = 1280;	
				GameWorld->GameEventData.screenDimensions[1] = 720;	
				break;

			default:
				if (AddObject(GameWorld, UI_ELEMENT, 0, 0, 0, 0, SETTINGS_MENU_CONTROLLER, 0, 0, 0, 0) != NULL)
				{
					MarkObjectForDeletion(MenuController);
				}
				break;
		}

		MenuController->arg5 = 1;
	}

	if (GameWorld->GamePaused == 0 && GameWorld->GameState != IN_MENU)
	{
		MarkObjectForDeletion(MenuController);
	}


	return LEMON_SUCCESS;
}



int SoundSettingsControl(Object *MenuController, World *GameWorld)
{
	if (MenuController == NULL || GameWorld == NULL || MenuController->ObjectID != UI_ELEMENT)
	{
		return MISSING_DATA;
	}

	if (MenuControl(MenuController, GameWorld))
	{
		switch (MenuController->arg2)
		{
			case 0:
				SetAllVolume(1.0);
				break;

			case 1:
				SetAllVolume(0.0);
				break;

			case 2:
				SetAllVolume(0.5);
				break;


			default:
				if (AddObject(GameWorld, UI_ELEMENT, 0, 0, 0, 0, SETTINGS_MENU_CONTROLLER, 0, 0, 0, 0) != NULL)
				{
					MarkObjectForDeletion(MenuController);
				}
				break;
		}
	}

	if (GameWorld->GamePaused == 0 && GameWorld->GameState != IN_MENU)
	{
		MarkObjectForDeletion(MenuController);
	}


	return LEMON_SUCCESS;
}


Object* AddOptionButton(const char spriteName[], int xPos, int yPos, Object *MenuController, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	Object *createdOption = AddObjectWithParent(GameWorld, MenuController, UI_ELEMENT, xPos, yPos, 32, 32, OPTION_BUTTON, MenuController->arg3, 0, 0, 0);

	if (createdOption == NULL)
	{
		return NULL;
	}

	switchSpriteByName(spriteName, USE_CURRENT_SPRITESET, createdOption->ObjectDisplay);
	matchBoxToDisplayDimensions(createdOption);
	UpdateOptionButton(createdOption);

	createdOption->ObjectDisplay->size = 1.0;
	MenuController->arg3++;		// number of options

	return createdOption;
}


int UpdateOptionButton(Object *Button)
{
	if (Button == NULL || Button->ParentObject == NULL || Button->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	if (Button->ObjectDisplay->spriteBuffer == NULL || strcmp(Button->ObjectDisplay->spriteBuffer->spriteName, "Missing") == 0)
	{
		Button->State = STATIC;
		return INVALID_DATA;
	}

	// Detect mouse overlap
	if (MouseOverlappingSprite(Button, NULL) == 1)
	{	
		if (Button->Interrupt == NO_INTERRUPT)					// Mouse hover
		{
			Button->ParentObject->arg2 = Button->arg2;
			Button->Interrupt = INTERACTION_INTERRUPT;
			Button->ParentObject->Interrupt = INTERACTION_INTERRUPT;
		}

		// Detect mouse click
		if (MouseInput.LeftButton == 1)				// mouse down
		{
			Button->Interrupt = MOUSEDOWN_INTERRUPT;
		}

		if (Button->Interrupt == MOUSEDOWN_INTERRUPT && MouseInput.LeftButton == 0)	// mouse release/click
		{
			Button->ParentObject->Interrupt = MOUSECLICK_INTERRUPT;
			Button->Interrupt = INTERACTION_INTERRUPT;
		}
	}
	else
	{
		Button->Interrupt = NO_INTERRUPT;
	}

	// animate
	if (Button->ParentObject->arg2 == Button->arg2)
	{
		if (keyboard[LMN_MENU_CONFIRM] == 1 || Button->Interrupt == MOUSEDOWN_INTERRUPT)
		{
			smoothSizeChangeTo(Button, 1.10, 2.0);
		}
		else
		{
			smoothSizeChangeTo(Button, 1.20, 5.0);
		}
		
		if (Button->arg3 == 0)
		{
			Button->ObjectDisplay->currentSprite++;
			Button->arg3 = 1;
		}
	}
	else
	{
		smoothSizeChangeTo(Button, 1.00, 4.0);

		if (Button->arg3 == 1)
		{
			Button->ObjectDisplay->currentSprite--;
			Button->arg3 = 0;
		}
	}


	return LEMON_SUCCESS;
}


int InitialiseUIText(Object *UIText, World *GameWorld)
{
	if (GameWorld == NULL || UIText == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	UIText->layer = HUD;
	UIText->ObjectBox->collideLayer = HUD;
	UIText->ObjectBox->solid = UNSOLID;

	TextInstance *currentText = GameWorld->TextQueue;

	switch(UIText->arg1)
	{
		case UNDEFINED_UI_ELEMENT:
		MarkObjectForDeletion(UIText);
		break;

		case TEXTOPTION_CURSOR:
			UIText->arg2 = -1;
			initialiseTextCharacter(UIText, '>', GameWorld);

			UpdateUIText(GameWorld, UIText);
		break;

		case TEXT_CHARACTER:
		int charValue = currentText->textPhrase[currentText->currentChar];
		initialiseTextCharacter(UIText, charValue, GameWorld);
		break;

		case TEXT_PORTRAIT:
		UIText->ParentLink = POSITION_LINK;
		break;

		default:
		break;
	}

	return LEMON_SUCCESS;
}


int UpdateUIText(World *GameWorld, Object *UIText)
{
	if (GameWorld == NULL || UIText == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}


	TextInstance *currentText = GameWorld->TextQueue;

	switch (UIText->arg1)
	{
	case TEXTOPTION_CURSOR:
		if (currentText == NULL || currentText->boxPtr == NULL || currentText->textTypeSetting != OPTION_PROMPT)
		{
			MarkObjectForDeletion(UIText);
			break;
		}

		struct TextOptionPrompt *optionPrompt = &currentText->textTypeData.OptionPrompt;
		int option = optionPrompt->SelectedOption;

		if (optionPrompt != NULL && UIText->arg2 != option)
		{
			UIText->ObjectBox->xPos = currentText->boxPtr->ObjectBox->xPos + currentText->boxOffsetX;
			UIText->ObjectBox->yPos = optionPrompt->OptionYPositions[option] + currentText->boxPtr->ObjectBox->yPos;
			UIText->arg2 = option;
			playTextVoice(currentText);
		}
		break;

	case TEXT_CHARACTER:
		if (GameWorld->TextQueue == NULL)
		{
			MarkObjectForDeletion(UIText);
		}
		break;

	default:
		break;
	}

	return LEMON_SUCCESS;
}


TextInstance* SayText(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld)
{
	TextInstance *newText = CreateText(inputPhrase, GameWorld);

	ApplyTextPresets(newText, Portrait, inputPreset);

	return newText;
}


// Triggerable events used here and in the TRIGGER_EVENT text type expect a specific type of function and input, as defined by AddFunctionArgumentsToTriggerEvent
TextInstance* SayTextOption(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld, int numberOfOptions, ...)
{
	if (inputPhrase == NULL || strlen(inputPhrase) >= OPTION_TEXT_MAX_LEN || numberOfOptions < 1 || numberOfOptions > MAX_OPTIONS)
	{
		return NULL;
	}

	TextInstance *newText = CreateText(inputPhrase, GameWorld);

	ApplyTextPresets(newText, Portrait, inputPreset);

	if (newText == NULL)
	{
		return NULL;
	}

	newText->textTypeSetting = OPTION_PROMPT;
	newText->textTypeData.OptionPrompt.numberOfOptions = numberOfOptions;
	newText->textTypeData.OptionPrompt.SelectedOption = 0;
	struct TextOptionPrompt *textData = &newText->textTypeData.OptionPrompt;

	va_list args;
    va_start(args, numberOfOptions);

    struct TextEventTrigger *eventTrigger;
    for (int i = 0; i < numberOfOptions; i++)
    {
    	eventTrigger = &textData->optionTriggers[i];

    	strcpy(textData->optionNames[i], va_arg(args, char*));
    	
    	eventTrigger->TriggerFunction = va_arg(args, void*);

    	AddFunctionArgumentsToTriggerEvent(eventTrigger, va_arg(args, void*));
    }
    va_end(args);

	return newText;
}


int AddTriggerableEventToText(TextInstance *inputText, void *triggerFunction, void *functionInput)
{
	if (inputText == NULL || triggerFunction == NULL)
	{
		return MISSING_DATA;
	}

	if (inputText->textTypeSetting != REGULAR_TEXT)
	{
		// Its already been loaded with custom data here, so it should refuse to alter/overwrite it
		return INVALID_DATA;
	}

	inputText->textTypeSetting = TRIGGER_EVENT;

	struct TextEventTrigger *eventTrigger = &inputText->textTypeData.TriggerEvent;
	eventTrigger->TriggerFunction = triggerFunction; 

   	AddFunctionArgumentsToTriggerEvent(eventTrigger, functionInput);
    	
	return LEMON_SUCCESS;
}


int AddFunctionArgumentsToTriggerEvent(struct TextEventTrigger *eventTrigger, void *functionInput)
{
	if (eventTrigger == NULL)
	{
		return MISSING_DATA;
	}

	if (eventTrigger->TriggerFunction == &StartCutscene)
	{
		eventTrigger->FunctionArguments.cutscene = (CutsceneID)functionInput;
	}
	else
	{
		eventTrigger->FunctionArguments.TriggerObject = (Object*)functionInput;
	}

	return LEMON_SUCCESS;
}


int ApplyTextPresets(TextInstance *inputText, const char Portrait[], TextPreset inputPreset)
{
	if (inputText == NULL)
	{
		return MISSING_DATA;
	}

	int topTextLocation = (screenHeight >> 1) - 100;
	int bottomTextLocation = 280 - (screenHeight >> 1);

	// Default settings
	inputText->currentXPos = -570;
	inputText->currentYPos = bottomTextLocation;

	inputText->boxOffsetX = 30;
	inputText->boxOffsetY = 240;
	inputText->textLengthSize = 1150;

	inputText->textDelayFrames = 3;
	strcpy(inputText->Portrait, Portrait);
	strcpy(inputText->font, "Pixel_White");
	strcpy(inputText->textBoxSprite, "TextBox_Basic");

	switch (inputPreset)
	{
		case PLAINTEXT_BOTTOM:
		strcpy(inputText->voice, "Text_snd");
		memset(inputText->textBoxSprite, 0, MAX_LEN);
		break;

		case PLAINTEXT_TOP:
		inputText->currentYPos = topTextLocation;
		strcpy(inputText->voice, "Text_snd");
		memset(inputText->textBoxSprite, 0, MAX_LEN);
		break;

		case WHITE_TOP:
		strcpy(inputText->textBoxSprite, "TextBox_White");
		strcpy(inputText->font, "Pixel_Black");
		inputText->currentYPos = topTextLocation;
		strcpy(inputText->voice, "Text_snd");
		break;

		case WHITE_TEXT:
		strcpy(inputText->textBoxSprite, "TextBox_White");
		strcpy(inputText->font, "Pixel_Black");
		strcpy(inputText->voice, "Text_snd");
		break;

		case BASIC_TEXT:
		strcpy(inputText->voice, "Text_snd");
		break;

		case BASIC_TOP:
		inputText->currentYPos = topTextLocation;
		strcpy(inputText->voice, "Text_snd");
		break;

		case BASIC_FAST:
		strcpy(inputText->voice, "Text_snd");
		inputText->textDelayFrames = 1;
		break;

		case BASIC_FLIP:
		strcpy(inputText->voice, "Text_snd");
		inputText->PortraitPosition = INSIDE_BOX_RIGHT;
		break;

		case BASIC_TOP_FLIP:
		inputText->currentYPos = topTextLocation;
		strcpy(inputText->voice, "Text_snd");
		inputText->PortraitPosition = INSIDE_BOX_RIGHT;
		break;

		case BASIC_FADE:
		strcpy(inputText->voice, "Text_snd");
		strcpy(inputText->textBoxSprite, "TextBox_BasicFade");
		break;

		case BASIC_TOP_FADE:
		strcpy(inputText->voice, "Text_snd");
		strcpy(inputText->textBoxSprite, "TextBox_BasicFade");
		inputText->currentYPos = topTextLocation;
		break;

		case SILENT_TOP:
		inputText->currentYPos = topTextLocation;
		break;

		default:
		break;
	}

	return LEMON_SUCCESS;
}


TextInstance* CreateText(const char inputPhrase[], World *GameWorld)
{
	if (GameWorld == NULL || inputPhrase == NULL)
	{
		return NULL;
	}

	// Check that string lengths are acceptable (< Max text length)
	int currentChar = 0;

	while (inputPhrase[currentChar] != 0 && currentChar < MAX_TEXT_LENGTH)
	{
		currentChar++;
	}

	if (currentChar >= MAX_TEXT_LENGTH)
	{
		return NULL;
	}

	// Create text instance
	TextInstance *newText = malloc(sizeof(TextInstance));

	if (newText == NULL)
	{
		return NULL;
	}

	newText->nextText = NULL;
	newText->boxPtr = NULL;
	memset(&newText->textTypeData, 0, sizeof(union TextTypeData));
	newText->textTypeSetting = REGULAR_TEXT;

	memset(newText->textPhrase, 0, MAX_TEXT_LENGTH - 1);
	strcpy(newText->textPhrase, inputPhrase);
	newText->textPhrase[MAX_TEXT_LENGTH - 1] = 0;
	memset(newText->voice, 0, MAX_LEN);
	memset(newText->Portrait, 0, MAX_LEN);

	newText->currentXPos = 0;
	newText->currentYPos = 0;
	newText->boxOffsetX = 0;
	newText->textLengthSize = 0;
	newText->boxOffsetY = 0;
	newText->currentChar = 0;
	newText->Counter = 0;
	newText->textDelayFrames = 0;

	memset(newText->font, 0, MAX_LEN);
	memset(newText->textBoxSprite, 0, MAX_LEN);
	newText->PortraitPosition = INSIDE_BOX_LEFT;
	newText->Skippable = 1;
	newText->voiceMode = PLAY_EACH_CHARACTER;

	if (GameWorld->TextQueue == NULL)
	{
		GameWorld->TextQueue = newText;
	}
	else
	{
		TextInstance *currentText;
		currentText = GameWorld->TextQueue;

		int i = 0;

		while (i < EngineSettings.MaxTextQueueLength && currentText->nextText != NULL)
		{
			currentText = currentText->nextText;
			i++;
		}

		if (currentText->nextText != NULL)
		{
			free(newText);
			return NULL;
		}

		currentText->nextText = newText;
	}

	// If a cutscene is playing/scheduled, create an associated sceneAction to control when the text plays
	if (GameWorld->CurrentCutscene != NO_CUTSCENE)
	{
		SceneAction_SayText(newText, GameWorld);
	}

	return newText;
}


// Play the text at the first slot in the linked list, then delete and shift everything up when done with that instance
int updateText(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL)
	{
		return MISSING_DATA;
	}

	TextInstance *currentText = GameWorld->TextQueue;

	if (!(GameWorld->GameState == GAMEPLAY || GameWorld->GameState == CUTSCENE) || GameWorld->GamePaused == 1 || !textSceneActionPresent(currentText, GameWorld))
	{
		return ACTION_DISABLED;
	}

	TextInteraction(GameWorld);


	if (currentText == NULL || currentText->currentChar < 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	// Skip text animation if skip button is held
	if (keyboard[LMN_TEXT_SKIP] && currentText->Skippable == 1)
	{
		keyboard[LMN_TEXT_CONFIRM] = 0;

		while (currentText->currentChar > -1)
		{
			displayNextCharacter(currentText, GameWorld);
		}
	}
	else
	{
		currentText->Counter++;

		if (!(currentText->Counter >= currentText->textDelayFrames || currentText->currentChar == 0))
		{
			return ACTION_DISABLED;
		}

		currentText->Counter = 0;

		displayNextCharacter(currentText, GameWorld);
	}
	
	playTextVoice(currentText);

	return LEMON_SUCCESS;
}


bool textSceneActionPresent(TextInstance *inputText, World *GameWorld)
{
	if (inputText == NULL || inputText->currentChar != 0 || GameWorld == NULL || GameWorld->SceneActionList == NULL)
	{
		return true;
	}


	SceneAction *currentAction = GameWorld->SceneActionList;

	while (currentAction != NULL)
	{
		if (currentAction->ActionID == SCENE_SAY_TEXT && currentAction->ActionData.sceneText == inputText)
		{
			return true;
		}

		if (currentAction->parallelAction == false)
		{
			return false;
		}

		currentAction = currentAction->nextSceneAction;
	}	

	return false;
}


void DeleteTextSceneAction(TextInstance *inputText, World *GameWorld)
{
	if (inputText == NULL || GameWorld == NULL || GameWorld->SceneActionList == NULL)
	{
		return;
	}


	SceneAction *currentAction = GameWorld->SceneActionList;

	while (currentAction != NULL)
	{
		if (currentAction->ActionID == SCENE_SAY_TEXT && currentAction->ActionData.sceneText == inputText)
		{
			currentAction = deleteSceneAction(currentAction, GameWorld);
		}
		else
		{
			currentAction = currentAction->nextSceneAction;
		}
	}	

	return;
}


int playTextVoice(TextInstance *currentText)
{
	if (currentText == NULL)
	{
		return MISSING_DATA;
	}

	if (currentText->voice != NULL && currentText->voice[0] > 32)
	{
		Lemon_PlaySound(currentText->voice, "Voices", SPEECH, 1.0);

		if (currentText->voiceMode == PLAY_ONCE)
		{
			currentText->voice[0] = 0;
		}
	}

	return LEMON_SUCCESS;
}


int TextInteraction(World *GameWorld)
{
	if (GameWorld->TextQueue == NULL || GameWorld->TextQueue->currentChar > -1)
	{
		return ACTION_DISABLED;
	}

	TextInstance *currentText = GameWorld->TextQueue;
	 
	switch (currentText->textTypeSetting)
	{
		case OPTION_PROMPT:
			handleOptionPrompt(currentText, GameWorld);
		break;

		default:
			if (keyboard[LMN_TEXT_CONFIRM] == 1)
			{
				endTextInstance(GameWorld);
			}
			break;
	}


	return LEMON_SUCCESS;
}


int handleOptionPrompt(TextInstance *inputText, World *GameWorld)
{
	struct TextOptionPrompt *optionData = &inputText->textTypeData.OptionPrompt;

	if (optionData->optionBeingPrinted < optionData->numberOfOptions)
	{
		inputText->currentChar = 1;

		if (strlen(inputText->textPhrase) > 0)
		{
			inputText->currentYPos -= 50;
		}
		inputText->currentXPos = 50 + inputText->boxOffsetX;

		memset(inputText->textPhrase, 0, MAX_TEXT_LENGTH);
		inputText->textPhrase[0] = ' ';
		strcpy(inputText->textPhrase + 1, optionData->optionNames[optionData->optionBeingPrinted]);
		optionData->OptionYPositions[optionData->optionBeingPrinted] = inputText->currentYPos;
		optionData->optionBeingPrinted++;

		return LEMON_SUCCESS;
	}

	if (optionData->optionBeingPrinted == optionData->numberOfOptions && inputText->boxPtr != NULL)
	{
		optionData->optionBeingPrinted++;
		AddObjectWithParent(GameWorld, inputText->boxPtr, UI_TEXT, 0, 0, 0, 0, TEXTOPTION_CURSOR, 0, 0, 0, 0);
	}

	if (keyboard[LMN_LEFT] == 1 || keyboard[LMN_UP] == 1)
	{
		optionData->SelectedOption = clamp(optionData->SelectedOption - 1, 0, optionData->numberOfOptions - 1);
	}

	if (keyboard[LMN_RIGHT] == 1 || keyboard[LMN_DOWN] == 1)
	{
		optionData->SelectedOption = clamp(optionData->SelectedOption + 1, 0, optionData->numberOfOptions - 1);
	}

	if (keyboard[LMN_TEXT_CONFIRM] == 1 || optionData->numberOfOptions < 1)
	{
		AcknowledgeHeldButtons();
		endTextInstance(GameWorld);
	}

	return LEMON_SUCCESS;
}


int displayNextCharacter(TextInstance *inputText, World *GameWorld)
{
	if (inputText == NULL || inputText->currentChar == -1)
	{
		return MISSING_DATA;
	}


	char currentChar = inputText->textPhrase[inputText->currentChar];

	if (inputText->currentChar == 0)
	{
		// create text box
		createTextBox(inputText, GameWorld);

		if (inputText->boxPtr == NULL)
		{
			endTextInstance(GameWorld);
			return LEMON_ERROR;
		}
	
		// Create portrait
		if (inputText->Portrait[0] != 0)
		{
			createTextBoxPortrait(inputText, GameWorld);
		}

		inputText->currentXPos = inputText->boxOffsetX;
		inputText->currentYPos = inputText->boxOffsetY;

		GameWorld->PlayingText = 1;
	}
	else
	{
		// Spawn next character		
		if (inputText->currentXPos + 40 > inputText->textLengthSize || currentChar == 10)
		{
			inputText->currentXPos = inputText->boxOffsetX;
			inputText->currentYPos -= 50;
		}

		switch(currentChar)
		{
			case 13:		// (/r) is repurposed to insert a delay of 30 ticks
			inputText->Counter = -30;
			break;

			case 12:		// (/f) is repurposed to insert a delay of 12 ticks  
			inputText->Counter = -12;
			break;
	
			case 11:		// (/v) is repurposed to insert a delay of 8 ticks 
			inputText->Counter = -8;
			break;

			default:
			break;
		}
	}

	int newTextXPos = inputText->currentXPos + (int)inputText->boxPtr->ObjectBox->xPos;
	int newTextYPos = inputText->currentYPos + (int)inputText->boxPtr->ObjectBox->yPos;

	AddObject(GameWorld, UI_TEXT, newTextXPos, newTextYPos, 0, 0, TEXT_CHARACTER, 0, 0, 0, 0);
	applyCharacterSpacing(inputText);

	inputText->currentChar++;

	
	// Finished creating text
	if (inputText->currentChar > MAX_TEXT_LENGTH || inputText->textPhrase[inputText->currentChar] == 0)
	{
		inputText->currentChar = -1;
	}

	
	return LEMON_SUCCESS;
}


int initialiseTextCharacter(Object *inputCharacter, char charValue, World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL || inputCharacter == NULL)
	{
		MarkObjectForDeletion(inputCharacter);
		return MISSING_DATA;
	}

	TextInstance *inputText = GameWorld->TextQueue;

	inputCharacter->ParentObject = inputText->boxPtr;
	inputCharacter->ParentLink = POSITION_LINK;
	inputCharacter->ObjectBox->xSize = 44;
	inputCharacter->ObjectBox->ySize = 44;

	if (charValue > 32 && charValue < 123)
	{
		switchSpriteByName(inputText->font, 0, inputCharacter->ObjectDisplay);
		mapTextToCharacter(inputCharacter, charValue);
	}
	else
	{
		MarkObjectForDeletion(inputCharacter);

		// Just in case
		inputCharacter->ObjectDisplay->RenderModeOverride = DO_NOT_RENDER;
	}


	return LEMON_SUCCESS;
}


Object* createTextBox(TextInstance *inputText, World *GameWorld)
{
	Object *TextBox = AddObject(GameWorld, UI_TEXT, inputText->currentXPos - inputText->boxOffsetX, inputText->currentYPos - inputText->boxOffsetY, 250, 250, TEXT_BOX, 0, 0, 0, 0);

	inputText->boxPtr = TextBox;

	if (inputText->boxPtr == NULL)
	{
		return NULL;
	}

	switchSpriteByName(inputText->textBoxSprite, 0, TextBox->ObjectDisplay);

	Sprite *switchedSprite = TextBox->ObjectDisplay->spriteBuffer;

	if (switchedSprite == NULL)
	{
		TextBox->ObjectDisplay->currentSprite = -1;
		return TextBox;
	}

	matchBoxToDisplayDimensions(TextBox);

	return TextBox;
}


Object* createTextBoxPortrait(TextInstance *inputText, World *GameWorld)
{
	if (inputText == NULL || inputText->boxPtr == NULL)
	{
		return NULL;
	}

	int portraitSize = 200;
	int boxXPos = (int)inputText->boxPtr->ObjectBox->xPos;
	int boxYPos = (int)inputText->boxPtr->ObjectBox->yPos + ((inputText->boxOffsetY - portraitSize) >> 1) + 44;

	Object *portrait = AddObjectWithParent(GameWorld, inputText->boxPtr, UI_TEXT, inputText->currentXPos, boxYPos, portraitSize, portraitSize, TEXT_PORTRAIT, 0, 0, 0, 0);

	if (portrait != NULL)
	{
		switchSpriteByName(inputText->Portrait, 0, portrait->ObjectDisplay);

		if (inputText->PortraitPosition == INSIDE_BOX_LEFT)
		{
			inputText->boxOffsetX += portraitSize + 30;
		}
		else if (inputText->PortraitPosition == INSIDE_BOX_RIGHT)
		{
			portrait->ObjectBox->xPos = boxXPos + inputText->textLengthSize - portraitSize;
			inputText->textLengthSize -= portraitSize + 30;
		}
	}

	return portrait;
}


int endTextInstance(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL)
	{
		return MISSING_DATA;
	}

	TextInstance *textToDelete = GameWorld->TextQueue;


	switch (textToDelete->textTypeSetting)
	{
		case OPTION_PROMPT:
		{
			struct TextOptionPrompt *optionData = &textToDelete->textTypeData.OptionPrompt;
			if (optionData->SelectedOption < 0 || optionData->SelectedOption >= optionData->numberOfOptions)
			{
				break;
			}

			struct TextEventTrigger *DataPtr = &optionData->optionTriggers[optionData->SelectedOption];

			if (DataPtr->TriggerFunction == &StartCutscene)
			{
				StartCutscene(DataPtr->FunctionArguments.cutscene, GameWorld);
			}
			else if (DataPtr->TriggerFunction != NULL)
			{
				TriggerableFunction function = (TriggerableFunction)DataPtr->TriggerFunction;
				(function)(DataPtr->FunctionArguments.TriggerObject, (void*)GameWorld);
			}
		} break;

		case TRIGGER_EVENT:
		{
			struct TextEventTrigger *DataPtr = &textToDelete->textTypeData.TriggerEvent;

			if (DataPtr->TriggerFunction == &StartCutscene)
			{
				StartCutscene(DataPtr->FunctionArguments.cutscene, GameWorld);
			}
			else if (DataPtr->TriggerFunction != NULL)
			{
				TriggerableFunction function = (TriggerableFunction)DataPtr->TriggerFunction;
				(function)(DataPtr->FunctionArguments.TriggerObject, (void*)GameWorld);
			}
		} break;

		default:
			break;
	}
	

	deleteTextInstance(textToDelete, GameWorld);

	return LEMON_SUCCESS;
}


// this function works this way (taking a pointer that it doesnt trust when it has access to the start of the text queue via gameworld)
// to avoid the case where in the process of ending a text instance, it executes a function that deletes itself before it reaches the actual line that was supposed to delete it.
// In this case, without this check it would simply delete the first text of whatever else was loaded/remains
// By including the pointer it can check whether it still is at the start of the queue or not so it knows it's deleting the correct thing
int deleteTextInstance(TextInstance *input, World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL || input != GameWorld->TextQueue)
	{
		return MISSING_DATA;
	}

	TextInstance *textToDelete = GameWorld->TextQueue;

	MarkObjectForDeletion(textToDelete->boxPtr);

	DeleteTextSceneAction(textToDelete, GameWorld);

	GameWorld->TextQueue = textToDelete->nextText;
	
	free(textToDelete);

	if (GameWorld->TextQueue == NULL)
	{
		GameWorld->PlayingText = 0;
	}

	return LEMON_SUCCESS;
}


int clearTextQueue(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL) { return MISSING_DATA; }

	int i = 0;

	while (i < EngineSettings.MaxTextQueueLength && GameWorld->TextQueue != NULL)
	{
		deleteTextInstance(GameWorld->TextQueue, GameWorld);
		i++;
	}

	if (GameWorld->TextQueue != NULL) { return LEMON_ERROR; }

	GameWorld->PlayingText = 0;

	return LEMON_SUCCESS;
}


int mapTextToCharacter(Object *inputText, int characterValue)
{
	if (inputText == NULL || inputText->ObjectDisplay == NULL || inputText->ObjectDisplay->spriteBuffer == NULL)
	{
		return MISSING_DATA;
	}

	int height = inputText->ObjectDisplay->spriteBuffer->height - 50;
	inputText->ObjectDisplay->pixelXOffset = 4 + (56 * ((characterValue - 32) % 16));
	inputText->ObjectDisplay->pixelYOffset = height - (56 * floor((float)(characterValue - 32) / 16.0));

	return LEMON_SUCCESS;
}


int applyCharacterSpacing(TextInstance *inputText)
{
	if (inputText == NULL)
	{
		return MISSING_DATA;
	}

	char character = inputText->textPhrase[inputText->currentChar];
	int widthValue = 0;

	switch(character)
	{
		case 'i':
		case 'l':
		case '!':
		case 39:
		case 46:
			widthValue = 16;
		break;

		case 'r':
		case 'f':
		case 32:
			widthValue = 24;
		break;

		case 'O':
		case 'N':
		case 'J':
			widthValue = 40;
		break;

		case 'w':
		case 'W':
		case 'm':
		case 'M':
		case 'Q':
		case '-':
			widthValue = 48;
		break;

		case 8:
		case 10:
		case 11:
		case 12:
		case 13:
			widthValue = 0;
		break;

		default:
			widthValue = 32;
		break;

	}

	
	inputText->currentXPos += widthValue;
	

	return LEMON_SUCCESS;
}