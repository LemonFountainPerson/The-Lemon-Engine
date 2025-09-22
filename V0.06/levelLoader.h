#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "spriteLoader.h"
#include "playerController.h"
#include "soundProcessor.h"
#include "UIObjects.h"
#include "LemonMain.h"
#endif


int loadDefault(World *GameWorld);

int loadLevelData(World *GameWorld, int level);

int checkFileHeader(FILE *fPtr, const char FileType[]);

int clearCurrentlyLoadedLevelData(World *GameWorld);

int loadLevel(World *GameWorld, int level);

int logLevel(World *GameWorld);

int lemon_Modulo(int x, int N);

FILE* convertTxtToLem(char FileName[MAX_LEN], FILE *txtPtr);

FILE* convertLemToTxt(char fileName[MAX_LEN + 4], FILE *lemPtr);


int loadObject(World *GameWorld, FILE *fPtr, int xOffset, int yOffset);

int loadRepeatingObject(World *GameWorld, FILE *fPtr);

int loadMovingPlatform(World *GameWorld, FILE *fPtr);

int loadLevelFlag(World *GameWorld, FILE *fPtr);

int ConvertEntryToObjectID(char entry[MAX_LEN]);

int ApplyObjectLoadCommands(Object *inputObject, char commands[32]);


int readIntArgs(FILE *fPtr, int argsDest[], int number);

int getNextArg(FILE *fPtr, char buffer[], int max);

int convertStrToInt(char str[], int size);

int convertIntToStr(char str[], int input);

int inRange(int input, int low, int high);