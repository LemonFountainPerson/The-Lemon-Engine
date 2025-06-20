#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#endif



// Returns tile at camera position given
//int getTileAtCamera(int levelData[][GRID_HEIGHT], char tileShape[], int x, int y);

// Draws tiles from gameWorld->levelData to screen array
//int drawTiles(uint32_t screen[], int width, int height, World *gameWorld);


// Clears screen array for new frame, and cleans up extra renderer data
int cleanRenderer(World *gameWorld, uint32_t *screen);


// Draws objects from gameWorld to screen array
int drawObjects(uint32_t *screen, World *gameWorld);


int renderObjectHitbox(uint32_t *screen, World *gameWorld, Object *currentObject, Layer drawLayer);


int renderObjectSprite(uint32_t *screen, World *gameWorld, Object *currentObject, Layer drawLayer);


int findElement(int array[], int element, int size);


int clamp(int input, int lowerBound, int upperBound);


int modulo(int x, int N);

// Draws and positions player relative to camera position in gameWorld
int drawPlayer(uint32_t *screen, World *gameWorld);


int WorldCameraControl(World *GameWorld);


int restrictCameraToBounds(World *GameWorld);


uint32_t blendPixel(uint32_t screenPixel, uint32_t inputPixel);



int renderBackGroundSprite(uint32_t *screen, World *gameWorld);


int renderSpriteInRenderMode(uint32_t *screen, DisplayData *inputData, int realXOffset, int realYOffset, PhysicsRect *inputBox);


int renderSprite_LRUD_FullAlpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_RLUD_FullAlpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRDU_FullAlpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_RLDU_FullAlpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRUD_Tile(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_RLUD_Tile(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRDU_Tile(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_RLDU_Tile(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRUD_TileFast(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRDU_TileFast(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);



int renderSprite_LRUD_Scale(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);


int renderSprite_RLUD_Scale(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);


int renderSprite_LRDU_Scale(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);


int renderSprite_RLDU_Scale(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);


int renderSprite_LRUD_Scale_Full_Alpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);


int renderSprite_RLUD_Scale_Full_Alpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);


int renderSprite_LRDU_Scale_Full_Alpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);


int renderSprite_RLDU_Scale_Full_Alpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);

