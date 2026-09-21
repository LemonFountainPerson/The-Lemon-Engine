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
		font = TextSettings.DebugFont.font;
	}
	else
	{
		font = typingText->usedFont->font;

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


int InitialiseUIText(Object *UIText, World *GameWorld)
{
	if (GameWorld == NULL || UIText == NULL)
	{
		return MISSING_DATA;
	}

	setDisplayLayer(UIText, HUD);
	UIText->ObjectBox->collideLayer = HUD;
	UIText->ObjectBox->solid = UNSOLID;
	UIText->reserved |= RFLAG_CUTSCENE_IMMUNITY; // mark this object as immune to being frozen during cutscenes

	switch(getSubType(UIText))
	{
	case TEXT_OPTION_CURSOR:
			UIText->arg2 = -1;
			initialiseTextCharacter(UIText, '>', GameWorld);
			switchSpriteByName("TextCursor", 0, UIText->ObjectDisplay);

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
	if (GameWorld == NULL || UIText == NULL)
	{
		return MISSING_DATA;
	}


	switch (getSubType(UIText))
	{
	case TEXT_OPTION_CURSOR:
	{
		TextBox *currentText = getCurrentTextBox(GameWorld);

		if (currentText == NULL || currentText->boxPtr == NULL || currentText->textTypeSetting != TEXTBOX_OPTION_PROMPT)
		{
			MarkObjectForDeletion(UIText);
			break;
		}

		TextOptionPrompt *optionPrompt = &currentText->textTypeData.OptionPrompt;
		int option = optionPrompt->SelectedOption;

		if (optionPrompt != NULL && UIText->arg2 != option)
		{
			//float sizeRatio = (TextSettings.defaultTextPointSize / DEFAULT_TEXT_SIZE);
			UIText->ObjectBox->xPos = (currentText->boxPtr->ObjectBox->xPos + currentText->boxOffsetX);
			UIText->ObjectBox->yPos = (currentText->boxPtr->ObjectBox->yPos + optionPrompt->OptionYPositions[option]);
			UIText->arg2 = option;
			playTextVoice(currentText);
		}
	} break;

	default:
		break;
	}

	return LEMON_SUCCESS;
}



TextBox* SayText(const char inputPhrase[], const char Portrait[], TextPreset preset, World *GameWorld)
{
	if (GameWorld == NULL || inputPhrase == NULL)
	{
		return NULL;
	}

	SceneAction *newAction = createSceneAction(SCENE_SAY_TEXT, GameWorld);

	if (newAction == NULL)
	{
		return NULL;
	}

	newAction->parallelAction = false;

	// get Text Box within scene action
	TextBox *newText = &newAction->ActionData.sceneText;

	memset(newText->textPhrase, 0, MAX_TEXT_LENGTH);
	LemonStrncpy(newText->textPhrase, inputPhrase, MAX_TEXT_LENGTH);

	memset(newText->Portrait, 0, MAX_LEN);
	if (Portrait != NULL)
	{
		LemonStrncpy(newText->Portrait, Portrait, MAX_LEN);
	}
	
	memset(newText->voice, 0, MAX_LEN);

	newText->boxPtr = NULL;
	memset(&newText->textTypeData, 0, sizeof(TextTypeData));
	newText->textTypeSetting = TEXTBOX_REGULAR_TEXT;

	newText->preset = preset;

	newText->boxStartPosX = 0;
	newText->boxStartPosY = 0;
	newText->currentXPos = 0;
	newText->currentYPos = 0;
	newText->boxOffsetX = 0;
	newText->boxOffsetY = 0;
	newText->textLengthSize = 0;
	newText->currentIndex = 0;
	newText->Counter = 0;
	newText->textDelayFrames = 0;
	newText->LineSpacing = 50;
	newText->TextSize = TextSettings.defaultTextPointSize;	

	memset(newText->font, 0, FONT_FILE_NAME_MAX);
	newText->PortraitPosition = PORTRAIT_INSIDE_BOX_LEFT;
	newText->Skippable = true;
	newText->voiceMode = VOICE_EACH_CHARACTER;

	newText->color.r = 255;
	newText->color.b = 255;
	newText->color.g = 255;
	newText->color.a = 255;
	newText->defaultColor = newText->color;

	return newText;
}


void disableProvidedGameEvents(va_list *args, int numberOfOptions)
{
	GameEvent *eventTrigger;

	for (int i = 0; i < numberOfOptions; i++)
    {
    	va_arg(*args, char*);

    	eventTrigger = va_arg(*args, GameEvent*);

    	if (eventTrigger != NULL)
    	{
			eventTrigger->EventID = NO_EVENT;	
	    }
    }

    return;
}

TextBox* SayTextOption(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld, int numberOfOptions, ...)
{
	if (inputPhrase == NULL || numberOfOptions < 1 || numberOfOptions > MAX_TEXT_OPTIONS)
	{
		return NULL;
	}

	TextBox *newText = SayText(inputPhrase, Portrait, inputPreset, GameWorld);

	va_list args;
    va_start(args, numberOfOptions);

    if (newText == NULL)
	{
		disableProvidedGameEvents(&args, numberOfOptions);

    	return NULL;
    }

    GameEvent *eventTrigger;
    TextOptionPrompt optionData = {0};	// temp location for arguments
    optionData.numberOfOptions = numberOfOptions;
    optionData.optionTriggers = malloc(sizeof(GameEvent) * numberOfOptions);

    if (optionData.optionTriggers == NULL)
	{
		disableProvidedGameEvents(&args, numberOfOptions);

    	return NULL;
    }

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
	    	memset(&optionData.optionTriggers[i], 0, sizeof(GameEvent));
	    	optionData.optionTriggers[i].EventID = NO_EVENT;
	    }
    }

    va_end(args);


	newText->textTypeSetting = TEXTBOX_OPTION_PROMPT;
	optionData.setUpComplete = false;	// initialising to 0 already sets this, but just in case

	memcpy(&newText->textTypeData.OptionPrompt, &optionData, sizeof(TextOptionPrompt));
	

	return newText;
}


