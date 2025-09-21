#ifndef IS_DEFINED
#include "data.h"
#include "spriteLoader.h"
#include "LemonMain.h"
#include "levelLoader.h"
#endif


int LoadSpritesAndAnimationData(SpriteSet *newSet, int ObjectID);


int loadAnimationsFromFile(const char FileName[], SpriteSet *destSet);


int PlayAnimation(const char desiredName[], int loopCount, DisplayData *inputData);


int PlayAnimationByIndex(int index, int loopCount, DisplayData *inputData);


int PlayNewAnimation(const char desiredName[], int loopCount, DisplayData *inputData);


int SwitchAnimation(const char desiredName[], int loopCount, DisplayData *inputData);


int PlayAnimationAfterOther(const char desiredName[], const char otherAnim[], int loopCount, DisplayData *inputData);


int PlayAnimationAfterOtherPrefix(const char desiredName[], const char otherPrefix[], int loopCount, DisplayData *inputData);


int iterateAnimation(DisplayData *inputData);


Animation* initialiseNewAnimation(const char animationName[], double frameRate, SpriteSet *inputSet);


AnimationFrame* addSpriteToAnimation(const char spriteName[], Animation *inputAnimation, SpriteSet *sourceSet);


AnimationFrame* addSpriteToAnimationWithAttributes(const char spriteName[], Animation *inputAnimation, SpriteSet *sourceSet, int XOffset, int YOffset, float rotation);


int deleteAnimation(Animation *deleteAnimation);