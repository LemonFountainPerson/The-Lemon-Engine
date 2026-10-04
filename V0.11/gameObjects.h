// Object Creation
// Creates new instance of an object and puts at the end of the object list
Object* AddObject(World *GameWorld, int objectID, float xPos, float yPos, int arg1, int arg2, int arg3, int arg4);

Object* AddNamedObject(World *GameWorld, const char name[], int objectID, float xPos, float yPos);

Object* AddObjectWithParent(World *GameWorld, Object *ParentObject, int objectID, float xPos, float yPos, int arg1, int arg2, int arg3, int arg4);

Object* AddObjectFromMeta(World *GameWorld, ObjectMeta input);

Object* AddParticle(World *GameWorld, ParticleSubType animation, float xPos, float yPos, int repeatCount, int particleLifeTime);

Object* cloneObject(Object *input, World *GameWorld);


// Initialises a new empty object to the object list
Object* getNewObject(ObjectType objectID, ObjectController *ObjectList);

Object* findNewObject(ObjectController *ObjectList);

int InitialiseGenericObject(Object *inputObject, ObjectType objectID, ObjectController *ObjectList);

void clearObjectData(Object *input);

SpriteSet* loadSpriteSet(ObjectController *ObjectList, int ObjectID);


int SetObjectName(Object *inputObject, const char name[]);

Object* FindObject(const char name[], ObjectController *ObjectList);

Object* FindObjectID(const char name[], int objectID, ObjectController ObjectList);


int snapPositionToTileGrid(Object *input, float xPos, float yPos);


PhysicsBox* createPhysicsBox(SolidType inputSolid);

int resetPhysicsBox(PhysicsBox *input);

DisplayData* createDisplayData(RenderMode startRenderMode);

int resetDisplayData(DisplayData *input);

DisplayData* getDisplay(Object *input);

Layer getDisplayLayer(Object *input);

bool setDisplayLayer(Object *input, Layer newLayer);

RenderMode getRenderModeOverride(Object *input);

RenderMode getRenderMode(Object *input);

bool setRenderModeOverride(Object *input, RenderMode newMode);

double getDisplayDirection(Object *input);

bool setTransparency(Object *input, float transparency);

bool changeTransparency(Object *input, float transparency);

float getTransparency(Object *input);

bool hideObject(Object *input);

bool showObject(Object *input);

bool toggleHidden(Object *input);

bool setRotateMode(Object *input, RotationMode mode);

int getSubType(Object *input);

int setSubType(Object *input, int subType);



// Helper function to set an object to a moving platform type 
Object* InitialiseMovingPlatform(Object *inputObject, int bound1, int bound2, int speed, int timer, World *GameWorld);

int InitialiseParticle(Object *particle, int animation, int repeatCount, int particleLifeTime);


// Object Attribute Modifiers
int switchObjectSprite(int spriteID, Object *inputObject);

int switchObjectSpriteByName(const char spriteName[], Object *inputObject);


Object* deleteObject(Object *input, ObjectController *objController);

void deleteAssociatedFrameFunction(Object *input, ObjectController *ObjectList);

void deleteAllObjects(ObjectController *objectList);

void deleteLevelObjects(ObjectController *ObjectList);

void deleteAllEnvironmentObjects(ObjectController *ObjectList);


int MarkObjectForDeletion(Object *inputObject);

int UnmarkObjectForDeletion(Object *inputObject);

int MarkObjectReferenceForDeletion(ObjectReference input);

bool ObjectDeleted(ObjectReference input);

bool ObjectMatchesReference(ObjectReference ref, Object *compare);

void SetObjectReference(ObjectReference *input, Object *obj);

void ClearObjectReference(ObjectReference *input);


void IncrementDrawPriority(ObjectController *objectList, Object *input);

void DecrementDrawPriority(ObjectController *objectList, Object *input);


// Sets given object to end of object list to give it layer priority when being drawn
void SetDrawPriorityToFront(ObjectController *objController, Object *input);

// Sets given object to start of object list to put it on the back layer when being drawn
void SetDrawPriorityToBack(ObjectController *objController, Object *input);



// Core Object Functionality
FuncResult updateObjects(World *GameWorld);


int updateComponents(World *GameWorld);


int ObjectBehaviour(World *GameWorld, Object *inputObject);

int updateObjectDisplays(World *GameWorld);

int UpdateObjectDisplay(Object *inputObject, float deltaTime);


void updatePreviousPositions(ObjectController *ObjectList);

int updateObjectsState(ObjectController *ObjectList, World *GameWorld);

int UpdatePhysicsState(Object *inputObject, World *GameWorld);

int applyMagnetisation(PhysicsBox *inputBox, PhysicsBox *GroundBox, World *GameWorld);

int ResolveAllObjects(World *GameWorld);

int UpdateParentChildLink(Object *inputObject);


FuncResult updateObjectsFrame(World *GameWorld);

int ExecuteFrameUpdateFunction(FrameUpdateFunction *inputFunction, World *GameWorld);

int AddFrameUpdateFunction(TriggerableFunction FunctionPointer, Object *inputObject, ObjectController *ObjectList);

