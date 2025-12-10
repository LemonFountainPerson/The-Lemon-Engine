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
#include "cutsceneManager.h"
#endif


// Object Creation
// Creates new instance of an object and puts at the end of the object list
Object* AddObject(World *GameWorld, int objectID, int xPos, int yPos, int xSize, int ySize, int arg1, int arg2, int arg3, int arg4, int arg5);

Object* AddNamedObject(World *GameWorld, const char name[], int objectID, int xPos, int yPos);

Object* AddObjectWithParent(World *GameWorld, Object *ParentObject, int objectID, int xPos, int yPos, int xSize, int ySize, int arg1, int arg2, int arg3, int arg4, int arg5);

Object* AddParticle(World *GameWorld, ParticleSubType animation, int xPos, int yPos, int repeatCount, int particleLifeTime);


// Initialises a new empty object to the object list
Object* createNewObject(ObjectType objectID, ObjectController *ObjectList);


Object* findNewObject(ObjectController *ObjectList);

void removePreAllocatedObject(Object *inputObject, ObjectController *ObjectList);


int SetObjectName(const char name[], Object *inputObject);

Object* FindObject(const char name[], ObjectController ObjectList);

Object* FindObjectID(const char name[], int objectID, ObjectController ObjectList);


int snapPositionToTileGrid(PhysicsRect *ObjectBox, int xPos, int yPos);


PhysicsRect* createPhysicsRect(SolidType inputSolid);

DisplayData* createDisplayData(RenderMode startRenderMode);


// Helper function to set an object to a moving platform type 
Object* InitialiseMovingPlatform(Object *inputObject, int objectID, int xPos, int yPos, int bound1, int bound2, int speed, int timer);


int InitialiseParticle(Object *particle, int animation, int repeatCount, int particleLifeTime);


// Attempts to create object sprite set if it does not already exist
SpriteSet* createObjectSpriteSet(ObjectController *ObjectList, int ObjectID);


// Object Attribute Modifiers
int switchObjectSprite(int spriteID, Object *inputObject);

int switchObjectSpriteName(const char spriteName[], Object *inputObject);


Object* deleteObject(Object *input, ObjectController *objController);

void deleteAllObjects(ObjectController *objectList);


int MarkObjectForDeletion(Object *inputObject);

int UnmarkObjectForDeletion(Object *inputObject, ObjectController *ObjectList);


void IncrementDrawPriority(ObjectController *objectList, Object *input);

void DecrementDrawPriority(ObjectController *objectList, Object *input);


// Sets given object to end of object list to give it layer priority when being drawn
void SetDrawPriorityToFront(ObjectController *objController, Object *input);

// Sets given object to start of object list to put it on the back layer when being drawn
void SetDrawPriorityToBack(ObjectController *objController, Object *input);


int moveObjectToCachedList(ObjectController *ObjectList, Object *inputObject);


int swapMainAndCachedLists(ObjectController *ObjectList);


int cacheObjects(ObjectController *ObjectList, PhysicsRect boundingBox);



// Convenience Functions
int GoTo(Object *inputObject, float destX, float destY);

int GoToWithCollision(Object *inputObject, float destX, float destY, ObjectController *ObjectList);

int SetXPosition(Object *inputObject, float newXPos);

int SetYPosition(Object *inputObject, float newYPos);


int PointObjectTowards(Object *inputObject, Object *pointDestination, RotateMode RotateSetting);

int PointObjectToMouse(Object *inputObject, World *GameWorld, RotateMode RotateSetting);

int PointObjectToXY(Object *inputObject, float xPos, float yPos, RotateMode RotateSetting);


int RotateObject(Object *inputObject, double rotationDegrees, RotateMode RotateSetting);

int SetObjectDirection(Object *inputObject, double rotationDegrees, RotateMode RotateSetting);


int ChangeObjectXSizeBy(int change, Object *inputObject, ObjectController *ObjectList);

int ChangeObjectYSizeBy(int change, Object *inputObject, ObjectController *ObjectList);

int setScaleSize(Object *input, float sizePercentage);

int changeScaleSize(Object *input, float sizePercentage);

int smoothSizeChangeTo(Object *input, float desiredSize, float rate);

int matchBoxToDisplayDimensions(Object *input);


float DistanceBetween(Object *Source, Object *Target);


bool onScreen(Object *inputObject, World *GameWorld);


int MouseOverlappingBox(Object *input, World *GameWorld);


int MouseOverlappingSprite(Object *input, World *GameWorld);


// Core Object Functionality
FuncResult updateObjects(World *GameWorld);


int ObjectBehaviour(World *GameWorld, Object *inputObject);


int UpdateObjectDisplay(Object *inputObject);


int UpdatePreviousPositions(ObjectController *ObjectList);


