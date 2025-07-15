#include "UIObjects.h"


int SpawnHUD(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	

	return 0;
}


int InitialiseUIElement(World *GameWorld, Object *UIElement)
{
	if (GameWorld == NULL || UIElement == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	UIElement->layer = HUD;
	UIElement->ObjectBox->solid = UNSOLID;


	switch(UIElement->arg1)
	{
		case TEXT_BOX:
		switchSpriteByName("TextBox_Basic", 0, UIElement->ObjectDisplay);
		switchSprite(UIElement->ObjectDisplay->currentSprite + UIElement->arg2, 0, UIElement->ObjectDisplay);

		if (UIElement->ObjectDisplay->spriteBuffer != NULL)
		{
			UIElement->ObjectBox->xSize = UIElement->ObjectDisplay->spriteBuffer->width;
			UIElement->ObjectBox->ySize = UIElement->ObjectDisplay->spriteBuffer->height;
		}
	
		break;

		case TEXT_CHARACTER:
		switchSpriteByName("Text_0", 0, UIElement->ObjectDisplay);
		switchSprite(UIElement->ObjectDisplay->currentSprite + UIElement->arg3, 0, UIElement->ObjectDisplay);
		mapTextToCharacter(UIElement);
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
			UIElement->ObjectBox->xPos = 3 * X_TILESCALE;
			UIElement->ObjectBox->yPos = 13 * Y_TILESCALE;
			UIElement->arg2 = 0;

			Object *createdOption = NULL;


			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 5 * X_TILESCALE, screenHeight - (Y_TILESCALE * 4), 200, 40, PAUSE_HEADER, 0, 0, 0, 0);
			
			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 13 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 0, 0, 0, 0);

			switchObjectSpriteName("ResumeGame", createdOption, GameWorld->ObjectList);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 9 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 1, 0, 0, 0);

			switchObjectSpriteName("Settings", createdOption, GameWorld->ObjectList);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 5 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 2, 0, 0, 0);

			switchObjectSpriteName("QuitGame", createdOption, GameWorld->ObjectList);
			
			
			switchObjectSpriteName("OptionCursor", UIElement, GameWorld->ObjectList);
			SetDrawPriorityToFront(GameWorld->ObjectList, UIElement);
		} break;


		case SETTINGS_MENU_CONTROLLER:
		{
			// In order to have particles be visible in the pause menu while hiding normal particles, the camera is moved elsewhere
			// and is restored to its previous position when unpaused  (There should not be any level geometry before X pos 0)
			// When game is resumed, even if x pos is not reset to original value, it will be corrected to 0 by WorldCameraControl
			UIElement->ObjectBox->xPos = 3 * X_TILESCALE;
			UIElement->ObjectBox->yPos = 17 * Y_TILESCALE;
			UIElement->arg2 = 0;

			Object *createdOption = NULL;


			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 4 * X_TILESCALE, screenHeight - (Y_TILESCALE * 2), 100, 40, SETTINGS_HEADER, 0, 0, 0, 0);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 17 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 0, 0, 0, 0);

			switchObjectSpriteName("RenderQuality_Option", createdOption, GameWorld->ObjectList);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 13 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 1, 0, 0, 0);

			switchObjectSpriteName("Volume_Option", createdOption, GameWorld->ObjectList);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 9 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 1, 0, 0, 0);

			switchObjectSpriteName("Test_Option", createdOption, GameWorld->ObjectList);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 5 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 2, 0, 0, 0);

			switchObjectSpriteName("BackButton", createdOption, GameWorld->ObjectList);
			
			
			switchObjectSpriteName("OptionCursor", UIElement, GameWorld->ObjectList);
			SetDrawPriorityToFront(GameWorld->ObjectList, UIElement);
		} break;


		default:
		break;
	}


	return 0;
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

	loadSpriteIntoSpriteSet("Test_Face", "TextDisplay", newSet, TILE);

	loadSpriteIntoSpriteSet("TextBox_Basic", "TextDisplay", newSet, TILE_FAST);
	loadSpriteIntoSpriteSet("TextBox_White", "TextDisplay", newSet, TILE_FAST);

	loadSpriteIntoSpriteSet("Text_0", "TextDisplay", newSet, TILE_FAST);
	loadSpriteIntoSpriteSet("Text_1", "TextDisplay", newSet, TILE);

	return 0;
}



