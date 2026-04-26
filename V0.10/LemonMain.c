#include "LemonEngine.h"


RenderFrame ScreenData = {0};

float deltaTime = 0.0;


int keyboard[INPUT_COUNT] = {0};

MouseData MouseInput = {0};

GamePadData GamePadInput = {0};


EngineConfig EngineSettings = { 
	.MaxGameEvents = MAX_QUEUED_GAME_EVENTS,
	.MaxObjects = MAX_OBJECTS };

RenderConfig RenderSettings = {0};

TextConfig TextSettings = {0};

DebugConfig DebugSettings = {0};

GameFlag GameFlags[GAME_FLAG_COUNT] = {0};


Uint64 TickNum = 0;




// Lemon Engine main
int RunLemonEngine(void)
{
	if (StartUpLemonEngine() == LEMON_ERROR)
	{
		return LEMON_ERROR;
	}


	// Game initialisation
	World GameWorld = {0};
    if (initialiseWorld(&GameWorld) != LEMON_SUCCESS)
    {
    	return LEMON_ERROR;
    }

	StartGame(&GameWorld);


	// The loop code is run directly here without MainLoop() for performance when executing as a standalone program
	Uint64 gameTick = 0;
	Uint64 renderRefresh = 0;
	Uint64 delta = 0;
    Uint64 lastFrameTime = SDL_GetTicksNS();
    Uint64 currentFrameTime = lastFrameTime;
	
    while(GameWorld.GameState != CLOSE_GAME)
    {
    	// Window messages
	    getExternalInput(&GameWorld, ScreenData.Renderer);

		// Timing
		currentFrameTime = SDL_GetTicksNS();
	    delta = (currentFrameTime - lastFrameTime);
	   	lastFrameTime = currentFrameTime;
	    deltaTime = (float)delta/(float)NS_PER_SECOND;
	    
	    gameTick += delta;
	    renderRefresh += delta;
	    ScreenData.FrameTimer += delta;


	    // World updates
	    GameFrame(&GameWorld);

	    while (gameTick >= EngineSettings.TickDelta)
	    {
	    	gameTick -= EngineSettings.TickDelta;
			GameTick(&GameWorld);
	    }

	    // Render screen
	    if (renderRefresh >= RenderSettings.RenderDelta)
	    {
			renderRefresh = renderRefresh % RenderSettings.RenderDelta;
		   	Render(&GameWorld, &ScreenData);
		}


		// Process sound
		IterateAudio(GameWorld.MainCamera);
    }

	CloseGame(&GameWorld, &ScreenData);

	putConsoleString("\nLemon Engine closed successfully!");

	return LEMON_SUCCESS;
}

int StartUpLemonEngine(void)
{
	putConsoleString("\nStarting up...\n");

	// SDL initialisation
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))// | SDL_INIT_GAMEPAD))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to initialise SDL! \nEnsure SDL3.dll is in directory with executable.", NULL);
		return LEMON_ERROR;
	}

	if (!TTF_Init())
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to initialise TTF! \nEnsure SDL3_ttf.dll is in directory with executable.", NULL);
		return LEMON_ERROR;
	}

	if (initialiseScreen(&ScreenData, H_RESOLUTION, V_RESOLUTION, false) != LEMON_SUCCESS)
	{
		return LEMON_ERROR;
	}

	if (initialiseAudio() != LEMON_SUCCESS)
	{
		return LEMON_ERROR;
	}

    // initialise text data to ensure pointers are null
    initialiseFontList(&TextSettings.FontList);
    initialiseTextList(&TextSettings.DebugTexts);
    initialiseTextList(&TextSettings.TextList);
    createConsoleCommands(DebugSettings.commands);

    SetEngineSettingsToDefault();
	SetRenderSettingsToDefault();
	SetTextSettingsToDefault();
	SetDebugSettingsToDefault();

	GamePadInput.gamepad = NULL;
    ClearInput();

	srand(RANDOM_SEED);

	putConsoleString("Engine initialised!\n");


	return LEMON_SUCCESS;
}


int initialiseWorld(World *GameWorld)
{
	// Game world creation
	memset(GameWorld, 0, sizeof(World));
	ResetCamera(&GameWorld->MainCamera);
    initialiseCameraViews(GameWorld->views);

	GameWorld->GameState = EMPTY_GAME;
	GameWorld->TextQueue = NULL;
	GameWorld->CurrentCutscene = NO_CUTSCENE;
	GameWorld->SceneActionQueue = NULL;

	// Check for resource data access
	if (CheckResourceData() == MISSING_DATA)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing Data", 
			"Missing Lemon resource data! \nPlease check that the LemonData folder is available and in the same directory as the executable.", ScreenData.Window);
		return MISSING_DATA;
	}

	
	// Object controller creation
	GameWorld->ObjectList = createObjectController();

	if (GameWorld->ObjectList == NULL)
	{
		return LEMON_ERROR;
	}

	putConsoleString("Object Controller initialised!\n");


	// Load backgrounds
	initialiseBackGround(&GameWorld->WorldBackground);

	InitialisePlayerData(&GameWorld->Player);
	putConsoleString("Initialised Player!\n");

	SetGravity(GameWorld, 1.0, 180.0);

	GameWorld->PhysicsType = PLATFORMER;

	putConsoleString("World Initialised!\n");

	return LEMON_SUCCESS;
}


int CloseGame(World *GameWorld, RenderFrame *ScreenData)
{
	// Clear game data and cleanup
	destroyWorld(GameWorld);

	cleanUpAudioData();

	cleanUpSDLRenderer(ScreenData);

    SDL_Quit();
    TTF_Quit();


	return LEMON_SUCCESS;
}


// Updates 60 times per second (can be modified)
int GameTick(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	// Input acknowledgement is delayed until the next frame by doing this so that gameFrame can use this frame's input correctly
	keyboard[ACKNOWLEDGE_INPUT] = 1;

	MasterControls(GameWorld, ScreenData.Window);

	if (DebugSettings.PauseEngine == ENGINE_PAUSED)
	{
		return ACTION_DISABLED;
	}

	TickNum++;

	CameraControl(GameWorld, &GameWorld->MainCamera);

	UpdateCutscene(GameWorld);

	updateObjects(GameWorld);

	updateTextBoxes(GameWorld);

	return LEMON_SUCCESS;
}


// Updates every frame
int GameFrame(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	HandleGameEvents(GameWorld, &ScreenData);

	if (DebugSettings.PauseEngine == ENGINE_PAUSED)
	{
		return ACTION_DISABLED;
	}

	updateObjectsFrame(GameWorld);

	updateObjectDisplays(GameWorld);		// for allowing animations to happen between ticks; not being restricted to multiples/factors of gameTick amount


	return LEMON_SUCCESS;
}


int RenderEngine(Camera renderCamera, World *GameWorld, SDL_Renderer *Screen)
{
	if (GameWorld == NULL || Screen == NULL)
	{
		return MISSING_DATA;
	}

	if (fabs(renderCamera.zoomX - 1.0) > 0.001 || fabs(renderCamera.zoomY - 1.0) > 0.001)
	{
		renderCamera.zoomedWidth = renderCamera.width / renderCamera.zoomX;
		renderCamera.zoomedHeight = renderCamera.height / renderCamera.zoomY;
	}
	else
	{
		renderCamera.zoomedWidth = renderCamera.width;
		renderCamera.zoomedHeight = renderCamera.height;
	}

	SDL_SetRenderScale(Screen, renderCamera.zoomX, renderCamera.zoomY);
	SDL_SetRenderLogicalPresentation(Screen, renderCamera.width, renderCamera.height, SDL_LOGICAL_PRESENTATION_STRETCH);

	renderBackGroundSprite(renderCamera, &GameWorld->WorldBackground, Screen);

	drawObjects(renderCamera, GameWorld, Screen);

	if (RenderSettings.drawHitboxes == 1)
	{
		drawHitboxes(renderCamera, GameWorld, Screen);
	}

	return LEMON_SUCCESS;
}


