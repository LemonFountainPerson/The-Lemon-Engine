#ifndef IS_DEFINED
#include "data.h"
#include "spriteLoader.h"
#include "LemonMain.h"
#include "levelLoader.h"
#endif


int LoadAnimations(SpriteSet *newSet, int ObjectID);


int loadAnimationsFromFile(const char FileName[], SpriteSet *destSet);


int LoopParticleAnimation(Object *particle);


int PlayAnimation(const char desiredName[], int loopCount, DisplayData *inputData);


int PlayNewAnimation(const char desiredName[], int loopCount, DisplayData *inputData);


int iterateAnimation(DisplayData *inputData);


Animation* initialiseNewAnimation(const char animationName[], int frameRate, SpriteSet *inputSet);


int addSpriteToAnimation(const char spriteName[], Animation *inputAnimation, SpriteSet *sourceSet);


int deleteAnimation(Animation *deleteAnimation);