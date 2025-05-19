#include "drawScreen.h"


// Controls what symbol is printed for each tile
static uint32_t tileMap[32] = {0x00AA00AA, 0xFFFF0088, 0xFF0088FF, 0xFF552299, 0xFF00AA00, 0xFFBB55AA, 0xFFD0CC00, 0xFF88FF44,
								0xFF77DD11, 0xFF11BB44, 0xFF0F449F, 0xFF000010, 0xFF001010, 0xFF101010, 0xFF1C1010, 0xFF101C10,
								0xFF2C1010, 0xFF102C10, 0xFF10103C, 0xFF40201C, 0xFF019E30, 0xFF0514CE, 0xFFE86AA3, 0xFF0A4321};



int drawPlayer(uint32_t screen[], int width, int height, World *gameWorld)
{
	if (gameWorld == NULL || gameWorld->drawPlayer == 0 || gameWorld->Player == NULL)
	{
		return MISSING_DATA;
	}

	PlayerData *player;
	player = gameWorld->Player;

	// Find offsets of player relative to center of screen/camera
	int xOffset = player->xPos - gameWorld->cameraX + (H_RESOLUTION >> 1);
	int yOffset = player->yPos - gameWorld->cameraY + (V_RESOLUTION >> 1);


	// Load correct sprite
	Sprite *spritePtr;
	spritePtr = player->spriteBuffer;

	if (spritePtr == NULL)
	{
		return MISSING_DATA;
	}

	// Load up data buffers for sprite to be rendered
	int spriteWidth = spritePtr->width;
	int spriteHeight = spritePtr->height;
	unsigned char *data = spritePtr->spriteData;		// Instead of copying the data, use a pointer to reduce memory overhead

	// Locate player on screen
	int xDraw = correct(xOffset - 8, 0, width);
	int yDraw = correct(yOffset, 0, height);
	int xDraw2 = correct(xOffset + spriteWidth - 8, -1, width - 1);
	int yDraw2 = correct(spriteHeight + yOffset, -1, height - 1);

	// Render to screen
	uint32_t hexValue = 0x00000000;
	size_t sizeOfPixel = sizeof(uint32_t);

	if (gameWorld->drawSprites == 1 && player->xFlip == 1)
	{
		int pixelx = 0;

		for (int i = xDraw; i < xDraw2; i++, pixelx++)
		{
			for (int j = yDraw, pixely = spriteHeight - 1 - (yDraw - yOffset); j < yDraw2; j++, pixely--)
			{
				if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0)
				{
					memcpy(screen + (width * j) + i, data + ((pixely << 2) * spriteWidth) + (pixelx << 2), sizeOfPixel);
				}

			}
		}
	}


	if (gameWorld->drawSprites == 1 &&player->xFlip == -1)
	{
		int pixelx = spriteWidth - 1;

		for (int i = xDraw; i < xDraw2; i++, pixelx--)
		{
			for (int j = yDraw, pixely = spriteHeight - 1; j < yDraw2; j++, pixely--)
			{
				if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0)
				{
					memcpy(screen + (width * j) + i, data + ((pixely << 2) * spriteWidth) + (pixelx << 2), sizeOfPixel);
				}

			}
		}
	}


	// Correct Offsets
	xDraw = correct(xOffset, 0, width - 1);
	yDraw = correct(yOffset, 0, height - 1);
	xDraw2 = correct(PLAYERWIDTH + xOffset, 0, width - 1);
	yDraw2 = correct(PLAYERHEIGHT + yOffset, 0, height - 1);

	// Player Hitbox
	if (gameWorld->drawHitboxes == 0)
	{
		return ACTION_DISABLED;
	}

	for (int i = xDraw; i < xDraw2; i++)
	{
		for (int j = yDraw; j < yDraw2; j++)
		{
			screen[(j * width) + i] = 0xFFFFFFFF;
		}
	}


	return 0;
}


int worldCameraControl(int width, int height, PlayerData *player, World *gameWorld)
{
	if (player == NULL || gameWorld == NULL)
	{
		return MISSING_DATA;
	}

	int xOffset = (int)player->xPos - gameWorld->cameraX;
	int yOffset = (int)player->yPos - gameWorld->cameraY;

	gameWorld->cameraX = player->xPos;

//	if ( (xOffset) >= (H_RESOLUTION * 0.1))
//	{
//		gameWorld->cameraX = player->xPos - (H_RESOLUTION * 0.1);
//	}
//
//	if (xOffset < (H_RESOLUTION * -0.1))
//	{
//		gameWorld->cameraX = player->xPos + (H_RESOLUTION * 0.1);
//	}

	if ( (yOffset) >= (V_RESOLUTION * 0.1))
	{
		gameWorld->cameraY = (int)player->yPos - (V_RESOLUTION * 0.1);
	}

	if (yOffset < (V_RESOLUTION * -0.2))
	{
		gameWorld->cameraY = (int)player->yPos + (V_RESOLUTION * 0.2);
	}


	return 0;
}


