int loadLevel(World *GameWorld, int level);

int loadPartition(World *GameWorld, int partID);

int loadSave(int saveFile, int flags[GAME_FLAG_COUNT], World *GameWorld);

int loadSettings(int settingsFile, World *GameWorld);

int loadSaveData(const char *fileName, int flags[GAME_FLAG_COUNT], World *GameWorld);

int loadLevelData(World *GameWorld, FILE *fPtr);

int loadLevelDataChunk(World *GameWorld, FILE *fPtr, int lineLimit);

int saveGameState(World *GameWorld);

int loadGameState(World *GameWorld);

int writeObjectIndices(Object *input, FILE *file);

int loadObjectIndices(Object *input, ObjectController *ObjectList, FILE *file);

int checkFileHeader(FILE *fPtr, const char FileType[]);

void closeFile(FILE *file);

FILE* openFile(const char fileName[], const char rootPath[], const char header[]);

int getCurrentLineNumber(FILE *fPtr);

int skipCommentInFile(FILE *fPtr);

int clearLevelData(World *GameWorld);

int logLevel(World *GameWorld);

int lemon_Modulo(int x, int N);

FILE* convertTxtToLem(char FileName[MAX_LEN], FILE *txtPtr);

FILE* convertLemToTxt(char fileName[MAX_LEN + 4], FILE *lemPtr);


int loadObject(World *GameWorld, FILE *fPtr, int xOffset, int yOffset);

int loadRepeatingObject(World *GameWorld, FILE *fPtr, int *objectsLoaded);

int loadLevelFlag(World *GameWorld, FILE *fPtr);

int loadConditionalStatement(World *GameWorld, FILE *fPtr);

int readBranch(World *GameWorld, FILE *fPtr, bool conditionMet);

bool bracketedStatementPresent(FILE *fPtr, const char expectedPhrase[]);

int consumeStatement(FILE *fPtr, char stopCharacter);

int ConvertEntryToObjectID(char entry[MAX_LEN]);

const char* ConvertIDToObjectName(ObjectType input);

const char* ConvertSolidTypeToName(SolidType input);

const char* ConvertSolidFlagToName(SolidFlag input);


int CheckObjectLoadCommands(FILE *fPtr, Object *inputObject);

int ApplyObjectLoadCommand(Object *inputObject, char command[MAX_LEN]);


int atEndOfLine(FILE *fPtr);

bool endOfFile(FILE *fPtr, char buffer[]);

int readIntArgs(FILE *fPtr, int argsDest[], int number);

int getNextArg(FILE *fPtr, char buffer[], int capacity);

int getNextArgInt(FILE *fPtr);

bool hasNextArgInt(FILE *fPtr);

float getNextArgFloat(FILE *fPtr);

int convertStrToInt(char str[], int size);

int convertIntToStr(char str[], int input);


int loadTestScene(World *GameWorld);
