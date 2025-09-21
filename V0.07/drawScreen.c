#include "drawScreen.h"


// Controls what symbol is printed for each tile
static uint32_t tileMap[32] = {0x00AA00AA, 0xFFF33030, 0xFFF08022, 0xFF70F020, 0xFF22F0E0, 0xFF2230F0, 0xFFD022F0, 0xFFF02290,
								0xFF77DD11, 0xFF11BB44, 0xFF0F449F, 0xFF009070, 0xFF006010, 0xFF701010, 0xFF1C1010, 0xFF101C10,
								0xFF2C1010, 0xFF102C10, 0xFF10103C, 0xFF40201C, 0xFF019E30, 0xFF0514CE, 0xFFE86AA3, 0xFF0A4321,
								0xFF2C1010, 0xFF102C10, 0xFF10103C, 0xFF40201C, 0xFF019E30, 0xFF0514CE, 0xFFE86AA3, 0xFF0A4321};



int drawPlayerHitboxes(Camera inputCamera, World *gameWorld)
{
	if (gameWorld == NULL || gameWorld->Player == NULL || screenBuffer == NULL)
	{
		return MISSING_DATA;
	}

	if (RenderSettings.drawPlayer == 0)
	{
		return ACTION_DISABLED;
	}

	PlayerData *player = gameWorld->Player;

	renderHitbox(inputCamera, gameWorld, player->InteractBox);

	return LEMON_SUCCESS;
}


int WorldCameraControl(World *GameWorld, Camera *inputCamera)
{
	if (GameWorld == NULL || inputCamera == NULL || GameWorld->GameState == EMPTY_GAME)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}

	// Control latching/buffering
	switch(inputCamera->CameraMode)
	{
		case MENU_CAMERA:
		{
			if (inputCamera->CameraLatch == 0)
			{
				inputCamera->CameraXBuffer = inputCamera->CameraX;
				inputCamera->CameraYBuffer = inputCamera->CameraY;
				inputCamera->CameraX = -32000;
				inputCamera->CameraY = 0;
				inputCamera->CameraLatch = 1;
			}
		} break;

		default:
		{
			if (inputCamera->CameraLatch == 1)
			{
				inputCamera->CameraX = inputCamera->CameraXBuffer;
				inputCamera->CameraY = inputCamera->CameraYBuffer;
				inputCamera->CameraLatch = 0;
			}
		} break;
	}


	// Move camera
	switch(inputCamera->CameraMode)
	{
		case FOLLOW_PLAYER:
		{
			if (GameWorld->Player == NULL || GameWorld->Player->PlayerBox == NULL)
			{
				return MISSING_DATA;
			}

			int xDifference = (int)GameWorld->Player->PlayerBox->xPos - inputCamera->CameraX;

			inputCamera->CameraX += (int)(xDifference / 5) + 1;

			if (abs(xDifference) < 1)
			{
				inputCamera->CameraX = (int)GameWorld->Player->PlayerBox->xPos;
			}
			

			int yOffset = (int)GameWorld->Player->PlayerBox->yPos - inputCamera->CameraY;

			if (yOffset >= (screenHeight * 0.1))
			{
				int difference = yOffset - (screenHeight * 0.1);

				inputCamera->CameraY += (int)(difference / 5) + 1;

				if (abs(difference) < 5)
				{
					inputCamera->CameraY = (int)GameWorld->Player->PlayerBox->yPos - (screenHeight * 0.1);
				}
			}

			if (yOffset < (screenHeight * -0.2))
			{
				int difference = yOffset + (screenHeight * 0.2);

				inputCamera->CameraY += (int)(difference / 5) + 1;

				if (abs(difference) < 5)
				{
					inputCamera->CameraY = (int)GameWorld->Player->PlayerBox->yPos + (screenHeight * 0.2);
				}
			}

			restrictCameraToBounds(inputCamera);
		} break;


		case FREE_ROAM_RESTRICTED:
		{
			restrictCameraToBounds(inputCamera);
		} break;


		default:
		break;
	}


	return LEMON_SUCCESS;
}

int restrictCameraToBounds(Camera *inputCamera)
{
	if (inputCamera == NULL)
	{
		return MISSING_DATA;
	}

	if (inputCamera->maxCameraX < inputCamera->minCameraX + screenWidth)
	{
		inputCamera->maxCameraX = inputCamera->minCameraX + screenWidth;
	}

	if (inputCamera->maxCameraY < inputCamera->minCameraY + screenHeight)
	{
		inputCamera->maxCameraY = inputCamera->minCameraY + screenHeight;
	}

	if (inputCamera->minCameraX < 0)
	{
		inputCamera->minCameraX = 0;
	}

	if (inputCamera->minCameraY < 0)
	{
		inputCamera->minCameraY = 0;
	}

	inputCamera->CameraX = clamp(inputCamera->CameraX, inputCamera->minCameraX, inputCamera->maxCameraX - screenWidth);
	inputCamera->CameraY = clamp(inputCamera->CameraY, inputCamera->minCameraY, inputCamera->maxCameraY - screenWidth);

	return LEMON_SUCCESS;
}


int drawObjects(Camera inputCamera, World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL || screenBuffer == NULL)
	{
		return MISSING_DATA;
	}

	if (RenderSettings.drawSprites == 0)
	{
		return ACTION_DISABLED;
	}

	Object *currentObject;

	for (Layer drawLayer = BACKGROUND; drawLayer < UNDEFINED_LAYER; drawLayer++)
	{
		currentObject = GameWorld->ObjectList->firstObject;

		while(currentObject != NULL)
		{
			// Drawing routine - max objects on screen is defined as Macro
			if (drawLayer == currentObject->layer)
			{
				renderObjectSprite(inputCamera, GameWorld, currentObject);
			}
		
			currentObject = currentObject->nextObject;
		}
	}

	
	return LEMON_SUCCESS;
}


int drawHitboxes(Camera inputCamera, World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL || screenBuffer == NULL)
	{
		return MISSING_DATA;
	}

	if (RenderSettings.drawHitboxes == 0)
	{
		return ACTION_DISABLED;
	}


	Object *currentObject = GameWorld->ObjectList->firstObject;

	while(currentObject != NULL)
	{
		if (currentObject->layer == HUD)
		{
			currentObject->ObjectBox->xPos += GameWorld->MainCamera.CameraX;
			currentObject->ObjectBox->yPos += GameWorld->MainCamera.CameraY;
		
			renderHitbox(inputCamera, GameWorld, currentObject->ObjectBox);

			currentObject->ObjectBox->xPos -= GameWorld->MainCamera.CameraX;
			currentObject->ObjectBox->yPos -= GameWorld->MainCamera.CameraY;
		}
		else
		{
			renderHitbox(inputCamera, GameWorld, currentObject->ObjectBox);
		}
		
		currentObject = currentObject->nextObject;
	}

	drawPlayerHitboxes(GameWorld->MainCamera, GameWorld);


	return LEMON_SUCCESS;
}


