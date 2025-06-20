#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "spriteloader.h"
#include "LemonMain.h"
#endif


int UpdateFlagObject(World *GameWorld, Object* inputObject);


int PauseGame(World *GameWorld, int keyboard[256]);


int ResumeGame(World *GameWorld);


int OpenSettings(World *GameWorld, int keyboard[256]);


int PlayCutscene(World *GameWorld, int keyboard[256]);


int StartCutscene(World *GameWorld);