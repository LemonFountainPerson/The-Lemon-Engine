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

SceneAction* SceneAction_SayText(TextInstance *text, World *GameWorld);

SceneAction* AnimateActor(char objName[], const char animName[], int loopCount, World *GameWorld);

SceneAction* SwitchActorSprite(char objName[], const char spriteName[], World *GameWorld);

SceneAction* SetActorPosition(char objName[], double xPosition, double yPosition, World *GameWorld);

SceneAction* MoveActor(char objName[], double xMovement, double yMovement, short repeatTimes, World *GameWorld);

SceneAction* MoveActorX(char objName[], double xMovement, short repeatTimes, World *GameWorld);

SceneAction* MoveActorY(char objName[], double yMovement, short repeatTimes, World *GameWorld);

SceneAction* SetActorDirection(char objName[], double rotation, World *GameWorld);

SceneAction* RotateActor(char objName[], double rotation, short repeatTimes, World *GameWorld);

SceneAction* HideActor(char objName[], World *GameWorld);

SceneAction* ShowActor(char objName[], World *GameWorld);

SceneAction* CreateActor(char objName[], ObjectType actorID, int xPos, int yPos, World *GameWorld);

SceneAction* SceneAction_PlaySound(char soundName[], char folderName[], ChannelName soundChannel, float volume, World *GameWorld);

SceneAction* SceneAction_SetCameraPosition(float xPos, float yPos, World *GameWorld);

SceneAction* SceneAction_MoveCamera(float xVel, float yVel, int repeatTimes, World *GameWorld);

SceneAction* SceneAction_MoveCameraSmooth(float xPos, float yPos, float coefficient, World *GameWorld);

SceneAction* SceneAction_SetZoom(float zoomX, float zoomY, World *GameWorld);

SceneAction* SceneAction_ChangeZoom(float zoomX, float zoomY, int repeatTimes, World *GameWorld);


SceneAction* createSceneAction(SceneActionID newActionID, World *GameWorld);

SceneAction* deleteSceneAction(SceneAction *deleteAction, World *GameWorld);

int deleteAllSceneActions(World *GameWorld);