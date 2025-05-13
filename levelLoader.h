#ifndef IS_DEFINED
#include "data.h"
#endif


void insertHLine(int level[GRID_WIDTH][GRID_HEIGHT], int i, int j, int itemID, int length);

void insertVLine(int level[GRID_WIDTH][GRID_HEIGHT], int i, int j, int itemID, int length);


int loadDefault(World *gameWorld);

int loadLevel(World *gameWorld, int level);

int switchLevel(World *gameWorld, int level);

int saveLevel(World *gameWorld);

int convertTxtToLem(char FileName[MAX_LEN], FILE *txtPtr);


int loadObject(World *gameWorld, FILE *fPtr);

int loadObjectOffset(World *gameWorld, FILE *fPtr, int xOffset, int yOffset);

int loadRepeatingObject(World *gameWorld, FILE *fPtr);

int loadMovingPlatform(World *gameWorld, FILE *fPtr);

int loadLevelFlag(World *gameWorld, FILE *fPtr);


int getNextArg(FILE *fPtr, char buffer[], int max, int convertToInteger);

int convertStrToInt(char str[], int size);

int convertIntToStr(char str[], int input);

int inRange(int input, int low, int high);