int Render(World *GameWorld, RenderFrame *ScreenData)
{
	if (GameWorld == NULL || ScreenData == NULL || ScreenData->Renderer == NULL)
	{
		return MISSING_DATA;
	}

	// main camera
	SDL_SetRenderDrawColor(ScreenData->Renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(ScreenData->Renderer);

	RenderEngine(GameWorld->MainCamera, GameWorld, ScreenData->Renderer);

    FPSCounter();

	renderTexts(GameWorld->MainCamera, GameWorld, ScreenData->Renderer);
	
	SDL_RenderPresent(ScreenData->Renderer);

	
	return LEMON_SUCCESS;
}


void initialiseCameraViews(CameraView list[VIEW_COUNT])
{
	for (int i = 0; i < VIEW_COUNT; i++)
	{
		ResetCamera(&list[i].cam);
		list[i].viewXPos = 0.0;
		list[i].viewYPos = 0.0;
		list[i].viewWidth = 100.0;
		list[i].viewHeight = 100.0;
		list[i].direction = DEFAULT_DIRECTION;
		list[i].layer = MIDDLEGROUND;
		list[i].useMainCam = false;
		list[i].attachedObj = NULL;
		list[i].target = NULL;

		list[i].ticksUntilRefresh = 1;
		list[i].nextRender = 0;

		list[i].active = false;
	}
}

CameraView* addCameraViewToList(float camX, float camY, int camWidth, int camHeight, float viewPosX, float viewPosY, float width, float height, Layer drawLayer, bool useMain, CameraView list[VIEW_COUNT])
{
	for (int i = 0; i < VIEW_COUNT; i++)
	{	
		if (!list[i].active)
		{
			list[i].active = true;

			ResetCamera(&list[i].cam);
			list[i].cam.CameraX = camX;
			list[i].cam.CameraY = camY;
			list[i].cam.width = camWidth;
			list[i].cam.height = camHeight;
			list[i].cam.zoomedWidth = camWidth;
			list[i].cam.zoomedHeight = camHeight;
			
			list[i].viewXPos = viewPosX - (width / 2.0);
			list[i].viewYPos = -viewPosY - (height / 2.0);
			list[i].viewWidth = width;
			list[i].viewHeight = height;

			list[i].layer = drawLayer;
			list[i].useMainCam = useMain;

			list[i].nextRender = TickNumber();
			list[i].ticksUntilRefresh = EngineSettings.GameTicksPerSecond / DEFAULT_VIEW_REFRESH_RATE;

			if (list[i].target != NULL)
			{
				SDL_DestroyTexture(list[i].target);
				list[i].target = NULL;
			}

			return &list[i];
		}
	}

	return NULL;
}

CameraView* addCameraView(float camX, float camY, int camWidth, int camHeight, float viewX, float viewY, float viewWidth, float viewHeight, Layer drawLayer, World *GameWorld)
{
	return addCameraViewToList(camX, camY, camWidth, camHeight, viewX, viewY, viewWidth, viewHeight, drawLayer, false, GameWorld->views); 
}

CameraView* addMainCameraView(float viewX, float viewY, float width, float height, Layer drawLayer, World *GameWorld)
{
	return addCameraViewToList(0.0, 0.0, 1, 1, viewX, viewY, width, height, drawLayer, true, GameWorld->views);
}

void attachCameraViewToObject(CameraView *input, Object *attach)
{
	if (input == NULL || attach == NULL)
	{
		return;
	}

	input->attachedObj = attach;

	attach->ObjectDisplay->RenderModeOverride = DO_NOT_RENDER;

	return;
}

CameraView* getCameraView(World *GameWorld, int id)
{
	if (id < 0 || id >= VIEW_COUNT || GameWorld == NULL)
	{
		return NULL;
	}

	if (GameWorld->views[id].active)
	{
		return &GameWorld->views[id];
	}
	else
	{
		return NULL;
	}
}

void printCameraViewInfo(CameraView list[VIEW_COUNT])
{
	for (int i = 0; i < VIEW_COUNT; i++)
	{
		if (list[i].active)
		{
			if (list[i].attachedObj != NULL)
			{
				putConsoleString("%d: (Active) (Attached object ID: %d) CamX: %.2f CamY: %.2f  Ticks until render: %lld", 
					i, list[i].attachedObj->index, list[i].cam.CameraX, list[i].cam.CameraY, list[i].cam.zoomedWidth, list[i].ticksUntilRefresh);
			}
			else
			{
				putConsoleString("%d: (Active) (No attached object) CamX: %.2f CamY: %.2f  Ticks until render: %lld", 
					i, list[i].cam.CameraX, list[i].cam.CameraY, list[i].ticksUntilRefresh);
			}
		}
	}
}

void removeCameraView(CameraView *input)
{
	if (input == NULL || input->active == false)
	{
		return;
	}

	input->active = false;

	if (input->attachedObj != NULL)
	{
		input->attachedObj->ObjectDisplay->RenderModeOverride = DEFAULT_TO_SPRITE;
	}

	input->attachedObj = NULL;

	if (input->target != NULL)
	{
		SDL_DestroyTexture(input->target);
		input->target = NULL;
	}

	return;
}

void removeAllCameraViewsFromList(CameraView list[VIEW_COUNT])
{
	for (int i = 0; i < VIEW_COUNT; i++)
	{	
		removeCameraView(&list[i]);
	}

	return;
}

void removeAllCameraViews(World *GameWorld)
{
	removeAllCameraViewsFromList(GameWorld->views);

	return;
}

void renderCameraViews(CameraView list[VIEW_COUNT], World *GameWorld, SDL_Renderer *Screen, Layer drawLayer)
{
	if (RenderSettings.drawCamViews == false)
	{
		return;
	}

	bool prevHUD = RenderSettings.drawHUD;
	RenderSettings.drawHUD = false;
	RenderSettings.drawCamViews = false;

	SDL_SetRenderDrawColor(Screen, 0x00, 0x00, 0x00, 0xFF);

	Camera mainCam = GameWorld->MainCamera;
	float halfW = (float)(mainCam.width >> 1);
	float halfH = (float)(mainCam.height >> 1);
	SDL_FRect box = {0};

	Object *attached = NULL;

	for (int i = 0; i < VIEW_COUNT; i++)
	{
		if (!list[i].active)
		{
			continue;
		}

		if (list[i].attachedObj != NULL)
		{
			attached = list[i].attachedObj;

			if (attached->State < DEFAULT_STATE)
			{
				list[i].attachedObj = NULL;
				removeCameraView(&list[i]);
				continue;
			}

			PhysicsBox *objBox = attached->ObjectBox;
			if (getRenderMode(attached) == SINGLE)
			{
				box.x = (objBox->xSize >> 1) + objBox->xPos - (box.w / 2.0) + halfW;
				box.y = (objBox->ySize >> 1) - objBox->yPos + (box.h / 2.0) + halfH;
				box.w = list[i].viewWidth;
				box.h = list[i].viewHeight;
			}
			else
			{
				box.w = objBox->xSize;
				box.h = objBox->ySize;
				box.x = halfW + objBox->xPos;
				box.y = halfH - objBox->yPos - (box.h);
			}	

			list[i].direction = getDisplayDirection(attached);
			list[i].layer = getDisplayLayer(list[i].attachedObj);
		}
		else
		{
			box.x = list[i].viewXPos + halfW;
			box.y = list[i].viewYPos + halfH;
			box.w = list[i].viewWidth;
			box.h = list[i].viewHeight;
		}

		if (list[i].layer != drawLayer)
		{
			continue;
		}

		if (list[i].layer != HUD)
		{
			box.x -= mainCam.CameraX;
			box.y += mainCam.CameraY;
		}

		if (box.x > GameWorld->MainCamera.zoomedWidth || box.y > GameWorld->MainCamera.zoomedHeight || box.x + box.w < 0 || box.y + box.h < 0)
		{
			continue;
		}

		if (list[i].nextRender > TickNumber())
		{
			goto Skip_CamView_Rerender;
		}

		if (list[i].target == NULL)
		{
			int width, height;
			if (list[i].useMainCam)
			{
				width = mainCam.zoomedWidth;
				height = mainCam.zoomedHeight;
			}
			else
			{
				width = list[i].cam.zoomedWidth;
				height = list[i].cam.zoomedHeight;
			}

			list[i].target = SDL_CreateTexture(Screen, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

			if (list[i].target == NULL)
			{
				removeCameraView(&list[i]);
				continue;
			}
		}

		SDL_Texture *previousTarget = SDL_GetRenderTarget(Screen);
		SDL_SetRenderTarget(Screen, list[i].target);
		SDL_RenderClear(Screen);

		if (list[i].useMainCam)
		{
			RenderEngine(mainCam, GameWorld, Screen);
		}
		else
		{
			RenderEngine(list[i].cam, GameWorld, Screen);
		}

		SDL_SetRenderTarget(Screen, previousTarget);

		list[i].nextRender = TickNumber() + list[i].ticksUntilRefresh;

		Skip_CamView_Rerender:

		if (list[i].direction != DEFAULT_DIRECTION)
		{
			SDL_RenderTextureRotated(Screen, list[i].target, NULL, &box, list[i].direction - DEFAULT_DIRECTION, NULL, SDL_FLIP_NONE);
		}
		else
		{
			SDL_RenderTexture(Screen, list[i].target, NULL, &box);
		}
	}

	RenderSettings.drawHUD = prevHUD;
	RenderSettings.drawCamViews = true;

	return;
}


int hashGameFlag(const char name[])
{
	unsigned int hash = 5381;

	int i = 0;

	while (name[i] != '\0' && i < MAX_LEN)
	{
		hash = (hash * 33) + name[i];
		i++;
	}

	return (int)(hash % GAME_FLAG_COUNT);
}

void addGameFlag(const char name[], int startValue)
{
	int length = strlen(name);
	if (length >= MAX_LEN || name[0] == '\0')
	{
		return;
	}

	int index = hashGameFlag(name);
	int count = 0;

	while (GameFlags[index].name[0] != '\0' && count < GAME_FLAG_COUNT)
	{
		if (strcmp(GameFlags[index].name, name) == 0)
		{
			// If there is a game flag with a matching name already, do not add
			return;
		}

		index = (index + 1) % GAME_FLAG_COUNT;
		count++;
	}

	if (GameFlags[index].name[0] == '\0')
	{
		strcpy(GameFlags[index].name, name);
		GameFlags[index].value = startValue;
		GameFlags[index].nameLength = length;
	}

	return;
}

// returns value
int checkGameFlag(const char name[])
{
	if (strlen(name) >= MAX_LEN || name[0] == '\0')
	{
		return -1;
	}

	int index = hashGameFlag(name);
	int count = 0;

	while (GameFlags[index].name[0] != '\0' && count < GAME_FLAG_COUNT)
	{
		if (strcmp(GameFlags[index].name, name) == 0)
		{
			return GameFlags[index].value;
		}

		count++;
		index = (index + 1) % GAME_FLAG_COUNT;
	}

	return -1;
}

// returns index
int getGameFlag(const char name[])
{
	if (strlen(name) >= MAX_LEN || name[0] == '\0')
	{
		return -1;
	}

	int index = hashGameFlag(name);
	int count = 0;

	while (GameFlags[index].name[0] != '\0' && count < GAME_FLAG_COUNT)
	{
		if (strcmp(GameFlags[index].name, name) == 0)
		{
			return index;
		}
		
		count++;
		index = (index + 1) % GAME_FLAG_COUNT;
	}

	return -1;
}

void setGameFlag(const char name[], int newValue)
{
	if (strlen(name) >= MAX_LEN || name[0] == '\0')
	{
		return;
	}

	int index = hashGameFlag(name);
	int count = 0;

	while (GameFlags[index].name[0] != '\0' && count < GAME_FLAG_COUNT)
	{
		if (strcmp(GameFlags[index].name, name) == 0)
		{
			GameFlags[index].value = newValue;
			return;
		}
		
		count++;
		index = (index + 1) % GAME_FLAG_COUNT;
	}

	return;
}


Uint64 TickNumber(void)
{
	return TickNum;
}

void setTickNumber(Uint64 input)
{
	TickNum = input;

	return;
}

int FPSCounter(void)
{
	static int TextIndex = -1;

    ScreenData.FramesElapsed++;

	if (ScreenData.FrameTimer > FRAMERATE_UPDATE_TICK)
    {
    	if (DebugSettings.FPSCounter)
		{
			char buffer[40] = {0};
	        snprintf(buffer, 40, "%d FPS", INTERP_FRAMERATE(ScreenData.FramesElapsed));

	        if (TextIndex < 0)
	        {
	        	TextIndex = addText(buffer, 20 - (ScreenData.screenWidth >> 1), (ScreenData.screenHeight >> 1) - 40);
	        	setFontSize("DefaultFont", 20.0);
	        }
	        else
	        {
	        	updateText(TextIndex, buffer);
	        }
		} 
       
	    ScreenData.FramesElapsed = 0;
	    ScreenData.FrameTimer = ScreenData.FramesElapsed % FRAMERATE_UPDATE_TICK;
    }

    if (DebugSettings.FPSCounter == 0 && TextIndex > -1)
    {
 		RemoveText(&TextSettings.TextList, TextIndex);
 		TextIndex = -1;
    }

	return LEMON_SUCCESS;
}


int initialiseBackGround(BackgroundData *input)
{
	if (input == NULL)
	{
		return MISSING_DATA;
	}

	input->bgParallax = 0.1;
	input->BackgroundSpriteBuffer = NULL;
	input->BackgroundRenderMode = DEFAULT_TO_SPRITE;
	initialiseSpriteSetList(&input->bgSpriteSets);

	input->tileBG.tiles = NULL;
	input->tileBG.GridWidth = 0;
	input->tileBG.GridHeight = 0;
	input->tileBG.tileWidth = X_TILESCALE;
	input->tileBG.tileHeight = Y_TILESCALE;
	input->tileBG.xPos = 0.0;
	input->tileBG.yPos = 0.0;

	loadSpriteSetFromFile("BackGrounds", &input->bgSpriteSets, BACKGROUND_SETID);
	
	
	return LEMON_SUCCESS;
}


ObjectController* createObjectController(void)
{
	ObjectController *newController = calloc(1, sizeof(ObjectController));

	if (newController == NULL)
	{
		putConsoleError("Error: Could not allocate space for Object Controller.\n\n");
		return NULL;
	}

	newController->lastObject = NULL;
	newController->firstObject = NULL;
	initialiseSpriteSetList(&newController->spriteSets);
	newController->cachedFirstObject = NULL;
	newController->cachedLastObject = NULL;
	newController->availableSlots = NULL;
	newController->FrameUpdates = NULL;

	initialiseComponents(newController);

	putConsoleString("Initialising object slots...");
	
	ComponentData *newArena = &newController->objectComponents;

	int i = EngineSettings.MaxObjects - 1;
	Object *newObject = NULL;

	while (i >= 0)
	{
		newObject = &newArena->Objects[i];
		clearObjectData(newObject);

		// This pointer nonsense is used to circumvent the const modifier; this should not be used elsewhere as these values should not change
		// pointers to the objectbox, objectdisplay and index values of the object are cast to regular values without const, before being dereferenced to be assigned with new values
		(*(PhysicsBox * *)&newObject->ObjectBox) = &newArena->PhysicsBoxes[i];
		(*(DisplayData * *)&newObject->ObjectDisplay) = &newArena->Displays[i];
		*((int *)&newObject->index) = i;

		if (newController->availableSlots != NULL)
		{
			newController->availableSlots->prevObject = newObject;
		}

		newObject->nextObject = newController->availableSlots;
		newController->availableSlots = newObject;
		i--;
	}


	return newController;
}


void deleteObjectController(ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return;
	}

	deleteAllObjects(ObjectList);
	deleteAllCachedObjects(ObjectList);	

	deleteAllSpriteSets(&ObjectList->spriteSets);

	free(ObjectList);

	return;
}


void destroyWorld(World *GameWorld)	// honestly picked this name because its funny
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return;
	}
	
	clearTextQueue(GameWorld);
	deleteAllSceneActions(GameWorld);

	deleteAllGameEvents(GameWorld);

	deleteObjectController(GameWorld->ObjectList);

	deleteAllSpriteSets(&GameWorld->WorldBackground.bgSpriteSets);

	removeAllCameraViews(GameWorld);

	memset(GameWorld, 0, sizeof(World));

	GameWorld->GameState = EMPTY_GAME;

	return;
}


