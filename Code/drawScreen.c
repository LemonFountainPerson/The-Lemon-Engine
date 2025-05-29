#include "drawScreen.h"


// Controls what symbol is printed for each tile
static uint32_t tileMap[32] = {0x00AA00AA, 0xFFFF0088, 0xFF0088FF, 0xFF552299, 0xFF00AA00, 0xFFBB55AA, 0xFFD0CC00, 0xFF88FF44,
								0xFF77DD11, 0xFF11BB44, 0xFF0F449F, 0xFF000010, 0xFF001010, 0xFF101010, 0xFF1C1010, 0xFF101C10,
								0xFF2C1010, 0xFF102C10, 0xFF10103C, 0xFF40201C, 0xFF019E30, 0xFF0514CE, 0xFFE86AA3, 0xFF0A4321,
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


int drawObjects(Layer drawLayer, uint32_t screen[], int width, int height, World *gameWorld)
{
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
	if (currentObject == NULL)
	{
		return MISSING_DATA;
	}

	if (currentObject->layer != drawLayer || currentObject->objectRenderMode == DO_NOT_RENDER)
	{
		return ACTION_DISABLED;
	}
	
	// Sprite
	renderObjectSprite(screen, width, height, gameWorld, currentObject);

	if (gameWorld->drawHitboxes == 0)
	{
		return 0;
	}

	int tile = tileMap[currentObject->objectID % 32];

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
	// WARNING! This function is really messy and not easy to read: All variations of rendering has its own copy of the
	// main render loop, even extremely similar ones. This was done to squeeze out performance (at the expense of a bit of memory)
	// As such, readability was sacrificed somewhat. Continue at your own risk!

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

	// Locate object on screen
	int xOffset = currentObject->xPos - gameWorld->cameraX + (H_RESOLUTION >> 1);
	int yOffset = currentObject->yPos - gameWorld->cameraY + (V_RESOLUTION >> 1);
	int xOffset2 = xOffset + currentObject->xSize;
	int yOffset2 = yOffset + currentObject->ySize;
	

	int xDraw2, xDraw, yDraw2, yDraw, reflect;

	// Set draw locations on screen
	if (currentRenderMode == SINGLE || currentRenderMode == SINGLE_FULL_ALPHA || currentRenderMode == SINGLE_FAST)
	{
		// Find center of object and then center sprite on that point
		int centerX = (xOffset2 + xOffset) >> 1;
		int centerY = (yOffset2 + yOffset) >> 1;
		xOffset = centerX - (spritePtr->width >> 1);
		yOffset = centerY - (spritePtr->width >> 1);

		xDraw = correct(centerX - (spritePtr->width >> 1), 0, screenWidth - 1);
		yDraw = correct(centerY - (spritePtr->height >> 1), 0, screenHeight - 1);
		xDraw2 = correct(centerX + (spritePtr->width >> 1), 0, screenWidth - 1);
		yDraw2 = correct(centerY + (spritePtr->height >> 1), 0, screenHeight - 1);

		// Make sure the sprite position is valid on screen to avoid a crash related to assuming the position is valid 
		if (xOffset >= screenWidth || spritePtr->width + xOffset < 0 || yOffset >= screenHeight || spritePtr->height + yOffset < 0)
		{
			return INVALID_DATA;
		}
	}
	else
	{
		xDraw = correct(xOffset, 0, screenWidth - 1);
		yDraw = correct(yOffset, 0, screenHeight - 1);
		xDraw2 = correct(currentObject->xSize + xOffset, 0, screenWidth - 1);
		yDraw2 = correct(currentObject->ySize + yOffset, 0, screenHeight - 1);

		// Make sure the sprite position is valid on screen to avoid a crash related to assuming the position is valid 
		if (xOffset >= screenWidth || currentObject->xSize + xOffset < 0 || yOffset >= screenHeight || currentObject->ySize + yOffset < 0)
		{
			return INVALID_DATA;
		}
	}


	switch(currentObject->layer)
	{

	case HUD:
		if (gameWorld->drawHud == 0 || gameWorld->drawnHudElements >= MAX_HUD_ELEMENTS)
		{
			return ACTION_DISABLED;
		}

		gameWorld->drawnHudElements++;
		break;

	case PARTICLES:
		if (gameWorld->drawParticles == 0 || gameWorld->drawnParticles >= MAX_PARTICLES)
		{
			return ACTION_DISABLED;
		}

		gameWorld->drawnParticles++;
		break;

	default:
		if (gameWorld->drawObjects == 0 || gameWorld->drawnObjects >= MAX_OBJECTS)
		{
			return ACTION_DISABLED;
		}

		gameWorld->drawnObjects++;
		break;
	}


	// Render sprite to screen
	switch(currentRenderMode)
	{
		case SINGLE:
		case TILE:
		{
			if (currentObject->xFlip == 1 && currentObject->yFlip == 1)
			{
				renderSprite_LRUD_Tile(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);
			}
			else if (currentObject->xFlip == -1 && currentObject->yFlip == 1)
			{
				renderSprite_RLUD_Tile(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);
			}
			else if (currentObject->xFlip == 1 && currentObject->yFlip == -1)
			{
				renderSprite_LRDU_Tile(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);
			}
			else
			{
				renderSprite_RLDU_Tile(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);
			}

			return 0;
		} break;


		case TILE_FULL_ALPHA:
		case SINGLE_FULL_ALPHA:
		{
			if (currentObject->xFlip == 1 && currentObject->yFlip == 1)
			{
				renderSprite_LRUD_FullAlpha(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);
			}
			else if (currentObject->xFlip == -1 && currentObject->yFlip == 1)
			{
				renderSprite_RLUD_FullAlpha(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);
			}
			else if (currentObject->xFlip == 1 && currentObject->yFlip == -1)
			{
				renderSprite_LRDU_FullAlpha(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);
			}
			else
			{
				renderSprite_RLDU_FullAlpha(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);
			}

			return 0;
		} break;


		case SINGLE_FAST:
		case TILE_FAST:
		{
			if (currentObject->xFlip == 1 && currentObject->yFlip == 1)
			{
				renderSprite_LRUD_TileFast(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);
			}
			else if (currentObject->xFlip == -1 && currentObject->yFlip == 1)
			{
				renderSprite_RLUD_Tile(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);
			}
			else if (currentObject->xFlip == 1 && currentObject->yFlip == -1)
			{
				renderSprite_LRDU_TileFast(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);
			}
			else
			{
				renderSprite_RLDU_Tile(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset);
			}

			return 0;
		} break;


		case SCALE:
		{
			// No reflection
			if (currentObject->xFlip == 1 && currentObject->yFlip == 1)
			{
				renderSprite_LRUD_Scale(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset, currentObject);
			}
			else if (currentObject->xFlip == -1 && currentObject->yFlip == 1)
			{
				renderSprite_RLUD_Scale(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset, currentObject);
			}
			else if (currentObject->xFlip == 1 && currentObject->yFlip == -1)
			{
				renderSprite_LRDU_Scale(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset, currentObject);
			}
			else
			{
				renderSprite_RLDU_Scale(screen, screenWidth, spritePtr, xDraw, xDraw2, yDraw, yDraw2, xOffset, yOffset, currentObject);
			}

			return 0;
		} break;


		case SCALE_FULL_ALPHA:
		{


			return 0;
		} break;

	}


	return 0;
}


int renderSprite_LRUD_FullAlpha(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	uint32_t hexValue;

	// Start render loop
	int pixely = spritePtr->height - 1 - ((yDraw - yOffset) % spritePtr->height);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = ((xDraw - xOffset) % spritePtr->width);

		for (int i = xDraw; i < xDraw2; i++)
		{
			hexValue = spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] << 24 | 
						spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 2] << 16 | 
						spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 1] << 8 | 
						spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) ];

			screen[(j * screenWidth) + i] = blendPixel(screen[(j * screenWidth) + i], hexValue);

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


