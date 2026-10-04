EXPORT void InitialiseObjectController(ObjectController *newController);

void ClearObjectController(ObjectController *ObjectList);

void deleteAllObjects(ObjectController *objectList);

void deleteLevelObjects(ObjectController *ObjectList);

void deleteAllEnvironmentObjects(ObjectController *ObjectList);


void InitialiseBSPTree(BSPTree *input);

void DeleteBSPNode(BSPNode *node);

int ClearBSPTree(BSPTree *input);

int BSPComparison(PhysicsBox *inputBox, BSPNode *node);

int BuildBSPTree(BSPTree *input, Object *firstObject);

BSPNode* BuildSubBSPTree(SDL_FRect nodeArea, int height, char axis, int totalObjects, Object *firstObject);

int AddObjectToBSPTree(Object *input, BSPTree *tree);

int RemoveObjectFromBSPTree(Object *input, BSPTree *tree);

void renderBSPNode(BSPNode *input, Camera inputCamera, SDL_Renderer *Screen);


// Collision Detection
Object* GetCollidingObject(PhysicsBox *inputBox, ObjectController *ObjectList);

Object* GetCollidingObjectFull(PhysicsBox *inputBox, ObjectController *ObjectList);

Object* GetOverlappingObject(PhysicsBox *inputBox, ObjectController *ObjectList);

Object* GetOverlappingObjectType(PhysicsBox *inputBox, int overlapObjectID, ObjectController *objectList);

Object* GetOverlappingSolid(PhysicsBox *inputBox, int solidID, ObjectController *ObjectList);

Object* GetOverlappingObjectAllSolids(PhysicsBox *inputBox, ObjectController *ObjectList);



int InitialiseComponents(ObjectController *ObjectList);

void InitialiseSparseList(SparseList *input, const char name[]);

int removeComponents(Object *input, ObjectController *ObjectList);

ComponentType* addComponent(Object *input, SparseList *List);

int removeComponent(Object *input, SparseList *List);

ComponentType* getComponentWithIndex(int index, SparseList *List);
	
ComponentType* getComponent(Object *input, SparseList *List);


GameEvent* addObjectEvent(Object *input, bool triggerOnce, World *GameWorld);

GameEvent* getObjectEvent(Object *input, World *GameWorld);

bool hasObjectEvent(Object *input, World *GameWorld);

void triggerObjectEvent(Object *input, World *GameWorld);



PhysicsComponent* addPhysics(Object *input, bool gravity, World *GameWorld);

PhysicsComponent* addPhysicsDefault(Object *input, World *GameWorld);

PhysicsComponent* getPhysicsComponent(Object *input, World *GameWorld);

bool HasPhysics(Object *input, World *GameWorld);

bool HasGravity(Object *input, World *GameWorld);

void SetPhysicsGravity(Object *input, bool gravity, World *GameWorld);

void updatePhysicsComponents(World *GameWorld);


Polygon* addPolygon(Object *input, World *GameWorld, int numOfVertices, ...);

Polygon* addQuad(Object *input, World *GameWorld);

Polygon* getPolygon(Object *input, World *GameWorld);

void movePolygonVertex(Object *input, int vertex, float newX, float newY, World *GameWorld);

SDL_Vertex* getPolygonVertex(Object *input, int vertex, World *GameWorld);


HealthComponent* addHealthComponent(Object *input, int Health, GroupType group, World *GameWorld);

HealthComponent* getHealthComponent(Object *input, World *GameWorld);

GroupType getGroupAffiliation(Object *input, World *GameWorld);

int inflictDamage(int damage, Object *input, World *GameWorld);

bool isHurt(Object *input, World *GameWorld);


BulletComponent* addBulletComponent(Object *input, Object *owner, int damage, ParticleSubType particleType, World *GameWorld);

bool isBullet(Object *input, World *GameWorld);

void bulletCollision(Object *bulletObject, World *GameWorld);


int addTileMap(Object *input, int centerTileX, int centerTileY, int tileSize, World *GameWorld);

TileMap* getTileMap(Object *input, World *GameWorld);


int startTimer(int ticks, Object *input, World *GameWorld);

int startTimerSeconds(float seconds, Object *input, World *GameWorld);

bool timerExpired(Object *input, World *GameWorld);

Uint64 checkTimer(Object *input, World *GameWorld);

Timer* getTimer(Object *input, World *GameWorld);

int endTimer(Object *input, World *GameWorld);

void pauseTimers(ComponentData *data, World *GameWorld);


int startStopWatch(Object *input, World *GameWorld);

float checkStopWatch(Object *input, World *GameWorld);

void pauseStopWatch(Object *input, World *GameWorld);

void unpauseStopWatch(Object *input, World *GameWorld);

float endStopWatch(Object *input, World *GameWorld);

