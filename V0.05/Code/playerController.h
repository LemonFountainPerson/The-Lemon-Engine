#ifndef IS_DEFINED
#include "data.h"
#include "soundProcessor.h"
#include "spriteLoader.h"
#include "gameObjects.h"
#endif


// Initialises an instance of the Player
PlayerData* InitialisePlayer(World *gameWorld);


int LoadPlayerSprites(DisplayData *PlayerDisplay);


int ResetPlayer(PlayerData *Player);


// Main Player update script
FunctionResult UpdatePlayer(PlayerData *Player, World *gameWorld, int keyboard[256]);


int PlayerJump(PlayerData *Player, int hAxis, int vAxis);


int HandlePlayerInteract(PlayerData *Player, int keyboard[]);


int setPlayerSprite(PlayerData *Player);



int MovePlayerX(PlayerData *Player, World *GameWorld);


int MovePlayerY(PlayerData *Player, World *GameWorld);


int checkIfGrounded(World *gameWorld, PlayerData *Player);



int switchPlayerSprite(int spriteID, int spriteSet, DisplayData *PlayerDisplay);


int switchPlayerSpriteName(char spriteName[MAX_LEN], int spriteSet, DisplayData *PlayerDisplay);
