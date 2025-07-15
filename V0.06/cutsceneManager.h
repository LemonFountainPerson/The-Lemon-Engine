#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "spriteloader.h"
#include "LemonMain.h"
#include "UIObjects.h"
#include "playerController.h"
#endif


int PlayCutscene(CutsceneID inputID, World *GameWorld);


int UpdateCutscene(World *GameWorld, int keyboard[256]);


int StartCutscene(World *GameWorld);


int EndCutscene(World *GameWorld);