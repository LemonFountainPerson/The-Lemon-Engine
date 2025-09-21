#include "LemonMain.h"


int MaxSoundsPerChannel = MAX_SOUNDS_PER_CHANNEL;

SoundChannel SoundChannels[CHANNEL_COUNT];

int GameTicksPerSecond = TICKS_PER_SECOND;

int TickDelta = TICK_DELTA;

int RendersPerSecond = RENDERS_PER_SECOND;

int RenderDelta = RENDER_DELTA;

int screenWidth = H_RESOLUTION;

int screenHeight = V_RESOLUTION;

uint32_t *screenBuffer;

unsigned char RotateRenderBuffer[MAX_SPRITE_SIZE * MAX_SPRITE_SIZE * 4] = {0};

MouseData MouseInput = {0};

int keyboard[256] = {0};


EngineData EngineSettings = {0};

RenderData RenderSettings = {0};

DebugData DebugSettings = {0};


// Lemon Engine main - SDL
int RunLemonEngine(void)
{
	// SDL initialisation
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0)
	{
		printf("Failed to initialise SDL\n");
		return -1;
	}

    SDL_Window *Window = NULL;
    SDL_Renderer *Renderer;
	SDL_Surface *Surface;

	Window = SDL_CreateWindow("Lemon Engine", screenWidth - 1, screenHeight - 1, SDL_WINDOW_BORDERLESS);

	/*
	Icon for application window
	SDL_Surface *IconSurface = SDL_CreateSurface(32, 32, RGBA_8888);

	SDL_SetWindowIcon(Window, IconSurface);
	*/

	Renderer = SDL_CreateRenderer(Window, NULL);
	  
	Surface = SDL_GetWindowSurface(Window);


	// Check that the window was successfully created
    if (Window == NULL || Surface == NULL || Renderer == NULL) 
	{
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

   
	// Core engine variables
	int gameRunning = 1;
	int gameTick = 0;
	int renderRefresh = 0;
	int timeElapsed = 0;
    clock_t lastFrameTime = 0;


    // Game initialisation
    screenBuffer = Surface->pixels;

	if (screenBuffer == NULL)
	{
		printf("Unable to allocate memory for game frame!\n");
		return LEMON_ERROR;
	}

	screenWidth = Surface->pitch;

	World GameWorld;
    InitialiseGame(&GameWorld);
    initialiseAudio();

	StartGame(&GameWorld);

	lastFrameTime = clock();

	
    // Game Loop
    while(GameWorld.GameState != CLOSE_GAME)
    {
    	// Window messages
        getExternalInput(&GameWorld);


		// Timing
        timeElapsed = ((clock() - lastFrameTime) / (double)CLOCKS_PER_SEC) * 10000;

        lastFrameTime = clock();

        gameTick += timeElapsed;
        renderRefresh += timeElapsed;


	    // World updates
	    while (gameTick >= TickDelta)
        {
        	gameTick -= TickDelta;
			GameTick(&GameWorld);
	    }

        GameFrame(&GameWorld);


	    // Render screen
	    if (renderRefresh >= RenderDelta)
	    {
	    	renderRefresh = renderRefresh % RenderDelta;
	    	
		   	RenderSDL(&GameWorld, Window, Renderer, Surface);
		}


		// Process sound
		IterateAudio();
    }

	// Clear game data and cleanup
	clearGameData(&GameWorld);

	cleanUpAudioData();

	cleanUpSDLRenderer(Window, Renderer, Surface);

    SDL_Quit();


	return LEMON_SUCCESS;
}


// Updates 60 times per second (can be modified)
int GameTick(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	UpdateCutscene(GameWorld);

	updateText(GameWorld);

	UpdatePlayer(GameWorld->Player, GameWorld);

	updateObjects(GameWorld);

	WorldCameraControl(GameWorld, &GameWorld->MainCamera);

	return LEMON_SUCCESS;
}


// Updates every frame
int GameFrame(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	updateObjectsFrame(GameWorld);

	HandleGameWorldEvents(GameWorld);


	MasterControls(GameWorld, GameWorld->Player);


	return LEMON_SUCCESS;
}


