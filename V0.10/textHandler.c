#include "LemonEngine.h"
#include "utf8Decoder.h"


void startTyping(SDL_Window *window, Text *inputTypingText)
{
	if (SDL_TextInputActive(window))
	{
		return;
	}

	SDL_StartTextInput(window);
	
	TextSettings.userInputIndex = 0;
	memset(TextSettings.userInputString, 0, USER_INPUT_MAX_LEN);

	TextSettings.cursorXPos = 0.0;

	TextSettings.Typing = true;

	TextSettings.typingText = inputTypingText;
	if (TextSettings.typingText != NULL)
	{
		updateText(TextSettings.typingText, "");
	}

	return;
}

void stopTyping(SDL_Window *window)
{
	if (!TextSettings.Typing)
	{
		return;
	}

	SDL_StopTextInput(window);

	TextSettings.Typing = false;
	TextSettings.userInputIndex = 0;
	TextSettings.cursorXPos = 0.0;
	TextSettings.typingText = NULL;

	return;
}

void updateTyping(SDL_Window *window, World *GameWorld)
{
	if (!TextSettings.Typing)
	{
		return;
	}

	if (buttonPressed(LMN_BACKSPACE) && TextSettings.userInputIndex > 0)
	{
		TextSettings.userInputIndex--;				

		char buffer[USER_INPUT_MAX_LEN] = {0};
		if (TextSettings.userInputIndex < USER_INPUT_MAX_LEN - 1)
		{
			strcpy(buffer, TextSettings.userInputString + TextSettings.userInputIndex + 1);
		}
			
		TextSettings.userInputString[TextSettings.userInputIndex] = 0;
		strcat(TextSettings.userInputString, buffer);

		if (TextSettings.typingText != NULL)
		{
			updateText(TextSettings.typingText, TextSettings.userInputString);
		}

		setCursorPos();
	}

	if (buttonPressed(LMN_LEFTARROW))
	{
		TextSettings.userInputIndex = clamp(TextSettings.userInputIndex - 1, 0, USER_INPUT_MAX_LEN);
		setCursorPos();
	}

	if (buttonPressed(LMN_RIGHTARROW))
	{
		TextSettings.userInputIndex = clamp(TextSettings.userInputIndex + 1, 0, strlen(TextSettings.userInputString));
		setCursorPos();
	}

	if (buttonPressed(LMN_TYPING_END) && TextSettings.userInputString[0] != '\0')
	{
		stopTyping(window);
	}

	
	ClearInput();

	return;
}

void inputTyping(const char input[])
{
	int prevLength = strlen(TextSettings.userInputString);
	TextSettings.userInputIndex = clamp(TextSettings.userInputIndex, 0, USER_INPUT_MAX_LEN - 1);

	if (TextSettings.userInputIndex >= USER_INPUT_MAX_LEN - 1 || prevLength >= USER_INPUT_MAX_LEN - 1 || input == NULL)
	{
		return;
	}

	char buffer[USER_INPUT_MAX_LEN] = {0};

	if (TextSettings.userInputString[TextSettings.userInputIndex] != '\0')
	{
		strcpy(buffer, TextSettings.userInputString + TextSettings.userInputIndex);
	}
	
	int bytesAvailable = USER_INPUT_MAX_LEN - prevLength - 1;

	int copiedLetters = LemonStrncpy(TextSettings.userInputString + TextSettings.userInputIndex, input, bytesAvailable);
	strcat(TextSettings.userInputString, buffer);

	TextSettings.userInputIndex += copiedLetters;

	TextSettings.userInputString[USER_INPUT_MAX_LEN - 1] = '\0';

	if (TextSettings.typingText != NULL)
	{
		updateText(TextSettings.typingText, TextSettings.userInputString);
	}

	setCursorPos();

	return;
}

