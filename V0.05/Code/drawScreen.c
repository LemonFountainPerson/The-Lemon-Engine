#include "drawScreen.h"


// Controls what symbol is printed for each tile
static uint32_t tileMap[32] = {0x00AA00AA, 0xFFFF0088, 0xFF0088FF, 0xFF552299, 0xFF00AA00, 0xFFBB55AA, 0xFFD0CC00, 0xFF88FF44,
								0xFF77DD11, 0xFF11BB44, 0xFF0F449F, 0xFF009070, 0xFF006010, 0xFF701010, 0xFF1C1010, 0xFF101C10,
								0xFF2C1010, 0xFF102C10, 0xFF10103C, 0xFF40201C, 0xFF019E30, 0xFF0514CE, 0xFFE86AA3, 0xFF0A4321,
								0xFF2C1010, 0xFF102C10, 0xFF10103C, 0xFF40201C, 0xFF019E30, 0xFF0514CE, 0xFFE86AA3, 0xFF0A4321};



int drawPlayer(uint32_t *screen, World *gameWorld)
{
	if (gameWorld == NULL || gameWorld->Player == NULL || gameWorld->Player->PlayerDisplay == NULL)
	{
		return MISSING_DATA;
	}

	if (gameWorld->drawPlayer == 0)
	{
		return ACTION_DISABLED;
	}

	PlayerData *player;
	player = gameWorld->Player;

	DisplayData *PlayerDisplay = player->PlayerDisplay;

	// Find offsets of player relative to center of screen/camera
	int xOffset = player->PlayerBox->xPos - gameWorld->CameraX + (screenWidth >> 1);
	int yOffset = player->PlayerBox->yPos - gameWorld->CameraY + (screenHeight >> 1);


	// Load clamp sprite
	Sprite *spritePtr;
	spritePtr = PlayerDisplay->spriteBuffer;


	if (spritePtr != NULL && gameWorld->drawPlayer == 1 && gameWorld->drawSprites == 1)
	{
		renderSpriteInRenderMode(screen, PlayerDisplay, xOffset, yOffset, player->PlayerBox);
	}
	
	// Player Hitbox
	if (gameWorld->drawHitboxes == 0)
	{
		return ACTION_DISABLED;
	}

	// clamp Offsets
	int xDraw = clamp(xOffset, 0, screenWidth - 1);
	int yDraw = clamp(yOffset, 0, screenHeight - 1);
	int xDraw2 = clamp(player->PlayerBox->xSize + xOffset, 0, screenWidth - 1);
	int yDraw2 = clamp(player->PlayerBox->ySize + yOffset, 0, screenHeight - 1);

	for (int i = xDraw; i < xDraw2; i++)
	{
		for (int j = yDraw; j < yDraw2; j++)
		{
			screen[(j * screenWidth) + i] = 0xFFFFFFFF;
		}
	}


	xOffset = player->InteractBox->xPos - gameWorld->CameraX + (screenWidth >> 1);
	yOffset = player->InteractBox->yPos - gameWorld->CameraY + (screenHeight >> 1);

	xDraw = clamp(xOffset, 0, screenWidth - 1);
	yDraw = clamp(yOffset, 0, screenHeight - 1);
	xDraw2 = clamp(player->InteractBox->xSize + xOffset, 0, screenWidth - 1);
	yDraw2 = clamp(player->InteractBox->ySize + yOffset, 0, screenHeight - 1);

	for (int i = xDraw; i < xDraw2; i++)
	{
		for (int j = yDraw; j < yDraw2; j++)
		{
			screen[(j * screenWidth) + i] = 0xFF99FF22;
		}
	}


	return 0;
}


int WorldCameraControl(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->Player == NULL || GameWorld->GameState == EMPTY_GAME)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}


	// Control latching/buffering
	switch(GameWorld->CameraMode)
	{
		case MENU_CAMERA:
		{
			if (GameWorld->CameraLatch == 0)
			{
				GameWorld->CameraXBuffer = GameWorld->CameraX;
				GameWorld->CameraYBuffer = GameWorld->CameraY;
				GameWorld->CameraX = -32000;
				GameWorld->CameraY = 0;
				GameWorld->CameraLatch = 1;
			}
		} break;

		default:
		{
			if (GameWorld->CameraLatch == 1)
			{
				GameWorld->CameraX = GameWorld->CameraXBuffer;
				GameWorld->CameraY = GameWorld->CameraYBuffer;
				GameWorld->CameraLatch = 0;
			}
		} break;
	}


	// Move camera
	switch(GameWorld->CameraMode)
	{
		case FOLLOW_PLAYER:
		{
			int xDifference = (int)GameWorld->Player->PlayerBox->xPos - GameWorld->CameraX;

			GameWorld->CameraX += (int)(xDifference / 5) + 1;

			if (abs(xDifference) < 1)
			{
				GameWorld->CameraX = (int)GameWorld->Player->PlayerBox->xPos;
			}
			

			int yOffset = (int)GameWorld->Player->PlayerBox->yPos - GameWorld->CameraY;

			if (yOffset >= (screenHeight * 0.1))
			{
				int difference = yOffset - (screenHeight * 0.1);

				GameWorld->CameraY += (int)(difference / 5) + 1;

				if (abs(difference) < 5)
				{
					GameWorld->CameraY = (int)GameWorld->Player->PlayerBox->yPos - (screenHeight * 0.1);
				}
			}

			if (yOffset < (screenHeight * -0.2))
			{
				int difference = yOffset + (screenHeight * 0.2);

				GameWorld->CameraY += (int)(difference / 5) + 1;

				if (abs(difference) < 5)
				{
					GameWorld->CameraY = (int)GameWorld->Player->PlayerBox->yPos + (screenHeight * 0.2);
				}
			}

			restrictCameraToBounds(GameWorld);
		} break;


		case FREE_ROAM_RESTRICTED:
		{
			restrictCameraToBounds(GameWorld);
		} break;


		default:
		break;
	}


	return 0;
}

