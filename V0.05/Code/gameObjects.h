#ifndef IS_DEFINED
#include "data.h"
#include "drawScreen.h"
#include "animations.h"
#include "spriteLoader.h"
#include "soundProcessor.h"
#include "playerController.h"
#include "eventManager.h"
#include "UIObjects.h"
#endif


// Creates new instance of an object and puts at the end of the object list
Object* AddObject(World *gameWorld, int objectID, int xPos, int yPos, int xSize, int ySize, int arg1, int arg2, int arg3, int arg4, int arg5);


Object* AddObjectWithParent(World *gameWorld, Object *ParentObject, int objectID, int xPos, int yPos, int xSize, int ySize, int arg1, int arg2, int arg3, int arg4, int arg5);


// Initialises a new empty object to the object list
Object* createNewObject(ObjectController *objectList, int xPos, int yPos, int objectID);


PhysicsRect* createPhysicsRect(SolidType inputSolid);


DisplayData* createDisplayData(RenderMode startRenderMode);


// Helper function to set an object to a moving platform type 
Object* InitialiseMovingPlatform(Object *inputObject, int objectID, int xPos, int yPos, int bound1, int bound2, int speed, int timer);


int InitialiseParticle(Object *particle, int animation, int repeatCount, int frameRate, int particleLifeTime);


// helper function used to define a level flag's attributes
int DefineLevelFlag(Object *inputObject, ObjectController *ObjectList);


// Attempts to create object sprite set if it does not already exist
SpriteSet* createObjectSpriteSet(ObjectController *ObjectList, int ObjectID);


int LoadSpriteSet(SpriteSet *newSet, int ObjectID);


int LoadParticleSprites(SpriteSet *newSet);


// Loads a new sprite to the sprite buffer in the object struct
int switchObjectSprite(int spriteID, Object *inputObject, ObjectController *objectList);


int switchObjectSpriteName(char spriteName[], Object *inputObject, ObjectController *objectList);


// deletes an object from the object list based on pointer provided and shifts surrounding objecs to fill
// empty space. Pointer given will subsequently be pointer to next available object or NULL
void deleteObject(Object **input, ObjectController *objController);


int MarkObjectForDeletion(Object *inputObject, ObjectController *ObjectList);


int UnmarkObjectForDeletion(Object *inputObject, ObjectController *ObjectList);


void deleteAllObjects(ObjectController *objectList);



void IncrementDrawPriority(ObjectController *objectList, Object *input);


void DecrementDrawPriority(ObjectController *objectList, Object *input);


// Sets given object to end of object list to give it layer priority when being drawn
void SetDrawPriorityToFront(ObjectController *objController, Object *input);

// Sets given object to start of object list to put it on the back layer when being drawn
void SetDrawPriorityToBack(ObjectController *objController, Object *input);



int ResolveXCollision(PhysicsRect *movingBox, double prevBoxXPos, World *GameWorld);


int ApplyXPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox);


int ResolveYCollision(PhysicsRect *movingBox, double prevBoxXPos, World *GameWorld, int *JumpProgressPtr);


int ApplyYPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox);


int ResolveXCollisionByPush(PhysicsRect *movingBox, PhysicsRect *compareBox, double prevObjXPos);


int ResolveYCollisionByPush(PhysicsRect *movingBox, PhysicsRect *compareBox, double prevObjYPos);


int moveObjectX(Object *inputObject, PlayerData *player);


int moveObjectY(Object *inputObject, PlayerData *player);


int SetObjectXPosition(Object *inputObject, double newXPos, PlayerData *Player);


int SetObjectYPosition(Object *inputObject, double newYPos, PlayerData *Player);


int ChangeObjectXSizeBy(int change, Object *inputObject, PlayerData *Player);


int ChangeObjectYSizeBy(int change, Object *inputObject, PlayerData *Player);


int GetPathToBox(Object *inputObject, PhysicsRect *target);



// Run every fram to operate objects that can move or be interacted with, etc.
FunctionResult updateObjects(World *gameWorld, int keyboard[256]);


int ObjectBehaviour(World *gameWorld, Object *inputObject, int keyboard[256]);



int UpdateParticle(World *GameWorld, Object *particle);


int CustomParticleBehaviour(World *GameWorld, Object *particle);


int UpdateHorizontalPlatform(PlayerData *player, Object *platform);


int UpdateVerticalPlatform(PlayerData *player, Object *platform);


int UpdateGateSwitch(PlayerData *player, Object *gateSwitch);


int UpdateVerticalGate(Object *door, ObjectController *objectList, PlayerData *player);


int UpdateHorizontalGate(Object *gate, ObjectController *objectList, PlayerData *player);


int gateControl(Object *gate, ObjectController *objectList);


int UpdateDoor(PlayerData *Player, Object *Door);





int CheckBoxCollidesBox(PhysicsRect *inputBox, PhysicsRect *compareBox);


Object* GetCollidingObject(PhysicsRect *inputBox, World *gameWorld);


int checkBoxOverlapsBox(PhysicsRect *inputBox, PhysicsRect *compareBox);


int checkBoxOverlapsBoxBottom(PhysicsRect *inputBox, PhysicsRect *compareBox);


int ClimbSlope(PhysicsRect *inputBox, PhysicsRect *compareBox, World *GameWorld);




int AssignDirection(PhysicsRect *inputBox, Object *currentObject);


// Checks for object overlap with a specific object ID
int OverlapsObjectType(ObjectController *objectList, int overlapObjectID, Object *inputObject);

// Checks for overlap with a specific object solid type
int OverlapsObjectSolid(ObjectController *objectList, int solidID, Object *inputObject);

// Checks for overlap with any solid objects
int OverlapsObjectAllSolids(ObjectController *objectList, Object *inputObject);


int OverlapsObject(Object *inputObject, Object *otherObject);



