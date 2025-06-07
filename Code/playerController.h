#ifndef IS_DEFINED
#include "data.h"
#include "soundProcessor.h"
#include "spriteLoader.h"
#include "gameObjects.h"
#endif


// Initialises an instance of the player
PlayerData* InitialisePlayer(World *gameWorld);


int ResetPlayer(PlayerData *Player);


// Main player update script
FunctionResult UpdatePlayer(PlayerData *player, World *gameWorld, int keyboard[256]);


int playerJump(PlayerData *player, int hAxis, int vAxis);


int setSprite(PlayerData *player);



int MovePlayerX(PlayerData *player, World *GameWorld);


int ApplyXPhysics(PlayerData *player, Object *inputObject);


int MovePlayerY(PlayerData *player, World *GameWorld);


int ApplyYPhysics(PlayerData *player, Object *inputObject);



int checkIfGrounded(World *gameWorld, PlayerData *player);



int switchPlayerSprite(int spriteID, int spriteSet, PlayerData *player);


int switchPlayerSpriteName(char spriteName[MAX_LEN], int spriteSet, PlayerData *player);