int renderSprite_RLUD_FullAlpha(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	uint32_t hexValue;

	// Start render loop
	int pixely = spritePtr->height - 1 - ((yDraw - yOffset) % spritePtr->height);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = spritePtr->width - 1 - ((xDraw - xOffset) % spritePtr->width);

		for (int i = xDraw; i < xDraw2; i++)
		{
			hexValue = spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] << 24 | 
						spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 2] << 16 | 
						spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 1] << 8 | 
						spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) ];

			screen[(j * screenWidth) + i] = blendPixel(screen[(j * screenWidth) + i], hexValue);

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


int renderSprite_LRDU_FullAlpha(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	uint32_t hexValue;

	// Start render loop
	int pixely = ((yDraw - yOffset) % spritePtr->height);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = ((xDraw - xOffset) % spritePtr->width);

		for (int i = xDraw; i < xDraw2; i++)
		{
			hexValue = spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] << 24 | 
						spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 2] << 16 | 
						spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 1] << 8 | 
						spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) ];

			screen[(j * screenWidth) + i] = blendPixel(screen[(j * screenWidth) + i], hexValue);

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


int renderSprite_RLDU_FullAlpha(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	uint32_t hexValue;

	// Start render loop
	int pixely = ((yDraw - yOffset) % spritePtr->height);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = spritePtr->width - 1 - ((xDraw - xOffset) % spritePtr->width);

		for (int i = xDraw; i < xDraw2; i++)
		{
			hexValue = spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] << 24 | 
						spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 2] << 16 | 
						spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 1] << 8 | 
						spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) ];

			screen[(j * screenWidth) + i] = blendPixel(screen[(j * screenWidth) + i], hexValue);

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

	uint8_t blue = (( ((screenPixel & 0x0000FF00) >> 8) * (1.0 - alpha)) + (((inputPixel & 0x0000FF00) >> 8) * alpha));; 

	uint8_t green = (( ((screenPixel & 0x000000FF)) * (1.0 - alpha)) + ((inputPixel & 0x000000FF) * alpha));; 

	return (0x00 | red << 16 | blue << 8 | green);
}


