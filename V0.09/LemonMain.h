int RunLemonEngine(void);

int StartUpLemonEngine();

int MainLoop(World *GameWorld);

int CloseGame(World *GameWorld);


int initialiseWorld(World *GameWorld);

int initialiseBackGround(BackgroundData *input);

ObjectController* createObjectController(void);

void clearGameData(World *GameWorld);


int GameTick(World *GameWorld);


int GameFrame(World *GameWorld);


int RenderEngine(World *GameWorld, Camera renderCamera, RenderFrame ScreenData);


int FPSCounter(void);


int ResetCamera(Camera *inputCam);

int setCameraPos(Camera *input, float xPos, float yPos);

int SetEngineSettingsToDefault(void);

int SetRenderSettingsToDefault(void);

int SetDebugSettingsToDefault(void);


FuncResult CheckResourceData(void);


int getExternalInput(World *GameWorld, SDL_Window *window);


int updateMouse(void);

float getMouseXCamRelative(Camera inputCamera);

float getMouseYCamRelative(Camera inputCamera);

float getMouseXZoom(Camera inputCamera);

float getMouseYZoom(Camera inputCamera);

int getMouseInput(SDL_MouseButtonEvent event);


int getKeyboardInput(SDL_Event *event);

void updateCustomKeys();

bool buttonPressed(int key);

bool keyPressed(int key);

bool buttonHeld(int key);

bool keyHeld(int key);

int ClearKeyboardInput();


int AcknowledgeHeldButtons();

int AcknowledgeButton(LemonKeys Key);


int setTickRate(int desiredTickRate);


int setRenderRefreshRate(int desiredRenderRate);


int SetGravity(World *GameWorld, float force, float directionDegrees);


void MasterControls(World *GameWorld, SDL_Window *window);


int initialiseScreen(RenderFrame *ScreenData, int width, int height, bool Fullscreen);

bool SetWindowIcon(const char fileName[]);

bool SetWindowTitle(const char newTitle[]);

int setVsync(bool enabled);

int RenderSDL(World *GameWorld);

int cleanUpSDLRenderer(RenderFrame ScreenData);


int resizeSDLWindow();


void printConsoleData();

void clearConsoleString();

int putConsoleString(const char input[], ...);

int putConsoleInteger(int input);

int putConsoleStrStr(const char input1[], const char input2[]);

int putConsoleStrInt(const char strInput[], int intInput);

int putConsoleStrIntStr(const char strInput1[], int intInput, const char strInput2[]);

int putConsoleFloat(float input);

int putConsoleStrFloat(const char strInput[], float floatInput);


int putConsoleDouble(double input);

int putConsoleStrDouble(const char strInput[], double doubleInput);


int clamp(int input, int lowerBound, int upperBound);

double dClamp(double input, double lowerBound, double upperBound);

float fClamp(float input, float lowerBound, float upperBound);

bool inRange(int input, int low, int high);

bool inRangeExclusive(int input, int low, int high);

int max(int left, int right);

int modulo(int x, int N);

float fModulo(float x, float N);


void stringToUpper(char input[]);

void stringToLower(char input[]);


int PickRandomIntBetween(int low, int high);

float PickRandomFloatBetween(float low, float high);


int sparseInitialise(IntSparseList *input);

int sparseAdd(int input, IntSparseList *List);

int sparseRemove(int input, IntSparseList *List);

int sparseGet(int input, IntSparseList *List);

int stackAdd(int input, StackArray *List);

int stackPop(StackArray *List);

int stackRemove(int input, StackArray *List);


void clearString(String *input);

void setString(String *input, const char stringInput[]);

void freeString(String *input);

void copyString(String source, String *destination);

void concatString(String *string1, String string2);

void concatStringCStr(String *string1, const char *string2);

void setStringUpper(String input);

void setStringLower(String input);

bool stringEquals(String input1, String input2);

bool stringContains(String input, String sub);

char at(String input, int index);

bool stringEqualsCString(String input1, const char input2[]);

void printString(String input);

void printStringLine(String input);