int SayTextAndTriggerEvent(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld, GameEvent *inputEvent)
{
	if (inputEvent == NULL || GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	TextBox *newText = SayText(inputPhrase, Portrait, inputPreset, GameWorld);

	if (newText == NULL)
	{
		return LEMON_ERROR;
	}

	newText->textTypeData.TriggerEvent = malloc(sizeof(GameEvent));
	if (newText->textTypeData.TriggerEvent == NULL)
	{
		inputEvent->EventID = NO_EVENT;
		return LEMON_ERROR;
	}

	newText->textTypeSetting = TEXTBOX_TRIGGER_EVENT;

	removeEventToTriggerLater(inputEvent, newText->textTypeData.TriggerEvent, GameWorld);

	return LEMON_SUCCESS;
}


int ApplyTextPresets(TextBox *inputText, World *GameWorld)
{
	if (inputText == NULL)
	{
		return MISSING_DATA;
	}

	int topTextLocation = (ScreenData.HUDHeight >> 1) - 360;
	int bottomTextLocation = 40 - (ScreenData.HUDHeight >> 1);
	
	// Default settings
	inputText->boxStartPosX = -600;
	inputText->boxStartPosY = bottomTextLocation;

	inputText->boxOffsetX = 30;
	inputText->boxOffsetY = 240;
	inputText->textLengthSize = 1150;
	inputText->textDelayFrames = 3;

	inputText->TextSize = TextSettings.defaultTextPointSize;

	char TextBoxSprite[MAX_LEN] = "TextBox_BasicFade";

	if (EXPERIMENTAL_TEXT)
	{
		strcpy(inputText->font, DEFAULT_FONT);
	}
	else
	{
		strcpy(inputText->font, "Pixel_White");
	}


	switch (inputText->preset)
	{
		case BLUE_FADE:
		inputText->color.r = 140;
		inputText->color.g = 216;
		inputText->color.b = 255;
		break;

		case COMIC_TEXT:
		strcpy(inputText->font, "ComicSans");
		break;

		case COMIC_TOP:
		strcpy(inputText->font, "ComicSans");
		inputText->currentYPos = topTextLocation;
		break;

		case PLAINTEXT_BOTTOM:
		strcpy(inputText->voice, "Voices/Text_snd");
		memset(TextBoxSprite, 0, MAX_LEN);
		break;

		case PLAINTEXT_TOP:
		inputText->currentYPos = topTextLocation;
		strcpy(inputText->voice, "Voices/Text_snd");
		memset(TextBoxSprite, 0, MAX_LEN);
		break;

		case BLACK_TOP:
		strcpy(TextBoxSprite, "TextBox_White");
		inputText->currentYPos = topTextLocation;
		inputText->color.r = 0;
		inputText->color.g = 0;
		inputText->color.b = 0;
		strcpy(inputText->voice, "Voices/Text_snd");
		break;

		case BLACK_TEXT:
		strcpy(TextBoxSprite, "TextBox_White");
		inputText->color.r = 0;
		inputText->color.g = 0;
		inputText->color.b = 0;
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

		case SILENT_TOP:
		inputText->currentYPos = topTextLocation;
		break;

		default:
		break;
	}

	inputText->defaultColor = inputText->color;
	inputText->LineSpacing = (inputText->TextSize + SPACE_BETWEEN_TEXT);
	createTextBoxSprite(inputText, TextBoxSprite, GameWorld);

	if (EXPERIMENTAL_TEXT)
	{
		loadFontWithSize(inputText->font, inputText->font, inputText->TextSize * ((float)ScreenData.screenHeight / (float)ScreenData.HUDHeight), GameWorld);
	}

	return LEMON_SUCCESS;
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

void removeControlCharacters(TextBox *input, int characterCount)
{
	const char *string = input->textPhrase;
	int index = 0;
	if (characterCount >= MAX_TEXT_LENGTH)
	{
		characterCount = MAX_TEXT_LENGTH;
	}

	while (index < characterCount)
	{
		for (int i = index; i < MAX_TEXT_LENGTH - 1 && string[i] != '\n' && string[i] < 32; i++)
		{
			index++;
			input->currentIndex++;
		}

		index++;
	}

	return;
}


// Play the text at the first slot in the linked list, then delete and shift everything up when done with that textbox
int displayText(TextBox *currentText, World *GameWorld)
{
	if (currentText == NULL)
	{
		return MISSING_DATA;
	}

	int response = LEMON_SUCCESS;

	// Skip text animation if skip button is held
	if (buttons[LMN_TEXT_SKIP] && currentText->Skippable == true)
	{
		buttons[LMN_TEXT_CONFIRM] = -1;

		while (currentText->currentIndex >= 0 && response == LEMON_SUCCESS)
		{
			response = displayNextCharacter(currentText, GameWorld);
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

		response = displayNextCharacter(currentText, GameWorld);
	}
	
	if (response != LEMON_SUCCESS)
	{
		return response;
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
		GameWorld->TextBox = true;

		ApplyTextPresets(inputText, GameWorld);

		if (inputText->boxPtr == NULL)
		{
			endTextBox(inputText, GameWorld);
			return LEMON_ERROR;
		}

		// Create portrait
		if (inputText->Portrait[0] > 32 && strcmp(inputText->Portrait, "NO_PORTRAIT"))
		{
			createTextBoxPortrait(inputText, GameWorld);
		}

		insertLineBreaks(inputText->textPhrase, inputText->textLengthSize - (int)inputText->TextSize);

		inputText->currentXPos = inputText->boxOffsetX;
		inputText->currentYPos = inputText->boxOffsetY;
	}

	int decodedIndex = inputText->currentIndex;
	int decodedChar = utf8_decode_next(inputText->textPhrase, decodedIndex, MAX_TEXT_LENGTH);
	inputText->currentIndex = utf8_setIndex();

	// Finished creating text
	if (decodedChar == '\0' || inputText->currentIndex >= MAX_TEXT_LENGTH || inputText->boxPtr->State == EMPTY_OBJECT)
	{
		inputText->currentIndex = -1;

		return EXECUTION_UNNECESSARY;
	}

	switch(decodedChar)
	{
		// '^N' indicates a waiting period of N ticks
		case '^':
		{
			char count[5] = {0};
			const char *string = inputText->textPhrase;

			decodedIndex++;
			for (int i = 0; decodedIndex < MAX_TEXT_LENGTH && i < 4 && inRange(string[decodedIndex], '0', '9'); i++)
			{
				count[i] = string[decodedIndex];
				decodedIndex++;
			}
			
			inputText->currentIndex = decodedIndex;
			inputText->Counter = -atoi(count);
			
			return EXECUTION_UNNECESSARY;
		} break;

		case '<':
		{
			setTextBoxColor(inputText, &decodedIndex);
		} break;

		case '.':
		case '?':
		{
			inputText->Counter = -15;
		} break;

		case ':':
		case ';':
		case ',':
		case '!':
		{
			inputText->Counter = -10;
		} break;

		default:
		break;
	}

	// Spawn next character		
	if (inputText->currentXPos - inputText->boxOffsetX + (int)inputText->TextSize > inputText->textLengthSize || decodedChar == '\n')
	{
		inputText->currentXPos = inputText->boxOffsetX;
		inputText->currentYPos -= inputText->LineSpacing;
	}

	if (EXPERIMENTAL_TEXT)
	{
		placeTextCharacter(inputText, decodedIndex, GameWorld);
	}
	else
	{
		int newTextXPos = inputText->currentXPos + (int)inputText->boxPtr->ObjectBox->xPos;
		int newTextYPos = inputText->currentYPos + (int)inputText->boxPtr->ObjectBox->yPos;
		AddObject(GameWorld, UI_TEXT, newTextXPos, newTextYPos, TEXT_CHARACTER, decodedChar, 0, 0, 0);

		inputText->currentXPos += getCharacterSpacing(decodedChar);
	}
	
	
	return LEMON_SUCCESS;
}


void setTextBoxColor(TextBox *inputText, int *decodedIndex)
{
	int index = *decodedIndex + 1;
	const char *string = inputText->textPhrase;

	if (index >= MAX_TEXT_LENGTH || !inRange(string[index], 'A', 'Z'))
	{
		return;
	}

	char color[16] = {0};

	int i = 0;
	while (index < MAX_TEXT_LENGTH && string[index] != '>' && i < 15)
	{
		color[i] = string[index];
		index++;
		i++;
	}

	if (index < MAX_TEXT_LENGTH && string[index] == '>')
	{
		index++;
	}

	if (EXPERIMENTAL_TEXT)
	{
		if (strcmp(color, "DEFAULT") == 0 || strcmp(color, "DEF") == 0)
		{
			inputText->color = inputText->defaultColor;
		}
		else if (strcmp(color, "PINK") == 0 || strcmp(color, "PNK") == 0)
		{
			inputText->color.r = 255;
			inputText->color.g = 130;
			inputText->color.b = 130;
		}
		else if (strcmp(color, "WHITE") == 0 || strcmp(color, "WHT") == 0)
		{
			inputText->color.r = 255;
			inputText->color.g = 255;
			inputText->color.b = 255;
		}
		else if (strcmp(color, "BLACK") == 0 || strcmp(color, "BLK") == 0)
		{
			inputText->color.r = 0;
			inputText->color.g = 0;
			inputText->color.b = 0;
		}
		else if (strcmp(color, "RED") == 0)
		{
			inputText->color.r = 255;
			inputText->color.g = 64;
			inputText->color.b = 64;
		}
		else if (strcmp(color, "GREEN") == 0 || strcmp(color, "GRN") == 0)
		{
			inputText->color.r = 30;
			inputText->color.g = 255;
			inputText->color.b = 30;
		}
		else if (strcmp(color, "BLUE") == 0 || strcmp(color, "BLU") == 0)
		{
			inputText->color.r = 30;
			inputText->color.g = 144;
			inputText->color.b = 255;
		}
		else if (strcmp(color, "ORANGE") == 0 || strcmp(color, "ORA") == 0)
		{
			inputText->color.r = 255;
			inputText->color.g = 165;
			inputText->color.b = 0;
		}
		else if (strcmp(color, "YELLOW") == 0 || strcmp(color, "YLW") == 0)
		{
			inputText->color.r = 255;
			inputText->color.g = 255;
			inputText->color.b = 42;
		}
		else if (strcmp(color, "CYAN") == 0 || strcmp(color, "CYA") == 0)
		{
			inputText->color.r = 0;
			inputText->color.g = 255;
			inputText->color.b = 255;
		}
		else if (strcmp(color, "PURPLE") == 0 || strcmp(color, "PUR") == 0)
		{
			inputText->color.r = 200;
			inputText->color.g = 32;
			inputText->color.b = 200;
		}
		else if (strcmp(color, "VIOLET") == 0 || strcmp(color, "VIO") == 0)
		{
			inputText->color.r = 127;
			inputText->color.g = 0;
			inputText->color.b = 255;
		}
		else
		{
			return;
		}
	}

	*decodedIndex = index;
	utf8_decode_next(inputText->textPhrase, index, MAX_TEXT_LENGTH);
	inputText->currentIndex = utf8_setIndex();

	return;
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
				endTextBox(currentText, GameWorld);
			}
		break;
	}


	return LEMON_SUCCESS;
}

int handleOptionPrompt(TextBox *inputText, World *GameWorld)
{
	TextOptionPrompt *optionData = &inputText->textTypeData.OptionPrompt;

	if (!optionData->setUpComplete)
	{
		if (strlen(inputText->textPhrase) > 0)
		{
			inputText->currentYPos -= inputText->LineSpacing;
		}

		if (optionData->SelectedOption > 0)
		{
			swapStrings(inputText->textPhrase, optionData->optionNames[optionData->SelectedOption - 1], OPTION_TEXT_MAX_LEN);
		}

		swapStrings(inputText->textPhrase, optionData->optionNames[optionData->SelectedOption], OPTION_TEXT_MAX_LEN);

		inputText->currentIndex = 0;

		inputText->currentXPos = inputText->boxOffsetX + (inputText->TextSize + SPACE_BETWEEN_TEXT);
		optionData->OptionYPositions[optionData->SelectedOption] = inputText->currentYPos;
		optionData->SelectedOption++;

		if (optionData->SelectedOption >= optionData->numberOfOptions)
		{
			optionData->setUpComplete = true;
			optionData->SelectedOption = 0;
		}

		return LEMON_SUCCESS;
	}
	else if (inputText->currentIndex == -1)
	{
		AddObjectWithParent(GameWorld, inputText->boxPtr, UI_TEXT, 0, 0, TEXT_OPTION_CURSOR, 0, 0, 0, 0);
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
		endTextBox(inputText, GameWorld);
	}

	return LEMON_SUCCESS;
}

Text* placeTextCharacter(TextBox *input, int decodedIndex, World *GameWorld)
{
	if (input == NULL || input->boxPtr == NULL || GameWorld == NULL)
	{
		return NULL;
	}

	float yRatio = (float)ScreenData.screenHeight / (float)ScreenData.HUDHeight;
	char subset[MAX_TEXT_LENGTH] = {0};
	int length = clamp(input->currentIndex - decodedIndex, 1, 8);
	memcpy(subset, input->textPhrase + decodedIndex, length);

	if (subset[0] < 32)
	{
		return NULL;
	}
	
	Text *newText = addTextWithFont(subset, input->currentXPos * yRatio, input->currentYPos * yRatio, input->font, GameWorld);

	if (newText == NULL)
	{
		return NULL;
	}

	// move position for next character
	int width = 0;
	TTF_GetStringSize(TTF_GetTextFont(newText->text), subset, 0, &width, NULL);
	input->currentXPos += width / yRatio;

	if (subset[0] == ' ')
	{
		RemoveText(newText, GameWorld);
		return NULL;
	}

	attachTextToObject(newText, input->boxPtr);
	setTextColour(newText, input->color);

	newText->yPos += 10.0 + (input->TextSize * yRatio);

    return newText;
}


Text* addText(const char textPhrase[], float xPos, float yPos, World *GameWorld)
{
	return addTextWithFont(textPhrase, xPos, yPos, NULL, GameWorld);
}

Text* addTextWithName(const char textPhrase[], const char name[], float xPos, float yPos, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	// If this name has already been used, do not allow it to be used again
	Text* text = getTextWithName(name, GameWorld);
	if (text != NULL)	
	{
		return NULL;
	}

	text = addTextWithFont(textPhrase, xPos, yPos, NULL, GameWorld);

	setTextName(text, name);

	return text;
}

Text* getTextWithName(const char name[], World *GameWorld)
{
	if (GameWorld == NULL || name[0] == '\0')
	{
		return NULL;
	}

	Text *list = GameWorld->TextList.texts;

	for (int i = 0; i < MAX_TEXTS; i++)
	{
		if (list[i].text != NULL && list[i].name[0] != '\0' && strcmp(name, list[i].name) == 0)
		{
			return &list[i];
		}
	}

	return NULL;
}

Text* addTextWithFont(const char textPhrase[], float xPos, float yPos, const char *desiredFont, World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}

	Text *TextArray = GameWorld->TextList.texts;

	Text *newText = NULL;

	//  find an available slot
	int index = 0;
	while (index < MAX_TEXTS && newText == NULL)
	{
		if (TextArray[index].text == NULL)
		{
			newText = &TextArray[index];
		}

		index++;
	}

	if (newText == NULL)
	{
		return NULL;
	}
	

	// get or create new font
	Font *renderFont;

	if (desiredFont == NULL || desiredFont[0] == '\0')
	{
		renderFont = loadFont(TextSettings.defaultFont, "DefaultFont", GameWorld);
		if (renderFont != NULL)
		{
			renderFont->deleteWhenUnused = false;
		}
	}
	else
	{
		renderFont = loadFont(desiredFont, desiredFont, GameWorld);
	}

	if (renderFont == NULL)
	{
		return NULL;
	}

    if (newText->text == NULL)
    {
    	newText->text = TTF_CreateText(ScreenData.textEngine, renderFont->font, textPhrase, 0);
    }
	else // text object already present, no need to delete, just reset font and text
	{
    	TTF_SetTextFont(newText->text, renderFont->font);
    	TTF_SetTextString(newText->text, textPhrase, 0);
    	TTF_SetTextColor(newText->text, 255, 255, 255, 255);	// default colour is white
    	TTF_SetTextWrapWidth(newText->text, 0);
    }

    if (newText->text == NULL)
    {
    	return NULL;
    }

    newText->xPos = xPos;
    newText->yPos = yPos;

    newText->CameraRelative = false;
    newText->attachedObj = NULL;
    newText->usedFont = renderFont;
    memset(newText->name, 0, MAX_LEN);

    GameWorld->TextList.count++;

    return newText;
}	


