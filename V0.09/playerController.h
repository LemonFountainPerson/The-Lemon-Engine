#ifndef IS_DEFINED
#include "data.h"
#include "soundProcessor.h"
#include "spriteLoader.h"
#include "gameObjects.h"
#include "animations.h"
#include "LemonMain.h"
#endif


// Initialises an instance of the Player
PlayerData* InitialisePlayerData(PlayerData *Player);


int InitialisePlayerObject(Object *Player, World *GameWorld);


int LoadPlayerSprites(SpriteSet *inputSet);


int ResetPlayer(PlayerData *Player);


int PlayerObjectAboutToBeDeleted(PlayerData *Player);


// Main Player update script
FuncResult UpdatePlayer(PlayerData *Player, World *GameWorld);


int PlayerPlatformerPhysics(PlayerData *Player, World *GameWorld);


int PlayerTopDownPhysics(PlayerData *Player, World *GameWorld);


int PlayerJump(PlayerData *Player, int hAxis, int vAxis);


int HandlePlayerInteract(PlayerData *Player);


int PlayerInteractingWithBox(PlayerData Player, PhysicsRect *inputBox);


int checkIfGrounded(World *GameWorld, PhysicsRect *inputBox);



int animatePlayer(PlayerData *Player);


int switchPlayerSprite(int spriteID, DisplayData *PlayerDisplay);


int switchPlayerSpriteName(char spriteName[MAX_LEN], DisplayData *PlayerDisplay);
