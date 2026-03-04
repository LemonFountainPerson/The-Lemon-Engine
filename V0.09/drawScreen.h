#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "LemonMain.h"
#include "spriteLoader.h"
#include "levelLoader.h"
#endif



// Draws objects from gameWorld to screen array
int drawObjects(Camera inputCamera, ObjectController *ObjectList, RenderFrame ScreenData);


int drawHitboxes(Camera inputCamera, ObjectController *ObjectList, SDL_Renderer *Screen);


int renderHitbox(Camera inputCamera, PhysicsBox *inputBox, SDL_Renderer *Screen);


int renderObject(int drawLayer, Camera inputCam, Object *inputObject, SDL_Renderer *Screen, ObjectController *ObjectList);

int renderObjectSprite(Camera inputCamera, Object *input, SDL_Renderer *Screen, ObjectController *ObjectList);

int renderTiledSprite(SDL_Renderer *Screen, Camera inputCamera, DisplayData inputData, SDL_FRect *renderBox, TileMap *map);




// Draws and positions player relative to camera position in gameWorld
int drawPlayerHitboxes(Camera inputCamera, World *gameWorld, SDL_Renderer *Screen);


int WorldCameraControl(World *GameWorld, Camera *inputCamera);


int restrictCameraToBounds(Camera *inputCamera);


int renderBackGroundSprite(Camera inputCamera, BackgroundData *WorldBackground, RenderFrame ScreenData);


int getTileAtPosition(float x, float y, TilePlane *input);


// Debug text functions
int AddDebugText(char inputPhrase[], DebugTextFormatting format, float x, float y, int TicksToDelete);


int DisplaySoundChannelDebugInfo(ChannelName channel);


int DisplayDebugInfo(World *GameWorld, Camera inputCamera, RenderFrame ScreenData);


int DisplayObjectDebugInfo(Object *input, int objectNumber, bool goToMouse, Camera renderCamera);


int RemoveDebugText(int index);


int RemoveAllDebugTexts();


int RenderDebugText(Camera inputCamera, RenderFrame ScreenData);