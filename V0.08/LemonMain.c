#include "LemonMain.h"


int Running_In_Windows_Mode = 0;


SoundChannel SoundChannels[CHANNEL_COUNT];

int screenWidth = H_RESOLUTION;

int screenHeight = V_RESOLUTION;

RenderFrame ScreenData = {0};


MouseData MouseInput = {0};

int keyboard[256] = {0};


EngineData EngineSettings = {0};

RenderData RenderSettings = {0};

DebugData DebugSettings = {0};




// Lemon Engine main - SDL
int RunLemonEngine(void)
{
	if (StartUpLemonEngine() == LEMON_ERROR)
	{
		return LEMON_ERROR;
	}

	// Game initialisation
	World GameWorld;
    initialiseWorld(&GameWorld);
	StartGame(&GameWorld);


	// The loop code is run directly here without MainLoop() for performance when executing as a standalone program
	int gameTick = 0;
	int renderRefresh = 0;
	int timeElapsed = 0;
    clock_t lastFrameTime = 0;
	
    while(GameWorld.GameState != CLOSE_GAME)
    {
    	// Window messages
	    getExternalInput(&GameWorld);


		// Timing
	    timeElapsed = (int)(((float)(clock() - lastFrameTime) / (float)CLOCKS_PER_SEC) * 10000);
	    lastFrameTime = clock();

	    gameTick += timeElapsed;
	    renderRefresh += timeElapsed;


	    // World updates
	    while (gameTick >= EngineSettings.TickDelta)
	    {
	    	gameTick -= EngineSettings.TickDelta;
			GameTick(&GameWorld);
	    }

	    GameFrame(&GameWorld);

	    // Render screen
	    if (renderRefresh >= RenderSettings.RenderDelta)
	    {
			renderRefresh = renderRefresh % RenderSettings.RenderDelta;
		   	RenderSDL(&GameWorld);
		}


		// Process sound
		IterateAudio();
    }

	CloseGame(&GameWorld);


	return LEMON_SUCCESS;
}


int StartUpLemonEngine()
{
	// SDL initialisation
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to initialise SDL! Ensure SDL.dll is in directory with executable.", NULL);
		return LEMON_ERROR;
	}

	if (!TTF_Init())
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to initialise TTF! Ensure SDL_ttf.dll is in directory with executable.", NULL);
		return LEMON_ERROR;
	}


	// Create RenderFrame
	if (initialiseScreen(&ScreenData, screenWidth, screenHeight, false) != LEMON_SUCCESS)
	{
		return LEMON_ERROR;
	}

	initialiseAudio();
    ClearKeyboardInput();

	return LEMON_SUCCESS;
}

/*
int MainLoop(World *GameWorld, static int gameTick, static int renderRefresh, static int timeElapsed, static int lastFrameTime)
{
	return LEMON_SUCCESS;
}
*/

int CloseGame(World *GameWorld)
{
	// Clear game data and cleanup
	clearGameData(GameWorld);

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
		return ACTION_DISABLED;
	}

	MasterControls(GameWorld, &GameWorld->Player);

	if (DebugSettings.PauseEngine == 1)
	{
		return ACTION_DISABLED;
	}

	UpdateCutscene(GameWorld);

	updateText(GameWorld);

	updateObjects(GameWorld);

	WorldCameraControl(GameWorld, &GameWorld->MainCamera);

	// Input acknowledgement is delayed until the next frame by doing this so that gameFrame can use this frame's input correctly
	keyboard[ACKNOWLEDGE_INPUT] = 1;

	return LEMON_SUCCESS;
}


// Updates every frame
int GameFrame(World *GameWorld)
{
	if (GameWorld == NULL || DebugSettings.PauseEngine == 1)
	{
		return ACTION_DISABLED;
	}

	updateObjectsFrame(GameWorld);

	HandleGameWorldEvents(GameWorld, &ScreenData);


	return LEMON_SUCCESS;
}


int RenderEngine(World *GameWorld, Camera renderCamera, RenderFrame ScreenData)
{
	if (GameWorld == NULL || ScreenData.Renderer == NULL)
	{
		return MISSING_DATA;
	}

	float camXCorrection = (float)((screenWidth - renderCamera.zoomedWidth) >> 1);
	float camYCorrection = (float)((renderCamera.zoomedHeight - screenHeight) >> 1);

	renderCamera.CameraX += camXCorrection;
	renderCamera.CameraY += camYCorrection;
	
	renderBackGroundSprite(renderCamera, GameWorld->WorldBackground, ScreenData);

	drawObjects(renderCamera, GameWorld, ScreenData);

	if (RenderSettings.drawHitboxes == 1)
	{
		drawHitboxes(renderCamera, GameWorld, ScreenData);
	}

    renderCamera.CameraX -= camXCorrection;
	renderCamera.CameraY -= camYCorrection;

	if (DebugSettings.DebugTextDisplayMode != DEBUG_TEXT_DISABLED)
    {
    	DisplayDebugInfo(GameWorld, DebugSettings.DebugTextDisplayMode);

		RenderDebugText(renderCamera, ScreenData);	
    }

    FPSCounter();

	return LEMON_SUCCESS;
}


