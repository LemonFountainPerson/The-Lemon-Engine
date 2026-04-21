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


int PlayerJump(PlayerData *Player, bool jump);


int HandlePlayerInteract(PlayerData *Player);


bool PlayerInteractingWithBox(PhysicsBox *inputBox, World *GameWorld);


int checkIfGrounded(World *GameWorld, PhysicsBox *inputBox);



int animatePlayer(PlayerData *Player);


int switchPlayerSprite(int spriteID, DisplayData *PlayerDisplay);


int switchPlayerSpriteName(const char spriteName[], DisplayData *PlayerDisplay);
