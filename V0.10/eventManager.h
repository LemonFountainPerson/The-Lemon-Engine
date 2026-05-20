EXPORT int StartGame(World *GameWorld);

int getCurrentIndex(int input[], int ID);

int getIndexDistance(int input[], int ID);

int getMostImagesOnRow(int row);

int getMostImagesOnColumn(int column);

int createRow(World *GameWorld);

int createColumn(World *GameWorld);


int HandleGameEvents(World *GameWorld, RenderFrame *ScreenData);

int ExecuteGameEvent(GameEvent *inputEvent, World *GameWorld, RenderFrame *ScreenData);

void startTyping(SDL_Window *window, Text *inputTypingText);

void stopTyping(SDL_Window *window);

void updateTyping(SDL_Window *window, World *GameWorld);

void inputTyping(const char input[]);


int deleteAllGameEvents(World *GameWorld);

int clearGameEvents(World *GameWorld);

GameEvent* addNewGameEvent(World *GameWorld);

int triggerGameEvent(GameEvent *inputEvent, World *GameWorld);

void removeEventToTriggerLater(GameEvent *inputEvent, GameEvent *storage, World *GameWorld);


GameEvent* switchLevel(int level, World *GameWorld);

GameEvent* playCutscene(int scene, World *GameWorld);

GameEvent* playCutsceneFromFile(const char name[], World *GameWorld);

GameEvent* Event_MovePlayer(float xPos, float yPos, World *GameWorld);

GameEvent* Event_MoveObject(Object *input, float xPos, float yPos, World *GameWorld);

GameEvent* Event_TeleportPlayerToExitDoor(Object *dest, World *GameWorld);

GameEvent* Event_SetScreenBrightness(float brightness, World *GameWorld);

GameEvent* scheduleEnvironmentDeletion(World *GameWorld);

GameEvent* streamPartition(int partID, World *GameWorld);

GameEvent* switchToNewPartition(int partID, World *GameWorld);

GameEvent* changeScreenSizeScaled(int newWidth, int newHeight, World *GameWorld);

GameEvent* changeScreenSize(int newWidth, int newHeight, World *GameWorld);

GameEvent* setScreenAndRendererSize(int newWidth, int newHeight, World *GameWorld);

GameEvent* enableFullscreen(World *GameWorld);

GameEvent* enableFullscreenScaled(World *GameWorld);

GameEvent* disableFullscreen(World *GameWorld);


int applyScreenSize(int newWidth, int newHeight, RenderFrame *ScreenData);

int applyScreenSizeScale(int newWidth, int newHeight, Camera *inputCamera, RenderFrame *ScreenData);

int applyEnableFullscreen(RenderFrame *ScreenData);

int applyEnableFullscreenScaled(RenderFrame *ScreenData, Camera *inputCamera);

int applyDisableFullscreen(RenderFrame *ScreenData, Camera *inputCamera);

int validateScreenDimensions(RenderFrame *ScreenData);

int validateZoom(Camera *inputCamera, RenderFrame *ScreenData);


GameEvent* getNextArgGameEvent(FILE *file, World *GameWorld);


int PauseGame(World *GameWorld);

int ResumeGame(World *GameWorld);


int InitialiseLevelFlag(Object *inputObject, ObjectController *ObjectList);

bool detectPlayer(Object* inputObject, PlayerData *Player);

bool detectCamera(Object* inputObject, Camera inputCamera);


int UpdateFlagObject(Object* inputObject, World *GameWorld);
