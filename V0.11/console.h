void updateConsole(SDL_Window *window, World *GameWorld);

ConsoleCommand* getConsoleCommand(const char name[]);

ConsoleVariable* getConsoleVariable(const char name[]);

ConsoleVariable* NewConsoleVariable(const char name[], const char helpString[], ConsoleVariableType valueType, const char value[], ConsoleCommandFlag flags);

int hashConVar(const char name[]);

void setConsoleVariable(ConsoleVariable *variable, const char value[], World *GameWorld);

void setConVarNamed(const char *name, const char *value, World *GameWorld);

int ConVarAsInt(ConsoleVariable *variable);

int getConVarAsInt(const char input[]);

float ConVarAsFloat(ConsoleVariable *variable);

float getConVarAsFloat(const char input[]);

bool ConVarAsBool(ConsoleVariable *variable);

bool getConVarAsBool(const char input[]);

const char* ConVarAsString(ConsoleVariable *variable);

const char* getConVarAsString(const char input[]);

const char* ConVarValueToString(ConsoleVariable *variable, char result[32]);


void consoleInput(const char inputSource[USER_INPUT_MAX_LEN], World *GameWorld);

void executeCommand(char input[USER_INPUT_MAX_LEN], World *GameWorld);

bool commandIsAllowed(ConsoleCommandFlag input);

void getNextConsoleArg(const char input[], char argDest[]);

bool nextConsoleArgIsNumber(const char input[USER_INPUT_MAX_LEN]);

bool hasNextConsoleArg(const char input[USER_INPUT_MAX_LEN]);

int getNextConsoleInt(const char input[USER_INPUT_MAX_LEN]);

float getNextConsoleFloat(const char input[USER_INPUT_MAX_LEN]);

void getNextConsoleFlag(char input[USER_INPUT_MAX_LEN], char argDest[USER_INPUT_MAX_LEN]);

bool getNextConsoleBool(const char input[USER_INPUT_MAX_LEN]);

Object* parseArgumentToFindObject(const char input[USER_INPUT_MAX_LEN], ObjectController *ObjectList);

void initialiseConsoleVariables(ConsoleVariableList *list);

void createConsoleCommands(ConsoleCommand commandList[MAX_CONSOLE_COMMANDS]);


int ConsoleCommand_Version(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Quit(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Restart(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Tick(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Say(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_SayText(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_StartServer(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_CloseServer(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Connect(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Disconnect(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_Kick(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_ListClients(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_ResetBlacklist(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_ServerPassword(char input[USER_INPUT_MAX_LEN], World *GameWorld);

int ConsoleCommand_SetUsername(char input[USER_INPUT_MAX_LEN], World *GameWorld);

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


void updateConsoleHistoryText(Text *input);

void renderConsole(World *GameWorld, SDL_Renderer *Screen);