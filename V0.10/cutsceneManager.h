int initialiseCutscene(CutsceneID inputID, World *GameWorld);

int initialiseCutsceneFromFile(const char sceneName[], World *GameWorld);

int LoadCutsceneFromFile(const char sceneName[], World *GameWorld);

SceneAction* loadSceneAction(char inputString[MAX_LEN], char textBoxString[MAX_LEN], World *GameWorld, FILE *fPtr);

int UpdateCutscene(World *GameWorld);

int updateSceneActions(World *GameWorld);

int RunSceneAction(SceneAction *inputAction, World *GameWorld);

const char* getSceneActionName(SceneActionID input);


int EndCutscene(World *GameWorld);

int WaitUntil(SceneAction *inputAction);

int Repeat(int repeatCount, int instructionCount, ...);

int RepeatArray(int repeatCount, int instructionCount, SceneAction *list);

SceneAction* enablePlayer(World *GameWorld);

SceneAction* disablePlayer(World *GameWorld);

SceneAction* Wait(float seconds, World *GameWorld);

SceneAction* setVariableTo(int variableIndex, int value, World *GameWorld);

SceneAction* changeVariableBy(int variableIndex, int value, World *GameWorld);

// branching based on variable
SceneAction* mapSymbolToIfAction(int variableIndex, int value, CutsceneID ifTrue, CutsceneID ifFalse, const char operator[], World *GameWorld);

SceneAction* ifEquals(int variableIndex, int value, CutsceneID cutsceneToTrigger, World *GameWorld);

SceneAction* ifNotEquals(int variableIndex, int value, CutsceneID cutsceneToTrigger, World *GameWorld);

SceneAction* ifLessThan(int variableIndex, int value, CutsceneID cutsceneToTrigger, World *GameWorld);

SceneAction* ifLessThanEquals(int variableIndex, int value, CutsceneID cutsceneToTrigger, World *GameWorld);

SceneAction* ifGreaterThan(int variableIndex, int value, CutsceneID cutsceneToTrigger, World *GameWorld);

SceneAction* ifGreaterThanEquals(int variableIndex, int value, CutsceneID cutsceneToTrigger, World *GameWorld);

SceneAction* ifFloatEquals(float* variable, float value, CutsceneID cutsceneToTrigger, World *GameWorld);

SceneAction* ifElse(SceneAction *inputIfStatement, CutsceneID cutsceneIfElse);


SceneAction* SceneAction_SayText(TextBox *text, World *GameWorld);


SceneAction* AnimateActor(char objName[], const char animName[], int loopCount, World *GameWorld);

SceneAction* SwitchActorSprite(char objName[], const char spriteName[], World *GameWorld);

SceneAction* SetActorPosition(char objName[], float xPosition, float yPosition, World *GameWorld);

SceneAction* MoveActor(char objName[], float xMovement, float yMovement, short repeatTimes, World *GameWorld);

SceneAction* MoveActorX(char objName[], float xMovement, short repeatTimes, World *GameWorld);

SceneAction* MoveActorY(char objName[], float yMovement, short repeatTimes, World *GameWorld);

SceneAction* SetActorDirection(char objName[], double rotation, World *GameWorld);

SceneAction* RotateActor(char objName[], double rotation, short repeatTimes, World *GameWorld);

SceneAction* HideActor(char objName[], World *GameWorld);

SceneAction* ShowActor(char objName[], World *GameWorld);

SceneAction* SetActorLayer(char objName[], Layer destLayer, World *GameWorld);

SceneAction* CreateActor(char objName[], ObjectType actorID, float xPos, float yPos, World *GameWorld);

SceneAction* ReleaseActor(char objName[], World *GameWorld);

SceneAction* placeInvisibleWall(int xPos, int yPos, int xSize, int ySize, World *GameWorld);


SceneAction* SceneAction_PlaySound(char soundName[], char folderName[], ChannelName soundChannel, float volume, World *GameWorld);

SceneAction* SceneAction_PlaySoundRepeat(char soundName[], char folderName[], ChannelName soundChannel, float volume, int repeatTimes, World *GameWorld);

SceneAction* SceneAction_SetSoundChannelVolume(ChannelName soundChannel, float newVolume, World *GameWorld);

SceneAction* SceneAction_ChangeSoundChannelVolume(ChannelName soundChannel, float change, World *GameWorld);

SceneAction* SceneAction_FadeSoundChannel(ChannelName soundChannel, float fadeValue, int repeatTimes, World *GameWorld);

SceneAction* SceneAction_SetCameraPosition(float xPos, float yPos, World *GameWorld);

SceneAction* SceneAction_MoveCamera(float xVel, float yVel, int repeatTimes, World *GameWorld);

SceneAction* SceneAction_MoveCameraSmooth(float xPos, float yPos, float coefficient, World *GameWorld);

SceneAction* SceneAction_SetZoom(float zoomX, float zoomY, World *GameWorld);

SceneAction* SceneAction_ChangeZoom(float zoomX, float zoomY, int repeatTimes, World *GameWorld);


SceneAction* createSceneAction(SceneActionID newActionID, World *GameWorld);

SceneAction* deleteSceneAction(SceneAction *deleteAction, World *GameWorld);

int deleteAllSceneActions(World *GameWorld);