int RenderSDL(World *GameWorld)
{
	if (GameWorld == NULL || ScreenData.Window == NULL || ScreenData.Renderer == NULL)
	{
		return MISSING_DATA;
	}

	SDL_SetRenderDrawColor(ScreenData.Renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(ScreenData.Renderer);
	
	RenderEngine(GameWorld, GameWorld->MainCamera, ScreenData);
	
	SDL_RenderPresent(ScreenData.Renderer);

	return LEMON_SUCCESS;
}


int FPSCounter(void)
{
	static int TextIndex = -1;
    ScreenData.FramesElapsed++;

    float timePassed = ((float)(clock() - ScreenData.lastSecond) / (float)CLOCKS_PER_SEC);

	if (timePassed > 0.999)
    {
    	if (DebugSettings.ConsoleTextEnabled != CONSOLE_TEXT_DISABLED)
    	{
    		printf("\n%d FPS at %.5f", ScreenData.FramesElapsed, timePassed);
    	}

    	if (DebugSettings.FPSCounter != 0 && timePassed > 0)
		{
			char buffer[40] = {0};
	        sprintf(buffer, "%d FPS", ScreenData.FramesElapsed);
	        RemoveDebugText(TextIndex);
			TextIndex = AddDebugText(buffer, SCREEN_LIST_FORMAT, 20 - (screenWidth >> 1), 0, 1000);
		} 
       
	    ScreenData.FramesElapsed = 0;
	    ScreenData.lastSecond = clock();
    }

    if (DebugSettings.FPSCounter == 0 && TextIndex > -1)
    {
 		RemoveDebugText(TextIndex);
 		TextIndex = -1;
    }

	printConsoleData();

	return LEMON_SUCCESS;
}


// Game functions
int initialiseWorld(World *GameWorld)
{
	SetEngineSettingsToDefault();
	SetRenderSettingsToDefault();
	SetDebugSettingsToDefault();

	putConsoleString("\nStarting up...\n");
	printConsoleData();

	memset(GameWorld, 0, sizeof(World));
	srand(0);

	// Game world creation
	ResetCamera(&GameWorld->MainCamera);

	GameWorld->level = 0;
	GameWorld->GameEvents.EventID = NO_EVENT;
	GameWorld->GameEvents.additionalEvent = NULL;
	memset(&GameWorld->GameEvents.EventData, 0, sizeof(GameEventData));

	GameWorld->GamePaused = 0;

	GameWorld->TextQueue = NULL;
	GameWorld->PlayingText = 0;
	GameWorld->CurrentCutscene = NO_CUTSCENE;
	GameWorld->SceneActionQueue = NULL;

	// Check for resource data access
	if (CheckResourceData() == MISSING_DATA)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing Data", 
			"Missing Lemon resource data! \nPlease check that the LemonData folder is available and in the same directory as the executable.", ScreenData.Window);
		GameWorld->GameState = CLOSE_GAME;
		return MISSING_DATA;
	}

	
	// Object controller creation
	ObjectController *objController = createObjectController();

	if (objController == NULL)
	{
		putConsoleString("\nError: Could not allocate space for Object Controller.\n\n");
		return LEMON_ERROR;
	}

	GameWorld->ObjectList = objController;


	// Load backgrounds
	GameWorld->WorldBackground.bgParallax = 0.1;
	GameWorld->WorldBackground.BackgroundSpriteBuffer = NULL;
	GameWorld->WorldBackground.BackgroundRenderMode = DEFAULT_TO_SPRITE;
	GameWorld->WorldBackground.BackgroundSpriteSet = NULL;

	setSourceToDesiredSpriteSet(&GameWorld->WorldBackground.BackgroundSpriteSet, BACKGROUND_SETID);
	loadAnimationsFromFile("BackGrounds", GameWorld->WorldBackground.BackgroundSpriteSet);

	InitialisePlayerData(&GameWorld->Player);

	GameWorld->GlobalGravityX = 0.0;
	GameWorld->GlobalGravityY = 0.0;
	SetGravity(GameWorld, 1.0, 180.0);

	GameWorld->PhysicsType = PLATFORMER;

	GameWorld->GameState = EMPTY_GAME;

	putConsoleString("Initialised World\n");
	printConsoleData();

	return LEMON_SUCCESS;
}