int renderHitbox(Camera inputCamera, World *gameWorld, PhysicsRect *inputBox)
{
	if (inputBox == NULL)
	{
		return MISSING_DATA;
	}

	int tile = tileMap[inputBox->solid % 32];
	int thickness = DebugSettings.HitboxOutlineThickness;

	if (inputBox->xSize < 1 || inputBox->ySize < 1 || thickness < 1)
	{
		return EXECUTION_UNNECESSARY;
	}

	int xOffset = (int)inputBox->xPos - inputCamera.CameraX + (screenWidth >> 1);
	int yOffset = (int)inputBox->yPos - inputCamera.CameraY + (screenHeight >> 1);
	int xOffset2 = xOffset + inputBox->xSize;
	int yOffset2 = yOffset + inputBox->ySize;

	if (xOffset >= screenWidth || xOffset2 < 0 || yOffset2 < 0 || yOffset >= screenHeight)
	{
		return EXECUTION_UNNECESSARY;
	}

	// Hitbox
	switch (inputBox->solid)
	{
		case FLAT_SLOPE:
		{
			// clamp Offsets
			double ySize = inputBox->ySize;
			double xSize = inputBox->xSize;

			int xDraw = clamp(xOffset, 0, screenWidth - 1);
			int yDraw = clamp(yOffset, 0, screenHeight - 1);
			int xDraw2 = clamp(xOffset2, 0, screenWidth - 1);
			int yDraw2 = clamp(yOffset2, 0, screenHeight - 1);

			double slope = ((double)ySize/(double)xSize);
			int angleLineWidth = thickness * ((double)xSize/(double)ySize);


			if (inputBox->xFlip == 1)
			{
				for (int i = yDraw; i < yDraw2; i++)
				{
					int row = (i * screenWidth);

					int slopeEdge;
					if (inputBox->yFlip == 1)
					{
						slopeEdge = xOffset + ((i - yOffset) / slope);
					}
					else
					{
						slopeEdge = xOffset + ( (ySize - (i - yOffset)) / slope);
					}

					int skipValue = xOffset2 - thickness;
					int Diff = xOffset2 - xOffset;

					for (int k = clamp(slopeEdge, 0, xDraw2); k < xDraw2; k++)
					{
						if (skipValue > slopeEdge + thickness && k - angleLineWidth >= slopeEdge && k < skipValue && (i - thickness >= yOffset && i + thickness < yOffset2) )
						{
							k = clamp(skipValue, xDraw, xDraw2);
						}

						screenBuffer[row + k] = tile;
					}
				}

			}
			else
			{
				for (int i = yDraw; i < yDraw2; i++)
				{
					int row = (i * screenWidth);

					int slopeEdge;
					if (inputBox->yFlip == 1)
					{
						slopeEdge = xOffset + ( (ySize - (i - yOffset)) / slope);
					}
					else
					{
						slopeEdge = xOffset + ((i - yOffset) / slope);
					}

					int skipValue = slopeEdge - angleLineWidth;

					for (int k = xDraw; k < xDraw2 && k < slopeEdge; k++)
					{
						if (skipValue > xOffset + thickness && k - thickness >= xOffset && k < skipValue && (i - thickness >= yOffset && i + thickness < yOffset2) )
						{
							k = clamp(skipValue, xDraw, xDraw2);
						}

						screenBuffer[row + k] = tile;
					}
					
				}
			}

			
		} break;


		default:
		{

			// clamp Offsets
			int xDraw = clamp(xOffset, 0, screenWidth - 1);
			int yDraw = clamp(yOffset, 0, screenHeight - 1);
			int xDraw2 = clamp(xOffset2, 0, screenWidth - 1);
			int yDraw2 = clamp(yOffset2, 0, screenHeight - 1);

			for (int i = yDraw; i < yDraw2; i++)
			{
				int row = (i * screenWidth);

				for (int k = xDraw; k < xDraw2 && k < xDraw2; k++)
				{
					if (k + thickness < xOffset2 && k - thickness >= xOffset && (i - thickness >= yOffset && i + thickness < yOffset2) )
					{
						k = clamp(xOffset2 - thickness, xDraw, xDraw2);
					}

					screenBuffer[row + k] = tile;
				}
			}
		} break;
	}
	

	return LEMON_SUCCESS;
}



