bool playingText(World *GameWorld);

int UpdateUIText(World *GameWorld, Object *UIText);

TextBox* SayText(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld);				// Equivalent to calling createText and then applyTextPresets

TextBox* SayTextOption(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld, int numberOfOptions, ...);

int SayTextAndTriggerEvent(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld, GameEvent *inputEvent);


int ApplyTextPresets(TextBox *inputText, World *GameWorld);

int insertLineBreaks(char *input, int maxLength);

void removeControlCharacters(TextBox *input, int characterCount);


int updateTextBoxes(World *GameWorld);

int TextInteraction(TextBox *currentText, World *GameWorld);

int handleOptionPrompt(TextBox *inputText, World *GameWorld);

int displayText(TextBox *currentText, World *GameWorld);

int displayNextCharacter(TextBox *inputText, World *GameWorld);

void setTextBoxColor(TextBox *inputText, int *decodedIndex);

int playTextVoice(TextBox *currentText);

Text* placeTextCharacter(TextBox *input, int decodedChar, World *GameWorld);


Text* addText(const char *textPhrase, float xPos, float yPos, World *GameWorld);

Text* addTextWithName(const char textPhrase[], const char name[], float xPos, float yPos, World *GameWorld);

Text* getTextWithName(const char name[], World *GameWorld);

Text* addTextWithFont(const char textPhrase[], float xPos, float yPos, const char *desiredFont, World *GameWorld);


void updateText(Text *input, const char newPhrase[]);

void updateTextWithName(const char name[], const char newPhrase[], World *GameWorld);

void attachTextToObject(Text *input, Object *obj);

void attachTextWithNameToObject(const char name[], Object *input, World *GameWorld);

void moveText(Text *input, float xPos, float yPos);

void moveTextWithName(const char name[], float xPos, float yPos, World *GameWorld);

void setTextTransparency(Text *input, float transparency);

void setTextWithNameTransparency(const char name[], float transparency, World *GameWorld);

void setTextColour(Text *input, SDL_Color colour);

void setTextColourWithName(const char name[], SDL_Color colour, World *GameWorld);

void setTextWrapWidth(Text *input, int wrapWidth);

void setTextName(Text *input, const char name[]);

int RemoveText(Text *input, World *GameWorld);

int RemoveTextFromList(Text *input, TextList *list);

int RemoveTextWithName(const char name[], World *GameWorld);



void initialiseTextList(TextList *input);

void printTextListinfo(TextList *list, const char name[]);

void RemoveAllTexts(World *GameWorld);

void RemoveObjectDebugTexts(void);


void initialiseFontList(FontList *input);

void closeFont(TTF_Font *font, World *GameWorld);

TTF_Font* loadFont(const char *desiredFont, const char *newName, World *GameWorld);

TTF_Font* loadFontWithSize(const char *desiredFont, const char *newName, float pointSize, World *GameWorld);

TTF_Font* getFont(const char *name, World *GameWorld);

void setFontSize(const char *name, int size, World *GameWorld);


void cleanUpTexts(TextList *list);

void cleanUpFonts(FontList *input);

void cleanUpTextData(RenderFrame *ScreenData);



int initialiseTextCharacter(Object *inputCharacter, char charValue, World *GameWorld);

Object* createTextBoxSprite(TextBox *inputText, const char textBoxName[], World *GameWorld);

Object* createTextBoxPortrait(TextBox *inputText, World *GameWorld);


int mapTextToCharacter(Object *inputText, int characterValue);

int getCharacterSpacing(char input);


int endTextBox(World *GameWorld);

int deleteTextBox(TextBox *input, World *GameWorld);

int clearTextQueue(World *GameWorld);


bool textSceneActionPresent(TextBox *inputText, World *GameWorld);

void DeleteTextSceneAction(TextBox *inputText, World *GameWorld);