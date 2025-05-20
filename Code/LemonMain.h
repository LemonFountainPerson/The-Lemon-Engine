#ifndef IS_DEFINED
#include "data.h"
#include "drawScreen.h"
#include "gameObjects.h"
#include "playerController.h"
#include "levelLoader.h"
#include "spriteLoader.h"
#include "soundProcessor.h"
#endif


int RunLemonEngine(void);


World* initialiseGame(PlayerData *player);


int PutScreenOnWindow(RenderFrame frame, SDL_Renderer *Renderer, SDL_Surface *screenSurface, SDL_Texture *texture);


int getKeyboardInput(SDL_Event *event, int keyboard[256]);


void DebugControls(World *GameWorld, PlayerData *player, int keyboard[256], Object *testObject);


void frameRate(int milliseconds, clock_t gameTick);


void clearGameData(World *gameWorld, PlayerData *player);


int CleanUpWindowRenderer(SDL_Window *Window, SDL_Renderer *Renderer, SDL_Surface *screenSurface, SDL_Texture *texture);
