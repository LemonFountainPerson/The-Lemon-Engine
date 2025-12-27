#ifndef IS_DEFINED
#include "data.h"
#include "spriteLoader.h"
#include "soundProcessor.h"
#include "levelLoader.h"
#include "drawScreen.h"
#include "gameObjects.h"
#include "playerController.h"
#include "animations.h"
#include "eventManager.h"
#include "cutsceneManager.h"
#include "UIObjects.h"
#endif


int RunLemonEngine(void);

int StartUpLemonEngine();

int MainLoop(World *GameWorld);

int CloseGame(World *GameWorld);


int initialiseWorld(World *GameWorld);

int LoadBackGroundSprites(BackgroundData *WorldBackground);

ObjectController* createObjectController(void);

void clearGameData(World *GameWorld);


int GameTick(World *GameWorld);


int GameFrame(World *GameWorld);


int RenderEngine(World *GameWorld, Camera renderCamera, RenderFrame ScreenData);


int FPSCounter(void);


int ResetCamera(Camera *inputCam);

int SetEngineSettingsToDefault(void);

int SetRenderSettingsToDefault(void);

int SetDebugSettingsToDefault(void);


FuncResult CheckResourceData(void);


int getExternalInput(World *GameWorld);


int updateMouse(void);

float getMouseXZoom(Camera inputCamera);

float getMouseYZoom(Camera inputCamera);

int getMouseInput(SDL_MouseButtonEvent event);


int getKeyboardInput(SDL_Event *event);


int ClearKeyboardInput();


int AcknowledgeHeldButtons();


int AcknowledgeButton(LemonKeys Key);


int setTickRate(int desiredTickRate);


int setRenderRefreshRate(int desiredRenderRate);


int SetGravity(World *GameWorld, float force, double directionDegrees);


void MasterControls(World *GameWorld, PlayerData *player);


int initialiseScreen(RenderFrame *ScreenData, int width, int height, bool Fullscreen);

bool SetWindowIcon(const char fileName[]);

bool SetWindowTitle(const char newTitle[]);

int RenderSDL(World *GameWorld);

int cleanUpSDLRenderer(RenderFrame ScreenData);


int resizeSDLWindow();


void printConsoleData();


void clearConsoleString();


int putConsoleString(const char input[]);


int putConsoleInteger(int input);


int putConsoleStrStr(const char input1[], const char input2[]);


int putConsoleStrInt(const char strInput[], int intInput);


int putConsoleStrIntStr(const char strInput1[], int intInput, const char strInput2[]);


int putConsoleDouble(double input);


int putConsoleStrDouble(const char strInput[], double doubleInput);


int clamp(int input, int lowerBound, int upperBound);

double dClamp(double input, double lowerBound, double upperBound);

float fClamp(float input, float lowerBound, float upperBound);


int modulo(int x, int N);

float fModulo(float x, float N);


void stringToLower(char input[]);


int PickRandomIntBetween(int low, int high);

float PickRandomFloatBetween(float low, float high);