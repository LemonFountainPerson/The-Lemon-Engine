#include "UIObjects.h"


int SpawnHUD(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
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
			double yPosOrigin = (13 * Y_TILESCALE) - (screenHeight >> 1);
			UIElement->ObjectBox->xPos = xPosOrigin;
			UIElement->ObjectBox->yPos = yPosOrigin;
			UIElement->arg2 = 0;

			Object *createdOption = NULL;


			//createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + (2 * X_TILESCALE), (screenHeight >> 1) - (Y_TILESCALE * 4), 200, 40, PAUSE_HEADER, 0, 0, 0, 0);
			
			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + (3 * X_TILESCALE), yPosOrigin, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 0, 0, 0, 0);

			switchObjectSpriteName("ResumeGame", createdOption, GameWorld->ObjectList);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + (3 * X_TILESCALE), yPosOrigin - (4 * Y_TILESCALE), X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 1, 0, 0, 0);

			switchObjectSpriteName("Settings", createdOption, GameWorld->ObjectList);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + (3 * X_TILESCALE), yPosOrigin - (8 * Y_TILESCALE), X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 2, 0, 0, 0);

			switchObjectSpriteName("QuitGame", createdOption, GameWorld->ObjectList);
			
			
			switchObjectSpriteName("OptionCursor", UIElement, GameWorld->ObjectList);
			SetDrawPriorityToFront(GameWorld->ObjectList, UIElement);
		} break;


		case SETTINGS_MENU_CONTROLLER:
		{
			// In order to have particles be visible in the pause menu while hiding normal particles, the camera is moved elsewhere
			// and is restored to its previous position when unpaused  (There should not be any level geometry before X pos 0)
			// When game is resumed, even if x pos is not reset to original value, it will be corrected to 0 by WorldCameraControl
			double xPosOrigin = (3 * X_TILESCALE) - (screenWidth >> 1);
			double yPosOrigin = (17 * Y_TILESCALE) - (screenHeight >> 1);
			UIElement->ObjectBox->xPos = xPosOrigin;
			UIElement->ObjectBox->yPos = yPosOrigin;
			UIElement->arg2 = 0;

			Object *createdOption = NULL;


			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin + X_TILESCALE, (screenHeight >> 1) - (Y_TILESCALE * 2), 100, 40, SETTINGS_HEADER, 0, 0, 0, 0);
			
			xPosOrigin += (3 * X_TILESCALE);

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin, yPosOrigin, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 0, 0, 0, 0);

			switchObjectSpriteName("RenderQuality_Option", createdOption, GameWorld->ObjectList);
			
			yPosOrigin -= 4 * Y_TILESCALE;

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin, yPosOrigin, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 1, 0, 0, 0);

			switchObjectSpriteName("Volume_Option", createdOption, GameWorld->ObjectList);
			
			yPosOrigin -= 4 * Y_TILESCALE;

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin, yPosOrigin, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 1, 0, 0, 0);

			switchObjectSpriteName("Test_Option", createdOption, GameWorld->ObjectList);
			
			yPosOrigin -= 4 * Y_TILESCALE;

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, xPosOrigin, yPosOrigin, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 2, 0, 0, 0);

			switchObjectSpriteName("BackButton", createdOption, GameWorld->ObjectList);
			
			
			switchObjectSpriteName("OptionCursor", UIElement, GameWorld->ObjectList);
			SetDrawPriorityToFront(GameWorld->ObjectList, UIElement);
		} break;


		case MOUSE_CURSOR:
			switchSpriteByName("MouseCursor", 0, UIElement->ObjectDisplay);

			AddFrameUpdateFunction(&UpdateCursor, UIElement, GameWorld);
		break;

		default:
		break;
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


	switch(UIText->arg1)
	{
		case UNDEFINED_UI_ELEMENT:
		MarkObjectForDeletion(UIText);
		break;

		case TEXT_BOX:
		break;

		case TEXT_CHARACTER:
		mapTextToCharacter(UIText);
		UIText->ParentLink = POSITION_LINK;
		break;

		case TEXT_PORTRAIT:
		UIText->ParentLink = POSITION_LINK;
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
		SettingsMenu(UIElement, GameWorld);
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

		default:
		break;
	}


	return LEMON_SUCCESS;
}


