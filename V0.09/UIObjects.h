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


int UpdateUIText(World *GameWorld, Object *UIText);

TextInstance* SayText(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld);				// Equivalent to calling createText and then applyTextPresets

TextInstance* SayTextOption(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld, int numberOfOptions, ...);

int AddTriggerableEventToText(TextInstance *inputText, TriggerableFunction triggerFunction, void *functionInput);

int AddFunctionArgumentsToTriggerEvent(struct TextEventTrigger *eventTrigger, va_list *argInput);


TextInstance* CreateText(const char inputPhrase[], World *GameWorld);

int ApplyTextPresets(TextInstance *inputText, const char Portrait[], TextPreset inputPreset);

int insertLineBreaks(char *input, int maxLength);


int updateText(World *GameWorld);

int TextInteraction(TextInstance *currentText, World *GameWorld);

int handleOptionPrompt(TextInstance *inputText, World *GameWorld);

int displayText(TextInstance *currentText, World *GameWorld);

int displayNextCharacter(TextInstance *inputText, World *GameWorld);

int playTextVoice(TextInstance *currentText);


int initialiseTextCharacter(Object *inputCharacter, char charValue, World *GameWorld);

Object* createTextBox(TextInstance *inputText, World *GameWorld);

Object* createTextBoxPortrait(TextInstance *inputText, World *GameWorld);


int mapTextToCharacter(Object *inputText, int characterValue);

int getCharacterSpacing(char input);


int endTextInstance(World *GameWorld);

int runTriggerableFunction(struct TextEventTrigger *DataPtr, World *GameWorld);

int deleteTextInstance(World *GameWorld);

int clearTextQueue(World *GameWorld);


bool textSceneActionPresent(TextInstance *inputText, World *GameWorld);

void DeleteTextSceneAction(TextInstance *inputText, World *GameWorld);