FuncResult CheckResourceData(void)
{
	char path[strlen(SPRITE_ROOT) + strlen(DEFAULT_TEXTURE) + 2];
	strcpy(path, SPRITE_ROOT);
	strcat(path, DEFAULT_TEXTURE);

	if (access(path, F_OK | R_OK | W_OK) == -1)
	{
		return MISSING_DATA;
	}

	char newPath[strlen(LEVELDATA_ROOT) + strlen("Level0.txt") + 2];
	strcpy(newPath, LEVELDATA_ROOT);
	strcat(newPath, "Level0.txt");

	if (access(newPath, F_OK | R_OK | W_OK) == -1)
	{
		return MISSING_DATA;
	}

	return LEMON_SUCCESS;
}


int getExternalInput(World *GameWorld)
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
    		case SDL_EVENT_QUIT:
    			  GameWorld->GameState = CLOSE_GAME;
    		break;

	    	case SDL_EVENT_KEY_DOWN:
	    	case SDL_EVENT_KEY_UP:
    			getKeyboardInput(&event);
    		break;

    		case SDL_EVENT_MOUSE_BUTTON_DOWN:
    		case SDL_EVENT_MOUSE_BUTTON_UP:
    			getMouseInput(event.button);
    		break;

    		case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
    			ScreenData.Fullscreen = true;
    		break;

    		case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
    			ScreenData.Fullscreen = false;
    		break;

    		default:
    		break;
    	}
	}

	updateMouse();

	return LEMON_SUCCESS;
}


int ClearKeyboardInput()
{
	for (int i = LMN_SPACE; i < INPUT_COUNT; i++)
	{
		keyboard[i] = 0;	
	}


	return LEMON_SUCCESS;
}


int AcknowledgeHeldButtons()
{
	for (int i = LMN_SPACE; i < INPUT_COUNT; i++)
	{
		if (keyboard[i] == 1)
		{
			keyboard[i] = 2;
		}
	}

	// Mouse buttons will be 1 when they are first clicked, then set to 2 automatically here 
	// (MouseInput.LeftButton != 0 => Mouse held down)  (MouseInput.LeftButton == 1 => Mouse just clicked)
	if (MouseInput.LeftButton == 1)
	{
		MouseInput.LeftButton = 2;
	}

	if (MouseInput.RightButton == 1)
	{
		MouseInput.RightButton = 2;
	}

	if (MouseInput.MiddleButton == 1)
	{
		MouseInput.MiddleButton = 2;
	}

	return LEMON_SUCCESS;
}

int AcknowledgeButton(LemonKeys Key)
{
	if (Key > INPUT_COUNT || Key < 0)
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

			default:
				return INVALID_DATA;
		}
	}

	if (keyboard[Key] == 1)
	{
		keyboard[Key] = 2;
	}

	return LEMON_SUCCESS;
}


int updateMouse(void)
{
	SDL_GetMouseState(&MouseInput.xPos, &MouseInput.yPos);

	MouseInput.xPos -= (ScreenData.windowWidth >> 1);
	MouseInput.yPos = (ScreenData.windowHeight >> 1) - MouseInput.yPos;

	MouseInput.xPos *= (float)screenWidth/(float)ScreenData.windowWidth;
	MouseInput.yPos *= (float)screenHeight/(float)ScreenData.windowHeight;

	return LEMON_SUCCESS;
}

// Get mouse position corrected for zoom (HUD layer is immune to zoom already, so this is only for other layers)
float getMouseXZoom(Camera inputCamera)
{
	return MouseInput.xPos / inputCamera.zoomX;
}

float getMouseYZoom(Camera inputCamera)
{
	return MouseInput.yPos / inputCamera.zoomY;
}


int getMouseInput(SDL_MouseButtonEvent event)
{
	switch(event.button)
	{	
		case SDL_BUTTON_LEFT:
			MouseInput.LeftButton = event.down;
			break;

		case SDL_BUTTON_RIGHT:
			MouseInput.RightButton = event.down;
			break;

		case SDL_BUTTON_MIDDLE:
			MouseInput.MiddleButton = event.down;
			break;

		default:
		break;
	}

	return LEMON_SUCCESS;
}


