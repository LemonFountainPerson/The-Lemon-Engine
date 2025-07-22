#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "LemonMain.h"
#endif



// Returns tile at camera position given
//int getTileAtCamera(int levelData[][GRID_HEIGHT], char tileShape[], int x, int y);

// Draws tiles from gameWorld->levelData to screen array
//int drawTiles(uint32_t screen[], int width, int height, World *gameWorld);


// Clears screen array for new frame, and cleans up extra renderer data
int cleanRenderer(World *gameWorld, uint32_t *screen);


// Draws objects from gameWorld to screen array
int drawObjects(uint32_t *screen, Camera inputCamera, World *gameWorld);


int renderObjectHitbox(uint32_t *screen, Camera inputCamera, World *gameWorld, Object *currentObject, Layer drawLayer);


int renderObjectSprite(uint32_t *screen, Camera inputCamera, World *gameWorld, Object *currentObject, Layer drawLayer);


int renderObjectSprite_MultiThreaded(uint32_t *screen, Camera inputCamera, World *gameWorld, Object *currentObject, Layer drawLayer);



// Draws and positions player relative to camera position in gameWorld
int drawPlayerHitboxes(uint32_t *screen, Camera inputCamera, World *gameWorld);


int WorldCameraControl(World *GameWorld, Camera *inputCamera);


int restrictCameraToBounds(Camera *inputCamera);


uint32_t blendPixel(uint32_t screenPixel, uint32_t inputPixel, float transparency);



int renderBackGroundSprite(uint32_t *screen, Camera inputCamera, World *gameWorld);


int renderSpriteInRenderMode(uint32_t *screen, DisplayData *inputData, int realXOffset, int realYOffset, PhysicsRect *inputBox);


int renderSpriteInRenderMode_MultiThreaded(uint32_t *screen, DisplayData *inputData, int realXOffset, int realYOffset, PhysicsRect *inputBox);



int renderSprite_LRUD_FullAlpha(struct threadRenderData *Data);


int renderSprite_RLUD_FullAlpha(struct threadRenderData *Data);


int renderSprite_LRDU_FullAlpha(struct threadRenderData *Data);


int renderSprite_RLDU_FullAlpha(struct threadRenderData *Data);


int renderSprite_LRUD_FullAlpha_MT(void *Data);


int renderSprite_RLUD_FullAlpha_MT(void *Data);


int renderSprite_LRDU_FullAlpha_MT(void *Data);


int renderSprite_RLDU_FullAlpha_MT(void *Data);



int renderSprite_LRUD_Tile(uint32_t screen[], DisplayData *inputData, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_RLUD_Tile(uint32_t screen[], DisplayData *inputData, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRDU_Tile(uint32_t screen[], DisplayData *inputData, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_RLDU_Tile(uint32_t screen[], DisplayData *inputData, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRUD_TileFast(uint32_t screen[], DisplayData *inputData, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRDU_TileFast(uint32_t screen[], DisplayData *inputData, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);




int renderSprite_LRUD_Scale(struct threadRenderData *Data);


int renderSprite_RLUD_Scale(struct threadRenderData *Data);


int renderSprite_LRDU_Scale(struct threadRenderData *Data);


int renderSprite_RLDU_Scale(struct threadRenderData *Data);


int renderSprite_LRUD_Scale_MT(void *Data);


int renderSprite_RLUD_Scale_MT(void *Data);


int renderSprite_LRDU_Scale_MT(void *Data);


int renderSprite_RLDU_Scale_MT(void *Data);




int renderSprite_LRUD_Scale_Full_Alpha(uint32_t screen[], DisplayData *inputData, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);


int renderSprite_RLUD_Scale_Full_Alpha(uint32_t screen[], DisplayData *inputData, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);


int renderSprite_LRDU_Scale_Full_Alpha(uint32_t screen[], DisplayData *inputData, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);


int renderSprite_RLDU_Scale_Full_Alpha(uint32_t screen[], DisplayData *inputData, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);