int UpdateCursor(Object *Cursor, World *GameWorld)
{
	if (Cursor == NULL || GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	Cursor->ObjectBox->xPos = MouseInput.xPos - (Cursor->ObjectBox->xSize >> 1);
	Cursor->ObjectBox->yPos = MouseInput.yPos - (Cursor->ObjectBox->ySize >> 1);


	SetDrawPriorityToFront(GameWorld->ObjectList, Cursor);

	if (MouseInput.LeftButton == 1)
	{
		if (GameWorld->Player != NULL && GameWorld->Player->PlayerBox != NULL)
		{
			int bulletXPos = (int)GameWorld->Player->PlayerBox->xPos;
			int bulletYPos = (int)GameWorld->Player->PlayerBox->yPos;

			Object *Bullet = AddObject(GameWorld, PROJECTILE, bulletXPos, bulletYPos, 0, 0, 0, 0, 0, 0, 0);

			PointObjectToMouse(Bullet, GameWorld, ROTATE_ALL);

			MouseInput.LeftButton = 2;
		}
	}

	if (MouseInput.RightButton == 1)
	{
		MarkObjectForDeletion(Cursor);

		MouseInput.RightButton = 2;
	}

	return LEMON_SUCCESS;
}


int PauseMenu(Object *MenuController, World *GameWorld)
{
	if (MenuController == NULL || GameWorld == NULL || MenuController->ObjectID != UI_ELEMENT)
	{
		return MISSING_DATA;
	}


	if (keyboard[LMN_DOWN])
	{
		keyboard[LMN_DOWN] = 0;

		MenuController->arg2++;
		MenuController->ObjectBox->yPos -= 4 * Y_TILESCALE;
	}

	if (keyboard[LMN_UP])
	{
		keyboard[LMN_UP] = 0;

		MenuController->arg2--;
		MenuController->ObjectBox->yPos += 4 * Y_TILESCALE;
	}

	if (MenuController->arg2 > 2)
	{
		MenuController->arg2 = 0;
		MenuController->ObjectBox->yPos = 13 * Y_TILESCALE - (screenHeight >> 1);
	}
	else if (MenuController->arg2 < 0)
	{
		MenuController->arg2 = 2;
		MenuController->ObjectBox->yPos = 5 * Y_TILESCALE - (screenHeight >> 1);
	}


	if (keyboard[LMN_INTERACT] == 1 || keyboard[LMN_JUMP] == 1 || keyboard[LMN_ENTER] == 1)
	{
		ClearKeyboardInput();

		switch (MenuController->arg2)
		{
			case 1:
				if (OpenSettings(GameWorld) == 0)
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



int SettingsMenu(Object *MenuController, World *GameWorld)
{
	if (MenuController == NULL || GameWorld == NULL || MenuController->ObjectID != UI_ELEMENT)
	{
		return MISSING_DATA;
	}


	if (keyboard[LMN_DOWN])
	{
		keyboard[LMN_DOWN] = 0;

		MenuController->arg2++;
		MenuController->ObjectBox->yPos -= 4 * Y_TILESCALE;
	}

	if (keyboard[LMN_UP])
	{
		keyboard[LMN_UP] = 0;

		MenuController->arg2--;
		MenuController->ObjectBox->yPos += 4 * Y_TILESCALE;
	}

	if (MenuController->arg2 > 3)
	{
		MenuController->arg2 = 0;
		MenuController->ObjectBox->yPos = (17 * Y_TILESCALE) - (screenHeight >> 1);
	}
	else if (MenuController->arg2 < 0)
	{
		MenuController->arg2 = 3;
		MenuController->ObjectBox->yPos = (5 * Y_TILESCALE) - (screenHeight >> 1);
	}


	if (keyboard[LMN_INTERACT] == 1 || keyboard[LMN_JUMP] == 1 || keyboard[LMN_ENTER] == 1)
	{
		ClearKeyboardInput();

		switch (MenuController->arg2)
		{
			case 0:
				screenHeight += 2;
				GameWorld->GameEvent = CHANGE_SCREEN_SIZE;
				break;

			case 1:
				break;

			case 2:
				break;

			case 3:
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

	if (Button->ParentObject->arg2 == Button->arg2 && Button->arg3 == 0)
	{
		Button->ObjectDisplay->currentSprite++;
		Button->arg3 = 1;
	}
	else if (Button->ParentObject->arg2 != Button->arg2 && Button->arg3 == 1)
	{
		Button->ObjectDisplay->currentSprite--;
		Button->arg3 = 0;
	}

	return 0;
}


TextInstance* SayText(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld)
{
	TextInstance *newText = CreateText(inputPhrase, GameWorld);

	ApplyTextPresets(newText, Portrait, inputPreset);

	return newText;
}


int ApplyTextPresets(TextInstance *inputText, const char Portrait[], TextPreset inputPreset)
{
	if (inputText == NULL)
	{
		return MISSING_DATA;
	}

	int topTextLocation = (screenHeight >> 1) - 80;
	int bottomTextLocation = 240 - (screenHeight >> 1);

	// Default settings
	inputText->textXPos = -570;
	inputText->textEndXPos = inputText->textXPos + 1140;
	inputText->textYPos = bottomTextLocation;
	inputText->textDelayFrames = 3;
	strcpy(inputText->Portrait, Portrait);
	strcpy(inputText->font, "Pixel_White");

	switch (inputPreset)
	{
		case PLAINTEXT_BOTTOM:
		strcpy(inputText->voice, "Text_snd");
		memset(inputText->textBoxSprite, 0, MAX_LEN);
		break;

		case PLAINTEXT_TOP:
		inputText->textYPos = topTextLocation;
		strcpy(inputText->voice, "Text_snd");
		memset(inputText->textBoxSprite, 0, MAX_LEN);
		break;

		case BW_TOP:
		strcpy(inputText->textBoxSprite, "TextBox_White");
		strcpy(inputText->font, "Pixel_Black");
		inputText->textYPos = topTextLocation;
		strcpy(inputText->voice, "Text_snd");
		break;

		case BW_BOTTOM:
		strcpy(inputText->textBoxSprite, "TextBox_White");
		strcpy(inputText->font, "Pixel_Black");
		strcpy(inputText->voice, "Text_snd");
		break;

		case WB_TOP:
		inputText->textYPos = topTextLocation;
		strcpy(inputText->voice, "Text_snd");
		break;

		case WB_TOP_FLIP:
		inputText->textYPos = topTextLocation;
		strcpy(inputText->voice, "Text_snd");
		inputText->PortraitPosition = INSIDE_BOX_RIGHT;
		break;

		case WB_BOTTOM:
		strcpy(inputText->voice, "Text_snd");
		break;

		case WB_BOTTOM_FLIP:
		strcpy(inputText->voice, "Text_snd");
		inputText->PortraitPosition = INSIDE_BOX_RIGHT;
		break;

		case SILENT_TOP:
		inputText->textYPos = topTextLocation;
		break;


		default:
		break;
	}

	return LEMON_SUCCESS;
}


TextInstance* CreateText(const char inputPhrase[], World *GameWorld)
{
	if (GameWorld == NULL || inputPhrase == NULL || inputPhrase[0] < 9)
	{
		return NULL;
	}

	// Check that string lengths are acceptable (< 200)
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
	strcpy(newText->textPhrase, inputPhrase);
	memset(newText->voice, 0, MAX_LEN);
	memset(newText->Portrait, 0, MAX_LEN);
	newText->textPhrase[MAX_TEXT_LENGTH - 1];
	newText->currentXPos = 0;
	newText->currentYPos = 0;
	newText->textXPos = 0;
	newText->textEndXPos = 0;
	newText->textYPos = 0;
	newText->currentChar = 0;
	newText->Counter = 0;
	newText->SceneTickOnTextEnd = 0;

	newText->textDelayFrames = 0;
	strcpy(newText->font, "Pixel_Regular");
	strcpy(newText->textBoxSprite, "TextBox_Basic");
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

		while (i < MAX_TEXTQUEUE_LENGTH && currentText->nextText != NULL)
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

	return newText;
}


TextInstance* SayTextCutscene(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld, int SceneTickOnTextEnd)
{
	TextInstance *newText = SayText(inputPhrase, Portrait, inputPreset, GameWorld);

	if (newText != NULL)
	{
		newText->SceneTickOnTextEnd = SceneTickOnTextEnd;
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

	if (GameWorld->GameState == CLOSE_GAME || GameWorld->GameState == EMPTY_GAME || GameWorld->GamePaused == 1 || GameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}


	TextInteraction(GameWorld);


	if (GameWorld->TextQueue == NULL || GameWorld->TextQueue->currentChar < 0)
	{
		return EXECUTION_UNNECESSARY;
	}


	TextInstance *currentText = GameWorld->TextQueue;

	// Skip text animation if skip button is held
	if (keyboard[LMN_INTERACT2] == 1 && currentText->Skippable == 1)
	{
		keyboard[LMN_INTERACT] = 0;

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

		// Play voice
		if (currentText->voice != NULL && currentText->voice[0] > 32)
		{
			LemonPlaySound(currentText->voice, "Voices", SPEECH, 1.0);

			if (currentText->voiceMode == PLAY_ONCE)
			{
				currentText->voice[0] = 0;
			}
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
	 
	switch (GameWorld->TextQueue->textTypeSetting)
	{

		default:
			if (keyboard[LMN_INTERACT] == 1)
			{
				keyboard[LMN_INTERACT] = 0;
				endTextInstance(GameWorld);
			}
			break;
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
		// Start text

		// create text box
		createTextBox(inputText, GameWorld);

		if (inputText->boxPtr == NULL)
		{
			endTextInstance(GameWorld);
			return ERROR;
		}
	
		// Create portrait
		if (inputText->Portrait[0] != 0)
		{
			createTextBoxPortrait(inputText, GameWorld);
		}

		// create first character
		inputText->currentXPos = inputText->textXPos;
		inputText->currentYPos = inputText->textYPos;
		createTextCharacter(inputText, GameWorld);
		
		GameWorld->PlayingText = 1;
	}
	else
	{
		// Spawn next character		
		if (inputText->currentXPos + 40 > inputText->textEndXPos || currentChar == 10)
		{
			inputText->currentXPos = inputText->textXPos;
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

		
		createTextCharacter(inputText, GameWorld);
	}

	
	inputText->currentChar++;

	
	// Finished creating text
	if (inputText->currentChar > MAX_TEXT_LENGTH || inputText->textPhrase[inputText->currentChar] == 0)
	{
		inputText->currentChar = -1;
	}

	
	return LEMON_SUCCESS;
}


Object* createTextCharacter(TextInstance *inputText, World *GameWorld)
{
	char currentChar = inputText->textPhrase[inputText->currentChar];

	Object *character = NULL;

	if (currentChar > 32 && currentChar < 123)
	{
		character = AddObjectWithParent(GameWorld, inputText->boxPtr, UI_TEXT, inputText->currentXPos, inputText->currentYPos, 40, 40, TEXT_CHARACTER, currentChar, 0, 0, 0);

		if (character != NULL)
		{
			switchSpriteByName(inputText->font, 0, character->ObjectDisplay);
		}
	}

	if (currentChar > 31 && currentChar < 123)
	{
		inputText->currentXPos += getCharacterSpacing(currentChar);
	}

	return LEMON_SUCCESS;
}


Object* createTextBox(TextInstance *inputText, World *GameWorld)
{
	Object *TextBox = AddObject(GameWorld, UI_TEXT, inputText->textXPos - 30, inputText->textYPos - 190, 1200, 260, TEXT_BOX, 0, 0, 0, 0);

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

	TextBox->ObjectBox->xSize = switchedSprite->width;
	TextBox->ObjectBox->ySize = switchedSprite->height;

	return TextBox;
}


Object* createTextBoxPortrait(TextInstance *inputText, World *GameWorld)
{
	if (inputText == NULL)
	{
		return NULL;
	}

	Object *portrait = AddObjectWithParent(GameWorld, inputText->boxPtr, UI_TEXT, inputText->textXPos, inputText->textYPos - 160, 200, 200, TEXT_PORTRAIT, 0, 0, 0, 0);

	if (portrait != NULL)
	{
		switchSpriteByName(inputText->Portrait, 0, portrait->ObjectDisplay);

		if (inputText->PortraitPosition == INSIDE_BOX_LEFT)
		{
			inputText->textXPos += 230;
		}
		else if (inputText->PortraitPosition == INSIDE_BOX_RIGHT)
		{
			portrait->ObjectBox->xPos = inputText->textEndXPos - 200;
			inputText->textEndXPos -= 230;
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

	if (GameWorld->TextQueue->SceneTickOnTextEnd > 0)
	{
		GameWorld->SceneTick = GameWorld->TextQueue->SceneTickOnTextEnd;
	}

	MarkObjectForDeletion(GameWorld->TextQueue->boxPtr);

	TextInstance *textToDelete = GameWorld->TextQueue;

	GameWorld->TextQueue = GameWorld->TextQueue->nextText;

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

	while (i < MAX_TEXTQUEUE_LENGTH && GameWorld->TextQueue != NULL)
	{
		endTextInstance(GameWorld);
		i++;
	}

	if (GameWorld->TextQueue != NULL) { return LEMON_ERROR; }

	return LEMON_SUCCESS;
}


int mapTextToCharacter(Object *inputText)
{
	if (inputText == NULL || inputText->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	inputText->ObjectDisplay->pixelXOffset = 8 + (56 * ((inputText->arg2 - 32) % 16));
	inputText->ObjectDisplay->pixelYOffset = 8 + (56 * (5 - floor((double)(inputText->arg2 - 32) / 16.0)));

	return 0;
}


int getCharacterSpacing(char inputCharacter)
{

	switch(inputCharacter)
	{
		case 'i':
		case 'l':
		case '!':
		case 39:
		case 46:
		return 16;

		case 'r':
		case 'f':
		case 32:
		return 24;

		case 'O':
		case 'N':
		case 'J':
		return 40;

		case 'w':
		case 'W':
		case 'm':
		case 'M':
		case 'Q':
		case '-':
		return 48;

		case 8:
		case 10:
		case 11:
		case 12:
		case 13:
		return 0;

		default:
		return 32;

	}

	return 0;
}