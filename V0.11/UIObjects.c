#include "LemonEngine.h"


int SpawnHUD(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}


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

void adjustHUD(int prevScreenWidth, int prevScreenHeight, RenderFrame *ScreenData, World *GameWorld)
{
	if (prevScreenWidth == ScreenData->screenWidth && prevScreenHeight == ScreenData->screenHeight)
	{
		return;
	}

	if (prevScreenWidth < 1 || prevScreenHeight < 1 || GameWorld == NULL)
	{
		// just in case to avoid division by zero
		return;
	}

	float differenceValueX = (float)ScreenData->screenWidth / (float)prevScreenWidth;
	float differenceValueY = (float)ScreenData->screenHeight / (float)prevScreenHeight;


	
	TTF_Font **fontList = GameWorld->FontList.fonts;
	float oldSize = 0;

	for (int i = 0; i < MAX_LOADED_FONTS; i++)
	{
		if (fontList[i] != NULL)
		{
			oldSize = TTF_GetFontSize(fontList[i]);
			TTF_SetFontSize(fontList[i], oldSize * differenceValueY);
		}
	}

	Text *textList = GameWorld->TextList.texts;

	for (int i = 0; i < MAX_TEXTS; i++)
	{
		if (textList[i].text != NULL)
		{
			textList[i].xPos *= differenceValueX;
			textList[i].yPos *= differenceValueY;
		}
	}

	//TextSettings.defaultTextPointSize *= differenceValueX;

	// TextBox *textBox = GameWorld->TextQueue;
	// if (textBox != NULL && textBox->boxPtr != NULL)
	// {
	// 	PhysicsBox *box = textBox->boxPtr->ObjectBox;
	// 	box->xPos *= differenceValueX;
	// 	box->yPos *= differenceValueY;
		// box->xSize *= differenceValueX;
		// box->ySize *= differenceValueY;
		// textBox->TextSize *= differenceValueX;
		// textLengthSize *= differenceValueX;
	// }

	return;
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
	if (GameWorld == NULL || UIElement == NULL)
	{
		return MISSING_DATA;
	}

	setDisplayLayer(UIElement, HUD);
	UIElement->ObjectBox->collideLayer = HUD;
	UIElement->ObjectBox->solid = UNSOLID;
	UIElement->reserved |= RFLAG_CUTSCENE_IMMUNITY;	// mark this object as immune to being frozen during cutscenes


	switch(getSubType(UIElement))
	{
	case LEVEL_FADE:
		UIElement->Action = 0;
		UIElement->reserved |= RFLAG_PRESERVE_OBJECT;	// This means do not delete between level transitions
		setTransparency(UIElement, 1.0);
		switchSpriteByName("FadeOut", 0, getDisplay(UIElement));
		UIElement->ObjectBox->xSize = ScreenData.HUDWidth;
		UIElement->ObjectBox->ySize = ScreenData.HUDHeight;
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
		float xPosOrigin = (3 * X_TILESCALE) - (ScreenData.HUDWidth >> 1);
		AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin, 132, PAUSE_HEADER, 0, 0, 0, 0);

		basicMenuCreation(xPosOrigin, 128, UIElement, GameWorld, 3, 
			"ResumeGame", "Settings", "QuitGame");
		setObjectName(UIElement, "PauseController");
	} break;


	case SAVE_OPTIONS_CONTROLLER:
	{
		float xPosOrigin = (3 * X_TILESCALE) - (ScreenData.HUDWidth >> 1);

		basicMenuCreation(xPosOrigin, 128, UIElement, GameWorld, 5, 
			"Save1", "Save2", "SaveState", "LoadState", "BackOption");
		setObjectName(UIElement, "SaveController");
	} break;

	case SETTINGS_MENU_CONTROLLER:
	{
		float xPosOrigin = (3 * X_TILESCALE) - (ScreenData.HUDWidth >> 1);

		//AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);
		AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + X_TILESCALE, 260, SETTINGS_HEADER, 0, 0, 0, 0);
		
		basicMenuCreation(xPosOrigin, 128, UIElement, GameWorld, 4, 
			"VideoSettings", "SoundSettings", "Settings", "BackOption");
		setObjectName(UIElement, "MenuController");
	} break;


	case VIDEO_SETTINGS_CONTROLLER:
	{
		float xPosOrigin = (3 * X_TILESCALE) - (ScreenData.HUDWidth >> 1);

		//AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);
		AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + X_TILESCALE, 328, SETTINGS_HEADER, 0, 0, 0, 0);
		
		basicMenuCreation(xPosOrigin, 128, UIElement, GameWorld, 5, 
			"Settings", "Settings", "Settings", "Settings", "BackOption");
		setObjectName(UIElement, "VideoController");
	} break;


	case SOUND_SETTINGS_CONTROLLER:
	{
		float xPosOrigin = (3 * X_TILESCALE) - (ScreenData.HUDWidth >> 1);

		//AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);
		AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + X_TILESCALE, 260, SETTINGS_HEADER, 0, 0, 0, 0);
		
		basicMenuCreation(xPosOrigin, 128, UIElement, GameWorld, 4, 
			"Settings", "Settings", "Settings", "BackOption");
		setObjectName(UIElement, "SoundController");
	} break;


	case MOUSE_CURSOR:
		switchSpriteByName("MouseCursor", USE_CURRENT_SPRITESET, getDisplay(UIElement));
		setDisplayLayer(UIElement, FRONT_LAYER);
		UIElement->ObjectBox->solid = SOLID;
		UIElement->ObjectBox->xSize = 64;
		UIElement->ObjectBox->ySize = 64;
		break;

	default:
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

	SetDrawPriorityToFront(&GameWorld->ObjectList, controller);
	switchObjectSpriteByName("OptionCursor", controller);
	

	return LEMON_SUCCESS;
}


