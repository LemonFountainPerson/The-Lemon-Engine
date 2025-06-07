#ifndef SPRITELOADER_H_
#define SPRITELOADER_H_
#endif

#ifndef IS_DEFINED
#include "data.h"
#endif


int switchBackGroundSprite(int spriteID, int desiredSetID, World *gameWorld);


int switchBackGroundSpriteName(char spriteName[], int desiredSetID, World *gameWorld);


int loadBackGroundSprite(char spriteName[], int desiredSetID, int tileMode, World *gameWorld);


int loadObjectSprite(char spriteName[], SpriteSet *inputSet, int tileMode);


int loadPlayerSprite(char spriteName[], int desiredSetID, int tileMode, PlayerData *player);


void deleteSprite(SpriteSet *spriteSet, Sprite **input);
