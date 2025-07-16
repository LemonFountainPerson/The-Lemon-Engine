#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "spriteLoader.h"
#include "playerController.h"
#include "soundProcessor.h"
#include "UIObjects.h"
#endif


int loadDefault(World *gameWorld);

int loadLevelData(World *gameWorld, int level);

int checkFileHeader(FILE *fPtr, const char FileType[]);

int clearCurrentlyLoadedLevelData(World *gameWorld);

int loadLevel(World *gameWorld, int level);

int saveLevel(World *gameWorld);

int lemon_Modulo(int x, int N);

FILE* convertTxtToLem(char FileName[MAX_LEN], FILE *txtPtr);

FILE* convertLemToTxt(char fileName[MAX_LEN + 4], FILE *lemPtr);


int loadObject(World *gameWorld, FILE *fPtr, int xOffset, int yOffset);

int loadRepeatingObject(World *gameWorld, FILE *fPtr);

int loadMovingPlatform(World *gameWorld, FILE *fPtr);

int loadLevelFlag(World *gameWorld, FILE *fPtr);

int ConvertEntryToObjectID(char entry[60]);

int ApplyObjectLoadCommands(Object *inputObject, char commands[32]);


int readIntArgs(FILE *fPtr, int argsDest[], int number);

int getNextArg(FILE *fPtr, char buffer[], int max);

int convertStrToInt(char str[], int size);

int convertIntToStr(char str[], int input);

int inRange(int input, int low, int high);