int UpdateUIElement(World *GameWorld, Object *UIElement)
{
	if (GameWorld == NULL || UIElement == NULL)
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
				startTimer(0.5, UIElement, GameWorld);
				switchLevel(UIElement->arg2, GameWorld);
			}
		}
		else if (timerExpired(UIElement, GameWorld))
		{
			changeTransparency(UIElement, 0.05);
			if (getTransparency(UIElement) > 0.99)
			{
				MarkObjectForDeletion(UIElement);
			}
		}
		break;

	case MOUSE_CURSOR:
		UpdateCursor(UIElement, GameWorld);
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

	Object *PlayerObject = GameWorld->Player.PlayerPtr;

	if (MouseInput.LeftButton)
	{
		setScaleSize(Cursor, 0.75);

		if (PlayerObject != NULL && Cursor->Action == IDLE)
		{
			Object *Bullet = AddNamedObject(GameWorld, "PlayerBullet", PROJECTILE, 0, 0);
			addBulletComponent(Bullet, PlayerObject, 30, SPARKLE, GameWorld);
			PointObjectToMouse(Bullet, GameWorld);
			
			// Bullet = AddNamedObject(GameWorld, "PlayerBullet", PROJECTILE, 0, 0);
			// addBulletComponent(Bullet, PlayerObject, 25, SPARKLE, GameWorld);
			// PointObjectToMouse(Bullet, GameWorld);
			// RotateObject(Bullet, 7.0);

			// Bullet = AddNamedObject(GameWorld, "PlayerBullet", PROJECTILE, 0, 0);
			// addBulletComponent(Bullet, PlayerObject, 25, SPARKLE, GameWorld);
			// PointObjectToMouse(Bullet, GameWorld);
			// RotateObject(Bullet, -7.0);

			// Bullet = AddNamedObject(GameWorld, "PlayerBullet", PROJECTILE, 0, 0);
			// addBulletComponent(Bullet, PlayerObject, 20, SPARKLE, GameWorld);
			// PointObjectToMouse(Bullet, GameWorld);
			// RotateObject(Bullet, 14.0);

			// Bullet = AddNamedObject(GameWorld, "PlayerBullet", PROJECTILE, 0, 0);
			// addBulletComponent(Bullet, PlayerObject, 20, SPARKLE, GameWorld);
			// PointObjectToMouse(Bullet, GameWorld);
			// RotateObject(Bullet, -14.0);


			Cursor->Action = MOUSECLICK_INTERRUPT;
			Cursor->arg2 = 0;
		}
	}

	smoothSizeChangeTo(Cursor, 1.0, 6.0);
	if (Cursor->arg2 < 1)
	{
		Cursor->Action = IDLE;
	}
	if (Cursor->Action == MOUSECLICK_INTERRUPT)
	{
		Cursor->arg2--;
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


	if ((buttons[LMN_MENU_CONFIRM] == 1 && MouseInput.LeftButton != 1) || MenuController->Action == MOUSECLICK_INTERRUPT)
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
		if (buttons[LMN_MENU_CONFIRM] == 1 || Button->Action == MOUSECLICK_INTERRUPT)
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