int renderObjectSprite(Camera inputCamera, World *gameWorld, Object *currentObject)
{
	// WARNING! The render pipeline is really messy and not easy to read: All variations of rendering has its own copy of the
	// main render loop, even extremely similar ones. This was done to squeeze out performance (at the expense of a bit of memory)
	// As such, readability was sacrificed somewhat. Continue at your own risk!

	if (currentObject == NULL ||  currentObject->ObjectDisplay == NULL || currentObject->ObjectDisplay->spriteBuffer == NULL)
	{
		return MISSING_DATA;
	}

	if (currentObject->ObjectDisplay->RenderModeOverride == DO_NOT_RENDER)
	{
		return ACTION_DISABLED;
	}

	switch(currentObject->ObjectID)
	{
		case UI_TEXT:
		case UI_ELEMENT:
			if (RenderSettings.drawUI == 0 || gameWorld->drawnHudElements >= RenderSettings.maxUIElements)
			{
				return ACTION_DISABLED;
			}
			break;


		case PARTICLE:
			if (RenderSettings.drawParticles == 0 || gameWorld->drawnParticles >= RenderSettings.maxParticles)
			{
				return ACTION_DISABLED;
			}
			break;


		case PLAYER_OBJECT:
			if (RenderSettings.drawPlayer == 0 || gameWorld->drawnObjects >= RenderSettings.maxObjects)
			{
				return ACTION_DISABLED;
			}
			break;


		default:
			if (RenderSettings.drawObjects == 0 || gameWorld->drawnObjects >= RenderSettings.maxObjects)
			{
				return ACTION_DISABLED;
			}
			break;
	}


	// Prepare data pointers
	DisplayData *inputData = currentObject->ObjectDisplay;
	PhysicsRect *inputBox = currentObject->ObjectBox;
	Sprite *spritePtr = inputData->spriteBuffer;
	Sprite RotateBuffer;


	// Decide how to render
	RenderMode inputRenderMode = inputData->RenderModeOverride;

	if (inputRenderMode == DEFAULT_TO_SPRITE)
	{
		inputRenderMode = spritePtr->RenderMode;
	}

	if (inputData->transparencyEffect < 1.0)	// Hijack render mode if transparency effect is being applied so that faster render function can be used when no transparency is applied
	{
		switch (inputRenderMode)
		{
			case SINGLE:
			inputRenderMode = SINGLE_FULL_ALPHA;
			break;

			case TILE:
			inputRenderMode = TILE_FULL_ALPHA;
			break;

			case SCALE:
			inputRenderMode = SCALE_FULL_ALPHA;
			break;

			default:
			break;
		}
	}


	// Locate object on screen
	int realXOffset = inputBox->xPos + (screenWidth >> 1) + inputData->spriteXOffset;
	int	realYOffset = inputBox->yPos + (screenHeight >> 1) + inputData->spriteYOffset;
	double renderDirection = inputData->direction;

	if (currentObject->layer != HUD)
	{
		realXOffset -= inputCamera.CameraX;
		realYOffset -= inputCamera.CameraY;
	}

	if (inputData->frameBuffer != NULL && inputData->currentAnimation > 0)
	{
		realXOffset += inputData->frameBuffer->SpriteXOffset;
		realYOffset += inputData->frameBuffer->SpriteYOffset;
		renderDirection += inputData->frameBuffer->rotation;
	}

	int xOffset2 = realXOffset + inputBox->xSize;
	int yOffset2 = realYOffset + inputBox->ySize;

	if (inputRenderMode == TILE || inputRenderMode == TILE_FAST || inputRenderMode == TILE_FULL_ALPHA)
	{
		goto Skip_Render_Effects;
	}


	int centerX = (xOffset2 + realXOffset) >> 1;
	int centerY = (yOffset2 + realYOffset) >> 1;

	int broadSize = ((spritePtr->width > inputBox->xSize ? spritePtr->width : inputBox->xSize) + (spritePtr->height > inputBox->ySize ? spritePtr->height : inputBox->ySize)) >> 1;

	if (centerX - broadSize >= screenWidth || centerX + broadSize < 0 || centerY - broadSize >= screenHeight || centerY + broadSize < 0)
	{
		return INVALID_DATA;
	}


	// Render effects
	if (renderDirection != RADIAN_90)
	{
		double sinVal = fabs(sin(renderDirection));
		double cosVal = fabs(cos(renderDirection));

		RotateBuffer = *(spritePtr);
		RotateBuffer.spriteData = RotateRenderBuffer;


		if (inputRenderMode == SCALE || inputRenderMode == SCALE_FULL_ALPHA)
		{
			inputRenderMode -= 6;
	
			RotateBuffer.width = (inputBox->ySize * cosVal) + (inputBox->xSize * sinVal) + 1;
			RotateBuffer.height = (inputBox->ySize * sinVal) + (inputBox->xSize * cosVal) + 1;

			if (RotateBuffer.width & 1 == 1)
			{
				RotateBuffer.width++;
			}

			if (RotateBuffer.height & 1 == 1)
			{
				RotateBuffer.height++;
			}

			if (RotateBuffer.width > MAX_SPRITE_SIZE)
			{
				RotateBuffer.width = MAX_SPRITE_SIZE;
			}

			if (RotateBuffer.height > MAX_SPRITE_SIZE)
			{
				RotateBuffer.height = MAX_SPRITE_SIZE;
			}

			memset(RotateRenderBuffer, 0, RotateBuffer.width * RotateBuffer.height << 2);

			ScaleRotateSpriteInBuffer(spritePtr, renderDirection, RotateBuffer.width, RotateBuffer.height, RotateRenderBuffer, inputBox->xSize, inputBox->ySize);
		}
		else
		{
			RotateBuffer.width = (spritePtr->width * sinVal) + (spritePtr->height * cosVal) + 1;
			RotateBuffer.height = (spritePtr->width * cosVal) + (spritePtr->height * sinVal) + 1;

			if (RotateBuffer.width & 1 == 1)
			{
				RotateBuffer.width++;
			}

			if (RotateBuffer.height & 1 == 1)
			{
				RotateBuffer.height++;
			}

			if (RotateBuffer.width > MAX_SPRITE_SIZE)
			{
				RotateBuffer.width = MAX_SPRITE_SIZE;
			}

			if (RotateBuffer.height > MAX_SPRITE_SIZE)
			{
				RotateBuffer.height = MAX_SPRITE_SIZE;
			}

			memset(RotateBuffer.spriteData, 0, RotateBuffer.width * RotateBuffer.height * 4);

			rotateSprite(spritePtr, renderDirection, RotateBuffer.width, RotateBuffer.height, RotateBuffer.spriteData);
		}

		spritePtr = &RotateBuffer;
	}
		

	if (inputRenderMode == SINGLE || inputRenderMode == SINGLE_FAST || inputRenderMode == SINGLE_FULL_ALPHA)
	{
		// Find center of object and then center sprite on that point
		realXOffset = centerX - (spritePtr->width >> 1);
		realYOffset = centerY - (spritePtr->height >> 1);
		xOffset2 = centerX + (spritePtr->width >> 1);
		yOffset2 = centerY + (spritePtr->height >> 1);
	}
	else
	{
		realXOffset = centerX - (inputBox->xSize >> 1);
		realYOffset = centerY - (inputBox->ySize >> 1);
		xOffset2 = realXOffset + inputBox->xSize;
		yOffset2 = realYOffset + inputBox->ySize;
	}


	Skip_Render_Effects:

	if (realXOffset >= screenWidth || xOffset2 < 0 || realYOffset >= screenHeight || yOffset2 < 0 || realXOffset >= xOffset2 || realYOffset >= yOffset2)
	{
		return INVALID_DATA;
	}

	int xDraw = clamp(realXOffset, 0, screenWidth - 1);
	int yDraw = clamp(realYOffset, 0, screenHeight - 1);
	int xDraw2 = clamp(xOffset2, 0, screenWidth - 1);
	int yDraw2 = clamp(yOffset2, 0, screenHeight - 1);
	

	realXOffset -= inputData->pixelXOffset;
	realYOffset -= inputData->pixelYOffset;


	// Render sprite to screen
	switch(inputRenderMode)
	{
		case SINGLE:
		case TILE:
		{
			if (inputBox->xFlip == -1 && inputBox->yFlip == -1)
			{
				renderSprite_RLDU_Tile(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			} 
			else if (inputBox->xFlip == -1 && inputBox->yFlip == 1)
			{
				renderSprite_RLUD_Tile(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else if (inputBox->xFlip == 1 && inputBox->yFlip == -1)
			{
				renderSprite_LRDU_Tile(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else
			{
				renderSprite_LRUD_Tile(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
		} break;


		case TILE_FULL_ALPHA:
		case SINGLE_FULL_ALPHA:
		{
			if (inputBox->xFlip == -1 && inputBox->yFlip == -1)
			{
				renderSprite_RLDU_FullAlpha(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputData->transparencyEffect);
			}
			else if (inputBox->xFlip == -1 && inputBox->yFlip == 1)
			{
				renderSprite_RLUD_FullAlpha(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputData->transparencyEffect);
			}
			else if (inputBox->xFlip == 1 && inputBox->yFlip == -1)
			{
				renderSprite_LRDU_FullAlpha(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputData->transparencyEffect);
			}
			else
			{
				renderSprite_LRUD_FullAlpha(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputData->transparencyEffect);
			}
		} break;


		case SINGLE_FAST:
		case TILE_FAST:
		{
			if (inputBox->xFlip == -1 && inputBox->yFlip == -1)
			{
				renderSprite_RLDU_Tile(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else if (inputBox->xFlip == -1 && inputBox->yFlip == 1)
			{
				renderSprite_RLUD_Tile(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else if (inputBox->xFlip == 1 && inputBox->yFlip == -1)
			{
				renderSprite_LRDU_TileFast(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
			else
			{
				renderSprite_LRUD_TileFast(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset);
			}
		} break;


		case SCALE:
		{
			if (inputBox->xFlip == -1 && inputBox->yFlip == -1)
			{
				renderSprite_RLDU_Scale(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputBox);
			}
			else if (inputBox->xFlip == -1 && inputBox->yFlip == 1)
			{
				renderSprite_RLUD_Scale(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputBox);
			}
			else if (inputBox->xFlip == 1 && inputBox->yFlip == -1)
			{
				renderSprite_LRDU_Scale(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputBox);
			}
			else
			{
				renderSprite_LRUD_Scale(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputBox);
			}
		} break;


		case SCALE_FULL_ALPHA:
		{
			if (inputBox->xFlip == -1 && inputBox->yFlip == -1)
			{
				renderSprite_RLDU_Scale_Full_Alpha(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputData->transparencyEffect, inputBox);
			}
			else if (inputBox->xFlip == -1 && inputBox->yFlip == 1)
			{
				renderSprite_RLUD_Scale_Full_Alpha(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputData->transparencyEffect, inputBox);
			}
			else if (inputBox->xFlip == 1 && inputBox->yFlip == -1)
			{
				renderSprite_LRDU_Scale_Full_Alpha(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputData->transparencyEffect, inputBox);
			}
			else
			{
				renderSprite_LRUD_Scale_Full_Alpha(spritePtr, xDraw, xDraw2, yDraw, yDraw2, realXOffset, realYOffset, inputData->transparencyEffect, inputBox);
			}
		} break;


		default:
			return INVALID_DATA;
		break;

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

	return LEMON_SUCCESS;
}

// UNUSED -> scales sprite only, not used because its faster to combine the scale and rotation code into one function
int ScaleSpriteInBuffer(Sprite *inputData, int width, int height, unsigned char destData[])
{
	if (width == 0 || height == 0)
	{
		return INVALID_DATA;
	}

	int sizeOfPixel = sizeof(char) << 2;
	double xScale = (double)inputData->width / (double)width;
	double yScale = (double)inputData->height / (double)height;

	double spriteX = 0.0;
	double spriteY = 0.0;

	int spriteIndex = 0;
	int spriteYIndex = 0;
	int destIndex = 0;
	int destYIndex = 0;

	for (int y = 0; y < height; ++y)
	{
		spriteX = 0.0;

		destYIndex = y * width;
		spriteYIndex = (int)spriteY * inputData->width;

		for (int x = 0; x < width; ++x)
		{
			spriteIndex = (spriteYIndex + (int)spriteX) << 2;
			destIndex = (destYIndex + x) << 2;

			memcpy(destData + destIndex, inputData->spriteData + spriteIndex, sizeOfPixel);

			spriteX += xScale;
		}

		spriteY += yScale;
	}

	return LEMON_SUCCESS;
}

int ScaleRotateSpriteInBuffer(Sprite *inputData, double direction, int width, int height, unsigned char destData[], int scaleWidth, int scaleHeight)
{
	if (scaleWidth == 0 || scaleHeight == 0)
	{
		return INVALID_DATA;
	}

	int sizeOfPixel = sizeof(char) << 2;
	int sizeValue = inputData->height * inputData->width << 2;
	int pitchValue = inputData->width << 2;
	double xScale = (double)inputData->width / (double)scaleWidth;
	double yScale = (double)inputData->height / (double)scaleHeight;


	double sinVal = sin(-direction + RADIAN_90);
	double cosVal = cos(-direction + RADIAN_90);

	double nx_x = rotateX(sinVal, cosVal, 1.0, 0.0) * xScale;
	double nx_y = rotateY(sinVal, cosVal, 1.0, 0.0) * yScale;
	double ny_x = rotateX(sinVal, cosVal, 0.0, 1.0) * xScale;
	double ny_y = rotateY(sinVal, cosVal, 0.0, 1.0) * yScale;

	double x0 = (rotateX(sinVal, cosVal, -(width >> 1), -(height >> 1)) + (scaleWidth >> 1)) * xScale;
	double y0 = (rotateY(sinVal, cosVal, -(width >> 1), -(height >> 1)) + (scaleHeight >> 1)) * yScale; 
	
	double x1 = 0.0;
	double y1 = 0.0;

	int yOffset = 0;
	int xInt = 0;
	int yInt = 0;

	for (int y = 0; y < height; ++y)
	{
		yOffset = (y << 2) * width;
		x1 = x0;
		y1 = y0;		

		for (int x = 0; x < width; ++x)
		{
			xInt = ((int)x1) << 2;
			yInt = ((int)y1) * pitchValue;

			if (xInt < pitchValue && yInt < sizeValue && xInt > -1 && yInt > -1)
			{
				memcpy(destData + yOffset + (x << 2), inputData->spriteData + (yInt + xInt), sizeOfPixel);
			}

			x1 += nx_x;
			y1 += nx_y;
		}

		x0 += ny_x;
		y0 += ny_y;
	}
	

	return LEMON_SUCCESS;
}


int rotateSprite(Sprite *inputData, double direction, int width, int height, unsigned char destData[])
{
	int pitchValue = inputData->width << 2;
	double sinVal = sin(-direction + RADIAN_90);
	double cosVal = cos(-direction + RADIAN_90);

	double nx_x = rotateX(sinVal, cosVal, 1.0, 0.0);
	double nx_y = rotateY(sinVal, cosVal, 1.0, 0.0);
	double ny_x = rotateX(sinVal, cosVal, 0.0, 1.0);
	double ny_y = rotateY(sinVal, cosVal, 0.0, 1.0);

	double x0 = rotateX(sinVal, cosVal, -(width >> 1), -(height >> 1)) + (inputData->width >> 1);
	double y0 = rotateY(sinVal, cosVal, -(width >> 1), -(height >> 1)) + (inputData->height >> 1); 
	
	double x1 = 0.0;
	double y1 = 0.0;

	int yOffset = 0;
	for (int y = 0; y < height; ++y)
	{
		yOffset = (y << 2) * width;
		x1 = x0;
		y1 = y0;

		for (int x = 0; x < width; ++x)
		{
			int xInt = (int)x1;
			int yInt = (int)y1;

			if (xInt < inputData->width && yInt < inputData->height && xInt > -1 && yInt > -1)
			{
				yInt *= pitchValue;
				xInt = xInt << 2;

				destData[yOffset + (x << 2)] = inputData->spriteData[yInt + xInt];
				destData[yOffset + (x << 2) + 1] = inputData->spriteData[yInt + xInt + 1];
				destData[yOffset + (x << 2) + 2] = inputData->spriteData[yInt + xInt + 2];
				destData[yOffset + (x << 2) + 3] = inputData->spriteData[yInt + xInt + 3];
			}

			x1 += nx_x;
			y1 += nx_y;
		}

		x0 += ny_x;
		y0 += ny_y;
	}
	

	return LEMON_SUCCESS;
}


double rotateX(double sinVal, double cosVal, double x, double y)
{
	return (x * cosVal) + (y * -sinVal);
}


double rotateY(double sinVal, double cosVal, double x, double y)
{
	return (x * sinVal) + (y * cosVal);
}


int renderSprite_LRUD_FullAlpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect)
{
	size_t sizeOfPixel = sizeof(uint32_t);
	uint32_t hexValue;

	// Start render loop
	int pixely = inputSprite->height - 1 - ((yDraw - yOffset) % inputSprite->height);

	int startPixelx = ((xDraw - xOffset) % inputSprite->width);

	
	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * inputSprite->width);
		int screenYOffset = j * screenWidth;

		for (int i = xDraw; i < xDraw2; i++)
		{
			hexValue = inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 3] << 24 | 
						inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 2] << 16 | 
						inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 1] << 8 | 
						inputSprite->spriteData[pixelYOffset + (pixelx << 2) ];

			if (hexValue >> 24 == 0xFF && transparencyEffect > 0.99)
			{
				screenBuffer[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screenBuffer[screenYOffset + i] = blendPixel(screenBuffer[screenYOffset + i], hexValue, transparencyEffect);
			}
			

			pixelx++;

			if (pixelx >= inputSprite->width)
			{
				pixelx = 0;
			}
		}

		pixely--;

		if (pixely < 0)
		{
			pixely = inputSprite->height - 1;
		}
	}


	return LEMON_SUCCESS;
}


int renderSprite_RLUD_FullAlpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect)
{
	size_t sizeOfPixel = sizeof(uint32_t);
	uint32_t hexValue;

	// Start render loop
	int pixely = inputSprite->height - 1 - ((yDraw - yOffset) % inputSprite->height);

	int startPixelx = inputSprite->width - 1 - ((xDraw - xOffset) % inputSprite->width);

	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * inputSprite->width);
		int screenYOffset = j * screenWidth;

		for (int i = xDraw; i < xDraw2; i++)
		{
			hexValue = inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 3] << 24 | 
						inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 2] << 16 | 
						inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 1] << 8 | 
						inputSprite->spriteData[pixelYOffset + (pixelx << 2) ];

			if (hexValue >> 24 == 0xFF && transparencyEffect > 0.99)
			{
				screenBuffer[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screenBuffer[screenYOffset + i] = blendPixel(screenBuffer[screenYOffset + i], hexValue, transparencyEffect);
			}

			pixelx--;

			if (pixelx < 0)
			{
				pixelx = inputSprite->width - 1;
			}
		}

		pixely--;

		if (pixely < 0)
		{
			pixely = inputSprite->width - 1;
		}

	}


	return LEMON_SUCCESS;
}


int renderSprite_LRDU_FullAlpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect)
{
	size_t sizeOfPixel = sizeof(uint32_t);
	uint32_t hexValue;

	// Start render loop
	int pixely = ((yDraw - yOffset) % inputSprite->height);

	int startPixelx = ((xDraw - xOffset) % inputSprite->width);

	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * inputSprite->width);
		int screenYOffset = j * screenWidth;

		for (int i = xDraw; i < xDraw2; i++)
		{
			hexValue = inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 3] << 24 | 
						inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 2] << 16 | 
						inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 1] << 8 | 
						inputSprite->spriteData[pixelYOffset + (pixelx << 2) ];

			if (hexValue >> 24 == 0xFF && transparencyEffect > 0.99)
			{
				screenBuffer[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screenBuffer[screenYOffset + i] = blendPixel(screenBuffer[screenYOffset + i], hexValue, transparencyEffect);
			}

			pixelx++;

			if (pixelx >= inputSprite->width)
			{
				pixelx = 0;
			}
		}

		pixely++;

		if (pixely >= inputSprite->width)
		{
			pixely = 0;
		}

	}


	return LEMON_SUCCESS;
}


