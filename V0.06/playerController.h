#ifndef IS_DEFINED
#include "data.h"
#include "soundProcessor.h"
#include "spriteLoader.h"
#include "gameObjects.h"
#include "animations.h"
#endif


// Initialises an instance of the Player
PlayerData* InitialisePlayerData(World *GameWorld);


int InitialisePlayerObject(Object *Player, World *GameWorld);


int LoadPlayerSprites(SpriteSet *inputSet);


int ResetPlayer(PlayerData *Player);


// Main Player update script
FunctionResult UpdatePlayer(PlayerData *Player, World *GameWorld, int keyboard[256]);


int PlayerPlatformerPhysics(PlayerData *Player, World *GameWorld, int keyboard[256]);


int PlayerTopDownPhysics(PlayerData *Player, World *GameWorld, int keyboard[256]);


int PlayerJump(PlayerData *Player, int hAxis, int vAxis);


int HandlePlayerInteract(World *GameWorld, int keyboard[]);


int PlayerInteractingWithBox(PlayerData *Player, PhysicsRect *inputBox);


int checkIfGrounded(World *GameWorld, PhysicsRect *inputBox);



int animatePlayer(PlayerData *Player);


int switchPlayerSprite(int spriteID, DisplayData *PlayerDisplay);


int switchPlayerSpriteName(char spriteName[MAX_LEN], DisplayData *PlayerDisplay);
