EXPORT int StartGame(World *GameWorld);


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


int applyScreenSize(int newWidth, int newHeight, RenderFrame *ScreenData);

int applyScreenSizeScale(int newWidth, int newHeight, Camera *inputCamera, RenderFrame *ScreenData);

int applyEnableFullscreen(RenderFrame *ScreenData);

int applyEnableFullscreenScaled(RenderFrame *ScreenData, Camera *inputCamera);

int applyDisableFullscreen(RenderFrame *ScreenData, Camera *inputCamera);

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

bool parseArgumentAsBoolean(const char input[USER_INPUT_MAX_LEN]);

void updateTypedCommand(SDL_Window *window, World *GameWorld);

void startTypedCommand(SDL_Window *window);

void addTypedCommand(const char input[]);

Object* parseArgumentToFindObject(const char input[USER_INPUT_MAX_LEN], ObjectController *ObjectList);

void createConsoleCommands(ConsoleCommand commandList[MAX_CONSOLE_COMMANDS]);


int ConsoleCommand_Version(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Quit(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Tick(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Show(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Vsync(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Fps(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Debug(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Draw(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_HitboxThickness(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_UsedMemory(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_AddObject(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Object(char input[USER_INPUT_MAX_LEN], World *GameWorld);

void displayObjectInfoConsole(Object *input);

int ConsoleCommand_BackGround(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Level(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Event(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_List(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_CamView(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Sound(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Cutscene(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Load(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_DebugText(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Pause(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_SetPos(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_SetCamPos(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_SetCamZoom(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_SetTickRate(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Save(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_LoadSave(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_SetGameFlag(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Help(char input[USER_INPUT_MAX_LEN], World *GameWorld);


void renderConsole(World *GameWorld, SDL_Renderer *Screen);