int deleteAllFrameUpdateFunctions(ObjectController *ObjectList);


int ApplyGravity(Object *entity, World *GameWorld);

int ApplyFriction(PhysicsBox *inputBox, float forwardFriction, float xFriction, float yFriction);


int MoveObject(Object *inputObject, World *GameWorld);

int moveObjectX(Object *input, World *GameWorld);

int moveObjectY(Object *input, World *GameWorld);

int moveObjectForward(Object *input, World *GameWorld);



// Object Behaviour
int UpdateCoin(Object *coin, World *GameWorld);

int UpdateSpring(Object *spring, World *GameWorld);


int UpdateParticle(World *GameWorld, Object *particle);

int CustomParticleBehaviour(World *GameWorld, Object *particle);


int UpdateGateSwitch(Object *gateSwitch, World *GameWorld);

int toggleGateSwitch(Object *gateSwitch, ObjectController *ObjectList);

int UpdateVerticalGate(Object *door, World *GameWorld);

int UpdateHorizontalGate(Object *gate, World *GameWorld);

int gateControl(Object *gate, ObjectController *objectList);


int UpdateHorizontalPlatform(Object *platform, World *GameWorld);

int UpdateVerticalPlatform(Object *platform, World *GameWorld);


int UpdateDoor(Object *Door, World *GameWorld);

int TeleportPlayerToExitDoor(Object *Door, World *GameWorld);

int UpdateLevelDoor(Object *Door, World *GameWorld);




// Convenience Functions
int GoTo(Object *inputObject, float destX, float destY);

int GoToWithCollision(Object *inputObject, float destX, float destY, World *GameWorld);

int SetXPosition(Object *inputObject, float newXPos);

int SetYPosition(Object *inputObject, float newYPos);

int centerOnXY(Object *input, float xPos, float yPos);

int centerOnObject(Object *input, Object *dest);

int centerOnMouse(Object *input, Camera inputCamera);


int PointObjectTowards(Object *inputObject, Object *pointDestination);

int PointObjectToMouse(Object *inputObject, World *GameWorld);

int PointObjectToXY(Object *inputObject, float xPos, float yPos);


int RotateObject(Object *inputObject, float rotationDegrees);

int SetObjectDirection(Object *inputObject, float rotationDegrees);


int setSize(Object *input, int xSize, int ySize);

int ChangeXSizeBy(int change, Object *inputObject, World *GameWorld);

int ChangeYSizeBy(int change, Object *inputObject, World *GameWorld);

int setScaleSize(Object *input, float sizePercentage);

int changeScaleSize(Object *input, float sizePercentage);

int smoothSizeChangeTo(Object *input, float desiredSize, float rate);

int matchBoxToDisplayDimensions(Object *input);


float DistanceBetween(Object *Source, Object *Target);


bool onScreen(Object *inputObject, World *GameWorld);


bool MouseOverlappingBox(Object *input, Camera inputCam);

bool MouseOverlappingSprite(Object *input, Camera inputCam);

bool MouseClickedObject(Object *input, Camera inputCam);




// Collision Detection
bool CheckBoxCollidesBox(PhysicsBox *inputBox, PhysicsBox *compareBox);

bool CheckBoxOverlapsBoxBroad(PhysicsBox *inputBox, PhysicsBox *compareBox);

bool CheckBoxOverlapsBox(PhysicsBox *inputBox, PhysicsBox *compareBox);

bool pointOverlapsWithSlope(float x, int y, PhysicsBox *slope);

bool circleOverlapsWithLine(float x1, float y1, float x2, float y2, float circleCenterX, float circleCenterY, float radius);

bool OverlapsObject(Object *inputObject, Object *otherObject);


bool evaluateIfCollidePush(PhysicsBox *movingBox, PhysicsBox *collideBox);


// Collision Resolution
int ResolveAllXCollision(PhysicsBox *movingBox, World *GameWorld);


int ResolveXCollision(PhysicsBox *movingBox, PhysicsBox *compareBox, World *GameWorld);


int ApplyXPhysics(PhysicsBox *inputBox, PhysicsBox *physicsBox);


int ResolveAllYCollision(PhysicsBox *movingBox, World *GameWorld);


int ResolveYCollision(PhysicsBox *movingBox, PhysicsBox *compareBox);


int ApplyYPhysics(PhysicsBox *inputBox, PhysicsBox *physicsBox);


int ResolveAllXCollisionsByPush(PhysicsBox *movingBox, World *GameWorld);


int ResolveAllYCollisionsByPush(PhysicsBox *movingBox, ObjectController *ObjectList);


int resolveForwardCollision(PhysicsBox *movingBox, World *GameWorld);

int ApplyForwardPhysics(PhysicsBox *inputBox, PhysicsBox *physicsBox);


Object* CheckForGround(Object *input, World *GameWorld);


void redoGroundCheck(Object *input, World *GameWorld);


int AssignDirection(PhysicsBox *inputBox, PhysicsBox *compareBox);


int ClimbSlope(PhysicsBox *inputBox, World *GameWorld);

