#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "spriteLoader.h"
#include "LemonMain.h"
#include "UIObjects.h"
#include "playerController.h"
#include "animations.h"
#include "soundProcessor.h"
#endif


int StartCutscene(CutsceneID inputID, World *GameWorld);

int UpdateCutscene(World *GameWorld);

int updateSceneActions(World *GameWorld);

int RunSceneAction(SceneAction *inputAction, World *GameWorld);

int EndCutscene(World *GameWorld);


int WaitUntil(SceneAction *inputAction);

int Repeat(int repeatCount, int instructionCount, ...);

SceneAction* Wait(float seconds, World *GameWorld);

// branching based on variable
SceneAction* ifEquals(int* variable, int value, CutsceneID cutsceneToTrigger, World *GameWorld);

SceneAction* ifEqualsElse(int* variable, int value, CutsceneID cutsceneIfTrue, CutsceneID cutsceneIfElse, World *GameWorld);

SceneAction* ifNotEquals(int* variable, int value, CutsceneID cutsceneToTrigger, World *GameWorld);

SceneAction* ifNotEqualsElse(int* variable, int value, CutsceneID cutsceneIfTrue, CutsceneID cutsceneIfElse, World *GameWorld);

SceneAction* ifLessThan(int* variable, int value, CutsceneID cutsceneToTrigger, World *GameWorld);

SceneAction* ifLessThanElse(int* variable, int value, CutsceneID cutsceneIfTrue, CutsceneID cutsceneIfElse, World *GameWorld);

SceneAction* ifGreaterThan(int* variable, int value, CutsceneID cutsceneToTrigger, World *GameWorld);

SceneAction* ifGreaterThanElse(int* variable, int value, CutsceneID cutsceneIfTrue, CutsceneID cutsceneIfElse, World *GameWorld);

SceneAction* ifFloatEquals(float* variable, float value, CutsceneID cutsceneToTrigger, World *GameWorld);

SceneAction* ifFloatEqualsElse(float* variable, float value, CutsceneID cutsceneIfTrue, CutsceneID cutsceneIfElse, World *GameWorld);


SceneAction* SceneAction_SayText(TextInstance *text, World *GameWorld);

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

SceneAction* CreateActor(char objName[], ObjectType actorID, int xPos, int yPos, World *GameWorld);

SceneAction* SetActorLayer(char objName[], Layer destLayer, World *GameWorld);

SceneAction* SceneAction_PlaySound(char soundName[], char folderName[], ChannelName soundChannel, float volume, World *GameWorld);

SceneAction* SceneAction_PlaySoundRepeat(char soundName[], char folderName[], ChannelName soundChannel, float volume, int repeatTimes, World *GameWorld);

SceneAction* SceneAction_SetSoundChannelVolume(ChannelName soundChannel, float newVolume, World *GameWorld);

SceneAction* SceneAction_FadeSoundChannel(ChannelName soundChannel, float fadeValue, int repeatTimes, World *GameWorld);

SceneAction* SceneAction_SetCameraPosition(float xPos, float yPos, World *GameWorld);

SceneAction* SceneAction_MoveCamera(float xVel, float yVel, int repeatTimes, World *GameWorld);

SceneAction* SceneAction_MoveCameraSmooth(float xPos, float yPos, float coefficient, World *GameWorld);

SceneAction* SceneAction_SetZoom(float zoomX, float zoomY, World *GameWorld);

SceneAction* SceneAction_ChangeZoom(float zoomX, float zoomY, int repeatTimes, World *GameWorld);


SceneAction* createSceneAction(SceneActionID newActionID, World *GameWorld);

SceneAction* deleteSceneAction(SceneAction *deleteAction, World *GameWorld);

int deleteAllSceneActions(World *GameWorld);