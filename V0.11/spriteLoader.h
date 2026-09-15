int switchBackGroundSprite(int spriteID, int desiredSetID, BackgroundData *inputData);


int switchBackGroundSpriteName(const char spriteName[], int desiredSetID, BackgroundData *inputData);


int loadSpriteFromPath(Sprite *inputSprite, const char inputPath[]);


void initialiseSpriteSetList(SpriteSetList *input);

SpriteSet* getSpriteSet(SpriteSetList *setList, int desiredSetID);

SpriteSet* createNewSpriteSet(SpriteSetList *setList, int desiredSetID);


Sprite* loadSprite(const char spriteName[], const char folderName[], RenderMode renderMode);


// Loads sprite directly into provided spriteSet (inputSet)
Sprite* loadSpriteIntoSpriteSet(const char spriteName[], const char folderName[], SpriteSet *inputSet, RenderMode renderMode);

// Do not put char arrays that dont end with a null terminator; it doesnt check for this
int removeFileExtension(char input[], const char fileName[]);


int compareFileNames(const char firstInput[], const char secondInput[]);


int getSpriteIndex(const char spriteName[], DisplayData *inputData);

int getSpriteIndexSpriteSet(const char spriteName[], SpriteSet *currentSet);

Sprite* getSpriteSpriteSet(const char spriteName[], SpriteSet *currentSet);

int switchSprite(int spriteID, int spriteSet, DisplayData *inputData);

int switchSpriteByName(const char spriteName[], int spriteSet, DisplayData *inputData);


Sprite* loadBackGroundSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, BackgroundData *inputData);

Sprite* loadObjectSprite(const char spriteName[], SpriteSet *inputSet, RenderMode renderMode);


int deleteSprite(SpriteSet *spriteSet, Sprite *input);


int deleteSpriteSet(SpriteSet *inputSet, SpriteSetList *setList);

int deleteAllSpriteSets(SpriteSetList *setList);

int deleteExcessSpriteSets(ObjectController *ObjectList, int numberToKeep);

