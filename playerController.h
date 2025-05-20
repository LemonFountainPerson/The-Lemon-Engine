#ifndef IS_DEFINED
#include "data.h"
#endif


// Initialises an instance of the player
PlayerData* initialisePlayer(World *gameWorld);


// Main player update script
FunctionResult updatePlayer(PlayerData *player, World *gameWorld, int keyboard[256], double deltaTime);


int playerJump(PlayerData *player, int hAxis, int vAxis);


int setSprite(PlayerData *player);



int MovePlayerX(PlayerData *player, World *gameWorld, double deltaTime);


int ApplyXPhysics(PlayerData *player, Object *inputObject);


int MovePlayerY(PlayerData *player, World *gameWorld, double deltaTime);


int ApplyYPhysics(PlayerData *player, Object *inputObject);


Object* OverlappingObject(PlayerData *player, World *gameWorld);


int overlapsBox(PlayerData *player, double X1, double X2, double Y1, double Y2);


int overlapsRightSlope(PlayerData *player, Object *inputObject, double slope);


int overlapsLeftSlope(PlayerData *player, Object *inputObject, double slope);


int overlapsBoxAtFeet(PlayerData *player, double X1, double X2, double Y1, double Y2);



int collideType(PlayerData *player, World *gameWorld, int objectID);


int checkIfGrounded(World *gameWorld, PlayerData *player);


int assignDirection(PlayerData *player, Object *currentObject);


int switchPlayerSprite(int spriteID, int spriteSet, PlayerData *player);


int switchPlayerSpriteName(char spriteName[MAX_LEN], int spriteSet, PlayerData *player);



int tileCollision(PlayerData *player, World *gameWorld, double dx, double dy);


int fixCollisionAtPoint(PlayerData *player, World *gameWorld, int x, int y, double dx, double dy, int part);


int getTileAtPosition(World *gameWorld, int x, int y);

