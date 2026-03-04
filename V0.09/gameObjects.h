// Object Creation
// Creates new instance of an object and puts at the end of the object list
Object* AddObject(World *GameWorld, int objectID, int xPos, int yPos, int arg1, int arg2, int arg3, int arg4, int arg5);

Object* AddNamedObject(World *GameWorld, const char name[], int objectID, int xPos, int yPos);

Object* AddObjectWithParent(World *GameWorld, Object *ParentObject, int objectID, int xPos, int yPos, int arg1, int arg2, int arg3, int arg4, int arg5);

Object* AddParticle(World *GameWorld, ParticleSubType animation, int xPos, int yPos, int repeatCount, int particleLifeTime);


// Initialises a new empty object to the object list
Object* getNewObject(ObjectType objectID, ObjectController *ObjectList);

int initialiseGenericObject(Object *inputObject, ObjectType objectID, ObjectController *ObjectList);

void clearObjectData(Object *input);

Object* createNewObject(void);

Object* findNewObject(ObjectController *ObjectList);


int setObjectName(Object *inputObject, const char name[]);

Object* FindObject(const char name[], ObjectController ObjectList);

Object* FindObjectID(const char name[], int objectID, ObjectController ObjectList);


int snapPositionToTileGrid(Object *input, int xPos, int yPos);


PhysicsBox* createPhysicsBox(SolidType inputSolid);

int resetPhysicsBox(PhysicsBox *input);

DisplayData* createDisplayData(RenderMode startRenderMode);

int resetDisplayData(DisplayData *input);


DisplayData* getDisplay(Object *input);

Layer getDisplayLayer(Object *input);

bool setDisplayLayer(Object *input, Layer newLayer);

RenderMode getRenderModeOverride(Object *input);

bool setRenderModeOverride(Object *input, RenderMode newMode);

bool setTransparency(Object *input, float transparency);

bool changeTransparency(Object *input, float transparency);

float getTransparency(Object *input);

bool hideObject(Object *input);

bool showObject(Object *input);

bool toggleHidden(Object *input);

int getSubType(Object *input);

int setSubType(Object *input, int subType);


// Helper function to set an object to a moving platform type 
Object* InitialiseMovingPlatform(Object *inputObject, int objectID, int xPos, int yPos, int bound1, int bound2, int speed, int timer);


int InitialiseParticle(Object *particle, int animation, int repeatCount, int particleLifeTime);


// Attempts to create object sprite set if it does not already exist
SpriteSet* createObjectSpriteSet(ObjectController *ObjectList, int ObjectID);


// Object Attribute Modifiers
int switchObjectSprite(int spriteID, Object *inputObject);

int switchObjectSpriteByName(const char spriteName[], Object *inputObject);


void removeSceneActionReferences(Object *input, World *GameWorld);

void deleteAssociatedFrameFunction(Object *input, ObjectController *ObjectList);

Object* deleteObject(Object *input, ObjectController *objController);

void deleteAllObjects(ObjectController *objectList);

void deleteAllObjectsSafe(ObjectController *ObjectList);

void deleteAllEnvironmentObjects(ObjectController *ObjectList);


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

int deleteAllCachedObjects(ObjectController *ObjectList);

int cacheObjects(ObjectController *ObjectList, PhysicsBox boundingBox);



// Convenience Functions
int GoTo(Object *inputObject, float destX, float destY);

int GoToWithCollision(Object *inputObject, float destX, float destY, ObjectController *ObjectList);

int SetXPosition(Object *inputObject, float newXPos);

int SetYPosition(Object *inputObject, float newYPos);

int centerOnXY(Object *input, float xPos, float yPos);

int centerOnObject(Object *input, Object *dest);


int PointObjectTowards(Object *inputObject, Object *pointDestination);

int PointObjectToMouse(Object *inputObject, World *GameWorld);

int PointObjectToXY(Object *inputObject, float xPos, float yPos);


int RotateObject(Object *inputObject, float rotationDegrees);

int SetObjectDirection(Object *inputObject, float rotationDegrees);


int setSize(Object *input, int xSize, int ySize);

int ChangeObjectXSizeBy(int change, Object *inputObject, ObjectController *ObjectList);

int ChangeObjectYSizeBy(int change, Object *inputObject, ObjectController *ObjectList);

int setScaleSize(Object *input, float sizePercentage);

int changeScaleSize(Object *input, float sizePercentage);

int smoothSizeChangeTo(Object *input, float desiredSize, float rate);

int matchBoxToDisplayDimensions(Object *input);


float DistanceBetween(Object *Source, Object *Target);


bool onScreen(Object *inputObject, World *GameWorld);


bool MouseOverlappingBox(Object *input, Camera inputCam);

bool MouseOverlappingSprite(Object *input, Camera inputCam);

bool MouseClickedObject(Object *input, Camera inputCam);


// Core Object Functionality
FuncResult updateObjects(World *GameWorld);


int ObjectBehaviour(World *GameWorld, Object *inputObject);

int UpdateObjectDisplay(Object *inputObject);


int updatePreviousPositions(ObjectController *ObjectList);

int updatePreviousPosition(Object *input);

int updateObjectsState(ObjectController *ObjectList, World *GameWorld);

int UpdatePhysicsState(Object *inputObject, World *GameWorld);

int applyMagnetisation(PhysicsBox *inputBox, PhysicsBox *GroundBox, World *GameWorld);

int ResolveAllObjects(ObjectController *ObjectList, WorldPhysics pType);


int UpdateParentChildLink(Object *inputObject);


int initialiseComponents(ObjectController *input);

