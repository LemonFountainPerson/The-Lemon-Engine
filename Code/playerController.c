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

	if (gameWorld != NULL)
	{
		gameWorld->Player = player;
	}

	// Player set-up
	player->xPos = 100.0;
	player->yPos = 100.0;
	player->xPosRight = 100 + PLAYERWIDTH - 1;
	player->yPosTop = 100 + PLAYERHEIGHT - 1;
	player->yVelocity = 0.0;
	player->xVelocity = 0.0;
	player->direction = 90.0;
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
	player->spriteCount = 0;
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

	if (player->spriteCount > 0)
	{
		printf("Initialised Player with %d sprite(s) of size %d\n", player->spriteCount, player->spriteSetPtr->spriteCount);
		fflush(stdout);
	}

	switchPlayerSprite(player->currentSprite, 1, player);

	return player;
}


FunctionResult updatePlayer(PlayerData *player, World *gameWorld, int keyboard[256], double deltaTime)
{
	if (player == NULL || gameWorld->objectList == NULL)
	{
		return MISSING_DATA;
	}

	if (gameWorld->GamePaused == 1)
	{
		return ACTION_DISABLED;
	}

	int hAxis = 0;
	int vAxis = 0;
	int jump = 0;
	int onGround = checkIfGrounded(gameWorld, player);


	if (onGround == 0 && player->inAir < 100)
	{
		player->inAir++;
	}

	if (onGround > 0)
	{
		player->inAir = 0;
	}


	// Player input
	hAxis = (keyboard[LMN_RIGHT] || keyboard['D']) - (keyboard[LMN_LEFT] || keyboard['A']);

	if (hAxis != 0)
	{
		player->xFlip = hAxis;
	}

	vAxis = (keyboard[LMN_UP] || keyboard['E']) - (keyboard[LMN_DOWN] || keyboard['S']);

	if (vAxis < 0 && onGround > 0)
	{
		player->crouch = 1;
		player->yVelocity -= 1.0;
	}

	if (vAxis > -1 && onGround > 0)
	{
		player->crouch = 0;
	}


	if (keyboard['M'] == 1)
	{
		player->yVelocity = 16.0;
	}

	if (keyboard['C'] || keyboard[LMN_SPACE] || keyboard['W'])
	{
		jump = 1;
	}
	else
	{
		player->jumpHeld = 0;
	}

	// Jump handling - player jump must be less than 0, and third condition ensures that the
	// player must be moving upwards or not be holding the button from previous jump to continue jumping/jump again
	// Jumping is set up this way both for variable jumping heights and coyote frames
	if (jump == 1 && ((player->inAir < 10 && player->jumpHeld == 0) || (player->yVelocity > 12.0 && player->jumpProgress > 0) ))
	{
		if (player->jumpProgress < 10)
		{
			playerJump(player, hAxis, vAxis);
		}

		if (player->jumpProgress < 100)
		{
			player->jumpProgress++;
		}

	}


	if (player->jumpProgress < 10 && jump == 0 && player->yVelocity > 12.0 && player->jumpProgress > 0)
	{
		player->yVelocity = 12.0 * deltaTime;

		player->jumpProgress = 99;
	}


	// Movement velocity acceleration/decceleration
	if (hAxis == 0)
	{
		player->xVelocity *= 0.9;
	}

	if (player->inAir > 0)
	{
		player->xVelocity += hAxis * 0.75 * deltaTime;
		player->xVelocity *= 0.951;

		if (fabs(player->PhysicsXVelocity) > 0.2)
		{
			player->xVelocity = player->PhysicsXVelocity + (hAxis * 0.75 * deltaTime);
		}
	}
	else
	{
		player->xVelocity += hAxis * 1.0  * deltaTime;
		player->xVelocity *= 0.935;
	}


	if (fabs(player->xVelocity) < 0.1)
	{
		player->xVelocity = 0.0;
	}


	if (fabs(player->yVelocity) < 0.1)
	{
		player->yVelocity = 0.0;
	}


	// Gravity
	player->yVelocity += (gameWorld->Gravity * deltaTime);

	if (fabs(player->xVelocity) > player->maxXVel)
	{
		player->xVelocity = (player->xVelocity/fabs(player->xVelocity)) * (player->maxXVel);
	}

	if (fabs(player->yVelocity) > player->maxYVel)
	{
		player->yVelocity = (player->yVelocity/fabs(player->yVelocity)) * (player->maxYVel);
	}


	// collision detection

	MovePlayerX(player, gameWorld, deltaTime);

	MovePlayerY(player, gameWorld, deltaTime);
	

	player->xPosRight = player->xPos + PLAYERWIDTH - 1;
	player->yPosTop = player->yPos + PLAYERHEIGHT - 1;

	setSprite(player);

	if (player->yPos < -60.0 || player->yPos > 60000.0)
	{
		player->yPos = 150.0;
		player->yVelocity = 10.0;
	}

	if (player->xPos < 0.0)
	{
		player->xPos = 0.0;
		player->xVelocity = 0.0;
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
	player->yVelocity = 16.0;

	if (player->jumpHeld == 0)
	{
		player->inAir = 1;

		LemonPlaySound("Jump", "Player", PLAYER_SFX, 1.0);
				
		if (hAxis != 0)
		{
			player->PhysicsXVelocity = 0.0;
		}
	}

	player->jumpHeld = 1;

	return 0;
}


int MovePlayerX(PlayerData *player, World *gameWorld, double deltaTime)
{
	double prevYPos = player->yPos;
	double prevXPos = player->xPos;

	player->xPos += (player->xVelocity * deltaTime);


	Object *currentObject;
	currentObject = gameWorld->objectList->firstObject;

	if (currentObject == NULL)
	{
		return 0;
	}

	double objX, objY, objX2, objY2, prevObjXCenter;
	int count = 0;
	int result = 0;


	while (currentObject != NULL && count < 16)
	{
		currentObject = OverlappingObject(player, gameWorld);

		count++;

		if (currentObject == NULL)
		{
			return 0;
		}


		objX = currentObject->xPos;
		objY = currentObject->yPos;
		objX2 = objX + currentObject->xSize;
		objY2 = objY + currentObject->ySize;
		prevObjXCenter = ((objX + objX2) / 2.0) - (currentObject->xVel * deltaTime);

		switch(currentObject->solid)
		{
			// Y = X * (ySize/xSize)
			case 2:
			{
				double slope = ((double)currentObject->ySize/(double)currentObject->xSize);

				if (prevXPos >= prevObjXCenter)
				{
					player->xPos = objX2;
				}		

			} break;

			// Y = (xSize - X) * (ySize/xSize)
			case 3:
			{
				double slope = ((double)currentObject->ySize/(double)currentObject->xSize);

				if (prevXPos < prevObjXCenter)
				{
					player->xPos = objX - PLAYERWIDTH;
				}

			} break;


			case 1:
			{
				if (prevXPos < prevObjXCenter)
				{
					player->xPos = objX - PLAYERWIDTH;
				}
				else
				{
					player->xPos = objX2;
				}

			} break;


			default:
				break;
		}

		ApplyXPhysics(player, currentObject);
	}

	player->yPos = prevYPos;

	return 0;
}


int ApplyXPhysics(PlayerData *player, Object *inputObject)
{
	if (inputObject->layer > FOREGROUND)
	{
		return 0;
	}

	if (player->xVelocity > 0.0)
	{
		if (inputObject->xVel > 0.1)
		{
			player->xVelocity = inputObject->xVel;
		}
		else
		{
			if (inputObject->solid == 2)
			{
				return 0;
			}

			player->PhysicsXVelocity = 0.0;
			player->xVelocity = 0.0;
		}

		return 0;
	}

	if (player->xVelocity < 0.0)
	{
		if (inputObject->xVel < -0.1)
		{
			player->xVelocity = inputObject->xVel;
		}
		else
		{		
			if (inputObject->solid == 3)
			{
				return 0;
			}

			player->PhysicsXVelocity = 0.0;
			player->xVelocity = 0.0;
		}

		return 0;
	}

	return 0;
}




int MovePlayerY(PlayerData *player, World *gameWorld, double deltaTime)
{
	double prevYPos = player->yPos;
	double prevXPos = player->xPos;

	player->yPos += (player->yVelocity * deltaTime);


	Object *currentObject;
	currentObject = gameWorld->objectList->firstObject;

	if (currentObject == NULL)
	{
		return 0;
	}

	double objX, objY, objX2, objY2, prevObjYCenter;
	int count = 0;
	int result = 0;


	while (currentObject != NULL && count < 16)
	{
		currentObject = OverlappingObject(player, gameWorld);

		count++;

		if (currentObject == NULL)
		{
			return 0;
		}


		objX = currentObject->xPos;
		objY = currentObject->yPos;
		objX2 = objX + currentObject->xSize;
		objY2 = objY + currentObject->ySize;
		prevObjYCenter = ((objY + objY2) / 2.0) - (currentObject->yVel * deltaTime);

		switch(currentObject->solid)
		{
			// Y = X * (ySize/xSize)
			case 2:
			{
				if (prevYPos < prevObjYCenter)
				{
					player->yPos = (objY - PLAYERHEIGHT);
					player->jumpProgress = 100;
				}
				else
				{
					// If player is halfway off edge, floor of slope continues to be calculated as Y = X * slope
					// So here it is reset to the expected maximum if it over
					double slope = ((double)currentObject->ySize/(double)currentObject->xSize);

					double slopeFloor = ((player->xPos + PLAYERWIDTH - objX) * slope);

					if (slopeFloor > currentObject->ySize)
					{
						slopeFloor = currentObject->ySize;
					}

					player->yPos = slopeFloor + objY;

					player->jumpProgress = 0;
				}		

			} break;

			// Y = (xSize - X) * (ySize/xSize)
			case 3:
			{
				if (prevYPos < prevObjYCenter)
				{
					player->yPos = (objY - PLAYERHEIGHT);
					player->jumpProgress = 100;
				}
				else
				{
					// If player is halfway off edge, floor of slope continues to be calculated as Y = xSize - X * slope
					// So here it is reset to the expected maximum if it over
					double slope = ((double)currentObject->ySize/(double)currentObject->xSize);

					double slopeFloor = ((currentObject->xSize - (player->xPos - objX)) * slope);

					if (slopeFloor > currentObject->ySize)
					{
						slopeFloor = currentObject->ySize;
					}

					player->yPos = slopeFloor + objY;

					player->jumpProgress = 0;
				}

			} break;


			case 4:
			{
				if (player->yVelocity < 0.1 && prevYPos >= prevObjYCenter && player->crouch < 1)
				{
					player->yPos = objY2;
					player->jumpProgress = 0;
				}

			} break;


			case 1:
			{
				if (prevYPos < prevObjYCenter)
				{
					player->yPos = (objY - PLAYERHEIGHT);
					player->jumpProgress = 100;
				}
				else
				{
					player->yPos = objY2;
					player->jumpProgress = 0;
				}

			} break;


			default:
				break;
		}

		ApplyYPhysics(player, currentObject);
	}

	player->xPos = prevXPos;

	return 0;








	currentObject = gameWorld->objectList->firstObject;

	if (currentObject == NULL)
	{
		return -1;
	}



	while (currentObject != NULL)
	{
		if (currentObject->layer > FOREGROUND)
		{
			goto Skip_YCollision;
		}

		objX = currentObject->xPos;
		objY = currentObject->yPos;
		objX2 = objX + currentObject->xSize;
		objY2 = objY + currentObject->ySize;

		result = overlapsBox(player, objX, objX2, objY, objY2);


		switch(currentObject->solid * result)
		{
			// Y = X * (ySize/xSize)
			case 2:
			{
				double slope = ((double)currentObject->ySize/(double)currentObject->xSize);

				result = overlapsRightSlope(player, currentObject, slope);

				// Check for overlap
				if (result == 0)
				{
					break;
				}

				// Attempt to resolve to top of slope
				if (slope < 2.0)
				{
					for (int i = 0; result == 1 && i < 24; i++)
					{
						player->yPos += 1.0;
						result = overlapsRightSlope(player, currentObject, slope);
					}

					player->yPos -= 24.0 * result;
				}

				if (result == 0)
				{
					if (player->yVelocity + player->PhysicsYVelocity > 0.0)
					{
						player->jumpProgress = 100;
					}
					else
					{
						player->jumpProgress = 0;
					}
					
					ApplyYPhysics(player, currentObject);
					break;
				}

				// If could not resolve, perform regular collision
				if (player->yVelocity + player->PhysicsYVelocity > 0.0)
				{
					player->yPos = (objY - PLAYERHEIGHT);
					player->jumpProgress = 100;
				}

				if (player->yVelocity  + player->PhysicsYVelocity < 0.0)
				{
					// If player is halfway off edge, floor of slope continues to be calculated as Y = X * slope
					// So here it is reset to the expected maximum if it over
					int slopeFloor = (((int)player->xPos + PLAYERWIDTH - objX) * slope);

					if (slopeFloor > currentObject->ySize)
					{
						slopeFloor = currentObject->ySize;
					}

					player->yPos = slopeFloor + objY;

					player->jumpProgress = 0;
				}

				ApplyYPhysics(player, currentObject);

			} break;

			// Y = (xSize - X) * (ySize/xSize)
			case 3:
			{
				double slope = ((double)currentObject->ySize/(double)currentObject->xSize);

				result = overlapsLeftSlope(player, currentObject, slope);

				// Check for overlap
				if (result == 0)
				{
					break;
				}

				// Attempt to resolve to top of slope
				if (slope < 2.0)
				{
					for (int i = 0; result == 1 && i < 24; i++)
					{
						player->yPos += 1.0;
						result = overlapsLeftSlope(player, currentObject, slope);
					}

					player->yPos -= 24.0 * result;
				}

				if (result == 0)
				{
					if (player->yVelocity + player->PhysicsYVelocity > 0.0)
					{
						player->jumpProgress = 100;
					}
					else
					{
						player->jumpProgress = 0;
					}
					
					ApplyYPhysics(player, currentObject);
					break;
				}

				
				if (player->yVelocity + player->PhysicsYVelocity > 0.0)
				{
					player->yPos = (objY - PLAYERHEIGHT);
					player->jumpProgress = 100;
				}

				if (player->yVelocity + player->PhysicsYVelocity < 0.0)
				{
					// If player is halfway off edge, floor of slope continues to be calculated as Y = xSize - X * slope
					// So here it is reset to the expected maximum if it over
					int slopeFloor = ((currentObject->xSize - ((int)player->xPos - objX)) * slope);

					if (slopeFloor > currentObject->ySize)
					{
						slopeFloor = currentObject->ySize;
					}

					player->yPos = slopeFloor + objY;

					player->jumpProgress = 0;
				}

				ApplyYPhysics(player, currentObject);
				

			} break;

			// Jump-through platforms should only collide when above it and travelling downwards
			case 4:
			{
				if (player->yVelocity < 0.0 && player->yPos - objY > (currentObject->ySize >> 2) && player->crouch < 1)
				{
					player->yPos += (objY2 - player->yPos);
					player->jumpProgress = 0;

					
					ApplyYPhysics(player, currentObject);
				}

			} break;

			// Non-solid objects
			case 0:
				break;

			default:
			{
				if (player->yVelocity + player->PhysicsYVelocity > 0.0)
				{
					player->yPos += (objY - player->yPos - PLAYERHEIGHT);
					player->jumpProgress = 100;
				}

				if (player->yVelocity + player->PhysicsYVelocity < 0.0)
				{
					player->yPos += (objY2 - player->yPos);
					player->jumpProgress = 0;
				}

				player->xPos = prevXPos;

				ApplyYPhysics(player, currentObject);
			} break;


		}

	Skip_YCollision:

		currentObject = currentObject->nextObject;
	}

	return result;
}


int ApplyYPhysics(PlayerData *player, Object *inputObject)
{
	if (inputObject->layer > FOREGROUND)
	{
		return 0;
	}

	if (player->yVelocity > 0.0)
	{
		if (inputObject->yVel > 0.0)
		{
			player->yVelocity = inputObject->yVel;
		}
		else
		{
			player->PhysicsYVelocity = 0.0;
			player->yVelocity = 0.0;
		}

		return 0;
	}

	if (player->yVelocity < 0.0)
	{
		if (inputObject->yVel < 0.0)
		{
			player->yVelocity = inputObject->yVel;
		}
		else
		{			
			player->PhysicsYVelocity = 0.0;
			player->yVelocity = 0.0;
		}

		return 0;
	}

	return 0;
}


// returns pointer of object overlapping, NULL if no object is detected
Object* OverlappingObject(PlayerData *player, World *gameWorld)
{
	Object *currentObject;
	currentObject = gameWorld->objectList->firstObject;

	if (currentObject == NULL)
	{
		return NULL;
	}

	int objX, objXRight, objY, objYTop;

	while (currentObject != NULL)
	{
		objX = currentObject->xPos;
		objY = currentObject->yPos;
		objXRight = objX + currentObject->xSize;
		objYTop = objY + currentObject->ySize;

		int result = overlapsBox(player, objX, objXRight, objY, objYTop);

		if (result == 0 || currentObject->layer > FOREGROUND)
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		switch(currentObject->solid)
		{
			case 2:
			{
				double slope = (double)currentObject->ySize/(double)currentObject->xSize;

				if (overlapsRightSlope(player, currentObject, slope) == 1)
				{
					return currentObject;
				}
			} break;

			case 3: 
			{
				double slope = (double)currentObject->ySize/(double)currentObject->xSize;

				if (overlapsLeftSlope(player, currentObject, slope) == 1)
				{
					return currentObject;
				}
			} break;

			case 1:
				return currentObject;
				break;
			
			default:
				break;
		}


		currentObject = currentObject->nextObject;

	}

	return NULL;
}


int overlapsBox(PlayerData *player, double X1, double X2, double Y1, double Y2)
{
	return !(player->xPos >= X2 || (player->xPos + PLAYERWIDTH) <= X1 || player->yPos >= Y2 || (player->yPos + PLAYERHEIGHT) <= Y1);
}


int overlapsRightSlope(PlayerData *player, Object *inputObject, double slope)
{
	return 0;
	double slopeFloor = ((player->xPos + PLAYERWIDTH - inputObject->xPos) * slope);

	if (slopeFloor > inputObject->ySize)
	{
		slopeFloor = inputObject->ySize;
	}

	return!((player->yPos - inputObject->yPos) >= slopeFloor);
}


int overlapsLeftSlope(PlayerData *player, Object *inputObject, double slope)
{
	return 0;
	double slopeFloor = ((inputObject->xSize - (player->xPos - inputObject->xPos)) * slope);

	if (slopeFloor > inputObject->ySize)
	{
		slopeFloor = inputObject->ySize;
	}

	return !((player->yPos - inputObject->yPos) >= slopeFloor);
}


int overlapsBoxAtFeet(PlayerData *player, double X1, double X2, double Y1, double Y2)
{
	return !(player->xPos >= X2 || (player->xPos + PLAYERWIDTH) <= X1 || (player->yPos - 8) >= Y2 || (player->yPos + 8) <= Y1);
}


int checkIfGrounded(World *gameWorld, PlayerData *player)
{
	Object *currentObject;
	currentObject = gameWorld->objectList->firstObject;

	int i = 0;
	int result = 0;

	while (currentObject != NULL || i > gameWorld->objectList->objectCount)
	{
		if (currentObject->layer > FOREGROUND)
		{
			currentObject = currentObject->nextObject;
			i++;
			continue;
		}

		double objX = currentObject->xPos;
		double objY = currentObject->yPos;
		double objX2 = objX + currentObject->xSize;
		double objY2 = objY + currentObject->ySize;

		player->yPos -= 2;

		result = overlapsBoxAtFeet(player, objX, objX2, objY, objY2);

		switch (currentObject->solid * result)
		{
		case 2:
			{
				result = overlapsRightSlope(player, currentObject, (double)currentObject->ySize/(double)currentObject->xSize);
			} break;

		case 3:
			{
				result = overlapsLeftSlope(player, currentObject, (double)currentObject->ySize/(double)currentObject->xSize);
			} break;

		case 0:
			{
				result = 0;
				break;
			}
		}

		player->yPos += 2;

		if (result > 0)
		{
			if (player->inAir > 0 && fabs(player->PhysicsXVelocity) > 0.1)
			{
				player->xVelocity = 0.0;
			}

			player->PhysicsXVelocity = currentObject->xVel;
			player->PhysicsYVelocity = currentObject->yVel;

			assignDirection(player, currentObject);

			return 1;
		}


		currentObject = currentObject->nextObject;
		i++;
	}

	player->direction = 1.5707963268;

	if (player->inAir == 0 && fabs(player->xVelocity) > 0.1)
	{
		player->PhysicsXVelocity = 0.0;
	}

	return 0;

}


int assignDirection(PlayerData *player, Object *currentObject)
{
	if (currentObject == NULL)
	{
		player->direction = 1.5707963268;
		return 0;
	}

	double slope = (double)currentObject->ySize/(double)currentObject->xSize;

	switch (currentObject->solid)
	{
			case 2:
				player->direction = (1.5707963268 - atan(slope));
				break;

			case 3:
				player->direction = (1.5707963268 + atan(slope));
				break;

			default:
				player->direction = 1.5707963268;
				break;
	}

	return 0;
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


// Depreciated tile-based collision - unlikely to be reimplemented
//int getTileAtPosition(World *gameWorld, int x, int y)
//{
//	int gridX, gridY = 0;
//
//	gridX = floor(x / X_TILESCALE);
//	gridY = floor(y / Y_TILESCALE);
//
//	if (y < 0 || x < 0)
//	{
//		return 0;
//	}
//
//	return gameWorld->levelData[gridX][gridY];
//}
//
//
//int tileCollision(PlayerData *player, World *gameWorld, double dx, double dy)
//{
//
//	// Feet - 0
//	fixCollisionAtPoint(player, gameWorld, player->xPos + PLAYERWIDTH, player->yPos, dx, dy, 0);
//	fixCollisionAtPoint(player, gameWorld, player->xPos, player->yPos, dx, dy, 0);
//
//
//
//	// Middle - 1
//	if (PLAYERHEIGHT > Y_TILESCALE)
//	{
//		fixCollisionAtPoint(player, gameWorld, player->xPos, player->yPos + (PLAYERHEIGHT >> 1), dx, dy, 1);
//		fixCollisionAtPoint(player, gameWorld, player->xPos + PLAYERWIDTH, player->yPos + (PLAYERHEIGHT >> 1), dx, dy, 1);
//	}
//
//
//	// Head - 2
//	fixCollisionAtPoint(player, gameWorld, player->xPos, (player->yPos + PLAYERHEIGHT - 1), dx, dy, 2);
//	fixCollisionAtPoint(player, gameWorld, (player->xPos + PLAYERWIDTH), (player->yPos + PLAYERHEIGHT - 1), dx, dy, 2);
//
//	return 0;
//}
//
//
//int fixCollisionAtPoint(PlayerData *player, World *gameWorld, int x, int y, double dx, double dy, int part)
//{
//	int tileID = getTileAtPosition(gameWorld, x, y);
//
//
//	if (tileID == 0)
//	{
//		if (part == 0 && player->jumpProgress < 99)
//		{
//			player->jumpProgress += 1;
//		}
//
//		return 0;
//	}
//
//	int shape = gameWorld->tileShape[tileID];
//
//	int modX = modulo(x, X_TILESCALE);
//	int modY = modulo(y, Y_TILESCALE);
//	int baseX = X_TILESCALE;
//	int baseY = Y_TILESCALE;
//
//	switch (shape)
//	{
//		case '=':
//			if (part != 0 || dy > 0.2)
//			{
//				return 0;
//			}
//
//			break;
//
//		case '_':
//			baseY = baseY / 2 + 1;
//
//			if (modY >= baseY)
//			{
//				return 0;
//			}
//
//			break;
//
//
//		case '/':
//			if (modY - modX > 0)
//			{
//				return 0;
//			}
//
//			baseY -= (X_TILESCALE - modX);
//
//			if (dx > 0.5)
//			{
//				modX -= (modY + 1);
//			}
//
//			break;
//
//		case 92:
//			if ((Y_TILESCALE - modY) - modX > 0)
//			{
//				return 0;
//			}
//
//			baseY -= modX;
//
//			break;
//
//		default:
//			break;
//	}
//
//	if (dx > 0.5)
//	{
//		player->xPos += -modX - 1;
//		player->xVel = 0.0;
//	}
//
//	if (dx < -0.5)
//	{
//		player->xPos += (baseX - modX);
//		player->xVel = 0.0;
//	}
//
//
//	if (dy > 0.2)
//	{
//		player->yPos += -modY - 1;
//		player->yVel = 0.0;
//		player->jumpProgress = 99;
//	}
//
//	if (dy < -0.2)
//	{
//		player->yPos += (baseY - modY);
//		player->yVel = 0.0;
//
//		if (part == 0)
//		{
//			player->jumpProgress = 0;
//		}
//	}
//
//	return 0;
//}
