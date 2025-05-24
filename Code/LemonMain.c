#include "LemonMain.h"

static int frameThrottle = 1;

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
    destRect.x = H_RESOLUTION>>1;
    destRect.y = V_RESOLUTION>>1;
    destRect.w = H_RESOLUTION;
    destRect.h = V_RESOLUTION;

	SDL_CreateWindowAndRenderer("Lemon Engine", H_RESOLUTION, V_RESOLUTION, 0, &Window, &Renderer);

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
    clock_t lastTick = clock();
    clock_t lastSecond = clock();
	int frames = 0;


    RenderFrame frame = {0};
	frame.width = H_RESOLUTION;
	frame.height = V_RESOLUTION;
	frame.screen = malloc(sizeof(uint32_t) * frame.width * frame.height);

	if (frame.screen == NULL)
	{
		printf("Unable to allocate memory for game frame!\n");
		return -1;
	}


    // Game initialisation
    PlayerData *player;
    World *GameWorld;

    GameWorld = initialiseGame(NULL);
    player = initialisePlayer(GameWorld);


    // test data
	loadLevel(GameWorld, 1);

	Object *testObject = GameWorld->objectList->lastObject;

	
    LemonPlaySound("StartUp", "Music", MUSIC_CHANNEL, 0.8);

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
		gameTick = clock() - lastTick;
		deltaTime = ((double)(clock() - lastTick)) * 0.0625;
		lastTick = clock();
		deltaTime = 1.0;


		// World updates
		updateObjects(GameWorld, keyboard);


		// Player control
		updatePlayer(player, GameWorld, keyboard);


		worldCameraControl(frame.width, frame.height, player, GameWorld);


		// Render screen
		cleanRenderer(GameWorld, frame.screen, frame.width, frame.height);

		renderBackGroundSprite(frame.screen, frame.width, frame.height, GameWorld);

		drawObjects(BACKGROUND, frame.screen, frame.width, frame.height, GameWorld);

		drawObjects(MIDDLEGROUND, frame.screen, frame.width, frame.height, GameWorld);
		
		drawObjects(FOREGROUND, frame.screen, frame.width, frame.height, GameWorld);
		
		drawObjects(PARTICLES, frame.screen, frame.width, frame.height, GameWorld);


		PutScreenOnWindow(frame, Renderer, screenSurface, texture);
		

		// Process sound
		IterateAudio();
       

		// Framerate control
    	frameRate(60, gameTick);


		if (((double)(clock() - lastSecond) / (double)CLOCKS_PER_SEC) > 0.99)
		{
   			printf("%d at %lf\n", frames, ((double)(clock() - lastSecond) / (double)CLOCKS_PER_SEC) );
   			frames = 0;
   			lastSecond = clock();
		}
		else
		{
   			frames++;
		}


		DebugControls(GameWorld, player, keyboard, testObject);

		if (keyboard[LMN_ESCAPE] == 1)
		{
    		gameRunning = 0;
		}
    }

	// Clear game data and cleanup
	clearGameData(GameWorld, player);

	CleanUpAudioData();

	CleanUpWindowRenderer(Window, Renderer, screenSurface, texture);

    SDL_Quit();


	return 0;
}


// SDL Functions
int CleanUpWindowRenderer(SDL_Window *Window, SDL_Renderer *Renderer, SDL_Surface *screenSurface, SDL_Texture *texture)
{
	// Close Renderer/Window
	SDL_DestroySurface(screenSurface);

	SDL_DestroyTexture(texture);

	SDL_DestroyRenderer(Renderer);

    SDL_DestroyWindow(Window);

	return 0;
}


