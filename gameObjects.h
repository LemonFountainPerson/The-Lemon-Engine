#ifndef IS_DEFINED
#include "data.h"
#endif


// Attempts to create object sprite set if it does not already exist
void createObjectSpriteSet(ObjectController *objController, int objectID);

// Loads a new sprite to the sprite buffer in the object struct
int switchObjectSprite(int spriteID, Object *inputObject, ObjectController *objectList);


int switchObjectSpriteName(char spriteName[], Object *inputObject, ObjectController *objectList);



Object* createNewObject(ObjectController *objectList, int xPos, int yPos, int objectID);


// Creates new instance of an object and puts at the end of the object list
Object* addObject(ObjectController *objController, int xPos, int yPos, int objectID, int arg1, int arg2);


// Creates new instance of a moving platform object and puts at the end of the object list
Object* addMovingPlatform(ObjectController *objController, int objectID, int xPos, int yPos, int bound1, int bound2, int speed, int timer);


Object* addFlagObject(ObjectController *objectList, Flags flagID, int xPos, int yPos, int arg1, int arg2, int arg3, int arg4, int arg5);


// deletes an object from the object list based on pointer provided and shifts surrounding objecs to fill
// empty space. Pointer given will subsequently be pointer to next available object or NULL
void deleteObject(ObjectController *objController, Object **input);


void deleteAllObjects(ObjectController *objectList);



void incrementDrawPriority(ObjectController *objectList, Object *input);


void decrementDrawPriority(ObjectController *objectList, Object *input);


// Sets given object to end of object list to give it layer priority when being drawn
void setDrawPriorityToFront(ObjectController *objController, Object *input);

// Sets given object to start of object list to put it on the back layer when being drawn
void setDrawPriorityToBack(ObjectController *objController, Object *input);


int moveObjectX(Object *inputObject, PlayerData *player, double deltaTime);


int moveObjectY(Object *inputObject, PlayerData *player, double deltaTime);


int changeObjectXSizeBy(int change, Object *inputObject, PlayerData *player);


int changeObjectYSizeBy(int change, Object *inputObject, PlayerData *player);


// Run every fram to operate objects that can move or be interacted with, etc.
void updateObjects(World *gameWorld, int keyboard[256], double deltaTime);



int updateHorizontalPlatform(PlayerData *player, Object *platform, double deltaTime);


int updateVerticalPlatform(PlayerData *player, Object *platform, double deltaTime);


int updateGateSwitch(PlayerData *player, Object *gateSwitch);


int updateVerticalGate(Object *door, ObjectController *objectList, double deltaTime, PlayerData *player);


int gateControl(Object *gate, ObjectController *objectList);




// Check for overlap with player
int overlapsPlayer(PlayerData *player, double X1, double X2, double Y1, double Y2);


int overlapsPlayerFeet(PlayerData *player, double X1, double X2, double Y1, double Y2);


// Checks for object overlap with a specific object ID
int OverlapsObjectType(ObjectController *objectList, int overlapObjectID, Object *inputObject);

// Checks for overlap with a specific object solid type
int OverlapsObjectSolid(ObjectController *objectList, int solidID, Object *inputObject);

// Checks for overlap with any solid objects
int OverlapsObjectAllSolids(ObjectController *objectList, Object *inputObject);


int OverlapsObject(Object *inputObject, Object *otherObject);