int getTileAtCamera(int levelData[][GRID_HEIGHT], char tileShape[], int x, int y)
{
	int Y = y - 22;
	int X = x - 10;

	if (Y < 0 || X < 0)
	{
		return 0;
	}

	int gridX, gridY = 0;
	gridX = floor(X/ X_TILESCALE);
	gridY = floor(Y/ Y_TILESCALE);


	if (gridY < 0 || gridX < 0)
	{
		return 0;
	}

	int ans = levelData[gridX][gridY];

	if (tileShape[ans] == '_' && (Y % Y_TILESCALE) > (Y_TILESCALE >> 1))
	{
		return 0;
	}

	if (tileShape[ans] == '/' && ((Y % Y_TILESCALE) - (X % X_TILESCALE) > 0))
	{
		return 0;
	}

	return ans;
}


int validScreenPos(int x, int y)
{
	if (y >= 0 && y < V_RESOLUTION && x >= 0 && x < H_RESOLUTION)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//int drawTiles(uint32_t screen[], int width, int height, World *gameWorld)
//{
//	if (gameWorld == NULL)
//	{
//		return -1;
//	}

//	int checkTile = 0;
//	int xOffset = gameWorld->cameraX - (width >> 1);
//	int yOffset = gameWorld->cameraY - (height >> 1);
//	int i = 0;
//	int j = 0;

	// Draw first few rows then columns
//	for (; i < width; i++)
//	{
//		for (; j < height; j++)
//		{
//			checkTile = getTileAtCamera(gameWorld->levelData, gameWorld->tileShape, xOffset + i, yOffset + j);

//			if (checkTile > 0)
//			{
//				if (i > -1 && i < width && j < height && j > -1)
//				{
//					screen[(j * width) + i] = tileMap[checkTile];
//				}

//			}

//		}

//	}

//	int prevYPos = j;
//
//	for (i = 0; i < (abs(xOffset) - 10) % 4; i++)
//		{
//			for (j = 0; j < height; j++)
//			{
//				checkTile = getTileAtCamera(gameWorld->levelData, gameWorld->tileShape, xOffset + i, yOffset + j);
//
//				if (checkTile > 0)
//				{
//					if (i > -1 && i < width && j < height && j > -1)
//					{
//						screen[(j * width) + i] = tileMap[checkTile];
//					}
//
//				}
//
//			}
//
//		}
//
//	int prevXPos = i;
//
//	printf("%d\n", i);
//	// Draws tiles as 4x4 squares
//	for (i = 0; i < width - prevXPos; i += 4)
//		{
//			for (j = 0; j < height - prevYPos; j += 4)
//			{
//				checkTile = getTileAtCamera(gameWorld->levelData, gameWorld->tileShape, xOffset + i + prevXPos, yOffset + j + prevYPos);
//
//				if (checkTile > 0)
//				{
//					for (int l = 0; l < 4; l++)
//					{
//						for (int k = 0; k < 4; k++)
//						{
//							if ((i + k + prevXPos) > -1 && (i + k + prevXPos) < width && (j + l + prevYPos) < height && (j + l + prevYPos) > -1)
//							{
//								screen[((j + l + prevYPos) * width) + i + k + prevXPos] = tileMap[checkTile];
//							}
//						}
//					}
//				}
//
//			}
//		}


//	return 0;
//}


int drawObjects(Layer drawLayer, uint32_t screen[], int width, int height, World *gameWorld)
{
	// WARNING! This function is really messy and not easy to read: All variations of rendering has its own copy of the
	// main render loop, even extremely similar ones. This was done to squeeze out performance (at the expense of a bit of memory)
	// As such, readability was sacrificed somewhat. Continue at your own risk!
	if (gameWorld == NULL || gameWorld->objectList == NULL)
	{
		return MISSING_DATA;
	}

	Object *currentObject;
	currentObject = gameWorld->objectList->firstObject;

	int i = gameWorld->objectList->objectCount;


	while(currentObject != NULL && i > 0)
	{
		// Drawing routine - max objects on screen is defined as Macro
		renderObject(gameWorld, currentObject, screen, width, height, drawLayer);
		
		currentObject = currentObject->nextObject;
		i--;
	}

	if (gameWorld->Player == NULL)
	{
		return -1;
	}

	if (gameWorld->Player->playerLayer == drawLayer)
	{
		drawPlayer(screen, width, height, gameWorld);
	}
	

	return 0;
}


int renderObject(World *gameWorld, Object *currentObject, uint32_t screen[], int width, int height, Layer drawLayer)
{
	if (currentObject == NULL || currentObject->objectRenderMode == DO_NOT_RENDER)
	{
		return MISSING_DATA;
	}

	if (gameWorld->drawnObjects > MAX_OBJECTS || currentObject->layer != drawLayer)
	{
		return ACTION_DISABLED;
	}

	
	// Sprite
	renderObjectSprite(screen, width, height, gameWorld, currentObject);

	gameWorld->drawnObjects++;


	if (gameWorld->drawHitboxes == 0)
	{
		return 0;
	}


	// Hitbox
	switch (currentObject->solid)
	{
		case 2:
		{
			// Correct Offsets
			double ySize = currentObject->ySize;
			double xSize = currentObject->xSize;

			int xOffset = (int)currentObject->xPos - gameWorld->cameraX + (H_RESOLUTION >> 1);
			int yOffset = (int)currentObject->yPos - gameWorld->cameraY + (V_RESOLUTION >> 1);
			int xDraw = correct(xOffset, 0, width - 1);
			int yDraw = correct(yOffset, 0, height - 1);
			int xDraw2 = correct(xOffset + currentObject->xSize, 0, width - 1);

			int tile = tileMap[currentObject->objectID];

			for (int k = xDraw; k < xDraw2; k++)
			{
				for (int i = yDraw; i < height && i - yOffset < (k - xOffset) * (ySize/xSize); i++)
				{
					screen[(i * width) + k] = tile;
				}
			}
		} break;


		case 3:
		{
			// Correct Offsets
			double ySize = currentObject->ySize;
			double xSize = currentObject->xSize;

			int xOffset = (int)currentObject->xPos - gameWorld->cameraX + (H_RESOLUTION >> 1);
			int yOffset = (int)currentObject->yPos - gameWorld->cameraY + (V_RESOLUTION >> 1);
			int xDraw = correct(xOffset, 0, width - 1);
			int yDraw = correct(yOffset, 0, height - 1);
			int xDraw2 = correct(xOffset + currentObject->xSize, 0, width - 1);

			int tile = tileMap[currentObject->objectID];

			for (int k = xDraw; k < xDraw2; k++)
			{
				for (int i = yDraw; i < height && i - yOffset < (xSize - (k - xOffset)) * (ySize/xSize); i++)
				{
					screen[(i * width) + k] = tile;
				}
			}
		} break;


		default:
		{

			// Correct Offsets
			int xOffset = (int)currentObject->xPos - gameWorld->cameraX + (H_RESOLUTION >> 1);
			int yOffset = (int)currentObject->yPos - gameWorld->cameraY + (V_RESOLUTION >> 1);
			int xDraw = correct(xOffset, 0, width - 1);
			int yDraw = correct(yOffset, 0, height - 1);
			int xDraw2 = correct(xOffset + currentObject->xSize, 0, width - 1);
			int yDraw2 = correct(yOffset + currentObject->ySize, 0, height - 1);

			int tile = tileMap[currentObject->objectID];

			for (int k = xDraw; k < xDraw2; k++)
			{
				for (int i = yDraw; i < yDraw2; i++)
				{
					screen[(i * width) + k] = tile;
				}

			}
		} break;
	}
	

	return 0;
}



int renderObjectSprite(uint32_t screen[], int screenWidth, int screenHeight, World *gameWorld, Object *currentObject)
{
	// 	RenderMode 0: Tile Mapping											(Sprite will tile across entire bounding box of object)
	// 	RenderMode 1: Tile Scaling											(Sprite will be scaled to fit entire box of object)
	//	RenderMode 2: Tile Mapping/Scaling on X/Y axis						(Sprite tiles on X axis, scales on Y axis) WIP
	//	RenderMode 2: Tile Mapping/Scaling on X/Y axis						(Sprite scales on X axis, tiles on Y axis) WIP
	//	RenderMode 4:  Straight sprite render								(Renders single instance of sprite directly to bottom-left corner)
	// 	All mapping methods support horizontal and vertical flipping

	if (currentObject == NULL || gameWorld->drawSprites == 0)
	{
		return MISSING_DATA;
	}

	Sprite *spritePtr;
	spritePtr = currentObject->spriteBuffer;

	if (spritePtr == NULL)
	{
		return MISSING_DATA;
	}

	// If the object's render mode is less than 0 (the default) the sprite is rendered according to the sprite's individual render mode
	RenderMode currentRenderMode;

	if (currentObject->objectRenderMode == DEFAULT_TO_SPRITE)
	{
		currentRenderMode = spritePtr->RenderMode;
	}
	else
	{
		currentRenderMode = currentObject->objectRenderMode;
	}

	// Load up data buffers for sprite to be rendered
	int spriteWidth = spritePtr->width;
	int spriteHeight = spritePtr->height;
	unsigned char *data = spritePtr->spriteData;				// Instead of copying the data, use a pointer to reduce memory overhead


	// Locate object on screen
	int xOffset = currentObject->xPos - gameWorld->cameraX + (H_RESOLUTION >> 1);
	int yOffset = currentObject->yPos - gameWorld->cameraY + (V_RESOLUTION >> 1);
	int xOffset2 = xOffset + currentObject->xSize;
	int yOffset2 = yOffset + currentObject->ySize;

	
	// Make sure the sprite position is valid on screen to avoid a crash related to assuming the position is valid
	if (xOffset >= screenWidth || currentObject->xSize + xOffset < 0 || yOffset >= screenHeight || currentObject->ySize + yOffset < 0)
	{
		return INVALID_DATA;
	}


	int xDraw2, xDraw, yDraw2, yDraw;

	// Set draw locations on screen
	if (currentRenderMode == SINGLE)
	{
		// Find center of object and then center sprite on that point
		int centerX = (xOffset2 + xOffset) >> 1;
		int centerY = (yOffset2 + yOffset) >> 1;
		xOffset = centerX - (spriteWidth >> 1);
		yOffset = centerY - (spriteWidth >> 1);

		xDraw = correct(centerX - (spriteWidth >> 1), 0, screenWidth - 1);
		yDraw = correct(centerY - (spriteWidth >> 1), 0, screenHeight - 1);
		xDraw2 = correct(centerX + (spriteWidth >> 1), 0, screenWidth - 1);
		yDraw2 = correct(centerY + (spriteHeight >> 1), 0, screenHeight - 1);
	}
	else
	{
		xDraw = correct(xOffset, 0, screenWidth - 1);
		yDraw = correct(yOffset, 0, screenHeight - 1);
		xDraw2 = correct(currentObject->xSize + xOffset, 0, screenWidth - 1);
		yDraw2 = correct(currentObject->ySize + yOffset, 0, screenHeight - 1);
	}


	// Render sprite to screen

	if (currentRenderMode == TILE || currentRenderMode == SINGLE)
	{
		// No reflection
		if (currentObject->xFlip == 1 && currentObject->yFlip == 1)
		{
			renderSprite_LRUD_TileMode(screen, screenWidth, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);

			return 0;
		}

		// Left-Right reflection
		if (currentObject->xFlip == -1 && currentObject->yFlip == 1)
		{
			renderSprite_RLUD_TileMode(screen, screenWidth, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);

			return 0;
		}

		// Up-Down reflection
		if (currentObject->xFlip == 1 && currentObject->yFlip == -1)
		{
			renderSprite_LRDU_TileMode(screen, screenWidth, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);

			return 0;
		}

		// Up-Down and Left-Right reflection
		if (currentObject->xFlip == -1 && currentObject->yFlip == -1)
		{
			renderSprite_RLDU_TileMode(screen, screenWidth, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);

			return 0;
		}

		return INVALID_DATA;
	}


	if (currentRenderMode == SCALE)
	{
		double xScale = (double)spriteWidth/(double)currentObject->xSize;
		double yScale = (double)spriteHeight/(double)currentObject->ySize;


		// No reflection
		if (currentObject->xFlip == 1 && currentObject->yFlip == 1)
		{
			// Small item -> render normally
			if (xDraw2 - xDraw < (spriteWidth << 1))
			{
				renderSprite_LRUD_ScaleMode_Slow(screen, screenWidth, screenHeight, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset, currentObject, xScale, yScale);

				return 0;
			}

			renderSprite_LRUD_ScaleMode(screen, screenWidth, screenHeight, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset, currentObject, xScale, yScale);

			return 0;
		}

		// Left-Right reflection
		if (currentObject->xFlip == -1 && currentObject->yFlip == 1)
		{
			// Small item -> render normally
			if (xDraw2 - xDraw < (spriteWidth << 1))
			{
				renderSprite_RLUD_ScaleMode_Slow(screen, screenWidth, screenHeight, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset, currentObject, xScale, yScale);

				return 0;
			}

			renderSprite_RLUD_ScaleMode(screen, screenWidth, screenHeight, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset, currentObject, xScale, yScale);

			return 0;
		}

		// Up-Down reflection
		if (currentObject->xFlip == 1 && currentObject->yFlip == -1)
		{
			// Small item -> render normally
			if (xDraw2 - xDraw < (spriteWidth << 1))
			{
				renderSprite_LRDU_ScaleMode_Slow(screen, screenWidth, screenHeight, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset, currentObject, xScale, yScale);

				return 0;
			}

			renderSprite_LRDU_ScaleMode(screen, screenWidth, screenHeight, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset, currentObject, xScale, yScale);

			return 0;
		}

		// Up-Down and Left-Right reflection
		if (currentObject->xFlip == -1 && currentObject->yFlip == -1)
		{
			// Small item -> render normally
			if (xDraw2 - xDraw < (spriteWidth << 1))
			{
				renderSprite_RLDU_ScaleMode_Slow(screen, screenWidth, screenHeight, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset, currentObject, xScale, yScale);

				return 0;
			}

			renderSprite_RLDU_ScaleMode(screen, screenWidth, screenHeight, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset, currentObject, xScale, yScale);

			return 0;
		}

		return INVALID_DATA;
	}


		
	if (currentRenderMode == TILE_FAST)
	{
		// No reflection
		if (currentObject->xFlip == 1 && currentObject->yFlip == 1)
		{
			renderSprite_LRUD_TileFastMode(screen, screenWidth, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);

			return 0;
		}

		// Left-Right reflection
		if (currentObject->xFlip == -1 && currentObject->yFlip == 1)
		{
			renderSprite_RLUD_TileMode(screen, screenWidth, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);

			return 0;
		}

		// Up-Down reflection
		if (currentObject->xFlip == 1 && currentObject->yFlip == -1)
		{
			renderSprite_LRDU_TileFastMode(screen, screenWidth, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);

			return 0;
		}

		// Up-Down and Left-Right reflection
		if (currentObject->xFlip == -1 && currentObject->yFlip == -1)
		{
			renderSprite_RLDU_TileMode(screen, screenWidth, data, spriteWidth, spriteHeight, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);

			return 0;
		}

		return INVALID_DATA;
	}



	return 0;
}



// renders sprite in tile mode (Render mode 0) with no reflections
int renderSprite_LRUD_TileMode(uint32_t screen[], int screenWidth, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	// set pixel x to difference between real xPos and first xPos on screen to obtain correct starting pixel
	// If difference is large enough, pixelx may be larger than width of sprite,
	// and cannot be less than 0 because then xDraw < xOffset meaning left edge is to the right of right side of screen

	// Start render loop
	int pixely = spriteHeight - 1 - ((yDraw - yOffset) % spriteHeight);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = ((xDraw - xOffset) % spriteWidth);
		int i = xDraw;

		if ((xOffset - i) % 2 != 0)
		{
			if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx++;
			i++;
		}


		for (; i + 1 < xDraw2; i+=2)
		{
			if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (j * screenWidth) + i + 1, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx+=2;

			if (pixelx > spriteWidth - 1)
			{
				pixelx = pixelx - spriteWidth;
			}
		}


		for (; i < xDraw2; i++)
		{
			if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx++;

			if (pixelx > spriteWidth - 1)
			{
				pixelx = pixelx - spriteWidth;
			}
		}

		pixely--;

		if (pixely < 0)
		{
			pixely = spriteHeight - 1;
		}

	}


	return 0;
}