int renderSprite_RLDU_FullAlpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect)
{
	size_t sizeOfPixel = sizeof(uint32_t);
	uint32_t hexValue;

	// Start render loop
	int pixely = ((yDraw - yOffset) % inputSprite->height);

	int startPixelx = inputSprite->width - 1 - ((xDraw - xOffset) % inputSprite->width);

	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * inputSprite->width);
		int screenYOffset = j * screenWidth;

		for (int i = xDraw; i < xDraw2; i++)
		{
			hexValue = inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 3] << 24 | 
						inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 2] << 16 | 
						inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 1] << 8 | 
						inputSprite->spriteData[pixelYOffset + (pixelx << 2) ];

			if (hexValue >> 24 == 0xFF && transparencyEffect > 0.99)
			{
				screenBuffer[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screenBuffer[screenYOffset + i] = blendPixel(screenBuffer[screenYOffset + i], hexValue, transparencyEffect);
			}

			pixelx--;

			if (pixelx < 0)
			{
				pixelx = inputSprite->width - 1;
			}
		}

		pixely++;

		if (pixely >= inputSprite->width)
		{
			pixely = 0;
		}

	}


	return LEMON_SUCCESS;
}


uint32_t blendPixel(uint32_t screenPixel, uint32_t inputPixel, float transparency)
{
	float alpha = ((inputPixel >> 24 ) * transparency) / 255.0;

	uint8_t rSrc = ((inputPixel & 0x00FF0000) >> 16);
	uint8_t rDest = ((screenPixel & 0x00FF0000) >> 16);

	uint8_t gSrc = ( (inputPixel & 0x0000FF00) >> 8);
	uint8_t gDest =  ((screenPixel & 0x0000FF00) >> 8);

	uint8_t bSrc = (inputPixel & 0x000000FF);
	uint8_t bDest = (screenPixel & 0x000000FF);

	uint8_t red = ( (rSrc - rDest) * alpha) + rDest; 

	uint8_t green = ( (gSrc - gDest) * alpha) + gDest;

	uint8_t blue = ( (bSrc - bDest) * alpha) + bDest;
	

/*

	int redSrc = ((inputPixel & 0x00FF0000) >> 16);
	int redDest = ((screenPixel & 0x00FF0000) >> 16);

	int greenSrc = ( (inputPixel & 0x0000FF00) >> 8);
	int greenDest =  ((screenPixel & 0x0000FF00) >> 8);

	int blueSrc = (inputPixel & 0x000000FF);
	int blueDest = (screenPixel & 0x000000FF);

	int alpha = ((inputPixel & 0xFF000000) >> 24 ) * transparency;

	int alphaMultiplication = (0x10000 * alpha) / 255;
	int remainder = 0x10000 - alphaMultiplication;

	int red = (redSrc * remainder + redDest * alphaMultiplication) >> 16;
	int green = (greenSrc * remainder + greenDest * alphaMultiplication) >> 16;
	int blue = (blueSrc * remainder + blueDest * alphaMultiplication) >> 16;
*/

	return (0xFF000000 | red << 16 | green << 8 | blue);
}


