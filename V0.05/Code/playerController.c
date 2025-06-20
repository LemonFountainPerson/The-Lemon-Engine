#include "PlayerController.h"


PlayerData* InitialisePlayer(World *gameWorld)
{
	PlayerData *Player = malloc(sizeof(PlayerData));

	if (Player == NULL)
	{
		printf("Error: Could not allocate space for Player.\n");
		fflush(stdout);
		return NULL;
	}

	Player->PlayerBox = createPhysicsRect(ENTITY);

	if (Player->PlayerBox == NULL)
	{
		printf("Error: Could not allocate space for Player Physics Box.\n");
		free(Player);
		return NULL;
	}


	Player->InteractBox = createPhysicsRect(UNSOLID);

	if (Player->InteractBox == NULL)
	{
		printf("Error: Could not allocate space for Player Interact hitbox.\n");
		free(Player);
		return NULL;
	}
	

	if (gameWorld != NULL)
	{
		gameWorld->Player = Player;
	}

	// Player set-up
	Player->PlayerBox->xPos = 100.0;
	Player->PlayerBox->yPos = 100.0;
	Player->PlayerBox->xSize = PLAYERWIDTH;
	Player->PlayerBox->ySize = PLAYERHEIGHT;
	Player->PlayerBox->xPosRight = 100 + PLAYERWIDTH;
	Player->PlayerBox->yPosTop = 100 + PLAYERHEIGHT;
	Player->PlayerBox->spriteYOffset = 14;

	Player->maxXVel = 15.0;
	Player->maxYVel = 32.0;
	Player->PlayerBox->PhysicsXVelocity = 0.0;
	Player->PlayerBox->PhysicsYVelocity = 0.0;

	Player->inAir = 0;
	Player->jumpProgress = 0;
	Player->jumpHeld = 0;
	Player->PlayerBox->crouch = 0;
	Player->coinCount = 0;
	Player->HP = 100;
	Player->PlayerLayer = MIDDLEGROUND;
	Player->PlayerState = DEFAULT;


	Player->PlayerDisplay = createDisplayData(DEFAULT_TO_SPRITE);

	if (Player->PlayerDisplay == NULL)
	{
		printf("Error: Could not allocate space for Player display data.\n");
		free(Player->PlayerDisplay);
		return Player;
	}


	LoadPlayerSprites(Player->PlayerDisplay);

	printf("Initialised Player\n");
	return Player;
}


int LoadPlayerSprites(DisplayData *PlayerDisplay)
{
	if (PlayerDisplay == NULL)
	{
		return MISSING_DATA;
	}

	loadSprite("Idle_R.png", "Player", &PlayerDisplay->spriteSetSource, 1, SINGLE);
	loadSprite("Spin_R.png", "Player", &PlayerDisplay->spriteSetSource, 1, SINGLE);

	return 0;
}


int ResetPlayer(PlayerData *Player)
{
	if (Player == NULL)
	{
		return MISSING_DATA;
	}

	Player->PlayerBox->xVelocity = 0.0;
	Player->PlayerBox->yVelocity = 0.0;
	Player->PlayerBox->PhysicsXVelocity = 0.0;
	Player->PlayerBox->PhysicsYVelocity = 0.0;
	
	switchPlayerSprite(2, 1, Player->PlayerDisplay);

	Player->jumpProgress = 0;
	Player->inAir = 0;
	Player->jumpHeld = 0;


	return 0;
}