void updateText(Text *input, const char newPhrase[])
{
	if (newPhrase == NULL || input == NULL)
	{
		return;
	}

	if (input->text != NULL)
	{
		TTF_SetTextString(input->text, newPhrase, 0);
	}

	return;
}

void updateTextWithName(const char name[], const char newPhrase[], World *GameWorld)
{
	if (newPhrase == NULL)
	{
		return;
	}

	Text *found = getTextWithName(name, GameWorld);

	if (found == NULL)
	{
		return;
	}

	TTF_SetTextString(found->text, newPhrase, 0);
	
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

	for (int i = 0; i < MAX_TEXTS; i++)
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
	Text *found = getTextWithName(name, GameWorld);

	if (found == NULL)
	{
		return;
	}

	found->xPos = xPos;
	found->yPos = yPos;

	return;
}


void setTextColour(Text *input, SDL_Color colour)
{
	if (input == NULL || input->text == NULL)
	{
		return;
	}

	TTF_SetTextColor(input->text, colour.r, colour.g, colour.b, colour.a);
	
	return;
}

void setTextColourWithName(const char name[], SDL_Color colour, World *GameWorld)
{
	Text *found = getTextWithName(name, GameWorld);

	if (found == NULL)
	{
		return;
	}

	TTF_SetTextColor(found->text, colour.r, colour.g, colour.b, colour.a);			

	return;
}

