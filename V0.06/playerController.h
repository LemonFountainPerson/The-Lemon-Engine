#ifndef IS_DEFINED
#include "data.h"
#include "soundProcessor.h"
#include "spriteLoader.h"
#include "gameObjects.h"
#include "animations.h"
#endif


// Initialises an instance of the Player
PlayerData* InitialisePlayerData(World *gameWorld);


int InitialisePlayerObject(Object *Player, World *gameWorld);


int LoadPlayerSprites(SpriteSet *inputSet);


int ResetPlayer(PlayerData *Player);


// Main Player update script
FunctionResult UpdatePlayer(PlayerData *Player, World *gameWorld, int keyboard[256]);


int PlayerPlatformerPhysics(PlayerData *Player, World *gameWorld, int keyboard[256]);


int PlayerTopDownPhysics(PlayerData *Player, World *gameWorld, int keyboard[256]);


int PlayerJump(PlayerData *Player, int hAxis, int vAxis);


int HandlePlayerInteract(World *GameWorld, int keyboard[]);


int PlayerInteractingWithBox(PlayerData *Player, PhysicsRect *inputBox);


int animatePlayer(PlayerData *Player);



int MovePlayerForward(PlayerData *Player, World *GameWorld);


int MovePlayerX(PlayerData *Player, World *GameWorld);


int MovePlayerY(PlayerData *Player, World *GameWorld);


int checkIfGrounded(World *GameWorld, PhysicsRect *inputBox);



int switchPlayerSprite(int spriteID, DisplayData *PlayerDisplay);


int switchPlayerSpriteName(char spriteName[MAX_LEN], DisplayData *PlayerDisplay);
