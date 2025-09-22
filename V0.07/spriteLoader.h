#ifndef IS_DEFINED
#include "data.h"
#include "animations.h"
#include "LemonMain.h"
#endif



int switchBackGroundSprite(int spriteID, int desiredSetID, BackgroundData *inputData);


int switchBackGroundSpriteName(const char spriteName[], int desiredSetID, BackgroundData *inputData);


int loadSpriteFromPath(Sprite *inputSprite, char path[]);


SpriteSet* setSourceToDesiredSpriteSet(SpriteSet **firstSet, int desiredSetID);


int flipRAndBChannels(Sprite *inputSprite);


// Loads sprite into spriteset determined from desiredSetID, searched for at spriteSetSource
int loadSprite(const char spriteName[], const char folderName[], SpriteSet **spriteSetSource, int desiredSetID, RenderMode renderMode);


// Loads sprite directly into provided spriteSet (inputSet)
int loadSpriteIntoSpriteSet(const char spriteName[], const char folderName[], SpriteSet *inputSet, RenderMode renderMode);


int switchSprite(int spriteID, int spriteSet, DisplayData *inputData);


int switchSpriteByName(const char spriteName[], int spriteSet, DisplayData *inputData);


int loadBackGroundSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, BackgroundData *inputData);


int loadObjectSprite(const char spriteName[], SpriteSet *inputSet, RenderMode renderMode);


int loadPlayerSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, PlayerData *player);


int deleteSprite(SpriteSet *spriteSet, Sprite **input);


int deleteSpriteSet(SpriteSet *inputSet, ObjectController *ObjectList);


int deleteExcessSpriteSets(ObjectController *ObjectList, int numberToKeep);


RenderMode convertStringToRenderMode(char string[]);