void setTextTransparency(Text *input, float transparency)
{
	if (input == NULL || input->text == NULL)
	{
		return;
	}

	SDL_Color color = {0};
	TTF_GetTextColor(input->text, &color.r, &color.g, &color.b, &color.a);
	color.a = (Uint8)((1.0 - fClamp(transparency, 0.0, 1.0)) * 255.0);

	TTF_SetTextColor(input->text, color.r, color.g, color.b, color.a);
	
	return;
}

void setTextWithNameTransparency(const char name[], float transparency, World *GameWorld)
{
	Text *found = getTextWithName(name, GameWorld);

	setTextTransparency(found, transparency);

	return;
}

void setTextWrapWidth(Text *input, int wrapWidth)
{
	if (input == NULL || input->text == NULL)
	{
		return;
	}

	TTF_SetTextWrapWidth(input->text, wrapWidth);
	
	return;
}

void setTextName(Text *input, const char name[])
{
	if (input != NULL && input->text != NULL)
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
	for (int i = 0; i < MAX_TEXTS; i++)
	{
		input->texts[i].text = NULL;
		input->texts[i].attachedObj = NULL;
	}
}

void printTextListinfo(TextList *list, const char name[])
{
	putConsole("\n%s: ", name);
	char buffer[MAX_LEN + TEXT_NAME_MAX_LEN + OBJECT_NAME_LENGTH] = {0};

	Text *array = list->texts;

	for (int i = 0; i < MAX_TEXTS; i++)
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
	return RemoveTextFromList(input, &GameWorld->TextList);
}