int UpdateUIElement(World *GameWorld, Object *UIElement, int keyboard[256])
{
	if (GameWorld == NULL || UIElement == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}


	switch (UIElement->arg1)
	{
		case PAUSE_MENU_CONTROLLER:
		PauseMenu(UIElement, GameWorld, keyboard);
		break;

		case SETTINGS_MENU_CONTROLLER:
		SettingsMenu(UIElement, GameWorld, keyboard);
		break;

		case OPTION_BUTTON:
		UpdateOptionButton(UIElement);
		break;

		default:
		break;
	}


	return 0;
}


int PauseMenu(Object *MenuController, World *GameWorld, int keyboard[256])
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
		MenuController->ObjectBox->yPos = 13 * Y_TILESCALE;
	}
	else if (MenuController->arg2 < 0)
	{
		MenuController->arg2 = 2;
		MenuController->ObjectBox->yPos = 5 * Y_TILESCALE;
	}


	if (keyboard[LMN_INTERACT] == 1 || keyboard[LMN_JUMP] == 1)
	{
		keyboard[LMN_INTERACT] = 0;
		keyboard[LMN_JUMP] = 0;

		switch (MenuController->arg2)
		{
			case 1:
				if (OpenSettings(GameWorld, keyboard) == 0)
				{
					MarkObjectForDeletion(MenuController, GameWorld->ObjectList);
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
		MarkObjectForDeletion(MenuController, GameWorld->ObjectList);
	}

	return 0;
}



int SettingsMenu(Object *MenuController, World *GameWorld, int keyboard[256])
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
		MenuController->ObjectBox->yPos = 17 * Y_TILESCALE;
	}
	else if (MenuController->arg2 < 0)
	{
		MenuController->arg2 = 3;
		MenuController->ObjectBox->yPos = 5 * Y_TILESCALE;
	}


	if (keyboard[LMN_INTERACT] == 1 || keyboard[LMN_JUMP] == 1)
	{
		keyboard[LMN_INTERACT] = 0;
		keyboard[LMN_JUMP] = 0;

		switch (MenuController->arg2)
		{
			case 0:
				break;

			case 1:
				break;

			case 2:
				break;


			default:
				PauseGame(GameWorld, keyboard);
				MarkObjectForDeletion(MenuController, GameWorld->ObjectList);
				break;
		}
	}

	if (GameWorld->GamePaused == 0 && GameWorld->GameState != IN_MENU)
	{
		MarkObjectForDeletion(MenuController, GameWorld->ObjectList);
	}

	return 0;
}


