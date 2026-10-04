int initialiseCutscene(CutsceneID inputID, World *GameWorld);

int initialiseCutsceneFromFile(const char sceneName[], World *GameWorld);

int LoadCutsceneFromFile(const char sceneName[], World *GameWorld);

int updateCutscene(World *GameWorld);

int updateSceneActions(World *GameWorld);

int SkipSceneActions(int skipCount, World *GameWorld);

FuncResult RunSceneAction(World *GameWorld);

bool ConditionIsTrue(ConditionalStatement *input);


SceneAction* loadSceneAction(char inputString[MAX_LEN], World *GameWorld, FILE *fPtr);

int loadBracketedSceneActions(FILE *fPtr, World *GameWorld);

int LoadConditionalStatement(FILE *fPtr, ConditionalStatement *input);



const char* getSceneActionName(SceneActionID input);

int EndCutscene(World *GameWorld);

SceneAction* SceneAction_SwitchCutscene(int sceneID, World *GameWorld);

SceneAction* SceneAction_TriggerGameEvent(GameEvent *inputEvent, World *GameWorld);

SceneAction* enablePlayer(World *GameWorld);

SceneAction* disablePlayer(World *GameWorld);

SceneAction* Wait(float seconds, World *GameWorld);

SceneAction* Repeat(int repeatTimes, int instructions, World *GameWorld);

SceneAction* RepeatUntil(ConditionalStatement condition, int instructions, World *GameWorld);

SceneAction* RepeatWhile(ConditionalStatement condition, int instructions, World *GameWorld);

SceneAction* setVariableTo(int variableIndex, int value, World *GameWorld);

SceneAction* changeVariableBy(int variableIndex, int value, World *GameWorld);

SceneAction* SceneAction_SayText(TextBox *text, World *GameWorld);

SceneAction* AnimateActor(char objName[], const char animName[], int loopCount, World *GameWorld);

SceneAction* AnimateActorAndWait(char objName[], const char animName[], int loopCount, World *GameWorld);

SceneAction* SwitchActorSprite(char objName[], const char spriteName[], World *GameWorld);

SceneAction* SetActorPosition(char objName[], float xPosition, float yPosition, World *GameWorld);

SceneAction* MoveActor(char objName[], float xMovement, float yMovement, World *GameWorld);

SceneAction* MoveActorX(char objName[], float xMovement, World *GameWorld);

SceneAction* MoveActorY(char objName[], float yMovement, World *GameWorld);

SceneAction* SetActorDirection(char objName[], double rotation, World *GameWorld);

SceneAction* RotateActor(char objName[], double rotation, World *GameWorld);

SceneAction* HideActor(char objName[], World *GameWorld);

SceneAction* ShowActor(char objName[], World *GameWorld);

SceneAction* SetActorLayer(char objName[], Layer destLayer, World *GameWorld);

SceneAction* CreateActor(char objName[], ObjectType actorID, float xPos, float yPos, World *GameWorld);

SceneAction* ReleaseActor(char objName[], World *GameWorld);

SceneAction* RenameActor(char objName[], const char newName[], World *GameWorld);

SceneAction* placeInvisibleWall(int xPos, int yPos, int xSize, int ySize, World *GameWorld);


SceneAction* SceneAction_PlaySound(char soundName[], ChannelName soundChannel, float volume, World *GameWorld);

SceneAction* SceneAction_PlaySoundRepeat(char soundName[], ChannelName soundChannel, float volume, int repeatTimes, World *GameWorld);

SceneAction* SceneAction_SetSoundChannelVolume(ChannelName soundChannel, float newVolume, World *GameWorld);

SceneAction* SceneAction_ChangeSoundChannelVolume(ChannelName soundChannel, float change, World *GameWorld);

SceneAction* SceneAction_SetCameraPosition(float xPos, float yPos, World *GameWorld);

SceneAction* SceneAction_MoveCamera(float xVel, float yVel, World *GameWorld);

SceneAction* SceneAction_MoveCameraTo(float xPos, float yPos, float coefficient, World *GameWorld);

SceneAction* SceneAction_MoveCameraToObject(char objectName[], float coefficient, World *GameWorld);

SceneAction* SceneAction_SetCameraMode(int mode, World *GameWorld);

SceneAction* SceneAction_SetZoom(float zoomX, float zoomY, World *GameWorld);

SceneAction* SceneAction_ChangeZoom(float zoomX, float zoomY, World *GameWorld);

SceneAction* SceneAction_ChangeZoomTo(float zoomX, float zoomY, float coefficient, World *GameWorld);


SceneAction* createSceneAction(SceneActionID newActionID, World *GameWorld);

SceneAction* deleteSceneAction(SceneAction *deleteAction, World *GameWorld);

int deleteAllSceneActions(World *GameWorld);