// renders sprite in tile mode (Render mode 0) with Left/Right reflections
int renderSprite_RLUD_TileMode(uint32_t screen[], int screenWidth, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	int sizeOfPixel = sizeof(uint32_t);

	int pixely = spriteHeight - 1 - ((yDraw - yOffset) % spriteHeight);

	for (int j = yDraw; j < yDraw2; j++)
	{
		int i = xDraw;
		int pixelx = spriteWidth - ((xDraw - xOffset) % spriteWidth);

		if ((xOffset - i) % 2 != 0)
		{
			if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx--;
			i++;
		}


		for (; i + 1 < xDraw2; i += 2)
		{
			if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (j * screenWidth) + i + 1, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx -= 2;

			if (pixelx < 0)
			{
				pixelx = spriteWidth + pixelx;
			}
		}


		if (i < xDraw2)
		{
			if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}
		}

		pixely--;

		if (pixely < 0)
		{
			pixely = spriteHeight - 1;
		}

	}

	return 0;
}


// renders sprite in tile mode (Render mode 0) with Up/Down reflections
int renderSprite_LRDU_TileMode(uint32_t screen[], int screenWidth, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	int sizeOfPixel = sizeof(uint32_t);

	int pixely = ((yDraw - yOffset) % spriteHeight);

	for (int j = yDraw; j < yDraw2; j++)
	{
		int i = xDraw;
		int pixelx = ((xDraw - xOffset) % spriteWidth);

		if ((xOffset - i) % 2 != 0)
		{
			if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx++;
			i++;
		}


		for (; i + 1 < xDraw2; i += 2)
		{
			if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (j * screenWidth) + i + 1, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx += 2;

			if (pixelx >= spriteWidth)
			{
				pixelx = pixelx - spriteWidth;
			}
		}


		if (i < xDraw2)
		{
			if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}
		}

		pixely++;

		if (pixely >= spriteHeight)
		{
			pixely = 0;
		}

	}

	return 0;
}