int PutScreenOnWindow(RenderFrame frame, SDL_Renderer *Renderer, SDL_Surface *screenSurface, SDL_Texture *texture)
{
	screenSurface = SDL_CreateSurfaceFrom(frame.width, frame.height, SDL_PIXELFORMAT_BGRA32, frame.screen, frame.width * sizeof(uint32_t));

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

	if (event->key.scancode > 3 && event->key.scancode < 30)
	{
		keyCode = 'A' + event->key.scancode - 4;
	}

	switch (event->key.scancode)
	{
		case 41:
			keyCode = LMN_ESCAPE;
			break;

		case 44:
			keyCode = LMN_SPACE;
			break;

		case 80:
			keyCode = LMN_LEFT;
			break;

		case 79:
			keyCode = LMN_RIGHT;
			break;

		case 81:
			keyCode = LMN_UP;
			break;

		case 82:
			keyCode = LMN_DOWN;
			break;

		default:
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
			
	return 0;

}



void DebugControls(World *GameWorld, PlayerData *player, int keyboard[256], Object *testObject)
{
	 // Debug Variables
	static int clickT = 0;
    static int clickY = 0;
    static int clickU = 0;
    static int clickI = 0;
    static int clickO = 0;
    static int clickP = 0;
	static int click0 = 0;
	static int displayPlayerData = 0;
	static int frames = 0;


    if (keyboard['T'] && clickT == 0)
	{
    	clickT = 1;
    	frameThrottle = (frameThrottle + 1) % 2;
    	printf("Toggling framerate throttle: %d\n", frameThrottle);
	}

	if (keyboard['T'] == 0)
	{
    	clickT = 0;
	}

	if (keyboard['Y'] && clickY == 0)
	{
		//clearGameData(gameWorld, player);
		//deleteObject(gameWorld->objectList, &gameWorld->objectList->startPtr);
		//switchLevel(gameWorld, 1);
    	testObject->layer = (testObject->layer + 1) % 4;
		SetDrawPriorityToBack(GameWorld->objectList, testObject);
		clickY = 1;
	}

	if (keyboard['Y'] == 0)
	{
		clickY = 0;
	}

	if (keyboard['U'] && clickU == 0)
	{
		clickU = 1;
		GameWorld->drawHitboxes = (GameWorld->drawHitboxes + 1) % 2;
		printf("Toggling draw Hitboxes:\n");
	}

	if (keyboard['U'] == 0)
	{
		clickU = 0;
	}

	if (keyboard['I'] && clickI == 0)
	{
		clickI = 1;
		GameWorld->drawBackGround = (GameWorld->drawBackGround + 1) % 2;
		printf("Toggling draw background:\n");
	}

	if (keyboard['I'] == 0)
	{
    	clickI = 0;
	}

	if (keyboard['O'] && clickO == 0)
	{
    	clickO = 1;
    	GameWorld->drawSprites = (GameWorld->drawSprites + 1) % 2;
    	printf("Toggling Draw Sprites:\n");
	}

	if (keyboard['O'] == 0)
	{
    	clickO = 0;
	}


	if (keyboard['P'] && clickP == 0)
	{
    	clickP = 1;
    	displayPlayerData = (displayPlayerData + 1 % 2);
    	printf("Toggling Player data display:\n");
	}

	if (keyboard['P'] == 0)
	{
		clickP = 0;
	}

	if (keyboard['0'] && click0 == 0)
	{
    	GameWorld->GamePaused = (GameWorld->GamePaused + 1) % 2;
		click0 = 1;
    	printf("Toggling Pause:\n");
	}

	if (keyboard['0'] == 0)
	{
		click0 = 0;
	}

	if (displayPlayerData == 1)
	{
    	printf("Player: X: %.2lf Y: %.2lf xVel: %lf yVel: %lf Direction: %lf\n", player->xPos, player->yPos, player->xVelocity, player->yVelocity, player->direction);
	}

    return;
}


// Game functions
World* initialiseGame(PlayerData *player)
{
	// Game world creation
	World *gameWorld = malloc(sizeof(World));

	if (gameWorld == NULL)
	{
		printf("\nError: Could not allocate space for game world.\n\n");
		fflush(stdout);
		return NULL;
	}

	gameWorld->Gravity = -1.0;
	gameWorld->cameraX = 0;
	gameWorld->cameraY = 120;
	gameWorld->level = 0;
	gameWorld->drawnObjects = 0;

	gameWorld->drawHitboxes = 0;
	gameWorld->drawSprites = 1;
	gameWorld->drawPlayer = 1;
	gameWorld->drawBackGround = 1;

	gameWorld->bgParallax = 0.1;
	gameWorld->bgTileVertically = 1;
	gameWorld->bgParallaxChunkSize = 64;
	gameWorld->bgChunkParallax = 0.00008;

	gameWorld->bgSpriteBuffer = NULL;
	gameWorld->playBgMusic = 1;

	gameWorld->GamePaused = 0;

	gameWorld->Player = player;

	// Object controller creation
	ObjectController *objController = malloc(sizeof(ObjectController));

	if (objController == NULL)
	{
		printf("\nError: Could not allocate space for Object Controller.\n\n");
		fflush(stdout);
		return NULL;
	}

	objController->lastObject = NULL;
	objController->firstObject = NULL;
	objController->objectCount = 0;
	objController->spriteSetCount = 0;
	objController->startSpriteSetPtr = NULL;

	gameWorld->objectList = objController;

	gameWorld->BackGrounds = NULL;

	gameWorld->bgSpriteBuffer = NULL;

	srand(0);


	// Load backgrounds
	loadBackGroundSprite("BG_Water", 1, 0, gameWorld);
	loadBackGroundSprite("BG_GreenHill", 1, 1, gameWorld);

	switchBackGroundSprite(1, 1, gameWorld);


	printf("Initialised World\n");
	fflush(stdout);

	return gameWorld;
}


void clearGameData(World *gameWorld, PlayerData *player)
{
	if (gameWorld == NULL || gameWorld->objectList == NULL)
	{
		return;
	}

	deleteAllObjects(gameWorld->objectList);


	SpriteSet *currentSet;
	currentSet = gameWorld->objectList->startSpriteSetPtr;

	SpriteSet *prevSet;
	prevSet = NULL;

	Sprite *currentSprite;

	while (currentSet != NULL)
	{
		currentSprite = currentSet->firstSprite;
		prevSet = currentSet;


		while (currentSprite != NULL)
		{
			deleteSprite(currentSet, &currentSprite);
		}
		
		currentSet = currentSet->nextSet;
		
		free(prevSet);
	}
	
	free(gameWorld->objectList);
	
	currentSet = gameWorld->BackGrounds;
	prevSet = NULL;
	
	while (currentSet != NULL)
	{
		currentSprite = currentSet->firstSprite;
		prevSet = currentSet;

		while (currentSprite != NULL && currentSet != NULL)
		{
			deleteSprite(currentSet, &currentSprite);
		}

		currentSet = currentSet->nextSet;

		free(prevSet);
	}

	gameWorld->objectList = NULL;
	gameWorld->bgSpriteBuffer = NULL;
	gameWorld->BackGrounds = NULL;


	if (player == NULL)
	{
		return;
	}

	currentSprite = player->spriteSetPtr->firstSprite;

	while (currentSprite != NULL)
	{
		deleteSprite(player->spriteSetPtr, &currentSprite);
	}

	free(player->spriteSetPtr);

	free(player);

	player = NULL;

	return;
}


void frameRate(int rate, clock_t gameTick)
{
	SDL_Delay((800.0/(double)rate) - ((double)gameTick / (double)CLOCKS_PER_SEC));
}