int cleanUpSDLRenderer(RenderFrame *ScreenData)
{
	cleanUpTextData(ScreenData);

	SDL_DestroyRenderer(ScreenData->Renderer);
    SDL_DestroyWindow(ScreenData->Window);
    ScreenData->Renderer = NULL;
    ScreenData->Window = NULL;

	return LEMON_SUCCESS;
}


FuncResult CheckResourceData(void)
{
	// check textures
	char path[strlen(SPRITE_ROOT) + strlen(DEFAULT_TEXTURE) + 2];
	strcpy(path, SPRITE_ROOT);
	strcat(path, DEFAULT_TEXTURE);

	if (access(path, F_OK | R_OK) == -1)
	{
		return MISSING_DATA;
	}

	// check level data
	char newPath[strlen(LEVELDATA_ROOT) + strlen("Level0.txt") + 2];
	strcpy(newPath, LEVELDATA_ROOT);
	strcat(newPath, "Level0.txt");

	if (access(newPath, F_OK | R_OK) == -1)
	{
		return MISSING_DATA;
	}

	// check save data

	return LEMON_SUCCESS;
}

int getExternalInput(World *GameWorld, SDL_Renderer *screen)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	if (keyboard[ACKNOWLEDGE_INPUT])
	{
		keyboard[ACKNOWLEDGE_INPUT] = 0;
		AcknowledgeHeldButtons();
	}

	SDL_Event event;

    while (SDL_PollEvent(&event)) 
    {
    	switch (event.type)
    	{
    	case SDL_EVENT_TERMINATING:
    	case SDL_EVENT_QUIT:
    			GameWorld->GameState = CLOSE_GAME;
    		break;

	    case SDL_EVENT_KEY_DOWN:
	    case SDL_EVENT_KEY_UP:
    			getKeyboardInput(&event.key);
    		break;

    	case SDL_EVENT_MOUSE_BUTTON_DOWN:
    	case SDL_EVENT_MOUSE_BUTTON_UP:
    			getMouseInput(&event.button);
    		break;

    	case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    	case SDL_EVENT_GAMEPAD_BUTTON_UP:
    			getGamepadInput(&event.gbutton);
    		break;

    		// case SDL_EVENT_MOUSE_MOTION:
    		// 	SDL_ConvertEventToRenderCoordinates(screen, &event);
    		// 	MouseInput.xPos = event.motion.x;
			// 	MouseInput.yPos = -event.motion.y;
    		// break;

    	case SDL_EVENT_MOUSE_WHEEL:
    		MouseInput.wheelX = event.wheel.x;
    		if (MouseInput.wheelX > 0.00000)
    		{
    			MouseInput.wheelXDir = 1;
    		}
    		else if (MouseInput.wheelX < 0.00000)
    		{
    			MouseInput.wheelXDir = -1;
    		}

    		MouseInput.wheelY = event.wheel.y;
    		if (MouseInput.wheelY > 0.00000)
    		{
    			MouseInput.wheelYDir = 1;
    		}
    		else if (MouseInput.wheelY < 0.00000)
    		{
    			MouseInput.wheelYDir = -1;
    		}
    		break;

    	case SDL_EVENT_GAMEPAD_ADDED:
    		SDL_JoystickID new = event.gdevice.which;

    		SDL_Gamepad *newJoy = SDL_OpenGamepad(new);

    		if (newJoy != NULL)
    		{
    			if (GamePadInput.gamepad != NULL)
	    		{
	    			SDL_CloseGamepad(GamePadInput.gamepad);
	    		}

    			GamePadInput.gamepad = newJoy;
    			GamePadInput.ID = new;
    		}
    		else
    		{
    			putConsoleStringTS("Failed to open gamepad ID: %u", new);
    		}
    		break;

    	case SDL_EVENT_GAMEPAD_REMOVED:
    		SDL_JoystickID which = event.gdevice.which;

    		if (which == GamePadInput.ID && GamePadInput.gamepad != NULL)
    		{
	    		SDL_CloseGamepad(GamePadInput.gamepad);
	    		
    			GamePadInput.gamepad = NULL;
    			GamePadInput.ID = 0;
    		}
    		break;

    	case SDL_EVENT_GAMEPAD_AXIS_MOTION:
    		updateGamepadAxis(&event.gaxis);
    		break;

    	case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
    			ScreenData.Fullscreen = true;
    		break;

    	case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
    			ScreenData.Fullscreen = false;
    		break;

    	case SDL_EVENT_TEXT_INPUT:
    			addTypedCommand(event.text.text);
    		break;

    	default:
    		break;
    	}
	}

	updateCustomKeys();
	updateMousePos(GameWorld->MainCamera);


	return LEMON_SUCCESS;
}