// renders sprite in tile mode (Render mode 0) with Left/Right and Up/Down reflections
int renderSprite_RLDU_TileMode(uint32_t screen[], int screenWidth, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	int sizeOfPixel = sizeof(uint32_t);

	int pixely = ((yDraw - yOffset) % spriteHeight);

	for (int j = yDraw; j < yDraw2; j++)
	{
		int i = xDraw;
		int pixelx = spriteWidth - ((xDraw - xOffset) % spriteWidth);

		if ((xOffset - i) % 2 != 0)
		{
			if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx--;
			i++;
		}


		for (; i + 1 < xDraw2; i += 2)
		{
			if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (j * screenWidth) + i + 1, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx -= 2;

			if (pixelx < 0)
			{
				pixelx = spriteWidth + pixelx;
			}
		}


		if (i < xDraw2)
		{
			if ((uint32_t)data[((pixely << 2) * spriteWidth) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}
		}

		pixely++;

		if (pixely >= spriteHeight)
		{
			pixely = 0;
		}

	}

	return 0;
}



int renderSprite_LRUD_ScaleMode(uint32_t screen[], int screenWidth, int screenHeight, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject, double xScale, double yScale)
{
	// Big item -> render optimised
	int sizeOfPixel = sizeof(uint32_t);

	double pixely = (currentObject->ySize - 1 - (yDraw - yOffset)) * yScale;

	for (int i = yDraw; i < yDraw2; i++)
	{
		double pixelx = xScale * ((xDraw - xOffset) % currentObject->xSize);

		int k = xDraw;

		// Align to first row of # pixels
		for (; (xOffset - k) % 4 != 0; k++)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx += xScale;
		}

		// Memcopy # pixels at a time
		for (; k + 3 < xDraw2; k+=4)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (i * screenWidth) + k + 1, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (i * screenWidth) + k + 2, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (i * screenWidth) + k + 3, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx += (xScale * 4);
		}

		// Fill in up to # - 1 pixels at right side of screen
		for (; k < xDraw2; k++)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx += xScale;
		}

		pixely -= yScale;
	}

	return 0;
}


