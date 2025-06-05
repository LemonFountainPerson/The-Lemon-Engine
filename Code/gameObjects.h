#ifndef IS_DEFINED
#include "data.h"
#include "drawScreen.h"
#include "spriteLoader.h"
#include "soundProcessor.h"
#include "playerController.h"
#endif


// Creates new instance of an object and puts at the end of the object list
Object* AddObject(World *gameWorld, int xPos, int yPos, int objectID, int arg1, int arg2, int arg3, int arg4, int arg5);


// Initialises a new empty object to the object list
Object* createNewObject(ObjectController *objectList, int xPos, int yPos, int objectID);


// Helper function to set an object to a moving platform type 
Object* DefineMovingPlatform(Object *inputObject, int objectID, int xPos, int yPos, int bound1, int bound2, int speed, int timer);


Object* AddFlagObject(World *gameWorld, Flags flagID, int xPos, int yPos, int arg1, int arg2, int arg3, int arg4, int arg5);


// Attempts to create object sprite set if it does not already exist
void CreateObjectSpriteSet(ObjectController *objController, int objectID);


int LoadParticleSprites(SpriteSet *newSet);


// Loads a new sprite to the sprite buffer in the object struct
int switchObjectSprite(int spriteID, Object *inputObject, ObjectController *objectList);


int switchObjectSpriteName(char spriteName[], Object *inputObject, ObjectController *objectList);


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



int ResolvePlayerToObjectCollisionX(Object *inputObject, PlayerData *player, double prevObjXPos, double prevObjXPosRight);


int ResolvePlayerToObjectCollisionY(Object *inputObject, PlayerData *player, double prevObjYPos, double prevObjYPosTop);


int moveObjectX(Object *inputObject, PlayerData *player);


int moveObjectY(Object *inputObject, PlayerData *player);


int SetObjectXPosition(Object *inputObject, double newXPos, PlayerData *Player);


int SetObjectYPosition(Object *inputObject, double newYPos, PlayerData *Player);


int ChangeObjectXSizeBy(int change, Object *inputObject, PlayerData *player);


int ChangeObjectYSizeBy(int change, Object *inputObject, PlayerData *player);


int GetPathToBox(Object *inputObject, PhysicsRect *target);



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





int CheckBoxOverlapsBox(PhysicsRect *inputBox, PhysicsRect *compareBox);


Object* GetObjectOverlappingBox(PhysicsRect *inputBox, World *gameWorld);


int boxOverlapsBox(PhysicsRect *inputBox, PhysicsRect *compareBox);


int boxOverlapsBoxBottom(PhysicsRect *inputBox, PhysicsRect *compareBox);


int ClimbFlatSlope(PhysicsRect *inputBox, PhysicsRect *compareBox, World *GameWorld);




int assignDirection(PhysicsRect *inputBox, Object *currentObject);


// Checks for object overlap with a specific object ID
int OverlapsObjectType(ObjectController *objectList, int overlapObjectID, Object *inputObject);

// Checks for overlap with a specific object solid type
int OverlapsObjectSolid(ObjectController *objectList, int solidID, Object *inputObject);

// Checks for overlap with any solid objects
int OverlapsObjectAllSolids(ObjectController *objectList, Object *inputObject);


int OverlapsObject(Object *inputObject, Object *otherObject);