// renders sprite in tile mode (Render mode 0) with no reflections  
int renderSprite_LRUD_Tile(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	// Start render loop
	int pixely =  spritePtr->height - 1 - ((yDraw - yOffset) % spritePtr->height);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = ((xDraw - xOffset) % spritePtr->width);
		int i = xDraw;

		if ((xOffset - i) % 2 != 0)
		{
			if ((uint32_t)spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
			}

			pixelx++;
			i++;
		}


		for (; i + 1 < xDraw2; i+=2)
		{
			if ((uint32_t)spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
				memcpy(screen + (j * screenWidth) + i + 1, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
			}

			pixelx += 2;

			if (pixelx > spritePtr->width - 1)
			{
				pixelx = pixelx - spritePtr->width;
			}
		}


		if (i < xDraw2)
		{
			if ((uint32_t)spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
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
int renderSprite_RLUD_Tile(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	int sizeOfPixel = sizeof(uint32_t);

	int pixely =  spritePtr->height - 1 - ((yDraw - yOffset) %  spritePtr->height);

	for (int j = yDraw; j < yDraw2; j++)
	{
		int i = xDraw;
		int pixelx = spritePtr->width - ((xDraw - xOffset) % spritePtr->width);

		if ((xOffset - i) % 2 != 0)
		{
			if ((uint32_t)spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
			}

			pixelx--;
			i++;
		}


		for (; i + 1 < xDraw2; i += 2)
		{
			if ((uint32_t)spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
				memcpy(screen + (j * screenWidth) + i + 1, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
			}

			pixelx -= 2;

			if (pixelx < 0)
			{
				pixelx = spritePtr->width + pixelx;
			}
		}


		if (i < xDraw2)
		{
			if ((uint32_t)spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
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
int renderSprite_LRDU_Tile(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	int sizeOfPixel = sizeof(uint32_t);

	int pixely = ((yDraw - yOffset) %  spritePtr->height);

	for (int j = yDraw; j < yDraw2; j++)
	{
		int i = xDraw;
		int pixelx = ((xDraw - xOffset) % spritePtr->width);

		if ((xOffset - i) % 2 != 0)
		{
			if ((uint32_t)spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
			}

			pixelx++;
			i++;
		}


		for (; i + 1 < xDraw2; i += 2)
		{
			if ((uint32_t)spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
				memcpy(screen + (j * screenWidth) + i + 1, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
			}

			pixelx += 2;

			if (pixelx >= spritePtr->width)
			{
				pixelx = pixelx - spritePtr->width;
			}
		}


		if (i < xDraw2)
		{
			if ((uint32_t)spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
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
int renderSprite_RLDU_Tile(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	int sizeOfPixel = sizeof(uint32_t);

	int pixely = ((yDraw - yOffset) %  spritePtr->height);

	for (int j = yDraw; j < yDraw2; j++)
	{
		int i = xDraw;
		int pixelx = spritePtr->width - ((xDraw - xOffset) % spritePtr->width);

		if ((xOffset - i) % 2 != 0)
		{
			if ((uint32_t)spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
			}

			pixelx--;
			i++;
		}


		for (; i + 1 < xDraw2; i += 2)
		{
			if ((uint32_t)spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
				memcpy(screen + (j * screenWidth) + i + 1, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
			}

			pixelx -= 2;

			if (pixelx < 0)
			{
				pixelx = spritePtr->width + pixelx;
			}
		}


		if (i < xDraw2)
		{
			if ((uint32_t)spritePtr->spriteData[((pixely << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screen + (j * screenWidth) + i, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
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


// renders sprite in tile mode (Render mode 5) with no reflections
int renderSprite_LRUD_TileFast(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);


	// Start render loop
	int pixely = spritePtr->height - 1 - (((yDraw - yOffset) % spritePtr->height));


	// Render sprite to screen
	for (int i = yDraw; i < yDraw2; i++)
	{
		int pixelx = ((xDraw - xOffset) % spritePtr->width);
		int k = xDraw;

		if (xDraw2 - xDraw > spritePtr->width)
		{
			memcpy(screen + (i * screenWidth) + k, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel * (spritePtr->width - pixelx));
			k += (spritePtr->width - pixelx);


			for (; k + spritePtr->width - 1 < xDraw2; k += spritePtr->width)
			{
				memcpy(screen + (i * screenWidth) + k, spritePtr->spriteData + ((pixely << 2) * spritePtr->width), spritePtr->width * sizeOfPixel);
			}
		}

		memcpy(screen + (i * screenWidth) + k, spritePtr->spriteData + ((pixely << 2) * spritePtr->width), ((xDraw2 - k) % spritePtr->width) * sizeOfPixel);

		pixely--;

		if (pixely < 0)
		{
			pixely = spritePtr->height - 1;
		}
	}


	return 0;
}


// renders sprite in tile mode (Render mode 5) with Up/Down reflection
int renderSprite_LRDU_TileFast(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	int j, i;

	// Start render loop
	int pixely = ((yDraw - yOffset) % spritePtr->height);


	for (j = yDraw; j < yDraw2; j++)
	{
		int pixelx = ((xDraw - xOffset) % spritePtr->width);
		int k = xDraw;

		if (xDraw2 - xDraw > spritePtr->width)
		{
			memcpy(screen + (i * screenWidth) + k, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel * (spritePtr->width - pixelx));
			k += (spritePtr->width - pixelx);


			for (; k + spritePtr->width - 1 < xDraw2; k += spritePtr->width)
			{
				memcpy(screen + (i * screenWidth) + k, spritePtr->spriteData + ((pixely << 2) * spritePtr->width), spritePtr->width * sizeOfPixel);
			}
		}

		memcpy(screen + (i * screenWidth) + k, spritePtr->spriteData + ((pixely << 2) * spritePtr->width), ((xDraw2 - k) % spritePtr->width) * sizeOfPixel);

		pixely++;

		if (pixely > spritePtr->height - 1)
		{
			pixely =0;
		}

	}


	return 0;
}


int renderSprite_LRUD_Scale(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject)
{
	int sizeOfPixel = sizeof(uint32_t);

	double xScale = (double)spritePtr->width/(double)currentObject->xSize;
	double yScale = (double)spritePtr->height/(double)currentObject->ySize;
	double xScale2 = (double)currentObject->xSize / (double)spritePtr->width;

	double pixely = (currentObject->ySize - 1 - (yDraw - yOffset)) * yScale;

	uint32_t hexValue;

	for (int i = yDraw; i < yDraw2; i++)
	{
		int pixelx = 0;

		int intPixelY = (int)pixely;

		double k = xOffset;


		if (k < 0)
		{
			for (; k < 0; k += xScale2)
			{
				pixelx++;
			}

			pixelx--;

			if ((uint32_t)spritePtr->spriteData[((intPixelY << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				for (int j = xDraw; j < xScale2; j++)
				{
					memcpy(screen + (i * screenWidth) + j, spritePtr->spriteData + ((intPixelY << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx++;
		}
		

		for (; k + xScale2 - 1.0 < (double)xDraw2; k += xScale2)
		{
			if ((uint32_t)spritePtr->spriteData[((intPixelY << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				for (int j = 0; j < xScale2; j++)
				{
					memcpy(screen + (i * screenWidth) + (int)k + j, spritePtr->spriteData + ((intPixelY << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx++;
		}

		pixely -= yScale;
	}

	return 0;
}


int renderSprite_LRUD_Scale_Slow(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject)
{
	int sizeOfPixel = sizeof(uint32_t);

	double xScale = (double)spritePtr->width/(double)currentObject->xSize;
	double yScale = (double)spritePtr->height/(double)currentObject->ySize;

	uint32_t *hexValue;

	double pixely = (currentObject->ySize - 1 - (yDraw - yOffset)) * yScale;

	for (int i = yDraw; i < yDraw2; i++)
	{
		double pixelx = xScale * ((xDraw - xOffset) % currentObject->xSize);

		for (int k = xDraw; k < xDraw2; k++)
		{
			double alpha = spritePtr->spriteData[(((int)pixely << 2) * spritePtr->width) + ((int)pixelx << 2) + 3] / 0xFF;

			if (alpha > 0.0)
			{
				memcpy(screen + (i * screenWidth) + k, spritePtr->spriteData + (((int)pixely << 2) * spritePtr->width) + ((int)pixelx << 2), sizeOfPixel);
			}

			pixelx += xScale;
		}

		pixely -= yScale;
	}



	return 0;
}


int renderSprite_RLUD_Scale(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject)
{
	int sizeOfPixel = sizeof(uint32_t);

	double xScale = (double)spritePtr->width/(double)currentObject->xSize;
	double yScale = (double)spritePtr->height/(double)currentObject->ySize;
	double xScale2 = (double)currentObject->xSize / (double)spritePtr->width;

	double pixely = (currentObject->ySize - 1 - (yDraw - yOffset)) * yScale;


	for (int i = yDraw; i < yDraw2; i++)
	{
		int pixelx = (int)((currentObject->xSize - 1) * xScale);

		int intPixelY = (int)pixely;

		double k = xOffset;


		if (k < 0)
		{
			for (; k < 0; k += xScale2)
			{
				pixelx--;
			}

			pixelx++;

			if ((uint32_t)spritePtr->spriteData[((intPixelY << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				for (int j = xDraw; j < xScale2; j++)
				{
					memcpy(screen + (i * screenWidth) + j, spritePtr->spriteData + ((intPixelY << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx--;
		}
		

		for (; k + xScale2 - 1.0 < (double)xDraw2; k += xScale2)
		{

			if ((uint32_t)spritePtr->spriteData[((intPixelY << 2) * spritePtr->width) + (pixelx << 2) + 3] > 0x00)
			{
				for (int j = 0; j < xScale2; j++)
				{
					memcpy(screen + (i * screenWidth) + (int)k + j, spritePtr->spriteData + ((intPixelY << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx--;
		}

		pixely -= yScale;
	}

	return 0;
}


int renderSprite_LRDU_Scale(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject)
{
	int sizeOfPixel = sizeof(uint32_t);

	double xScale = (double)spritePtr->width/(double)currentObject->xSize;
	double yScale = (double)spritePtr->height/(double)currentObject->ySize;
	double xScale2 = (double)currentObject->xSize / (double)spritePtr->width;

	double pixely = (yDraw - yOffset) * yScale;


	for (int i = yDraw; i < yDraw2; i++)
	{
		double pixelx = 0;

		int intPixelY = (int)pixely;
		int intPixelx = (int)pixelx;

		double k = xOffset;


		if (k < 0)
		{
			for (; k < 0; k += xScale2)
			{
				pixelx++;
			}

			intPixelx = (int)pixelx - 1;

			if ((uint32_t)spritePtr->spriteData[((intPixelY << 2) * spritePtr->width) + (intPixelx << 2) + 3] > 0x00)
			{
				for (int j = xDraw; j < xScale2; j++)
				{
					memcpy(screen + (i * screenWidth) + j, spritePtr->spriteData + ((intPixelY << 2) * spritePtr->width) + (intPixelx << 2), sizeOfPixel);
				}
				
			}
		}
		

		// Memcopy # pixels at a time
		for (; k + xScale2 - 1.0 < (double)xDraw2; k += xScale2)
		{
			intPixelx = (int)pixelx;

			if ((uint32_t)spritePtr->spriteData[((intPixelY << 2) * spritePtr->width) + (intPixelx << 2) + 3] > 0x00)
			{
				for (int j = 0; j < xScale2; j++)
				{
					memcpy(screen + (i * screenWidth) + (int)k + j, spritePtr->spriteData + ((intPixelY << 2) * spritePtr->width) + (intPixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx++;
		}

		pixely += yScale;
	}

	return 0;
}


int renderSprite_RLDU_Scale(uint32_t screen[], int screenWidth, Sprite *spritePtr, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, Object *currentObject)
{
	int sizeOfPixel = sizeof(uint32_t);

	double xScale = (double)spritePtr->width/(double)currentObject->xSize;
	double yScale = (double)spritePtr->height/(double)currentObject->ySize;
	double xScale2 = (double)currentObject->xSize / (double)spritePtr->width;

	double pixely = (yDraw - yOffset) * yScale;


	for (int i = yDraw; i < yDraw2; i++)
	{
		double pixelx = currentObject->xSize - 1;

		int intPixelY = (int)pixely;
		int intPixelx = (int)pixelx;

		double k = xOffset;


		if (k < 0)
		{
			for (; k < 0; k += xScale2)
			{
				pixelx--;
			}

			intPixelx = (int)pixelx + 1;

			if ((uint32_t)spritePtr->spriteData[((intPixelY << 2) * spritePtr->width) + (intPixelx << 2) + 3] > 0x00)
			{
				for (int j = xDraw; j < xScale2; j++)
				{
					memcpy(screen + (i * screenWidth) + j, spritePtr->spriteData + ((intPixelY << 2) * spritePtr->width) + (intPixelx << 2), sizeOfPixel);
				}
				
			}
		}
		

		// Memcopy # pixels at a time
		for (; k + xScale2 - 1.0 < (double)xDraw2; k += xScale2)
		{
			intPixelx = (int)pixelx;

			if ((uint32_t)spritePtr->spriteData[((intPixelY << 2) * spritePtr->width) + (intPixelx << 2) + 3] > 0x00)
			{
				for (int j = 0; j < xScale2; j++)
				{
					memcpy(screen + (i * screenWidth) + (int)k + j, spritePtr->spriteData + ((intPixelY << 2) * spritePtr->width) + (intPixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx--;
		}

		pixely += yScale;
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

	size_t sizeOfPixel = sizeof(uint32_t);
	int i, k;

	int pixely = spriteHeight - 1 - (((yDraw + yOffset) % spriteHeight));
	int pixelx = 0;


	//	RenderMode 0: Tile Mapping								(Sprite will tile across entire bounding box of object)
	//	RenderMode 1: Single sprite render 						(Renders single instance of sprite directly to bottom-left corner)
	//  RenderMode 2: Row Parallax

	if (spritePtr->RenderMode == 1)
	{
		xDraw = correct(0 - xOffset, 0, screenWidth - 1);
		yDraw = correct(0 - yOffset, 0, screenHeight - 1);
		xDraw2 = correct(spriteWidth - xOffset, 0, screenWidth - 1);
		yDraw2 = correct(spriteHeight - yOffset, 0, screenHeight - 1);

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

	// Correct y bounds if in tile mode 0
	if (gameWorld->bgTileVertically == 0)
	{
		if (spriteHeight - yOffset < screenHeight)
		{
			yDraw2 = spriteHeight - yOffset;
		}

	}


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
	memset(screen, 0x00, sizeof(uint32_t) * width * height);


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
