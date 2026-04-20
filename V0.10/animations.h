SpriteSet* loadSpriteSetFromFile(const char FileName[], SpriteSetList *setList, int desiredID);


int stopAnimation(DisplayData *inputData);


int useThisAnimation(Animation *anim, int loopCount, DisplayData *inputData);


int PlayAnimation(const char desiredName[], int loopCount, DisplayData *inputData);

int PlayObjectAnimation(const char desiredName[], int loopCount, Object *input);


int PlayAnimationByIndex(int index, int loopCount, DisplayData *inputData);

int PlayObjectAnimationByIndex(int index, int loopCount, Object *input);


int PlayNewAnimation(const char desiredName[], int loopCount, DisplayData *inputData);

int PlayNewObjectAnimation(const char desiredName[], int loopCount, Object *input);


int SwitchAnimation(const char desiredName[], int loopCount, DisplayData *inputData);


int getAnimationIndex(const char animationName[], DisplayData *inputData);


bool playingAnimation(DisplayData *inputData);

bool playingThisAnimation(DisplayData *inputData, const char name[]);

bool objectPlayingAnimation(Object *input);

bool objectPlayingThisAnimation(Object *input, const char name[]);


int iterateAnimation(DisplayData *inputData, float deltaTime);


Animation* initialiseNewAnimation(const char animationName[], float frameRate, SpriteSet *inputSet);


AnimationFrame* addSpriteToAnimation(const char spriteName[], Animation *inputAnimation, SpriteSet *sourceSet);


AnimationFrame* addSpriteToAnimationWithAttributes(const char spriteName[], Animation *inputAnimation, SpriteSet *sourceSet, float XOffset, float YOffset, float rotation);


int deleteAnimation(SpriteSet *inputSet, Animation *deleteAnimation);


// tile set stuff

int makeNewBackgroundTileMap(TilePlane *input, int gridWidth, int gridHeight);

int loadTileSetBackground(const char *name, BackgroundData *bg);