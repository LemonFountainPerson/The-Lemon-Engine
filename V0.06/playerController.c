#include "PlayerController.h"


int InitialisePlayerObject(Object *Player, World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->Player == NULL || Player == NULL)
	{
		return MISSING_DATA;
	}

	Player->ObjectBox->xPos = 100.0;
	Player->ObjectBox->yPos = 100.0;
	Player->ObjectBox->xSize = PLAYERWIDTH;
	Player->ObjectBox->ySize = PLAYERHEIGHT;
	Player->ObjectBox->xPosRight = 100 + PLAYERWIDTH;
	Player->ObjectBox->yPosTop = 100 + PLAYERHEIGHT;
	Player->ObjectBox->solid = ENTITY;

	Player->ObjectDisplay->spriteYOffset = 14;

	GameWorld->Player->PlayerPtr = Player;
	GameWorld->Player->PlayerBox = Player->ObjectBox;
	GameWorld->Player->PlayerDisplay = Player->ObjectDisplay;
	GameWorld->Player->PlayerPtr->layer = MIDDLEGROUND_2;
	GameWorld->Player->PlayerPtr->State = DEFAULT;

	ResetPlayer(GameWorld->Player);


	return 0;
}


PlayerData* InitialisePlayerData(World *GameWorld)
{
	if (GameWorld == NULL)
	{
		return NULL;
	}


	PlayerData *Player = malloc(sizeof(PlayerData));

	if (Player == NULL)
	{
		printf("Error: Could not allocate space for Player Data.\n");
		fflush(stdout);
		return NULL;
	}


	Player->InteractBox = createPhysicsRect(UNSOLID);

	if (Player->InteractBox == NULL)
	{
		printf("Error: Could not allocate space for Player Interact hitbox.\n");
		free(Player);
		return NULL;
	}

	// Player set-up
	Player->maxXVel = 15.0;
	Player->maxYVel = 32.0;
	Player->inAir = 0;
	Player->jumpProgress = 0;
	Player->jumpHeld = 0;
	Player->coinCount = 0;
	Player->HP = 100;

	Player->PlayerBox = NULL;
	Player->PlayerDisplay = NULL;
	Player->PlayerPtr = NULL;


	printf("Initialised Player\n");
	return Player;
}


int LoadPlayerSprites(SpriteSet *inputSet)
{
	if (inputSet == NULL)
	{
		return MISSING_DATA;
	}

	loadSprite("Idle.png", "Player", &inputSet, PLAYER_OBJECT, SINGLE);
	loadSprite("Spin.png", "Player", &inputSet, PLAYER_OBJECT, SINGLE);

	return 0;
}


int ResetPlayer(PlayerData *Player)
{
	if (Player == NULL)
	{
		return MISSING_DATA;
	}

	Player->PlayerBox->forwardVelocity = 0.0;
	Player->PlayerBox->xVelocity = 0.0;
	Player->PlayerBox->yVelocity = 0.0;
	Player->PlayerBox->PhysicsXVelocity = 0.0;
	Player->PlayerBox->PhysicsYVelocity = 0.0;
	Player->PlayerBox->crouch = 0;
	
	switchSprite(1, PLAYER_OBJECT, Player->PlayerDisplay);

	Player->jumpProgress = 0;
	Player->inAir = 0;
	Player->jumpHeld = 0;


	return 0;
}


FunctionResult UpdatePlayer(PlayerData *Player, World *GameWorld, int keyboard[256])
{
	if (Player == NULL || Player->PlayerPtr == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GamePaused > 0 || GameWorld->GameState == EMPTY_GAME || GameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}

	if ((GameWorld->GameState != CUTSCENE || Player->PlayerPtr->State == ACTOR) && GameWorld->PlayingText == 0)
	{
		if (GameWorld->PhysicsType == PLATFORMER)
		{
			PlayerPlatformerPhysics(Player, GameWorld, keyboard);
		}
		else
		{
			PlayerTopDownPhysics(Player, GameWorld, keyboard);
		}
	}

	animatePlayer(Player);

	UpdateObjectDisplay(GameWorld, Player->PlayerPtr);

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

	return 0;
}


