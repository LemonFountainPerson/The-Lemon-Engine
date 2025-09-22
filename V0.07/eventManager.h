#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "spriteloader.h"
#include "LemonMain.h"
#include "levelLoader.h"
#include "UIObjects.h"
#include "cutsceneManager.h"
#endif


int StartGame(World *GameWorld);


int HandleGameWorldEvents(World *GameWorld);


int PauseGame(World *GameWorld);


int ResumeGame(World *GameWorld);


int OpenSettings(World *GameWorld);


int InitialiseLevelFlag(Object *inputObject, ObjectController *ObjectList);


int UpdateFlagObject(World *GameWorld, Object* inputObject);