int getKeyboardInput(SDL_Event *event)
{
	if (event->key.repeat)
	{
		return EXECUTION_UNNECESSARY;
	}

	int keyCode = 0;

	switch (event->key.scancode)
	{
		case SDL_SCANCODE_ESCAPE:
			keyCode = LMN_ESCAPE;
			break;

		case SDL_SCANCODE_SPACE:
			keyCode = LMN_SPACE;
			break;

		case SDL_SCANCODE_KP_ENTER:
		case SDL_SCANCODE_RETURN:
			keyCode = LMN_ENTER;
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
			if (event->key.scancode >= SDL_SCANCODE_A && event->key.scancode <= SDL_SCANCODE_Z)
			{
				keyCode = 'A' + event->key.scancode - SDL_SCANCODE_A;
			}
			else if (event->key.scancode >= SDL_SCANCODE_1 && event->key.scancode <= SDL_SCANCODE_9)
			{
				keyCode = '1' + event->key.scancode - SDL_SCANCODE_1;
			}
			break;
	}
	
	if (event->type == SDL_EVENT_KEY_DOWN)
	{
		keyboard[keyCode] = (keyboard[keyCode] < 2);
	}
	else
	{
		keyboard[keyCode] = 0;
	}
	
	switch(keyCode)
	{
		case 'A':
		case LMN_LEFTARROW:
		keyboard[LMN_LEFT] = keyboard['A'] || keyboard[LMN_LEFTARROW];
		break;


		case 'D':
		case LMN_RIGHTARROW:
		keyboard[LMN_RIGHT] = keyboard['D'] || keyboard[LMN_RIGHTARROW];
		break;

		case 'W':
		case LMN_UPARROW:
		keyboard[LMN_UP] = keyboard['W'] || keyboard[LMN_UPARROW];
		break;

		case 'S':
		case LMN_DOWNARROW:
			keyboard[LMN_DOWN] = keyboard['S'] || keyboard[LMN_DOWNARROW];
		break;

		case LMN_SPACE:
		keyboard[LMN_JUMP] = keyboard[LMN_SPACE];
		break;

		case 'E':
		case 'Z':
		keyboard[LMN_INTERACT] = keyboard['E'] || keyboard['Z'];
		break;

		case 'Q':
		case 'X':
		keyboard[LMN_INTERACT2] = keyboard['Q'] || keyboard['X'];
		break;

		case 'R':
		case 'C':
		keyboard[LMN_INTERACT3] = keyboard['R'] || keyboard['C'];
		break;

		default:
		break;
	}

	keyboard[LMN_TEXT_CONFIRM] = keyboard[LMN_INTERACT] || keyboard[LMN_ENTER];
	keyboard[LMN_TEXT_SKIP] = keyboard[LMN_INTERACT2] || keyboard[LMN_SPACE];
	keyboard[LMN_MENU_CONFIRM] = keyboard[LMN_INTERACT] || keyboard[LMN_ENTER];
			
	return LEMON_SUCCESS;

}