int UpdateOptionButton(Object *Button)
{
	if (Button == NULL || Button->ParentObject == NULL || Button->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
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
	char voice[MAX_LEN] = {0};
	Fonts font = PIXEL_REGULAR;
	int xPos = (screenWidth >> 1) - 580;
	int yPos = 240;
	int textBox = 0;
	int textDelay = 3;
	int skipState = 1;
	VoiceMode voiceMode = PLAY_EACH_CHARACTER;


	switch (inputPreset)
	{
		case INVERTED_TOP:
		textBox = 1;
		font = PIXEL_BLACK;
		yPos = screenHeight - 80;
		strcpy(voice, "Text_snd");
		break;

		case INVERTED_BOTTOM:
		textBox = 1;
		font = PIXEL_BLACK;
		strcpy(voice, "Text_snd");
		break;

		case DEFAULT_TOP:
		yPos = screenHeight - 80;
		strcpy(voice, "Text_snd");
		break;

		case DEFAULT_BOTTOM:
		strcpy(voice, "Text_snd");
		break;

		case SILENT_TOP:
		yPos = screenHeight - 80;
		break;


		default:
		break;
	}


	TextInstance *newText = CreateText(inputPhrase, Portrait, voice, voiceMode, font, textBox, textDelay, skipState, xPos, yPos, GameWorld);

	return newText;
}


TextInstance* CreateText(const char inputPhrase[], const char Portrait[], const char Voice[], VoiceMode voiceMode, int font, int textBox, int textDelay, int skipState, int xPos, int yPos, World *GameWorld)
{
	if (GameWorld == NULL || inputPhrase == NULL || inputPhrase[0] < 1)
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

	currentChar = 0;

	while (Voice[currentChar] != 0 && Portrait[currentChar] != 0 && currentChar < MAX_LEN)
	{
		currentChar++;
	}

	if (currentChar >= MAX_LEN)
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
	newText->firstChar = NULL;
	strcpy(newText->textPhrase, inputPhrase);
	strcpy(newText->voice, Voice);
	strcpy(newText->Portrait, Portrait);
	newText->textPhrase[MAX_TEXT_LENGTH - 1];
	newText->xOffset = xPos;
	newText->yOffset = yPos;
	newText->currentChar = 0;
	newText->Counter = 0;
	newText->Skippable = skipState;
	newText->PlayOnSceneTick = 0;
	newText->SceneTickOnTextEnd = 0;
	newText->voiceMode = voiceMode;

	newText->textDelay = clamp(textDelay, 0, 60);
	newText->font = clamp(font, 0, 999);
	newText->textBoxSprite = clamp(textBox, 0, 999);

	if (GameWorld->TextQueue == NULL)
	{
		GameWorld->TextQueue = newText;
	}
	else
	{
		TextInstance *currentText;
		currentText = GameWorld->TextQueue;

		while (currentText->nextText != NULL)
		{
			currentText = currentText->nextText;
		}

		currentText->nextText = newText;
	}

	return newText;
}


TextInstance* SayTextCutscene(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld, int PlayOnSceneTick, int SceneTickOnTextEnd)
{
	TextInstance *newText = SayText(inputPhrase, Portrait, inputPreset, GameWorld);

	if (newText != NULL)
	{
		newText->PlayOnSceneTick = PlayOnSceneTick;
		newText->SceneTickOnTextEnd = SceneTickOnTextEnd;
	}

	return newText;
}


TextInstance* CreateTextBasic(const char inputPhrase[], const char Portrait[], const char Voice[], VoiceMode voiceMode, int font, int textDelay, int skipState, int yPos, World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return NULL;
	}

	TextInstance *newText = CreateText(inputPhrase, Portrait, Voice, voiceMode, font, 0, textDelay, skipState, 60, yPos, GameWorld);
	
	return newText;
}


TextInstance* CreateTextCutscene(const char inputPhrase[], const char Portrait[], const char Voice[], VoiceMode voiceMode, int font, int textBox, int textDelay, int skipState, int xPos, int yPos, World *GameWorld, int PlayOnSceneTick, int SceneTickOnTextEnd)
{
	TextInstance *newText = CreateText(inputPhrase, Portrait, Voice, voiceMode, font, textBox, textDelay, skipState, xPos, yPos, GameWorld);

	if (newText != NULL)
	{
		newText->PlayOnSceneTick = PlayOnSceneTick;
		newText->SceneTickOnTextEnd = SceneTickOnTextEnd;
	}

	return newText;
}


// Play the text at the first slot in the linked list, then delete and shift everything up when done with that instance
int UpdateText(World *GameWorld, int keyboard[256])
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState == CLOSE_GAME || GameWorld->GameState == EMPTY_GAME || GameWorld->GamePaused == 1 || GameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}

	if (GameWorld->TextQueue->currentChar < 0 && keyboard[LMN_INTERACT] == 1)
	{
		keyboard[LMN_INTERACT] = 0;
		endTextInstance(GameWorld);

		if (GameWorld->TextQueue == NULL)
		{
			return MISSING_DATA;
		}

	}

	if (GameWorld->TextQueue->currentChar < 0)
	{
		return EXECUTION_UNNECESSARY;
	}


	if (GameWorld->TextQueue->PlayOnSceneTick != 0 && GameWorld->TextQueue->PlayOnSceneTick != GameWorld->SceneTick)
	{
		return ACTION_DISABLED;
	}


	TextInstance *currentText = GameWorld->TextQueue;

	// Skip text animation if skip button is held
	if (keyboard[LMN_INTERACT2] == 1 && GameWorld->TextQueue->Skippable == 1)
	{
		keyboard[LMN_INTERACT] = 0;
		keyboard[LMN_INTERACT2] = 0;

		while (currentText->currentChar > 0)
		{
			GameWorld->TextQueue->Counter = currentText->textDelay;
			displayNextCharacter(currentText, GameWorld);
		}
	}
	else
	{
		displayNextCharacter(currentText, GameWorld);
	}
	

	return 0;
}


