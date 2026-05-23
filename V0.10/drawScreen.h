// Draws objects from gameWorld to screen 
int drawObjects(Camera inputCamera, World *GameWorld, SDL_Renderer *Screen);


void drawHitboxes(Camera inputCamera, World *GameWorld, SDL_Renderer *Screen);


int renderHitbox(Camera inputCamera, PhysicsBox *inputBox, SDL_Renderer *Screen);

void drawPlayerHitboxes(Camera inputCamera, World *gameWorld, SDL_Renderer *Screen);


int renderObject(Camera inputCam, Object *inputObject, SDL_Renderer *Screen);

int renderTiledSprite(SDL_Renderer *Screen, Camera inputCamera, DisplayData inputData, SDL_FRect *renderBox, TileMap *map);



int CameraControl(World *GameWorld, Camera *inputCamera);


int restrictCameraToBounds(Camera *inputCamera);


int renderBackGroundSprite(Camera inputCamera, BackgroundData *WorldBackground, SDL_Renderer *Screen);


int getTileAtPosition(float x, float y, TilePlane *input);


// Debug text functions
void DisplayDebugInfo(Camera renderCamera, World *GameWorld, SDL_Renderer *Screen);


int DisplayObjectDebugInfo(Object *input, int objectNumber, bool goToMouse, Camera renderCamera);


Text* addDebugText(const char inputPhrase[], float x, float y, int wrapwidth, DebugTextFormatting format);

Text* addDebugTextWithName(const char textPhrase[], const char name[], float xPos, float yPos, int wrapWidth, DebugTextFormatting format);

Text* getDebugTextWithName(const char name[]);

int RemoveDebugTextWithName(const char name[]);


void renderTexts(Camera renderCamera, World *GameWorld, SDL_Renderer *Screen);

void RenderTextList(TextList *list, Camera inputCamera, SDL_Renderer *Screen);
