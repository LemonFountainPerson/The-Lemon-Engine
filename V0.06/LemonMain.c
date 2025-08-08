#include "LemonMain.h"

int gameRunning = 1;

SoundChannel SoundChannels[CHANNEL_COUNT];

int screenWidth = H_RESOLUTION;

int screenHeight = V_RESOLUTION;

int frameThrottle = 0;

int enableDebugText = 0;


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
	SDL_Texture *Texture;

	Window = SDL_CreateWindow("Lemon Engine", screenWidth - 1, screenHeight - 1, SDL_WINDOW_BORDERLESS);

	Renderer = SDL_CreateRenderer(Window, NULL);
	  
	Texture = SDL_CreateTexture(Renderer, 
			      SDL_PIXELFORMAT_ARGB8888, 
			      SDL_TEXTUREACCESS_STREAMING, 
			      screenWidth, 
			      screenHeight);


	// Check that the window was successfully created
    if (Window == NULL || Texture == NULL || Renderer == NULL) 
	{
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }


	initialiseAudio();

   
	// Core engine variables/data
    int keyboard[256] = {0};

	clock_t gameTick = clock();
    clock_t currentFrameTime = 0;
	clock_t lastFrameTime = 0;
	clock_t lastSecond = clock();

    int windowsFrames = 0;


	uint32_t *screenBuffer = malloc(sizeof(uint32_t) * screenHeight * screenWidth);

	if (screenBuffer == NULL)
	{
		printf("Unable to allocate memory for game frame!\n");
		return LEMON_ERROR;
	}


    // Game initialisation
    World *GameWorld;

    GameWorld = InitialiseGame();

	StartGame(GameWorld);

	
    // Game Loop
    while(gameRunning == 1)
    {
    	// Window messages
        SDL_Event event;

        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_EVENT_QUIT) 
            {
                gameRunning = 0;
            }

			getKeyboardInput(&event, keyboard);
		}


		// Timing
		currentFrameTime = clock() - lastFrameTime;
		
	    lastFrameTime = clock();

        gameTick += currentFrameTime;


	    // World updates
        if (gameTick >= TICK_DELTA)
        {
			GameTick(GameWorld, keyboard);

            gameTick = gameTick % TICK_DELTA;
        }

        HandleGameWorldEvents(GameWorld, keyboard, &screenBuffer);


		// Render screen
		RenderSDL(GameWorld, screenBuffer, Window, Renderer, Texture);

		// Process sound
		IterateAudio();


		// Framerate control
		if (frameThrottle == 1)
		{
			frameRate(60, gameTick);
       
		}
    	
      	if (((double)(clock() - lastSecond) / (double)CLOCKS_PER_SEC) > 0.99)
	    {
            if (GameWorld->GamePaused == 0)
            {
                printf("\n%d at %lf", windowsFrames, ((double)(clock() - lastSecond) / (double)CLOCKS_PER_SEC) );
            }
           
   		    windowsFrames = 0;
   		    lastSecond = clock();
	    }
	   
   		windowsFrames++;


		if (GameWorld->GameState == CLOSE_GAME)
        {
            gameRunning = 0;
        }
    }

	// Clear game data and cleanup
	clearGameData(GameWorld, screenBuffer);

	free(GameWorld);

	cleanUpAudioData();

	cleanUpSDLRenderer(Window, Renderer, Texture);

    SDL_Quit();


	return 0;
}


int GameTick(World *GameWorld, int keyboard[256])
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}


	MasterControls(GameWorld, keyboard, GameWorld->Player);


	UpdateCutscene(GameWorld, keyboard);

	updateObjects(GameWorld, keyboard);

	UpdatePlayer(GameWorld->Player, GameWorld, keyboard);

	WorldCameraControl(GameWorld, &GameWorld->MainCamera);

	updateText(GameWorld, keyboard);

	return 0;
}


int RenderSDL(World *GameWorld, uint32_t *screenBuffer, SDL_Window *Window, SDL_Renderer *Renderer, SDL_Texture *Texture)
{
	if (GameWorld == NULL || screenBuffer == NULL || Window == NULL || Renderer == NULL || Texture == NULL)
	{
		return MISSING_DATA;
	}

	int pitch = screenWidth * sizeof(uint32_t);


	SDL_LockTexture(Texture, NULL, (void **)&screenBuffer, &pitch);

	RenderEngine(GameWorld, screenBuffer);

	SDL_UnlockTexture(Texture);


	SDL_RenderClear(Renderer);
  	SDL_RenderTextureRotated(Renderer, Texture, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);
  	SDL_RenderPresent(Renderer);


	return LEMON_SUCCESS;
}


int RenderEngine(World *GameWorld, uint32_t *screenBuffer)
{
	if (GameWorld == NULL || screenBuffer == NULL)
	{
		return MISSING_DATA;
	}

	GameWorld->drawnObjects = 0;
	GameWorld->drawnParticles = 0;
	GameWorld->drawnHudElements = 0;
	
	renderBackGroundSprite(screenBuffer, GameWorld->MainCamera, GameWorld);

	drawObjects(screenBuffer, GameWorld->MainCamera, GameWorld);

	return 0;
}


