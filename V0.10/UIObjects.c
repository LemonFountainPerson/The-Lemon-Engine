#include "LemonEngine.h"
#include "utf8Decoder.h"


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
		if (getDisplayLayer(currentObject) == HUD)
		{
			hideObject(currentObject);
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
		if (getDisplayLayer(currentObject) == HUD)
		{
			showObject(currentObject);
		}

		currentObject = currentObject->nextObject;
	}


	return LEMON_SUCCESS;
}


UISubType convertEntryToUIType(char name[])
{
	if (!strcmp(name, "Cursor"))
	{
		return MOUSE_CURSOR;
	}
	else if (!strcmp(name, "BasicGraphic") || !strcmp(name, "Basic_Graphic"))
	{
		return BASIC_GRAPHIC;
	}


	return UNDEFINED_UI_ELEMENT;
}


char* convertUITypeToName(UISubType input)
{
	switch(input)
	{
	case BASIC_GRAPHIC:
		return "Basic Graphic";

	case MOUSE_CURSOR:
		return "Cursor";

	default:
		return "Unknown";
	}
}


int basicMenuCreation(float x, float yDiff, Object *controller, World *GameWorld, int count, ...);


int InitialiseUIElement(Object *UIElement, World *GameWorld)
{
	if (GameWorld == NULL || UIElement == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	setDisplayLayer(UIElement, HUD);
	UIElement->ObjectBox->collideLayer = HUD;
	UIElement->ObjectBox->solid = UNSOLID;
	UIElement->reserved |= RFLAG_CUTSCENE_IMMUNITY;	// mark this object as immune to being frozen during cutscenes


	switch(getSubType(UIElement))
	{
	case UNDEFINED_UI_ELEMENT:
		MarkObjectForDeletion(UIElement);
		break;


	case LEVEL_FADE:
		UIElement->Action = 0;
		UIElement->reserved |= RFLAG_PRESERVE_OBJECT;	// This means do not delete between level transitions
		setTransparency(UIElement, 1.0);
		switchSpriteByName("FadeOut", 0, getDisplay(UIElement));
		UIElement->ObjectBox->xSize = ScreenData.screenWidth;
		UIElement->ObjectBox->ySize = ScreenData.screenHeight;
		centerOnXY(UIElement, 0.0, 0.0);
		if (UIElement->prevObject != NULL && UIElement->prevObject->ObjectID == UI_ELEMENT && getSubType(UIElement->prevObject) == LEVEL_FADE)
		{
			MarkObjectForDeletion(UIElement);
		}
		break;


	case PAUSE_BACKGROUND:
		switchSpriteByName("PauseBackground", 0, getDisplay(UIElement));
		break;


	case PAUSE_HEADER:
		switchSpriteByName("PauseHeader", 0, getDisplay(UIElement));
		break;


	case SETTINGS_HEADER:
		switchSpriteByName("SettingsHeader", 0, getDisplay(UIElement));
		break;


	case PAUSE_MENU_CONTROLLER:
	{
		float xPosOrigin = (3 * X_TILESCALE) - (ScreenData.screenWidth >> 1);
		AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin, 132, PAUSE_HEADER, 0, 0, 0, 0);

		basicMenuCreation(xPosOrigin, 128, UIElement, GameWorld, 3, 
			"ResumeGame", "Settings", "QuitGame");
		setObjectName(UIElement, "PauseController");
	} break;


	case SAVE_OPTIONS_CONTROLLER:
	{
		float xPosOrigin = (3 * X_TILESCALE) - (ScreenData.screenWidth >> 1);

		basicMenuCreation(xPosOrigin, 128, UIElement, GameWorld, 5, 
			"Save1", "Save2", "SaveState", "LoadState", "BackOption");
		setObjectName(UIElement, "SaveController");
	} break;

	case SETTINGS_MENU_CONTROLLER:
	{
		float xPosOrigin = (3 * X_TILESCALE) - (ScreenData.screenWidth >> 1);

		//AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);
		AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + X_TILESCALE, 260, SETTINGS_HEADER, 0, 0, 0, 0);
		
		basicMenuCreation(xPosOrigin, 128, UIElement, GameWorld, 4, 
			"VideoSettings", "SoundSettings", "Settings", "BackOption");
		setObjectName(UIElement, "MenuController");
	} break;


	case VIDEO_SETTINGS_CONTROLLER:
	{
		float xPosOrigin = (3 * X_TILESCALE) - (ScreenData.screenWidth >> 1);

		//AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);
		AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + X_TILESCALE, 328, SETTINGS_HEADER, 0, 0, 0, 0);
		
		basicMenuCreation(xPosOrigin, 128, UIElement, GameWorld, 5, 
			"Settings", "Settings", "Settings", "Settings", "BackOption");
		setObjectName(UIElement, "VideoController");
	} break;


	case SOUND_SETTINGS_CONTROLLER:
	{
		float xPosOrigin = (3 * X_TILESCALE) - (ScreenData.screenWidth >> 1);

		//AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);
		AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + X_TILESCALE, 260, SETTINGS_HEADER, 0, 0, 0, 0);
		
		basicMenuCreation(xPosOrigin, 128, UIElement, GameWorld, 4, 
			"Settings", "Settings", "Settings", "BackOption");
		setObjectName(UIElement, "SoundController");
	} break;


	case MOUSE_CURSOR:
		switchSpriteByName("MouseCursor", USE_CURRENT_SPRITESET, getDisplay(UIElement));
		AddFrameUpdateFunction(&UpdateCursor, UIElement, GameWorld->ObjectList);
		setDisplayLayer(UIElement, FRONT_LAYER);
		UIElement->ObjectBox->solid = SOLID;
		UIElement->ObjectBox->xSize = 64;
		UIElement->ObjectBox->ySize = 64;
		break;

	default:
		SWITCH_TO_MISSING(UIElement);
		break;
	}


	return LEMON_SUCCESS;
}


