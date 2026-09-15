EXPORT int StartGame(World *GameWorld);


int HandleGameEvents(World *GameWorld, RenderFrame *ScreenData);

int ExecuteGameEvent(GameEvent *inputEvent, World *GameWorld, RenderFrame *ScreenData);

void startTyping(SDL_Window *window, Text *inputTypingText);

void stopTyping(SDL_Window *window);

void updateTyping(SDL_Window *window, World *GameWorld);

void inputTyping(const char input[]);

void setCursorPos(void);


int deleteAllGameEvents(GameEventManager *manager);

int clearGameEvents(GameEventManager *manager);

GameEvent* findAvailableEvent(GameEventManager *Manager);

int eventNetworkingPermission(GameEventID input);

GameEvent* addNewGameEvent(GameEventID eventID, World *GameWorld);

int triggerGameEvent(GameEvent *inputEvent, World *GameWorld);

void removeEventToTriggerLater(GameEvent *inputEvent, GameEvent *storage, World *GameWorld);


GameEvent* switchLevel(int level, World *GameWorld);

GameEvent* playCutscene(int scene, World *GameWorld);

GameEvent* playCutsceneFromFile(const char name[], World *GameWorld);

GameEvent* Message(char msg[], int speakerID, World *GameWorld);

GameEvent* Event_MovePlayer(float xPos, float yPos, World *GameWorld);

GameEvent* Event_MoveObject(Object *input, float xPos, float yPos, World *GameWorld);

GameEvent* Event_TeleportPlayerToExitDoor(Object *dest, World *GameWorld);

GameEvent* Event_SetScreenBrightness(float brightness, World *GameWorld);

GameEvent* streamPartition(int partID, World *GameWorld);

GameEvent* changeScreenSizeScaled(int newWidth, int newHeight, World *GameWorld);

GameEvent* changeScreenSize(int newWidth, int newHeight, World *GameWorld);

GameEvent* setScreenAndRendererSize(int newWidth, int newHeight, World *GameWorld);

GameEvent* enableFullscreen(World *GameWorld);

GameEvent* enableFullscreenScaled(World *GameWorld);

GameEvent* disableFullscreen(World *GameWorld);


int applyScreenSize(int newWidth, int newHeight, RenderFrame *ScreenData, World *GameWorld);

int applyScreenSizeScale(int newWidth, int newHeight, Camera *inputCamera, RenderFrame *ScreenData);

int applyEnableFullscreen(RenderFrame *ScreenData, World *GameWorld);

int applyEnableFullscreenScaled(RenderFrame *ScreenData, Camera *inputCamera);

int applyDisableFullscreen(RenderFrame *ScreenData, Camera *inputCamera, World *GameWorld);

int validateScreenDimensions(RenderFrame *ScreenData);

int validateZoom(Camera *inputCamera, RenderFrame *ScreenData);


bool getNextArgGameEvent(FILE *file, GameEvent *newEvent, World *GameWorld);

void addGameEventInt(GameEvent *input, const char name[], int val);

int getGameEventInt(GameEvent *input, const char name[]);

void addGameEventFloat(GameEvent *input, const char name[], float val);

int getGameEventFloat(GameEvent *input, const char name[]);

void addGameEventString(GameEvent *input, const char name[], const char val[]);

char* getGameEventString(GameEvent *input, const char name[]);

void addGameEventTick(GameEvent *input, const char name[], Uint64 val);

Uint64 getGameEventTick(GameEvent *input, const char name[]);



int PauseGame(World *GameWorld);

int ResumeGame(World *GameWorld);


int InitialiseLevelFlag(Object *inputObject, ObjectController *ObjectList);

bool detectPlayer(Object* inputObject, PlayerData *Player);

bool detectCamera(Object* inputObject, Camera inputCamera);


int UpdateFlagObject(Object* inputObject, World *GameWorld);