int RemoveTextFromList(Text *input, TextList *list)
{
	if (input->text == NULL)
	{
		return EXECUTION_UNNECESSARY;
	}

	if (input->usedFont != NULL)
	{
		Font *usedFont = input->usedFont;
		usedFont->textCount--;
		if (usedFont->textCount <= 0 && usedFont->deleteWhenUnused)
		{
			closeFont(usedFont);
		}

		input->usedFont = NULL;
	}

	TTF_DestroyText(input->text);
	input->text = NULL;
	input->attachedObj = NULL;
	list->count--;

	return LEMON_SUCCESS;
}

int RemoveTextWithName(const char name[], World *GameWorld)
{
	return RemoveText(getTextWithName(name, GameWorld), GameWorld);
}


void RemoveAllTexts(World *GameWorld)
{
	TextList *list = &GameWorld->TextList;

	int i = 0;
	while (i < MAX_TEXTS && list->count > 0)
	{
		RemoveText(&list->texts[i], GameWorld);

		i++;
	}

	list->count = 0;

	return;
}

void RemoveObjectDebugTexts(void)
{
	TextList *list = &TextSettings.DebugTextList;
	Text *texts = list->texts;

	int i = 0;
	while (i < MAX_TEXTS && list->count > 0)
	{
		if (list->texts[i].text != NULL && list->texts[i].name[0] == '\0')
		{
			RemoveTextFromList(&texts[i], list);
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

	for (int i = 0; i < MAX_TEXTS; i++)
	{
		if (array[i].attachedObj == input)
		{
			RemoveText(&array[i], GameWorld);
		}
	}

	return;
}

void initialiseFontList(FontList *input)
{
	input->count = 0;
	Font *list = input->fonts;

	for (int i = 0; i < MAX_LOADED_FONTS; i++)
	{
		list[i].font = NULL;
		memset(list[i].name, 0, FONT_FILE_NAME_MAX);
	}

	return;
}

void closeFont(Font *input)
{
	if (input == NULL || input->textCount > 0)
	{
		return;
	}

	TTF_CloseFont(input->font);
	input->font = NULL;
	input->name[0] = '\0';

	return;
}

Font* loadFont(const char *desiredFont, const char *newName, World *GameWorld)
{	
	if (GameWorld == NULL)
	{
		return NULL;
	}

	Font *newFont = getFont(newName, GameWorld);
	if (newFont != NULL)
	{
		return newFont;
	}

	Font *list = GameWorld->FontList.fonts;

	int index = 0;
	while (index < MAX_LOADED_FONTS && newFont == NULL)
	{
		if (list[index].font == NULL)
		{
			newFont = &list[index];
		}
		index++;
	}

	if (newFont == NULL)
	{
		return NULL;
	}
	
	char fontName[MAX_LEN] = FONT_ROOT;
	strcat(fontName, desiredFont);

	if (fontName[strlen(fontName) - 4] != '.')
	{
		strcat(fontName, ".ttf");
	}
	
	newFont->font = TTF_OpenFont(fontName, TextSettings.defaultTextPointSize);

	if (newFont->font == NULL)
	{ 
    	putConsole("\nFailed to load font! (%s)\n", SDL_GetError());
    	return NULL;
	}

	strcpy(newFont->name, newName);
	newFont->textCount = 0;
	newFont->deleteWhenUnused = true;

	GameWorld->FontList.count++;

	return newFont;
}

Font* loadFontWithSize(const char *desiredFont, const char *newName, float pointSize, World *GameWorld)
{
	Font *loaded = loadFont(desiredFont, newName, GameWorld);

	if (loaded != NULL)
	{
		TTF_SetFontSize(loaded->font, pointSize);
	}

	return loaded;
}

Font* getFont(const char *name, World *GameWorld)
{
	if (GameWorld == NULL || strcmp(name, "DebugFont") == 0 || name[0] == '\0')
	{
		return &TextSettings.DebugFont;
	}

	Font *list = GameWorld->FontList.fonts;

	for (int i = 0; i < MAX_LOADED_FONTS; i++)
	{
		if (strcmp(list[i].name, name) == 0 && list[i].font != NULL)
		{
			return &list[i];
		}
	}

	return NULL;
}

void setFontSize(const char *name, int size, World *GameWorld)
{
	Font *font = getFont(name, GameWorld);
	
	if (font == NULL)
	{
		return;
	}

	TTF_SetFontSize(font->font, size);

	return;
}


void cleanUpTexts(TextList *list)
{
	if (list == NULL)
	{
		return;
	}

	Text *array = list->texts;

	for (int i = 0; i < MAX_TEXTS; i++)
	{
		if (array[i].text != NULL)
		{
			RemoveTextFromList(&array[i], list);
		}
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

	Font *list = input->fonts;

	for (int i = 0; i < MAX_LOADED_FONTS; i++)
	{
		if (list[i].font != NULL)
		{
			TTF_CloseFont(list[i].font);
			list[i].font = NULL;
		}

		list[i].name[0] = '\0';
	}

	input->count = 0;

	return;
}

void cleanUpTextData(RenderFrame *ScreenData)
{
	cleanUpTexts(&TextSettings.DebugTextList);

	if (TextSettings.DebugFont.font != NULL)
	{
		TTF_CloseFont(TextSettings.DebugFont.font);
		TextSettings.DebugFont.font = NULL;
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
	if (inputCharacter == NULL)
	{
		return MISSING_DATA;
	}

	TextBox *inputText = getCurrentTextBox(GameWorld);

	if (inputText == NULL)
	{
		MarkObjectForDeletion(inputCharacter);
		return MISSING_DATA;
	}

	inputCharacter->Parent = inputText->boxPtr;
	inputCharacter->ParentLink = MOTION_LINK | HIDDEN_LINK;
	inputCharacter->ObjectBox->xSize = (int)inputText->TextSize;
	inputCharacter->ObjectBox->ySize = (int)inputText->TextSize;

	if (charValue > 32 && charValue < 123)
	{
		switchSpriteByName("Pixel_White", 0, inputCharacter->ObjectDisplay);
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


Object* createTextBoxSprite(TextBox *inputText, const char textBoxName[], World *GameWorld)
{
	Object *Box = AddObject(GameWorld, UI_TEXT, inputText->boxStartPosX, inputText->boxStartPosY, TEXT_BOX, 0, 0, 0, 0);

	inputText->boxPtr = Box;

	DisplayData *boxDisplay = getDisplay(Box);

	if (inputText->boxPtr == NULL || boxDisplay == NULL)
	{
		return NULL;
	}

	setObjectName(Box, "TextBox");

	if (textBoxName[0] == '\0')
	{
		boxDisplay->currentSprite = -1;
		boxDisplay->spriteBuffer = NULL;
		return Box;
	}

	switchSpriteByName(textBoxName, 0, boxDisplay);

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

	Object *portrait = AddObjectWithParent(GameWorld, inputText->boxPtr, UI_TEXT, inputText->boxStartPosX, boxYPos, TEXT_PORTRAIT, 0, 0, 0, 0);

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
			portrait->ObjectBox->prevXPos = portrait->ObjectBox->xPos;
		}

		inputText->textLengthSize -= TextSettings.portraitSize + 30;
	}

	return portrait;
}

int mapTextToCharacter(Object *inputText, int characterValue)
{
	DisplayData *charDisplay = inputText->ObjectDisplay;

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


int endTextBox(TextBox *text, World *GameWorld)
{
	if (text == NULL)
	{
		return MISSING_DATA;
	}

	switch (text->textTypeSetting)
	{
		case TEXTBOX_OPTION_PROMPT:
		{
			TextOptionPrompt *optionData = &text->textTypeData.OptionPrompt;
			optionData->setUpComplete = false;
			swapStrings(text->textPhrase, optionData->optionNames[optionData->numberOfOptions - 1], OPTION_TEXT_MAX_LEN);
		
			if (optionData->SelectedOption < 0 || optionData->SelectedOption >= optionData->numberOfOptions)
			{
				optionData->SelectedOption = 0;
				break;
			}

			if (optionData->optionTriggers != NULL)
			{
				triggerGameEvent(&optionData->optionTriggers[optionData->SelectedOption], GameWorld);
			}

			optionData->SelectedOption = 0;
		} break;

		case TEXTBOX_TRIGGER_EVENT:
		{
			triggerGameEvent(text->textTypeData.TriggerEvent, GameWorld);
		} break;

		default:
			break;
	}


	deleteTextBox(text, GameWorld);


	return LEMON_SUCCESS;
}


int deleteTextBox(TextBox *input, World *GameWorld)
{
	if (input == NULL)
	{
		return MISSING_DATA;
	}

	MarkObjectForDeletion(input->boxPtr);

	removeAttachedTexts(input->boxPtr, GameWorld);	// technically unnecessary, as deleting the object also deletes text 
	input->boxPtr = NULL;
	input->currentIndex = 0;

	GameWorld->TextBox = false;

	return LEMON_SUCCESS;
}

TextBox* getCurrentTextBox(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->SceneActionQueue == NULL)
	{
		return NULL;
	}

	SceneAction *currentAction = GameWorld->nextSceneAction;

	while (currentAction != NULL)
	{
		if (currentAction->ActionID == SCENE_SAY_TEXT)
		{
			return &currentAction->ActionData.sceneText;
		}

		if (currentAction->parallelAction == false)
		{
			return NULL;
		}

		currentAction = currentAction->nextSceneAction;
	}	

	return NULL;
}
