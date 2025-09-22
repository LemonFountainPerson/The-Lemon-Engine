#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "LemonMain.h"
#endif



// Returns tile at camera position given
//int getTileAtCamera(int levelData[][GRID_HEIGHT], char tileShape[], int x, int y);

// Draws tiles from gameWorld->levelData to screen array
//int drawTiles(uint32_t screen[], int width, int height, World *gameWorld);


// Draws objects from gameWorld to screen array
int drawObjects(Camera inputCamera, World *GameWorld);


int drawHitboxes(Camera inputCamera, World *GameWorld);


int renderHitbox(Camera inputCamera, World *gameWorld, PhysicsRect *inputBox);


int renderObjectSprite(Camera inputCamera, World *gameWorld, Object *currentObject);


int renderObjectSprite_MultiThreaded(Camera inputCamera, World *gameWorld, Object *currentObject);



// Draws and positions player relative to camera position in gameWorld
int drawPlayerHitboxes(Camera inputCamera, World *gameWorld);


int WorldCameraControl(World *GameWorld, Camera *inputCamera);


int restrictCameraToBounds(Camera *inputCamera);


int renderBackGroundSprite(Camera inputCamera, BackgroundData WorldBackground);


int ScaleSpriteInBuffer(Sprite *inputData, int width, int height, unsigned char destData[]);


int ScaleRotateSpriteInBuffer(Sprite *inputData, double direction, int width, int height, unsigned char destData[], int scaleWidth, int scaleHeight);


int rotateSprite(Sprite *inputData, double direction, int width, int height, unsigned char destData[]);


double rotateX(double sinVal, double cosVal, double x, double y);


double rotateY(double sinVal, double cosVal, double x, double y);


uint32_t blendPixel(uint32_t screenPixel, uint32_t inputPixel, float transparency);



int renderSprite_LRUD_FullAlpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect);


int renderSprite_RLUD_FullAlpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect);


int renderSprite_LRDU_FullAlpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect);


int renderSprite_RLDU_FullAlpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect);




int renderSprite_LRUD_Tile(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_RLUD_Tile(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRDU_Tile(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_RLDU_Tile(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRUD_TileFast(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);


int renderSprite_LRDU_TileFast(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset);




int renderSprite_LRUD_Scale(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);


int renderSprite_RLUD_Scale(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);


int renderSprite_LRDU_Scale(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);


int renderSprite_RLDU_Scale(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox);




int renderSprite_LRUD_Scale_Full_Alpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect, PhysicsRect *inputBox);


int renderSprite_RLUD_Scale_Full_Alpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect, PhysicsRect *inputBox);


int renderSprite_LRDU_Scale_Full_Alpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect, PhysicsRect *inputBox);


int renderSprite_RLDU_Scale_Full_Alpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect, PhysicsRect *inputBox);

