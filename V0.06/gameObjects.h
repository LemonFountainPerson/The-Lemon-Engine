#ifndef IS_DEFINED
#include "data.h"
#include "drawScreen.h"
#include "animations.h"
#include "spriteLoader.h"
#include "soundProcessor.h"
#include "playerController.h"
#include "eventManager.h"
#include "UIObjects.h"
#include "enemies.h"
#include "LemonMain.h"
#include "levelLoader.h"
#endif


// Creates new instance of an object and puts at the end of the object list
Object* AddObject(World *GameWorld, int objectID, int xPos, int yPos, int xSize, int ySize, int arg1, int arg2, int arg3, int arg4, int arg5);


Object* AddObjectWithParent(World *GameWorld, Object *ParentObject, int objectID, int xPos, int yPos, int xSize, int ySize, int arg1, int arg2, int arg3, int arg4, int arg5);


Object* AddParticle(World *GameWorld, ParticleSubType animation, int xPos, int yPos, int repeatCount, int frameRate, int particleLifeTime);


// Initialises a new empty object to the object list
Object* createNewObject(ObjectController *objectList, int xPos, int yPos, int objectID);


void resetPhysicsRect(PhysicsRect inputRect, SolidType inputSolid);


void resetDisplayData(PhysicsRect inputRect, RenderMode startRenderMode);


int ResetPhysicsRect(PhysicsRect *inputRect, SolidType inputSolid);


int ResetDisplayData(DisplayData *inputDisplay, RenderMode startRenderMode);


PhysicsRect* createPhysicsRect(SolidType inputSolid);


DisplayData* createDisplayData(RenderMode startRenderMode);


// Helper function to set an object to a moving platform type 
Object* InitialiseMovingPlatform(Object *inputObject, int objectID, int xPos, int yPos, int bound1, int bound2, int speed, int timer);


int InitialiseParticle(Object *particle, int animation, int repeatCount, int frameRate, int particleLifeTime);


// Attempts to create object sprite set if it does not already exist
SpriteSet* createObjectSpriteSet(ObjectController *ObjectList, int ObjectID);


int LoadSpriteSet(SpriteSet *newSet, int ObjectID);


int LoadParticleSprites(SpriteSet *newSet);


// Loads a new sprite to the sprite buffer in the object struct
int switchObjectSprite(int spriteID, Object *inputObject, ObjectController *objectList);


int switchObjectSpriteName(char spriteName[], Object *inputObject, ObjectController *objectList);


// deletes an object from the object list based on pointer provided and shifts surrounding objecs to fill
// empty space. Pointer given will subsequently be turned NULL, and should not be used
void deleteObject(Object **input, ObjectController *objController);


int MarkObjectForDeletion(Object *inputObject);


int UnmarkObjectForDeletion(Object *inputObject, ObjectController *ObjectList);


void deleteAllObjects(ObjectController *objectList);



void IncrementDrawPriority(ObjectController *objectList, Object *input);


void DecrementDrawPriority(ObjectController *objectList, Object *input);


// Sets given object to end of object list to give it layer priority when being drawn
void SetDrawPriorityToFront(ObjectController *objController, Object *input);

// Sets given object to start of object list to put it on the back layer when being drawn
void SetDrawPriorityToBack(ObjectController *objController, Object *input);


int moveObjectToCachedList(ObjectController *ObjectList, Object *inputObject);


int swapMainAndCachedLists(ObjectController *ObjectList);


int cacheObjects(ObjectController *ObjectList, PhysicsRect *boundingBox, PlayerData *Player);



int UpdateEntityPhysics(Object *entity, PlayerData *Player, World *GameWorld);


int moveObjectX(Object *inputObject, World *GameWorld);


int moveObjectY(Object *inputObject, World *GameWorld, int *jumpProgressPtr);


int SetObjectXPosition(Object *inputObject, double newXPos, ObjectController *ObjectList);


int SetObjectYPosition(Object *inputObject, double newYPos, ObjectController *ObjectList);


