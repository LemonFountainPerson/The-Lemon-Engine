#include "playerController.h"


PlayerData* initialisePlayer(World *gameWorld)
{
	PlayerData *player = malloc(sizeof(PlayerData));

	if (player == NULL)
	{
		printf("Error: Could not allocate space for player.\n");
		fflush(stdout);
		return NULL;
	}

	player->PlayerBox = malloc(sizeof(PhysicsRect));

	if (player->PlayerBox == NULL)
	{
		printf("Error: Could not allocate space for player Physics Box.\n");
		fflush(stdout);
		return NULL;
	}

	if (gameWorld != NULL)
	{
		gameWorld->Player = player;
	}

	// Player set-up
	player->PlayerBox->xPos = 100.0;
	player->PlayerBox->yPos = 100.0;
	player->PlayerBox->xSize = PLAYERWIDTH;
	player->PlayerBox->ySize = PLAYERHEIGHT;
	player->PlayerBox->xPosRight = 100 + PLAYERWIDTH;
	player->PlayerBox->yPosTop = 100 + PLAYERHEIGHT;
	player->PlayerBox->yVelocity = 0.0;
	player->PlayerBox->xVelocity = 0.0;
	player->PlayerBox->solid = SOLID;
	player->PlayerBox->direction = RADIAN_90;

	player->maxXVel = 15.0;
	player->maxYVel = 32.0;
	player->PhysicsXVelocity = 0.0;
	player->PhysicsYVelocity = 0.0;

	player->inAir = 0;
	player->jumpProgress = 0;
	player->jumpHeld = 0;
	player->crouch = 0;
	player->coinCount = 0;
	player->xFlip = 1;
	player->currentSprite = 2;
	player->playerLayer = MIDDLEGROUND;

	SpriteSet *newSetPtr = malloc(sizeof(SpriteSet));
	player->spriteSetPtr = newSetPtr;

	if (newSetPtr == NULL)
	{
		printf("Error: Could not allocate space for player sprite set.\n");
		fflush(stdout);
		free(newSetPtr);
		return player;
	}

	// Load player sprites
	player->spriteSetPtr->setID = 1;
	player->spriteSetPtr->spriteCount = 0;
	player->spriteSetPtr->prevSet = NULL;
	player->spriteSetPtr->nextSet = NULL;
	player->spriteSetPtr->firstSprite = NULL;
	player->spriteSetPtr->lastSprite = NULL;
	

	loadPlayerSprite("PLAYER_Missing.png", 1, 4, player);
	loadPlayerSprite("Idle_R.png", 1, 4, player);
	loadPlayerSprite("Spin_R.png", 1, 4, player);

	printf("Initialised Player with %d sprite(s)\n", player->spriteSetPtr->spriteCount);
	fflush(stdout);

	switchPlayerSprite(player->currentSprite, 1, player);

	return player;
}


int ResetPlayer(PlayerData *Player)
{
	if (Player == NULL)
	{
		return MISSING_DATA;
	}

	Player->PlayerBox->xPos = 64.0;
	Player->PlayerBox->yPos = 96.0;
	Player->PlayerBox->xVelocity = 0.0;
	Player->PlayerBox->yVelocity = 0.0;
	Player->PhysicsXVelocity = 0.0;
	Player->PhysicsYVelocity = 0.0;
	
	switchPlayerSprite(2, 1, Player);

	Player->jumpProgress = 0;
	Player->inAir = 0;
	Player->jumpHeld = 0;


	return 0;
}


