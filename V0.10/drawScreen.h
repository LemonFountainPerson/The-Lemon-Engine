#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "LemonMain.h"
#include "spriteLoader.h"
#include "levelLoader.h"
#endif



// Draws objects from gameWorld to screen 
int drawObjects(Camera inputCamera, World *GameWorld, SDL_Renderer *Screen);


void drawHitboxes(Camera inputCamera, World *GameWorld, SDL_Renderer *Screen);


int renderHitbox(Camera inputCamera, PhysicsBox *inputBox, SDL_Renderer *Screen);


int renderObject(Camera inputCam, Object *inputObject, SDL_Renderer *Screen);

int renderTiledSprite(SDL_Renderer *Screen, Camera inputCamera, DisplayData inputData, SDL_FRect *renderBox, TileMap *map);




// Draws and positions player relative to camera position in gameWorld
int drawPlayerHitboxes(Camera inputCamera, World *gameWorld, SDL_Renderer *Screen);


int CameraControl(World *GameWorld, Camera *inputCamera);


int restrictCameraToBounds(Camera *inputCamera);


int renderBackGroundSprite(Camera inputCamera, BackgroundData *WorldBackground, SDL_Renderer *Screen);


int getTileAtPosition(float x, float y, TilePlane *input);


// Debug text functions
void DisplayDebugInfo(Camera renderCamera, World *GameWorld, SDL_Renderer *Screen);


int DisplayObjectDebugInfo(Object *input, int objectNumber, bool goToMouse, Camera renderCamera);


int AddDebugText(const char inputPhrase[], float x, float y, int wrapwidth, DebugTextFormatting format);


void renderTexts(Camera renderCamera, World *GameWorld, SDL_Renderer *Screen);

void RenderTextList(TextList *list, Camera inputCamera);


float getCursorPos(void);