int getExternalInput(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
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

    		default:
    		break;
    	}
	}

	updateMousePosition();

	return LEMON_SUCCESS;
}



int updateMousePosition(void)
{
	SDL_GetMouseState(&MouseInput.xPos, &MouseInput.yPos);

	MouseInput.xPos -= (screenWidth >> 1);
	MouseInput.yPos = (screenHeight >> 1) - MouseInput.yPos;

	return LEMON_SUCCESS;
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
	int keyCode = 0;

	switch (event->key.scancode)
	{
		case 41:
			keyCode = LMN_ESCAPE;
			break;

		case 44:
			keyCode = LMN_SPACE;
			break;

		case 80:
			keyCode = LMN_LEFTARROW;
			break;

		case 79:
			keyCode = LMN_RIGHTARROW;
			break;

		case 82:
			keyCode = LMN_UPARROW;
			break;

		case 81:
			keyCode = LMN_DOWNARROW;
			break;

		default:
			if (event->key.scancode > 3 && event->key.scancode < 30)
			{
				keyCode = 'A' + event->key.scancode - 4;
			}
			break;
	}
	

	if (event->type == SDL_EVENT_KEY_DOWN)
	{
		keyboard[keyCode] = 1;
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

			
	return LEMON_SUCCESS;

}


int RenderSDL(World *GameWorld, SDL_Window *Window, SDL_Renderer *Renderer, SDL_Surface *Surface)
{
	if (GameWorld == NULL || Window == NULL || Renderer == NULL || Surface == NULL)
	{
		return MISSING_DATA;
	}


	SDL_LockSurface(Surface);
	//RenderEngine(GameWorld);
	SDL_UnlockSurface(Surface);

	SDL_UpdateWindowSurface(Window);

	return LEMON_SUCCESS;
}


int RenderEngine(World *GameWorld)
{
	if (GameWorld == NULL || screenBuffer == NULL)
	{
		return MISSING_DATA;
	}

	GameWorld->drawnObjects = 0;
	GameWorld->drawnParticles = 0;
	GameWorld->drawnHudElements = 0;
	
	renderBackGroundSprite(GameWorld->MainCamera, GameWorld->WorldBackground);

	drawObjects(GameWorld->MainCamera, GameWorld);

	if (RenderSettings.drawHitboxes == 1)
	{
		drawHitboxes(GameWorld->MainCamera, GameWorld);
	}

	return LEMON_SUCCESS;
}


// SDL Functions
int cleanUpSDLRenderer(SDL_Window *Window, SDL_Renderer *Renderer, SDL_Surface *Surface)
{
	// Close Renderer/Window
	SDL_DestroySurface(Surface);

	SDL_DestroyRenderer(Renderer);

    SDL_DestroyWindow(Window);

	return LEMON_SUCCESS;
}


int resizeSDLWindow()
{



	return LEMON_SUCCESS;
}


int setTickRate(int desiredTickRate)
{
	if (desiredTickRate > 200 || desiredTickRate < 0)
	{
		return INVALID_DATA;
	}

	GameTicksPerSecond = desiredTickRate;

	double TargetDelta = (1.0/(double)GameTicksPerSecond);
	TickDelta = (int)((TargetDelta) * 10000);

	return LEMON_SUCCESS;
}


int setRenderRefreshRate(int desiredRenderRate)
{
	if (desiredRenderRate > 200 || desiredRenderRate < 0)
	{
		return INVALID_DATA;
	}

	RendersPerSecond = desiredRenderRate;

	double TargetDelta = (1.0/(double)RendersPerSecond);
	RenderDelta = (int)((TargetDelta) * 10000);

	return LEMON_SUCCESS;
}


void MasterControls(World *GameWorld, PlayerData *player)
{
	if (GameWorld == NULL || GameWorld->GameState == EMPTY_GAME)
	{
		return;
	}


	if (keyboard['Y'] == 1)
	{
		AddObject(GameWorld, PROJECTILE, 100, 100, 0, 0, 0, 0, 0, 0, 0);

		RotateObject(GameWorld->Player->PlayerPtr, 45.0, ROTATE_SPRITE);

		keyboard['Y'] = 2;
	}

	if (keyboard['T'] == 1)
	{
		keyboard['T'] = 2;
		DebugSettings.DebugTextEnabled = (DebugSettings.DebugTextEnabled + 1) % 3;
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


	if (keyboard['U'] == 1)
	{
		keyboard['U'] = 2;
		RenderSettings.drawHitboxes = (RenderSettings.drawHitboxes + 1) % 2;
		putDebugStrInt("Toggling draw hitboxes: ", RenderSettings.drawHitboxes);
	}


	if (keyboard['I'] == 1)
	{
		keyboard['I'] = 2;
		RenderSettings.drawBackGround = (RenderSettings.drawBackGround + 1) % 2;
		putDebugStrInt("Toggling draw background: ", RenderSettings.drawBackGround);
	}


	if (keyboard['O'] == 1)
	{
    	keyboard['O'] = 2;
    	RenderSettings.drawSprites = (RenderSettings.drawSprites + 1) % 2;
    	putDebugStrInt("Toggling Draw Sprites: ", RenderSettings.drawSprites);
	}


	if (keyboard['P'] == 1)
	{
    	keyboard['P'] = 2;
    	DebugSettings.DisplayPlayerData = (DebugSettings.DisplayPlayerData + 1) % 2;
    	putDebugStrInt("Toggling Player data display: ", DebugSettings.DisplayPlayerData);
	}


	if (keyboard['0'] == 1)
	{
		keyboard['0'] = 2;
    	//GameWorld->GamePaused = (GameWorld->GamePaused + 1) % 2;
		//putDebugStrInt("Toggling Pause: ", GameWorld->GamePaused);
		GameWorld->PhysicsType = (GameWorld->PhysicsType + 1) % 2;
    }


	if (player->PlayerBox != NULL && DebugSettings.DisplayPlayerData == 1 && GameWorld->GamePaused == 0)
	{
		putDebugStrDouble("Player: X: ", player->PlayerBox->xPos);
		putDebugStrDouble(" Y: ", player->PlayerBox->yPos);
		putDebugStrDouble(" xVel: ", player->PlayerBox->xVelocity);
		putDebugStrDouble(" yVel: ", player->PlayerBox->yVelocity);
		putDebugStrDouble(" Forward Velocity: ", player->PlayerBox->forwardVelocity);
		putDebugStrDouble(" Direction (radians): ", player->PlayerBox->direction);
		putDebugStrDouble(" Direction (radians): ", player->PlayerDisplay->direction);
	}


	// Debug Variables
	static clock_t lastSecond = 0;
    static int windowsFrames = 0;

    if (DebugSettings.DebugTextEnabled != DEBUG_TEXT_DISABLED)
    {
		if (((double)(clock() - lastSecond) / (double)CLOCKS_PER_SEC) > 0.98)
	    {
	        printf("\n%d at %lf", windowsFrames, ((double)(clock() - lastSecond) / (double)CLOCKS_PER_SEC) );
	   
		    windowsFrames = 0;
		    lastSecond = clock();
	    }
	   
		windowsFrames++;

		printDebugData();
	}


    return;
}


int printDebugData()
{
	if (DebugSettings.DebugString[0] > 9)
	{
		printf("\n\n---------------------------------------");
		printf("\nDebug Info:\n");
		printf(DebugSettings.DebugString);
		printf("\n---------------------------------------\n");
	}

	clearDebugString();

	return LEMON_SUCCESS;
}


int printDebugDataNoHeader()
{
	if (DebugSettings.DebugTextEnabled == DEBUG_TEXT_DISABLED)
	{
		return ACTION_DISABLED;
	}

	if (DebugSettings.DebugString[0] > 9)
	{
		printf(DebugSettings.DebugString);
	}

	clearDebugString();

	return LEMON_SUCCESS;
}


int clearDebugString()
{
	memset(DebugSettings.DebugString, 0, sizeof(char) * DEBUG_STRING_LENGTH);

	return 0;
}


int putDebugString(const char input[])
{
	if (input[0] < 9)
	{
		return INVALID_DATA;
	}

	int stringIndex = 0;

	while (stringIndex < DEBUG_STRING_LENGTH && DebugSettings.DebugString[stringIndex] != 0)
	{
		stringIndex++;
	}

	if (stringIndex >= DEBUG_STRING_LENGTH)
	{
		return AT_FULL_CAPACITY;
	}

	int spaceLeft = DEBUG_STRING_LENGTH - stringIndex;
	stringIndex = 0;

	while (stringIndex < spaceLeft && input[stringIndex] != 0)
	{
		stringIndex++;
	}

	if (stringIndex >= spaceLeft)
	{
		return INVALID_DATA;
	}

	strcat(DebugSettings.DebugString, input);

	return LEMON_SUCCESS;
}


int putDebugInteger(int input)
{
	char buffer[30] = {0};

	convertIntToStr(buffer, input);

	return putDebugString(buffer);
}


int putDebugStrStr(const char input1[], const char input2[])
{
	if (putDebugString(input1) != LEMON_SUCCESS)
	{
		return AT_FULL_CAPACITY;
	}

	return putDebugString(input2);
}


int putDebugStrInt(const char strInput[], int intInput)
{
	if (putDebugString(strInput) != LEMON_SUCCESS)
	{
		return AT_FULL_CAPACITY;
	}

	return putDebugInteger(intInput);
}


int putDebugStrIntStr(const char strInput1[], int intInput, const char strInput2[])
{
	if (putDebugString(strInput1) != LEMON_SUCCESS)
	{
		return AT_FULL_CAPACITY;
	}

	if (putDebugInteger(intInput) != LEMON_SUCCESS)
	{
		return AT_FULL_CAPACITY;
	}
	
	return putDebugString(strInput2);
}


int putDebugDouble(double input)
{
	char buffer[64] = {0};
	sprintf(buffer, "%lf", input);

	return putDebugString(buffer);
}


int putDebugStrDouble(const char strInput[], double doubleInput)
{
	if (putDebugString(strInput) != LEMON_SUCCESS)
	{
		return AT_FULL_CAPACITY;
	}	

	return putDebugDouble(doubleInput);
}


void frameRate(int rate, clock_t gameTick)
{
	SDL_Delay((800/(float)rate) - ((double)gameTick / (double)CLOCKS_PER_SEC));
}


// Game functions
FunctionResult InitialiseGame(World *GameWorld)
{
	SetEngineSettingsToDefault();
	SetRenderSettingsToDefault();
	SetDebugSettingsToDefault();

	srand(0);

	// Game world creation
	clearDebugString();
	ResetCamera(&GameWorld->MainCamera);

	GameWorld->level = 0;
	GameWorld->GameEvent = NO_EVENT;
	GameWorld->CurrentCutscene = EMPTY_CUTSCENE;
	GameWorld->SceneTick = 0;
	GameWorld->PlayingText = 0;

	GameWorld->drawnObjects = 0;
	GameWorld->drawnParticles = 0;
	GameWorld->drawnHudElements = 0;

	GameWorld->GamePaused = 0;

	GameWorld->TextQueue = NULL;

	
	// Object controller creation
	ObjectController *objController = createObjectController();

	if (objController == NULL)
	{
		printf("\nError: Could not allocate space for Object Controller.\n\n");
		return LEMON_ERROR;
	}

	GameWorld->ObjectList = objController;


	// Load backgrounds
	GameWorld->WorldBackground.bgParallax = 0.1;
	GameWorld->WorldBackground.bgRowParallax = 0.00006;
	GameWorld->WorldBackground.ParallaxRowCutOff = 640;
	GameWorld->WorldBackground.ParallaxRowStart = 1000;
	GameWorld->WorldBackground.BackgroundSpriteBuffer = NULL;
	GameWorld->WorldBackground.BackgroundSpriteSet = NULL;

	LoadBackGroundSprites(&GameWorld->WorldBackground);

	GameWorld->Player = InitialisePlayerData(GameWorld);

	GameWorld->GlobalGravityX = 0.0;
	GameWorld->GlobalGravityY = 0.0;
	SetGravity(GameWorld, 1.0, 180.0);

	GameWorld->PhysicsType = PLATFORMER;

	GameWorld->GameState = EMPTY_GAME;

	printf("Initialised World\n");
	fflush(stdout);

	return LEMON_SUCCESS;
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
	inputCam->CameraLatch = 0;
	inputCam->CameraXBuffer = 0;
	inputCam->CameraYBuffer = 0;
	inputCam->CameraMode = FOLLOW_PLAYER;


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
	
	return LEMON_SUCCESS;
}


int SetRenderSettingsToDefault(void)
{
	RenderSettings.drawSprites = 1;
	RenderSettings.drawHitboxes = 0;
	RenderSettings.drawBackGround = 1;
	RenderSettings.drawObjects = 1;
	RenderSettings.drawParticles = 1;
	RenderSettings.drawUI = 1;
	RenderSettings.drawPlayer = 1;

	RenderSettings.maxObjects = MAX_OBJECTS_RENDER;
	RenderSettings.maxParticles = MAX_PARTICLES_RENDER;
	RenderSettings.maxUIElements = MAX_HUD_ELEMENTS_RENDER;
	
	return LEMON_SUCCESS;
}


int SetDebugSettingsToDefault(void)
{
	DebugSettings.HitboxOutlineThickness = 4;
	DebugSettings.DisplayPlayerData = 0;
	DebugSettings.DebugTextEnabled = ONLY_ERRORS;
	memset(DebugSettings.DebugString, 0, DEBUG_STRING_LENGTH);

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
	newController->cachedCount = 0;

	newController->FrameUpdates = NULL;

	if (EngineSettings.ObjectPreAllocationEnabled == 1)
	{
		int i = 0;

		while (i < EngineSettings.MaxObjects)
		{
			createNewObject(0, newController);
			i++;
		}

		newController->lastObject = NULL;
	}

	return newController;
}


void clearGameData(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return;
	}

	PlayerData *Player = GameWorld->Player;
	
	clearTextQueue(GameWorld);

	deleteAllObjects(GameWorld->ObjectList);

	SpriteSet *currentSet;
	currentSet = GameWorld->ObjectList->startSpriteSetPtr;

	SpriteSet *prevSet;
	prevSet = NULL;

	Sprite *currentSprite;

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

	GameWorld->GameState = EMPTY_GAME;


	if (Player == NULL)
	{
		return;
	}


	free(Player->InteractBox);
	free(Player);


	return;
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

	GameWorld->GlobalGravityX = force * sinVal;
	GameWorld->GlobalGravityY = force * cosVal;

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
	loadSprite("BG_Water", "Backgrounds", &WorldBackground->BackgroundSpriteSet, 1, TILE);
	loadSprite("BG_GreenHill", "Backgrounds", &WorldBackground->BackgroundSpriteSet, 1, SINGLE);

	return 0;
}


int ClearKeyboardInput()
{
	if (keyboard == NULL)
	{
		return MISSING_DATA;
	}

	keyboard[LMN_INTERACT] = 0;
	keyboard[LMN_INTERACT2] = 0;
	keyboard[LMN_INTERACT3] = 0;
	keyboard[LMN_JUMP] = 0;
	keyboard[LMN_LEFT] = 0;
	keyboard[LMN_RIGHT] = 0;
	keyboard[LMN_UP] = 0;
	keyboard[LMN_DOWN] = 0;
	keyboard[LMN_ESCAPE] = 0;
	keyboard[LMN_SPACE] = 0;
	keyboard[LMN_ENTER] = 0;


	return 0;
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


int modulo(int x, int N)
{
    return (x % N + N) % N;
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