FunctionResult updatePlayer(PlayerData *player, World *gameWorld, int keyboard[256])
{
	if (player == NULL || gameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	if (gameWorld->GamePaused > 0 || gameWorld->GameState != GAMEPLAY)
	{
		return ACTION_DISABLED;
	}

	int hAxis = 0;
	int vAxis = 0;
	int jump = 0;


	// Player input
	hAxis = (keyboard[LMN_RIGHT] || keyboard['D']) - (keyboard[LMN_LEFT] || keyboard['A']);

	if (hAxis != 0)
	{
		player->xFlip = hAxis;
	}

	vAxis = (keyboard[LMN_UP] || keyboard['E']) - (keyboard[LMN_DOWN] || keyboard['S']);

	player->PlayerBox->yVelocity += vAxis;

	if (vAxis < 0 && player->inAir == 0)
	{
		player->crouch = 1;
		player->PlayerBox->yVelocity -= 1.0;
	}

	if (vAxis > -1 && player->inAir == 0)
	{
		player->crouch = 0;
	}


	if (keyboard['M'] == 1)
	{
		player->PlayerBox->yVelocity = 16.0;
	}

	if (keyboard['C'] || keyboard[LMN_SPACE] || keyboard['W'])
	{
		jump = 1;
	}
	else
	{
		player->jumpHeld = 0;
	}


	player->PlayerBox->yPos += 2.0;
	int againstCeiling = (GetObjectOverlappingBox(player->PlayerBox, gameWorld) != NULL);
	player->PlayerBox->yPos -= 2.0;

	if (againstCeiling == 0 && jump == 1 && ((player->inAir < 10 && player->jumpHeld == 0) || (player->PlayerBox->yVelocity > 12.0 && player->jumpProgress > 0) ))
	{
		playerJump(player, hAxis, vAxis);
	}

	if (player->jumpProgress < 10 && jump == 0 && player->PlayerBox->yVelocity > 12.0 && player->jumpProgress > 0)
	{
		player->PlayerBox->yVelocity = 12.0 * deltaTime;

		player->jumpProgress = 99;
	}


	// Movement velocity acceleration/decceleration
	if (hAxis == 0)
	{
		player->PlayerBox->xVelocity *= 0.8;
	}

	if (player->inAir > 0)
	{
		player->PlayerBox->xVelocity += hAxis * 0.75 * deltaTime;
		player->PlayerBox->xVelocity *= 0.951;

		if (hAxis != 0)
		{
			player->PhysicsXVelocity = 0.0;
		}

		if (fabs(player->PhysicsXVelocity) > 0.1)
		{
			player->PlayerBox->xVelocity = player->PhysicsXVelocity;
		}
	}
	else
	{
		player->PlayerBox->xVelocity += hAxis * 1.0 * deltaTime;
		player->PlayerBox->xVelocity *= 0.935;
	}


	if (fabs(player->PlayerBox->xVelocity) < 0.1)
	{
		player->PlayerBox->xVelocity = 0.0;
	}


	if (fabs(player->PlayerBox->yVelocity) < 0.1)
	{
		player->PlayerBox->yVelocity = 0.0;
	}


	// Gravity
	player->PlayerBox->yVelocity += (gameWorld->Gravity * deltaTime);

	if (fabs(player->PlayerBox->xVelocity) > player->maxXVel)
	{
		player->PlayerBox->xVelocity = (player->PlayerBox->xVelocity/fabs(player->PlayerBox->xVelocity)) * (player->maxXVel);
	}

	if (fabs(player->PlayerBox->yVelocity) > player->maxYVel)
	{
		player->PlayerBox->yVelocity = (player->PlayerBox->yVelocity/fabs(player->PlayerBox->yVelocity)) * (player->maxYVel);
	}


	// collision detection
	MovePlayerX(player, gameWorld);

	MovePlayerY(player, gameWorld);
	


	player->PlayerBox->xPosRight = player->PlayerBox->xPos + player->PlayerBox->xSize;
	player->PlayerBox->yPosTop = player->PlayerBox->yPos + player->PlayerBox->ySize;


	int onGround = checkIfGrounded(gameWorld, player);

	if (onGround == 0 && player->inAir < 100)
	{
		player->inAir++;
	}

	if (onGround > 0)
	{
		player->inAir = 0;
	}


	setSprite(player);

	if (player->PlayerBox->yPos < -60.0 || player->PlayerBox->yPos > 60000.0)
	{
		player->PlayerBox->yPos = 150.0;
		player->PlayerBox->yVelocity = 10.0;
	}

	if (player->PlayerBox->xPos < 0.0)
	{
		player->PlayerBox->xPos = 0.0;
		player->PlayerBox->xVelocity = 0.0;
	}

	return 0;
}


int setSprite(PlayerData *player)
{
	int prevSprite = player->currentSprite;

	if (player->jumpProgress > 0)
	{
		player->currentSprite = 3;
	}

	if (player->inAir == 0)
	{
		player->currentSprite = 2;
	}

	if (prevSprite != player->currentSprite)
	{
		switchPlayerSprite(player->currentSprite, 1, player);
	}

	return 0;
}


int playerJump(PlayerData *player, int hAxis, int vAxis)
{
	// Jump handling - player jump must be less than 0, and third condition ensures that the
	// player must be moving upwards or not be holding the button from previous jump to continue jumping/jump again
	// Jumping is set up this way both for variable jumping heights and coyote frames

	if (player->jumpProgress < 100)
	{
		player->jumpProgress++;
	}

	if (player->jumpProgress > 9)
	{
		return 0;
	}

	player->PlayerBox->yVelocity = 16.0;

	if (player->jumpHeld == 0)
	{
		LemonPlaySound("Jump", "Player", PLAYER_SFX, 1.0);
				
		if (hAxis == player->PlayerBox->xVelocity/fabs(player->PlayerBox->xVelocity) && fabs(player->PhysicsXVelocity) < fabs(player->PlayerBox->xVelocity))
		{
			player->PlayerBox->xVelocity += player->PhysicsXVelocity;
		}
	}

	player->jumpHeld = 1;

	return 0;
}


int MovePlayerX(PlayerData *player, World *GameWorld)
{
	if (fabs(player->PlayerBox->xVelocity) < 0.1)
	{
		return 0;
	}

	PhysicsRect *PlayerBox = player->PlayerBox;
	double prevXPos = PlayerBox->xPos;

	PlayerBox->xPos += (PlayerBox->xVelocity * deltaTime);


	Object *currentObject;
	currentObject = GameWorld->ObjectList->firstObject;


	int count = 0;

	while (currentObject != NULL && count < 16)
	{
		currentObject = GetObjectOverlappingBox(player->PlayerBox, GameWorld);

		count++;

		if (currentObject == NULL)
		{
			return 0;
		}

		int slopeClimb = 0;
		while (CheckBoxOverlapsBox(PlayerBox, currentObject->ObjectBox) == 1 && slopeClimb < 8)
		{
			PlayerBox->yPos++;
			slopeClimb++;
		}

		if (GetObjectOverlappingBox(player->PlayerBox, GameWorld) == NULL)
		{
			continue;
		}

		PlayerBox->yPos -= slopeClimb;


		double objX = currentObject->ObjectBox->xPos;
		double objY = currentObject->ObjectBox->yPos;
		double objXRight = objX + currentObject->ObjectBox->xSize;
		double objYTop = objY + currentObject->ObjectBox->ySize;
		double prevObjXCenter = ((objX + objXRight) / 2.0) - (currentObject->ObjectBox->xVelocity * deltaTime);


		switch(currentObject->ObjectBox->solid)
		{
			// Y = X * (ySize/xSize)
			case FLAT_SLOPE_LR:
			{
				int prevObjXRight = objXRight - (currentObject->ObjectBox->xVelocity * deltaTime);

				if ((int)prevXPos >= prevObjXRight)
				{
					PlayerBox->xPos = objXRight;
				}
				else
				{
					PlayerBox->xPos = prevXPos;
					ClimbFlatSlope(PlayerBox, currentObject->ObjectBox, GameWorld);
				}


			} break;

			// Y = (xSize - X) * (ySize/xSize)
			case FLAT_SLOPE_RL:
			{
				int prevObjX = objX - (currentObject->ObjectBox->xVelocity * deltaTime);

				if ((int)prevXPos + PlayerBox->xSize <= prevObjX)
				{
					PlayerBox->xPos = objX - PlayerBox->xSize;
				}
				else
				{
					PlayerBox->xPos = prevXPos;
					ClimbFlatSlope(PlayerBox, currentObject->ObjectBox, GameWorld);
				}

			} break;


			case JUMP_THROUGH:
			case UNSOLID:
				break;


			default:
			{
				if (prevXPos < prevObjXCenter)
				{
					PlayerBox->xPos = objX - PlayerBox->xSize;
				}
				else
				{
					PlayerBox->xPos = objXRight;
				}
			} break;
		}


		ApplyXPhysics(player, currentObject);
	}


	return 0;
}


int ApplyXPhysics(PlayerData *player, Object *inputObject)
{
	if (inputObject == NULL || inputObject->layer > FOREGROUND)
	{
		return 0;
	}

	if (player->PlayerBox->xVelocity > 0.0)
	{
		if (inputObject->ObjectBox->xVelocity > 0.1)
		{
			player->PlayerBox->xVelocity = inputObject->ObjectBox->xVelocity;
		}
		else
		{
			if (inputObject->ObjectBox->solid == 2)
			{
				return 0;
			}

			player->PhysicsXVelocity = 0.0;
			player->PlayerBox->xVelocity = 0.0;
		}

		return 0;
	}

	if (player->PlayerBox->xVelocity < 0.0)
	{
		if (inputObject->ObjectBox->xVelocity < -0.1)
		{
			player->PlayerBox->xVelocity = inputObject->ObjectBox->xVelocity;
		}
		else
		{		
			if (inputObject->ObjectBox->solid == 3)
			{
				return 0;
			}

			player->PhysicsXVelocity = 0.0;
			player->PlayerBox->xVelocity = 0.0;
		}

		return 0;
	}

	return 0;
}




int MovePlayerY(PlayerData *player, World *GameWorld)
{
	PhysicsRect *PlayerBox = player->PlayerBox;

	double prevYPos = PlayerBox->yPos;

	PlayerBox->yPos += PlayerBox->yVelocity * deltaTime;
	
	
	Object *currentObject;
	currentObject = GameWorld->ObjectList->firstObject;

	if (currentObject == NULL)
	{
		return 0;
	}


	int count = 0;
	int result = 0;

	while (currentObject != NULL && count < 16)
	{
		currentObject = GetObjectOverlappingBox(PlayerBox, GameWorld);

		count++;

		if (currentObject == NULL)
		{
			return 0;
		}


		double objX = currentObject->ObjectBox->xPos;
		double objY = currentObject->ObjectBox->yPos;
		double objXRight = objX + currentObject->ObjectBox->xSize;
		double objYTop = objY + currentObject->ObjectBox->ySize;
		double prevObjYCenter = ((objY + objYTop) / 2.0) - (currentObject->ObjectBox->yVelocity * deltaTime);

		switch(currentObject->ObjectBox->solid)
		{
			// Y = X * (ySize/xSize)
			case FLAT_SLOPE_LR:
			{
				int prevObjY = objY - (currentObject->ObjectBox->yVelocity * deltaTime);

				if ((int)prevYPos + PlayerBox->ySize <= prevObjY)
				{
					PlayerBox->yPos = (objY - PlayerBox->ySize);
					player->jumpProgress = 100;
				}
				else
				{
					// If player is halfway off edge, floor of slope continues to be calculated as Y = X * slope
					// So here it is reset to the expected maximum if it over
					double slope = ((double)currentObject->ObjectBox->ySize/(double)currentObject->ObjectBox->xSize);

					double slopeFloor = ((PlayerBox->xPos + PlayerBox->xSize - objX) * slope);

					if (slopeFloor > currentObject->ObjectBox->ySize)
					{
						slopeFloor = currentObject->ObjectBox->ySize;
					}

					PlayerBox->yPos = slopeFloor + objY;

					player->jumpProgress = 0;
				}		

			} break;

			// Y = (xSize - X) * (ySize/xSize)
			case FLAT_SLOPE_RL:
			{
				int prevObjY = objY - (currentObject->ObjectBox->yVelocity * deltaTime);

				if ((int)prevYPos + PlayerBox->ySize <= prevObjY)
				{
					PlayerBox->yPos = (objY - PlayerBox->ySize);
					player->jumpProgress = 100;
				}
				else
				{
					// If player is halfway off edge, floor of slope continues to be calculated as Y = xSize - X * slope
					// So here it is reset to the expected maximum if it over
					double slope = ((double)currentObject->ObjectBox->ySize/(double)currentObject->ObjectBox->xSize);

					double slopeFloor = ((currentObject->ObjectBox->xSize - (PlayerBox->xPos - objX)) * slope);

					if (slopeFloor > currentObject->ObjectBox->ySize)
					{
						slopeFloor = currentObject->ObjectBox->ySize;
					}

					PlayerBox->yPos = slopeFloor + objY;

					player->jumpProgress = 0;
				}

			} break;


			case JUMP_THROUGH:
			{
				if (PlayerBox->yVelocity < 0.1 && prevYPos >= prevObjYCenter && player->crouch < 1)
				{
					PlayerBox->yPos = objYTop;
					player->jumpProgress = 0;
				}

			} break;


			case UNSOLID:
				break;


			default:
			{
				if (prevYPos < prevObjYCenter)
				{
					PlayerBox->yPos = objY - PlayerBox->ySize;
					player->jumpProgress = 100;
				}
				else
				{
					PlayerBox->yPos = objYTop;
					player->jumpProgress = 0;
				}

			} break;

		}

		ApplyYPhysics(player, currentObject);
	}

	return 0;

}


int ApplyYPhysics(PlayerData *player, Object *inputObject)
{
	if (inputObject->layer > FOREGROUND)
	{
		return 0;
	}

	if (player->PlayerBox->yVelocity > 0.0)
	{
		if (inputObject->ObjectBox->yVelocity > 0.0)
		{
			player->PlayerBox->yVelocity = inputObject->ObjectBox->yVelocity;
		}
		else
		{
			player->PhysicsYVelocity = 0.0;
			player->PlayerBox->yVelocity = 0.0;
		}

		return 0;
	}

	if (player->PlayerBox->yVelocity < 0.0)
	{
		if (inputObject->ObjectBox->yVelocity < 0.0)
		{
			player->PlayerBox->yVelocity = inputObject->ObjectBox->yVelocity;
		}
		else
		{			
			player->PhysicsYVelocity = 0.0;
			player->PlayerBox->yVelocity = 0.0;
		}

		return 0;
	}

	return 0;
}


int checkIfGrounded(World *gameWorld, PlayerData *player)
{
	Object *detectedObject;
	detectedObject = gameWorld->ObjectList->firstObject;

	int i = 0;
	int result = 0;

	int prevYSize = player->PlayerBox->ySize;
	player->PlayerBox->ySize = 8;
	player->PlayerBox->yPos -= 2.0;

	detectedObject = GetObjectOverlappingBox(player->PlayerBox, gameWorld);

	player->PlayerBox->yPos += 2.0;
	player->PlayerBox->ySize = prevYSize;


	if (detectedObject == NULL)
	{
		player->PlayerBox->direction = RADIAN_90;

		return 0;
	}

	if (player->inAir > 0 && fabs(player->PhysicsXVelocity) > 0.1)
	{
		player->PlayerBox->xVelocity = 0.0;
	}

	player->PhysicsXVelocity = detectedObject->ObjectBox->xVelocity;
	player->PhysicsYVelocity = detectedObject->ObjectBox->yVelocity;

	assignDirection(player->PlayerBox, detectedObject);

	return 1;
}


int switchPlayerSprite(int spriteID, int spriteSet, PlayerData *player)
{
	// Find correct player sprite set
	SpriteSet *playerSprSet = player->spriteSetPtr;

	if (playerSprSet == NULL)
	{
		return -1;
	}

	while (playerSprSet->nextSet != NULL && playerSprSet->setID != spriteSet)
	{
		playerSprSet = playerSprSet->nextSet;
	}

	if (playerSprSet->setID != spriteSet)
	{
		printf("Couldn't find sprite set %d\n", spriteSet);
		return -1;
	}

	// Find correct sprite from sprite set
	Sprite *currentSprite;
	int i;

	if (spriteID > playerSprSet->spriteCount >> 1)
	{
		// Start from end
		currentSprite = playerSprSet->lastSprite;
		i = playerSprSet->spriteCount;

		while (i > 0 && currentSprite != NULL && currentSprite->spriteID != spriteID)
		{
			currentSprite = currentSprite->prevSprite;
			i--;
		}

	}
	else
	{
		// Start from front
		currentSprite = playerSprSet->firstSprite;
		i = 1;

		while (i < playerSprSet->spriteCount && currentSprite != NULL && currentSprite->spriteID != spriteID)
		{
			currentSprite = currentSprite->nextSprite;
			i++;
		}

	}

	if (currentSprite == NULL || currentSprite->spriteID != spriteID)
	{
		printf("Could not find sprite %d for Player\n", spriteID);
		fflush(stdout);
		return -1;
	}

	player->spriteBuffer = currentSprite;
	player->currentSprite = spriteID;

	return 0;
}


int switchPlayerSpriteName(char spriteName[MAX_LEN], int spriteSet, PlayerData *player)
{
	// Find correct player sprite set
	SpriteSet *playerSprSet = player->spriteSetPtr;

	int i = 1;

	while (playerSprSet->nextSet != NULL && i < spriteSet)
	{
		playerSprSet = playerSprSet->nextSet;
		i++;
	}

	// Find correct sprite from sprite set
	Sprite *currentSprite;
	currentSprite = playerSprSet->firstSprite;
	i = 1;

	while (i < playerSprSet->spriteCount && currentSprite != NULL && strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		currentSprite = currentSprite->nextSprite;
		i++;
	}

	
	if (currentSprite == NULL || strcmp(currentSprite->spriteName, spriteName) != 0)
	{
		printf("Could not find sprite '%s' for Player\n", spriteName);
		fflush(stdout);
		return -1;
	}

	player->spriteBuffer = currentSprite;
	player->currentSprite = currentSprite->spriteID;

	return 0;
}

