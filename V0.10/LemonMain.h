EXPORT int RunLemonEngine(void);

EXPORT int StartUpLemonEngine(void);

EXPORT int MainLoop(World *GameWorld);

EXPORT int CloseGame(World *GameWorld, RenderFrame *ScreenData);


EXPORT FuncResult CheckResourceData(void);

EXPORT int initialiseWorld(World *GameWorld);

EXPORT int initialiseBackGround(BackgroundData *input);

EXPORT ObjectController* createObjectController(void);

EXPORT void destroyWorld(World *GameWorld);


EXPORT int getExternalInput(World *GameWorld, SDL_Renderer *screen);

EXPORT int GameTick(World *GameWorld);

EXPORT int GameFrame(World *GameWorld);

EXPORT int Render(World *GameWorld, RenderFrame *ScreenData);

EXPORT void RenderEngine(Camera renderCamera, World *GameWorld, SDL_Renderer *Screen);


EXPORT Uint64 TickNumber(void);

EXPORT int FPSCounter(World *GameWorld);


EXPORT void initialiseCameraViews(CameraView list[VIEW_COUNT]);

EXPORT CameraView* addCameraViewToList(float camX, float camY, int camWidth, int camHeight, float viewPosX, float viewPosY, float width, float height, Layer drawLayer, bool useMain, CameraView list[VIEW_COUNT]);

EXPORT CameraView* addCameraView(float camX, float camY, int camWidth, int camHeight, float viewX, float viewY, float viewWidth, float viewHeight, Layer drawLayer, World *GameWorld);

EXPORT CameraView* addMainCameraView(float viewX, float viewY, float width, float height, Layer drawLayer, World *GameWorld);

EXPORT void attachCameraViewToObject(CameraView *input, Object *attach);

EXPORT CameraView* getCameraView(World *GameWorld, int id);

EXPORT void printCameraViewInfo(CameraView list[VIEW_COUNT]);

EXPORT void removeCameraView(CameraView *input);

EXPORT void removeAllCameraViewsFromList(CameraView list[VIEW_COUNT]);

EXPORT void removeAllCameraViews(World *GameWorld);

EXPORT void renderCameraViews(Camera mainCam, World *GameWorld, SDL_Renderer *Screen, Layer drawLayer);


EXPORT void addGameFlag(const char name[], int startValue);

EXPORT int checkGameFlag(const char name[]);

EXPORT int getGameFlag(const char name[]);

EXPORT void setGameFlag(const char name[], int newValue);

EXPORT void changeGameFlagBy(const char name[], int value);


EXPORT void putConsole(const char input[], ...);

EXPORT void putConsoleTS(const char input[], ...);

EXPORT void putConsoleError(const char input[], ...);

EXPORT void addMessageHistory(const char input[], MessageHistory *history);

EXPORT char* getPreviousMessageHistory(MessageHistory *history);

EXPORT char* getNextMessageHistory(MessageHistory *history);


EXPORT void initialiseChatLog(ChatLog *chat);

EXPORT void addMessageToChatLog(const char msg[], int ID, Uint64 tickSent);


EXPORT int ResetCamera(Camera *inputCam);

EXPORT int setCameraPos(Camera *input, float xPos, float yPos);

EXPORT void SetEngineSettingsToDefault(void);

EXPORT void SetRenderSettingsToDefault(void);

EXPORT void SetTextSettingsToDefault(void);

EXPORT void SetDebugSettingsToDefault(void);


EXPORT int getKeyboardInput(SDL_KeyboardEvent *key);

EXPORT void updateCustomKeys(void);

EXPORT bool buttonPressed(int key);

EXPORT bool keyPressed(int key);

EXPORT bool buttonHeld(int key);

EXPORT bool keyHeld(int key);

EXPORT void ClearInput(void);

EXPORT void updateMousePos();

EXPORT float getMouseXCam(Camera inputCamera);

EXPORT float getMouseYCam(Camera inputCamera);

EXPORT int getMouseInput(SDL_MouseButtonEvent *event);

EXPORT int getGamepadInput(SDL_GamepadButtonEvent *event);

EXPORT void updateGamepadAxis(SDL_GamepadAxisEvent *event);


EXPORT void AcknowledgeHeldButtons(void);

EXPORT void AcknowledgeMouse(void);

EXPORT void AcknowledgeButton(LemonKeys Key);


EXPORT int setTickRate(int desiredTickRate);


EXPORT int setRenderRefreshRate(int desiredRenderRate);


EXPORT int SetGravity(World *GameWorld, float force, float directionDegrees);


EXPORT void MasterControls(World *GameWorld, SDL_Window *window);


EXPORT int initialiseScreen(RenderFrame *ScreenData, int width, int height, bool Fullscreen);

EXPORT bool SetWindowIcon(const char fileName[]);

EXPORT bool SetWindowTitle(const char newTitle[]);

EXPORT int setVsync(bool enabled);

EXPORT int cleanUpSDLRenderer(RenderFrame *ScreenData);


EXPORT int clamp(int input, int lowerBound, int upperBound);

EXPORT double dClamp(double input, double lowerBound, double upperBound);

EXPORT float fClamp(float input, float lowerBound, float upperBound);

EXPORT bool inRange(int input, int low, int high);

EXPORT bool inRangeExclusive(int input, int low, int high);

EXPORT int max(int left, int right);

EXPORT int min(int left, int right);

EXPORT int modulo(int x, int N);

EXPORT float fModulo(float x, float N);


EXPORT void stringToUpper(char input[]);

EXPORT void stringToLower(char input[]);

EXPORT int LemonStrncpy(char dest[], const char source[], int capacity);

EXPORT void removeChar(char string[], char remove, int capacity);


EXPORT int PickRandomIntBetween(int low, int high);

EXPORT float PickRandomFloatBetween(float low, float high);


EXPORT int sparseInitialise(IntSparseList *input);

EXPORT int sparseAdd(int input, IntSparseList *List);

EXPORT int sparseRemove(int input, IntSparseList *List);

EXPORT int sparseGet(int input, IntSparseList *List);

EXPORT int stackAdd(int input, StackArray *List);

EXPORT int stackPop(StackArray *List);

EXPORT int stackRemove(int input, StackArray *List);


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