int UpdatePhysicsState(Object *inputObject, World *GameWorld);


int applyMagnetisation(PhysicsRect *inputBox, PhysicsRect *GroundBox, World *GameWorld);
	

int UpdateParentChildLink(Object *inputObject);


int ResolveAllObjects(World *GameWorld);


FuncResult updateObjectsFrame(World *GameWorld);


int ExecuteFrameUpdateFunction(FrameUpdateFunction *inputFunction, World *GameWorld);


int AddFrameUpdateFunction(int (*FunctionPointer)(Object*, World*), Object *inputObject, World *GameWorld);


int DamagedFrames(Object *inputObject);


int moveObjectX(PhysicsRect *inputBox, ObjectController *ObjectList);


int moveObjectY(PhysicsRect *inputBox, ObjectController *ObjectList);


int moveObjectForward(PhysicsRect *movingBox, ObjectController *ObjectList);



// Object Behaviour
int UpdateParticle(World *GameWorld, Object *particle);


int CustomParticleBehaviour(World *GameWorld, Object *particle);


int UpdateHorizontalPlatform(Object *platform);


int UpdateVerticalPlatform(Object *platform);


int UpdateGateSwitch(PlayerData player, Object *gateSwitch, ObjectController *ObjectList);


int UpdateVerticalGate(Object *door, World *GameWorld);


int UpdateHorizontalGate(Object *gate, World *GameWorld);


int gateControl(Object *gate, ObjectController *objectList);


int UpdateDoor(PlayerData Player, Object *Door, World *GameWorld);


int TeleportPlayerToExitDoor(Object *Door, void *GameWorld);


int UpdateLevelDoor(PlayerData Player, Object *Door, World *GameWorld);


int UpdateSpring(Object *spring, World *GameWorld);


int UpdateCoin(Object *coin, World *GameWorld);



// Components
int MoveObject(Object *inputObject, World *GameWorld);


int ApplyGravity(Object *entity, World *GameWorld);


int ApplyFriction(PhysicsRect *inputBox, float forwardFriction, float xFriction, float yFriction);



// Collision Detection
Object* GetCollidingObject(PhysicsRect *inputBox, ObjectController *ObjectList);


int CheckBoxCollidesBox(PhysicsRect *inputBox, PhysicsRect *compareBox);


int checkBoxOverlapsBoxBroad(PhysicsRect *inputBox, PhysicsRect *compareBox);


int CheckBoxOverlapsBox(PhysicsRect *inputBox, PhysicsRect *compareBox);


Object* GetOverlappingObject(Object *inputObject, ObjectController *objectList);


Object* GetOverlappingObjectType(Object *inputObject, int overlapObjectID, ObjectController *objectList);


Object* GetOverlappingObjectSolid(Object *inputObject, int solidID, ObjectController *objectList);


Object* GetOverlappingObjectAllSolids(Object *inputObject, ObjectController *ObjectList);


int OverlapsObject(Object *inputObject, Object *otherObject);


CollideType evaluateCollideMode(PhysicsRect *movingBox, PhysicsRect *collideBox);


int constructQuadTree(ObjectController *ObjectList);


int deleteQuadTree(struct QuadTree *inputTree);


int insertObjectIntoQuadTree(Object *inputObject, struct QuadTree *inputTree);



// Collision Resolution
int ResolveAllXCollision(PhysicsRect *movingBox, ObjectController *ObjectList);


int ResolveXCollision(PhysicsRect *movingBox, PhysicsRect *compareBox, ObjectController *ObjectList);


int ApplyXPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox);


int ResolveAllYCollision(PhysicsRect *movingBox, ObjectController *ObjectList);


int ResolveYCollision(PhysicsRect *movingBox, PhysicsRect *compareBox);


int ApplyYPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox);


int ResolveXCollisionByPush(PhysicsRect *movingBox, PhysicsRect *compareBox);


int ResolveYCollisionByPush(PhysicsRect *movingBox, PhysicsRect *compareBox);


int ResolveAllXCollisionsByPush(PhysicsRect *movingBox, ObjectController *ObjectList);


int ResolveAllYCollisionsByPush(PhysicsRect *movingBox, ObjectController *ObjectList);


int ApplyForwardPhysics(PhysicsRect *inputBox, PhysicsRect *physicsBox);


Object* CheckForGround(PhysicsRect *movingBox, World *GameWorld);


int resetGroundCheck(PhysicsRect *inputBox, World *GameWorld);


int AdjustDirection(PhysicsRect *movingBox, World *GameWorld);


int AssignDirection(PhysicsRect *inputBox, PhysicsRect *compareBox);


int ClimbSlope(PhysicsRect *inputBox, PhysicsRect *compareBox, ObjectController *ObjectList);