// renders sprite in tile mode (Render mode 0) with no reflections  
int renderSprite_LRUD_Tile(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	// Start render loop
	int pixely = inputSprite->height - 1 - ((yDraw - yOffset) % inputSprite->height);

	int startPixelx = ((xDraw - xOffset) % inputSprite->width);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		unsigned int pixelYOffset = (pixely << 2) * inputSprite->width;


		for (int i = xDraw; i < xDraw2; i++)
		{
			if ((uint32_t)inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 3] != 0x00)
			{
				memcpy(screenBuffer + (j * screenWidth) + i, inputSprite->spriteData + pixelYOffset + (pixelx << 2), sizeOfPixel);
			}

			pixelx++;

			if (pixelx > inputSprite->width - 1)
			{
				pixelx = 0;
			}
		}


		pixely--;

		if (pixely < 0)
		{
			pixely = inputSprite->height - 1;
		}

	}


	return LEMON_SUCCESS;
}


// renders sprite in tile mode (Render mode 0) with Left/Right reflections
int renderSprite_RLUD_Tile(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	int sizeOfPixel = sizeof(uint32_t);

	int pixely =  inputSprite->height - 1 - ((yDraw - yOffset) %  inputSprite->height);

	int startPixelx = inputSprite->width - 1 - ((xDraw - xOffset) % inputSprite->width);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		unsigned int pixelYOffset = (pixely << 2) * inputSprite->width;


		for (int i = xDraw; i < xDraw2; i++)
		{
			if ((uint32_t)inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screenBuffer + (j * screenWidth) + i, inputSprite->spriteData + pixelYOffset + (pixelx << 2), sizeOfPixel);
			}

			pixelx--;

			if (pixelx < 0)
			{
				pixelx = inputSprite->width - 1;
			}
		}

		pixely--;

		if (pixely < 0)
		{
			pixely =  inputSprite->height - 1;
		}

	}

	return LEMON_SUCCESS;
}