int getKeyboardInput(SDL_KeyboardEvent *key)
{
	if (key->repeat && !DebugSettings.TypingInConsole)
	{
		return EXECUTION_UNNECESSARY;
	}

	int keyCode = 0;

	switch (key->scancode)
	{
		case SDL_SCANCODE_ESCAPE:
			keyCode = LMN_ESCAPE;
			break;

		case SDL_SCANCODE_SPACE:
			keyCode = LMN_SPACE;
			break;

		case SDL_SCANCODE_BACKSPACE:
			keyCode = LMN_BACKSPACE;
			break;

		case SDL_SCANCODE_KP_ENTER:
		case SDL_SCANCODE_RETURN:
			keyCode = LMN_ENTER;
			break;

		case SDL_SCANCODE_TAB:
			keyCode = LMN_TAB;
			break;

		case SDL_SCANCODE_LSHIFT:
			keyCode = LMN_LSHIFT;
			break;

		case SDL_SCANCODE_RSHIFT:
			keyCode = LMN_RSHIFT;
			break;

		case SDL_SCANCODE_GRAVE:
			keyCode = LMN_GRAVE;
			break;

		case SDL_SCANCODE_COMMA:
			keyCode = LMN_COMMA;
			break;

		case SDL_SCANCODE_PERIOD:
			keyCode = LMN_PERIOD;
			break;

		case SDL_SCANCODE_SLASH:
			keyCode = LMN_SLASH;
			break;

		case SDL_SCANCODE_LEFT:
			keyCode = LMN_LEFTARROW;
			break;

		case SDL_SCANCODE_RIGHT:
			keyCode = LMN_RIGHTARROW;
			break;

		case SDL_SCANCODE_UP:
			keyCode = LMN_UPARROW;
			break;

		case SDL_SCANCODE_DOWN:
			keyCode = LMN_DOWNARROW;
			break;

		case SDL_SCANCODE_0:
			keyCode = '0';
			break;

		default:
			if (key->scancode >= SDL_SCANCODE_A && key->scancode <= SDL_SCANCODE_Z)
			{
				keyCode = 'A' + key->scancode - SDL_SCANCODE_A;
			}
			else if (key->scancode >= SDL_SCANCODE_1 && key->scancode <= SDL_SCANCODE_9)
			{
				keyCode = '1' + key->scancode - SDL_SCANCODE_1;
			}
			break;
	}
	
	keyboard[keyCode] = key->down;

	return LEMON_SUCCESS;
}

void keyPressedWhen(int key, bool keyMap)
{
	if (keyboard[key] == 0 || keyMap == 0)
	{
		keyboard[key] = keyMap;
	}

	return;
}

void updateCustomKeys(void)
{
	keyPressedWhen(LMN_LEFT, keyboard['A'] || keyboard[LMN_LEFTARROW] || GamePadInput.dPadLeft || (GamePadInput.leftStickX < -0.9));
	keyPressedWhen(LMN_RIGHT, keyboard['D'] || keyboard[LMN_RIGHTARROW] || GamePadInput.dPadRight || (GamePadInput.leftStickX > 0.9));
	keyPressedWhen(LMN_UP, keyboard['W'] || keyboard[LMN_UPARROW] || GamePadInput.dPadUp || (GamePadInput.leftStickY > 0.9));
	keyPressedWhen(LMN_DOWN, keyboard['S'] || keyboard[LMN_DOWNARROW] || GamePadInput.dPadDown || (GamePadInput.leftStickY < -0.9));

	keyPressedWhen(LMN_JUMP, keyboard[LMN_SPACE] || GamePadInput.southButton);
	keyPressedWhen(LMN_INTERACT, keyboard['E'] || keyboard['Z'] || GamePadInput.westButton);
	keyPressedWhen(LMN_INTERACT2, keyboard['Q'] || keyboard['X'] || GamePadInput.eastButton);
	keyPressedWhen(LMN_INTERACT3, keyboard['R'] || keyboard['C'] || GamePadInput.northButton);

	keyPressedWhen(LMN_TEXT_SKIP, keyboard[LMN_INTERACT2] || MouseInput.RightButton || keyboard[LMN_LSHIFT]);
	keyPressedWhen(LMN_TEXT_CONFIRM, keyboard[LMN_INTERACT] || MouseInput.LeftButton || keyboard[LMN_ENTER]);
	keyPressedWhen(LMN_MENU_CONFIRM, keyboard[LMN_INTERACT] || keyboard[LMN_ENTER]);

	return;
}


void ClearInput(void)
{
	for (int i = ACKNOWLEDGE_INPUT + 1; i < INPUT_COUNT; i++)
	{
		keyboard[i] = 0;	
	}

	memset(&MouseInput, 0, sizeof(MouseData));
	MouseInput.wheelX = 0.0;
	MouseInput.wheelY = 0.0;

	return;
}

void AcknowledgeHeldButtons(void)
{
	for (int i = ACKNOWLEDGE_INPUT + 1; i < INPUT_COUNT; i++)
	{
		if (keyboard[i] == 1)
		{
			keyboard[i] = 2;
		}
	}

	// Mouse buttons will be 1 when they are first clicked, then set to 2 automatically here 
	// (MouseInput.LeftButton != 0 => Mouse held down)  (MouseInput.LeftButton == 1 => Mouse just clicked)
	AcknowledgeMouse();

	return;
}

