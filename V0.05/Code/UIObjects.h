#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "animations.h"
#include "spriteLoader.h"
#include "soundProcessor.h"
#include "playerController.h"
#include "eventManager.h"
#endif



int SpawnHUD(World *GameWorld);


int InitialiseUIElement(World *GameWorld, Object *UIElement);


int LoadUISprites(SpriteSet *newSet);


int UpdateUIElement(World *GameWorld, Object *UIElement, int keyboard[256]);


int UpdateTextBox(Object *textBox, int keyboard[256]);


int PauseMenu(Object *MenuController, World *GameWorld, int keyboard[256]);


int SettingsMenu(Object *MenuController, World *GameWorld, int keyboard[256]);


int UpdateOptionButton(Object *Button);