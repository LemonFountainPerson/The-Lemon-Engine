#include "LemonMain.h"

int gameRunning = 1;

int screenWidth = H_RESOLUTION;

int screenHeight = V_RESOLUTION;

int frameThrottle = 0;


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
	SDL_Surface *screenSurface = NULL;
	SDL_Renderer *Renderer = NULL;
    SDL_Texture* texture = NULL;
    SDL_FRect destRect;
    destRect.x = screenWidth>>1;
    destRect.y = screenHeight>>1;
    destRect.w = screenWidth;
    destRect.h = screenHeight;

	SDL_CreateWindowAndRenderer("Lemon Engine", screenWidth, screenHeight, 0, &Window, &Renderer);

	// Check that the window was successfully created
    if (Window == NULL || Renderer == NULL) 
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

            gameTick = 0;
        }


		// Render screen
		cleanRenderer(GameWorld, screenBuffer);

		renderBackGroundSprite(screenBuffer, GameWorld->MainCamera, GameWorld);

		drawObjects(screenBuffer, GameWorld->MainCamera, GameWorld);

		putScreenOnWindow(screenBuffer, Renderer, screenSurface, texture);
		

		// Process sound
		IterateAudio();
       

		MasterControls(GameWorld, keyboard, GameWorld->Player);

		// Framerate control
    	frameRate(60, gameTick);


		if (GameWorld->GameState == CLOSE_GAME)
        {
            gameRunning = 0;
        }
    }

	// Clear game data and cleanup
	clearGameData(GameWorld, screenBuffer);

	free(GameWorld);

	cleanUpAudioData();

	cleanUpWindowRenderer(Window, Renderer, screenSurface, texture);

    SDL_Quit();


	return 0;
}


int GameTick(World *GameWorld, int keyboard[256])
{
	if (GameWorld == NULL)
	{
		return MISSING_DATA;
	}


	UpdateCutscene(GameWorld, keyboard);

	updateObjects(GameWorld, keyboard);

	UpdatePlayer(GameWorld->Player, GameWorld, keyboard);

	WorldCameraControl(GameWorld, &GameWorld->MainCamera);

	UpdateText(GameWorld, keyboard);

	return 0;
}


// SDL Functions
int cleanUpWindowRenderer(SDL_Window *Window, SDL_Renderer *Renderer, SDL_Surface *screenSurface, SDL_Texture *texture)
{
	// Close Renderer/Window
	SDL_DestroySurface(screenSurface);

	SDL_DestroyTexture(texture);

	SDL_DestroyRenderer(Renderer);

    SDL_DestroyWindow(Window);

	return 0;
}