void AcknowledgeMouse(void)
{
	AcknowledgeButton(MOUSE_LEFT);
	AcknowledgeButton(MOUSE_RIGHT);
	AcknowledgeButton(MOUSE_MIDDLE);
	AcknowledgeButton(MOUSE_SIDE1);
	AcknowledgeButton(MOUSE_SIDE2);
	
	MouseInput.wheelX = 0.0;
	MouseInput.wheelY = 0.0;
	MouseInput.wheelXDir = 0;
	MouseInput.wheelYDir = 0;

	return;
}

void AcknowledgeButton(LemonKeys Key)
{
	if (Key >= INPUT_COUNT || Key < 0)
	{
		switch (Key)
		{
			case MOUSE_LEFT:
				if (MouseInput.LeftButton == 1)
				{
					MouseInput.LeftButton = 2;
				}
				break;

			case MOUSE_RIGHT:
				if (MouseInput.RightButton == 1)
				{
					MouseInput.RightButton = 2;
				}
				break;

			case MOUSE_MIDDLE:
				if (MouseInput.MiddleButton == 1)
				{
					MouseInput.MiddleButton = 2;
				}
				break; 

			case MOUSE_SIDE1:
				if (MouseInput.SideButton1 == 1)
				{
					MouseInput.SideButton1 = 2;
				}
				break;

			case MOUSE_SIDE2:
				if (MouseInput.SideButton2 == 1)
				{
					MouseInput.SideButton2 = 2;
				}
				break;

			default:
				return;
		}

		return;
	}

	if (keyboard[Key] == 1)
	{
		keyboard[Key] = 2;
	}

	return;
}

int updateMousePos(Camera inputCam)
{
	SDL_GetMouseState(&MouseInput.xPos, &MouseInput.yPos);

	MouseInput.xPos -= (ScreenData.screenWidth >> 1);
	MouseInput.xPos *= (float)inputCam.width/(float)ScreenData.screenWidth;

	MouseInput.yPos = ((ScreenData.screenHeight >> 1) - MouseInput.yPos);
	MouseInput.yPos *= (float)inputCam.height/(float)ScreenData.screenHeight;

	return LEMON_SUCCESS;
}

// Get mouse position corrected for camera position and zoom (HUD layer is immune to this already, so this is only for other layers)
float getMouseXCam(Camera inputCamera)
{
	return (MouseInput.xPos / inputCamera.zoomX) + inputCamera.CameraX;
}

float getMouseYCam(Camera inputCamera)
{
	return (MouseInput.yPos / inputCamera.zoomY) + inputCamera.CameraY;
}


int getMouseInput(SDL_MouseButtonEvent *event)
{
	switch(event->button)
	{	
		case SDL_BUTTON_LEFT:
			MouseInput.LeftButton = event->down;
			break;

		case SDL_BUTTON_RIGHT:
			MouseInput.RightButton = event->down;
			break;

		case SDL_BUTTON_MIDDLE:
			MouseInput.MiddleButton = event->down;
			break;

		case SDL_BUTTON_X1:
			MouseInput.SideButton1 = event->down;
			break;

		case SDL_BUTTON_X2:
			MouseInput.SideButton2 = event->down;
			break;

		default:
		break;
	}

	updateCustomKeys();

	return LEMON_SUCCESS;
}

int getGamepadInput(SDL_GamepadButtonEvent *event)
{
	switch(event->button)
	{	
		case SDL_GAMEPAD_BUTTON_SOUTH:
			GamePadInput.southButton = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_NORTH:
			GamePadInput.northButton = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_EAST:
			GamePadInput.eastButton = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_WEST:
			GamePadInput.westButton = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_BACK:
			GamePadInput.back = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_START:
			GamePadInput.start = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_GUIDE:
			GamePadInput.guide = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_DPAD_UP:
			GamePadInput.dPadUp = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
			GamePadInput.dPadDown = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
			GamePadInput.dPadLeft = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
			GamePadInput.dPadRight = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
			GamePadInput.leftShoulder = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
			GamePadInput.rightShoulder = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_LEFT_STICK:
			GamePadInput.leftStick = event->down;
			break;

		case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
			GamePadInput.rightStick = event->down;
			break;

		default:
		break;
	}

	updateCustomKeys();

	return LEMON_SUCCESS;
}

int updateGamepadAxis(SDL_GamepadAxisEvent *event)
{
	float value = fClamp(((float)event->value) / 32767.0, -1.0, 1.0);

	putConsoleStringTS("Axis event with value %d -> %f", event->value, value);

	switch(event->axis)
	{
	case SDL_GAMEPAD_AXIS_LEFTX:
		GamePadInput.leftStickX = value;
		break;

	case SDL_GAMEPAD_AXIS_RIGHTX:
		GamePadInput.rightStickX = value;
		break;

	case SDL_GAMEPAD_AXIS_LEFTY:
		GamePadInput.leftStickY = value;
		break;

	case SDL_GAMEPAD_AXIS_RIGHTY:
		GamePadInput.rightStickY = value;
		break;

	case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
		GamePadInput.leftTrigger = value;
		break;

	case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
		GamePadInput.rightTrigger = value;
		break;

	default:
		break;
	}

	return LEMON_SUCCESS;
}


bool buttonPressed(int key)
{
	if (key < 0)
	{
		return false;
	}

	if (key > INPUT_COUNT)
	{
		switch (key)
		{
			case MOUSE_LEFT:
				return (MouseInput.LeftButton == 1);

			case MOUSE_RIGHT:
				return (MouseInput.RightButton == 1);

			case MOUSE_MIDDLE:
				return (MouseInput.MiddleButton == 1); 

			case MOUSE_SIDE1:
				return (MouseInput.SideButton1 == 1);

			case MOUSE_SIDE2:
				return (MouseInput.SideButton2 == 1);

			default:
				return false;
		}
	}
	else
	{
		return (keyboard[key] == 1);
	}
}

bool keyPressed(int key)
{
	return buttonPressed(key);
}

bool buttonHeld(int key)
{
	if (key < 0)
	{
		return false;
	}


	if (key > INPUT_COUNT)
	{
		switch (key)
		{
			case MOUSE_LEFT:
				return (MouseInput.LeftButton > 0);

			case MOUSE_RIGHT:
				return (MouseInput.RightButton > 0);

			case MOUSE_MIDDLE:
				return (MouseInput.MiddleButton > 0);

			case MOUSE_SIDE1:
				return (MouseInput.SideButton1 > 0);

			case MOUSE_SIDE2:
				return (MouseInput.SideButton2 > 0);  

			default:
				return false;
		}
	}
	else
	{
		return (keyboard[key] > 0);
	}
}

bool keyHeld(int key)
{
	return buttonHeld(key);
}


int initialiseScreen(RenderFrame *ScreenData, int width, int height, bool Fullscreen)
{
	if (ScreenData == NULL)
	{
		return MISSING_DATA;
	}

	ScreenData->Window = NULL;
	ScreenData->Renderer = NULL;
	ScreenData->Fullscreen = false;
	ScreenData->Scaled = false;
	ScreenData->FramesElapsed = 0;
	ScreenData->FrameTimer  = 0;

	// Any additional flags such as borderless or moveable can be applied here
	SDL_WindowFlags windowFlag = 0;

	if (Fullscreen == true)
	{
		windowFlag |= SDL_WINDOW_FULLSCREEN;
	}

	SDL_CreateWindowAndRenderer("Starting up...", width, height, windowFlag, &ScreenData->Window, &ScreenData->Renderer);

	if (ScreenData->Window == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Lemon failed to create window!", NULL);
		return LEMON_ERROR;
	}

	SDL_GetWindowSizeInPixels(ScreenData->Window, &ScreenData->screenWidth, &ScreenData->screenHeight);

	if (ScreenData->Renderer == NULL)
	{
		cleanUpSDLRenderer(ScreenData);
		ScreenData->Window = NULL;

		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Lemon failed to create renderer!", NULL);

		return LEMON_ERROR;
	}

	SDL_SetRenderLogicalPresentation(ScreenData->Renderer, width, height, SDL_LOGICAL_PRESENTATION_STRETCH);

	ScreenData->textEngine = TTF_CreateRendererTextEngine(ScreenData->Renderer);
	if (ScreenData->textEngine == NULL)
	{
		putConsoleError("Failed to create Text renderer.");
	}

	SetWindowTitle("Lemon Engine");							// Initial window title
	SetWindowIcon("MissingIcon");							// Initial window icon

	putConsoleString("Screen initialised!\n");

	return LEMON_SUCCESS;
}



bool SetWindowIcon(const char fileName[])
{
	if (fileName == NULL || strlen(fileName) >= MAX_LEN)
	{
		return false;
	}

	char path[4 + (MAX_LEN << 1)] = SPRITE_ROOT;
	strcat(path, fileName);
	strcat(path, ".bmp");

	SDL_Surface *icon = SDL_LoadBMP(path);

	if (icon == NULL)
	{
		putConsoleError("Error loading window icon: %s", SDL_GetError());
		return false;
	}
	
	bool result =  SDL_SetWindowIcon(ScreenData.Window, icon);
	SDL_DestroySurface(icon);

	return result;
}


