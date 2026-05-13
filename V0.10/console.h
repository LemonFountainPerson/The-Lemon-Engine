void executeCommand(char inputSource[USER_INPUT_MAX_LEN], World *GameWorld);

void parseArgument(const char input[USER_INPUT_MAX_LEN], char argDest[USER_INPUT_MAX_LEN]);

int parseArgumentAsInt(const char input[USER_INPUT_MAX_LEN]);

float parseArgumentAsFloat(const char input[USER_INPUT_MAX_LEN]);

void parseArgumentFlag(char input[USER_INPUT_MAX_LEN], char argDest[USER_INPUT_MAX_LEN]);

bool parseArgumentAsBoolean(const char input[USER_INPUT_MAX_LEN]);

void updateConsole(SDL_Window *window, World *GameWorld);

Object* parseArgumentToFindObject(const char input[USER_INPUT_MAX_LEN], ObjectController *ObjectList);

void createConsoleCommands(ConsoleCommand commandList[MAX_CONSOLE_COMMANDS]);


int ConsoleCommand_Version(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Quit(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Restart(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Tick(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_SetCameraZoom(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Fullscreen(char input[USER_INPUT_MAX_LEN], World *GameWorld);

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

int ConsoleCommand_SaveSettings(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_LoadSave(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_LoadSettings(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_AddGameFlag(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_SetGameFlag(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_CheckGameFlag(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Help(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_DoABarrelRoll(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Noclip(char input[USER_INPUT_MAX_LEN], World *GameWorld);


void renderConsole(World *GameWorld, SDL_Renderer *Screen);