int basicMenuCreation(float x, float yDiff, Object *controller, World *GameWorld, int count, ...)
{
	if (count < 1)
	{
		return EXECUTION_UNNECESSARY;
	}

	float y = ((yDiff / 2.0) * (count - 2)) - 32.0;
	controller->ObjectBox->xPos = x;
	controller->ObjectBox->yPos = y;
	controller->arg4 = (int)y;		// arg4 is y pos of first option

	va_list args;
	va_start(args, count);

	y -= 5.0;
	x -= 130.0;
	for (int i = 0; i < count; i++)
	{
		AddOptionButton(va_arg(args, char*), x, y, controller, GameWorld);
		y -= yDiff;
	}

	va_end(args);

	SetDrawPriorityToFront(GameWorld->ObjectList, controller);
	switchObjectSpriteByName("OptionCursor", controller);
	

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


	switch (getSubType(UIElement))
	{
	case PAUSE_MENU_CONTROLLER:
		PauseMenu(UIElement, GameWorld);
		break;

	case SAVE_OPTIONS_CONTROLLER:
		SaveMenu(UIElement, GameWorld);
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
		UpdateOptionButton(UIElement, GameWorld->MainCamera);
		break;

	case LEVEL_FADE:
		if (UIElement->Action == 0)
		{
			changeTransparency(UIElement, -0.03);
			if (getTransparency(UIElement) < 0.01)
			{
				UIElement->Action = 1;
				startTimer(0.5, UIElement);
				switchLevel(UIElement->arg2, GameWorld);
			}
		}
		else if (timerExpired(UIElement))
		{
			changeTransparency(UIElement, 0.05);
			if (getTransparency(UIElement) > 0.99)
			{
				MarkObjectForDeletion(UIElement);
			}
		}
		break;

	case MOUSE_CURSOR:
		smoothSizeChangeTo(UIElement, 1.0, 6.0);
		if (UIElement->arg2 < 1)
		{
			UIElement->Action = IDLE;
		}
		if (UIElement->Action == MOUSECLICK_INTERRUPT)
		{
			UIElement->arg2--;
		}
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

	centerOnMouse(Cursor, GameWorld->MainCamera);

	if (GameWorld->GamePaused == 1)
	{
		return ACTION_DISABLED;
	}

	Object *PlayerBox = GameWorld->Player.PlayerPtr;

	if (MouseInput.LeftButton)
	{
		setScaleSize(Cursor, 0.75);

		if (PlayerBox != NULL && Cursor->Action == IDLE)
		{
			Object *Bullet = AddNamedObject(GameWorld, "PlayerBullet", PROJECTILE, 0, 0);
			addBulletComponentWithCollision(Bullet, PlayerBox, 40, SPARKLE);
			PointObjectToMouse(Bullet, GameWorld);
			
			Bullet = AddNamedObject(GameWorld, "PlayerBullet", PROJECTILE, 0, 0);
			addBulletComponentWithCollision(Bullet, PlayerBox, 40, SPARKLE);
			PointObjectToMouse(Bullet, GameWorld);
			RotateObject(Bullet, 17.5);

			Bullet = AddNamedObject(GameWorld, "PlayerBullet", PROJECTILE, 0, 0);
			addBulletComponentWithCollision(Bullet, PlayerBox, 40, SPARKLE);
			PointObjectToMouse(Bullet, GameWorld);
			RotateObject(Bullet, -17.5);

			Cursor->Action = MOUSECLICK_INTERRUPT;
			Cursor->arg2 = 3;
		}
	}


	return LEMON_SUCCESS;
}


bool MenuControl(Object *MenuController, World *GameWorld)
{
	if (MenuController == NULL || GameWorld == NULL || MenuController->ObjectID != UI_ELEMENT)
	{
		return false;
	}

	// arg2: Option selected
	// arg3: Number of options
	// arg4: Y Pos for first option

	if (buttonPressed(LMN_DOWN))
	{
		AcknowledgeButton(LMN_DOWN);

		MenuController->arg2++;
		MenuController->Action = INTERACTION_INTERRUPT;

		if (MenuController->arg2 >= MenuController->arg3)
		{
			MenuController->arg2 = 0;
		}
	}

	if (buttonPressed(LMN_UP))
	{
		AcknowledgeButton(LMN_UP);
		
		MenuController->arg2--;
		MenuController->Action = INTERACTION_INTERRUPT;

		if (MenuController->arg2 < 0)
		{
			MenuController->arg2 = MenuController->arg3 - 1;
		}
	}

	if (MenuController->Action == INTERACTION_INTERRUPT)
	{
		MenuController->ObjectBox->yPos = (float)MenuController->arg4 - (MenuController->arg2 * 128);
		MenuController->Action = IDLE;
	}


	if ((keyboard[LMN_MENU_CONFIRM] == 1 && MouseInput.LeftButton != 1) || MenuController->Action == MOUSECLICK_INTERRUPT)
	{
		AcknowledgeHeldButtons();
		MenuController->Action = IDLE;

		return true;
	}


	return false;
}

void refreshMenu(Object *MenuController, World *GameWorld)
{
	if (MenuController->State < DEFAULT_STATE)
	{
		return;
	}

	MarkObjectForDeletion(MenuController);
	Object *newMenu = AddObject(GameWorld, UI_ELEMENT, 0, 0, getSubType(MenuController), 0, 0, 0, 0);

	if (newMenu != NULL)
	{
		newMenu->arg2 = MenuController->arg2;
		newMenu->ObjectBox->yPos = MenuController->ObjectBox->yPos;
	}
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
				if (AddObject(GameWorld, UI_ELEMENT, 0, 0, SETTINGS_MENU_CONTROLLER, 0, 0, 0, 0) != NULL)
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

int SaveMenu(Object *MenuController, World *GameWorld)
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
				loadSave(1, GameWorld);
				break;

			case 1:
				loadSettings(1, GameWorld);
				break;

			case 2:
				saveGameState(GameWorld);
				break;

			case 3:
				loadGameState(GameWorld);
				break;

			default:
				if (AddObject(GameWorld, UI_ELEMENT, 0, 0, SETTINGS_MENU_CONTROLLER, 0, 0, 0, 0) != NULL)
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
				if (AddObject(GameWorld, UI_ELEMENT, 0, 0, VIDEO_SETTINGS_CONTROLLER, 0, 0, 0, 0) != NULL)
				{
					MarkObjectForDeletion(MenuController);
				}
				break;

			case 1:
				if (AddObject(GameWorld, UI_ELEMENT, 0, 0, SOUND_SETTINGS_CONTROLLER, 0, 0, 0, 0) != NULL)
				{
					MarkObjectForDeletion(MenuController);
				}
				break;

			case 2:
				if (AddObject(GameWorld, UI_ELEMENT, 0, 0, SAVE_OPTIONS_CONTROLLER, 0, 0, 0, 0) != NULL)
				{
					MarkObjectForDeletion(MenuController);
				}
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
			// THIS IS TEMPORARY - NEED TO IMPLEMENT SOME KIND OF SCREEN SIZE SELECTOR GUI
			case 0:
				changeScreenSizeScaled(640, 360, GameWorld);
				break;

			case 1:
				changeScreenSizeScaled(1280, 720, GameWorld);
				break;

			case 2:
				changeScreenSize(320, 180, GameWorld);	
				break; 

			case 3:
				changeScreenSize(1280, 720, GameWorld);
				break;

			default:
				if (AddObject(GameWorld, UI_ELEMENT, 0, 0, SETTINGS_MENU_CONTROLLER, 0, 0, 0, 0) != NULL)
				{
					MarkObjectForDeletion(MenuController);
				}
				break;
		}

		refreshMenu(MenuController, GameWorld);
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
				if (AddObject(GameWorld, UI_ELEMENT, 0, 0, SETTINGS_MENU_CONTROLLER, 0, 0, 0, 0) != NULL)
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

	Object *createdOption = AddObjectWithParent(GameWorld, MenuController, UI_ELEMENT, xPos, yPos, OPTION_BUTTON, MenuController->arg3, 0, 0, 0);
	if (createdOption == NULL)
	{
		return NULL;
	}

	switchSpriteByName(spriteName, USE_CURRENT_SPRITESET, getDisplay(createdOption));
	matchBoxToDisplayDimensions(createdOption);
	UpdateOptionButton(createdOption, GameWorld->MainCamera);

	setScaleSize(createdOption, 1.0);
	MenuController->arg3++;		// number of options

	return createdOption;
}


