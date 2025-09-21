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


int GameTick(World *GameWorld);


int GameFrame(World *GameWorld);


int RenderEngine(World *GameWorld);


FunctionResult InitialiseGame(World *GameWorld);


int ResetCamera(Camera *inputCam);


int SetEngineSettingsToDefault(void);


int SetRenderSettingsToDefault(void);


int SetDebugSettingsToDefault(void);


void clearGameData(World *GameWorld);


ObjectController* createObjectController(void);


int LoadBackGroundSprites(BackgroundData *WorldBackground);


int getExternalInput(World *GameWorld);


int updateMousePosition(void);


int getMouseInput(SDL_MouseButtonEvent event);


int getKeyboardInput(SDL_Event *event);


int ClearKeyboardInput();


int setTickRate(int desiredTickRate);


int setRenderRefreshRate(int desiredRenderRate);


void MasterControls(World *GameWorld, PlayerData *player);


int printDebugData();


int printDebugDataNoHeader();


int clearDebugString();


int putDebugString(const char input[]);


int putDebugInteger(int input);


int putDebugStrStr(const char input1[], const char input2[]);


int putDebugStrInt(const char strInput[], int intInput);


int putDebugStrIntStr(const char strInput1[], int intInput, const char strInput2[]);


int putDebugDouble(double input);


int putDebugStrDouble(const char strInput[], double doubleInput);


void frameRate(int milliseconds, clock_t gameTick);


int SetGravity(World *GameWorld, float force, double directionDegrees);


int RenderSDL(World *GameWorld, SDL_Window *Window, SDL_Renderer *Renderer, SDL_Surface *Surface);


int cleanUpSDLRenderer(SDL_Window *Window, SDL_Renderer *Renderer, SDL_Surface *Surface);


int resizeSDLWindow();


int clamp(int input, int lowerBound, int upperBound);


double dClamp(double input, double lowerBound, double upperBound);


int modulo(int x, int N);


void stringToLower(char input[]);