int initialiseSparseList(SparseList *input);

int removeComponents(Object *input, ObjectController *ObjectList);

ComponentType* addComponent(Object *input, SparseList *List);

int removeComponent(Object *input, SparseList *List);

ComponentType* getComponent(Object *input, SparseList *List);

int addHealthComponent(Object *input, int Health, ObjectController *inputData);

int removeHealthComponent(Object *input, ObjectController *inputData);

HealthComponent* getHealthComponent(Object *input, ObjectController *inputData);

int takeDamageStandard(int damage, Object *input, HealthComponent *HP);

int inflictDamage(int damage, Object *input, ObjectController *ObjectList);


int addTileMap(Object *input, unsigned int centerTileX, unsigned int centerTileY, unsigned int tileSize, ObjectController *inputData);

int removeTileMap(Object *input, ObjectController *inputData);

TileMap* getTileMap(Object *input, ObjectController *inputData);


FuncResult updateObjectsFrame(World *GameWorld);

int ExecuteFrameUpdateFunction(FrameUpdateFunction *inputFunction, World *GameWorld);

int AddFrameUpdateFunction(TriggerableFunction FunctionPointer, Object *inputObject, ObjectController *ObjectList);

int deleteAllFrameUpdateFunctions(ObjectController *ObjectList);



int moveObjectX(PhysicsBox *inputBox, ObjectController *ObjectList);

int moveObjectY(PhysicsBox *inputBox, ObjectController *ObjectList);

int moveObjectForward(PhysicsBox *movingBox, ObjectController *ObjectList);



// Object Behaviour
int UpdateParticle(World *GameWorld, Object *particle);

int CustomParticleBehaviour(World *GameWorld, Object *particle);


int UpdateHorizontalPlatform(Object *platform);

int UpdateVerticalPlatform(Object *platform);


int UpdateGateSwitch(PlayerData player, Object *gateSwitch, ObjectController *ObjectList);

int toggleGateSwitch(Object *gateSwitch, ObjectController *ObjectList);

int UpdateVerticalGate(Object *door, World *GameWorld);

int UpdateHorizontalGate(Object *gate, World *GameWorld);

int gateControl(Object *gate, ObjectController *objectList);


int UpdateDoor(PlayerData Player, Object *Door, World *GameWorld);

int TeleportPlayerToExitDoor(Object *Door, World *GameWorld);

int UpdateLevelDoor(PlayerData Player, Object *Door, World *GameWorld);


int UpdateSpring(Object *spring, World *GameWorld);


int UpdateCoin(Object *coin, World *GameWorld);



// Components
int MoveObject(Object *inputObject, World *GameWorld);


int ApplyGravity(Object *entity, World *GameWorld);


int ApplyFriction(PhysicsBox *inputBox, float forwardFriction, float xFriction, float yFriction);



// Collision Detection
Object* GetCollidingObject(PhysicsBox *inputBox, ObjectController *ObjectList);

int putObjectIntoCollisionGrid(Object *input, ObjectController *list);

int* getObjectIndicesAtGridCell(float xPos, float yPos, ObjectController *list);


int CheckBoxCollidesBox(PhysicsBox *inputBox, PhysicsBox *compareBox);

bool checkBoxOverlapsBoxBroad(PhysicsBox *inputBox, PhysicsBox *compareBox);

bool CheckBoxOverlapsBox(PhysicsBox *inputBox, PhysicsBox *compareBox);


Object* GetOverlappingObject(PhysicsBox *inputBox, ObjectController *ObjectList);


Object* GetOverlappingObjectType(PhysicsBox *inputBox, int overlapObjectID, ObjectController *objectList);


Object* GetOverlappingObjectSolid(PhysicsBox *inputBox, int solidID, ObjectController *objectList);


Object* GetOverlappingObjectAllSolids(PhysicsBox *inputBox, ObjectController *ObjectList);


bool OverlapsObject(Object *inputObject, Object *otherObject);


CollideType evaluateCollideMode(PhysicsBox *movingBox, PhysicsBox *collideBox);


// Collision Resolution
int ResolveAllXCollision(PhysicsBox *movingBox, ObjectController *ObjectList);


int ResolveXCollision(PhysicsBox *movingBox, PhysicsBox *compareBox, ObjectController *ObjectList);


int ApplyXPhysics(PhysicsBox *inputBox, PhysicsBox *physicsBox);


int ResolveAllYCollision(PhysicsBox *movingBox, ObjectController *ObjectList);


int ResolveYCollision(PhysicsBox *movingBox, PhysicsBox *compareBox);


int ApplyYPhysics(PhysicsBox *inputBox, PhysicsBox *physicsBox);


int ResolveXCollisionByPush(PhysicsBox *movingBox, PhysicsBox *compareBox);


int ResolveYCollisionByPush(PhysicsBox *movingBox, PhysicsBox *compareBox);


int ResolveAllXCollisionsByPush(PhysicsBox *movingBox, ObjectController *ObjectList);


int ResolveAllYCollisionsByPush(PhysicsBox *movingBox, ObjectController *ObjectList);


int ApplyForwardPhysics(PhysicsBox *inputBox, PhysicsBox *physicsBox);


Object* CheckForGround(PhysicsBox *movingBox, World *GameWorld);


int resetGroundCheck(PhysicsBox *inputBox, World *GameWorld);


int AdjustDirection(PhysicsBox *movingBox, World *GameWorld);


int AssignDirection(PhysicsBox *inputBox, PhysicsBox *compareBox);


int ClimbSlope(PhysicsBox *inputBox, PhysicsBox *compareBox, ObjectController *ObjectList);

