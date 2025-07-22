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


int GameTick(World *GameWorld, int keyboard[256]);


int RenderEngine(World *GameWorld, uint32_t *screenBuffer);


World* InitialiseGame();


int LoadBackGroundSprites(World *GameWorld);


int putScreenOnWindow(uint32_t *screenBuffer, SDL_Window *Window, SDL_Surface *destSurface, SDL_Surface *screenSurface);


int getKeyboardInput(SDL_Event *event, int keyboard[256]);


int ClearKeyboardInput(int keyboard[]);


void MasterControls(World *GameWorld, int keyboard[256], PlayerData *player);


void frameRate(int milliseconds, clock_t gameTick);


void clearGameData(World *GameWorld, uint32_t screen[]);


int RenderSDL(World *GameWorld, uint32_t *screenBuffer, SDL_Window *Window, SDL_Surface *destSurface, SDL_Surface *screenSurface);


int cleanUpWindowRenderer(SDL_Window *Window, SDL_Surface *destSurface, SDL_Surface *screenSurface);


int clamp(int input, int lowerBound, int upperBound);


int modulo(int x, int N);


void stringToLower(char input[]);