int restrictCameraToBounds(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->GameState == EMPTY_GAME)
	{
		return MISSING_DATA;
	}

	if (GameWorld->maxCameraX < GameWorld->minCameraX + screenWidth)
	{
		GameWorld->maxCameraX = GameWorld->minCameraX + screenWidth;
	}

	if (GameWorld->maxCameraY < GameWorld->minCameraY + screenHeight)
	{
		GameWorld->maxCameraY = GameWorld->minCameraY + screenHeight;
	}

	if (GameWorld->minCameraX < 0)
	{
		GameWorld->minCameraX = 0;
	}

	if (GameWorld->minCameraY < 0)
	{
		GameWorld->minCameraY = 0;
	}

	if (GameWorld->CameraX < GameWorld->minCameraX)
	{
		GameWorld->CameraX = GameWorld->minCameraX;
	}

	if (GameWorld->CameraY < GameWorld->minCameraY)
	{
		GameWorld->CameraY = GameWorld->minCameraY;
	}

	if (GameWorld->CameraX > GameWorld->maxCameraX - screenWidth)
	{
		GameWorld->CameraX = GameWorld->maxCameraX - screenWidth;
		
	}

	if (GameWorld->CameraY + screenHeight > GameWorld->maxCameraY)
	{
		GameWorld->CameraY = GameWorld->maxCameraY - screenHeight;
	}

	return 0;
}


int drawObjects(uint32_t *screen, World *gameWorld)
{
	if (gameWorld == NULL || gameWorld->ObjectList == NULL || gameWorld->GameState == EMPTY_GAME)
	{
		return MISSING_DATA;
	}

	Object *currentObject;

	for (Layer drawLayer = BACKGROUND; drawLayer < UNDEFINED_LAYER; drawLayer++)
	{
		currentObject = gameWorld->ObjectList->firstObject;

		int i = gameWorld->ObjectList->objectCount;


		while(currentObject != NULL && i > 0)
		{
			// Drawing routine - max objects on screen is defined as Macro
			renderObjectSprite(screen, gameWorld, currentObject, drawLayer);

			if (gameWorld->drawHitboxes == 1 && currentObject->ObjectID != UI_ELEMENT)
			{
				renderObjectHitbox(screen, gameWorld, currentObject, drawLayer);
			}
		
			currentObject = currentObject->nextObject;
			i--;
		}


		if (gameWorld->Player != NULL && gameWorld->Player->PlayerLayer == drawLayer)
		{
			drawPlayer(screen, gameWorld);
		}
	}

	return 0;
}


int renderObjectHitbox(uint32_t *screen, World *gameWorld, Object *currentObject, Layer drawLayer)
{
	if (currentObject == NULL)
	{
		return MISSING_DATA;
	}

	if (drawLayer != currentObject->layer || currentObject->ObjectBox->xSize < 0 || currentObject->ObjectBox->ySize < 0 || gameWorld->drawHitboxes == 0)
	{
		return ACTION_DISABLED;
	}


	int tile = tileMap[currentObject->ObjectID % 32];

	// Hitbox
	switch (currentObject->ObjectBox->solid)
	{
		case FLAT_SLOPE:
		{
			// clamp Offsets
			double ySize = currentObject->ObjectBox->ySize;
			double xSize = currentObject->ObjectBox->xSize;

			int xOffset = (int)currentObject->ObjectBox->xPos - gameWorld->CameraX + (screenWidth >> 1);
			int yOffset = (int)currentObject->ObjectBox->yPos - gameWorld->CameraY + (screenHeight >> 1);
			int xDraw = clamp(xOffset, 0, screenWidth - 1);
			int yDraw = clamp(yOffset, 0, screenHeight - 1);
			int xDraw2 = clamp(xOffset + currentObject->ObjectBox->xSize, 0, screenWidth - 1);
			int yDraw2 = clamp(yOffset + currentObject->ObjectBox->ySize, 0, screenHeight - 1);

			double slope = ((double)ySize/(double)xSize);

			if (currentObject->ObjectBox->xFlip == 1)
			{
				for (int i = yDraw; i < yDraw2; i++)
				{
					int row = (i * screenWidth);

					for (int k = xDraw2; k > xDraw && k - xOffset > (i - yOffset) / slope; k--)
					{
						screen[row + k] = tile;
					}
				}
			}
			else
			{
				for (int i = yDraw; i < yDraw2; i++)
				{
					int row = (i * screenWidth);

					for (int k = xDraw; k < xDraw2 && k - xOffset < xSize - ((i - yOffset) / slope); k++)
					{
						screen[row + k] = tile;
					}
				}
			}

			
		} break;


		default:
		{

			// clamp Offsets
			int xOffset = (int)currentObject->ObjectBox->xPos - gameWorld->CameraX + (screenWidth >> 1);
			int yOffset = (int)currentObject->ObjectBox->yPos - gameWorld->CameraY + (screenHeight >> 1);
			int xDraw = clamp(xOffset, 0, screenWidth - 1);
			int yDraw = clamp(yOffset, 0, screenHeight - 1);
			int xDraw2 = clamp(xOffset + currentObject->ObjectBox->xSize, 0, screenWidth - 1);
			int yDraw2 = clamp(yOffset + currentObject->ObjectBox->ySize, 0, screenHeight - 1);

			for (int i = yDraw; i < yDraw2; i++)
			{
				int row = (i * screenWidth);

				for (int k = xDraw; k < xDraw2; k++)
				{
					screen[row + k] = tile;
				}
			}
		} break;
	}
	

	return 0;
}



int renderObjectSprite(uint32_t *screen, World *gameWorld, Object *currentObject, Layer drawLayer)
{
	// WARNING! This function is really messy and not easy to read: All variations of rendering has its own copy of the
	// main render loop, even extremely similar ones. This was done to squeeze out performance (at the expense of a bit of memory)
	// As such, readability was sacrificed somewhat. Continue at your own risk!

	if (currentObject == NULL || gameWorld == NULL || currentObject->ObjectDisplay == NULL || currentObject->ObjectDisplay->spriteBuffer == NULL)
	{
		return MISSING_DATA;
	}

	if (currentObject->layer != drawLayer || gameWorld->drawSprites == 0 || currentObject->ObjectDisplay->RenderModeOverride == DO_NOT_RENDER)
	{
		return ACTION_DISABLED;
	}

	switch(currentObject->ObjectID)
	{
		case UI_ELEMENT:
			if (gameWorld->drawUI == 0 || gameWorld->drawnHudElements >= MAX_HUD_ELEMENTS_RENDER)
			{
				return ACTION_DISABLED;
			}
			break;


		case PARTICLE:
			if (gameWorld->drawParticles == 0 || gameWorld->drawnParticles >= MAX_PARTICLES_RENDER)
			{
				return ACTION_DISABLED;
			}
			break;


		default:
			if (gameWorld->drawObjects == 0 || gameWorld->drawnObjects >= MAX_OBJECTS_RENDER)
			{
				return ACTION_DISABLED;
			}
			break;
	}


	// Locate object on screen
	int xOffset = 0;
	int	yOffset = 0;

	if (currentObject->ObjectID == UI_ELEMENT)
	{
		xOffset = currentObject->ObjectBox->xPos;
		yOffset = currentObject->ObjectBox->yPos;
	}
	else
	{
		xOffset = currentObject->ObjectBox->xPos - gameWorld->CameraX + (screenWidth >> 1);
		yOffset = currentObject->ObjectBox->yPos - gameWorld->CameraY + (screenHeight >> 1);
	}

	// Render
	int renderResult = renderSpriteInRenderMode(screen, currentObject->ObjectDisplay, xOffset, yOffset, currentObject->ObjectBox);
	
	if (renderResult != LEMON_SUCCESS)
	{
		return EXECUTION_UNNECESSARY;
	}


	switch(currentObject->ObjectID)
	{
		case UI_ELEMENT:
			gameWorld->drawnHudElements++;
			break;


		case PARTICLE:
			gameWorld->drawnParticles++;
			break;


		default:
			gameWorld->drawnObjects++;
			break;
	}

	return 0;
}