int UpdateOptionButton(Object *Button, Camera inputCam)
{
	DisplayData *buttonDisplay = getDisplay(Button);
	if (Button == NULL || Button->Parent == NULL || buttonDisplay == NULL)
	{
		return MISSING_DATA;
	}

	if (buttonDisplay->spriteBuffer == NULL || strcmp(buttonDisplay->spriteBuffer->name, "Missing") == 0)
	{
		return INVALID_DATA;
	}

	// Detect mouse overlap
	if (MouseOverlappingSprite(Button, inputCam) == 1)
	{	
		if (Button->Action == IDLE)					// Mouse hover
		{
			Button->Parent->arg2 = Button->arg2;
			Button->Action = INTERACTION_INTERRUPT;
			Button->Parent->Action = INTERACTION_INTERRUPT;
		}

		// Detect mouse click
		if (MouseInput.LeftButton == 1)				// mouse down
		{
			Button->Action = MOUSECLICK_INTERRUPT;
		}

		if (Button->Action == MOUSECLICK_INTERRUPT && MouseInput.LeftButton == 0)	// mouse release/click
		{
			Button->Parent->Action = MOUSECLICK_INTERRUPT;
			Button->Action = INTERACTION_INTERRUPT;
			Button->Parent->arg2 = Button->arg2;
		}
	}
	else
	{
		Button->Action = IDLE;
	}

	// animate
	if (Button->Parent->arg2 == Button->arg2)
	{
		if (keyboard[LMN_MENU_CONFIRM] == 1 || Button->Action == MOUSECLICK_INTERRUPT)
		{
			smoothSizeChangeTo(Button, 1.10, 2.0);
		}
		else
		{
			smoothSizeChangeTo(Button, 1.20, 5.0);
		}
		
		if (Button->arg3 == 0)
		{
			buttonDisplay->currentSprite++;
			Button->arg3 = 1;
		}
	}
	else
	{
		smoothSizeChangeTo(Button, 1.00, 4.0);

		if (Button->arg3 == 1)
		{
			buttonDisplay->currentSprite--;
			Button->arg3 = 0;
		}
	}


	return LEMON_SUCCESS;
}


bool playingText(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL)
	{
		return false;
	}

	return textSceneActionPresent(GameWorld->TextQueue, GameWorld);
}