int initialiseScreen(RenderFrame *ScreenData, int width, int height, bool Fullscreen)
{
	if (ScreenData == NULL)
	{
		return MISSING_DATA;
	}

	width = clamp(width, 144, 4000);
	height = clamp(height, 144, 4000);

	ScreenData->Window = NULL;
	ScreenData->Screen = NULL;
	ScreenData->Renderer = NULL;
	ScreenData->Fullscreen = false;
	ScreenData->FramesElapsed = 0;
	ScreenData->lastSecond = 0;

	// Any additional flags such as borderless or moveable can be applied here
	SDL_WindowFlags windowFlag = 0;

	if (Fullscreen == true)
	{
		windowFlag = windowFlag | SDL_WINDOW_FULLSCREEN;
	}

	SDL_CreateWindowAndRenderer("Starting up...", width, height, windowFlag, &ScreenData->Window, &ScreenData->Renderer);		

	if (ScreenData->Window == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Lemon failed to create window!", NULL);
		return LEMON_ERROR;
	}

	SDL_GetWindowSizeInPixels(ScreenData->Window, &ScreenData->windowWidth, &ScreenData->windowHeight);
	screenWidth = ScreenData->windowWidth;
	screenHeight = ScreenData->windowHeight;

	putConsoleString("\nInitialising Screen...");

	if (ScreenData->Renderer == NULL)
	{
		cleanUpSDLRenderer(*ScreenData);
		ScreenData->Window = NULL;

		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Lemon failed to create renderer!", NULL);

		return LEMON_ERROR;
	}


	SDL_SetRenderLogicalPresentation(ScreenData->Renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_STRETCH);
	SetWindowTitle("Lemon Engine");							// Initial window title
	SetWindowIcon("MissingIcon");							// Initial window icon

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
		putConsoleStrStr("\nError loading window icon: ", SDL_GetError());
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


int setTickRate(int desiredTickRate)
{
	if (desiredTickRate > 200 || desiredTickRate < 0)
	{
		return INVALID_DATA;
	}

	EngineSettings.GameTicksPerSecond = desiredTickRate;

	double TargetDelta = (1.0/(double)EngineSettings.GameTicksPerSecond);
	EngineSettings.TickDelta = (int)((TargetDelta) * 10000) - 5;

	if (EngineSettings.GameTicksPerSecond == RenderSettings.RendersPerSecond)
	{
		EngineSettings.TickDelta = RenderSettings.RenderDelta;
	}

	return LEMON_SUCCESS;
}


int setRenderRefreshRate(int desiredRenderRate)
{
	if (desiredRenderRate > 200 || desiredRenderRate < 0)
	{
		return INVALID_DATA;
	}

	RenderSettings.RendersPerSecond = desiredRenderRate;

	double TargetDelta = (1.0/(double)RenderSettings.RendersPerSecond);
	RenderSettings.RenderDelta = (int)(TargetDelta * 10000) - 5;

	if (EngineSettings.GameTicksPerSecond == RenderSettings.RendersPerSecond)
	{
		RenderSettings.RenderDelta = EngineSettings.TickDelta;
	}

	return LEMON_SUCCESS;
}


void MasterControls(World *GameWorld, PlayerData *player)
{
	if (GameWorld == NULL || GameWorld->GameState == EMPTY_GAME)
	{
		return;
	}


	if (keyboard['L'])
	{
		if (keyboard['1'] == 1)
		{
			keyboard['1'] = 2;

			DebugSettings.DebugTextInfoPreset = (DebugSettings.DebugTextInfoPreset + 1) % 16;
		}

		if (keyboard['2'] == 1)
		{
			keyboard['2'] = 2;

			DebugSettings.DebugTextInfoPreset--;
			if (DebugSettings.DebugTextInfoPreset < 0)
			{
				DebugSettings.DebugTextInfoPreset = 15;
			}
		}

		if (keyboard['3'] == 1)
		{
			keyboard['3'] = 2;

			DebugSettings.CameraInfo = (DebugSettings.CameraInfo + 1) % 4;
		}

		if (keyboard['4'] == 1)
		{
			keyboard['4'] = 2;

			DebugSettings.PauseStatus = (DebugSettings.PauseStatus + 1) % 2;
		}

		if (keyboard['5'] == 1)
		{
			keyboard['5'] = 2;

			DebugSettings.FPSCounter = (DebugSettings.FPSCounter + 1) % 2;
		}

		if (keyboard['6'] == 1)
		{
			keyboard['6'] = 2;

			DebugSettings.ObjectCount = (DebugSettings.ObjectCount + 1) % 2;
		}

		if (keyboard['7'] == 1)
		{
			keyboard['7'] = 2;
			DebugSettings.CutsceneInfo = (DebugSettings.CutsceneInfo + 1) % 2;
		}

		if (keyboard['0'] == 1)
		{
			keyboard['0'] = 2;

			DebugSettings.SoundInfo = (DebugSettings.SoundInfo + 1) % (CHANNEL_COUNT + 1);
		}
	}

	if (keyboard['1'] == 1)
	{
    	keyboard['1'] = 2;
    	RenderSettings.drawSprites = (RenderSettings.drawSprites + 1) % 2;
    	putConsoleStrInt("\nToggling Draw Sprites: ", RenderSettings.drawSprites);
	}


	if (keyboard['2'] == 1)
	{
		keyboard['2'] = 2;
		RenderSettings.drawBackGround = (RenderSettings.drawBackGround + 1) % 2;
		putConsoleStrInt("\nToggling draw background: ", RenderSettings.drawBackGround);
	}


	if (keyboard['3'] == 1)
	{
		keyboard['3'] = 2;
		RenderSettings.drawHitboxes = (RenderSettings.drawHitboxes + 1) % 2;
		putConsoleStrInt("\nToggling draw hitboxes: ", RenderSettings.drawHitboxes);
	}


	if (keyboard['4'] == 1)
	{
		keyboard['4'] = 2;
		DebugSettings.ConsoleTextEnabled = (DebugSettings.ConsoleTextEnabled + 1) % CONSOLE_TEXT_SETTING_COUNT;
	}


	if (keyboard['5'] == 1)
	{
		keyboard['5'] = 2;

		DebugSettings.DebugTextDisplayMode = (DebugSettings.DebugTextDisplayMode + 1) % DEBUG_TEXT_MODE_COUNT;

		putConsoleStrInt("\nToggling draw Debug Text: ", DebugSettings.DebugTextDisplayMode);
	}


	if (keyboard['6'] == 1)
	{
		keyboard['6'] = 2;
    	DebugSettings.PauseEngine = (DebugSettings.PauseEngine + 1) % 2;
		putConsoleStrInt("\nToggling Pause: ", DebugSettings.PauseEngine);
    }

	if (keyboard['7'] == 1)
	{
		keyboard['7'] = 2;
		DebugSettings.PauseEngine = 2;
		GameTick(GameWorld);
		GameFrame(GameWorld);
		DebugSettings.PauseEngine = 1;		
	}


	if (keyboard['8'] == 1)
	{ 
		keyboard['8'] = 2;
		DebugSettings.HitboxOutlineThickness++;
	}

	if (keyboard['9'] == 1)
	{
		keyboard['9'] = 2;
		DebugSettings.HitboxOutlineThickness--;
	}

	if (DebugSettings.PauseEngine == 1 && keyboard['L'])
	{
		if (keyboard[LMN_LEFT] == 1)
		{
			GameWorld->MainCamera.CameraX -= 16;
		}

		if (keyboard[LMN_RIGHT] == 1)
		{
			GameWorld->MainCamera.CameraX += 16;
		}

		if (keyboard[LMN_UP] == 1)
		{
			GameWorld->MainCamera.CameraY += 16;
		}

		if (keyboard[LMN_DOWN] == 1)
		{
			GameWorld->MainCamera.CameraY -= 16;
		}
	}

	
	if (keyboard['P'] == 1)
	{
		keyboard['P'] = 2;
		streamPartition(1, GameWorld);
	}

/*
	if (keyboard['O'] == 1)
	{
		keyboard['O'] = 2;
		GameWorld->GameEvent = DISABLE_FULLSCREEN;
	}

	if (keyboard['K'] == 1)
	{
		keyboard['K'] = 2;
		GameWorld->GameEvent = ENABLE_FULLSCREEN_SCALE;
	}


	if (keyboard['I'] == 1)
	{
		keyboard['I'] = 1;
		GameWorld->GameEvent = CHANGE_SCREEN_ZOOM;
		GameWorld->GameEventData.zoomScales[0] = 0.01;
		GameWorld->GameEventData.zoomScales[1] = 0.01;
	}

	if (keyboard['U'] == 1)
	{
		keyboard['U'] = 1;
		GameWorld->GameEvent = CHANGE_SCREEN_ZOOM;
		GameWorld->GameEventData.zoomScales[0] = -0.01;
		GameWorld->GameEventData.zoomScales[1] = -0.01;	

		//String new = {0};
		//String new2 = {0};
		//setString(&new, "HELLOO");
		//setString(&new2, " World!");
		//concatString(&new, new2);
		//printString(new);
		//printString(new2);

		//freeString(&new);
		//freeString(&new2);
	}

	*/


    return;
}


void printConsoleData()
{
	if (DebugSettings.ConsoleString[0] > 9 && DebugSettings.ConsoleTextEnabled != CONSOLE_TEXT_DISABLED)
	{
		printf("%s", DebugSettings.ConsoleString);
	}

	clearConsoleString();

	fflush(stdout);

	return;
}


void clearConsoleString()
{
	memset(DebugSettings.ConsoleString, 0, sizeof(char) * CONSOLE_STRING_LENGTH);

	return;
}


int putConsoleString(const char input[])
{
	if (input[0] < 9)
	{
		return INVALID_DATA;
	}

	int stringIndex = 0;

	while (stringIndex < CONSOLE_STRING_LENGTH && DebugSettings.ConsoleString[stringIndex] != 0)
	{
		stringIndex++;
	}

	if (stringIndex >= CONSOLE_STRING_LENGTH)
	{
		return AT_FULL_CAPACITY;
	}

	int spaceLeft = CONSOLE_STRING_LENGTH - stringIndex;
	stringIndex = 0;

	while (stringIndex < spaceLeft && input[stringIndex] != 0)
	{
		stringIndex++;
	}

	if (stringIndex >= spaceLeft)
	{
		return INVALID_DATA;
	}

	strcat(DebugSettings.ConsoleString, input);

	return LEMON_SUCCESS;
}


int putConsoleInteger(int input)
{
	char buffer[30] = {0};

	convertIntToStr(buffer, input);

	return putConsoleString(buffer);
}


int putConsoleStrStr(const char input1[], const char input2[])
{
	if (putConsoleString(input1) != LEMON_SUCCESS)
	{
		return AT_FULL_CAPACITY;
	}

	return putConsoleString(input2);
}


int putConsoleStrInt(const char strInput[], int intInput)
{
	if (putConsoleString(strInput) != LEMON_SUCCESS)
	{
		return AT_FULL_CAPACITY;
	}

	return putConsoleInteger(intInput);
}


int putConsoleStrIntStr(const char strInput1[], int intInput, const char strInput2[])
{
	if (putConsoleString(strInput1) != LEMON_SUCCESS)
	{
		return AT_FULL_CAPACITY;
	}

	if (putConsoleInteger(intInput) != LEMON_SUCCESS)
	{
		return AT_FULL_CAPACITY;
	}
	
	return putConsoleString(strInput2);
}


int putConsoleDouble(double input)
{
	char buffer[64] = {0};
	sprintf(buffer, "%lf", input);

	return putConsoleString(buffer);
}


int putConsoleStrDouble(const char strInput[], double doubleInput)
{
	if (putConsoleString(strInput) != LEMON_SUCCESS)
	{
		return AT_FULL_CAPACITY;
	}	

	return putConsoleDouble(doubleInput);
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
	inputCam->zoomedWidth = screenWidth;
	inputCam->zoomedHeight = screenHeight;

	return LEMON_SUCCESS;
}


int SetEngineSettingsToDefault(void)
{
	EngineSettings.MultiThreadingEnabled = MULTITHREADED_ENABLED;
	EngineSettings.MaxObjects = MAX_OBJECTS;
	EngineSettings.PreservedSpriteSets = PRESERVED_SPRITESETS;
	EngineSettings.ReservedObjects = RESERVED_OBJECTS;

	EngineSettings.ObjectPreAllocationEnabled = OBJECT_PREALLOCATION;

	EngineSettings.WorldBoundX = X_WORLD_BOUND;
	EngineSettings.WorldBoundY = Y_WORLD_BOUND;

	EngineSettings.MaxSoundsPerChannel = MAX_SOUNDS_PER_CHANNEL;
	EngineSettings.MaxTextQueueLength = MAX_TEXTQUEUE_LENGTH;
	EngineSettings.MaxGameEvents = MAX_QUEUED_GAME_EVENTS;

	EngineSettings.GameTicksPerSecond = 0;
	EngineSettings.TickDelta = 999999999;
	setTickRate(TICKS_PER_SECOND);


	if (EngineSettings.DefaultTexture == NULL)
	{
		EngineSettings.DefaultTexture = loadSprite(DEFAULT_TEXTURE, NULL, TILE);
	}
	
	return LEMON_SUCCESS;
}


int SetRenderSettingsToDefault(void)
{
	RenderSettings.drawSprites = 1;
	RenderSettings.drawBackGround = 1;
	RenderSettings.drawObjects = 1;
	RenderSettings.drawParticles = 1;
	RenderSettings.drawUI = 1;
	RenderSettings.drawPlayer = 1;

	RenderSettings.drawHitboxes = 0;

	RenderSettings.maxObjects = MAX_OBJECTS_RENDER;
	RenderSettings.maxParticles = MAX_PARTICLES_RENDER;
	RenderSettings.maxUIElements = MAX_HUD_ELEMENTS_RENDER;

	RenderSettings.drawnObjects = 0;
	RenderSettings.drawnParticles = 0;
	RenderSettings.drawnHudElements = 0;

	RenderSettings.RendersPerSecond = 0;
	RenderSettings.RenderDelta = 1;
	setRenderRefreshRate(RENDERS_PER_SECOND);

	return LEMON_SUCCESS;
}


int SetDebugSettingsToDefault(void)
{
	DebugSettings.HitboxOutlineThickness = 4;
	DebugSettings.DisplayPlayerData = 0;
	DebugSettings.ConsoleTextEnabled = ONLY_ERRORS;
	clearConsoleString();

	DebugSettings.DebugTextDisplayMode = DEBUG_TEXT_DISABLED;
	DebugSettings.DebugTextInfoPreset = 0;
	DebugSettings.ObjectCount = 0;
	DebugSettings.FPSCounter = 0;
	DebugSettings.PauseStatus = 0;
	DebugSettings.CameraInfo = 0;
	DebugSettings.SoundInfo = 0;

	DebugSettings.DebugTextColour.r = 255;
	DebugSettings.DebugTextColour.g = 255;
	DebugSettings.DebugTextColour.b = 255;
	DebugSettings.DebugTextColour.a = SDL_ALPHA_TRANSPARENT;
	RemoveAllDebugTexts();
	memset(DebugSettings.DebugTexts, 0, MAX_DEBUG_TEXTS * sizeof(DebugText));

	DebugSettings.PauseEngine = 0;

	// Set rendering to default
	RenderSettings.drawSprites = 1;
	RenderSettings.drawBackGround = 1;
	RenderSettings.drawObjects = 1;
	RenderSettings.drawParticles = 1;
	RenderSettings.drawUI = 1;
	RenderSettings.drawPlayer = 1;

	RenderSettings.drawHitboxes = 0;

	return LEMON_SUCCESS;
}



ObjectController* createObjectController(void)
{
	ObjectController *newController = malloc(sizeof(ObjectController));

	if (newController == NULL)
	{
		return NULL;
	}

	newController->lastObject = NULL;
	newController->firstObject = NULL;
	newController->objectCount = 0;
	newController->spriteSetCount = 0;
	newController->startSpriteSetPtr = NULL;
	newController->cachedFirstObject = NULL;
	newController->cachedLastObject = NULL;
	newController->availableSlots = NULL;
	newController->cachedCount = 0;

	newController->FrameUpdates = NULL;

	if (EngineSettings.ObjectPreAllocationEnabled == 1)
	{
		int i = 0;
		Object *newObject = NULL;

		while (i < EngineSettings.MaxObjects)
		{
			newObject = createNewObject();

			if (newObject == NULL)
			{
				return newController;
			}

			if (newController->availableSlots != NULL)
			{
				newController->availableSlots->prevObject = newObject;
			}

			newObject->nextObject = newController->availableSlots;
			newController->availableSlots = newObject;
			i++;
		}
	}

	return newController;
}


void clearGameData(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return;
	}
	
	EngineSettings.ObjectPreAllocationEnabled = 0;
	clearLevelData(GameWorld);
	deleteAllGameEvents(GameWorld);

	SpriteSet *currentSet;
	currentSet = GameWorld->ObjectList->startSpriteSetPtr;

	SpriteSet *prevSet;
	prevSet = NULL;

	while (currentSet != NULL)
	{
		prevSet = currentSet;
		currentSet = currentSet->nextSet;

		deleteSpriteSet(prevSet, GameWorld->ObjectList);
	}
	
	free(GameWorld->ObjectList);
	GameWorld->ObjectList = NULL;
	
	currentSet = GameWorld->WorldBackground.BackgroundSpriteSet;
	prevSet = NULL;
	
	while (currentSet != NULL)
	{
		prevSet = currentSet;
		currentSet = currentSet->nextSet;

		deleteSpriteSet(prevSet, NULL);
	}

	GameWorld->WorldBackground.BackgroundSpriteSet = NULL;
	GameWorld->WorldBackground.BackgroundSpriteBuffer = NULL;

	free(GameWorld->Player.InteractBox);
	memset(&GameWorld->Player, 0, sizeof(PlayerData));

	GameWorld->GameState = EMPTY_GAME;

	return;
}