FunctionResult UpdatePlayer(PlayerData *Player, World *gameWorld, int keyboard[256])
{
	if (Player == NULL || gameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (gameWorld->GamePaused > 0 || gameWorld->GameState == EMPTY_GAME || gameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}

	if (gameWorld->GameState == CUTSCENE && Player->PlayerState != CUTSCENE_ACTOR)
	{
		Player->PlayerBox->xVelocity = 0.0;
		Player->PlayerBox->yVelocity = 0.0;
		return ACTION_DISABLED;
	}

	int hAxis = 0;
	int vAxis = 0;
	int jump = 0;


	// Player input
	hAxis = keyboard[LMN_RIGHT] - keyboard[LMN_LEFT];

	vAxis = keyboard[LMN_UP] - keyboard[LMN_DOWN];

	if (keyboard[LMN_JUMP])
	{
		jump = 1;
	}
	else
	{
		Player->jumpHeld = 0;
	}

	if (Player->PlayerState == PAUSE_BEHAVIOUR)
	{
		jump = 0;
		hAxis = 0;
		vAxis = 0;
	}


	// Debug
	if (keyboard['M'] == 1)
	{
		Player->PlayerBox->yVelocity = 16.0;
	}


	if (hAxis != 0)
	{
		Player->PlayerBox->xFlip = hAxis;
	}

	if (vAxis < 0 && Player->inAir == 0)
	{
		Player->PlayerBox->crouch = 1;
		Player->PlayerBox->yVelocity -= 1.0;
	}

	if (vAxis > -1 && Player->inAir == 0)
	{
		Player->PlayerBox->crouch = 0;
	}

	Player->PlayerBox->yPos += 6.0;
	int againstCeiling = (GetCollidingObject(Player->PlayerBox, gameWorld) != NULL);
	Player->PlayerBox->yPos -= 6.0;

	if (againstCeiling == 0 && jump == 1 && ((Player->inAir < 6 && Player->jumpHeld == 0) || (Player->PlayerBox->yVelocity > 12.0 && Player->jumpProgress > 0) ))
	{
		PlayerJump(Player, hAxis, vAxis);
	}

	if (Player->jumpProgress < 10 && jump == 0 && Player->PlayerBox->yVelocity > 12.0 && Player->jumpProgress > 0)
	{
		Player->PlayerBox->yVelocity = 12.0;
		Player->jumpProgress = 99;
	}


	// Movement velocity acceleration/decceleration
	if (hAxis == 0)
	{
		Player->PlayerBox->xVelocity *= 0.8;
	}

	if (Player->inAir > 0)
	{
		Player->PlayerBox->xVelocity += hAxis * 0.75;
		Player->PlayerBox->xVelocity *= 0.951;

		if (hAxis != 0)
		{
			Player->PlayerBox->PhysicsXVelocity = 0.0;
		}

		if (fabs(Player->PlayerBox->PhysicsXVelocity) > 0.1)
		{
			Player->PlayerBox->xVelocity = Player->PlayerBox->PhysicsXVelocity;
		}
	}
	else
	{
		Player->PlayerBox->xVelocity += hAxis;
		Player->PlayerBox->xVelocity *= 0.935;
	}


	if (fabs(Player->PlayerBox->xVelocity) < 0.1)
	{
		Player->PlayerBox->xVelocity = 0.0;
	}


	if (fabs(Player->PlayerBox->yVelocity) < 0.1)
	{
		Player->PlayerBox->yVelocity = 0.0;
	}


	// Gravity
	Player->PlayerBox->yVelocity += gameWorld->Gravity;

	if (fabs(Player->PlayerBox->xVelocity) > Player->maxXVel)
	{
		Player->PlayerBox->xVelocity = (Player->PlayerBox->xVelocity/fabs(Player->PlayerBox->xVelocity)) * (Player->maxXVel);
	}

	if (fabs(Player->PlayerBox->yVelocity) > Player->maxYVel)
	{
		Player->PlayerBox->yVelocity = (Player->PlayerBox->yVelocity/fabs(Player->PlayerBox->yVelocity)) * (Player->maxYVel);
	}


	// collision detection
	MovePlayerX(Player, gameWorld);

	MovePlayerY(Player, gameWorld);
	


	Player->PlayerBox->xPosRight = Player->PlayerBox->xPos + Player->PlayerBox->xSize;
	Player->PlayerBox->yPosTop = Player->PlayerBox->yPos + Player->PlayerBox->ySize;

	Player->InteractBox->xPosRight = Player->InteractBox->xPos + Player->InteractBox->xSize;
	Player->InteractBox->yPosTop = Player->InteractBox->yPos + Player->InteractBox->ySize;


	int onGround = checkIfGrounded(gameWorld, Player);

	if (onGround == 0 && Player->inAir < 100)
	{
		Player->inAir++;
	}

	if (onGround > 0)
	{
		Player->inAir = 0;
	}


	setPlayerSprite(Player);

	if (Player->PlayerBox->yPos < -60.0 || Player->PlayerBox->yPos > 60000.0)
	{
		Player->PlayerBox->yPos = 150.0;
		Player->PlayerBox->yVelocity = 10.0;
	}

	if (Player->PlayerBox->xPos < 0.0)
	{
		Player->PlayerBox->xPos = 0.0;
		Player->PlayerBox->xVelocity = 0.0;
	}

	HandlePlayerInteract(Player, keyboard);

	return 0;
}


int PlayerJump(PlayerData *Player, int hAxis, int vAxis)
{
	// Jump handling - Player jump must be less than 0, and third condition ensures that the
	// Player must be moving upwards or not be holding the button from previous jump to continue jumping/jump again
	// Jumping is set up this way both for variable jumping heights and coyote frames

	if (Player->jumpProgress < 100)
	{
		Player->jumpProgress++;
	}

	if (Player->jumpProgress > 9)
	{
		return 0;
	}

	Player->PlayerBox->yVelocity = 16.0;

	if (Player->jumpHeld == 0)
	{
		LemonPlaySound("Jump", "Player", PLAYER_SFX, 1.0);
				
		if (hAxis == Player->PlayerBox->xVelocity/fabs(Player->PlayerBox->xVelocity) && fabs(Player->PlayerBox->PhysicsXVelocity) < fabs(Player->PlayerBox->xVelocity))
		{
			Player->PlayerBox->xVelocity += Player->PlayerBox->PhysicsXVelocity;
		}
	}

	Player->jumpHeld = 1;

	return 0;
}


int HandlePlayerInteract(PlayerData *Player, int keyboard[])
{
	if (Player == NULL)
	{
		return MISSING_DATA;
	}

	if (keyboard[LMN_INTERACT] == 1)
	{
		Player->InteractBox->xSize = X_TILESCALE;
		Player->InteractBox->ySize = Y_TILESCALE;
		Player->InteractBox->xPos = Player->PlayerBox->xPos + (Player->PlayerBox->xSize>>1) - (Player->InteractBox->xSize>>1) + (Player->PlayerBox->xFlip * 8);
		Player->InteractBox->yPos = Player->PlayerBox->yPos + (Player->PlayerBox->ySize>>1) - (Player->InteractBox->ySize>>1);
		keyboard[LMN_INTERACT] = 0;
	}

	if (Player->InteractBox->xSize > 0 && Player->InteractBox->ySize > 0)
	{
		Player->InteractBox->xPos = Player->PlayerBox->xPos + (Player->PlayerBox->xSize>>1) - (Player->InteractBox->xSize>>1) + (Player->PlayerBox->xFlip * 8);
		Player->InteractBox->yPos = Player->PlayerBox->yPos + (Player->PlayerBox->ySize>>1) - (Player->InteractBox->ySize>>1);
		Player->InteractBox->xSize -= 2;
		Player->InteractBox->ySize -= 2;
	}

	return 0;
}


int setPlayerSprite(PlayerData *Player)
{
	if (Player == NULL || Player->PlayerDisplay == NULL)
	{
		return MISSING_DATA;
	}

	DisplayData *PlayerDisplay = Player->PlayerDisplay;
	int prevSprite = PlayerDisplay->currentSprite;

	if (Player->jumpProgress > 0)
	{
		PlayerDisplay->currentSprite = 2;
	}

	if (Player->inAir == 0)
	{
		PlayerDisplay->currentSprite = 1;
	}

	if (prevSprite != PlayerDisplay->currentSprite)
	{
		switchSprite(PlayerDisplay->currentSprite, 1, PlayerDisplay);
	}

	return 0;
}


int MovePlayerX(PlayerData *Player, World *GameWorld)
{
	PhysicsRect *PlayerBox = Player->PlayerBox;
	double prevXPos = PlayerBox->xPos;
	double prevYPos = PlayerBox->yPos;

	double sinVal = sin(PlayerBox->direction);
	double cosVal = cos(PlayerBox->direction);

	if (fabs(sinVal) > 0.001)
	{
		PlayerBox->xPos += PlayerBox->xVelocity * sinVal;
	}

	if (fabs(cosVal) > 0.001)
	{
		PlayerBox->xPos += PlayerBox->yVelocity * cosVal;
	}

	ResolveXCollision(PlayerBox, prevXPos, GameWorld);

	PlayerBox->yPos = prevYPos;

	return 0;
}


int MovePlayerY(PlayerData *Player, World *GameWorld)
{
	PhysicsRect *PlayerBox = Player->PlayerBox;

	double prevXPos = PlayerBox->xPos;
	double prevYPos = PlayerBox->yPos;

	double sinVal = sin(PlayerBox->direction);
	double cosVal = cos(PlayerBox->direction);

	if (fabs(sinVal) > 0.001)
	{
		PlayerBox->yPos += PlayerBox->yVelocity * sinVal;
	}

	if (fabs(cosVal) > 0.001)
	{
		PlayerBox->yPos += PlayerBox->xVelocity * cosVal;
	}

	
	int result = ResolveYCollision(PlayerBox, prevYPos, GameWorld, &Player->jumpProgress);

	PlayerBox->xPos = prevXPos;

	return 0;
}


int checkIfGrounded(World *gameWorld, PlayerData *Player)
{
	Object *detectedObject;
	detectedObject = gameWorld->ObjectList->firstObject;

	int i = 0;
	int result = 0;

	int prevYSize = Player->PlayerBox->ySize;
	Player->PlayerBox->ySize = 8;
	Player->PlayerBox->yPos -= 2.0;

	detectedObject = GetCollidingObject(Player->PlayerBox, gameWorld);

	Player->PlayerBox->yPos += 2.0;
	Player->PlayerBox->ySize = prevYSize;


	if (detectedObject == NULL)
	{
		Player->PlayerBox->direction = RADIAN_90;

		return 0;
	}

	if (Player->inAir > 0 && fabs(Player->PlayerBox->PhysicsXVelocity) > 0.1)
	{
		Player->PlayerBox->xVelocity = 0.0;
	}

	Player->PlayerBox->PhysicsXVelocity = detectedObject->ObjectBox->xVelocity;
	Player->PlayerBox->PhysicsYVelocity = detectedObject->ObjectBox->yVelocity;

	AssignDirection(Player->PlayerBox, detectedObject);


	return 1;
}


int switchPlayerSprite(int spriteID, int spriteSet, DisplayData *PlayerDisplay)
{
	if (PlayerDisplay == NULL || PlayerDisplay->spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}

	SpriteSet *currentSet = PlayerDisplay->spriteSetSource;

	if (currentSet->setID == spriteSet && PlayerDisplay->spriteBuffer != NULL && PlayerDisplay->spriteBuffer->spriteID == spriteID)
	{
		return EXECUTION_UNNECESSARY;
	}

	// Find correct Player sprite set
	if (spriteSet > currentSet->setID)
	{
		while (currentSet->nextSet != NULL && currentSet->setID != spriteSet)
		{
			currentSet = currentSet->nextSet;
		}

	}
	else
	{
		while (currentSet->prevSet != NULL && currentSet->setID != spriteSet)
		{
			currentSet = currentSet->prevSet;
		}
	}


	if (currentSet->setID != spriteSet)
	{
		printf("Couldn't find sprite set %d\n", spriteSet);
		return -1;
	}

	// Find correct sprite from sprite set
	Sprite *currentSprite;
	int i;

	if (spriteID > currentSet->spriteCount >> 1)
	{
		// Start from end
		currentSprite = currentSet->lastSprite;
		i = currentSet->spriteCount;

		while (i > 0 && currentSprite != NULL && currentSprite->spriteID != spriteID)
		{
			currentSprite = currentSprite->prevSprite;
			i--;
		}

	}
	else
	{
		// Start from front
		currentSprite = currentSet->firstSprite;
		i = 1;

		while (i < currentSet->spriteCount && currentSprite != NULL && currentSprite->spriteID != spriteID)
		{
			currentSprite = currentSprite->nextSprite;
			i++;
		}

	}

	if (currentSprite == NULL || currentSprite->spriteID != spriteID)
	{
		printf("Could not find sprite %d for Player\n", spriteID);
		return MISSING_DATA;
	}

	PlayerDisplay->spriteBuffer = currentSprite;
	PlayerDisplay->currentSprite = spriteID;
	PlayerDisplay->spriteSetSource = currentSet;

	return 0;
}


int switchPlayerSpriteName(char spriteName[MAX_LEN], int spriteSet, DisplayData *PlayerDisplay)
{
	if (PlayerDisplay == NULL || PlayerDisplay->spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}

	SpriteSet *currentSet = PlayerDisplay->spriteSetSource;

	if (currentSet->setID == spriteSet && PlayerDisplay->spriteBuffer != NULL && strcmp(PlayerDisplay->spriteBuffer->spriteName, spriteName) == 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	// Find correct Player sprite set
	if (currentSet->setID < spriteSet)
	{
		while (currentSet->nextSet != NULL && currentSet->setID != spriteSet)
		{
			currentSet = currentSet->nextSet;
		}
	}
	else
	{
		while (currentSet->prevSet != NULL && currentSet->setID != spriteSet)
		{
			currentSet = currentSet->prevSet;
		}
	}

	if (currentSet->setID != spriteSet)
	{
		return MISSING_DATA;
	}

	// Find correct sprite from sprite set
	Sprite *currentSprite;
	currentSprite = currentSet->firstSprite;
	int i = 1;

	while (i < currentSet->spriteCount && currentSprite != NULL && strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		currentSprite = currentSprite->nextSprite;
		i++;
	}

	
	if (currentSprite == NULL || strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		printf("Could not find sprite '%s' for Player\n", spriteName);
		return MISSING_DATA;
	}

	PlayerDisplay->spriteBuffer = currentSprite;
	PlayerDisplay->currentSprite = currentSprite->spriteID;
	PlayerDisplay->spriteSetSource = currentSet;

	return 0;
}