bool SetWindowTitle(const char newTitle[])
{
	if (newTitle == NULL || strlen(newTitle) >= MAX_LEN)
	{
		return false;
	}

	return SDL_SetWindowTitle(ScreenData.Window, newTitle);
}

int setVsync(bool enabled)
{
	if (enabled)
	{
		SDL_SetRenderVSync(ScreenData.Renderer, 1);
	}
	else
	{
		SDL_SetRenderVSync(ScreenData.Renderer, 0);
	}

	RenderSettings.vSync = enabled;

	return LEMON_SUCCESS;
}

int setTickRate(int desiredTickRate)
{
	if (desiredTickRate > 1000 || desiredTickRate < 1)
	{
		return INVALID_DATA;
	}

	EngineSettings.GameTicksPerSecond = desiredTickRate;

	EngineSettings.TickDelta = (Uint64)(NS_PER_SECOND/(double)EngineSettings.GameTicksPerSecond);

	if (EngineSettings.GameTicksPerSecond == RenderSettings.RendersPerSecond)
	{
        RenderSettings.RenderDelta = EngineSettings.TickDelta;
	}

	return LEMON_SUCCESS;
}


int setRenderRefreshRate(int desiredRenderRate)
{
	if (desiredRenderRate > 1000000 || desiredRenderRate < 1)
	{
		return INVALID_DATA;
	}

	RenderSettings.RendersPerSecond = desiredRenderRate;

	RenderSettings.RenderDelta = (Uint64)(NS_PER_SECOND/(double)RenderSettings.RendersPerSecond);

	if (EngineSettings.GameTicksPerSecond == RenderSettings.RendersPerSecond)
	{
        EngineSettings.TickDelta = RenderSettings.RenderDelta;
	}

	return LEMON_SUCCESS;
}


void MasterControls(World *GameWorld, SDL_Window *window)
{
	if (GameWorld == NULL || !DEBUG_MODE || DebugSettings.TypingInConsole)
	{
		return;
	}

	if (keyboard[LMN_LSHIFT] == 0)
	{
		return;
	}

	if (keyboard['J'] == 1)
	{
		enableFullscreen(GameWorld);
	}

	if (keyboard['H'] == 1)
	{
		disableFullscreen(GameWorld);
	}

	if (keyboard[LMN_BACKSPACE] == 1)
	{
		setVsync(!RenderSettings.vSync);
	}

	if (keyboard['1'] == 1)
	{
		DebugSettings.DebugTextDisplayMode = (DebugSettings.DebugTextDisplayMode + 1) % DEBUG_TEXT_MODE_COUNT;
		putConsoleString("\nToggling draw Debug Text: %d", DebugSettings.DebugTextDisplayMode);
	}

	if (keyboard['2'] == 1)
	{
    	DebugSettings.PauseEngine = (DebugSettings.PauseEngine + 1) % 2;
		putConsoleString("\nToggling Pause: %d", DebugSettings.PauseEngine);
    }

	if (keyboard[LMN_ENTER] == 1 && DebugSettings.PauseEngine == ENGINE_PAUSED)
	{
		GameWorld->MainCamera.CameraMode = FREE_ROAM;
		DebugSettings.PauseEngine = ENGINE_SINGLE_TICK;
		GameFrame(GameWorld);
		GameTick(GameWorld);
		DebugSettings.PauseEngine = ENGINE_PAUSED;	
		GameWorld->MainCamera.CameraMode = FOLLOW_PLAYER;	
	}


	if (keyboard['3'] == 1)
	{
		DebugSettings.ConsoleTextEnabled = (DebugSettings.ConsoleTextEnabled + 1) % CONSOLE_TEXT_SETTING_COUNT;
		putConsoleString("\nToggling Console Text: %d",DebugSettings.ConsoleTextEnabled);
	}


	if (DebugSettings.DebugTextDisplayMode)
	{
		if (keyboard[LMN_COMMA] == 1)
		{
			DebugSettings.DebugTextInfoPreset = modulo(DebugSettings.DebugTextInfoPreset - 1, 15);
		}
		
		if (keyboard[LMN_PERIOD] == 1)
		{
			DebugSettings.DebugTextInfoPreset = (DebugSettings.DebugTextInfoPreset + 1) % 15;
		}

		if (keyboard['4'] == 1)
		{
			DebugSettings.CameraInfo = (DebugSettings.CameraInfo + 1) % 4;
		}

		if (keyboard['5'] == 1)
		{
			DebugSettings.DebugOverlay = (DebugSettings.DebugOverlay + 1) % 2;
		}

		if (keyboard['6'] == 1)
		{
			DebugSettings.FPSCounter = (DebugSettings.FPSCounter + 1) % 2;
		}

		if (keyboard['7'] == 1)
		{
			DebugSettings.SoundInfo = (DebugSettings.SoundInfo + 1) % (CHANNEL_COUNT + 1);
		}
	}

	if (DebugSettings.PauseEngine == ENGINE_PAUSED || GameWorld->Player.PlayerPtr == NULL)
	{
		if (keyboard[LMN_LEFT])
		{
			GameWorld->MainCamera.CameraX -= 16.0;
		}

		if (keyboard[LMN_RIGHT])
		{
			GameWorld->MainCamera.CameraX += 16.0;
		}

		if (keyboard[LMN_UP])
		{
			GameWorld->MainCamera.CameraY += 12.0;
		}

		if (keyboard[LMN_DOWN])
		{
			GameWorld->MainCamera.CameraY -= 12.0;
		}
	}

	if (keyboard[LMN_UPARROW] == 1 && RenderSettings.drawHitboxes == 1)
	{ 
		RenderSettings.HitboxOutlineThickness++;
	}

	if (keyboard[LMN_DOWNARROW] == 1 && RenderSettings.drawHitboxes == 1)
	{
		RenderSettings.HitboxOutlineThickness--;
	}

	if (keyboard['R'] == 1)
	{
		setCameraPos(&GameWorld->MainCamera, 0.0, 0.0);
	}

	if (keyboard['I'] == 1)
	{
		if (GameWorld->PhysicsType == PLATFORMER)
		{
			GameWorld->PhysicsType = TOP_DOWN;
		}
		else
		{
			GameWorld->PhysicsType = PLATFORMER;
		}
	}

	if (keyboard['O'] == 1)
	{
		AddObject(GameWorld, UI_ELEMENT, 0, 0, LEVEL_FADE, 0, 0, 0, 0);
	}

    return;
}


void putConsoleString(const char input[], ...)
{
	if (input[0] < 9)
	{
		return;
	}

	va_list argptr;
    va_start(argptr, input);
    vsnprintf(DebugSettings.ConsoleString, CONSOLE_STRING_LENGTH, input, argptr);
    va_end(argptr);

    addInputHistory(DebugSettings.ConsoleString, &DebugSettings.consoleHistory);

    if (DEBUG_MODE)
    {
    	printf("%s\n", DebugSettings.ConsoleString);
    	fflush(stdout);
    }

	return;
}


void putConsoleStringTS(const char input[], ...)
{
	if (input[0] < 9)
	{
		return;
	}

	char buffer[CONSOLE_STRING_LENGTH] = {0};
	snprintf(buffer, CONSOLE_STRING_LENGTH, "[Tick: %llu] %s", TickNum, input);

	va_list argptr;
    va_start(argptr, input);
    vsnprintf(DebugSettings.ConsoleString, CONSOLE_STRING_LENGTH, buffer, argptr);
    va_end(argptr);

    addInputHistory(DebugSettings.ConsoleString, &DebugSettings.consoleHistory);

   	if (DEBUG_MODE)
    {
    	printf("%s\n", DebugSettings.ConsoleString);
    	fflush(stdout);
    }
	
	return;
}

void putConsoleError(const char input[], ...)
{
	if (input[0] < 9 || !DebugSettings.showErrors)
	{
		return;
	}

	char errorString[CONSOLE_STRING_LENGTH] = {0};
	snprintf(errorString, CONSOLE_STRING_LENGTH, "Error: %s", input);

	va_list argptr;
    va_start(argptr, input);
    vsnprintf(DebugSettings.ConsoleString, CONSOLE_STRING_LENGTH, errorString, argptr);
    va_end(argptr);

    addInputHistory(DebugSettings.ConsoleString, &DebugSettings.consoleHistory);

    if (DEBUG_MODE)
    {
    	printf("%s\n", DebugSettings.ConsoleString);
    	fflush(stdout);
    }

    return;
}

