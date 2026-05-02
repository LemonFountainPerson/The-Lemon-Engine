EXPORT int RunLemonEngine(void);

EXPORT int StartUpLemonEngine(void);

int MainLoop(World *GameWorld);

EXPORT int CloseGame(World *GameWorld, RenderFrame *ScreenData);


FuncResult CheckResourceData(void);

EXPORT int initialiseWorld(World *GameWorld);

int initialiseBackGround(BackgroundData *input);

ObjectController* createObjectController(void);

EXPORT void destroyWorld(World *GameWorld);


EXPORT int getExternalInput(World *GameWorld, SDL_Renderer *screen);

EXPORT int GameTick(World *GameWorld);

EXPORT int GameFrame(World *GameWorld);

EXPORT int Render(World *GameWorld, RenderFrame *ScreenData);

EXPORT void RenderEngine(Camera *renderCamera, World *GameWorld, SDL_Renderer *Screen);


EXPORT Uint64 TickNumber(void);

int FPSCounter(void);


void initialiseCameraViews(CameraView list[VIEW_COUNT]);

CameraView* addCameraViewToList(float camX, float camY, int camWidth, int camHeight, float viewPosX, float viewPosY, float width, float height, Layer drawLayer, bool useMain, CameraView list[VIEW_COUNT]);

CameraView* addCameraView(float camX, float camY, int camWidth, int camHeight, float viewX, float viewY, float viewWidth, float viewHeight, Layer drawLayer, World *GameWorld);

CameraView* addMainCameraView(float viewX, float viewY, float width, float height, Layer drawLayer, World *GameWorld);

void attachCameraViewToObject(CameraView *input, Object *attach);

CameraView* getCameraView(World *GameWorld, int id);

void printCameraViewInfo(CameraView list[VIEW_COUNT]);

void removeAllCameraViewsFromList(CameraView list[VIEW_COUNT]);

void removeAllCameraViews(World *GameWorld);

void renderCameraViews(CameraView list[VIEW_COUNT], World *GameWorld, SDL_Renderer *Screen, Layer drawLayer);


void addGameFlag(const char name[], int startValue);

int checkGameFlag(const char name[]);

int getGameFlag(const char name[]);

void setGameFlag(const char name[], int newValue);


void putConsoleString(const char input[], ...);

void putConsoleStringTS(const char input[], ...);

void putConsoleError(const char input[], ...);

void addInputHistory(const char input[], InputHistory *history);

char* getPreviousInputHistory(InputHistory *history);

char* getNextInputHistory(InputHistory *history);


int ResetCamera(Camera *inputCam);

int setCameraPos(Camera *input, float xPos, float yPos);

void SetEngineSettingsToDefault(void);

void SetRenderSettingsToDefault(void);

void SetTextSettingsToDefault(void);

void SetDebugSettingsToDefault(void);


int getKeyboardInput(SDL_KeyboardEvent *key);

void updateCustomKeys(void);

bool buttonPressed(int key);

bool keyPressed(int key);

bool buttonHeld(int key);

bool keyHeld(int key);

void ClearInput(void);

void updateMousePos();

float getMouseXCam(Camera inputCamera);

float getMouseYCam(Camera inputCamera);

int getMouseInput(SDL_MouseButtonEvent *event);

int getGamepadInput(SDL_GamepadButtonEvent *event);

void updateGamepadAxis(SDL_GamepadAxisEvent *event);


void AcknowledgeHeldButtons(void);

void AcknowledgeMouse(void);

void AcknowledgeButton(LemonKeys Key);


int setTickRate(int desiredTickRate);


int setRenderRefreshRate(int desiredRenderRate);


int SetGravity(World *GameWorld, float force, float directionDegrees);


void MasterControls(World *GameWorld, SDL_Window *window);


int initialiseScreen(RenderFrame *ScreenData, int width, int height, bool Fullscreen);

bool SetWindowIcon(const char fileName[]);

bool SetWindowTitle(const char newTitle[]);

int setVsync(bool enabled);

int cleanUpSDLRenderer(RenderFrame *ScreenData);


int clamp(int input, int lowerBound, int upperBound);

double dClamp(double input, double lowerBound, double upperBound);

float fClamp(float input, float lowerBound, float upperBound);

bool inRange(int input, int low, int high);

bool inRangeExclusive(int input, int low, int high);

int max(int left, int right);

int min(int left, int right);

int modulo(int x, int N);

float fModulo(float x, float N);


void stringToUpper(char input[]);

void stringToLower(char input[]);

void LemonStrncpy(char dest[], const char source[], int capacity);

void removeChar(char string[], char remove, int capacity);


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