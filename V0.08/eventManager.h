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

int setCameraZoom(float newZoomX, float newZoomY, Camera *inputCamera, RenderFrame *ScreenData);

int setScreenAndRendererSize(int newWidth, int newHeight, RenderFrame *ScreenData);

int setScreenSize(int newWidth, int newHeight, RenderFrame *ScreenData);

int setScreenSizeScale(int newWidth, int newHeight, RenderFrame *ScreenData);

int enableFullscreen(RenderFrame *ScreenData);

int disableFullscreen(RenderFrame *ScreenData);

int validateScreenDimensions(RenderFrame *ScreenData);

int validateZoom(Camera *inputCamera, RenderFrame *ScreenData);


int PauseGame(World *GameWorld);


int ResumeGame(World *GameWorld);


int InitialiseLevelFlag(Object *inputObject, ObjectController *ObjectList);


int UpdateFlagObject(Object* inputObject, PlayerData *Player, World *GameWorld);
