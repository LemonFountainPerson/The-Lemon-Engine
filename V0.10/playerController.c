#include "LemonEngine.h"


int InitialisePlayerObject(Object *Player, World *GameWorld)
{
	if (GameWorld == NULL || Player == NULL || Player->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsBox *PlayerBox = Player->ObjectBox;

	PlayerBox->xSize = 32;
	PlayerBox->ySize = 50;
	PlayerBox->solid = BODY;

	if (GameWorld->Player.PlayerPtr != NULL && GameWorld->Player.PlayerPtr->State != EMPTY_OBJECT)
	{
		return ACTION_DISABLED;
	}

	GameWorld->MainCamera.CameraMode = FOLLOW_PLAYER;
	GameWorld->Player.PlayerPtr = Player;
	GameWorld->Player.PlayerBox = Player->ObjectBox;
	GameWorld->Player.PlayerDisplay = getDisplay(Player);
	setDisplayLayer(GameWorld->Player.PlayerPtr, MIDDLEGROUND_2);
	Player->State = DEFAULT_STATE;
	setRotateMode(Player, LEFT_RIGHT_ROTATION);
	addPhysics(Player, true);
	
	setObjectName(Player, "MainPlayer");

	ResetPlayer(&GameWorld->Player);

	char counter[30];
	snprintf(counter, 30, "CoinCount: %d", GameWorld->Player.coinCount);
	addTextWithName(counter, "CoinCounter", -600.0, 300.0, GameWorld);

	return LEMON_SUCCESS;
}


PlayerData* InitialisePlayerData(PlayerData *Player)
{
	if (Player == NULL)
	{
		return NULL;
	}


	resetPhysicsBox(&Player->InteractBox);


	// Player set-up
	Player->coinCount = 0;

	Player->jumpProgress = 0;
	Player->jumpHeld = false;
	Player->coyoteFrames = 7;
	Player->jumpRange = 11;
	Player->cancelRange = 11;
	Player->jumpForce = 14.0;

	Player->PlayerBox = NULL;
	Player->PlayerDisplay = NULL;
	Player->PlayerPtr = NULL;

	return Player;
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
	if (Player == NULL || Player->PlayerPtr == NULL || Player->PlayerPtr->State == EMPTY_OBJECT || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GamePaused > 0)
	{
		return ACTION_DISABLED;
	}

	if (DebugSettings.noclip)
	{
		playerNoclip(Player);
		return LEMON_SUCCESS;
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


void playerNoclip(PlayerData *Player)
{
	PhysicsBox *playerBox = Player->PlayerBox;
	playerBox->solid = UNSOLID;

	if (buttons[LMN_LEFT])
	{
		playerBox->xVelocity -= 2.5;
	}

	if (buttons[LMN_RIGHT])
	{
		playerBox->xVelocity += 2.5;
	}

	if (buttons[LMN_UP])
	{
		playerBox->yVelocity += 2.5;
	}

	if (buttons[LMN_DOWN])
	{
		playerBox->yVelocity -= 2.5;
	}

	playerBox->xVelocity *= 0.88;
	playerBox->yVelocity *= 0.88;
	playerBox->forwardVelocity = 0.0;

	HandlePlayerInteract(Player, NULL);
	
	return;
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

	PhysicsBox *PlayerBox = Player->PlayerBox;

	float hAxis = 0.0;
	float vAxis = 0.0;
	bool jump = false;

	// Player input
	if (!playingText(GameWorld) && PlayerObject->State != PAUSE_STATE)
	{
		if (fabs(GamePadInput.leftStickX) > 0.001)
		{
			hAxis = GamePadInput.leftStickX;
		}
		else
		{
			hAxis = (float)(buttons[LMN_RIGHT] != 0) - (buttons[LMN_LEFT] != 0);
		}

		if (fabs(GamePadInput.leftStickY) > 0.001)
		{
			vAxis = GamePadInput.leftStickY;
		}
		else
		{
			vAxis = (float)(buttons[LMN_UP] != 0) - (buttons[LMN_DOWN] != 0);
		}

		if (buttons[LMN_JUMP])
		{
			jump = true;
		}
		else if (PlayerBox->yVelocity < 0.0)
		{
			Player->jumpHeld = false;
		}

		HandlePlayerInteract(Player, GameWorld);
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
		PlayerBox->forwardVelocity += hAxis * 0.85;
		forwardFriction = 0.92;
	}
	else
	{
		PlayerBox->forwardVelocity += hAxis;
		forwardFriction = 0.907;

		if (hAxis == 0)
		{
			forwardFriction = 0.6;
			xFriction = 0.6;
		}
	}

	ApplyFriction(PlayerBox, forwardFriction, xFriction, 1.0);


	PlayerBox->yPos += 4.0;
	bool spaceAboveHead = (GetCollidingObject(PlayerBox, GameWorld->ObjectList) == NULL);
	PlayerBox->yPos -= 4.0;

	if (PlayerBox->inAir < 1)
	{
		Player->jumpProgress = 0;	
	}

	if (vAxis < -0.75 && PlayerBox->inAir < 20)
	{
		PlayerBox->crouch = 1;
		PlayerBox->ySize = 32;
	}
	else if (vAxis >= -0.5 && spaceAboveHead)
	{
		PlayerBox->crouch = 0;
		PlayerBox->ySize = 50;
	}

	if ((jump && !Player->jumpHeld && PlayerBox->inAir < Player->coyoteFrames && Player->jumpProgress == 0) || (PlayerBox->yVelocity > 0.0 && Player->jumpProgress > 0) )
	{
		PlayerJump(Player, jump);
	}


	return LEMON_SUCCESS;
}



int PlayerTopDownPhysics(PlayerData *Player, World *GameWorld)
{
	if (Player == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	Object *PlayerObject = Player->PlayerPtr;
	PhysicsBox *PlayerBox = Player->PlayerBox;

	if (PlayerObject == NULL || PlayerBox == NULL)
	{
		return MISSING_DATA;
	}

	float speed = 14.0;

	int hAxis = 0;
	int vAxis = 0;


	// Player input
	if (!playingText(GameWorld) && PlayerObject->State != PAUSE_STATE)
	{
		hAxis = (buttons[LMN_RIGHT] != 0) - (buttons[LMN_LEFT] != 0);

		vAxis = (buttons[LMN_UP] != 0) - (buttons[LMN_DOWN] != 0);

		HandlePlayerInteract(Player, GameWorld);
	}


	if (hAxis != 0 || vAxis != 0)
	{
		double radDirection = atan2((double)hAxis, (double)vAxis);

		if (hAxis != 0)
		{
			PlayerBox->xVelocity = speed * sin(radDirection);
		}
		else
		{
			PlayerBox->xVelocity = 0.0;
		}
		
		if (vAxis != 0)
		{
			PlayerBox->yVelocity = speed * cos(radDirection);
		}
		else
		{
			PlayerBox->yVelocity = 0.0;
		}

		PlayerBox->direction = radDirection * RADIAN_TO_DEGREE_PI;
	}
	else
	{
		PlayerBox->xVelocity = 0.0;
		PlayerBox->yVelocity = 0.0;
	}

	return LEMON_SUCCESS;
}


int PlayerJump(PlayerData *Player, bool jump)
{
	// Jump handling - Player jump must be less than 0, and third condition ensures that the
	// Player must be moving upwards or not be holding the button from previous jump to continue jumping/jump again
	// Jumping is set up this way both for variable jumping heights and coyote frames

	if (Player->jumpProgress < Player->jumpRange && jump)
	{
		if (Player->jumpProgress < 1)
		{
			PlaySound("Player/Jump", PLAYER_SFX, 0.8);
			PlayNewAnimation("Jump", 9, Player->PlayerDisplay);
			Player->PlayerBox->yVelocity = Player->jumpForce;
			Player->jumpHeld = true;
			Player->jumpProgress = 0;	// in case jump progress is somehow negative
		}
		else
		{
			Player->PlayerBox->yVelocity = Player->jumpForce * 0.9;
		}
	}

	if (Player->jumpProgress < Player->cancelRange && !jump)
	{
		Player->PlayerBox->yVelocity *= 0.5;
		Player->jumpProgress = Player->jumpRange;
	}
	
	if (Player->jumpProgress < 100)
	{
		Player->jumpProgress++;
	}

	return LEMON_SUCCESS;
}


int HandlePlayerInteract(PlayerData *Player, World *GameWorld)
{
	if (Player == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsBox *InteractBox = &Player->InteractBox;

	if (Player->PlayerPtr->State == ACTOR_STATE || (GameWorld != NULL && GameWorld->CurrentCutscene != NO_CUTSCENE))
	{
		InteractBox->xSize = 0;
		InteractBox->ySize = 0;

		return ACTION_DISABLED;
	}

	PhysicsBox *PlayerBox = Player->PlayerBox;
	
	if (buttons[LMN_INTERACT] == 1)
	{
		InteractBox->xSize = 50;
		InteractBox->ySize = 50;
		float playerCenterX = PlayerBox->xPos + (PlayerBox->xSize>>1);
		float playerCenterY = PlayerBox->yPos + (PlayerBox->ySize>>1);

		InteractBox->xPos = playerCenterX - (InteractBox->xSize>>1) + (PlayerBox->xFlip << 4);
		InteractBox->yPos = playerCenterY - (InteractBox->ySize>>1);
	}
	else
	{
		InteractBox->xSize = 0;
		InteractBox->ySize = 0;
	}

	return LEMON_SUCCESS;
}


bool PlayerInteractingWithBox(PhysicsBox *inputBox, World *GameWorld)
{
	if (inputBox == NULL || GameWorld == NULL)
	{
		return false;
	}

	PhysicsBox *interactBox = &GameWorld->Player.InteractBox;

	if (checkBoxOverlapsBoxBroad(inputBox, interactBox))
	{
		interactBox->xSize = 0;
		return true;
	}

	return false;
}


int animatePlayer(PlayerData *Player)
{
	if (Player == NULL || Player->PlayerDisplay == NULL)
	{
		return MISSING_DATA;
	}

	DisplayData *PlayerDisplay = Player->PlayerDisplay;

	if (Player->jumpProgress <= 0)
	{
		PlayNewAnimation("Stand", 0, PlayerDisplay);
	}

	return LEMON_SUCCESS;
}


int checkIfGrounded(World *GameWorld, PhysicsBox *inputBox)
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

	return switchSprite(spriteID, USE_CURRENT_SPRITESET, PlayerDisplay);
}


int switchPlayerSpriteName(const char spriteName[], DisplayData *PlayerDisplay)
{
	if (PlayerDisplay == NULL || PlayerDisplay->spriteSetSource == NULL)
	{
		return MISSING_DATA;
	}

	if (PlayerDisplay->spriteBuffer != NULL && strcmp(PlayerDisplay->spriteBuffer->name, spriteName) == 0)
	{
		return EXECUTION_UNNECESSARY;
	}


	return switchSpriteByName(spriteName, USE_CURRENT_SPRITESET, PlayerDisplay);
}

