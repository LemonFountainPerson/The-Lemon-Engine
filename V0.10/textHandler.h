bool playingText(World *GameWorld);

int UpdateUIText(World *GameWorld, Object *UIText);

TextBox* SayText(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld);				// Equivalent to calling createText and then applyTextPresets

TextBox* SayTextOption(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld, int numberOfOptions, ...);

int AddTriggerableEventToText(TextBox *inputText, GameEvent *inputEvent, World *GameWorld);


TextBox* CreateText(const char inputPhrase[], World *GameWorld);

int ApplyTextPresets(TextBox *inputText, const char Portrait[], TextPreset inputPreset);

int insertLineBreaks(char *input, int maxLength);

void removeControlCharacters(char input[MAX_TEXT_LENGTH], int characterCount);


int updateTextBoxes(World *GameWorld);

int TextInteraction(TextBox *currentText, World *GameWorld);

int handleOptionPrompt(TextBox *inputText, World *GameWorld);

int displayText(TextBox *currentText, World *GameWorld);

int displayNextCharacter(TextBox *inputText, World *GameWorld);

int playTextVoice(TextBox *currentText);


Text* addText(const char *textPhrase, float xPos, float yPos, World *GameWorld);

Text* addTextWithFont(const char textPhrase[], float xPos, float yPos, const char *font, World *GameWorld);

Text* addTextWithName(const char textPhrase[], const char name[], float xPos, float yPos, World *GameWorld);

Text* getTextWithName(const char name[], World *GameWorld);

Text* addTextToList(TextList *list, const char textPhrase[], float xPos, float yPos, int wrapWidth, const char *desiredFont);


void updateText(Text *input, const char newPhrase[]);

void updateTextWithName(const char name[], const char newPhrase[], World *GameWorld);

void attachTextToObject(Text *input, Object *obj);

void attachTextWithNameToObject(const char name[], Object *input, World *GameWorld);

void moveText(Text *input, float xPos, float yPos);

void moveTextWithName(const char name[], float xPos, float yPos, World *GameWorld);

void setTextColour(Text *input, SDL_Color *colour);

void setTextColourWithName(const char name[], SDL_Color *colour, World *GameWorld);

void setTextName(Text *input, const char name[]);

int RemoveText(Text *input, World *GameWorld);

int RemoveTextWithName(const char name[], World *GameWorld);



void initialiseTextList(TextList *input);

void printTextsinfo(TextList *list, const char name[]);

void RemoveAllTexts(TextList *list);

void RemoveUnnamedTexts(TextList *list);

void closeFont(TTF_Font *font, TextList *list);

Text* experimentalText(TextBox *input, World *GameWorld);


void initialiseFontList(FontList *input);

TTF_Font* loadFont(const char *desiredFont, const char *newName, TextList *texts);

TTF_Font* loadFontWithSize(const char *desiredFont, const char *newName, float pointSize, TextList *texts);

TTF_Font* getFont(const char *name);

void setFontSize(const char *name, int size);


void cleanUpTexts(TextList *list);

void cleanUpTextData(RenderFrame *ScreenData);



int initialiseTextCharacter(Object *inputCharacter, char charValue, World *GameWorld);

Object* createTextBox(TextBox *inputText, World *GameWorld);

Object* createTextBoxPortrait(TextBox *inputText, World *GameWorld);


int mapTextToCharacter(Object *inputText, int characterValue);

int getCharacterSpacing(char input);


int endTextBox(World *GameWorld);

int deleteTextBox(TextBox *input, World *GameWorld);

int clearTextQueue(World *GameWorld);


bool textSceneActionPresent(TextBox *inputText, World *GameWorld);

void DeleteTextSceneAction(TextBox *inputText, World *GameWorld);