int displayNextCharacter(TextInstance *inputText, World *GameWorld)
{
	if (inputText == NULL || inputText->currentChar == -1)
	{
		return MISSING_DATA;
	}

	inputText->Counter++;

	if (!(inputText->Counter >= inputText->textDelay || inputText->currentChar == 0))
	{
		return ACTION_DISABLED;
	}

	inputText->Counter = 0;


	// Play voice
	if (inputText->voice[0] > 32 && (inputText->currentChar == 0 || inputText->voiceMode == PLAY_EACH_CHARACTER) )
	{
		if (LemonPlaySound(inputText->voice, "Voices", SPEECH, 1.0) != LEMON_SUCCESS)
		{
			inputText->voice[0] = 0;
		}
	}


	// Start text
	if (inputText->currentChar == 0)
	{
		// create text box
		Object *TextBox = AddObject(GameWorld, UI_ELEMENT, inputText->xOffset - 30, inputText->yOffset - 190, 1214, 260, TEXT_BOX, inputText->textBoxSprite, 0, 0, 0);


		// Create portrait
		Object *portrait = NULL;
		if (inputText->Portrait[0] != 0)
		{
			portrait = AddObject(GameWorld, UI_ELEMENT, inputText->xOffset, inputText->yOffset - 160, 200, 200, BASIC_GRAPHIC, 0, 0, 0, 0);

			if (portrait != NULL)
			{
				switchSpriteByName(inputText->Portrait, 0, portrait->ObjectDisplay);
			}

			inputText->xOffset += 230;
		}

		// create first character
		inputText->firstChar = AddObject(GameWorld, UI_ELEMENT, inputText->xOffset, inputText->yOffset, 40, 40, TEXT_CHARACTER, inputText->textPhrase[0], inputText->font, 0, 0);

		if (inputText->firstChar == NULL)
		{
			MarkObjectForDeletion(portrait, GameWorld->ObjectList);
			MarkObjectForDeletion(TextBox, GameWorld->ObjectList);
			endTextInstance(GameWorld);
			return ERROR;
		}

		if (portrait != NULL)
		{
			portrait->ParentObject = inputText->firstChar;
		}

		if (TextBox != NULL)
		{
			TextBox->ParentObject = inputText->firstChar;
		}


		GameWorld->PlayingText = 1;
		inputText->currentChar = 1;
	}
	else
	{
		// Spawn next character
		char currentChar = inputText->textPhrase[inputText->currentChar];
		
		if (inputText->xOffset + 100 > (screenWidth >> 1) + 600 || currentChar == 10)
		{
			inputText->xOffset = inputText->firstChar->ObjectBox->xPos;
			inputText->yOffset -= 50;
		}
		else
		{
			switch(currentChar)
			{
				case 13:	// (/r) is repurposed to insert a delay of 30 ticks
				inputText->Counter = -30;
				break;

				case 12:	// (/f) is repurposed to insert a delay of 12 ticks  
				inputText->Counter = -12;
				break;
		
				case 11:		// (/v) is repurposed to insert a delay of 8 ticks 
				inputText->Counter = -8;
				break;

				default:
				break;
			}

			inputText->xOffset += getCharacterSpacing(inputText->textPhrase[inputText->currentChar - 1]);
		}

		if (currentChar > 32 && currentChar < 123)
		{
			AddObjectWithParent(GameWorld, inputText->firstChar, UI_ELEMENT, inputText->xOffset, inputText->yOffset, 40, 40, TEXT_CHARACTER, currentChar, inputText->font, 0, 0);
		}

		inputText->currentChar++;
	}

	
	// Finished creating text
	if (inputText->currentChar > MAX_TEXT_LENGTH || inputText->textPhrase[inputText->currentChar] == 0)
	{
		inputText->currentChar = -1;
	}

	
	return 0;
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

	MarkObjectForDeletion(GameWorld->TextQueue->firstChar, GameWorld->ObjectList);

	TextInstance *textToDelete = GameWorld->TextQueue;

	GameWorld->TextQueue = GameWorld->TextQueue->nextText;

	free(textToDelete);

	if (GameWorld->TextQueue == NULL)
	{
		GameWorld->PlayingText = 0;
	}

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
		case 13:
		return 0;

		default:
		return 32;

	}

	return 0;
}