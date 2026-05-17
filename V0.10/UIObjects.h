int SpawnHUD(World *GameWorld);

int HideHUD(ObjectController *ObjectList);

int ShowHUD(ObjectController *ObjectList);


UISubType convertEntryToUIType(char name[]);

char* convertUITypeToName(UISubType input);


int InitialiseUIElement(Object *UIElement, World *GameWorld);

int InitialiseUIText(Object *UIText, World *GameWorld);

int LoadUISprites(SpriteSet *newSet);

int LoadUITextSprites(SpriteSet *newSet);


int UpdateUIElement(World *GameWorld, Object *UIElement);

int UpdateCursor(Object *Cursor, World *GameWorld);

int UpdateTextBox(Object *textBox);


bool MenuControl(Object *MenuController, World *GameWorld);

int PauseMenu(Object *MenuController, World *GameWorld);

int SaveMenu(Object *MenuController, World *GameWorld);

int SettingsMenuControl(Object *MenuController, World *GameWorld);

int VideoSettingsControl(Object *MenuController, World *GameWorld);

int SoundSettingsControl(Object *MenuController, World *GameWorld);

Object* AddOptionButton(const char spriteName[], int xPos, int yPos, Object *MenuController, World *GameWorld);

int UpdateOptionButton(Object *Button, Camera inputCam);


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


Text* addText(const char *textPhrase, float xPos, float yPos);

Text* addTextWithFont(const char textPhrase[], float xPos, float yPos, const char *font);

Text* addTextWithName(const char textPhrase[], const char name[], float xPos, float yPos);

Text* getTextWithName(const char name[]);

Text* addTextToList(TextList *list, const char textPhrase[], float xPos, float yPos, int wrapWidth, const char *desiredFont);


void updateText(Text *input, const char newPhrase[]);

void updateTextWithName(const char name[], const char newPhrase[]);

void attachTextToObject(Text *input, Object *obj);

void attachTextWithNameToObject(const char name[], Object *input);

void moveText(Text *input, float xPos, float yPos);

void moveTextWithName(const char name[], float xPos, float yPos);

void setTextColour(Text *input, SDL_Color *colour);

void setTextColourWithName(const char name[], SDL_Color *colour);

void setTextName(Text *input, const char name[]);

int RemoveText(Text *input);

int RemoveTextWithName(const char name[]);



void initialiseTextList(TextList *input);

void printTextsinfo(TextList *list, const char name[]);

void RemoveAllTexts(TextList *list);

void removeAttachedTexts(Object *input);

Text* experimentalText(TextBox *input);


void initialiseFontList(FontList *input);

TTF_Font* loadFont(const char *desiredFont, const char *newName);

TTF_Font* loadFontWithSize(const char *desiredFont, const char *newName, float pointSize);

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