int putScreenOnWindow(uint32_t *screenBuffer, SDL_Renderer *Renderer, SDL_Surface *screenSurface, SDL_Texture *texture)
{
	screenSurface = SDL_CreateSurfaceFrom(screenWidth, screenHeight, SDL_PIXELFORMAT_BGRA32, screenBuffer, screenWidth * sizeof(uint32_t));

	if (!screenSurface)
	{
		printf("Couldnt load surface: %s\n", SDL_GetError());
	}

	SDL_FlipSurface(screenSurface, SDL_FLIP_VERTICAL);
	texture = SDL_CreateTextureFromSurface(Renderer, screenSurface);

	if (!texture)
	{
		printf("Couldnt load texture: %s\n", SDL_GetError());
	}

	SDL_SetRenderDrawColor(Renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(Renderer);

	SDL_RenderTexture(Renderer, texture, NULL, NULL);

	SDL_RenderPresent(Renderer);
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

		case 81:
			keyCode = LMN_UPARROW;
			break;

		case 82:
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
	if (GameWorld == NULL)
	{
		return;
	}


	if (keyboard[LMN_ESCAPE] == 1)
	{
		keyboard[LMN_ESCAPE] = 2;

		if (GameWorld->GamePaused == 0)
		{
			PauseGame(GameWorld, keyboard);
		}
		else
		{
			ResumeGame(GameWorld);
		}
	}


	 // Debug Variables
	static int displayPlayerData = 0;
	static int frames = 0;


    if (keyboard['T'] == 1)
	{
    	keyboard['T'] = 2;
    	frameThrottle = (frameThrottle + 1) % 2;
    	printf("Toggling framerate throttle: %d\n", frameThrottle);
	}


	if (keyboard['Y'] == 1)
	{
		//clearGameData(GameWorld, player);
		//deleteAllObjects(GameWorld->ObjectList);
		//loadLevel(GameWorld, 1);

		PhysicsRect cameraBox;
		cameraBox.xPos = GameWorld->MainCamera.CameraX - screenWidth;
		cameraBox.yPos = GameWorld->MainCamera.CameraY - screenHeight;
		cameraBox.xSize = screenWidth << 1;
		cameraBox.ySize = screenHeight << 1;

		//cacheObjects(GameWorld->ObjectList, &cameraBox, GameWorld->Player);

		printf("Object size: %d\n", sizeof(Object) + sizeof(PhysicsRect) + sizeof(DisplayData));

		keyboard['Y'] = 2;
	}


	if (keyboard['U'] == 1)
	{
		keyboard['U'] = 2;
		GameWorld->drawHitboxes = (GameWorld->drawHitboxes + 1) % 2;
		printf("Toggling draw Hitboxes:\n");
	}


	if (keyboard['I'] == 1)
	{
		keyboard['I'] = 2;
		GameWorld->drawBackGround = (GameWorld->drawBackGround + 1) % 2;
		printf("Toggling draw background:\n");
	}


	if (keyboard['O'] == 1)
	{
    	keyboard['O'] = 2;
    	GameWorld->drawSprites = (GameWorld->drawSprites + 1) % 2;
    	printf("Toggling Draw Sprites:\n");
	}


	if (keyboard['P'] == 1)
	{
    	keyboard['P'] = 2;
    	displayPlayerData = (displayPlayerData + 1) % 2;
    	printf("Toggling Player data display: %d\n", displayPlayerData);
	}


	if (keyboard['0'] == 1)
	{
    	GameWorld->GamePaused = (GameWorld->GamePaused + 1) % 2;
		keyboard['0'] = 2;
    	printf("Toggling Pause:\n");
	}


	if (player->PlayerBox != NULL && displayPlayerData == 1 && GameWorld->GamePaused == 0)
	{
    	printf("Player: X: %.2lf Y: %.2lf xVel: %lf yVel: %lf \nDirection: %lf forwardVelocity: %lf PhysicsX: %lf PhysicsY: %lf\n\n", 
			player->PlayerBox->xPos, player->PlayerBox->yPos, player->PlayerBox->xVelocity, player->PlayerBox->yVelocity, player->PlayerBox->direction, player->PlayerBox->forwardVelocity,
		player->PlayerBox->PhysicsXVelocity, player->PlayerBox->PhysicsYVelocity);
	}

    return;
}


void frameRate(int rate, clock_t gameTick)
{
	SDL_Delay((800.0/(float)rate) - ((double)gameTick / (double)CLOCKS_PER_SEC));
}


// Game functions
World* InitialiseGame()
{
	// Game world creation
	World *gameWorld = malloc(sizeof(World));

	if (gameWorld == NULL)
	{
		printf("\nError: Could not allocate space for game world.\n\n");
		fflush(stdout);
		return NULL;
	}

	gameWorld->MainCamera.CameraX = 1000;
	gameWorld->MainCamera.minCameraX = 0;
	gameWorld->MainCamera.maxCameraX = 32768;
	gameWorld->MainCamera.CameraY = 120;
	gameWorld->MainCamera.minCameraY = 0;
	gameWorld->MainCamera.maxCameraY = 32768;
	gameWorld->MainCamera.CameraLatch = 0;
	gameWorld->MainCamera.CameraXBuffer = 0;
	gameWorld->MainCamera.CameraYBuffer = 0;
	gameWorld->MainCamera.CameraMode = FOLLOW_PLAYER;

	gameWorld->level = 0;
	gameWorld->CurrentCutscene = EMPTY_CUTSCENE;
	gameWorld->SceneTick = 0;
	gameWorld->PlayingText = 0;

	gameWorld->drawnObjects = 0;
	gameWorld->drawnParticles = 0;
	gameWorld->drawnHudElements = 0;
	gameWorld->drawHitboxes = 0;
	gameWorld->drawSprites = 1;
	gameWorld->drawPlayer = 1;
	gameWorld->drawBackGround = 1;
	gameWorld->drawParticles = 1;
	gameWorld->drawUI = 1;

	gameWorld->bgParallax = 0.1;
	gameWorld->bgParallaxChunkSize = 64;
	gameWorld->bgChunkParallax = 0.00008;

	gameWorld->bgSpriteBuffer = NULL;

	gameWorld->GamePaused = 0;

	gameWorld->TextQueue = NULL;

	
	// Object controller creation
	ObjectController *objController = malloc(sizeof(ObjectController));

	if (objController == NULL)
	{
		printf("\nError: Could not allocate space for Object Controller.\n\n");
		free(gameWorld);
		return NULL;
	}

	objController->lastObject = NULL;
	objController->firstObject = NULL;
	objController->objectCount = 0;
	objController->spriteSetCount = 0;
	objController->startSpriteSetPtr = NULL;
	objController->cachedFirstObject = NULL;
	objController->cachedLastObject = NULL;
	objController->cachedCount = 0;

	gameWorld->ObjectList = objController;

	gameWorld->BackGrounds = NULL;

	gameWorld->bgSpriteBuffer = NULL;

	srand(0);


	// Load backgrounds
	LoadBackGroundSprites(gameWorld);

	gameWorld->GameState = EMPTY_GAME;

	gameWorld->Player = InitialisePlayerData(gameWorld);

	gameWorld->Gravity = -1.0;
	gameWorld->PhysicsType = PLATFORMER;

	printf("Initialised World\n");
	fflush(stdout);

	return gameWorld;
}


void clearGameData(World *gameWorld, uint32_t screen[])
{
	if (gameWorld == NULL || gameWorld->ObjectList == NULL)
	{
		return;
	}

	PlayerData *Player = gameWorld->Player;

	if (screen != NULL)
	{
		free(screen);
	}

	int i = 0;

	while (gameWorld->TextQueue != NULL && i < 500)
	{
		endTextInstance(gameWorld);
		i++;
	}


	deleteAllObjects(gameWorld->ObjectList);

	SpriteSet *currentSet;
	currentSet = gameWorld->ObjectList->startSpriteSetPtr;

	SpriteSet *prevSet;
	prevSet = NULL;

	Sprite *currentSprite;

	while (currentSet != NULL)
	{
		prevSet = currentSet;
		currentSet = currentSet->nextSet;

		deleteSpriteSet(prevSet);
	}
	
	free(gameWorld->ObjectList);
	
	currentSet = gameWorld->BackGrounds;
	prevSet = NULL;
	
	while (currentSet != NULL)
	{
		prevSet = currentSet;
		currentSet = currentSet->nextSet;

		deleteSpriteSet(prevSet);
	}

	gameWorld->ObjectList = NULL;
	gameWorld->bgSpriteBuffer = NULL;
	gameWorld->BackGrounds = NULL;
	gameWorld->GameState = EMPTY_GAME;


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


int modulo(int x, int N)
{
    return (x % N + N) % N;
}