int PlayerPlatformerPhysics(PlayerData *Player, World *GameWorld, int keyboard[256])
{
	if (Player == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (Player->PlayerPtr->State == PAUSE_BEHAVIOUR)
	{
		return ACTION_DISABLED;
	}

	PhysicsRect *PlayerBox = Player->PlayerBox;


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


	// Debug
	if (keyboard['M'] == 1)
	{
		PlayerBox->yVelocity = 16.0;
	}


	if (hAxis != 0)
	{
		PlayerBox->xFlip = hAxis;
	}

	if (vAxis < 0 && Player->inAir == 0)
	{
		PlayerBox->crouch = 1;
		//PlayerBox->yVelocity -= 1.0;
	}

	if (vAxis > -1 && Player->inAir == 0)
	{
		PlayerBox->crouch = 0;
	}

	PlayerBox->yPos += 8.0;
	int againstCeiling = (GetCollidingObject(PlayerBox, GameWorld->ObjectList) != NULL);
	PlayerBox->yPos -= 8.0;

	if (againstCeiling == 0 && jump == 1 && ((Player->inAir < 6 && Player->jumpHeld == 0) || (PlayerBox->yVelocity > 12.0 && Player->jumpProgress > 0) ))
	{
		PlayerJump(Player, hAxis, vAxis);
	}

	if (Player->jumpProgress < 10 && jump == 0 && PlayerBox->yVelocity > 12.0 && Player->jumpProgress > 0)
	{
		PlayerBox->yVelocity = 12.0;
		Player->jumpProgress = 99;
	}


	// Movement velocity acceleration/decceleration
	if (hAxis == 0)
	{
		PlayerBox->forwardVelocity *= 0.8;
		PlayerBox->xVelocity *= 0.8;
	}

	if (Player->inAir > 0)
	{
		PlayerBox->forwardVelocity += hAxis * 0.75;
		PlayerBox->friction = 0.951;

		if (hAxis != 0)
		{
			PlayerBox->PhysicsXVelocity = 0.0;
		}

		if (fabs(PlayerBox->PhysicsXVelocity) > 0.1)
		{
			PlayerBox->xVelocity = PlayerBox->PhysicsXVelocity;
		}
	}
	else
	{
		PlayerBox->forwardVelocity += hAxis;
		PlayerBox->friction = 0.935;
	}


	if (fabs(PlayerBox->forwardVelocity) < 0.1)
	{
		PlayerBox->forwardVelocity = 0.0;
	}

	if (fabs(PlayerBox->xVelocity) < 0.1)
	{
		PlayerBox->xVelocity = 0.0;
	}


	if (fabs(PlayerBox->yVelocity) < 0.1)
	{
		PlayerBox->yVelocity = 0.0;
	}


	// Gravity
	PlayerBox->yVelocity += GameWorld->Gravity;

	if (fabs(PlayerBox->forwardVelocity) > Player->maxXVel)
	{
		PlayerBox->forwardVelocity = (PlayerBox->forwardVelocity/fabs(PlayerBox->forwardVelocity)) * (Player->maxXVel);
	}

	if (fabs(PlayerBox->yVelocity) > Player->maxYVel)
	{
		PlayerBox->yVelocity = (PlayerBox->yVelocity/fabs(PlayerBox->yVelocity)) * (Player->maxYVel);
	}


	// collision detection
	MoveForward(PlayerBox, GameWorld);

	moveObjectX(Player->PlayerPtr, GameWorld);

	moveObjectY(Player->PlayerPtr, GameWorld, &Player->jumpProgress);
	


	PlayerBox->xPosRight = PlayerBox->xPos + PlayerBox->xSize;
	PlayerBox->yPosTop = PlayerBox->yPos + PlayerBox->ySize;

	Player->InteractBox->xPosRight = Player->InteractBox->xPos + Player->InteractBox->xSize;
	Player->InteractBox->yPosTop = Player->InteractBox->yPos + Player->InteractBox->ySize;


	int onGround = checkIfGrounded(GameWorld, PlayerBox);

	if (Player->inAir > 0 && fabs(PlayerBox->PhysicsXVelocity) > 0.1)
	{
		PlayerBox->xVelocity = 0.0;
	}

	if (onGround == 0 && Player->inAir < 100)
	{
		Player->inAir++;
	}

	if (Player->inAir > 50 && Player->PlayerBox->crouch == 1)
	{
		Player->PlayerBox->crouch = 0;
	}

	if (onGround > 0)
	{
		Player->inAir = 0;
	}

	HandlePlayerInteract(GameWorld, keyboard);

	return 0;
}



int PlayerTopDownPhysics(PlayerData *Player, World *GameWorld, int keyboard[256])
{
	if (Player == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsRect *PlayerBox = Player->PlayerBox;

	if (Player->PlayerPtr->State == PAUSE_BEHAVIOUR)
	{
		return ACTION_DISABLED;
	}


	float acceleration = 0.92;
	float decceleration = 0.8;
	float speed = 0.88;

	int hAxis = 0;
	int vAxis = 0;


	// Player input
	hAxis = keyboard[LMN_RIGHT] - keyboard[LMN_LEFT];

	vAxis = keyboard[LMN_UP] - keyboard[LMN_DOWN];


	if (hAxis != 0)
	{
		PlayerBox->xFlip = hAxis;
	}


	if (hAxis != 0 || vAxis != 0)
	{
		PlayerBox->direction = fabs(atan2((double)hAxis, (double)vAxis));
	}

	// Movement velocity acceleration/decceleration
	PlayerBox->xVelocity += speed * hAxis * sin(PlayerBox->direction);
	

	PlayerBox->yVelocity += speed * abs(vAxis) * cos(PlayerBox->direction);
	

	PlayerBox->xVelocity *= acceleration;
	PlayerBox->yVelocity *= acceleration;

	if (hAxis == 0)
	{
		PlayerBox->xVelocity *= decceleration;
	}

	if (vAxis == 0)
	{
		PlayerBox->yVelocity *= decceleration;
	}



	if (fabs(PlayerBox->xVelocity) < 0.1)
	{
		PlayerBox->xVelocity = 0.0;
	}


	if (fabs(PlayerBox->yVelocity) < 0.1)
	{
		PlayerBox->yVelocity = 0.0;
	}


	if (fabs(PlayerBox->xVelocity) > Player->maxXVel)
	{
		PlayerBox->xVelocity = (PlayerBox->xVelocity/fabs(PlayerBox->xVelocity)) * (Player->maxXVel);
	}

	if (fabs(PlayerBox->yVelocity) > Player->maxYVel)
	{
		PlayerBox->yVelocity = (PlayerBox->yVelocity/fabs(PlayerBox->yVelocity)) * (Player->maxYVel);
	}


	// collision detection
	double prevXPos = PlayerBox->xPos;
	double prevYPos = PlayerBox->yPos;


	moveObjectX(Player->PlayerPtr, GameWorld);

	moveObjectY(Player->PlayerPtr, GameWorld, &Player->jumpProgress);


	Player->InteractBox->xPosRight = Player->InteractBox->xPos + Player->InteractBox->xSize;
	Player->InteractBox->yPosTop = Player->InteractBox->yPos + Player->InteractBox->ySize;

	HandlePlayerInteract(GameWorld, keyboard);

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

		PlayNewAnimation("StartJump", 1, Player->PlayerDisplay);
	}

	Player->jumpHeld = 1;

	return 0;
}


int HandlePlayerInteract(World *GameWorld, int keyboard[])
{
	if (GameWorld == NULL || GameWorld->Player == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->PlayingText == 1)
	{
		return ACTION_DISABLED;
	}

	PhysicsRect *InteractBox = GameWorld->Player->InteractBox;
	PhysicsRect *PlayerBox = GameWorld->Player->PlayerBox;

	if (keyboard[LMN_INTERACT] == 1)
	{
		InteractBox->xSize = 64;
		InteractBox->ySize = 64;
		InteractBox->xPos = PlayerBox->xPos + (PlayerBox->xSize>>1) - (InteractBox->xSize>>1) + (PlayerBox->xFlip << 4);
		InteractBox->yPos = PlayerBox->yPos + (PlayerBox->ySize>>1) - (InteractBox->ySize>>1);
		keyboard[LMN_INTERACT] = 0;
	}

	if (InteractBox->xSize > 0 && InteractBox->ySize > 0)
	{
		InteractBox->xPos = PlayerBox->xPos + (PlayerBox->xSize>>1) - (InteractBox->xSize>>1) + (PlayerBox->xFlip << 4);
		InteractBox->yPos = PlayerBox->yPos + (PlayerBox->ySize>>1) - (InteractBox->ySize>>1);
		InteractBox->xSize -= 2;
		InteractBox->ySize -= 2;
	}

	return 0;
}


int PlayerInteractingWithBox(PlayerData *Player, PhysicsRect *inputBox)
{
	if (Player == NULL || Player->InteractBox == NULL || inputBox == NULL)
	{
		return MISSING_DATA;
	}


	int result = checkBoxOverlapsBox(inputBox, Player->InteractBox);

	if (result == 1)
	{
		Player->InteractBox->xSize = 0;
		Player->InteractBox->ySize = 0;
	}

	return result;
}


int animatePlayer(PlayerData *Player)
{
	if (Player == NULL || Player->PlayerDisplay == NULL)
	{
		return MISSING_DATA;
	}

	DisplayData *PlayerDisplay = Player->PlayerDisplay;

	if (Player->jumpProgress > 0)
	{
		PlayAnimationAfterOther("JumpLoop", "StartJump", 0, PlayerDisplay);
	}

	if (Player->inAir == 0)
	{
		PlayNewAnimation("Stand", 0, PlayerDisplay);
	}


	return 0;
}


int checkIfGrounded(World *GameWorld, PhysicsRect *inputBox)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL || inputBox == NULL)
	{
		return 0;
	}

	Object *detectedObject;
	detectedObject = GameWorld->ObjectList->firstObject;

	int i = 0;
	int result = 0;


	inputBox->yPos += GameWorld->Gravity * 2;

	detectedObject = GetCollidingObject(inputBox, GameWorld->ObjectList);

	inputBox->yPos -= GameWorld->Gravity * 2;


	if (detectedObject == NULL)
	{
		inputBox->direction = RADIAN_90;

		return 0;
	}

	inputBox->PhysicsXVelocity = detectedObject->ObjectBox->xVelocity;
	inputBox->PhysicsYVelocity = detectedObject->ObjectBox->yVelocity;


	return 1;
}


int switchPlayerSprite(int spriteID, DisplayData *PlayerDisplay)
{
	if (PlayerDisplay == NULL || PlayerDisplay->spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}

	if (PlayerDisplay->spriteBuffer != NULL && PlayerDisplay->spriteBuffer->spriteID == spriteID)
	{
		return EXECUTION_UNNECESSARY;
	}

	SpriteSet *currentSet = PlayerDisplay->spriteSetSource;

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

	return 0;
}


int switchPlayerSpriteName(char spriteName[MAX_LEN], DisplayData *PlayerDisplay)
{
	if (PlayerDisplay == NULL || PlayerDisplay->spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}

	SpriteSet *currentSet = PlayerDisplay->spriteSetSource;

	if (PlayerDisplay->spriteBuffer != NULL && strcmp(PlayerDisplay->spriteBuffer->spriteName, spriteName) == 0)
	{
		return EXECUTION_UNNECESSARY;
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

	return 0;
}

