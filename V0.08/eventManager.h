#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "spriteLoader.h"
#include "LemonMain.h"
#include "levelLoader.h"
#include "UIObjects.h"
#include "cutsceneManager.h"
#include "drawScreen.h"
#endif


int StartGame(World *GameWorld);


int HandleGameWorldEvents(World *GameWorld, RenderFrame *ScreenData);

int ExecuteGameEvent(GameEvent *inputEvent, World *GameWorld, RenderFrame *ScreenData);

int deleteAllGameEvents(World *GameWorld);

int clearGameEvents(World *GameWorld);

GameEvent* addNewGameEvent(World *GameWorld);

int switchLevel(int level, World *GameWorld);

int streamPartition(int sceneID, World *GameWorld);

int changeScreenSizeScaled(int newWidth, int newHeight, World *GameWorld);

int changeScreenSize(int newWidth, int newHeight, World *GameWorld);

int setScreenAndRendererSize(int newWidth, int newHeight, World *GameWorld);

int enableFullscreen(World *GameWorld);

int enableFullscreenScaled(World *GameWorld);

int disableFullscreen(World *GameWorld);

int toggleFullscreen(World *GameWorld);

int setCameraZoom(float zoomX, float zoomY, World *GameWorld);

int changeCameraZoom(float zoomX, float zoomY, World *GameWorld);

int applyCameraZoom(float newZoomX, float newZoomY, Camera *inputCamera, RenderFrame *ScreenData);

int applyScreenAndRendererSize(int newWidth, int newHeight, RenderFrame *ScreenData);

int applyScreenSize(int newWidth, int newHeight, RenderFrame *ScreenData);

int applyScreenSizeScale(int newWidth, int newHeight, RenderFrame *ScreenData);

int applyEnableFullscreen(RenderFrame *ScreenData);

int applyDisableFullscreen(RenderFrame *ScreenData);

int validateScreenDimensions(RenderFrame *ScreenData);

int validateZoom(Camera *inputCamera, RenderFrame *ScreenData);


int PauseGame(World *GameWorld);

int ResumeGame(World *GameWorld);


int InitialiseLevelFlag(Object *inputObject, ObjectController *ObjectList);

bool detectPlayer(Object* inputObject, PlayerData *Player);

bool detectCamera(Object* inputObject, Camera inputCamera);



int UpdateFlagObject(Object* inputObject, PlayerData *Player, World *GameWorld);