void addInputHistory(const char input[], InputHistory *history)
{
	int length = strlen(input);
	if (length >= CONSOLE_STRING_LENGTH)
	{
		length = CONSOLE_STRING_LENGTH - 1;
	}

	memcpy(history->inputs[history->head], input, length);
	history->inputs[history->head % USER_INPUT_HISTORY_LEN][length] = 0;

	history->head = (history->head + 1) % USER_INPUT_HISTORY_LEN;
	history->searchIndex = history->head;
	if (history->entries < USER_INPUT_HISTORY_LEN)
	{
		history->entries++;
	}
	

	return;
}

char* getPreviousInputHistory(InputHistory *history)
{
	history->searchIndex = modulo(history->searchIndex - 1, USER_INPUT_HISTORY_LEN);
		
	if (history->inputs[history->searchIndex][0] == '\0')
	{
		history->searchIndex = modulo(history->searchIndex + 1, USER_INPUT_HISTORY_LEN);
	} 

	return history->inputs[history->searchIndex];
}

char* getNextInputHistory(InputHistory *history)
{
	if (history->searchIndex == history->head)
	{
		return history->inputs[history->searchIndex];
	}

	history->searchIndex = modulo(history->searchIndex + 1, USER_INPUT_HISTORY_LEN);
		
	if (history->inputs[history->searchIndex][0] == '\0')
	{
		history->searchIndex = modulo(history->searchIndex - 1, USER_INPUT_HISTORY_LEN);
	} 

	return history->inputs[history->searchIndex];
}


int ResetCamera(Camera *inputCam)
{
	if (inputCam == NULL)
	{
		return MISSING_DATA;
	}

	inputCam->CameraX = 0;
	inputCam->CameraY = 0;
	inputCam->minCameraX = -(int)EngineSettings.WorldBoundX;
	inputCam->maxCameraX = (int)EngineSettings.WorldBoundX;
	inputCam->minCameraY = -(int)EngineSettings.WorldBoundY;
	inputCam->maxCameraY = (int)EngineSettings.WorldBoundY;
	inputCam->CameraLatch = false;
	inputCam->CameraXBuffer = 0;
	inputCam->CameraYBuffer = 0;
	inputCam->CameraMode = FOLLOW_PLAYER;

	inputCam->zoomX = 1.0;
	inputCam->zoomY = 1.0;
	inputCam->zoomedWidth = ScreenData.screenWidth;
	inputCam->zoomedHeight = ScreenData.screenHeight;
	inputCam->width = ScreenData.screenWidth;
	inputCam->height = ScreenData.screenHeight;

	return LEMON_SUCCESS;
}

int setCameraPos(Camera *input, float xPos, float yPos)
{
	if (input == NULL) { return MISSING_DATA; }

	input->CameraX = clamp(xPos, input->minCameraX, input->maxCameraX);
	input->CameraY = clamp(yPos, input->minCameraY, input->maxCameraY);;


	return LEMON_SUCCESS;
}


void SetEngineSettingsToDefault(void)
{
	EngineSettings.ContiguousAnimAllocation = CONTIGUOUS_ANIMATION_ALLOCATION;

	EngineSettings.PreservedSpriteSets = PRESERVED_SPRITESETS;
	EngineSettings.ReservedObjects = RESERVED_OBJECTS;

	EngineSettings.WorldBoundX = X_WORLD_BOUND;
	EngineSettings.WorldBoundY = Y_WORLD_BOUND;

	EngineSettings.MaxSoundsPerChannel = MAX_SOUNDS_PER_CHANNEL;
	EngineSettings.MaxTextQueueLength = MAX_TEXTQUEUE_LENGTH;
	EngineSettings.MaxSceneActions = MAX_SCENEACTIONS;

	EngineSettings.GameTicksPerSecond = 0;
	EngineSettings.TickDelta = 999999999;
	setTickRate(TICKS_PER_SECOND);


	if (EngineSettings.DefaultTexture == NULL)
	{
		EngineSettings.DefaultTexture = loadSprite(DEFAULT_TEXTURE, NULL, TILE);
	}

	resetDisplayData(&EngineSettings.DefaultDisplay);
	EngineSettings.DefaultDisplay.spriteBuffer = EngineSettings.DefaultTexture;
}


void SetRenderSettingsToDefault(void)
{
	RenderSettings.drawSprites = true;
	RenderSettings.drawBackGround = true;
	RenderSettings.drawParticles = true;
	RenderSettings.drawHUD = true;
	RenderSettings.drawCamViews = true;

	RenderSettings.drawHitboxes = false;
	RenderSettings.HitboxOutlineThickness = 4;

	RenderSettings.maxObjects = MAX_OBJECTS_RENDER;
	RenderSettings.maxParticles = MAX_PARTICLES_RENDER;
	RenderSettings.maxUIElements = MAX_HUD_ELEMENTS_RENDER;

	RenderSettings.drawnObjects = 0;

	RenderSettings.RendersPerSecond = 0;
	RenderSettings.RenderDelta = 1;
	setRenderRefreshRate(RENDERS_PER_SECOND);

	setVsync(VSYNC_DEFAULT);
}


void SetTextSettingsToDefault(void)
{
	TextSettings.defaultTextPointSize = 44.0;
	TextSettings.portraitSize = 200;
	strcpy(TextSettings.defaultFont, DEFAULT_FONT);

	TextSettings.DebugTextColour.r = 255;
	TextSettings.DebugTextColour.g = 255;
	TextSettings.DebugTextColour.b = 255;
	TextSettings.DebugTextColour.a = SDL_ALPHA_TRANSPARENT;
	TextSettings.DebugTextPointSize = 18.0;

	if (TextSettings.FontList.font[0] == NULL)
	{
		loadFontWithSize("PTSansBold.ttf", "DebugFont", TextSettings.DebugTextPointSize);
	}

	RemoveAllTexts(&TextSettings.DebugTexts);
}

void SetDebugSettingsToDefault(void)
{
	DebugSettings.DebugTextDisplayMode = DEBUG_TEXT_DISABLED;
	DebugSettings.DebugTextInfoPreset = 0;
	DebugSettings.FPSCounter = 0;
	DebugSettings.CameraInfo = 0;
	DebugSettings.DebugOverlay = 0;
	DebugSettings.SoundInfo = 0;
	DebugSettings.showEvents = DEBUG_MODE ? true : false;
	DebugSettings.showSceneActions = DEBUG_MODE ? true : false;
	DebugSettings.showSpriteset = DEBUG_MODE ? true : false;
	DebugSettings.showErrors = DEBUG_MODE ? true : false;

	DebugSettings.ConsoleTextEnabled = DEBUG_MODE ? CONSOLE_ONLY_ERRORS : CONSOLE_TEXT_DISABLED;
	DebugSettings.TypingInConsole = SDL_TextInputActive(ScreenData.Window);
	DebugSettings.userInputIndex = -1;
	DebugSettings.argIndex = 0;
	DebugSettings.cursorXPos = 0.0;

	DebugSettings.PauseEngine = ENGINE_UNPAUSED;

	DebugSettings.noclip = false;
}


int SetGravity(World *GameWorld, float force, float directionDegrees)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}


	double directionRadian = directionDegrees * DEGREE_TO_RADIAN_PI;

	double sinVal = sin(directionRadian);
	double cosVal = cos(directionRadian);

	GameWorld->GlobalGravityX = (float)(force * sinVal);
	GameWorld->GlobalGravityY = (float)(force * cosVal);

	if (fabs(GameWorld->GlobalGravityX) < 0.01)
	{
		GameWorld->GlobalGravityX = 0.0;
	}

	if (fabs(GameWorld->GlobalGravityY) < 0.01)
	{
		GameWorld->GlobalGravityY = 0.0;
	}


	return LEMON_SUCCESS;
}


// Utility functions
int clamp(int input, int lowerBound, int upperBound)
{
	if (lowerBound > upperBound)
	{
		return input;
	}

	if (input < lowerBound)
		return lowerBound;

	if (input > upperBound)
		return upperBound;
	
	return input;
}

double dClamp(double input, double lowerBound, double upperBound)
{
	if (lowerBound > upperBound)
	{
		return input;
	}
	
	if (input < lowerBound)
	{
		return lowerBound;
	}

	if (input > upperBound)
	{
		return upperBound;
	}

	return input;
}

float fClamp(float input, float lowerBound, float upperBound)
{
	if (lowerBound > upperBound)
	{
		return input;
	}

	if (input < lowerBound)
	{
		return lowerBound;
	}

	if (input > upperBound)
	{
		return upperBound;
	}

	return input;
}


bool inRange(int input, int low, int high)
{
	if (input < low || input > high)
	{
		return false;
	}
	else 
	{
		return true;
	}
	
}

