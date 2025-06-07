#ifndef IS_DEFINED
#include "data.h"
#endif



// Returns tile at camera position given
//int getTileAtCamera(int levelData[][GRID_HEIGHT], char tileShape[], int x, int y);

// Draws tiles from gameWorld->levelData to screen array
//int drawTiles(uint32_t screen[], int width, int height, World *gameWorld);


// Clears screen array for new frame, and cleans up extra renderer data
int cleanRenderer(World *gameWorld, uint32_t screen[], int width, int height);


// Draws objects from gameWorld to screen array
int drawObjects(uint32_t screen[], int width, int height, World *gameWorld);


int renderObject(World *gameWorld, Object *currentObject, uint32_t screen[], int width, int height, Layer drawLayer);


int renderObjectSprite(uint32_t screen[], int screenWidth, int screenHeight, World *gameWorld, Object *currentObject);


int findElement(int array[], int element, int size);


int clamp(int offset, int bound1, int bound2);


int modulo(int x, int N);

// Draws and positions player relative to camera position in gameWorld
int drawPlayer(uint32_t screen[], int width, int height, World *gameWorld);


int WorldCameraControl(int width, int height, PlayerData *player, World *gameWorld);



uint32_t blendPixel(uint32_t screenPixel, uint32_t inputPixel);



int renderBackGroundSprite(uint32_t screen[], int screenWidth, int screenHeight, World *gameWorld);


int renderSprite_LRUD_Tile(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_RLUD_Tile(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRDU_Tile(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_RLDU_Tile(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRUD_Scale(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject);


int renderSprite_RLUD_Scale(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject);


int renderSprite_LRDU_Scale(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject);


int renderSprite_RLDU_Scale(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject);


int renderSprite_LRUD_Scale_Slow(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject);



int renderSprite_LRUD_TileFast(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRDU_TileFast(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);



int renderSprite_LRUD_FullAlpha(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_RLUD_FullAlpha(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRDU_FullAlpha(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_RLDU_FullAlpha(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);