// SDL Functions
int cleanUpSDLRenderer(SDL_Window *Window, SDL_Renderer *Renderer, SDL_Texture *Texture)
{
	// Close Renderer/Window
	SDL_DestroyTexture(Texture);

	SDL_DestroyRenderer(Renderer);

    SDL_DestroyWindow(Window);

	return 0;
}


int getKeyboardInput(SDL_Event *event, int keyboard[256])
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

	if (event->type == SDL_EVENT_KEY_UP)
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

			
	return 0;

}



void MasterControls(World *GameWorld, int keyboard[256], PlayerData *player)
{
	if (GameWorld == NULL || GameWorld->GameState == EMPTY_GAME)
	{
		return;
	}


	 // Debug Variables
	static int displayPlayerData = 0;
	static int frames = 0;


    if (keyboard['T'] == 1)
	{
    	keyboard['T'] = 2;
    	frameThrottle = (frameThrottle + 1) % 2;

    	putStringIntoDebug(GameWorld, "\nToggling framerate throttle: ");
    	putIntegerIntoDebug(GameWorld, frameThrottle);
	}


	if (keyboard['Y'] == 1)
	{
		//AddObject(GameWorld, UI_ELEMENT, 0, 0, 0, 0, FADEOUT, 0, 0, 0, 0);

		GameWorld->Player->PlayerPtr->Action = DAMAGED;
		GameWorld->Player->PlayerDisplay->invincibilityFrames = 200;

		enableDebugText = (enableDebugText + 1) % 2;

		keyboard['Y'] = 2;
	}


	if (keyboard['U'] == 1)
	{
		keyboard['U'] = 2;
		GameWorld->drawHitboxes = (GameWorld->drawHitboxes + 1) % 2;
		putStringIntoDebug(GameWorld, "\nToggling draw hitboxes: ");
    	putIntegerIntoDebug(GameWorld, GameWorld->drawHitboxes);
	}


	if (keyboard['I'] == 1)
	{
		keyboard['I'] = 2;
		GameWorld->drawBackGround = (GameWorld->drawBackGround + 1) % 2;
		putStringIntoDebug(GameWorld, "\nToggling draw background: ");
    	putIntegerIntoDebug(GameWorld, GameWorld->drawBackGround);
	}


	if (keyboard['O'] == 1)
	{
    	keyboard['O'] = 2;
    	GameWorld->drawSprites = (GameWorld->drawSprites + 1) % 2;
    	putStringIntoDebug(GameWorld, "\nToggling Draw Sprites: ");
    	putIntegerIntoDebug(GameWorld, GameWorld->drawSprites);
	}


	if (keyboard['P'] == 1)
	{
    	keyboard['P'] = 2;
    	displayPlayerData = (displayPlayerData + 1) % 2;
    	putStringIntoDebug(GameWorld, "\nToggling Player data display: ");
    	putIntegerIntoDebug(GameWorld, displayPlayerData);
	}


	if (keyboard['0'] == 1)
	{
    	GameWorld->GamePaused = (GameWorld->GamePaused + 1) % 2;
		keyboard['0'] = 2;
		putStringIntoDebug(GameWorld, "\nToggling Pause: ");
    	putIntegerIntoDebug(GameWorld, GameWorld->GamePaused);
    }


	if (player->PlayerBox != NULL && displayPlayerData == 1 && GameWorld->GamePaused == 0)
	{
    	printf("Player: X: %.2lf Y: %.2lf xVel: %lf yVel: %lf \nDirection: %lf forwardVelocity: %lf PhysicsX: %lf PhysicsY: %lf\n\n", 
			player->PlayerBox->xPos, player->PlayerBox->yPos, player->PlayerBox->xVelocity, player->PlayerBox->yVelocity, player->PlayerBox->direction, player->PlayerBox->forwardVelocity,
			player->PlayerBox->PhysicsXVelocity, player->PlayerBox->PhysicsYVelocity);
	}

	if (enableDebugText == 1 && GameWorld->debugString[0] > 9)
	{
		printf("\n---------------------------------------");
		printf("\nDebug Info:\n");
		printf(GameWorld->debugString);
		printf("\n---------------------------------------\n");
	}

	clearDebugString(GameWorld);

    return;
}


int clearDebugString(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	memset(GameWorld->debugString, 0, sizeof(char) * DEBUG_STRING_LENGTH);

	return 0;
}


