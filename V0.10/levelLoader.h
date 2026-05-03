int loadLevel(World *GameWorld, int level);

int loadPartition(World *GameWorld, int partID);

int loadSave(int saveFile, World *GameWorld);

int loadSettings(int settingsFile, World *GameWorld);

int saveGame(int saveFile, World *GameWorld);

int saveSettings(int saveFile, World *GameWorld);

int loadSaveData(const char *fileName, World *GameWorld);

int loadLevelData(World *GameWorld, FILE *fPtr);

int loadLevelDataChunk(World *GameWorld, FILE *fPtr, int lineLimit);

int saveGameState(World *GameWorld);

int loadGameState(World *GameWorld);

int writeObjectIndices(Object *input, FILE *file);

int loadObjectIndices(Object *input, ObjectController *ObjectList, FILE *file);

int checkFileHeader(FILE *fPtr, const char FileType[]);

void encodeLEMFile(FILE *file, const char name[MAX_LEN * 2]);

FILE* decodeLEMFile(FILE *file);

FILE* openFile(const char fileName[], const char rootPath[], const char header[]);

void closeFile(FILE *file);

int getCurrentLineNumber(FILE *fPtr);

int skipCommentInFile(FILE *fPtr);

int clearLevelData(World *GameWorld);

int logLevel(World *GameWorld);

int lemon_Modulo(int x, int N);

FILE* convertTxtToLem(char FileName[MAX_LEN], FILE *txtPtr);

FILE* convertLemToTxt(char fileName[MAX_LEN + 4], FILE *lemPtr);


int loadObject(World *GameWorld, FILE *fPtr, int xOffset, int yOffset);

int loadRepeatingObject(World *GameWorld, FILE *fPtr, int *objectsLoaded);

int loadObjectRepeated(World *GameWorld, FILE *fPtr);

int getNextArgGameFlag(FILE *fPtr);

int loadLevelFlag(World *GameWorld, FILE *fPtr);

int loadConditionalStatement(World *GameWorld, FILE *fPtr);

int readBranch(World *GameWorld, FILE *fPtr, bool conditionMet);

bool bracketedStatementPresent(FILE *fPtr, const char expectedPhrase[]);

int consumeStatement(FILE *fPtr, char stopCharacter);


int atEndOfLine(FILE *fPtr);

bool endOfFile(FILE *fPtr);

void moveFilePos(FILE *file, int offset);

void setFilePos(FILE *file, int pos);

void returnFilePos(FILE *file);

int readIntArgs(FILE *fPtr, int argsDest[], int number);

int getNextArg(FILE *fPtr, char buffer[], int capacity);

int getNextArgInt(FILE *fPtr);

bool hasNextArgNumber(FILE *fPtr);

bool getNextArgBool(FILE *fPtr);

float getNextArgFloat(FILE *fPtr);

int convertStrToInt(const char str[], int size);

int convertIntToStr(char str[], int input);


int loadTestScene(World *GameWorld);
