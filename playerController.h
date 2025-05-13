#ifndef IS_DEFINED
#include "data.h"
#endif


// Initialises an instance of the player
PlayerData* initialisePlayer(World *gameWorld);


// Main player update script
int updatePlayer(PlayerData *player, World *gameWorld, int keyboard[256], double deltaTime);


int playerJump(PlayerData *player, int hAxis, int vAxis);


int setSprite(PlayerData *player);



int objectCollisionX(PlayerData *player, World *gameWorld);


int ApplyXPhysics(PlayerData *player, Object *inputObject);


int objectCollisionY(PlayerData *player, World *gameWorld);


int ApplyYPhysics(PlayerData *player, Object *inputObject);


int overlapsBox(PlayerData *player, double X1, double X2, double Y1, double Y2);


int overlapsRightSlope(PlayerData *player, double X1, double X2, double Y, double slope);


int overlapsLeftSlope(PlayerData *player, double X1, double X2, double Y, double slope);


int collideType(PlayerData *player, World *gameWorld, int objectID);


int checkIfGrounded(World *gameWorld, PlayerData *player);


int assignDirection(PlayerData *player, Object *currentObject);


int switchPlayerSprite(int spriteID, int spriteSet, PlayerData *player);


int switchPlayerSpriteName(char spriteName[MAX_LEN], int spriteSet, PlayerData *player);



int tileCollision(PlayerData *player, World *gameWorld, double dx, double dy);


int fixCollisionAtPoint(PlayerData *player, World *gameWorld, int x, int y, double dx, double dy, int part);


int getTileAtPosition(World *gameWorld, int x, int y);