int renderSprite_LRUD_ScaleMode_Slow(uint32_t screen[], int screenWidth, int screenHeight, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject, double xScale, double yScale)
{
	int sizeOfPixel = sizeof(uint32_t);

	uint32_t *hexValue;

	double pixely = (currentObject->ySize - 1 - (yDraw - yOffset)) * yScale;

	for (int i = yDraw; i < yDraw2; i++)
	{
		double pixelx = xScale * ((xDraw - xOffset) % currentObject->xSize);

		for (int k = xDraw; k < xDraw2; k++)
		{
			double alpha = data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] / 0xFF;

			if (alpha > 0.0)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx += xScale;
		}

		pixely -= yScale;
	}



	return 0;
}


int renderSprite_RLUD_ScaleMode(uint32_t screen[], int screenWidth, int screenHeight, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject, double xScale, double yScale)
{
	// Big item -> render optimised
	int sizeOfPixel = sizeof(uint32_t);

	double pixely = (currentObject->ySize - 1 - (yDraw - yOffset)) * yScale;

	for (int i = yDraw; i < yDraw2; i++)
	{
		double pixelx = (currentObject->xSize - 1 - (xDraw - xOffset)) * xScale;

		int k = xDraw;

		// Align to first row of # pixels
		for (; (xOffset - k) % 4 != 0; k++)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx -= xScale;
		}

		// Memcopy # pixels at a time
		for (; k + 3 < xDraw2; k+=4)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (i * screenWidth) + k + 1, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (i * screenWidth) + k + 2, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (i * screenWidth) + k + 3, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx -= (xScale * 4);
		}

		// Fill in up to # - 1 pixels at right side of screen
		for (; k < xDraw2; k++)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx -= xScale;
		}

		pixely -= yScale;
	}

	return 0;
}

