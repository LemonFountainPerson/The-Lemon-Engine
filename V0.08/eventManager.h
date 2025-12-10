#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "spriteloader.h"
#include "LemonMain.h"
#include "levelLoader.h"
#include "UIObjects.h"
#include "cutsceneManager.h"
#include "drawScreen.h"
#endif


int StartGame(World *GameWorld);


int HandleGameWorldEvents(World *GameWorld, RenderFrame *ScreenData);

int validateZoom(RenderFrame *ScreenData);


int PauseGame(World *GameWorld);


int ResumeGame(World *GameWorld);


int InitialiseLevelFlag(Object *inputObject, ObjectController *ObjectList);


int UpdateFlagObject(Object* inputObject, PlayerData *Player, World *GameWorld);
