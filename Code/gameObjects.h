#ifndef IS_DEFINED
#include "data.h"
#include "drawScreen.h"
#include "spriteLoader.h"
#include "soundProcessor.h"
#include "playerController.h"
#endif


// Creates new instance of an object and puts at the end of the object list
Object* AddObject(ObjectController *objController, int xPos, int yPos, int objectID, int arg1, int arg2, int arg3, int arg4, int arg5);


// Helper function to set an object to a moving platform type 
Object* DefineMovingPlatform(Object *inputObject, int objectID, int xPos, int yPos, int bound1, int bound2, int speed, int timer);


Object* AddFlagObject(ObjectController *objectList, Flags flagID, int xPos, int yPos, int arg1, int arg2, int arg3, int arg4, int arg5);


// Attempts to create object sprite set if it does not already exist
void CreateObjectSpriteSet(ObjectController *objController, int objectID);


int LoadParticleSprites(SpriteSet *newSet);


// Loads a new sprite to the sprite buffer in the object struct
int switchObjectSprite(int spriteID, Object *inputObject, ObjectController *objectList);


int switchObjectSpriteName(char spriteName[], Object *inputObject, ObjectController *objectList);


// Initialises a new empty object to the object list
Object* createNewObject(ObjectController *objectList, int xPos, int yPos, int objectID);


// deletes an object from the object list based on pointer provided and shifts surrounding objecs to fill
// empty space. Pointer given will subsequently be pointer to next available object or NULL
void deleteObject(ObjectController *objController, Object **input);


int MarkObjectForDeletion(Object *inputObject);


int UnmarkObjectForDeletion(Object *inputObject);


void deleteAllObjects(ObjectController *objectList);



void IncrementDrawPriority(ObjectController *objectList, Object *input);


void DecrementDrawPriority(ObjectController *objectList, Object *input);


// Sets given object to end of object list to give it layer priority when being drawn
void SetDrawPriorityToFront(ObjectController *objController, Object *input);

// Sets given object to start of object list to put it on the back layer when being drawn
void SetDrawPriorityToBack(ObjectController *objController, Object *input);


int moveObjectX(Object *inputObject, PlayerData *player);


int moveObjectY(Object *inputObject, PlayerData *player);


int ChangeObjectXSizeBy(int change, Object *inputObject, PlayerData *player);


int ChangeObjectYSizeBy(int change, Object *inputObject, PlayerData *player);


// Run every fram to operate objects that can move or be interacted with, etc.
FunctionResult updateObjects(World *gameWorld, int keyboard[256]);


int ObjectBehaviour(World *gameWorld, Object *inputObject);



int UpdateParticle(World *GameWorld, Object *particle);


int customParticleBehaviour(World *GameWorld, Object *particle);


int LoopParticleAnimation(Object *particle);


int UpdateHorizontalPlatform(PlayerData *player, Object *platform);


int UpdateVerticalPlatform(PlayerData *player, Object *platform);


int UpdateGateSwitch(PlayerData *player, Object *gateSwitch);


int UpdateVerticalGate(Object *door, ObjectController *objectList, PlayerData *player);


int UpdateHorizontalGate(Object *gate, ObjectController *objectList, PlayerData *player);


int gateControl(Object *gate, ObjectController *objectList);




// Check for overlap with player
int boxOverlapsPlayer(PlayerData *player, double X1, double X2, double Y1, double Y2);


int boxOverlapsPlayerFeet(PlayerData *player, double X1, double X2, double Y1, double Y2);


int rightSlopeOverlapsPlayer(PlayerData *player, Object *inputObject);


int leftSlopeOverlapsPlayer(PlayerData *player, Object *inputObject);


// Checks for object overlap with a specific object ID
int OverlapsObjectType(ObjectController *objectList, int overlapObjectID, Object *inputObject);

// Checks for overlap with a specific object solid type
int OverlapsObjectSolid(ObjectController *objectList, int solidID, Object *inputObject);

// Checks for overlap with any solid objects
int OverlapsObjectAllSolids(ObjectController *objectList, Object *inputObject);


int OverlapsObject(Object *inputObject, Object *otherObject);