// renders sprite in tile mode (Render mode 0) with Up/Down reflections
int renderSprite_LRDU_Tile(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	int sizeOfPixel = sizeof(uint32_t);

	int pixely = ((yDraw - yOffset) %  inputSprite->height);

	int startPixelx = ((xDraw - xOffset) % inputSprite->width);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * inputSprite->width);

		for (int i = xDraw; i + 1 < xDraw2; i++)
		{
			if ((uint32_t)inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screenBuffer + (j * screenWidth) + i, inputSprite->spriteData + pixelYOffset + (pixelx << 2), sizeOfPixel);
			}

			pixelx++;

			if (pixelx >= inputSprite->width)
			{
				pixelx = 0;
			}
		}

		pixely++;

		if (pixely >= inputSprite->height)
		{
			pixely = 0;
		}

	}

	return LEMON_SUCCESS;
}


// renders sprite in tile mode (Render mode 0) with Left/Right and Up/Down reflections
int renderSprite_RLDU_Tile(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	int sizeOfPixel = sizeof(uint32_t);

	int pixely = ((yDraw - yOffset) %  inputSprite->height);

	int startPixelx = inputSprite->width - 1 - ((xDraw - xOffset) % inputSprite->width);


	for (int j = yDraw; j < yDraw2; j++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * inputSprite->width);

		for (int i = xDraw; i + 1 < xDraw2; i++)
		{
			if ((uint32_t)inputSprite->spriteData[pixelYOffset + (pixelx << 2) + 3] > 0x00)
			{
				memcpy(screenBuffer + (j * screenWidth) + i, inputSprite->spriteData + pixelYOffset + (pixelx << 2), sizeOfPixel);
			}

			pixelx--;

			if (pixelx < 0)
			{
				pixelx = inputSprite->width - 1;
			}
		}

		pixely++;

		if (pixely >= inputSprite->height)
		{
			pixely = 0;
		}

	}

	return LEMON_SUCCESS;
}


int renderSprite_LRUD_TileFast(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);


	// Start render loop
	int pixely = inputSprite->height - 1 - (((yDraw - yOffset) % inputSprite->height));

	int startPixelx = ((xDraw - xOffset) % inputSprite->width);
	int screenYOffset = yDraw * screenWidth;


	// Render sprite to screen
	for (int i = yDraw; i < yDraw2; i++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * inputSprite->width);
		int k = xDraw;


		if (xDraw2 - xDraw >= inputSprite->width)
		{
			memcpy(screenBuffer + screenYOffset + k, inputSprite->spriteData + pixelYOffset + (pixelx << 2), sizeOfPixel * (inputSprite->width - pixelx));
			k += (inputSprite->width - pixelx);


			for (; k + inputSprite->width - 1 < xDraw2; k += inputSprite->width)
			{
				memcpy(screenBuffer + screenYOffset + k, inputSprite->spriteData + pixelYOffset, inputSprite->width * sizeOfPixel);
			}

			pixelx = 0;
		}

		memcpy(screenBuffer + screenYOffset + k, inputSprite->spriteData + pixelYOffset + (pixelx << 2), (xDraw2 - k) * sizeOfPixel);

		pixely--;
		screenYOffset += screenWidth;

		if (pixely < 0)
		{
			pixely = inputSprite->height - 1;
		}
	}


	return LEMON_SUCCESS;
}



int renderSprite_LRDU_TileFast(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset)
{
	size_t sizeOfPixel = sizeof(uint32_t);

	// Start render loop
	int pixely = ((yDraw - yOffset) % inputSprite->height);

	int startPixelx = ((xDraw - xOffset) % inputSprite->width);
	int screenYOffset = yDraw * screenWidth;

	for (int i = yDraw; i < yDraw2; i++)
	{
		int pixelx = startPixelx;
		int pixelYOffset = ((pixely << 2) * inputSprite->width);
		int k = xDraw;

		if (xDraw2 - xDraw >= inputSprite->width)
		{
			memcpy(screenBuffer + screenYOffset + k, inputSprite->spriteData + pixelYOffset + (pixelx << 2), sizeOfPixel * (inputSprite->width - pixelx));
			k += (inputSprite->width - pixelx);


			for (; k + inputSprite->width - 1 < xDraw2; k += inputSprite->width)
			{
				memcpy(screenBuffer + screenYOffset + k, inputSprite->spriteData + pixelYOffset, inputSprite->width * sizeOfPixel);
			}

			pixelx = 0;
		}

		memcpy(screenBuffer + screenYOffset + k, inputSprite->spriteData + pixelYOffset + (pixelx << 2), (xDraw2 - k) * sizeOfPixel);

		pixely++;
		screenYOffset += screenWidth;

		if (pixely > inputSprite->height - 1)
		{
			pixely = 0;
		}

	}


	return LEMON_SUCCESS;
}


