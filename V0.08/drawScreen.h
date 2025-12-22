#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "LemonMain.h"
#include "spriteLoader.h"
#endif



// Draws objects from gameWorld to screen array
int drawObjects(Camera inputCamera, World *GameWorld, RenderFrame ScreenData);


int drawHitboxes(Camera inputCamera, World *GameWorld, RenderFrame ScreenData);


int renderHitbox(Camera inputCamera, World *gameWorld, PhysicsRect *inputBox, SDL_Renderer *Screen);


int renderObjectSprite(Camera inputCamera, DisplayData inputData, PhysicsRect inputBox, SDL_Renderer *Screen);


int countObjectRenders(Object *inputObject, int result, World *GameWorld);



// Draws and positions player relative to camera position in gameWorld
int drawPlayerHitboxes(Camera inputCamera, World *gameWorld, SDL_Renderer *Screen);


int WorldCameraControl(World *GameWorld, Camera *inputCamera);


int restrictCameraToBounds(Camera *inputCamera);


int renderBackGroundSprite(Camera inputCamera, BackgroundData WorldBackground, RenderFrame ScreenData);


// Debug text functions
int AddDebugText(char inputPhrase[], DebugTextFormatting format, int x, int y, int TicksToDelete);


int DisplaySoundChannelDebugInfo(ChannelName channel);


int DisplayDebugInfo(World *GameWorld, DebugTextMode debugTextMode);


int DisplayObjectDebugInfo(Object *input, int objectNumber, World *GameWorld);


int RemoveDebugText(int index);


int RemoveAllDebugTexts();


int RenderDebugText(Camera inputCamera, RenderFrame ScreenData);