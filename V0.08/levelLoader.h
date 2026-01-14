#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "spriteLoader.h"
#include "playerController.h"
#include "soundProcessor.h"
#include "UIObjects.h"
#include "LemonMain.h"
#include "cutsceneManager.h"
#endif


int loadLevel(World *GameWorld, int level);

int loadPartition(World *GameWorld, int sceneID);

int loadLevelData(World *GameWorld, FILE *fPtr);

int loadLevelDataChunk(World *GameWorld, FILE *fPtr, int lineLimit);

int checkFileHeader(FILE *fPtr, const char FileType[]);

FILE* openFile(const char fileName[], const char rootPath[], const char header[]);

int getCurrentLineNumber(FILE *fPtr);

int skipCommentInFile(FILE *fPtr, int maxLength);

int clearLevelData(World *GameWorld);

int logLevel(World *GameWorld);

int lemon_Modulo(int x, int N);

FILE* convertTxtToLem(char FileName[MAX_LEN], FILE *txtPtr);

FILE* convertLemToTxt(char fileName[MAX_LEN + 4], FILE *lemPtr);


int loadObject(World *GameWorld, FILE *fPtr, int xOffset, int yOffset);

int loadRepeatingObject(World *GameWorld, FILE *fPtr, int *objectsLoaded);

int loadMovingPlatform(World *GameWorld, FILE *fPtr);

int loadLevelFlag(World *GameWorld, FILE *fPtr);

int ConvertEntryToObjectID(char entry[MAX_LEN]);

int CheckObjectLoadCommands(FILE *fPtr, Object *inputObject);

int ApplyObjectLoadCommand(Object *inputObject, char command[MAX_LEN]);


int atEndOfLine(FILE *fPtr);

int readIntArgs(FILE *fPtr, int argsDest[], int number);

int getNextArg(FILE *fPtr, char buffer[], int capacity);

int getNextArgInt(FILE *fPtr);

float getNextArgFloat(FILE *fPtr);

int convertStrToInt(char str[], int size);

int convertIntToStr(char str[], int input);

bool inRange(int input, int low, int high);



int loadTestScene(World *GameWorld);
