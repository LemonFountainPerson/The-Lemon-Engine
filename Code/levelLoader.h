#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "spriteLoader.h"
#include "playerController.h"
#endif


int loadDefault(World *gameWorld);

int loadLevel(World *gameWorld, int level);

int clearCurrentlyLoadedLevelData(World *gameWorld);

int switchLevel(World *gameWorld, int level);

int saveLevel(World *gameWorld);

int convertTxtToLem(char FileName[MAX_LEN], FILE *txtPtr);

int convertLemToTxt(char fileName[MAX_LEN + 4], FILE *lemPtr);


int loadObject(World *gameWorld, FILE *fPtr);

int loadObjectOffset(World *gameWorld, FILE *fPtr, int xOffset, int yOffset);

int loadRepeatingObject(World *gameWorld, FILE *fPtr);

int loadMovingPlatform(World *gameWorld, FILE *fPtr);

int loadLevelFlag(World *gameWorld, FILE *fPtr);


int readMultipleIntArgs(FILE *fPtr, int argsBuffer[], int number, int argMaxLength);

int getNextArg(FILE *fPtr, char buffer[], int max);

int convertStrToInt(char str[], int size);

int convertIntToStr(char str[], int input);

int inRange(int input, int low, int high);