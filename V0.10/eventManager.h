int StartGame(World *GameWorld);


int HandleGameEvents(World *GameWorld, RenderFrame *ScreenData);

int ExecuteGameEvent(GameEvent *inputEvent, World *GameWorld, RenderFrame *ScreenData);

int deleteAllGameEvents(World *GameWorld);

int clearGameEvents(World *GameWorld);

GameEvent* addNewGameEvent(World *GameWorld);

int triggerGameEvent(GameEvent *inputEvent, World *GameWorld);

void removeEventToTriggerLater(GameEvent *inputEvent, GameEvent *storage, World *GameWorld);


GameEvent* switchLevel(int level, World *GameWorld);

GameEvent* playCutscene(int scene, World *GameWorld);

GameEvent* playCutsceneFromFile(const char name[], World *GameWorld);

GameEvent* Event_movePlayer(float xPos, float yPos, World *GameWorld);

GameEvent* Event_moveObject(Object *input, float xPos, float yPos, World *GameWorld);

GameEvent* Event_teleportPlayerToExitDoor(Object *dest, World *GameWorld);

GameEvent* Event_setScreenBrightness(float brightness, World *GameWorld);

GameEvent* scheduleEnvironmentDeletion(World *GameWorld);

GameEvent* streamPartition(int partID, World *GameWorld);

GameEvent* switchToNewPartition(int partID, World *GameWorld);

GameEvent* changeScreenSizeScaled(int newWidth, int newHeight, World *GameWorld);

GameEvent* changeScreenSize(int newWidth, int newHeight, World *GameWorld);

GameEvent* setScreenAndRendererSize(int newWidth, int newHeight, World *GameWorld);

GameEvent* enableFullscreen(World *GameWorld);

GameEvent* enableFullscreenScaled(World *GameWorld);

GameEvent* disableFullscreen(World *GameWorld);

GameEvent* setCameraZoom(float zoomX, float zoomY, World *GameWorld);

GameEvent* changeCameraZoom(float zoomX, float zoomY, World *GameWorld);


int applyCameraZoom(float newZoomX, float newZoomY, Camera *inputCamera, RenderFrame *ScreenData);

int applyScreenAndRendererSize(int newWidth, int newHeight, RenderFrame *ScreenData);

int applyScreenSize(int newWidth, int newHeight, RenderFrame *ScreenData);

int applyScreenSizeScale(int newWidth, int newHeight, RenderFrame *ScreenData);

int applyEnableFullscreen(RenderFrame *ScreenData);

int applyEnableFullscreenScaled(RenderFrame *ScreenData);

int applyDisableFullscreen(RenderFrame *ScreenData);

int validateScreenDimensions(RenderFrame *ScreenData);

int validateZoom(Camera *inputCamera, RenderFrame *ScreenData);



int PauseGame(World *GameWorld);

int ResumeGame(World *GameWorld);


int InitialiseLevelFlag(Object *inputObject, ObjectController *ObjectList);

bool detectPlayer(Object* inputObject, PlayerData *Player);

bool detectCamera(Object* inputObject, Camera inputCamera);


int UpdateFlagObject(Object* inputObject, PlayerData *Player, World *GameWorld);


void executeCommand(char inputSource[], World *GameWorld);

void parseArgument(const char input[USER_INPUT_MAX_LEN], char argDest[USER_INPUT_MAX_LEN]);

int parseArgumentAsInt(const char input[USER_INPUT_MAX_LEN]);

float parseArgumentAsFloat(const char input[USER_INPUT_MAX_LEN]);

void parseArgumentFlag(char input[USER_INPUT_MAX_LEN], char argDest[USER_INPUT_MAX_LEN]);

bool parseBooleanCommand(const char input[USER_INPUT_MAX_LEN]);

void updateTypedCommand(SDL_Window *window, World *GameWorld);

void startTypedCommand(SDL_Window *window);

void addTypedCommand(const char input[]);

Object* parseArgumentToFindObject(const char input[USER_INPUT_MAX_LEN], ObjectController *ObjectList);

void displayObjectInfoConsole(Object *input);

void renderConsole(World *GameWorld, SDL_Renderer *Screen);