int InitialiseUIText(Object *UIText, World *GameWorld)
{
	if (GameWorld == NULL || UIText == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	setDisplayLayer(UIText, HUD);
	UIText->ObjectBox->collideLayer = HUD;
	UIText->ObjectBox->solid = UNSOLID;
	UIText->reserved |= RFLAG_CUTSCENE_IMMUNITY; // mark this object as immune to being frozen during cutscenes

	switch(getSubType(UIText))
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
		initialiseTextCharacter(UIText, UIText->arg2, GameWorld);
		break;

	case TEXT_PORTRAIT:
		UIText->ParentLink = MOTION_LINK;
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


	TextBox *currentText = GameWorld->TextQueue;

	switch (getSubType(UIText))
	{
	case TEXTOPTION_CURSOR:
		if (currentText == NULL || currentText->boxPtr == NULL || currentText->textTypeSetting != TEXTBOX_OPTION_PROMPT)
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

	default:
		break;
	}

	return LEMON_SUCCESS;
}


TextBox* SayText(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld)
{
	TextBox *newText = CreateText(inputPhrase, GameWorld);

	// if position of box should be automated, you can do it here
	if (newText == NULL)
	{
		return NULL;
	}

	ApplyTextPresets(newText, Portrait, inputPreset);

	// int topTextLocation = (screenHeight >> 1) - 100;
	// int bottomTextLocation = 280 - (screenHeight >> 1);

	// newText->currentXPos = -570;
	// newText->currentYPos = bottomTextLocation;

	return newText;
}


// Triggerable events used here and in the TEXTBOX_TRIGGER_EVENT text type expect a specific type of function and input, as defined by AddFunctionArgumentsToTriggerEvent
TextBox* SayTextOption(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld, int numberOfOptions, ...)
{
	if (inputPhrase == NULL || strlen(inputPhrase) >= OPTION_TEXT_MAX_LEN || numberOfOptions < 1 || numberOfOptions > MAX_TEXT_OPTIONS)
	{
		return NULL;
	}

	TextBox *newText = CreateText(inputPhrase, GameWorld);

	va_list args;
    va_start(args, numberOfOptions);

    GameEvent *eventTrigger;
    struct TextOptionPrompt optionData = {0};	// temp location for arguments; must be loaded as events need to be removed even if text box is failed to be created
    optionData.numberOfOptions = numberOfOptions;

    for (int i = 0; i < numberOfOptions; i++)
    {
    	strcpy(optionData.optionNames[i], va_arg(args, char*));

    	eventTrigger = va_arg(args, GameEvent*);

    	if (eventTrigger != NULL)
    	{
    		removeEventToTriggerLater(eventTrigger, &optionData.optionTriggers[i], GameWorld);
    	}
    	else
    	{
    		optionData.optionTriggers[i].EventID = NO_EVENT;
    	}
    }

    va_end(args);

	if (newText == NULL)
	{
		return NULL;
	}

	ApplyTextPresets(newText, Portrait, inputPreset);

	newText->textTypeSetting = TEXTBOX_OPTION_PROMPT;

	memcpy(&newText->textTypeData.OptionPrompt, &optionData, sizeof(struct TextOptionPrompt));
	

	return newText;
}


int AddTriggerableEventToText(TextBox *inputText, GameEvent *inputEvent, World *GameWorld)
{
	if (inputText == NULL || inputEvent == NULL || GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (inputText->textTypeSetting != TEXTBOX_REGULAR_TEXT)
	{
		// Its already been loaded with custom data here, so it should refuse to alter/overwrite it
		return INVALID_DATA;
	}

	inputText->textTypeSetting = TEXTBOX_TRIGGER_EVENT;

	removeEventToTriggerLater(inputEvent, &inputText->textTypeData.TriggerEvent, GameWorld);

	return LEMON_SUCCESS;
}


int ApplyTextPresets(TextBox *inputText, const char Portrait[], TextPreset inputPreset)
{
	if (inputText == NULL)
	{
		return MISSING_DATA;
	}

	int topTextLocation = (ScreenData.screenHeight >> 1) - 100;
	int bottomTextLocation = 280 - (ScreenData.screenHeight >> 1);

	// Default settings
	inputText->currentXPos = -570;
	inputText->currentYPos = bottomTextLocation;

	inputText->boxOffsetX = 30;
	inputText->boxOffsetY = 240;
	inputText->textLengthSize = 1150;

	inputText->textDelayFrames = 3;
	strcpy(inputText->Portrait, Portrait);

	if (EXPERIMENTAL_TEXT)
	{
		strcpy(inputText->font, DEFAULT_FONT);
	}
	else
	{
		strcpy(inputText->font, "Pixel_White");
	}

	strcpy(inputText->textBoxSprite, "TextBox_Basic");
	inputText->LineSpacing = 50;
	inputText->TextSize = TextSettings.defaultTextPointSize;

	switch (inputPreset)
	{
		case COMIC_TEXT:
		strcpy(inputText->font, "ComicSans");
		break;

		case COMIC_TOP:
		strcpy(inputText->font, "ComicSans");
		inputText->currentYPos = topTextLocation;
		break;

		case PLAINTEXT_BOTTOM:
		strcpy(inputText->voice, "Text_snd");
		memset(inputText->textBoxSprite, 0, MAX_LEN);
		break;

		case PLAINTEXT_TOP:
		inputText->currentYPos = topTextLocation;
		strcpy(inputText->voice, "Text_snd");
		memset(inputText->textBoxSprite, 0, MAX_LEN);
		break;

		case BLACK_TOP:
		strcpy(inputText->textBoxSprite, "TextBox_White");
		strcpy(inputText->font, "Pixel_Black");
		inputText->currentYPos = topTextLocation;
		strcpy(inputText->voice, "Text_snd");
		break;

		case BLACK_TEXT:
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
		inputText->PortraitPosition = PORTRAIT_INSIDE_BOX_RIGHT;
		break;

		case BASIC_TOP_FLIP:
		inputText->currentYPos = topTextLocation;
		strcpy(inputText->voice, "Text_snd");
		inputText->PortraitPosition = PORTRAIT_INSIDE_BOX_RIGHT;
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


TextBox* CreateText(const char inputPhrase[], World *GameWorld)
{
	if (GameWorld == NULL || inputPhrase == NULL)
	{
		return NULL;
	}

	if (strlen(inputPhrase) >= MAX_TEXT_LENGTH)
	{
		return NULL;
	}

	// Create text box
	TextBox *newText = malloc(sizeof(TextBox));

	if (newText == NULL)
	{
		return NULL;
	}

	if (GameWorld->TextQueue == NULL)
	{
		GameWorld->TextQueue = newText;
	}
	else
	{
		TextBox *currentText = GameWorld->TextQueue;

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

	memset(newText->textPhrase, 0, MAX_TEXT_LENGTH - 1);
	strcpy(newText->textPhrase, inputPhrase);	
	memset(newText->voice, 0, MAX_LEN);
	memset(newText->Portrait, 0, MAX_LEN);

	newText->nextText = NULL;
	newText->boxPtr = NULL;
	memset(&newText->textTypeData, 0, sizeof(union TextTypeData));
	newText->textTypeSetting = TEXTBOX_REGULAR_TEXT;

	newText->currentXPos = 0;
	newText->currentYPos = 0;
	newText->boxOffsetX = 0;
	newText->textLengthSize = 0;
	newText->boxOffsetY = 0;
	newText->currentIndex = 0;
	newText->Counter = 0;
	newText->textDelayFrames = 0;
	newText->LineSpacing = 50;
	newText->TextSize = TextSettings.defaultTextPointSize;
	newText->textIndex = -1;

	memset(newText->font, 0, FONT_FILE_NAME_MAX);
	memset(newText->textBoxSprite, 0, MAX_LEN);
	newText->PortraitPosition = PORTRAIT_INSIDE_BOX_LEFT;
	newText->Skippable = true;
	newText->voiceMode = VOICE_EACH_CHARACTER;

	newText->color.r = 255;
	newText->color.g = 255;
	newText->color.b = 255;
	newText->color.a = 255;

	// create an associated sceneAction to control when the text plays
	SceneAction_SayText(newText, GameWorld);

	return newText;
}


int insertLineBreaks(char *input, int maxLength)
{
	if (input == NULL || input[0] == 0 || EXPERIMENTAL_TEXT)
	{
		return MISSING_DATA;
	}

	char decoded = 0;
	int distance = 0;
	int lastSpace = -1;
	int Index = 0;
	int length = strlen(input);

	while (Index < length)
	{
		decoded = utf8_decode_next(input, Index, length);
		distance += getCharacterSpacing(decoded);

		if (decoded == '\n')
		{
			distance = 0;
		}
		else if (decoded <= ' ')
		{
			lastSpace = Index;
		}

		Index = utf8_setIndex();

		if (distance > maxLength)
		{
			distance = 0;
			if (lastSpace >= 1)
			{
				input[lastSpace] = '\n';
				Index = lastSpace + 1;
			}
		}
	}


	return LEMON_SUCCESS;
}

void removeControlCharacters(char input[MAX_TEXT_LENGTH], int characterCount)
{
	int i = characterCount - 8;
	if (i < 0)
	{
		i = 0;
	}

	while (i < characterCount)
	{
		if (input[i] < 33 && input[i] != '\n')
		{
			input[i] = ' ';
		}
		i++;
	}
}


// Play the text at the first slot in the linked list, then delete and shift everything up when done with that textbox
int updateTextBoxes(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL)
	{
		return MISSING_DATA;
	}

	TextBox *currentText = GameWorld->TextQueue;

	if (!(GameWorld->GameState == GAMEPLAY || GameWorld->GameState == CUTSCENE) || GameWorld->GamePaused == 1 || !textSceneActionPresent(currentText, GameWorld))
	{
		return ACTION_DISABLED;
	}

	if (currentText->currentIndex < 0)
	{
		TextInteraction(currentText, GameWorld);
	}
	else
	{
		displayText(currentText, GameWorld);
	}
	
	return LEMON_SUCCESS;
}


int displayText(TextBox *currentText, World *GameWorld)
{
	if (currentText == NULL)
	{
		return MISSING_DATA;
	}

	// Skip text animation if skip button is held
	if (keyboard[LMN_TEXT_SKIP] && currentText->Skippable == true)
	{
		keyboard[LMN_TEXT_CONFIRM] = -1;

		if (EXPERIMENTAL_TEXT)
		{
			currentText->currentIndex = strlen(currentText->textPhrase) - 1;
			displayNextCharacter(currentText, GameWorld);
			currentText->currentIndex = -1;
		}
		else
		{
			while (currentText->currentIndex >= 0)
			{
				displayNextCharacter(currentText, GameWorld);
			}
		}
	}
	else
	{
		currentText->Counter++;

		if (!(currentText->Counter >= currentText->textDelayFrames || currentText->currentIndex == 0))
		{
			return ACTION_DISABLED;
		}

		currentText->Counter = 0;

		displayNextCharacter(currentText, GameWorld);
	}
	
	playTextVoice(currentText);

	return LEMON_SUCCESS;
}

int displayNextCharacter(TextBox *inputText, World *GameWorld)
{
	if (inputText == NULL || inputText->currentIndex < 0)
	{
		return MISSING_DATA;
	}


	if (inputText->boxPtr == NULL)
	{
		// create text box
		createTextBox(inputText, GameWorld);

		if (inputText->boxPtr == NULL)
		{
			endTextBox(GameWorld);
			return LEMON_ERROR;
		}
	
		// Create portrait
		if (inputText->Portrait[0] != 0 && strcmp(inputText->Portrait, "NO_PORTRAIT"))
		{
			createTextBoxPortrait(inputText, GameWorld);
		}

		insertLineBreaks(inputText->textPhrase, inputText->textLengthSize - (int)inputText->TextSize);

		inputText->textIndex = -1;
		inputText->currentXPos = inputText->boxOffsetX;
		inputText->currentYPos = inputText->boxOffsetY;
	}

	int decodedChar = utf8_decode_next(inputText->textPhrase, inputText->currentIndex, MAX_TEXT_LENGTH);
	inputText->currentIndex = utf8_setIndex();

	// Finished creating text
	if (decodedChar == 0 || inputText->currentIndex >= MAX_TEXT_LENGTH || inputText->boxPtr->State == EMPTY_OBJECT)
	{
		inputText->currentIndex = -1;

		return LEMON_SUCCESS;
	}

	switch(decodedChar)
	{
		case 13:		// (/r) is repurposed to insert a delay of 30 ticks
		inputText->Counter = -30;

		return LEMON_SUCCESS;

		case 12:		// (/f) is repurposed to insert a delay of 12 ticks  
		inputText->Counter = -12;
		return LEMON_SUCCESS;

		case 11:		// (/v) is repurposed to insert a delay of 8 ticks 
		inputText->Counter = -8;
		return LEMON_SUCCESS;

		default:
		break;
	}

	removeControlCharacters(inputText->textPhrase, inputText->currentIndex);


	if (EXPERIMENTAL_TEXT)
	{
		experimentalText(inputText);

		return LEMON_SUCCESS;
	}


	// Spawn next character		
	if (inputText->currentXPos - inputText->boxOffsetX + (int)inputText->TextSize > inputText->textLengthSize || decodedChar == '\n')
	{
		inputText->currentXPos = inputText->boxOffsetX;
		inputText->currentYPos -= inputText->LineSpacing;
	}
	

	int newTextXPos = inputText->currentXPos + (int)inputText->boxPtr->ObjectBox->xPos;
	int newTextYPos = inputText->currentYPos + (int)inputText->boxPtr->ObjectBox->yPos;
	AddObject(GameWorld, UI_TEXT, newTextXPos, newTextYPos, TEXT_CHARACTER, decodedChar, 0, 0, 0);

	inputText->currentXPos += getCharacterSpacing(decodedChar);

	
	return LEMON_SUCCESS;
}


int playTextVoice(TextBox *currentText)
{
	if (currentText == NULL)
	{
		return MISSING_DATA;
	}

	if (currentText->voice[0] > 32)
	{
		PlaySound(currentText->voice, "Voices", SPEECH, 1.0);

		if (currentText->voiceMode == VOICE_ONCE)
		{
			currentText->voice[0] = 0;
		}
	}

	return LEMON_SUCCESS;
}


int TextInteraction(TextBox *currentText, World *GameWorld)
{
	if (currentText == NULL)
	{
		return MISSING_DATA;
	}
	 
	switch (currentText->textTypeSetting)
	{
		case TEXTBOX_OPTION_PROMPT:
			handleOptionPrompt(currentText, GameWorld);
		break;

		default:
			if (buttonPressed(LMN_TEXT_CONFIRM) || buttonPressed(MOUSE_LEFT))
			{
				endTextBox(GameWorld);
			}
		break;
	}


	return LEMON_SUCCESS;
}

int handleOptionPrompt(TextBox *inputText, World *GameWorld)
{
	struct TextOptionPrompt *optionData = &inputText->textTypeData.OptionPrompt;

	if (optionData->optionBeingPrinted < optionData->numberOfOptions)
	{
		if (strlen(inputText->textPhrase) > 0)
		{
			inputText->currentYPos -= 50;
		}
		inputText->currentXPos = inputText->boxOffsetX + 50;

		memset(inputText->textPhrase, 0, MAX_TEXT_LENGTH);
		strcpy(inputText->textPhrase, optionData->optionNames[optionData->optionBeingPrinted]);
		inputText->currentIndex = 0;
		inputText->textIndex = -1;

		optionData->OptionYPositions[optionData->optionBeingPrinted] = inputText->currentYPos;
		optionData->optionBeingPrinted++;
		return LEMON_SUCCESS;
	}
	else if (inputText->currentIndex == -1)
	{
		AddObjectWithParent(GameWorld, inputText->boxPtr, UI_TEXT, 0, 0, TEXTOPTION_CURSOR, 0, 0, 0, 0);
		inputText->currentIndex = -2;

		return LEMON_SUCCESS;
	}

	if (buttonPressed(LMN_UP))
	{
		optionData->SelectedOption = clamp(optionData->SelectedOption - 1, 0, optionData->numberOfOptions - 1);
	}

	if (buttonPressed(LMN_DOWN))
	{
		optionData->SelectedOption = clamp(optionData->SelectedOption + 1, 0, optionData->numberOfOptions - 1);
	}

	bool selectOption = buttonPressed(LMN_TEXT_CONFIRM);

	// detect mouse input
	if (inputText->boxPtr != NULL)
	{
		PhysicsBox *boxRect = inputText->boxPtr->ObjectBox;
		PhysicsBox stateSave;
		memcpy(&stateSave, boxRect, sizeof(PhysicsBox));

		boxRect->xSize = 600;
		boxRect->ySize = 50;
		boxRect->xPos = inputText->boxOffsetX + boxRect->xPos;
		float boxYPos = boxRect->yPos;

		int i = 0;
		while (i < optionData->numberOfOptions)
		{
			boxRect->yPos = optionData->OptionYPositions[i] + boxYPos;

			if (MouseOverlappingBox(inputText->boxPtr, GameWorld->MainCamera))
			{
				optionData->SelectedOption = clamp(i, 0, optionData->numberOfOptions - 1);
				i = optionData->numberOfOptions;
			}

			i++;
		}
		
		memcpy(boxRect, &stateSave, sizeof(PhysicsBox));
	}


	if (selectOption || optionData->numberOfOptions < 1)
	{
		AcknowledgeHeldButtons();
		endTextBox(GameWorld);
	}

	return LEMON_SUCCESS;
}


int addText(const char *textPhrase, float xPos, float yPos)
{
	return addTextToList(&TextSettings.TextList, textPhrase, xPos, yPos, 0, NULL);
}

int addTextWithFont(const char *textPhrase, float xPos, float yPos, const char *font)
{
	return addTextToList(&TextSettings.TextList, textPhrase, xPos, yPos, 0, font);
}

int addTextToList(TextList *list, const char *textPhrase, float xPos, float yPos, int wrapWidth, const char *desiredFont)
{
	if (list == NULL)
	{
		return MISSING_DATA;
	}

	Text *TextArray = list->texts;

	Text *newText = NULL;
	int index = 0;

	//  find an available slot
	while (index < MAX_TEXT_TEXTURES && newText == NULL)
	{
		if (TextArray[index].beingUsed == false)
		{
			newText = &TextArray[index];
			continue;
		}

		index++;
	}

	if (newText == NULL)
	{
		return ACTION_DISABLED;
	}
	

	// get or create new font
	TTF_Font *renderFont;

	if (desiredFont == NULL || strlen(desiredFont) < 1)
	{
		renderFont = loadFont(TextSettings.defaultFont, "DefaultFont");
	}
	else
	{
		renderFont = loadFont(desiredFont, desiredFont);
	}

	if (renderFont == NULL)
	{
		return LEMON_ERROR;
	}

	newText->xPos = xPos;
    newText->yPos = yPos;

    newText->CameraRelative = false;
    newText->beingUsed = true;
    newText->textBox = NULL;

    if (newText->text == NULL)
    {
    	newText->text = TTF_CreateText(ScreenData.textEngine, renderFont, textPhrase, 0);
    }
	else // text object already present, no need to delete, just reset font and text
	{
    	TTF_SetTextFont(newText->text, renderFont);
    	TTF_SetTextString(newText->text, textPhrase, 0);
    }

    TTF_SetTextWrapWidth(newText->text, wrapWidth);

    list->count++;

    return index;
}	

void updateText(int index, const char *newPhrase)
{
	if (newPhrase == NULL || index < 0 || index >= MAX_TEXT_TEXTURES)
	{
		return;
	}

	Text *list = TextSettings.TextList.texts;

	if (list[index].text != NULL && list[index].beingUsed)
	{
		TTF_SetTextString(list[index].text, newPhrase, 0);
	}

	return;
}

void moveText(int index, float xPos, float yPos)
{
	if (index < 0 || index >= MAX_TEXT_TEXTURES)
	{
		return;
	}

	Text *list = TextSettings.TextList.texts;

	if (list[index].text != NULL)
	{
		list[index].xPos = xPos;
		list[index].yPos = yPos;
	}

	return;
}

void setTextColour(int index, SDL_Color *colour)
{
	if (index < 0 || index >= MAX_TEXT_TEXTURES)
	{
		return;
	}

	Text *list = TextSettings.TextList.texts;

	if (list[index].text != NULL)
	{
		TTF_SetTextColor(list[index].text, colour->r, colour->g, colour->b, colour->a);
	}
	
	return;
}

void initialiseTextList(TextList *input)
{
	if (input == NULL)
	{
		return;
	}

	memset(input, 0, sizeof(TextList));
	for (int i = 0; i < MAX_TEXT_TEXTURES; i++)
	{
		input->texts[i].text = NULL;
		input->texts[i].textBox = NULL;
	}
}

void printTextsinfo(TextList *list, const char name[])
{
	putConsoleString("\n%s: ", name);
	char buffer[64] = {0};

	Text *array = list->texts;

	for (int i = 0; i < MAX_TEXT_TEXTURES; i++)
	{
		if (array[i].text != NULL)
		{
			snprintf(buffer, 20, "(Data loaded)  ");
		}
		else
		{
			snprintf(buffer, 20, "(Data empty)   ");
		}

		if (array[i].beingUsed)
		{
			strcat(buffer, "(Being used)");
		}
		else
		{
			strcat(buffer, "(Unused)");
		}

		if (array[i].CameraRelative)
		{
			strcat(buffer, "(Camera relative)");
		}
		else
		{
			strcat(buffer, "(Screen relative)");
		}

		if (array[i].textBox)
		{
			strcat(buffer, "(Connected to textBox)");
		}
		else
		{
			strcat(buffer, "(independent)");
		}

		putConsoleString("Index: %d  %s", i, buffer);
	}

	return;
}

int RemoveText(TextList *list, int index)
{
	Text *texts = list->texts; 

	if (index < 0 || index >= MAX_TEXT_TEXTURES || !texts[index].beingUsed)
	{
		return EXECUTION_UNNECESSARY;
	}

	texts[index].beingUsed = false;
	list->count--;

	return LEMON_SUCCESS;
}


int RemoveAllTexts(TextList *list)
{
	int i = 0;
	while (i < MAX_TEXT_TEXTURES)
	{
		RemoveText(list, i);
		i++;
	}

	list->count = 0;

	return LEMON_SUCCESS;
}

void removeAssociatedTexts(TextBox *input)
{
	if (input == NULL || !EXPERIMENTAL_TEXT)
	{
		return;
	}

	TextList *list = &TextSettings.TextList;

	Text *array = list->texts;

	for (int i = 0; i < MAX_TEXT_TEXTURES; i++)
	{
		if (array[i].textBox == input)
		{
			array[i].textBox = NULL;
			array[i].beingUsed = false;
			list->count--;
		}
	}
}

int experimentalText(TextBox *input)
{
	if (input == NULL || input->boxPtr == NULL)
	{
		return MISSING_DATA;
	}

	char subset[MAX_TEXT_LENGTH] = {0};
	memcpy(subset, input->textPhrase, input->currentIndex);

	if (input->textIndex >= 0)
	{
		updateText(input->textIndex, subset);
		return input->textIndex;
	}
	
	TextList *list = &TextSettings.TextList;
	input->textIndex = addTextToList(list, subset, input->currentXPos, input->currentYPos, input->textLengthSize, input->font);

	if (input->textIndex < 0)
	{
		return LEMON_ERROR;
	}

	Text *newText = &list->texts[input->textIndex];
	newText->textBox = input;
	TTF_SetTextColor(newText->text, input->color.r, input->color.g, input->color.b, input->color.a);

    return input->textIndex;
}


void closeFont(TTF_Font *font)
{
	TextList *list = &TextSettings.TextList;
	Text *array = list->texts;

	for (int i = 0; i < MAX_TEXT_TEXTURES; i++)
	{
		if (TTF_GetTextFont(array[i].text) == font)
		{
			TTF_DestroyText(array[i].text);
			array[i].text = NULL;
			array[i].beingUsed = false;
			list->count--;
		}
	}

	TTF_CloseFont(font);

	return;
}

TTF_Font* loadFont(const char *desiredFont, const char *newName)
{	
	TTF_Font *newFont = getFont(desiredFont);
	if (newFont != NULL)
	{
		return newFont;
	}

	FontList *list = &TextSettings.FontList;

	int head = list->head % MAX_LOADED_FONTS;
	if (list->font[head] != NULL)
	{
		closeFont(list->font[head]);
		list->font[head] = NULL;
	}
	
	char fontName[MAX_LEN] = FONT_ROOT;
	strcat(fontName, desiredFont);

	if (fontName[strlen(fontName) - 4] != '.')
	{
		strcat(fontName, ".ttf");
	}
	
	newFont = TTF_OpenFont(fontName, TextSettings.defaultTextPointSize);

	if (newFont == NULL)
	{ 
		list->name[head][0] = 0;
    	putConsoleString("\nFailed to load font! (%s)\n", SDL_GetError());
    	return NULL;
	}

	strcpy(list->name[head], newName);
	list->font[head] = newFont;

	list->head = (head + 1) % MAX_LOADED_FONTS;

	return newFont;
}

TTF_Font* loadFontWithSize(const char *desiredFont, const char *newName, float pointSize)
{
	TTF_Font *font = loadFont(desiredFont, newName);

	if (font != NULL)
	{
		TTF_SetFontSize(font, pointSize);
	}

	return font;
}

TTF_Font* getFont(const char *name)
{
	FontList *list = &TextSettings.FontList;

	for (int i = 0; i < MAX_LOADED_FONTS; i++)
	{
		if (strcmp(list->name[i], name) == 0 && list->font[i] != NULL)
		{
			return list->font[i];
		}
	}

	return NULL;
}

void setFontSize(const char *name, int size)
{
	TTF_Font *font = getFont(name);
	
	if (font == NULL)
	{
		return;
	}

	TTF_SetFontSize(font, size);

	return;
}


void initialiseFontList(FontList *input)
{
	input->head = 0;
	memset(input->name, 0, MAX_LOADED_FONTS * FONT_FILE_NAME_MAX);
	for (int i = 0; i < MAX_LOADED_FONTS; i++)
	{
		input->font[i] = NULL;
	}

	return;
}


void cleanUpTexts(TextList *list)
{
	if (list == NULL)
	{
		return;
	}

	Text *array = list->texts;

	for (int i = 0; i < MAX_TEXT_TEXTURES; i++)
	{
		if (array[i].text != NULL)
		{
			TTF_DestroyText(array[i].text);
			array[i].text = NULL;
		}

		array[i].beingUsed = false;
	}

	list->count = 0;

	return;
}

void cleanUpTextData(RenderFrame *ScreenData)
{
	cleanUpTexts(&TextSettings.TextList);
	cleanUpTexts(&TextSettings.DebugTexts);

	TTF_Font **fonts = TextSettings.FontList.font;
	for (int i = 0; i < MAX_LOADED_FONTS; i++)
	{
		if (fonts[i] != NULL)
		{
			TTF_CloseFont(fonts[i]);
			fonts[i] = NULL;
		}
	}
	TextSettings.FontList.head = 0;

	if (ScreenData->textEngine != NULL)
	{
		TTF_DestroyRendererTextEngine(ScreenData->textEngine);
		ScreenData->textEngine = NULL;
	}
	
	return;
}



int initialiseTextCharacter(Object *inputCharacter, char charValue, World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL || inputCharacter == NULL)
	{
		MarkObjectForDeletion(inputCharacter);
		return MISSING_DATA;
	}

	TextBox *inputText = GameWorld->TextQueue;

	inputCharacter->Parent = inputText->boxPtr;
	inputCharacter->ParentLink = MOTION_LINK;
	inputCharacter->ObjectBox->xSize = (int)inputText->TextSize;
	inputCharacter->ObjectBox->ySize = (int)inputText->TextSize;

	if (charValue > 32 && charValue < 123)
	{
		switchSpriteByName("Pixel_White", 0, getDisplay(inputCharacter));
		mapTextToCharacter(inputCharacter, charValue);
	}
	else
	{
		MarkObjectForDeletion(inputCharacter);

		// Just in case
		setRenderModeOverride(inputCharacter, DO_NOT_RENDER);
	}

	return LEMON_SUCCESS;
}


Object* createTextBox(TextBox *inputText, World *GameWorld)
{
	Object *Box = AddObject(GameWorld, UI_TEXT, inputText->currentXPos - inputText->boxOffsetX, inputText->currentYPos - inputText->boxOffsetY, TEXT_BOX, 0, 0, 0, 0);

	inputText->boxPtr = Box;

	DisplayData *boxDisplay = getDisplay(Box);

	if (inputText->boxPtr == NULL || boxDisplay == NULL)
	{
		return NULL;
	}

	switchSpriteByName(inputText->textBoxSprite, 0, boxDisplay);

	if (boxDisplay->spriteBuffer == NULL)
	{
		boxDisplay->currentSprite = -1;
		return Box;
	}

	matchBoxToDisplayDimensions(Box);

	return Box;
}


Object* createTextBoxPortrait(TextBox *inputText, World *GameWorld)
{
	if (inputText == NULL || inputText->boxPtr == NULL)
	{
		return NULL;
	}

	int boxXPos = (int)inputText->boxPtr->ObjectBox->xPos;
	int boxYPos = (int)inputText->boxPtr->ObjectBox->yPos + ((inputText->boxOffsetY - TextSettings.portraitSize) >> 1) + 44;

	Object *portrait = AddObjectWithParent(GameWorld, inputText->boxPtr, UI_TEXT, inputText->currentXPos, boxYPos, TEXT_PORTRAIT, 0, 0, 0, 0);

	if (portrait != NULL)
	{
		switchSpriteByName(inputText->Portrait, 0, getDisplay(portrait));

		portrait->ObjectBox->xSize = TextSettings.portraitSize;
		portrait->ObjectBox->ySize = TextSettings.portraitSize;

		if (inputText->PortraitPosition == PORTRAIT_INSIDE_BOX_LEFT)
		{
			inputText->boxOffsetX += TextSettings.portraitSize + 30;
		}
		else if (inputText->PortraitPosition == PORTRAIT_INSIDE_BOX_RIGHT)
		{
			portrait->ObjectBox->xPos = boxXPos + inputText->textLengthSize - TextSettings.portraitSize;
		}

		inputText->textLengthSize -= TextSettings.portraitSize + 30;
	}

	return portrait;
}

int mapTextToCharacter(Object *inputText, int characterValue)
{
	DisplayData *charDisplay = getDisplay(inputText);

	if (inputText == NULL || charDisplay == NULL || charDisplay->spriteBuffer == NULL)
	{
		return MISSING_DATA;
	}

	int height = charDisplay->spriteBuffer->height - 50;
	charDisplay->pixelXOffset = 4 + (56 * ((characterValue - 32) % 16));
	charDisplay->pixelYOffset = height - (56 * floor((float)(characterValue - 32) / 16.0));

	return LEMON_SUCCESS;
}

int getCharacterSpacing(char input)
{
	int widthValue = 0;

	switch(input)
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

	return widthValue;
}


int endTextBox(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL)
	{
		return MISSING_DATA;
	}

	TextBox *text = GameWorld->TextQueue;

	switch (text->textTypeSetting)
	{
		case TEXTBOX_OPTION_PROMPT:
		{
			struct TextOptionPrompt *optionData = &text->textTypeData.OptionPrompt;
			if (optionData->SelectedOption < 0 || optionData->SelectedOption >= optionData->numberOfOptions)
			{
				break;
			}

			triggerGameEvent(&optionData->optionTriggers[optionData->SelectedOption], GameWorld);
		} break;

		case TEXTBOX_TRIGGER_EVENT:
		{
			triggerGameEvent(&text->textTypeData.TriggerEvent, GameWorld);
		} break;

		default:
			break;
	}


	deleteTextBox(GameWorld);


	return LEMON_SUCCESS;
}


int deleteTextBox(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL)
	{
		return MISSING_DATA;
	}

	TextBox *textToDelete = GameWorld->TextQueue;
	GameWorld->TextQueue = textToDelete->nextText;

	MarkObjectForDeletion(textToDelete->boxPtr);

	DeleteTextSceneAction(textToDelete, GameWorld);

	removeAssociatedTexts(textToDelete);
	
	free(textToDelete);

	return LEMON_SUCCESS;
}


int clearTextQueue(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL) { return MISSING_DATA; }

	int i = 0;

	while (i < EngineSettings.MaxTextQueueLength && GameWorld->TextQueue != NULL)
	{
		deleteTextBox(GameWorld);
		i++;
	}

	return LEMON_SUCCESS;
}


bool textSceneActionPresent(TextBox *inputText, World *GameWorld)
{
	if (inputText == NULL || GameWorld == NULL || GameWorld->SceneActionQueue == NULL)
	{
		return false;
	}

	if (inputText->currentIndex != 0)
	{
		// if the textbox has already begun typing, skip checking for the presence of the scene action associated as it should only control when it starts
		return true;
	}

	SceneAction *currentAction = GameWorld->SceneActionQueue;

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


void DeleteTextSceneAction(TextBox *inputText, World *GameWorld)
{
	if (inputText == NULL || GameWorld == NULL || GameWorld->SceneActionQueue == NULL)
	{
		return;
	}


	SceneAction *currentAction = GameWorld->SceneActionQueue;

	while (currentAction != NULL)
	{
		if (currentAction->ActionID == SCENE_SAY_TEXT && currentAction->ActionData.sceneText == inputText)
		{
			currentAction = deleteSceneAction(currentAction, GameWorld);
			return;
		}
		else
		{
			currentAction = currentAction->nextSceneAction;
		}
	}	

	return;
}