void setCursorPos(void)
{
	if (TextSettings.userInputIndex < 1)
	{
		TextSettings.cursorXPos = 0.0;
		return;
	}

	TTF_Font *font = NULL;


	int width = 0;
	int height = 0;
	int wrapWidth = 0;

	Text *typingText = TextSettings.typingText;

	if (typingText == NULL)
	{
		font = TextSettings.DebugFont;
	}
	else
	{
		font = TTF_GetTextFont(typingText->text);

		TTF_GetTextWrapWidth(typingText->text, &wrapWidth);
	}
	
	if (font == NULL)
	{
		return;
	}
	
	TTF_GetStringSizeWrapped(font, TextSettings.userInputString, TextSettings.userInputIndex, wrapWidth, &width, &height);

	if (wrapWidth > 0)
	{
		int lineSkip = TTF_GetFontLineSkip(font);

		TTF_SubString lastLine = {0};
		TTF_GetTextSubStringForLine(typingText->text, (height / lineSkip) - 1, &lastLine);

		TTF_GetStringSize(font, TextSettings.userInputString + lastLine.offset, TextSettings.userInputIndex - lastLine.offset, &width, &height);
		TextSettings.cursorYPos = (float)(height - lineSkip);
	}
	else
	{
		TextSettings.cursorYPos = 0.0;
	}

	TextSettings.cursorXPos = (float)width;

    return;
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
		strcpy(inputText->voice, "Voices/Text_snd");
		memset(inputText->textBoxSprite, 0, MAX_LEN);
		break;

		case PLAINTEXT_TOP:
		inputText->currentYPos = topTextLocation;
		strcpy(inputText->voice, "Voices/Text_snd");
		memset(inputText->textBoxSprite, 0, MAX_LEN);
		break;

		case BLACK_TOP:
		strcpy(inputText->textBoxSprite, "TextBox_White");
		strcpy(inputText->font, "Pixel_Black");
		inputText->currentYPos = topTextLocation;
		strcpy(inputText->voice, "Voices/Text_snd");
		break;

		case BLACK_TEXT:
		strcpy(inputText->textBoxSprite, "TextBox_White");
		strcpy(inputText->font, "Pixel_Black");
		strcpy(inputText->voice, "Voices/Text_snd");
		break;

		case BASIC_TEXT:
		strcpy(inputText->voice, "Voices/Text_snd");
		break;

		case BASIC_TOP:
		inputText->currentYPos = topTextLocation;
		strcpy(inputText->voice, "Voices/Text_snd");
		break;

		case BASIC_FAST:
		strcpy(inputText->voice, "Voices/Text_snd");
		inputText->textDelayFrames = 1;
		break;

		case BASIC_FLIP:
		strcpy(inputText->voice, "Voices/Text_snd");
		inputText->PortraitPosition = PORTRAIT_INSIDE_BOX_RIGHT;
		break;

		case BASIC_TOP_FLIP:
		inputText->currentYPos = topTextLocation;
		strcpy(inputText->voice, "Voices/Text_snd");
		inputText->PortraitPosition = PORTRAIT_INSIDE_BOX_RIGHT;
		break;

		case BASIC_FADE:
		strcpy(inputText->voice, "Voices/Text_snd");
		strcpy(inputText->textBoxSprite, "TextBox_BasicFade");
		break;

		case BASIC_TOP_FADE:
		strcpy(inputText->voice, "Voices/Text_snd");
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
	if (GameWorld == NULL || inputPhrase == NULL)// || GameWorld->currentCutscene != NO_CUTSCENE)
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
	newText->portraitObj = NULL;
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
	newText->textReference = NULL;

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
	int index = 0;
	if (characterCount >= MAX_TEXT_LENGTH)
	{
		characterCount = MAX_TEXT_LENGTH;
	}

	while (index < characterCount)
	{
		if (input[index] == '^' || (input[index] < 32 && input[index] != '\n'))
		{
			input[index] = '\t';

			for (int i = index + 1; i < MAX_TEXT_LENGTH - 1 && inRange(input[i], '0', '9'); i++)
			{
				input[i] = '\t';
			}
		}

		if (input[index] < 32 && input[index] != '\n')
		{
			for (int i = index; i < MAX_TEXT_LENGTH - 1; i++)
			{
				input[i] = input[i + 1];
			}

			input[MAX_TEXT_LENGTH - 1] = '\0';
			characterCount--;
		}
		else
		{
			index++;
		}
	}

	return;
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
	if (buttons[LMN_TEXT_SKIP] && currentText->Skippable == true)
	{
		buttons[LMN_TEXT_CONFIRM] = -1;

		if (EXPERIMENTAL_TEXT)
		{
			removeControlCharacters(currentText->textPhrase, strlen(currentText->textPhrase));
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

		int response = displayNextCharacter(currentText, GameWorld);

		if (response != LEMON_SUCCESS)
		{
			return response;
		}
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

		inputText->textReference = NULL;
		inputText->currentXPos = inputText->boxOffsetX;
		inputText->currentYPos = inputText->boxOffsetY;
	}

	int decodedIndex = inputText->currentIndex;
	int decodedChar = utf8_decode_next(inputText->textPhrase, inputText->currentIndex, MAX_TEXT_LENGTH);
	inputText->currentIndex = utf8_setIndex();

	// Finished creating text
	if (decodedChar == 0 || inputText->currentIndex >= MAX_TEXT_LENGTH || inputText->boxPtr->State == EMPTY_OBJECT)
	{
		inputText->currentIndex = -1;

		return EXECUTION_UNNECESSARY;
	}

	switch(decodedChar)
	{
		case '\f':		// '\fN' or '^N' indicates a waiting period of N ticks
		case '^':
		{
			char count[12] = {0};
			int index = inputText->currentIndex;
			char *string = inputText->textPhrase;

			string[decodedIndex] = '\f';
			for (int i = 0; i < 11 && inRange(string[index], '0', '9'); i++)
			{
				count[i] = string[index];
				string[index] = '\f';
				index++;
			}
			
			inputText->Counter = -atoi(count);
			if (inputText->currentIndex >= 0)
			{
				inputText->currentIndex--;
			}
			
			removeControlCharacters(inputText->textPhrase, index);
			
			return EXECUTION_UNNECESSARY;
		} break;

		default:
		break;
	}

	if (EXPERIMENTAL_TEXT)
	{
		experimentalText(inputText, GameWorld);

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
		PlaySound(currentText->voice, SPEECH, 1.0);

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
		inputText->textReference = NULL;

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
				break;
			}

			i++;

			if (i >= optionData->numberOfOptions && buttonPressed(MOUSE_LEFT))
			{
				selectOption = false;
			}
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

Text* experimentalText(TextBox *input, World *GameWorld)
{
	if (input == NULL || input->boxPtr == NULL || GameWorld == NULL)
	{
		return NULL;
	}

	char subset[MAX_TEXT_LENGTH] = {0};
	LemonStrncpy(subset, input->textPhrase, input->currentIndex + 1);

	if (input->textReference != NULL)
	{
		updateText(input->textReference, subset);
		return input->textReference;
	}
	
	input->textReference = addTextToList(subset, input->currentXPos, input->currentYPos, input->textLengthSize, input->font, GameWorld);

	if (input->textReference == NULL)
	{
		return NULL;
	}

	attachTextToObject(input->textReference, input->boxPtr);
	TTF_SetTextColor(input->textReference->text, input->color.r, input->color.g, input->color.b, input->color.a);

	input->textReference->yPos += 10.0 + input->TextSize;

    return input->textReference;
}


Text* addText(const char textPhrase[], float xPos, float yPos, World *GameWorld)
{
	return addTextToList(textPhrase, xPos, yPos, 0, NULL, GameWorld);
}

Text* addTextWithFont(const char textPhrase[], float xPos, float yPos, const char *font, World *GameWorld)
{
	return addTextToList(textPhrase, xPos, yPos, 0, font, GameWorld);
}

Text* addTextWithName(const char textPhrase[], const char name[], float xPos, float yPos, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	Text* text = getTextWithName(name, GameWorld);
	if (text != NULL)
	{
		TTF_SetTextString(text->text, textPhrase, 0);
		text->xPos = xPos;
		text->yPos = yPos;
		return text;
	}

	text = addTextToList(textPhrase, xPos, yPos, 0, NULL, GameWorld);

	setTextName(text, name);

	return text;
}

Text* getTextWithName(const char name[], World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	Text *list = GameWorld->TextList.texts;

	for (int i = 0; i < MAX_TEXT_TEXTURES; i++)
	{
		if (list[i].beingUsed && strcmp(name, list[i].name) == 0)
		{
			return &list[i];
		}
	}

	return NULL;
}

Text* addTextToList(const char textPhrase[], float xPos, float yPos, int wrapWidth, const char *desiredFont, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	Text *TextArray = GameWorld->TextList.texts;

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
		return NULL;
	}
	

	// get or create new font
	TTF_Font *renderFont;

	if (desiredFont == NULL || strlen(desiredFont) < 1)
	{
		renderFont = loadFont(TextSettings.defaultFont, "DefaultFont", GameWorld);
	}
	else
	{
		renderFont = loadFont(desiredFont, desiredFont, GameWorld);
	}

	if (renderFont == NULL)
	{
		return NULL;
	}

	newText->xPos = xPos;
    newText->yPos = yPos;

    newText->CameraRelative = false;
    newText->beingUsed = true;
    newText->attachedObj = NULL;
    newText->textPos = TEXT_BOTTOM_LEFT;
    memset(newText->name, 0, MAX_LEN);

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

    GameWorld->TextList.count++;

    return newText;
}	


void updateText(Text *input, const char newPhrase[])
{
	if (newPhrase == NULL || input == NULL)
	{
		return;
	}

	if (input->text != NULL && input->beingUsed)
	{
		TTF_SetTextString(input->text, newPhrase, 0);
	}

	return;
}

void updateTextWithName(const char name[], const char newPhrase[], World *GameWorld)
{
	if (newPhrase == NULL || name == NULL || GameWorld == NULL)
	{
		return;
	}

	Text *list = GameWorld->TextList.texts;

	for (int i = 0; i < MAX_TEXT_TEXTURES; i++)
	{
		if (strcmp(name, list[i].name) == 0)
		{
			TTF_SetTextString(list[i].text, newPhrase, 0);
			return;
		}
	}

	return;
}

void attachTextToObject(Text *input, Object *obj)
{
	if (input == NULL || obj == NULL)
	{
		return;
	}

	input->attachedObj = obj;
	input->recordedInstance = obj->instanceNumber;

	return;
}

void attachTextWithNameToObject(const char name[], Object *input, World *GameWorld)
{
	if (input == NULL || name == NULL || GameWorld == NULL)
	{
		return;
	}

	Text *list = GameWorld->TextList.texts;

	for (int i = 0; i < MAX_TEXT_TEXTURES; i++)
	{
		if (strcmp(name, list[i].name) == 0)
		{
			list[i].attachedObj = input;
			list[i].recordedInstance = input->instanceNumber;
			return;
		}
	}

	return;
}

void moveText(Text *input, float xPos, float yPos)
{
	if (input == NULL)
	{
		return;
	}

	if (input->text != NULL)
	{
		input->xPos = xPos;
		input->yPos = yPos;
	}

	return;
}

void moveTextWithName(const char name[], float xPos, float yPos, World *GameWorld)
{
	if (name == NULL || GameWorld == NULL)
	{
		return;
	}

	Text *list = GameWorld->TextList.texts;

	for (int i = 0; i < MAX_TEXT_TEXTURES; i++)
	{
		if (list[i].text != NULL && strcmp(name, list[i].name) == 0)
		{
			list[i].xPos = xPos;
			list[i].yPos = yPos;
			return;
		}
	}

	return;
}


void setTextColour(Text *input, SDL_Color *colour)
{
	if (input == NULL)
	{
		return;
	}

	if (input->text != NULL)
	{
		TTF_SetTextColor(input->text, colour->r, colour->g, colour->b, colour->a);
	}
	
	return;
}

void setTextColourWithName(const char name[], SDL_Color *colour, World *GameWorld)
{
	if (name == NULL || GameWorld == NULL)
	{
		return;
	}

	Text *list = GameWorld->TextList.texts;

	for (int i = 0; i < MAX_TEXT_TEXTURES; i++)
	{
		if (list[i].text != NULL && strcmp(name, list[i].name) == 0)
		{
			TTF_SetTextColor(list[i].text, colour->r, colour->g, colour->b, colour->a);
			return;
		}
	}

	return;
}

void setTextName(Text *input, const char name[])
{
	if (input == NULL)
	{
		return;
	}

	if (input->text != NULL)
	{
		LemonStrncpy(input->name, name, TEXT_NAME_MAX_LEN);
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
		input->texts[i].attachedObj = NULL;
	}
}

void printTextsinfo(TextList *list, const char name[])
{
	putConsole("\n%s: ", name);
	char buffer[MAX_LEN + TEXT_NAME_MAX_LEN + OBJECT_NAME_LENGTH] = {0};

	Text *array = list->texts;

	for (int i = 0; i < MAX_TEXT_TEXTURES; i++)
	{
		if (array[i].name[0] != '\0')
		{
			snprintf(buffer, TEXT_NAME_MAX_LEN + 2, "'%s'", array[i].name);
		}
		else
		{
			strcpy(buffer, "(No name)");
		}

		
		if (array[i].text != NULL)
		{
			strcat(buffer, " (Data loaded)  ");
		}
		else
		{
			strcat(buffer, " (Data empty)   ");
		}

		if (array[i].beingUsed)
		{
			strcat(buffer, "(Being used)   ");
		}
		else
		{
			strcat(buffer, "(Unused)   ");
		}

		if (array[i].CameraRelative)
		{
			strcat(buffer, "(Camera relative)   ");
		}
		else
		{
			strcat(buffer, "(Screen relative)   ");
		}

		if (array[i].attachedObj != NULL)
		{
			strcat(buffer, "(Connected to object '");
			strcat(buffer, array[i].attachedObj->name);
			strcat(buffer, "')");
		}
		else
		{
			strcat(buffer, "(independent)");
		}

		putConsole("Index: %d  %s", i, buffer);
	}

	return;
}

int RemoveText(Text *input, World *GameWorld)
{
	if (input == NULL || GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (!input->beingUsed)
	{
		return EXECUTION_UNNECESSARY;
	}

	input->beingUsed = false;
	input->attachedObj = NULL;
	GameWorld->TextList.count--;

	return LEMON_SUCCESS;
}

int RemoveTextWithName(const char name[], World *GameWorld)
{
	return RemoveText(getTextWithName(name, GameWorld), GameWorld);
}


void RemoveAllTexts(TextList *list)
{
	int i = 0;
	while (i < MAX_TEXT_TEXTURES && list->count > 0)
	{
		if (list->texts[i].beingUsed)
		{
			list->texts[i].beingUsed = false;
			list->texts[i].attachedObj = NULL;
			list->count--;
		}

		i++;
	}

	list->count = 0;

	return;
}

void RemoveUnnamedTexts(TextList *list)
{
	int i = 0;
	while (i < MAX_TEXT_TEXTURES && list->count > 0)
	{
		if (list->texts[i].beingUsed && list->texts[i].name[0] == '\0')
		{
			list->texts[i].beingUsed = false;
			list->texts[i].attachedObj = NULL;
			list->count--;
		}

		i++;
	}

	return;
}

void removeAttachedTexts(Object *input, World *GameWorld)
{
	if (input == NULL || GameWorld == NULL || !EXPERIMENTAL_TEXT)
	{
		return;
	}

	TextList *list = &GameWorld->TextList;

	if (list->count < 1)
	{
		return;
	}

	Text *array = list->texts;

	for (int i = 0; i < MAX_TEXT_TEXTURES; i++)
	{
		if (array[i].attachedObj == input)
		{
			RemoveText(&array[i], GameWorld);
		}
	}

	return;
}


void closeFont(TTF_Font *font, TextList *list)
{
	if (list == NULL)	// it's assumed here that no provided text list means there are no texts that use this font
	{
		TTF_CloseFont(font);
		return;
	}

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

TTF_Font* loadFont(const char *desiredFont, const char *newName, World *GameWorld)
{	
	if (GameWorld == NULL)
	{
		return NULL;
	}

	TTF_Font *newFont = getFont(newName, GameWorld);
	if (newFont != NULL)
	{
		return newFont;
	}

	FontList *list = &GameWorld->FontList;
	TextList *texts = &GameWorld->TextList;

	int head = list->head % MAX_LOADED_FONTS;
	
	char fontName[MAX_LEN] = FONT_ROOT;
	strcat(fontName, desiredFont);

	if (fontName[strlen(fontName) - 4] != '.')
	{
		strcat(fontName, ".ttf");
	}
	
	newFont = TTF_OpenFont(fontName, TextSettings.defaultTextPointSize);

	if (newFont == NULL)
	{ 
    	putConsole("\nFailed to load font! (%s)\n", SDL_GetError());
    	return NULL;
	}

	if (list->fonts[head] != NULL)
	{
		closeFont(list->fonts[head], texts);
		list->fonts[head] = NULL;
	}

	strcpy(list->names[head], newName);
	list->fonts[head] = newFont;

	list->head = (head + 1) % MAX_LOADED_FONTS;

	return newFont;
}

TTF_Font* loadFontWithSize(const char *desiredFont, const char *newName, float pointSize, World *GameWorld)
{
	TTF_Font *font = loadFont(desiredFont, newName, GameWorld);

	if (font != NULL)
	{
		TTF_SetFontSize(font, pointSize);
	}

	return font;
}

TTF_Font* getFont(const char *name, World *GameWorld)
{
	if (GameWorld == NULL || strcmp(name, "DebugFont") == 0)
	{
		return TextSettings.DebugFont;
	}

	FontList *list = &GameWorld->FontList;

	for (int i = 0; i < MAX_LOADED_FONTS; i++)
	{
		if (strcmp(list->names[i], name) == 0 && list->fonts[i] != NULL)
		{
			return list->fonts[i];
		}
	}

	return NULL;
}

void setFontSize(const char *name, int size, World *GameWorld)
{
	TTF_Font *font = getFont(name, GameWorld);
	
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
	memset(input->names, 0, MAX_LOADED_FONTS * FONT_FILE_NAME_MAX);
	for (int i = 0; i < MAX_LOADED_FONTS; i++)
	{
		input->fonts[i] = NULL;
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

void cleanUpFonts(FontList *input)
{
	if (input == NULL)
	{
		return;
	}

	for (int i = 0; i < MAX_LOADED_FONTS; i++)
	{
		if (input->fonts[i] != NULL)
		{
			TTF_CloseFont(input->fonts[i]);
			input->fonts[i] = NULL;
		}

		input->names[i][0] = '\0';
	}

	input->head = 0;

	return;
}

void cleanUpTextData(RenderFrame *ScreenData)
{
	cleanUpTexts(&TextSettings.DebugTextList);

	if (TextSettings.DebugFont != NULL)
	{
		TTF_CloseFont(TextSettings.DebugFont);
		TextSettings.DebugFont = NULL;
	}

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

	inputText->portraitObj = portrait;

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


	deleteTextBox(text, GameWorld);


	return LEMON_SUCCESS;
}


int deleteTextBox(TextBox *input, World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL || input == NULL)
	{
		return MISSING_DATA;
	}

	if (input == GameWorld->TextQueue)
	{
		GameWorld->TextQueue = input->nextText;
	}
	else
	{
		TextBox *prev = GameWorld->TextQueue;

		while (prev->nextText != input && prev->nextText != NULL)
		{
			prev = prev->nextText;
		}

		if (prev->nextText == input)
		{
			prev->nextText = input->nextText;
		}
	}

	switch (input->textTypeSetting)
	{
		case TEXTBOX_OPTION_PROMPT:
		{
			struct TextOptionPrompt *optionData = &input->textTypeData.OptionPrompt;

			for (int i = 0; i < optionData->numberOfOptions; i++)
			{
				cleanUpGameEventArgs(&optionData->optionTriggers[i]);
			}
		} break;

		case TEXTBOX_TRIGGER_EVENT:
		{
			cleanUpGameEventArgs(&input->textTypeData.TriggerEvent);
		} break;

		default:
		break;	
	}

	MarkObjectForDeletion(input->boxPtr);

	removeAttachedTexts(input->boxPtr, GameWorld);	// technically unnecessary, as deleting the object also deletes text references

	DeleteTextSceneAction(input, GameWorld);

	free(input);

	return LEMON_SUCCESS;
}


int clearTextQueue(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->TextQueue == NULL) { return MISSING_DATA; }

	int i = 0;

	while (i < EngineSettings.MaxTextQueueLength && GameWorld->TextQueue != NULL)
	{
		deleteTextBox(GameWorld->TextQueue, GameWorld);
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

	SceneAction *currentAction = GameWorld->nextSceneAction;

	while (currentAction != NULL)
	{
		if (currentAction->ActionID == SCENE_SAY_TEXT && currentAction->ActionData.sceneText == inputText)
		{
			return true;
		}

		if (currentAction->parallelAction == false || currentAction->ActionID == SCENE_SAY_TEXT)
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
			GameWorld->nextSceneAction = currentAction->nextSceneAction;

			if (GameWorld->nextSceneAction == NULL)
			{
				deleteAllSceneActions(GameWorld);
			}
			return;
		}
		else
		{
			currentAction = currentAction->nextSceneAction;
		}
	}	

	return;
}