int renderSprite_RLUD_ScaleMode_Slow(uint32_t screen[], int screenWidth, int screenHeight, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject, double xScale, double yScale)
{

	int sizeOfPixel = sizeof(uint32_t);

	double pixely = (currentObject->ySize - 1 - (yDraw - yOffset)) * yScale;

	// Small item -> render normally
	for (int i = yDraw; i < yDraw2; i++)
	{
		double pixelx = (currentObject->xSize - 1 - (xDraw - xOffset)) * xScale;

		for (int k = xDraw; k < xDraw2; k++)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx -= xScale;
		}

		pixely -= yScale;
	}

	return 0;
}


int renderSprite_LRDU_ScaleMode(uint32_t screen[], int screenWidth, int screenHeight, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject, double xScale, double yScale)
{
	int sizeOfPixel = sizeof(uint32_t);

	double pixely =  yScale * ((yDraw - yOffset) % currentObject->ySize);


	// Big item -> render optimised
	for (int i = yDraw; i < yDraw2; i++)
	{
		double pixelx = xScale * ((xDraw - xOffset) % currentObject->xSize);

		int k = xDraw;

		// Align to first row of # pixels
		for (; (xOffset - k) % 4 != 0; k++)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx += xScale;
		}

		// Memcopy # pixels at a time
		for (; k + 3 < xDraw2; k+=4)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (i * screenWidth) + k + 1, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (i * screenWidth) + k + 2, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (i * screenWidth) + k + 3, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx += (xScale * 4);
		}

		// Fill in up to # - 1 pixels at right side of screen
		for (; k < xDraw2; k++)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx += xScale;
		}

		pixely += yScale;
	}


	return 0;
}


