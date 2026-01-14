#include "playerController.h"


int InitialisePlayerObject(Object *Player, World *GameWorld)
{
	if (GameWorld == NULL || Player == NULL || Player->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsRect *PlayerBox = Player->ObjectBox;

	PlayerBox->xSize = 32;
	PlayerBox->ySize = 50;
	PlayerBox->xPos = 0.0;
	PlayerBox->yPos = 100.0;
	PlayerBox->xPosRight = 100 + PlayerBox->xSize;
	PlayerBox->yPosTop = 100 + PlayerBox->ySize;
	PlayerBox->solid = ENTITY;

	if (GameWorld->Player.PlayerPtr != NULL)
	{
		strcpy(GameWorld->Player.PlayerPtr->name, "DisabledPlayer");
	}

	GameWorld->Player.PlayerPtr = Player;
	GameWorld->Player.PlayerBox = Player->ObjectBox;
	GameWorld->Player.PlayerDisplay = Player->ObjectDisplay;
	GameWorld->Player.PlayerPtr->layer = MIDDLEGROUND_2;
	GameWorld->Player.PlayerPtr->State = DEFAULT;

	strcpy(Player->name, "MainPlayer");

	ResetPlayer(&GameWorld->Player);

	return LEMON_SUCCESS;
}


PlayerData* InitialisePlayerData(PlayerData *Player)
{
	if (Player == NULL)
	{
		return NULL;
	}


	Player->InteractBox = createPhysicsRect(UNSOLID);

	if (Player->InteractBox == NULL)
	{
		putConsoleString("Error: Could not allocate space for Player Interact hitbox.\n");
		free(Player);
		return NULL;
	}

	// Player set-up
	Player->jumpProgress = 0;
	Player->jumpHeld = 0;
	Player->coinCount = 0;
	Player->HP = 100;

	Player->PlayerBox = NULL;
	Player->PlayerDisplay = NULL;
	Player->PlayerPtr = NULL;


	putConsoleString("Initialised Player\n");
	return Player;
}


int LoadPlayerSprites(SpriteSet *inputSet)
{
	if (inputSet == NULL)
	{
		return MISSING_DATA;
	}

	loadSpriteIntoDesiredSet("Idle.png", "Player", &inputSet, PLAYER_OBJECT, SINGLE);
	loadSpriteIntoDesiredSet("Spin.png", "Player", &inputSet, PLAYER_OBJECT, SINGLE);

	return LEMON_SUCCESS;
}


int ResetPlayer(PlayerData *Player)
{
	if (Player == NULL || Player->PlayerBox == NULL)
	{
		return MISSING_DATA;
	}

	Player->PlayerBox->forwardVelocity = 0.0;
	Player->PlayerBox->xVelocity = 0.0;
	Player->PlayerBox->yVelocity = 0.0;
	Player->PlayerBox->PhysicsXVelocity = 0.0;
	Player->PlayerBox->PhysicsYVelocity = 0.0;
	Player->PlayerBox->inAir = 0;
	Player->PlayerBox->crouch = false;
	
	stopAnimation(Player->PlayerDisplay);
	switchSprite(1, PLAYER_OBJECT, Player->PlayerDisplay);

	return LEMON_SUCCESS;
}


int PlayerObjectAboutToBeDeleted(PlayerData *Player)
{
	Player->PlayerBox = NULL;
	Player->PlayerPtr = NULL;
	Player->PlayerDisplay = NULL;

	return LEMON_SUCCESS;
}


FuncResult UpdatePlayer(PlayerData *Player, World *GameWorld)
{
	if (Player == NULL || Player->PlayerPtr == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}


	if (GameWorld->GamePaused > 0 || GameWorld->GameState == EMPTY_GAME || GameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}

	if (GameWorld->PhysicsType == PLATFORMER)
	{
		PlayerPlatformerPhysics(Player, GameWorld);
	}
	else
	{
		PlayerTopDownPhysics(Player, GameWorld);
	}

	animatePlayer(Player);

	return LEMON_SUCCESS;
}


int PlayerPlatformerPhysics(PlayerData *Player, World *GameWorld)
{
	if (Player == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	Object *PlayerObject = Player->PlayerPtr; 

	if (PlayerObject == NULL)
	{
		return MISSING_DATA;
	}

	if (PlayerObject->State == PAUSE_BEHAVIOUR || (GameWorld->GameState == CUTSCENE && PlayerObject->State != ACTOR) || GameWorld->PlayingText != 0)
	{
		ResetPlayer(Player);
		return ACTION_DISABLED;
	}

	PhysicsRect *PlayerBox = Player->PlayerBox;

	int hAxis = 0;
	int vAxis = 0;
	int jump = 0;


	// Player input
	hAxis = (keyboard[LMN_RIGHT] != 0) - (keyboard[LMN_LEFT] != 0);

	vAxis = (keyboard[LMN_UP] != 0) - (keyboard[LMN_DOWN] != 0);

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
		PlayerBox->yVelocity = 20.0;
	}


	if (hAxis != 0)
	{
		PlayerBox->xFlip = hAxis;
	}


	// Movement velocity acceleration/decceleration
	float forwardFriction = 1.0;
	float xFriction = 1.0;

	if (PlayerBox->inAir > 0)
	{
		PlayerBox->forwardVelocity += hAxis * 0.75;
		forwardFriction = 0.95;
	}
	else
	{
		PlayerBox->forwardVelocity += hAxis;
		forwardFriction = 0.93;

		if (hAxis == 0)
		{
			forwardFriction = 0.6;
			xFriction = 0.6;
		}
	}

	ApplyFriction(PlayerBox, forwardFriction, xFriction, 1.0);


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
	ApplyGravity(Player->PlayerPtr, GameWorld);

	if (PlayerBox->inAir < 1)
	{
		Player->jumpProgress = 0;
	}

	if (vAxis < 0 && PlayerBox->inAir < 1)
	{
		PlayerBox->crouch = true;
	}
	else if (PlayerBox->inAir < 1)
	{
		PlayerBox->crouch = false;
	}

	PlayerBox->yPos += 4.0;
	bool againstCeiling = (GetCollidingObject(PlayerBox, GameWorld->ObjectList) != NULL);
	PlayerBox->yPos -= 4.0;

	float jumpInterruptVel = 9.0;

	if (!againstCeiling && jump == 1 && ((PlayerBox->inAir < 6 && Player->jumpHeld == 0) || (PlayerBox->yVelocity > jumpInterruptVel && Player->jumpProgress > 0) ))
	{
		PlayerJump(Player, hAxis, vAxis);
	}

	if (Player->jumpProgress < 10 && jump == 0 && PlayerBox->yVelocity > jumpInterruptVel && Player->jumpProgress > 0)
	{
		PlayerBox->yVelocity = jumpInterruptVel;
		Player->jumpProgress = 99;
	}

	if (PlayerBox->inAir > 20 && PlayerBox->crouch == 1)
	{
		PlayerBox->crouch = false;
	}


	HandlePlayerInteract(Player);


	return LEMON_SUCCESS;
}



int PlayerTopDownPhysics(PlayerData *Player, World *GameWorld)
{
	if (Player == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	Object *PlayerObject = Player->PlayerPtr;
	PhysicsRect *PlayerBox = Player->PlayerBox;

	if (PlayerObject == NULL || PlayerBox == NULL)
	{
		return MISSING_DATA;
	}

	if (PlayerObject->State == PAUSE_BEHAVIOUR || (GameWorld->GameState == CUTSCENE && PlayerObject->State != ACTOR) || GameWorld->PlayingText != 0)
	{
		return ACTION_DISABLED;
	}

	float speed = 10.0;

	int hAxis = 0;
	int vAxis = 0;


	// Player input
	hAxis = keyboard[LMN_RIGHT] - keyboard[LMN_LEFT];

	vAxis = keyboard[LMN_UP] - keyboard[LMN_DOWN];


	if (hAxis != 0)
	{
		PlayerBox->xFlip = hAxis;
		Player->PlayerDisplay->direction = RADIAN_90;
	}


	if (hAxis != 0 || vAxis != 0)
	{
		PlayerBox->direction = atan2((double)hAxis, (double)vAxis);
	}

	// Movement velocity acceleration/decceleration
	if (hAxis != 0)
	{
		PlayerBox->xVelocity = speed * sin(PlayerBox->direction);
	}
	else
	{
		PlayerBox->xVelocity = 0.0;
	}
	
	if (vAxis != 0)
	{
		PlayerBox->yVelocity = speed * cos(PlayerBox->direction);
	}
	else
	{
		PlayerBox->yVelocity = 0.0;
	}

	HandlePlayerInteract(Player);

	return LEMON_SUCCESS;
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
		return LEMON_SUCCESS;
	}

	Player->PlayerBox->yVelocity = 14.0;

	if (Player->jumpHeld == 0)
	{
		Lemon_PlaySound("Jump", "Player", PLAYER_SFX, 1.0);
	}

	Player->jumpHeld = 1;

	return LEMON_SUCCESS;
}


int HandlePlayerInteract(PlayerData *Player)
{
	if (Player == NULL)
	{
		return MISSING_DATA;
	}

	if (Player->PlayerPtr->State == ACTOR)
	{
		return ACTION_DISABLED;
	}

	PhysicsRect *InteractBox = Player->InteractBox;
	PhysicsRect *PlayerBox = Player->PlayerBox;

	if (keyboard[LMN_INTERACT] == 1)
	{
		InteractBox->xSize = 50;
		InteractBox->ySize = 50;
		InteractBox->solid = SOLID;
	}

	if (InteractBox->xSize > 0 && InteractBox->ySize > 0)
	{
		float playerCenterX = PlayerBox->xPos + (PlayerBox->xSize>>1);
		float playerCenterY = PlayerBox->yPos + (PlayerBox->ySize>>1);

		InteractBox->xPos = playerCenterX - (InteractBox->xSize>>1) + (PlayerBox->xFlip << 4);
		InteractBox->yPos = playerCenterY - (InteractBox->ySize>>1);
		InteractBox->xSize -= 5;
		InteractBox->ySize -= 5;
		InteractBox->xPosRight = InteractBox->xPos + InteractBox->xSize;
		InteractBox->yPosTop = InteractBox->yPos + InteractBox->ySize;
	}

	return LEMON_SUCCESS;
}


// 1 for true, 0 for false
int PlayerInteractingWithBox(PlayerData Player, PhysicsRect *inputBox)
{
	if (Player.InteractBox == NULL || inputBox == NULL)
	{
		return MISSING_DATA;
	}

	int result = checkBoxOverlapsBoxBroad(inputBox, Player.InteractBox);

	if (result == 1 && Player.InteractBox->solid == SOLID)
	{
		Player.InteractBox->solid = UNSOLID;

		return 1;
	}

	return 0;
}


int animatePlayer(PlayerData *Player)
{
	if (Player == NULL || Player->PlayerDisplay == NULL)
	{
		return MISSING_DATA;
	}

	DisplayData *PlayerDisplay = Player->PlayerDisplay;
	//PhysicsRect *PlayerBox = Player->PlayerBox;

	if (Player->jumpProgress > 0)
	{
		PlayNewAnimation("Jump", 90, PlayerDisplay);
	}

	if (Player->jumpProgress == 0)
	{
		PlayNewAnimation("Stand", 0, PlayerDisplay);
	}

	return LEMON_SUCCESS;
}


int checkIfGrounded(World *GameWorld, PhysicsRect *inputBox)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL || inputBox == NULL)
	{
		return 0;
	}

	Object *detectedObject = GameWorld->ObjectList->firstObject;

	inputBox->yPos += GameWorld->GlobalGravityY * 2;
	inputBox->xPos += GameWorld->GlobalGravityX * 2;

	detectedObject = GetCollidingObject(inputBox, GameWorld->ObjectList);

	inputBox->yPos -= GameWorld->GlobalGravityY * 2;
	inputBox->xPos -= GameWorld->GlobalGravityX * 2;


	if (detectedObject == NULL)
	{
		inputBox->direction = RADIAN_90;

		return 0;
	}

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
		putConsoleStrIntStr("Could not find sprite ", spriteID, " for Player");
		return MISSING_DATA;
	}

	PlayerDisplay->spriteBuffer = currentSprite;
	PlayerDisplay->currentSprite = spriteID;

	return LEMON_SUCCESS;
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
		putConsoleStrStr("Could not find sprite '", strcat(spriteName, "' for Player") );
		return MISSING_DATA;
	}

	PlayerDisplay->spriteBuffer = currentSprite;
	PlayerDisplay->currentSprite = currentSprite->spriteID;

	return LEMON_SUCCESS;
}