int renderSprite_LRUD_Scale(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);

	double xScale = (double)inputSprite->width/(double)inputBox->xSize;
	double yScale = (double)inputSprite->height/(double)inputBox->ySize;
	double xScale2 = (double)inputBox->xSize / (double)inputSprite->width;

	double pixely = (inputBox->ySize - 1 - (yDraw - yOffset)) * yScale;


	for (int i = yDraw; i < yDraw2; ++i)
	{
		unsigned int pixelx = 0;

		int intPixelY = (((int)pixely << 2) * inputSprite->width);
		int screenYOffset = i * screenWidth;

		double k = xOffset;

		if (k < 0.0)
		{
			for (; k < 0.0; k += xScale2)
			{
				pixelx++;
			}

			pixelx--;

			if ((uint32_t)inputSprite->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = xDraw; j < k; ++j)
				{
					memcpy(screenBuffer + screenYOffset + j, inputSprite->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx++;
		}

		for (; (int)k < xDraw2 && pixelx < inputSprite->width; k += xScale2)
		{
			if ((uint32_t)inputSprite->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = 0; j < xScale2 && (int)k + j < xDraw2; ++j)
				{
					memcpy(screenBuffer + screenYOffset + (int)k + j, inputSprite->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx++;
		}

		pixely -= yScale;
	}

	return LEMON_SUCCESS;
}


int renderSprite_RLUD_Scale(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);

	double xScale = (double)inputSprite->width/(double)inputBox->xSize;
	double yScale = (double)inputSprite->height/(double)inputBox->ySize;
	double xScale2 = (double)inputBox->xSize / (double)inputSprite->width;

	double pixely = (inputBox->ySize - 1 - (yDraw - yOffset)) * yScale;

	int startPixelx = inputSprite->width - 1;
	int screenYOffset = yDraw * screenWidth;

	for (int i = yDraw; i < yDraw2; ++i)
	{
		unsigned int pixelx = startPixelx;

		int intPixelY = (((int)pixely << 2) * inputSprite->width);

		double k = xOffset;


		if (k < 0)
		{
			for (; k < 0; k += xScale2)
			{
				pixelx--;
			}

			pixelx++;

			if ((uint32_t)inputSprite->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = xDraw; j < k; ++j)
				{
					memcpy(screenBuffer + screenYOffset + j, inputSprite->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx--;
		}
		

		for (; (int)k < xDraw2 && pixelx < inputSprite->width; k += xScale2)
		{
			if ((uint32_t)inputSprite->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = 0; j < xScale2 && (int)k + j < xDraw2; ++j)
				{
					memcpy(screenBuffer + screenYOffset + (int)k + j, inputSprite->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx--;
		}

		screenYOffset += screenWidth;
		pixely -= yScale;
	}

	return LEMON_SUCCESS;
}


int renderSprite_LRDU_Scale(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);

	double xScale = (double)inputSprite->width/(double)inputBox->xSize;
	double yScale = (double)inputSprite->height/(double)inputBox->ySize;
	double xScale2 = (double)inputBox->xSize / (double)inputSprite->width;

	double pixely = (yDraw - yOffset) * yScale;


	for (int i = yDraw; i < yDraw2; i++)
	{
		int intPixelY = (((int)pixely << 2) * inputSprite->width);
		unsigned int pixelx = 0;

		double k = xOffset;


		if (k < 0)
		{
			for (; k < 0; k += xScale2)
			{
				pixelx++;
			}

			pixelx--;

			if ((uint32_t)inputSprite->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = xDraw; j < k; j++)
				{
					memcpy(screenBuffer + (i * screenWidth) + j, inputSprite->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx++;
		}
		

		for (; (int)k < xDraw2 && pixelx < inputSprite->width; k += xScale2)
		{
			if ((uint32_t)inputSprite->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = 0; j < xScale2 && (int)k + j < xDraw2; j++)
				{
					memcpy(screenBuffer + (i * screenWidth) + (int)k + j, inputSprite->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx++;
		}

		pixely += yScale;
	}

	return LEMON_SUCCESS;
}


int renderSprite_RLDU_Scale(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);

	double xScale = (double)inputSprite->width/(double)inputBox->xSize;
	double yScale = (double)inputSprite->height/(double)inputBox->ySize;
	double xScale2 = (double)inputBox->xSize / (double)inputSprite->width;

	double pixely = (yDraw - yOffset) * yScale;

	int startPixelx = inputSprite->width - 1;


	for (int i = yDraw; i < yDraw2; i++)
	{
		int intPixelY = (((int)pixely << 2) * inputSprite->width);
		unsigned int pixelx = startPixelx;

		double k = xOffset;


		if (k < 0)
		{
			for (; k < 0; k += xScale2)
			{
				pixelx--;
			}

			pixelx++;

			if ((uint32_t)inputSprite->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = xDraw; j < k; j++)
				{
					memcpy(screenBuffer + (i * screenWidth) + j, inputSprite->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx--;
		}
		

		for (; (int)k < xDraw2 && pixelx < inputSprite->width; k += xScale2)
		{
			if ((uint32_t)inputSprite->spriteData[intPixelY + (pixelx << 2) + 3] != 0x00)
			{
				for (int j = 0; j < xScale2 && (int)(j + k) < xDraw2; j++)
				{
					memcpy(screenBuffer + (i * screenWidth) + (int)k + j, inputSprite->spriteData + intPixelY + (pixelx << 2), sizeOfPixel);
				}
				
			}

			pixelx--;
		}

		pixely += yScale;
	}

	return LEMON_SUCCESS;
}


int renderSprite_LRUD_Scale_Full_Alpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);
	uint32_t hexValue;

	double xScale = (double)inputSprite->width/(double)inputBox->xSize;
	double yScale = (double)inputSprite->height/(double)inputBox->ySize;


	double pixely = (inputBox->ySize - 1 - (yDraw - yOffset)) * yScale;

	double startPixelx = xScale * (xDraw - xOffset);


	for (int k = yDraw; k < yDraw2; k++)
	{
		int pixelYOffset = (((int)pixely) << 2) * inputSprite->width;
		int screenYOffset = k * screenWidth;
		double pixelx = startPixelx;

		for (int i = xDraw; i < xDraw2; i++)
		{
			int intPixelx = ((int)pixelx) << 2;

			hexValue = inputSprite->spriteData[pixelYOffset + intPixelx + 3] << 24 | 
						inputSprite->spriteData[pixelYOffset + intPixelx + 2] << 16 | 
						inputSprite->spriteData[pixelYOffset + intPixelx + 1] << 8 | 
						inputSprite->spriteData[pixelYOffset + intPixelx];

			if (hexValue >> 24 == 0xFF && transparencyEffect > 0.99)
			{
				screenBuffer[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screenBuffer[screenYOffset + i] = blendPixel(screenBuffer[screenYOffset + i], hexValue, transparencyEffect);
			}
			

			pixelx += xScale;
		}

		pixely -= yScale;
	}


	return LEMON_SUCCESS;
}


int renderSprite_RLUD_Scale_Full_Alpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);
	uint32_t hexValue;

	double xScale = (double)inputSprite->width/(double)inputBox->xSize;
	double yScale = (double)inputSprite->height/(double)inputBox->ySize;


	double pixely = (inputBox->ySize - 1 - (yDraw - yOffset)) * yScale;

	double startPixelx = (inputBox->xSize - 1 - (xDraw - xOffset)) * xScale;


	for (int k = yDraw; k < yDraw2; k++)
	{
		int pixelYOffset = (((int)pixely) << 2) * inputSprite->width;
		int screenYOffset = k * screenWidth;
		double pixelx = startPixelx;

		for (int i = xDraw; i < xDraw2; i++)
		{
			int intPixelx = ((int)pixelx) << 2;

			hexValue = inputSprite->spriteData[pixelYOffset + intPixelx + 3] << 24 | 
						inputSprite->spriteData[pixelYOffset + intPixelx + 2] << 16 | 
						inputSprite->spriteData[pixelYOffset + intPixelx + 1] << 8 | 
						inputSprite->spriteData[pixelYOffset + intPixelx];

			if (hexValue >> 24 == 0xFF && transparencyEffect > 0.99)
			{
				screenBuffer[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screenBuffer[screenYOffset + i] = blendPixel(screenBuffer[screenYOffset + i], hexValue, transparencyEffect);
			}
			

			pixelx -= xScale;
		}

		pixely -= yScale;
	}


	return LEMON_SUCCESS;
}


int renderSprite_LRDU_Scale_Full_Alpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);
	uint32_t hexValue;

	double xScale = (double)inputSprite->width/(double)inputBox->xSize;
	double yScale = (double)inputSprite->height/(double)inputBox->ySize;


	double pixely = (inputBox->ySize - 1 - (yDraw - yOffset)) * yScale;

	double startPixelx = xScale * (xDraw - xOffset);


	for (int k = yDraw; k < yDraw2; k++)
	{
		int pixelYOffset = (((int)pixely) << 2) * inputSprite->width;
		int screenYOffset = k * screenWidth;
		double pixelx = startPixelx;

		for (int i = xDraw; i < xDraw2; i++)
		{
			int intPixelx = ((int)pixelx) << 2;

			hexValue = inputSprite->spriteData[pixelYOffset + intPixelx + 3] << 24 | 
						inputSprite->spriteData[pixelYOffset + intPixelx + 2] << 16 | 
						inputSprite->spriteData[pixelYOffset + intPixelx + 1] << 8 | 
						inputSprite->spriteData[pixelYOffset + intPixelx];

			if (hexValue >> 24 == 0xFF && transparencyEffect > 0.99)
			{
				screenBuffer[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screenBuffer[screenYOffset + i] = blendPixel(screenBuffer[screenYOffset + i], hexValue, transparencyEffect);
			}
			

			pixelx += xScale;
		}

		pixely -= yScale;
	}


	return LEMON_SUCCESS;
}


int renderSprite_RLDU_Scale_Full_Alpha(Sprite *inputSprite, int xDraw, int xDraw2, int yDraw, int yDraw2, int xOffset, int yOffset, float transparencyEffect, PhysicsRect *inputBox)
{
	int sizeOfPixel = sizeof(uint32_t);
	uint32_t hexValue;

	double xScale = (double)inputSprite->width/(double)inputBox->xSize;
	double yScale = (double)inputSprite->height/(double)inputBox->ySize;


	double pixely = (inputBox->ySize - 1 - (yDraw - yOffset)) * yScale;

	double startPixelx = (inputBox->xSize - 1 - (xDraw - xOffset)) * xScale;


	for (int k = yDraw; k < yDraw2; k++)
	{
		int pixelYOffset = (((int)pixely) << 2) * inputSprite->width;
		int screenYOffset = k * screenWidth;
		double pixelx = startPixelx;

		for (int i = xDraw; i < xDraw2; i++)
		{
			int intPixelx = ((int)pixelx) << 2;

			hexValue = inputSprite->spriteData[pixelYOffset + intPixelx + 3] << 24 | 
						inputSprite->spriteData[pixelYOffset + intPixelx + 2] << 16 | 
						inputSprite->spriteData[pixelYOffset + intPixelx + 1] << 8 | 
						inputSprite->spriteData[pixelYOffset + intPixelx];

			if (hexValue >> 24 == 0xFF && transparencyEffect > 0.99)
			{
				screenBuffer[screenYOffset + i] = hexValue;
			}
			else if (hexValue >> 24 != 0x00)
			{
				screenBuffer[screenYOffset + i] = blendPixel(screenBuffer[screenYOffset + i], hexValue, transparencyEffect);
			}
			

			pixelx -= xScale;
		}

		pixely -= yScale;
	}


	return LEMON_SUCCESS;
}


int renderBackGroundSprite(Camera inputCamera, BackgroundData WorldBackground)
{
	if (screenBuffer == NULL)
	{ 
		return MISSING_DATA;
	}

	if (RenderSettings.drawBackGround == 0)
	{
		memset(screenBuffer, 0, screenWidth * screenHeight * sizeof(uint32_t));
		return MISSING_DATA;
	}

	Sprite *spritePtr;
	spritePtr = WorldBackground.BackgroundSpriteBuffer;

	if (spritePtr == NULL || spritePtr->RenderMode == DO_NOT_RENDER)
	{
		memset(screenBuffer, 0, screenWidth * screenHeight * sizeof(uint32_t));
		return MISSING_DATA;
	}

	
	// Locate object on screen
	int xOffset = inputCamera.CameraX * WorldBackground.bgParallax;
	int yOffset = inputCamera.CameraY * WorldBackground.bgParallax;
	int xOffset2 = xOffset + spritePtr->width;
	int yOffset2 = yOffset + spritePtr->height;

	int xDraw2, xDraw, yDraw2, yDraw;

	size_t sizeOfPixel = sizeof(uint32_t);

	int pixely = spritePtr->height - 1 - (((yDraw + yOffset) % spritePtr->height));
	int pixelx = 0;


	//	RenderMode 0: Tile Mapping								(Sprite will tile across entire bounding box of object)
	//	RenderMode 1: Single sprite render 						(Renders single instance of sprite directly to bottom-left corner)
	//  RenderMode 2: Row Parallax

	if (spritePtr->RenderMode == SINGLE || spritePtr->RenderMode == SINGLE_FAST || spritePtr->RenderMode == SINGLE_FULL_ALPHA)
	{
		xDraw = clamp(0 - xOffset, 0, screenWidth - 1);
		yDraw = clamp(0 - yOffset, 0, screenHeight - 1);
		xDraw2 = clamp(spritePtr->width - xOffset, 0, screenWidth - 1);
		yDraw2 = clamp(spritePtr->height - yOffset, 0, screenHeight - 1);

		memset(screenBuffer, 0, screenWidth * yDraw * sizeof(uint32_t));

		for (int i = yDraw; i < yDraw2; i++)
		{
			pixelx = (((xDraw + xOffset) % spritePtr->width));

			memcpy(screenBuffer + (i * screenWidth) + xDraw, spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), ((xDraw2 - xDraw) % spritePtr->width) * sizeOfPixel);

			memset(screenBuffer + (i * screenWidth) + xDraw2, 0, (screenWidth - xDraw2) * sizeof(uint32_t));
			
			pixely--;

			if (pixely < 0)
			{
				pixely = spritePtr->height - 1;
			}
		}

		memset(screenBuffer + (yDraw2 * screenWidth), 0, (screenHeight - yDraw2) * screenWidth * sizeof(uint32_t));

		return LEMON_SUCCESS;
	}
	

	xDraw = 0;
	yDraw = 0;
	xDraw2 = screenWidth - 1;
	yDraw2 = screenHeight - 1;

	pixelx = ((xDraw + xOffset) % spritePtr->width);
	int screenx = 0;


	// Render sprite to screen
	for (int i = yDraw; i < yDraw2; i++)
	{
		if (spritePtr->RenderMode == BG_ROW_PARALLAX && WorldBackground.ParallaxRowCutOff < pixely && WorldBackground.ParallaxRowStart >= pixely)
		{
			xOffset = inputCamera.CameraX * (WorldBackground.bgParallax - (WorldBackground.bgRowParallax * (i + yOffset) ) );
			pixelx = ((xDraw + xOffset) % spritePtr->width);
		}

		screenx = clamp((spritePtr->width - pixelx), 0, screenWidth);

		memcpy(screenBuffer + (i * screenWidth), spritePtr->spriteData + ((pixely << 2) * spritePtr->width) + (pixelx << 2), sizeOfPixel * screenx);
		
		for (; (screenx + spritePtr->width) < xDraw2; screenx += spritePtr->width)
		{
			memcpy(screenBuffer + (i * screenWidth) + screenx, spritePtr->spriteData + ((pixely << 2) * spritePtr->width), spritePtr->width * sizeOfPixel);
		}

		int j = clamp((screenWidth - screenx), 0, screenWidth);

		memcpy(screenBuffer + (i * screenWidth) + screenx, spritePtr->spriteData + ((pixely << 2) * spritePtr->width), j * sizeOfPixel);

		pixely--;

		if (pixely < 0)
		{
			pixely = spritePtr->height - 1;
		}
	}


	return LEMON_SUCCESS;
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
//	return LEMON_SUCCESS;
//}