int renderSprite_LRDU_ScaleMode_Slow(uint32_t screen[], int screenWidth, int screenHeight, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject, double xScale, double yScale)
{
	int sizeOfPixel = sizeof(uint32_t);

	double pixely =  yScale * ((yDraw - yOffset) % currentObject->ySize);

	for (int i = yDraw; i < yDraw2; i++)
	{
		double pixelx = xScale * ((xDraw - xOffset) % currentObject->xSize);

		for (int k = xDraw; k < xDraw2; k++)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx += xScale;
		}

		pixely += yScale;
	}


	return 0;
}


int renderSprite_RLDU_ScaleMode(uint32_t screen[], int screenWidth, int screenHeight, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject, double xScale, double yScale)
{
	int sizeOfPixel = sizeof(uint32_t);

	double pixely =  yScale * ((yDraw - yOffset) % currentObject->ySize);


	// Big item -> render optimised
	for (int i = yDraw; i < yDraw2; i++)
	{
		double pixelx = (currentObject->xSize - 1 - (xDraw - xOffset)) * xScale;

		int k = xDraw;

		// Align to first row of # pixels
		for (; (xOffset - k) % 2 != 0; k++)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx -= xScale;
		}

		// Memcopy # pixels at a time
		for (; k + 3 < xDraw2; k+=2)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				memcpy(screen + (i * screenWidth) + k + 1, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				//memcpy(screen + (i * screenWidth) + k + 2, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
				//memcpy(screen + (i * screenWidth) + k + 3, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx -= (xScale * 2);
		}

		// Fill in up to # - 1 pixels at right side of screen
		for (; k < xDraw2; k++)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx -= xScale;
		}

		pixely += yScale;
	}

	return 0;
}


