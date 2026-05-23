int SpawnHUD(World *GameWorld);

int HideHUD(ObjectController *ObjectList);

int ShowHUD(ObjectController *ObjectList);

void adjustHUD(int prevScreenWidth, int prevScreenHeight, RenderFrame *ScreenData, World *GameWorld);


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