int ChangeObjectXSizeBy(int change, Object *inputObject, ObjectController *ObjectList);


int ChangeObjectYSizeBy(int change, Object *inputObject, ObjectController *ObjectList);



int MoveForward(PhysicsRect *movingBox, World *GameWorld);


int AdjustDirection(World *GameWorld, PhysicsRect *movingBox);


int ApplyForwardPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox);



int ResolveAllXCollision(PhysicsRect *movingBox, ObjectController *ObjectList);


int ResolveXCollision(PhysicsRect *movingBox, PhysicsRect *compareBox, ObjectController *ObjectList);


int magnetiseObjectsX(PhysicsRect *movingBox, ObjectController *ObjectList);


int ApplyXPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox);


int ResolveAllYCollision(PhysicsRect *movingBox, ObjectController *ObjectList, int *JumpProgressPtr);


int ResolveYCollision(PhysicsRect *movingBox, PhysicsRect *compareBox, int *jumpProgressPtr);


int magnetiseObjectsY(PhysicsRect *movingBox, ObjectController *ObjectList);


int ApplyYPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox);


int ResolveXCollisionByPush(PhysicsRect *movingBox, PhysicsRect *compareBox);


int ResolveYCollisionByPush(PhysicsRect *movingBox, PhysicsRect *compareBox);


int ResolveAllXCollisionsByPush(PhysicsRect *movingBox, ObjectController *ObjectList);


int ResolveAllYCollisionsByPush(PhysicsRect *movingBox, ObjectController *ObjectList);



// Run every fram to operate objects that can move or be interacted with, etc.
FunctionResult updateObjects(World *GameWorld, int keyboard[256]);


int ObjectBehaviour(World *GameWorld, Object *inputObject, int keyboard[256]);


int UpdateObjectDisplay(World *GameWorld, Object *inputObject);


int UpdateParentChildLink(Object *inputObject);


int UpdateParticle(World *GameWorld, Object *particle);


int CustomParticleBehaviour(World *GameWorld, Object *particle);


int UpdateHorizontalPlatform(PlayerData *player, Object *platform);


int UpdateVerticalPlatform(PlayerData *player, Object *platform);


int UpdateGateSwitch(PlayerData *player, Object *gateSwitch, ObjectController *ObjectList);


int UpdateVerticalGate(Object *door, World *GameWorld);


int UpdateHorizontalGate(Object *gate, World *GameWorld);


int gateControl(Object *gate, ObjectController *objectList);


int UpdateDoor(PlayerData *Player, Object *Door, World *GameWorld);


int UpdateLevelDoor(PlayerData *Player, Object *Door, World *GameWorld);


int UpdateSpring(Object *spring, World *GameWorld);


int UpdateCoin(Object *coin, World *GameWorld);





int CheckBoxCollidesBox(PhysicsRect *inputBox, PhysicsRect *compareBox);


Object* GetCollidingObject(PhysicsRect *inputBox, ObjectController *ObjectList);


int checkBoxOverlapsBox(PhysicsRect *inputBox, PhysicsRect *compareBox);


int checkBoxOverlapsBoxBottom(PhysicsRect *inputBox, PhysicsRect *compareBox);


// Checks for object overlap with a specific object ID
int OverlapsObjectType(ObjectController *objectList, int overlapObjectID, Object *inputObject);

// Checks for overlap with a specific object solid type
int OverlapsObjectSolid(ObjectController *objectList, int solidID, Object *inputObject);

// Checks for overlap with any solid objects
int OverlapsObjectAllSolids(ObjectController *objectList, Object *inputObject);


int OverlapsObject(Object *inputObject, Object *otherObject);


CollideType evaluateCollideMode(PhysicsRect *movingBox, PhysicsRect *collideBox);



int AssignDirection(PhysicsRect *inputBox, PhysicsRect *compareBox);


int ClimbSlope(PhysicsRect *inputBox, PhysicsRect *compareBox, ObjectController *ObjectList);

