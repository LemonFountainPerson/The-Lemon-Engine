int switchBackGroundSprite(int spriteID, int desiredSetID, BackgroundData *inputData);


int switchBackGroundSpriteName(const char spriteName[], int desiredSetID, BackgroundData *inputData);


int loadSpriteFromPath(Sprite *inputSprite, const char inputPath[]);


SpriteSet* createSpriteSet(SpriteSet **firstSet, int desiredSetID);


Sprite* loadSprite(const char spriteName[], const char folderName[], RenderMode renderMode);

// Loads sprite into spriteset determined from desiredSetID, searched for at spriteSetSource
int loadSpriteIntoDesiredSet(const char spriteName[], const char folderName[], SpriteSet **spriteSetSource, int desiredSetID, RenderMode renderMode);

// Loads sprite directly into provided spriteSet (inputSet)
int loadSpriteIntoSpriteSet(const char spriteName[], const char folderName[], SpriteSet *inputSet, RenderMode renderMode);

// Do not put char arrays that dont end with a null terminator; it doesnt check for this
int removeFileExtension(char input[], const char fileName[]);


int compareFileNames(const char firstInput[], const char secondInput[]);


int getSpriteIndex(const char spriteName[], DisplayData *inputData);

int getSpriteIndexSpriteSet(const char spriteName[], SpriteSet *currentSet);

Sprite* getSpriteSpriteSet(const char spriteName[], SpriteSet *currentSet);

int switchSprite(int spriteID, int spriteSet, DisplayData *inputData);

int switchSpriteByName(const char spriteName[], int spriteSet, DisplayData *inputData);


int loadBackGroundSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, BackgroundData *inputData);


int loadObjectSprite(const char spriteName[], SpriteSet *inputSet, RenderMode renderMode);


int loadPlayerSprite(const char spriteName[], int desiredSetID, RenderMode renderMode, PlayerData *player);


int deleteSprite(SpriteSet *spriteSet, Sprite *input);


int deleteSpriteSet(SpriteSet *inputSet, ObjectController *ObjectList);


int deleteExcessSpriteSets(ObjectController *ObjectList, int numberToKeep);


RenderMode convertStringToRenderMode(char string[]);

const char* getRenderModeName(RenderMode input);