int renderSprite_RLDU_ScaleMode_Slow(uint32_t screen[], int screenWidth, int screenHeight, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject, double xScale, double yScale)
{
	int sizeOfPixel = sizeof(uint32_t);

	double pixely =  yScale * ((yDraw - yOffset) % currentObject->ySize);


	// Small item -> render normally
	for (int i = yDraw; i < yDraw2; i++)
	{
		double pixelx = (currentObject->xSize - 1 - (xDraw - xOffset)) * xScale;

		for (int k = xDraw; k < xDraw2; k++)
		{
			if ((uint32_t)data[(((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (i * screenWidth) + k, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx -= xScale;
		}

		pixely += yScale;
	}

	return 0;
}


// renders sprite in tile mode (Render mode 5) with no reflections
int renderSprite_LRUD_TileFastMode(uint32_t screen[], int screenWidth, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);


	// Start render loop
	int pixely = spriteHeight - 1 - (((yDraw - yOffset) % spriteHeight));


	// Render sprite to screen
	for (int i = yDraw; i < yDraw2; i++)
	{
		int pixelx = ((xDraw - xOffset) % spriteWidth);
		int k = xDraw;

		memcpy(screen + (i * screenWidth) + k, data + ((pixely << 2) * spriteWidth) + (pixelx << 2), sizeOfPixel * (spriteWidth - pixelx));

		for (k = spriteWidth - pixelx; k + spriteWidth < xDraw2; k += spriteWidth)
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


// renders sprite in tile mode (Render mode 5) with no reflections
int renderSprite_LRDU_TileFastMode(uint32_t screen[], int screenWidth, unsigned char *data, int spriteWidth, int spriteHeight, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	int j, i;

	// Start render loop
	int pixely = ((yDraw - yOffset) % spriteHeight);


	for (j = yDraw; j < yDraw2; j++)
	{
		int pixelx = ((xDraw - xOffset) % spriteWidth);
		i = xDraw;

		memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), sizeOfPixel * (spriteWidth - pixelx));

		pixelx = 0;

		for (; i + spriteWidth - 1 < xDraw2; i += spriteWidth)
		{
			memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), spriteWidth * sizeOfPixel);
		}

			
		memcpy(screen + (j * screenWidth) + i, data + (((int)pixely << 2) * spriteWidth) + ((int)pixelx << 2), ((xDraw2 - i) % spriteWidth) * sizeOfPixel);
			

		pixely++;

		if (pixely > spriteHeight - 1)
		{
			pixely =0;
		}

	}


	return 0;
}


int renderBackGroundSprite(uint32_t screen[], int screenWidth, int screenHeight, World *gameWorld)
{
	if (gameWorld == NULL || gameWorld->drawBackGround == 0)
	{
		return MISSING_DATA;
	}

	Sprite *spritePtr;
	spritePtr = gameWorld->bgSpriteBuffer;

	if (spritePtr == NULL)
	{
		return MISSING_DATA;
	}

	// Load up data buffers for sprite to be rendered
	int spriteWidth = spritePtr->width;
	int spriteHeight = spritePtr->height;
	unsigned char *data = spritePtr->spriteData;		// Instead of copying the data, use a pointer to reduce memory overhead

	// Locate object on screen
	int xOffset = gameWorld->cameraX * gameWorld->bgParallax;
	int yOffset = gameWorld->cameraY * gameWorld->bgParallax;
	int xOffset2 = xOffset + spritePtr->width;
	int yOffset2 = yOffset + spritePtr->height;

	int xDraw2, xDraw, yDraw2, yDraw;


	//	RenderMode 0: Tile Mapping								(Sprite will tile across entire bounding box of object)
	//	RenderMode 1: Single sprite render 						(Renders single instance of sprite directly to bottom-left corner)
	//  RenderMode 2: Row Parallax

	if (spritePtr->RenderMode == 1)
	{
		xDraw = correct(xOffset, 0, screenWidth - 1);
		yDraw = correct(yOffset, 0, screenHeight - 1);
		xDraw2 = correct(spriteWidth + xOffset, 0, screenWidth - 1);
		yDraw2 = correct(spriteHeight + yOffset, 0, screenHeight - 1);
	}
	else
	{
		xDraw = 0;
		yDraw = 0;
		xDraw2 = screenWidth - 1;
		yDraw2 = screenHeight - 1;

		// Correct y bounds if in tile mode 0
		if (gameWorld->bgTileVertically == 0)
		{
			if (spriteHeight - yOffset < screenHeight)
			{
				yDraw2 = spriteHeight - yOffset;
			}

		}
	}


	size_t sizeOfPixel = sizeof(uint32_t);
	int i, k;

	int pixely = spriteHeight - 1 - (((yDraw + yOffset) % spriteHeight));
	int pixelx = 0;


	// Render sprite to screen
	for (i = yDraw; i < yDraw2; i++)
	{
		if (spritePtr->RenderMode == 2 && (i + yOffset) % gameWorld->bgParallaxChunkSize == 0)
		{
			xOffset = gameWorld->cameraX * (gameWorld->bgParallax - (gameWorld->bgChunkParallax * (i + yOffset) ) );
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


int correct(int offset, int bound1, int bound2)
{
	if (offset < bound1)
	{
		return bound1;
	}

	if (offset > bound2)
	{
		return bound2;
	}

	return offset;
}


int modulo(int x, int N)
{
    return (x % N + N) % N;
}


int cleanRenderer(World *gameWorld, uint32_t screen[], int width, int height)
{
	memset(screen, 0x55, sizeof(uint32_t) * width * height);


	if (gameWorld != NULL)
	{
		gameWorld->drawnObjects = 0;
	}

	return 0;
}



// Old method of rendering - renders each pixel one at a time
// New method renders in 2 pixel rows, meaning less fidelity on x-axis (However this is in line with art style)
//// No reflection
//if (currentObject->xFlip == 1 && currentObject->yFlip == 1)
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
//if (currentObject->xFlip == -1 && currentObject->yFlip == 1)
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
//if (currentObject->xFlip == 1 && currentObject->yFlip == -1)
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
//if (currentObject->xFlip == -1 && currentObject->yFlip == -1)
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