int putStringIntoDebug(World *GameWorld, char input[])
{
	if (GameWorld == NULL || input[0] < 9)
	{
		return MISSING_DATA;
	}

	int stringIndex = 0;

	while (stringIndex < DEBUG_STRING_LENGTH && GameWorld->debugString[stringIndex] != 0)
	{
		stringIndex++;
	}

	if (stringIndex >= DEBUG_STRING_LENGTH)
	{
		return END_OF_FILE;
	}

	int stringLength = DEBUG_STRING_LENGTH - stringIndex;
	stringIndex = 0;

	while (stringIndex < stringLength && input[stringIndex] != 0)
	{
		stringIndex++;
	}


	if (stringIndex >= stringLength)
	{
		return INVALID_DATA;
	}

	strcat(GameWorld->debugString, input);

	return LEMON_SUCCESS;
}


int putIntegerIntoDebug(World *GameWorld, int input)
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}

	char buffer[30] = {0};

	convertIntToStr(buffer, input);

	return putStringIntoDebug(GameWorld, buffer);
}


void frameRate(int rate, clock_t gameTick)
{
	SDL_Delay((800.0/(float)rate) - ((double)gameTick / (double)CLOCKS_PER_SEC));
}


// Game functions
World* InitialiseGame()
{
	// Game world creation
	World *GameWorld = malloc(sizeof(World));

	if (GameWorld == NULL)
	{
		printf("\nError: Could not allocate space for game world.\n\n");
		fflush(stdout);
		return NULL;
	}

	clearDebugString(GameWorld);
	GameWorld->MainCamera.CameraX = 1000;
	GameWorld->MainCamera.minCameraX = 0;
	GameWorld->MainCamera.maxCameraX = 32768;
	GameWorld->MainCamera.CameraY = 120;
	GameWorld->MainCamera.minCameraY = 0;
	GameWorld->MainCamera.maxCameraY = 32768;
	GameWorld->MainCamera.CameraLatch = 0;
	GameWorld->MainCamera.CameraXBuffer = 0;
	GameWorld->MainCamera.CameraYBuffer = 0;
	GameWorld->MainCamera.CameraMode = FOLLOW_PLAYER;

	GameWorld->level = 0;
	GameWorld->GameEvent = NO_EVENT;
	GameWorld->CurrentCutscene = EMPTY_CUTSCENE;
	GameWorld->SceneTick = 0;
	GameWorld->PlayingText = 0;

	GameWorld->drawnObjects = 0;
	GameWorld->drawnParticles = 0;
	GameWorld->drawnHudElements = 0;
	GameWorld->drawHitboxes = 0;
	GameWorld->drawSprites = 1;
	GameWorld->drawPlayer = 1;
	GameWorld->drawBackGround = 1;
	GameWorld->drawParticles = 1;
	GameWorld->drawUI = 1;

	GameWorld->bgParallax = 0.1;
	GameWorld->bgRowParallax = 0.00006;
	GameWorld->ParallaxRowCutOff = 640;
	GameWorld->ParallaxRowStart = 1000;

	GameWorld->bgSpriteBuffer = NULL;

	GameWorld->GamePaused = 0;

	GameWorld->TextQueue = NULL;

	
	// Object controller creation
	ObjectController *objController = createObjectController();

	if (objController == NULL)
	{
		printf("\nError: Could not allocate space for Object Controller.\n\n");
		free(GameWorld);
		return NULL;
	}

	GameWorld->ObjectList = objController;

	GameWorld->BackGrounds = NULL;

	GameWorld->bgSpriteBuffer = NULL;

	srand(0);


	// Load backgrounds
	LoadBackGroundSprites(GameWorld);

	GameWorld->GameState = EMPTY_GAME;

	GameWorld->Player = InitialisePlayerData(GameWorld);

	GameWorld->Gravity = -1.0;
	GameWorld->PhysicsType = PLATFORMER;

	printf("Initialised World\n");
	fflush(stdout);

	return GameWorld;
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

	return newController;
}


void clearGameData(World *GameWorld, uint32_t screen[])
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return;
	}

	PlayerData *Player = GameWorld->Player;

	if (screen != NULL)
	{
		free(screen);
	}

	
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
	
	currentSet = GameWorld->BackGrounds;
	prevSet = NULL;
	
	while (currentSet != NULL)
	{
		prevSet = currentSet;
		currentSet = currentSet->nextSet;

		deleteSpriteSet(prevSet, NULL);
	}

	GameWorld->ObjectList = NULL;
	GameWorld->bgSpriteBuffer = NULL;
	GameWorld->BackGrounds = NULL;
	GameWorld->GameState = EMPTY_GAME;


	if (Player == NULL)
	{
		return;
	}


	free(Player->InteractBox);
	free(Player);

	return;
}


int LoadBackGroundSprites(World *GameWorld)
{
	loadSprite("BG_Water", "Backgrounds", &GameWorld->BackGrounds, 1, TILE);
	loadSprite("BG_GreenHill", "Backgrounds", &GameWorld->BackGrounds, 1, SINGLE);

	return 0;
}


int ClearKeyboardInput(int keyboard[])
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


int dClamp(double input, double lowerBound, double upperBound)
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