int renderSpriteInRenderMode(uint32_t *screen, DisplayData *inputData, int realXOffset, int realYOffset, PhysicsRect *inputBox)
{
	if (screen == NULL || inputData == NULL || inputBox == NULL || inputData->spriteBuffer == NULL)
	{
		return MISSING_DATA;
	}

	Sprite *spritePtr = inputData->spriteBuffer;
	RenderMode inputRenderMode = inputData->RenderModeOverride;

	if (inputRenderMode == DEFAULT_TO_SPRITE)
	{
		inputRenderMode = spritePtr->RenderMode;
	}


	realXOffset += inputBox->spriteXOffset;
	realYOffset += inputBox->spriteYOffset;
	int xOffset2 = realXOffset + inputBox->xSize;
	int yOffset2 = realYOffset + inputBox->ySize;

	int xDraw2, xDraw, yDraw2, yDraw;

	// Set draw locations on screen
	if (inputRenderMode == SINGLE || inputRenderMode == SINGLE_FULL_ALPHA || inputRenderMode == SINGLE_FAST)
	{
		// Find center of object and then center sprite on that point
		int centerX = (xOffset2 + realXOffset) >> 1;
		int centerY = (yOffset2 + realYOffset) >> 1;
		realXOffset = centerX - (spritePtr->width >> 1);
		realYOffset = centerY - (spritePtr->height >> 1);
		xOffset2 = centerX + (spritePtr->width >> 1);
		yOffset2 = centerY + (spritePtr->height >> 1);

		if (realXOffset >= screenWidth || xOffset2 < 0 || realYOffset >= screenHeight || yOffset2 < 0)
		{
			return INVALID_DATA;
		}

		xDraw = clamp(centerX - (spritePtr->width >> 1), 0, screenWidth - 1);
		yDraw = clamp(centerY - (spritePtr->height >> 1), 0, screenHeight - 1);
		xDraw2 = clamp(centerX + (spritePtr->width >> 1), 0, screenWidth - 1);
		yDraw2 = clamp(centerY + (spritePtr->height >> 1), 0, screenHeight - 1);
	}
	else
	{
		if (realXOffset >= screenWidth || xOffset2 < 0 || realYOffset >= screenHeight || yOffset2 < 0)
		{
			return INVALID_DATA;
		}

		xDraw = clamp(realXOffset, 0, screenWidth - 1);
		yDraw = clamp(realYOffset, 0, screenHeight - 1);
		xDraw2 = clamp(inputBox->xSize + realXOffset, 0, screenWidth - 1);
		yDraw2 = clamp(inputBox->ySize + realYOffset, 0, screenHeight - 1);
	}



	// Render sprite to screen
	switch(inputRenderMode)
	{
		case SINGLE:
		case TILE:
		{
			if (inputBox->xFlip == 1 && inputBox->yFlip == 1)
			{
				renderSprite_LRUD_Tile(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else if (inputBox->xFlip == -1 && inputBox->yFlip == 1)
			{
				renderSprite_RLUD_Tile(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else if (inputBox->xFlip == 1 && inputBox->yFlip == -1)
			{
				renderSprite_LRDU_Tile(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else
			{
				renderSprite_RLDU_Tile(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}

			return 0;
		} break;


		case TILE_FULL_ALPHA:
		case SINGLE_FULL_ALPHA:
		{
			if (inputBox->xFlip == 1 && inputBox->yFlip == 1)
			{
				renderSprite_LRUD_FullAlpha(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else if (inputBox->xFlip == -1 && inputBox->yFlip == 1)
			{
				renderSprite_RLUD_FullAlpha(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else if (inputBox->xFlip == 1 && inputBox->yFlip == -1)
			{
				renderSprite_LRDU_FullAlpha(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else
			{
				renderSprite_RLDU_FullAlpha(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}

			return 0;
		} break;


		case SINGLE_FAST:
		case TILE_FAST:
		{
			if (inputBox->xFlip == 1 && inputBox->yFlip == 1)
			{
				renderSprite_LRUD_TileFast(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else if (inputBox->xFlip == -1 && inputBox->yFlip == 1)
			{
				renderSprite_RLUD_Tile(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else if (inputBox->xFlip == 1 && inputBox->yFlip == -1)
			{
				renderSprite_LRDU_TileFast(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else
			{
				renderSprite_RLDU_Tile(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}

			return 0;
		} break;


		case SCALE:
		{
			if (inputBox->xFlip == 1 && inputBox->yFlip == 1)
			{
				renderSprite_LRUD_Scale(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputBox);
			}
			else if (inputBox->xFlip == -1 && inputBox->yFlip == 1)
			{
				renderSprite_RLUD_Scale(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputBox);
			}
			else if (inputBox->xFlip == 1 && inputBox->yFlip == -1)
			{
				renderSprite_LRDU_Scale(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputBox);
			}
			else
			{
				renderSprite_RLDU_Scale(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputBox);
			}

			return 0;
		} break;


		case SCALE_FULL_ALPHA:
		{
			if (inputBox->xFlip == 1 && inputBox->yFlip == 1)
			{
				renderSprite_LRUD_Scale_Full_Alpha(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputBox);
			}
			else if (inputBox->xFlip == -1 && inputBox->yFlip == 1)
			{
				renderSprite_RLUD_Scale_Full_Alpha(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputBox);
			}
			else if (inputBox->xFlip == 1 && inputBox->yFlip == -1)
			{
				renderSprite_LRDU_Scale_Full_Alpha(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputBox);
			}
			else
			{
				renderSprite_RLDU_Scale_Full_Alpha(screen, spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputBox);
			}

			return 0;
		} break;

		default:
		return INVALID_DATA;
		break;

	}


	return 0;
}


int renderSprite_LRUD_FullAlpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	uint32_t hexValue;

	// Start render loop
	int pixely = spritePtr->height - 1 - ((yDraw - yOffset) % spritePtr->height);

	int startPixelx = ((xDraw - xOffset) % spritePtr->width);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * spritePtr->width);
		int screenYOffset = j * screenWidth;

		for (int i = xDraw; i < xDraw2; i++)
		{
			hexValue = spritePtr->spriteData[pixelYOffset + (pixelx << 2) + 3] << 24 | 
						spritePtr->spriteData[pixelYOffset + (pixelx << 2) + 2] << 16 | 
						spritePtr->spriteData[pixelYOffset + (pixelx << 2) + 1] << 8 | 
						spritePtr->spriteData[pixelYOffset + (pixelx << 2) ];

			if (hexValue >> 24 == 0xFF)
			{
				screen[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screen[screenYOffset + i] = blendPixel(screen[screenYOffset + i], hexValue);
			}
			

			pixelx++;

			if (pixelx >= spritePtr->width)
			{
				pixelx = 0;
			}
		}

		pixely--;

		if (pixely < 0)
		{
			pixely = spritePtr->height - 1;
		}

	}


	return 0;
}


int renderSprite_RLUD_FullAlpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	uint32_t hexValue;

	// Start render loop
	int pixely = spritePtr->height - 1 - ((yDraw - yOffset) % spritePtr->height);

	int startPixelx = spritePtr->width - 1 - ((xDraw - xOffset) % spritePtr->width);

	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * spritePtr->width);
		int screenYOffset = j * screenWidth;

		for (int i = xDraw; i < xDraw2; i++)
		{
			hexValue = spritePtr->spriteData[pixelYOffset + (pixelx << 2) + 3] << 24 | 
						spritePtr->spriteData[pixelYOffset + (pixelx << 2) + 2] << 16 | 
						spritePtr->spriteData[pixelYOffset + (pixelx << 2) + 1] << 8 | 
						spritePtr->spriteData[pixelYOffset + (pixelx << 2) ];

			if (hexValue >> 24 == 0xFF)
			{
				screen[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screen[screenYOffset + i] = blendPixel(screen[screenYOffset + i], hexValue);
			}

			pixelx--;

			if (pixelx < 0)
			{
				pixelx = spritePtr->width - 1;
			}
		}

		pixely--;

		if (pixely < 0)
		{
			pixely = spritePtr->width - 1;
		}

	}


	return 0;
}


int renderSprite_LRDU_FullAlpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	uint32_t hexValue;

	// Start render loop
	int pixely = ((yDraw - yOffset) % spritePtr->height);

	int startPixelx = ((xDraw - xOffset) % spritePtr->width);

	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * spritePtr->width);
		int screenYOffset = j * screenWidth;

		for (int i = xDraw; i < xDraw2; i++)
		{
			hexValue = spritePtr->spriteData[pixelYOffset + (pixelx << 2) + 3] << 24 | 
						spritePtr->spriteData[pixelYOffset + (pixelx << 2) + 2] << 16 | 
						spritePtr->spriteData[pixelYOffset + (pixelx << 2) + 1] << 8 | 
						spritePtr->spriteData[pixelYOffset + (pixelx << 2) ];

			if (hexValue >> 24 == 0xFF)
			{
				screen[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screen[screenYOffset + i] = blendPixel(screen[screenYOffset + i], hexValue);
			}

			pixelx++;

			if (pixelx >= spritePtr->width)
			{
				pixelx = 0;
			}
		}

		pixely++;

		if (pixely >= spritePtr->width)
		{
			pixely = 0;
		}

	}


	return 0;
}


int renderSprite_RLDU_FullAlpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	uint32_t hexValue;

	// Start render loop
	int pixely = ((yDraw - yOffset) % spritePtr->height);

	int startPixelx = spritePtr->width - 1 - ((xDraw - xOffset) % spritePtr->width);

	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * spritePtr->width);
		int screenYOffset = j * screenWidth;

		for (int i = xDraw; i < xDraw2; i++)
		{
			hexValue = spritePtr->spriteData[pixelYOffset + (pixelx << 2) + 3] << 24 | 
						spritePtr->spriteData[pixelYOffset + (pixelx << 2) + 2] << 16 | 
						spritePtr->spriteData[pixelYOffset + (pixelx << 2) + 1] << 8 | 
						spritePtr->spriteData[pixelYOffset + (pixelx << 2) ];

			if (hexValue >> 24 == 0xFF)
			{
				screen[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screen[screenYOffset + i] = blendPixel(screen[screenYOffset + i], hexValue);
			}

			pixelx--;

			if (pixelx < 0)
			{
				pixelx = spritePtr->width - 1;
			}
		}

		pixely++;

		if (pixely >= spritePtr->width)
		{
			pixely = 0;
		}

	}


	return 0;
}


uint32_t blendPixel(uint32_t screenPixel, uint32_t inputPixel)
{
	float alpha = ((inputPixel & 0xFF000000) >> 24 ) / 255.0;

	uint8_t red = (( ((screenPixel & 0x00FF0000) >> 16) * (1.0 - alpha)) + (((inputPixel & 0x00FF0000) >> 16) * alpha));; 

	uint8_t green = (( ((screenPixel & 0x0000FF00) >> 8) * (1.0 - alpha)) + (((inputPixel & 0x0000FF00) >> 8) * alpha));; 

	uint8_t blue = (( ((screenPixel & 0x000000FF)) * (1.0 - alpha)) + ((inputPixel & 0x000000FF) * alpha));; 

	return (red << 16 | green << 8 | blue);
}


// renders sprite in tile mode (Render mode 0) with no reflections  
int renderSprite_LRUD_Tile(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	// Start render loop
	int pixely = spritePtr->height - 1 - ((yDraw - yOffset) % spritePtr->height);

	int startPixelx = ((xDraw - xOffset) % spritePtr->width);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		int i = xDraw;
		unsigned int pixelOffset = (pixely << 2) * spritePtr->width;


		for (int i = xDraw; i < xDraw2; i++)
		{
			if ((uint32_t)spritePtr->spriteData[pixelOffset + (pixelx << 2) + 3] != 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + pixelOffset + (pixelx << 2), sizeOfPixel);
			}

			pixelx++;

			if (pixelx > spritePtr->width - 1)
			{
				pixelx = 0;
			}
		}


		pixely--;

		if (pixely < 0)
		{
			pixely = spritePtr->height - 1;
		}

	}


	return 0;
}


// renders sprite in tile mode (Render mode 0) with Left/Right reflections
int renderSprite_RLUD_Tile(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	int sizeOfPixel = sizeof(uint32_t);

	int pixely =  spritePtr->height - 1 - ((yDraw - yOffset) %  spritePtr->height);

	int startPixelx = spritePtr->width - 1 - ((xDraw - xOffset) % spritePtr->width);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int i = xDraw;
		int pixelx = startPixelx;
		int pixelyOffset = ((pixely << 2) * spritePtr->width);

		if ((xOffset - i) % 2 != 0)
		{
			if ((uint32_t)spritePtr->spriteData[pixelyOffset + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + pixelyOffset + (pixelx << 2), sizeOfPixel);
			}

			pixelx--;
			i++;
		}


		for (; i + 1 < xDraw2; i += 2)
		{
			if ((uint32_t)spritePtr->spriteData[pixelyOffset + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + pixelyOffset + (pixelx << 2), sizeOfPixel);
				memcpy(screen + (j * screenWidth) + i + 1, spritePtr->spriteData + pixelyOffset + (pixelx << 2), sizeOfPixel);
			}

			pixelx -= 2;

			if (pixelx < 0)
			{
				pixelx = spritePtr->width + pixelx;
			}
		}


		if (i < xDraw2)
		{
			if ((uint32_t)spritePtr->spriteData[pixelyOffset + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + pixelyOffset + (pixelx << 2), sizeOfPixel);
			}
		}

		pixely--;

		if (pixely < 0)
		{
			pixely =  spritePtr->height - 1;
		}

	}

	return 0;
}


// renders sprite in tile mode (Render mode 0) with Up/Down reflections
int renderSprite_LRDU_Tile(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	int sizeOfPixel = sizeof(uint32_t);

	int pixely = ((yDraw - yOffset) %  spritePtr->height);

	int startPixelx = ((xDraw - xOffset) % spritePtr->width);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int i = xDraw;
		int pixelx = startPixelx;
		int pixelyOffset = ((pixely << 2) * spritePtr->width);

		if ((xOffset - i) % 2 != 0)
		{
			if ((uint32_t)spritePtr->spriteData[pixelyOffset + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + pixelyOffset + (pixelx << 2), sizeOfPixel);
			}

			pixelx++;
			i++;
		}


		for (; i + 1 < xDraw2; i += 2)
		{
			if ((uint32_t)spritePtr->spriteData[pixelyOffset + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + pixelyOffset + (pixelx << 2), sizeOfPixel);
				memcpy(screen + (j * screenWidth) + i + 1, spritePtr->spriteData + pixelyOffset + (pixelx << 2), sizeOfPixel);
			}

			pixelx += 2;

			if (pixelx >= spritePtr->width)
			{
				pixelx = pixelx - spritePtr->width;
			}
		}


		if (i < xDraw2)
		{
			if ((uint32_t)spritePtr->spriteData[pixelyOffset + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + pixelyOffset + (pixelx << 2), sizeOfPixel);
			}
		}

		pixely++;

		if (pixely >=  spritePtr->height)
		{
			pixely = 0;
		}

	}

	return 0;
}


// renders sprite in tile mode (Render mode 0) with Left/Right and Up/Down reflections
int renderSprite_RLDU_Tile(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	int sizeOfPixel = sizeof(uint32_t);

	int pixely = ((yDraw - yOffset) %  spritePtr->height);

	int startPixelx = spritePtr->width - 1 - ((xDraw - xOffset) % spritePtr->width);

	for (int j = yDraw; j < yDraw2; j++)
	{
		int i = xDraw;
		int pixelx = startPixelx;
		int pixelyOffset = ((pixely << 2) * spritePtr->width);

		if ((xOffset - i) % 2 != 0)
		{
			if ((uint32_t)spritePtr->spriteData[pixelyOffset + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + pixelyOffset + (pixelx << 2), sizeOfPixel);
			}

			pixelx--;
			i++;
		}


		for (; i + 1 < xDraw2; i += 2)
		{
			if ((uint32_t)spritePtr->spriteData[pixelyOffset + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + pixelyOffset + (pixelx << 2), sizeOfPixel);
				memcpy(screen + (j * screenWidth) + i + 1, spritePtr->spriteData + pixelyOffset + (pixelx << 2), sizeOfPixel);
			}

			pixelx -= 2;

			if (pixelx < 0)
			{
				pixelx = spritePtr->width + pixelx;
			}
		}


		if (i < xDraw2)
		{
			if ((uint32_t)spritePtr->spriteData[pixelyOffset + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + pixelyOffset + (pixelx << 2), sizeOfPixel);
			}
		}

		pixely++;

		if (pixely >=  spritePtr->height)
		{
			pixely = 0;
		}

	}

	return 0;
}


int renderSprite_LRUD_TileFast(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);


	// Start render loop
	int pixely = spritePtr->height - 1 - (((yDraw - yOffset) % spritePtr->height));

	int startPixelx = ((xDraw - xOffset) % spritePtr->width);
	int screenYOffset = yDraw * screenWidth;

	// Render sprite to screen
	for (int i = yDraw; i < yDraw2; i++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * spritePtr->width);
		int k = xDraw;


		if (xDraw2 - xDraw >= spritePtr->width)
		{
			memcpy(screen + screenYOffset + k, spritePtr->spriteData + pixelYOffset + (pixelx << 2), sizeOfPixel * (spritePtr->width - pixelx));
			k += (spritePtr->width - pixelx);


			for (; k + spritePtr->width - 1 < xDraw2; k += spritePtr->width)
			{
				memcpy(screen + screenYOffset + k, spritePtr->spriteData + pixelYOffset, spritePtr->width * sizeOfPixel);
			}

			pixelx = 0;
		}

		memcpy(screen + screenYOffset + k, spritePtr->spriteData + pixelYOffset + (pixelx << 2), (xDraw2 - k) * sizeOfPixel);

		pixely--;
		screenYOffset += screenWidth;

		if (pixely < 0)
		{
			pixely = spritePtr->height - 1;
		}
	}


	return 0;
}



int renderSprite_LRDU_TileFast(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	int j, i;

	// Start render loop
	int pixely = ((yDraw - yOffset) % spritePtr->height);

	int startPixelx = ((xDraw - xOffset) % spritePtr->width);
	int screenYOffset = yDraw * screenWidth;

	for (j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * spritePtr->width);
		int k = xDraw;

		if (xDraw2 - xDraw >= spritePtr->width)
		{
			memcpy(screen + screenYOffset + k, spritePtr->spriteData + pixelYOffset + (pixelx << 2), sizeOfPixel * (spritePtr->width - pixelx));
			k += (spritePtr->width - pixelx);


			for (; k + spritePtr->width - 1 < xDraw2; k += spritePtr->width)
			{
				memcpy(screen + screenYOffset + k, spritePtr->spriteData + pixelYOffset, spritePtr->width * sizeOfPixel);
			}

			pixelx = 0;
		}

		memcpy(screen + screenYOffset + k, spritePtr->spriteData + pixelYOffset + (pixelx << 2), (xDraw2 - k) * sizeOfPixel);

		pixely++;
		screenYOffset += screenWidth;

		if (pixely > spritePtr->height - 1)
		{
			pixely = 0;
		}

	}


	return 0;
}


int renderSprite_LRUD_Scale(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);

	double xScale = (double)spritePtr->width/(double)inputBox->xSize;
	double yScale = (double)spritePtr->height/(double)inputBox->ySize;
	double xScale2 = (double)inputBox->xSize / (double)spritePtr->width;

	double pixely = (inputBox->ySize - 1 - (yDraw - yOffset)) * yScale;


	for (int i = yDraw; i < yDraw2; ++i)
	{
		unsigned int pixelx = 0;

		int intPixelY = (((int)pixely << 2) * spritePtr->width);
		int screenYOffset = i * screenWidth;

		double k = xOffset;

		if (k < 0.0)
		{
			for (; k < 0; k += xScale2)
			{
				pixelx++;
			}

			pixelx--;

			if ((uint32_t)spritePtr->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = xDraw; j < k; ++j)
				{
					memcpy(screen + screenYOffset + j, spritePtr->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx++;
		}

		for (; (int)k < xDraw2 && pixelx < spritePtr->width; k += xScale2)
		{
			if ((uint32_t)spritePtr->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = 0; j < xScale2 && (int)k + j < xDraw2; ++j)
				{
					memcpy(screen + screenYOffset + (int)k + j, spritePtr->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx++;
		}

		pixely -= yScale;
	}

	return 0;
}


int renderSprite_RLUD_Scale(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);

	double xScale = (double)spritePtr->width/(double)inputBox->xSize;
	double yScale = (double)spritePtr->height/(double)inputBox->ySize;
	double xScale2 = (double)inputBox->xSize / (double)spritePtr->width;

	double pixely = (inputBox->ySize - 1 - (yDraw - yOffset)) * yScale;

	int startPixelx = spritePtr->width - 1;
	int screenYOffset = yDraw * screenWidth;

	for (int i = yDraw; i < yDraw2; ++i)
	{
		unsigned int pixelx = startPixelx;

		int intPixelY = (((int)pixely << 2) * spritePtr->width);

		double k = xOffset;


		if (k < 0)
		{
			for (; k < 0; k += xScale2)
			{
				pixelx--;
			}

			pixelx++;

			if ((uint32_t)spritePtr->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = xDraw; j < k; ++j)
				{
					memcpy(screen + screenYOffset + j, spritePtr->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx--;
		}
		

		for (; (int)k < xDraw2 && pixelx < spritePtr->width; k += xScale2)
		{
			if ((uint32_t)spritePtr->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = 0; j < xScale2 && (int)k + j < xDraw2; ++j)
				{
					memcpy(screen + screenYOffset + (int)k + j, spritePtr->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx--;
		}

		screenYOffset += screenWidth;
		pixely -= yScale;
	}

	return 0;
}


int renderSprite_LRDU_Scale(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);

	double xScale = (double)spritePtr->width/(double)inputBox->xSize;
	double yScale = (double)spritePtr->height/(double)inputBox->ySize;
	double xScale2 = (double)inputBox->xSize / (double)spritePtr->width;

	double pixely = (yDraw - yOffset) * yScale;


	for (int i = yDraw; i < yDraw2; i++)
	{
		int intPixelY = (((int)pixely << 2) * spritePtr->width);
		unsigned int pixelx = 0;

		double k = xOffset;


		if (k < 0)
		{
			for (; k < 0; k += xScale2)
			{
				pixelx++;
			}

			pixelx--;

			if ((uint32_t)spritePtr->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = xDraw; j < k; j++)
				{
					memcpy(screen + (i * screenWidth) + j, spritePtr->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx++;
		}
		

		for (; (int)k < xDraw2 && pixelx < spritePtr->width; k += xScale2)
		{
			if ((uint32_t)spritePtr->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = 0; j < xScale2 && (int)k + j < xDraw2; j++)
				{
					memcpy(screen + (i * screenWidth) + (int)k + j, spritePtr->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx++;
		}

		pixely += yScale;
	}

	return 0;
}


int renderSprite_RLDU_Scale(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);

	double xScale = (double)spritePtr->width/(double)inputBox->xSize;
	double yScale = (double)spritePtr->height/(double)inputBox->ySize;
	double xScale2 = (double)inputBox->xSize / (double)spritePtr->width;

	double pixely = (yDraw - yOffset) * yScale;

	int startPixelx = spritePtr->width - 1;


	for (int i = yDraw; i < yDraw2; i++)
	{
		int intPixelY = (((int)pixely << 2) * spritePtr->width);
		unsigned int pixelx = startPixelx;

		double k = xOffset;


		if (k < 0)
		{
			for (; k < 0; k += xScale2)
			{
				pixelx--;
			}

			pixelx++;

			if ((uint32_t)spritePtr->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = xDraw; j < k; j++)
				{
					memcpy(screen + (i * screenWidth) + j, spritePtr->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx--;
		}
		

		for (; (int)k < xDraw2 && pixelx < spritePtr->width; k += xScale2)
		{
			if ((uint32_t)spritePtr->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = 0; j < xScale2 && (int)(j + k) < xDraw2; j++)
				{
					memcpy(screen + (i * screenWidth) + (int)k + j, spritePtr->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx--;
		}

		pixely += yScale;
	}

	return 0;
}


int renderSprite_LRUD_Scale_Full_Alpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);
	uint32_t hexValue;

	double xScale = (double)spritePtr->width/(double)inputBox->xSize;
	double yScale = (double)spritePtr->height/(double)inputBox->ySize;


	double pixely = (inputBox->ySize - 1 - (yDraw - yOffset)) * yScale;

	double startPixelx = xScale * (xDraw - xOffset);


	for (int k = yDraw; k < yDraw2; k++)
	{
		int pixelYOffset = (((int)pixely) << 2) * spritePtr->width;
		int screenYOffset = k * screenWidth;
		double pixelx = startPixelx;

		for (int i = xDraw; i < xDraw2; i++)
		{
			int intPixelx = ((int)pixelx) << 2;

			hexValue = spritePtr->spriteData[pixelYOffset + intPixelx + 3] << 24 | 
						spritePtr->spriteData[pixelYOffset + intPixelx + 2] << 16 | 
						spritePtr->spriteData[pixelYOffset + intPixelx + 1] << 8 | 
						spritePtr->spriteData[pixelYOffset + intPixelx];

			if (hexValue >> 24 == 0xFF)
			{
				screen[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screen[screenYOffset + i] = blendPixel(screen[screenYOffset + i], hexValue);
			}
			

			pixelx += xScale;
		}

		pixely -= yScale;
	}


	return 0;
}


int renderSprite_RLUD_Scale_Full_Alpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);
	uint32_t hexValue;

	double xScale = (double)spritePtr->width/(double)inputBox->xSize;
	double yScale = (double)spritePtr->height/(double)inputBox->ySize;


	double pixely = (inputBox->ySize - 1 - (yDraw - yOffset)) * yScale;

	double startPixelx = (inputBox->xSize - 1 - (xDraw - xOffset)) * xScale;


	for (int k = yDraw; k < yDraw2; k++)
	{
		int pixelYOffset = (((int)pixely) << 2) * spritePtr->width;
		int screenYOffset = k * screenWidth;
		double pixelx = startPixelx;

		for (int i = xDraw; i < xDraw2; i++)
		{
			int intPixelx = ((int)pixelx) << 2;

			hexValue = spritePtr->spriteData[pixelYOffset + intPixelx + 3] << 24 | 
						spritePtr->spriteData[pixelYOffset + intPixelx + 2] << 16 | 
						spritePtr->spriteData[pixelYOffset + intPixelx + 1] << 8 | 
						spritePtr->spriteData[pixelYOffset + intPixelx];

			if (hexValue >> 24 == 0xFF)
			{
				screen[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screen[screenYOffset + i] = blendPixel(screen[screenYOffset + i], hexValue);
			}
			

			pixelx -= xScale;
		}

		pixely -= yScale;
	}


	return 0;
}


int renderSprite_LRDU_Scale_Full_Alpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);
	uint32_t hexValue;

	double xScale = (double)spritePtr->width/(double)inputBox->xSize;
	double yScale = (double)spritePtr->height/(double)inputBox->ySize;


	double pixely = (inputBox->ySize - 1 - (yDraw - yOffset)) * yScale;

	double startPixelx = xScale * (xDraw - xOffset);


	for (int k = yDraw; k < yDraw2; k++)
	{
		int pixelYOffset = (((int)pixely) << 2) * spritePtr->width;
		int screenYOffset = k * screenWidth;
		double pixelx = startPixelx;

		for (int i = xDraw; i < xDraw2; i++)
		{
			int intPixelx = ((int)pixelx) << 2;

			hexValue = spritePtr->spriteData[pixelYOffset + intPixelx + 3] << 24 | 
						spritePtr->spriteData[pixelYOffset + intPixelx + 2] << 16 | 
						spritePtr->spriteData[pixelYOffset + intPixelx + 1] << 8 | 
						spritePtr->spriteData[pixelYOffset + intPixelx];

			if (hexValue >> 24 == 0xFF)
			{
				screen[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screen[screenYOffset + i] = blendPixel(screen[screenYOffset + i], hexValue);
			}
			

			pixelx += xScale;
		}

		pixely -= yScale;
	}


	return 0;
}


int renderSprite_RLDU_Scale_Full_Alpha(uint32_t screen[], Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);
	uint32_t hexValue;

	double xScale = (double)spritePtr->width/(double)inputBox->xSize;
	double yScale = (double)spritePtr->height/(double)inputBox->ySize;


	double pixely = (inputBox->ySize - 1 - (yDraw - yOffset)) * yScale;

	double startPixelx = (inputBox->xSize - 1 - (xDraw - xOffset)) * xScale;


	for (int k = yDraw; k < yDraw2; k++)
	{
		int pixelYOffset = (((int)pixely) << 2) * spritePtr->width;
		int screenYOffset = k * screenWidth;
		double pixelx = startPixelx;

		for (int i = xDraw; i < xDraw2; i++)
		{
			int intPixelx = ((int)pixelx) << 2;

			hexValue = spritePtr->spriteData[pixelYOffset + intPixelx + 3] << 24 | 
						spritePtr->spriteData[pixelYOffset + intPixelx + 2] << 16 | 
						spritePtr->spriteData[pixelYOffset + intPixelx + 1] << 8 | 
						spritePtr->spriteData[pixelYOffset + intPixelx];

			if (hexValue >> 24 == 0xFF)
			{
				screen[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screen[screenYOffset + i] = blendPixel(screen[screenYOffset + i], hexValue);
			}
			

			pixelx -= xScale;
		}

		pixely -= yScale;
	}


	return 0;
}


int renderBackGroundSprite(uint32_t *screen, World *gameWorld)
{
	if (gameWorld == NULL || gameWorld->drawBackGround == 0)
	{
		return MISSING_DATA;
	}

	Sprite *spritePtr;
	spritePtr = gameWorld->bgSpriteBuffer;

	if (spritePtr == NULL || spritePtr->RenderMode == DO_NOT_RENDER)
	{
		return MISSING_DATA;
	}

	// Load up data buffers for sprite to be rendered
	int spriteWidth = spritePtr->width;
	int spriteHeight = spritePtr->height;
	unsigned char *data = spritePtr->spriteData;		// Instead of copying the data, use a pointer to reduce memory overhead

	// Locate object on screen
	int xOffset = gameWorld->CameraX * gameWorld->bgParallax;
	int yOffset = gameWorld->CameraY * gameWorld->bgParallax;
	int xOffset2 = xOffset + spritePtr->width;
	int yOffset2 = yOffset + spritePtr->height;

	int xDraw2, xDraw, yDraw2, yDraw;

	size_t sizeOfPixel = sizeof(uint32_t);
	int i, k;

	int pixely = spriteHeight - 1 - (((yDraw + yOffset) % spriteHeight));
	int pixelx = 0;


	//	RenderMode 0: Tile Mapping								(Sprite will tile across entire bounding box of object)
	//	RenderMode 1: Single sprite render 						(Renders single instance of sprite directly to bottom-left corner)
	//  RenderMode 2: Row Parallax

	if (spritePtr->RenderMode == SINGLE || spritePtr->RenderMode == SINGLE_FAST || spritePtr->RenderMode == SINGLE_FULL_ALPHA)
	{
		xDraw = clamp(0 - xOffset, 0, screenWidth - 1);
		yDraw = clamp(0 - yOffset, 0, screenHeight - 1);
		xDraw2 = clamp(spriteWidth - xOffset, 0, screenWidth - 1);
		yDraw2 = clamp(spriteHeight - yOffset, 0, screenHeight - 1);

		for (i = yDraw; i < yDraw2; i++)
		{
			pixelx = (((xDraw + xOffset) % spriteWidth));

			memcpy(screen + (i * screenWidth) + xDraw, data + ((pixely << 2) * spriteWidth) + (pixelx << 2), ((xDraw2 - xDraw) % spriteWidth) * sizeOfPixel);
			
			pixely--;

			if (pixely < 0)
			{
				pixely = spriteHeight - 1;
			}

		}

		return 0;
	}
	

	xDraw = 0;
	yDraw = 0;
	xDraw2 = screenWidth - 1;
	yDraw2 = screenHeight - 1;


	// Render sprite to screen
	for (i = yDraw; i < yDraw2; i++)
	{
		if (spritePtr->RenderMode == BG_ROW_PARALLAX && (i + yOffset) % gameWorld->bgParallaxChunkSize == 0)
		{
			xOffset = gameWorld->CameraX * (gameWorld->bgParallax - (gameWorld->bgChunkParallax * (i + yOffset) ) );
		}
		

		pixelx = ((xDraw + xOffset) % spriteWidth);

		memcpy(screen + (i * screenWidth), data + ((pixely << 2) * spriteWidth) + (pixelx << 2), sizeOfPixel * (spriteWidth - pixelx));
		
		k = spriteWidth - pixelx;

		for (; k + spriteWidth < xDraw2; k += spriteWidth)
		{
			memcpy(screen + (i * screenWidth) + k, data + ((pixely << 2) * spriteWidth), spriteWidth * sizeOfPixel);
		}

	
		memcpy(screen + (i * screenWidth) + k, data + ((pixely << 2) * spriteWidth), ((xDraw2 - k) % spriteWidth) * sizeOfPixel);

		pixely--;

		if (pixely < 0)
		{
			pixely = spriteHeight - 1;
		}

	}


	return 0;
}


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


int cleanRenderer(World *gameWorld, uint32_t *screen)
{
	memset(screen, 0x00, sizeof(uint32_t) * screenWidth * screenHeight);


	if (gameWorld != NULL)
	{
		gameWorld->drawnObjects = 0;
		gameWorld->drawnParticles = 0;
		gameWorld->drawnHudElements = 0;
	}

	return 0;
}



// Old method of rendering - renders each pixel one at a time
// New method renders in 2 pixel rows, meaning less fidelity on x-axis (However this is in line with art style)
//// No reflection
//if (currentObject->ObjectBox->xFlip == 1 && currentObject->ObjectBox->yFlip == 1)
//{
//	pixelx = (xDraw - xOffset) % currentObject->xSize;
//
//	for (int k = xDraw; k < xDraw2; k++)
//	{
//		pixely = currentObject->ySize - 1 - (yDraw - yOffset);
//
//		if (pixely < 0)
//		{
//			pixely = currentObject->ySize - 1;
//		}
//
//		for (int i = yDraw; i < yDraw2; i++)
//		{
//			int adjustedPx = pixelx * xScale;
//			int adjustedPy = pixely * yScale;
//
//			if ((uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2) + 3] > 0x00)
//			{
//				hexValue =  0x00 << 24 |
//				(uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2)] << 16 |
//				(uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2) + 1] << 8 |
//				(uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2) + 2];
//
//				screen[(i * screenWidth) + k] = hexValue;
//			}
//
//			pixely--;
//		}
//
//		pixelx++;
//	}
//
//	return 0;
//}
//
//// Left-Right reflection
//if (currentObject->ObjectBox->xFlip == -1 && currentObject->ObjectBox->yFlip == 1)
//{
//	pixelx = currentObject->xSize - 1 - (xDraw - xOffset);
//
//	if (pixelx < 0)
//	{
//		pixelx = currentObject->xSize - 1;
//	}
//
//
//	for (int k = xDraw; k < xDraw2; k++)
//	{
//		pixely = currentObject->ySize - 1 - (yDraw - yOffset);
//
//		if (pixely < 0)
//		{
//			pixely = currentObject->ySize - 1;
//		}
//
//		for (int i = yDraw; i < yDraw2; i++)
//		{
//			int adjustedPx = pixelx * xScale;
//			int adjustedPy = pixely * yScale;
//
//			if ((uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2) + 3] > 0x00)
//			{
//				hexValue =  0x00 << 24 |
//				(uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2)] << 16 |
//				(uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2) + 1] << 8 |
//				(uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2) + 2];
//
//				screen[(i * screenWidth) + k] = hexValue;
//			}
//
//			pixely--;
//
//		}
//
//		pixelx--;
//	}
//
//	return 0;
//}
//
//
//// Up-Down reflection
//if (currentObject->ObjectBox->xFlip == 1 && currentObject->ObjectBox->yFlip == -1)
//{
//	pixelx = (xDraw - xOffset) % currentObject->xSize;
//
//	for (int k = xDraw; k < xDraw2; k++)
//	{
//		pixely = (yDraw - yOffset) % currentObject->ySize;;
//
//
//		for (int i = yDraw; i < yDraw2; i++)
//		{
//			int adjustedPx = pixelx * xScale;
//			int adjustedPy = pixely * yScale;
//
//			if ((uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2) + 3] > 0x00)
//			{
//				hexValue =  0x00 << 24 |
//				(uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2)] << 16 |
//				(uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2) + 1] << 8 |
//				(uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2) + 2];
//
//				screen[(i * screenWidth) + k] = hexValue;
//			}
//
//			pixely++;
//		}
//
//		pixelx++;
//	}
//
//	return 0;
//}
//
//// Up-Down and Left-Right reflection
//if (currentObject->ObjectBox->xFlip == -1 && currentObject->ObjectBox->yFlip == -1)
//{
//	pixelx = currentObject->xSize - 1 - (xDraw - xOffset);
//
//	if (pixelx < 0)
//	{
//		pixelx = currentObject->xSize - 1;
//	}
//
//
//	for (int k = xDraw; k < xDraw2; k++)
//	{
//		pixely = (yDraw - yOffset) % currentObject->ySize;
//
//		for (int i = yDraw; i < yDraw2; i++)
//		{
//			int adjustedPx = pixelx * xScale;
//			int adjustedPy = pixely * yScale;
//
//			if ((uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2) + 3] > 0x00)
//			{
//				hexValue =  0x00 << 24 |
//				(uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2)] << 16 |
//				(uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2) + 1] << 8 |
//				(uint32_t)data[((adjustedPy << 2) * spriteWidth) + (adjustedPx << 2) + 2];
//
//				screen[(i * screenWidth) + k] = hexValue;
//			}
//
//			pixely++;
//		}
//
//		pixelx--;
//	}
//
//	return 0;
//}