bool inRangeExclusive(int input, int low, int high)
{
	if (input <= low || input >= high)
	{
		return false;
	}
	else 
	{
		return true;
	}
	
}

int max(int left, int right)
{
	if (left >= right)
	{
		return left;
	}
	else
	{
		return right;
	}
}

int min(int left, int right)
{
	if (left <= right)
	{
		return left;
	}
	else
	{
		return right;
	}
}

int modulo(int x, int N)
{
    return (x % N + N) % N;
}

float fModulo(float x, float N)
{
	if (x < 0.0)
	{
		return fmodf(fmodf(x, N) + N, N);
	}
	else
	{
		return fmodf(x, N);
	}
    
}


void stringToUpper(char input[])
{
	int i = 0;

	while (input[i] > 31 && i < MAX_LEN)
	{
		input[i] = toupper(input[i]);
		
		i++;
	}

	return;
}

void stringToLower(char input[])
{
	int i = 0;

	while (input[i] > 31 && i < MAX_LEN)
	{
		input[i] = tolower(input[i]);
		
		i++;
	}

	return;
}


int PickRandomIntBetween(int low, int high)
{
	if (low >= high)
	{
		return 0;
	}

	int range = high - low;

	return (rand() % range) + low;
}

float PickRandomFloatBetween(float low, float high)
{
	if (low >= high)
	{
		return 0.0;
	}
	
	float range = high - low;

	float generatedValue = ((float)rand()/(float)(RAND_MAX)) * range;

	return generatedValue + low;
}


int sparseInitialise(IntSparseList *input)
{
	int *sparse = input->sparse;

	// -1 is tombstone value (empty slot)
	for (int i = 0; i < EngineSettings.MaxObjects; i++)
	{
		sparse[i] = -1;
	}

	input->storedElements = 0;

	memset(input->dense, 0, sizeof(int) * MAX_OBJECTS);

	return LEMON_SUCCESS;
}

int sparseAdd(int input, IntSparseList *List)
{
	if (input < 0 || List == NULL)
	{
		return -1;
	}

	if (List->storedElements >= MAX_OBJECTS)
	{
		return -1;
	}

	// dont add/overwrite if the slot is already present (-1 indicates empty)
	if (List->sparse[input] >= 0)
	{
		return -1;
	}

	List->dense[List->storedElements] = input;
	int newSlot = List->dense[List->storedElements];
	List->sparse[input] = List->storedElements;
	List->storedElements++;

	return newSlot;
}

int sparseRemove(int input, IntSparseList *List)
{
	if (input < 0)
	{
		return INVALID_DATA;
	}

	int denseIndex = List->sparse[input];
	if (denseIndex < 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	// set to -1 to indicate its empty, deletion of data is optional
	int *denseList = List->dense;
	int lastIndex = List->storedElements - 1;

	// swap last and component to delete
	if (denseIndex != lastIndex)
	{
		int temp = denseList[denseIndex];

		denseList[denseIndex] = denseList[lastIndex];

		denseList[lastIndex] = temp;
	}

	List->sparse[input] = -1;
	List->storedElements--;

	return LEMON_SUCCESS;
}

int sparseGet(int input, IntSparseList *List)
{
	if (input < 0 || List == NULL)
	{
		return -1;
	}

	int *sparse = List->sparse;
	int *dense = List->dense;

	if (sparse[input] < 0)
	{
		return -1;
	}
	else
	{
		return dense[sparse[input]];
	}
}

int stackAdd(int input, StackArray *List)
{
	if (input < 0 || List == NULL)
	{
		return -1;
	}

	if (List->storedElements >= STACKARRAY_LENGTH)
	{
		return -1;
	}

	List->list[List->storedElements] = input;
	List->storedElements++;

	return input;
}

int stackPop(StackArray *List)
{
	if (List == NULL)
	{
		return MISSING_DATA;
	}

	List->storedElements--;

	return LEMON_SUCCESS;
}

int stackRemove(int input, StackArray *List)
{
	if (input < 0)
	{
		return INVALID_DATA;
	}

	int foundIndex = STACKARRAY_LENGTH - 1;
	while (foundIndex >= 0 && List->list[foundIndex] != input)
	{
		foundIndex--;
	}

	if (foundIndex < 0)
	{	
		return INVALID_DATA;
	}


	// deletion of data is optional
	int lastIndex = List->storedElements - 1;

	// swap last and component to delete
	if (foundIndex != lastIndex)
	{
		int temp = List->list[foundIndex];

		List->list[foundIndex] = List->list[lastIndex];

		List->list[lastIndex] = temp;
	}

	List->storedElements--;

	return LEMON_SUCCESS;
}


void clearString(String *input)
{
	if (input == NULL)
	{
		return;
	}

	if (input->stringChars)
	{
		free(input->stringChars);
		input->stringChars = NULL;
	}

	input->length = 0;

	return;
}

void setString(String *input, const char stringInput[])
{
	if (input == NULL || stringInput == NULL)
	{
		return;
	}

	int length = strlen(stringInput);

	if (length < 1)
	{
		return;
	}

	if (input->stringChars != NULL)
	{
		free(input->stringChars);
	}

	input->stringChars = malloc(sizeof(char) * length);
	if (input->stringChars == NULL)
	{
		input->length = 0;
		return;
	}

	memcpy(input->stringChars, stringInput, length * sizeof(char));
	input->length = length;

	return;
}


void freeString(String *input)
{
	if (input == NULL)
	{
		return;
	}

	input->length = 0;

	if (input->stringChars)
	{
		free(input->stringChars);
	}

	return;
}


void copyString(String source, String *destination)
{
	if (source.stringChars == NULL)
	{
		return;
	}

	freeString(destination);

	destination->stringChars = malloc(sizeof(char) * source.length);
	if (destination->stringChars == NULL)
	{
		return;
	}

	destination->length = source.length;

	memcpy(destination->stringChars, source.stringChars, source.length);

	return;
}


void concatString(String *string1, String string2)
{
	String temp = {0};
	copyString(*(string1), &temp);

	freeString(string1);


	string1->length = temp.length + string2.length;
	string1->stringChars = malloc(sizeof(char) * string1->length);

	if (string1->stringChars == NULL)
	{
		string1->length = 0;
		return;
	}

	memcpy(string1->stringChars, temp.stringChars, temp.length);
	memcpy(string1->stringChars + temp.length, string2.stringChars, string2.length);

	return;
}

void concatStringCStr(String *string1, const char *string2)
{
	String temp = {0};
	copyString(*(string1), &temp);

	freeString(string1);

	int str2Length = 0;
	while(string2[str2Length] != 0)
	{
		str2Length++;
	}

	string1->length = temp.length + str2Length;
	string1->stringChars = malloc(sizeof(char) * string1->length);

	if (string1->stringChars == NULL)
	{
		string1->length = 0;
		return;
	}

	memcpy(string1->stringChars, temp.stringChars, temp.length);
	memcpy(string1->stringChars + temp.length, string2, str2Length);

	return;
}

void setStringUpper(String input)
{
	for (int i = 0; i < input.length; i++)
	{
		input.stringChars[i] = toupper(input.stringChars[i]);
	}

	return;
}

void setStringLower(String input)
{
	for (int i = 0; i < input.length; i++)
	{
		input.stringChars[i] = tolower(input.stringChars[i]);
	}

	return;
}

bool stringEquals(String input1, String input2)
{
	if (input1.length != input2.length)
	{
		return false;
	}

	for (int i = 0; i < input1.length; i++)
	{
		if (input1.stringChars[i] != input2.stringChars[i])
		{
			return false;
		}
	}

	return true;
}

bool stringContains(String input, String sub)
{
	int subIndex = 0;
	int iterations = 0;

	for (int i = iterations; i < input.length && subIndex < sub.length; i++)
	{
		if (input.stringChars[i] == sub.stringChars[subIndex])
		{
			subIndex++;
		}
		else
		{
			subIndex = 0;
			iterations++;
			i = iterations;
		}
	}

	if (subIndex == sub.length)
	{
		return true;
	}

	return false;
}
// not great, has complexity of n(n + 1)/2

char at(String input, int index)
{
	if (index < 0 || index > input.length || input.stringChars == NULL)
	{
		return 0;
	}

	return input.stringChars[index];
}

bool stringEqualsCString(String input1, const char input2[])
{
	if (input2 == NULL || input1.stringChars == NULL)
	{
		return false;
	}

	String temp = {0};
	setString(&temp, input2);

	return stringEquals(input1, temp);
}

void printString(String input)
{
	printf("%.*s", input.length, input.stringChars);	

	return;
}

void printStringLine(String input)
{
	printf("\n%.*s", input.length, input.stringChars);	

	return;
}