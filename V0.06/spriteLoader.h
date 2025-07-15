#ifndef IS_DEFINED
#include "data.h"
#include "animations.h"
#endif



int switchBackGroundSprite(int spriteID, int desiredSetID, World *gameWorld);


int switchBackGroundSpriteName(const char spriteName[], int desiredSetID, World *gameWorld);


int loadSpriteFromPath(Sprite *inputSprite, char path[]);


SpriteSet* setSourceToDesiredSpriteSet(SpriteSet **firstSet, int desiredSetID);


int flipRAndBChannels(Sprite *inputSprite);


// Loads sprite into spriteset determined from desiredSetID, searched for at spriteSetSource
int loadSprite(const char spriteName[], const char folderName[], SpriteSet **spriteSetSource, int desiredSetID, RenderMode renderMode);


// Loads sprite directly into provided spriteSet (inputSet)
int loadSpriteIntoSpriteSet(const char spriteName[], const char folderName[], SpriteSet *inputSet, RenderMode renderMode);


int switchSprite(int spriteID, int spriteSet, DisplayData *inputData);


int switchSpriteByName(const char spriteName[MAX_LEN], int spriteSet, DisplayData *inputData);


int loadBackGroundSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, World *gameWorld);


int loadObjectSprite(const char spriteName[], SpriteSet *inputSet, RenderMode renderMode);


int loadPlayerSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, PlayerData *player);


int deleteSprite(SpriteSet *spriteSet, Sprite **input);


int deleteSpriteSet(SpriteSet *inputSet);