int cleanUpSDLRenderer(RenderFrame ScreenData)
{
	// Destroy window 
	SDL_DestroyRenderer(ScreenData.Renderer);
    SDL_DestroyWindow(ScreenData.Window);

    SetDebugSettingsToDefault();

	return LEMON_SUCCESS;
}


int SetGravity(World *GameWorld, float force, double directionDegrees)
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


int LoadBackGroundSprites(BackgroundData *WorldBackground)
{
	loadSpriteIntoDesiredSet("BG_Water", "Backgrounds", &WorldBackground->BackgroundSpriteSet, BACKGROUND_SETID, TILE);
	loadSpriteIntoDesiredSet("BG_GreenHill", "Backgrounds", &WorldBackground->BackgroundSpriteSet, BACKGROUND_SETID, SINGLE);

	return LEMON_SUCCESS;
}


// Utility functions
int clamp(int input, int lowerBound, int upperBound)
{
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


double dClamp(double input, double lowerBound, double upperBound)
{
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
	if (low > high)
	{
		return 0;
	}

	int range = high - low;

	return (rand() % range) + low;
}


float PickRandomFloatBetween(float low, float high)
{
	if (low > high)
	{
		return 0.0;
	}
	
	float range = high - low;

	float generatedValue = ((float)rand()/(float)(RAND_MAX)) * range;

	return generatedValue + low;
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

	if (input->stringChars == NULL)
	{
		return;
	}

	free(input->stringChars);

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

	for (int i = 0; i < source.length; i++)
	{
		destination->stringChars[i] = source.stringChars[i];
	}

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

	for (int i = 0; i < temp.length; i++)
	{
		string1->stringChars[i] = temp.stringChars[i];
	}

	for (int i = temp.length; i < string1->length; i++)
	{
		string1->stringChars[i] = string2.stringChars[i];
		printf("\n%c", string2.stringChars[i]);
	}


	return;
}


void printString(String input)
{
	for (int i = 0; i < input.length; i++)
	{
		printf("%c", input.stringChars[i]);
	}

	return;
}