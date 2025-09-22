#ifndef IS_DEFINED
#include "data.h"
#include "drawScreen.h"
#include "gameObjects.h"
#include "playerController.h"
#include "animations.h"
#include "levelLoader.h"
#include "eventManager.h"
#include "spriteLoader.h"
#include "soundProcessor.h"
#endif


int RunLemonEngine(void);


int GameTick(World *GameWorld, int keyboard[256]);


World* InitialiseGame(PlayerData *player);


int LoadBackGroundSprites(World *GameWorld);


int putScreenOnWindow(uint32_t *screenBuffer, SDL_Renderer *Renderer, SDL_Surface *screenSurface, SDL_Texture *texture);


int getKeyboardInput(SDL_Event *event, int keyboard[256]);


int ClearKeyboardInput(int keyboard[]);


void MasterControls(World *GameWorld, int keyboard[256], PlayerData *player, Object *testObject);


void frameRate(int milliseconds, clock_t gameTick);


void clearGameData(World *gameWorld, PlayerData *player, uint32_t screen[]);


int cleanUpWindowRenderer(SDL_Window *Window, SDL_Renderer *Renderer, SDL_Surface *screenSurface, SDL_Texture *texture);

