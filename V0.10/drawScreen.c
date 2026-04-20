#include "LemonEngine.h"


// Controls what symbol is printed for each tile
const Uint32 colourMap[32] = {	0x00FF00FF, 0x00FF1010, 0x0000FF10, 0x00FFAA00, 0x00F8F800, 0x000010FF, 0x00AAFF10, 0x0000DDDD,
								0x00AAEE30, 0xFF11BB44, 0xFF0F449F, 0xFF009070, 0xFF006010, 0xFF701010, 0xFF1C1010, 0xFF101C10,
								0xFF2C1010, 0xFF102C10, 0xFF10103C, 0xFF40201C, 0xFF019E30, 0xFF0514CE, 0xFFE86AA3, 0xFF0A4321,
								0xFF2C1010, 0xFF102C10, 0xFF10103C, 0xFF40201C, 0xFF019E30, 0xFF0514CE, 0xFFE86AA3, 0xFF0A4321};

// global camera to be used in this file
Camera renderCamera;


int drawPlayerHitboxes(Camera inputCamera, World *gameWorld, SDL_Renderer *Screen)
{
	if (gameWorld == NULL || Screen == NULL)
	{
		return MISSING_DATA;
	}

	PlayerData *player = &gameWorld->Player;

	renderHitbox(inputCamera, &player->InteractBox, Screen);

	return LEMON_SUCCESS;
}


int CameraControl(World *GameWorld, Camera *inputCamera)
{
	if (GameWorld == NULL || inputCamera == NULL || GameWorld->GameState == EMPTY_GAME)
	{
		return MISSING_DATA;
	}

	if (GameWorld->GameState == LOADING)
	{
		return ACTION_DISABLED;
	}

	int camMode = inputCamera->CameraMode;


	// Control latching/buffering
	switch(camMode)
	{
		case MENU_CAMERA:
		{
			if (inputCamera->CameraLatch == false)
			{
				inputCamera->CameraXBuffer = inputCamera->CameraX;
				inputCamera->CameraYBuffer = inputCamera->CameraY;
				inputCamera->CameraX = -32000.0;
				inputCamera->CameraY = 0;
				inputCamera->CameraLatch = true;
			}
		} break;

		default:
		{
			if (inputCamera->CameraLatch == true)
			{
				inputCamera->CameraX = inputCamera->CameraXBuffer;
				inputCamera->CameraY = inputCamera->CameraYBuffer;
				inputCamera->CameraLatch = false;
			}
		} break;
	}

	if (GameWorld->GameState == CUTSCENE)
	{
		camMode = FREE_ROAM_RESTRICTED;
	}

	// Move camera
	switch(camMode)
	{
		case FOLLOW_PLAYER:
		{
			PhysicsBox *PlayerBox = GameWorld->Player.PlayerBox;

			if (PlayerBox == NULL)
			{
				inputCamera->CameraMode = FREE_ROAM_RESTRICTED;
				return MISSING_DATA;
			}

			float xDifference = PlayerBox->xPos - inputCamera->CameraX;

			inputCamera->CameraX += (xDifference / 5.0);

			if (fabs(xDifference) < 1)
			{
				inputCamera->CameraX = PlayerBox->xPos;
			}
			

			float yOffset = PlayerBox->yPos - inputCamera->CameraY;

			if (yOffset >= (inputCamera->height * 0.1))
			{
				float difference = yOffset - (inputCamera->height * 0.1);

				inputCamera->CameraY += (difference / 5.0);

				if (fabs(difference) < 5)
				{
					inputCamera->CameraY = PlayerBox->yPos - (inputCamera->height * 0.1);
				}
			}
			else if (yOffset < (inputCamera->height * -0.2))
			{
				float difference = yOffset + (inputCamera->height * 0.2);

				inputCamera->CameraY += (difference / 5.0) + 1;

				if (fabs(difference) < 5)
				{
					inputCamera->CameraY = PlayerBox->yPos + (inputCamera->height * 0.2);
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

	float halfWidth = (float)(inputCamera->width / 2);
	float halfHeight = (float)(inputCamera->height / 2);
	float minX = inputCamera->minCameraX + halfWidth;
	float maxX = inputCamera->maxCameraX - halfWidth;
	float minY = inputCamera->minCameraY + halfHeight;
	float maxY = inputCamera->maxCameraY - halfHeight;

	if (maxX < minX)
	{
		inputCamera->maxCameraX = inputCamera->minCameraX + inputCamera->width;
	}

	if (maxY < minY)
	{
		inputCamera->maxCameraY = inputCamera->minCameraY + inputCamera->height;
	}


	inputCamera->CameraX = fClamp(inputCamera->CameraX, minX, maxX);
	inputCamera->CameraY = fClamp(inputCamera->CameraY, minY, maxY);

	return LEMON_SUCCESS;
}


int drawObjects(Camera inputCamera, World *GameWorld, SDL_Renderer *Screen)
{
	if (GameWorld->ObjectList == NULL || Screen == NULL)
	{
		return MISSING_DATA;
	}

	if (RenderSettings.drawSprites == false)
	{
		return ACTION_DISABLED;
	}

	RenderSettings.drawnObjects = 0;

	inputCamera.CameraX += (float)((inputCamera.width - inputCamera.zoomedWidth) >> 1);
	inputCamera.CameraY += (float)((inputCamera.zoomedHeight - inputCamera.height) >> 1);
	Camera hudCam = {0};
	ResetCamera(&hudCam);

	Object *layerStarts[LAYER_COUNT] = {NULL};		// the first instances of each layer; helps to speed up rendering especially if no objects use a specific layer
	Layer objLayer = 0;
	Object *currentObject = GameWorld->ObjectList->firstObject;
	
	for (Layer drawLayer = BACKGROUND; drawLayer < LAYER_COUNT; drawLayer++)
	{	
		if (drawLayer != BACKGROUND)
		{
			if (layerStarts[drawLayer] != NULL)
			{
				currentObject = layerStarts[drawLayer];
			}
			else
			{
				goto No_Objects_To_Render;
			}
		}
		
		if (drawLayer == HUD)
		{
			if (RenderSettings.drawHUD == false)
			{
				continue;
			}

			SDL_SetRenderScale(Screen, 1.0, 1.0);
			SDL_SetRenderLogicalPresentation(Screen, hudCam.width, hudCam.height, SDL_LOGICAL_PRESENTATION_STRETCH);

			while(currentObject != NULL)
			{
				objLayer = getDisplayLayer(currentObject);

				if (objLayer >= 0 && layerStarts[objLayer] == NULL)
				{
					layerStarts[objLayer] = currentObject;
				}

				if (drawLayer == objLayer)
				{
					renderObject(hudCam, currentObject, Screen);
				}
				
				currentObject = currentObject->nextObject;
			}

			SDL_SetRenderScale(Screen, inputCamera.zoomX, inputCamera.zoomY);
			SDL_SetRenderLogicalPresentation(Screen, inputCamera.width, inputCamera.height, SDL_LOGICAL_PRESENTATION_STRETCH);
		}
		else
		{
			if (drawLayer == PARTICLES && RenderSettings.drawParticles == false)
			{
				continue;
			}

			while(currentObject != NULL)
			{
				objLayer = getDisplayLayer(currentObject);

				if (objLayer >= 0 && layerStarts[objLayer] == NULL)
				{
					layerStarts[objLayer] = currentObject;
				}

				if (drawLayer == objLayer)
				{
					renderObject(inputCamera, currentObject, Screen);
				}

				currentObject = currentObject->nextObject;
			}
		}

		No_Objects_To_Render:
		renderCameraViews(GameWorld->views, GameWorld, Screen, drawLayer);
	}

	return LEMON_SUCCESS;
}


void drawHitboxes(Camera inputCamera, World *GameWorld, SDL_Renderer *Screen)
{
	if (Screen == NULL || RenderSettings.drawHitboxes == false)
	{
		return;
	}


	Object *currentObject = GameWorld->ObjectList->firstObject;

	inputCamera.CameraX += (float)((inputCamera.width - inputCamera.zoomedWidth) >> 1);
	inputCamera.CameraY += (float)((inputCamera.zoomedHeight - inputCamera.height) >> 1);
	Camera hudCamera = {0};
	ResetCamera(&hudCamera);

	while(currentObject != NULL)
	{
		if (getDisplayLayer(currentObject) == HUD)
		{
			SDL_SetRenderScale(Screen, 1.0, 1.0);

			renderHitbox(hudCamera, currentObject->ObjectBox, Screen);

			SDL_SetRenderScale(Screen, inputCamera.zoomX, inputCamera.zoomY);
		}
		else
		{
			renderHitbox(inputCamera, currentObject->ObjectBox, Screen);
		}
		
		currentObject = currentObject->nextObject;
	}


	drawPlayerHitboxes(inputCamera, GameWorld, Screen);

	return;
}


int renderHitbox(Camera inputCamera, PhysicsBox *inputBox, SDL_Renderer *Screen)
{
	if (inputBox == NULL)
	{
		return MISSING_DATA;
	}

	uint32_t colour = colourMap[inputBox->solid % 32];

	SDL_SetRenderDrawColor(Screen, (colour & 0x00FF0000) >> 16, (colour & 0x0000FF00) >> 8, (colour & 0x000000FF), 0xFF);
	
	SDL_FRect Hitbox;

	float xCoord = (inputCamera.width >> 1) + inputBox->xPos - inputCamera.CameraX;
	float yCoord = inputCamera.CameraY + (inputCamera.height >> 1) - inputBox->yPos - inputBox->ySize;
	Hitbox.x = xCoord;
	Hitbox.y = yCoord;
	Hitbox.h = (float)inputBox->ySize;

	if (RenderSettings.HitboxOutlineThickness > 99)
	{
		Hitbox.w = (float)inputBox->xSize;
		SDL_RenderFillRect(Screen, &Hitbox);
		return EXECUTION_UNNECESSARY;
	}

	Hitbox.w = (float)clamp(RenderSettings.HitboxOutlineThickness, 0, inputBox->xSize);

	SDL_RenderFillRect(Screen, &Hitbox);

	Hitbox.x += (float)(inputBox->xSize - clamp(RenderSettings.HitboxOutlineThickness, 0, inputBox->xSize));
	SDL_RenderFillRect(Screen, &Hitbox);

	Hitbox.x = xCoord;
	Hitbox.w = (float)inputBox->xSize;
	Hitbox.h = (float)clamp(RenderSettings.HitboxOutlineThickness, 0, inputBox->ySize);
	SDL_RenderFillRect(Screen, &Hitbox);

	Hitbox.y += (float)(inputBox->ySize - clamp(RenderSettings.HitboxOutlineThickness, 0, inputBox->ySize));
	SDL_RenderFillRect(Screen, &Hitbox);

	// SDL why do you make it so difficult to draw thick lines )':
	if (inputBox->solid == FLAT_SLOPE)
	{
		float xCoord2 = xCoord + (float)inputBox->xSize;
		float yCoord2 = yCoord + (float)inputBox->ySize;

		if (inputBox->xFlip == -1)
		{
			float temp = xCoord;
			xCoord = xCoord2;
			xCoord2 = temp;
		}

		if (inputBox->yFlip == -1)
		{
			float temp = yCoord;
			yCoord = yCoord2;
			yCoord2 = temp;
		}

		SDL_RenderLine(Screen, xCoord, yCoord2, xCoord2, yCoord);
		SDL_RenderLine(Screen, xCoord, yCoord2 - 2.0, xCoord2 - 2.0, yCoord);
		SDL_RenderLine(Screen, xCoord, yCoord2 - 1.0, xCoord2 - 1.0, yCoord);
		SDL_RenderLine(Screen, xCoord + 2.0, yCoord2, xCoord2, yCoord + 2.0);
		SDL_RenderLine(Screen, xCoord + 1.0, yCoord2, xCoord2, yCoord + 1.0);
	}

	return LEMON_SUCCESS;
}

int renderObject(Camera inputCamera, Object *input, SDL_Renderer *Screen)
{
	if (RenderSettings.drawnObjects >= RenderSettings.maxObjects)
	{
		return ACTION_DISABLED;
	}

	DisplayData inputData = (*getDisplay(input));
	PhysicsBox inputBox = (*input->ObjectBox);

	if (inputData.hidden || inputData.RenderModeOverride == DO_NOT_RENDER || inputData.spriteBuffer == NULL)
	{
		return ACTION_DISABLED;
	}

	if (inputData.transparency > 0.999)
	{
		setTransparency(input, 1.0);
		return EXECUTION_UNNECESSARY;
	}

	const Sprite *spritePtr = inputData.spriteBuffer;


	// Locate object on screen
	float realXOffset = (inputCamera.width >> 1) + inputBox.xPos + inputData.spriteXOffset - inputCamera.CameraX;
	float realYOffset = (inputCamera.height >> 1) - inputBox.yPos - inputData.spriteYOffset + inputCamera.CameraY - inputBox.ySize;
	double renderDirection = inputBox.direction;

	if (inputData.frameBuffer != NULL)
	{
		if (inputData.rotateMode == LEFT_RIGHT_ROTATION && inputBox.xFlip == -1)
		{
			renderDirection -= inputData.frameBuffer->rotation;
		}
		else 
		{
			renderDirection += inputData.frameBuffer->rotation;
		}

		realXOffset += inputData.frameBuffer->SpriteXOffset;
		realYOffset -= inputData.frameBuffer->SpriteYOffset;
		// This line is unnecessary and really just a safety instruction
		spritePtr = inputData.frameBuffer->frameSprite;
	}


	if (inputData.rotateMode == DONT_ROTATE)
	{
		renderDirection = DEFAULT_DIRECTION;
	}


	float xOffset2 = realXOffset + inputBox.xSize;
	float yOffset2 = realYOffset + inputBox.ySize;

	// Decide how to render
	RenderMode inputRenderMode = inputData.RenderModeOverride;

	if (inputRenderMode == DEFAULT_TO_SPRITE)
	{
		inputRenderMode = spritePtr->RenderMode;
	}

	if (inputRenderMode == TILE || inputRenderMode == TILE_FAST)
	{
		goto Skip_Render_Effects;
	}

	float centerX = realXOffset + (inputBox.xSize >> 1);
	float centerY = realYOffset + (inputBox.ySize >> 1);

	if (inputRenderMode == SINGLE)
	{
		inputBox.xSize = spritePtr->width;
		inputBox.ySize = spritePtr->height;
	}

	if (inputData.size > 0.00001 && fabs(inputData.size - 1.0) > 0.0001)	// Modify box size/pos and rendermode if size value is set to a non 1.0 value
	{
		inputBox.xSize = (int)(inputBox.xSize * inputData.size);
		inputBox.ySize = (int)(inputBox.ySize * inputData.size);

		inputBox.xPos = (float)(centerX - (inputBox.xSize >> 1));
		inputBox.yPos = (float)(centerY - (inputBox.ySize >> 1));	

		inputRenderMode = SCALE;				
	}

	realXOffset = centerX - (inputBox.xSize >> 1);
	realYOffset = centerY - (inputBox.ySize >> 1);
	xOffset2 = realXOffset + inputBox.xSize;
	yOffset2 = realYOffset + inputBox.ySize;

	Skip_Render_Effects:

	if (realXOffset >= inputCamera.zoomedWidth || xOffset2 < 0 || yOffset2 < 0 || realYOffset >= inputCamera.zoomedHeight || realXOffset >= xOffset2 || realYOffset >= yOffset2)
	{
		return INVALID_DATA;
	}


	// render polygon if it exists
	Polygon *polygon = getPolygon(input);

	if (polygon != NULL)
	{
		SDL_Vertex renderPoly[polygon->vertices];
		memcpy(renderPoly, polygon->vertexList, polygon->vertices * sizeof(SDL_Vertex));

		for (int i = 0; i < polygon->vertices; i++)
		{
			renderPoly[i].position.x += (float)realXOffset;
			renderPoly[i].position.y += (float)realYOffset;
			renderPoly[i].color.a -= inputData.transparency;
		}

		SDL_RenderGeometry(Screen, spritePtr->texture, renderPoly, polygon->vertices, polygon->indicies, 0);
		return LEMON_SUCCESS;
	}


	Uint8 alphaVal = (Uint8)fClamp((1.0 - inputData.transparency) * 255.0, 0.0, 255.0); 
	SDL_SetTextureAlphaMod(spritePtr->texture, alphaVal);

	SDL_FRect renderBox;
	renderBox.x = (float)realXOffset;
	renderBox.y = (float)realYOffset;
	renderBox.w = (float)inputBox.xSize;
	renderBox.h = (float)inputBox.ySize;

	SDL_FlipMode flip = SDL_FLIP_NONE;

	if (inputBox.xFlip == -1)
	{
		flip = SDL_FLIP_HORIZONTAL;
	}

	if (inputBox.yFlip == -1)
	{
		flip |= SDL_FLIP_VERTICAL;
	}

	if (inputRenderMode == TILE || inputRenderMode == TILE_FAST)
	{
		renderTiledSprite(Screen, inputCamera, inputData, &renderBox, getTileMap(input));
	}
	else if (renderDirection != DEFAULT_DIRECTION)
	{
		SDL_RenderTextureRotated(Screen, spritePtr->texture, NULL, &renderBox, renderDirection - DEFAULT_DIRECTION, NULL, flip);
	}
	else
	{
		SDL_RenderTextureRotated(Screen, spritePtr->texture, NULL, &renderBox, 0.0, NULL,flip);
	}
	
	RenderSettings.drawnObjects++;

	return LEMON_SUCCESS;
}

void cullTiledSprite(SDL_FRect *renderBox, Camera inputCamera, float sWidth, float sHeight)
{
	float calculated;
	if (renderBox->x < 0.0)
	{
		calculated = fModulo(renderBox->x, sWidth) - sWidth;	
		renderBox->w += renderBox->x - calculated;
		renderBox->x = calculated;
	}

	if (renderBox->y < 0.0)
	{
		calculated = fModulo(renderBox->y, sHeight) - sHeight;
		renderBox->h += renderBox->y - calculated;
		renderBox->y = calculated;
	}

	if ((renderBox->x + renderBox->w) > inputCamera.zoomedWidth)
	{
		float offset = inputCamera.zoomedWidth - renderBox->x;
		renderBox->w = offset + fModulo((renderBox->w + offset), sWidth); 
	}

	if ((renderBox->y + renderBox->h) > inputCamera.zoomedHeight)
	{
		float offset = inputCamera.zoomedHeight - renderBox->y;
		renderBox->h = offset + fModulo((renderBox->h + offset), sHeight); 
	}
}

int renderTiledSprite(SDL_Renderer *Screen, Camera inputCamera, DisplayData inputData, SDL_FRect *renderBox, TileMap *map)
{
	const Sprite *sprite = inputData.spriteBuffer;
	SDL_FRect spriteBox;


	if (map == NULL || sprite->width < (map->tileSize * 3) || sprite->height < (map->tileSize * 3))
	{
		float modPixelY = (float)modulo(inputData.pixelYOffset, sprite->height);
		float fHeight = (float)sprite->height;
		spriteBox.x = (float)inputData.pixelXOffset;
		spriteBox.y = fModulo((fHeight - renderBox->h - modPixelY), fHeight);
		spriteBox.w = (float)sprite->width - spriteBox.x;
		spriteBox.h = fHeight - modPixelY;

		// cull rendered box to visible portion, as tiled sprites can be infinitely long
		cullTiledSprite(renderBox, inputCamera, spriteBox.w, spriteBox.h);

		SDL_RenderTextureTiled(Screen, sprite->texture, &spriteBox, inputData.size, renderBox);

		return LEMON_SUCCESS;
	}

	SDL_FRect tileBox;
	float renderSize = map->tileSize * inputData.size;
	float renderSize2 = renderSize * 2.0;

	//cullTiledSprite(renderBox, inputCamera, renderSize, renderSize);

	// render middle chunk
	spriteBox.x = map->centerTileX;
	spriteBox.y = map->centerTileY;
	spriteBox.w = map->tileSize;
	spriteBox.h = map->tileSize;

	tileBox.x = renderBox->x + renderSize;
	tileBox.w = renderBox->w - renderSize2;
	tileBox.y = renderBox->y + renderSize;
	tileBox.h = renderBox->h - renderSize2;

	if (renderBox->w > renderSize2)
	{
		cullTiledSprite(&tileBox, inputCamera, map->tileSize, map->tileSize);
		if (renderBox->h > renderSize2)
		{
			SDL_RenderTextureTiled(Screen, sprite->texture, &spriteBox, inputData.size, &tileBox);
		}

		if (renderBox->h > renderSize)
		{
			// render middle bottom line
			spriteBox.y = map->centerTileY + map->tileSize;

			tileBox.y = renderBox->y + renderBox->h - renderSize;
			tileBox.h = renderSize;
		
			SDL_RenderTextureTiled(Screen, sprite->texture, &spriteBox, inputData.size, &tileBox);
		}

		// render top middle line
		spriteBox.y = map->centerTileY - map->tileSize;

		tileBox.y = renderBox->y;
		tileBox.h = renderSize;

		SDL_RenderTextureTiled(Screen, sprite->texture, &spriteBox, inputData.size, &tileBox);
	}

	if (renderBox->w > renderSize)
	{
		// render middle right line
		spriteBox.x = map->centerTileX + map->tileSize;
		spriteBox.y = map->centerTileY;

		tileBox.x = renderBox->x + renderBox->w - renderSize;
		tileBox.y = renderBox->y + renderSize;
		tileBox.w = renderSize;
		tileBox.h = renderBox->h - renderSize2;

		cullTiledSprite(&tileBox, inputCamera, map->tileSize, map->tileSize);

		if (renderBox->h > renderSize2)
		{
			SDL_RenderTextureTiled(Screen, sprite->texture, &spriteBox, inputData.size, &tileBox);
		}

		if (renderBox->h > renderSize)
		{
			// render bottom right corner
			spriteBox.y = map->centerTileY + map->tileSize;

			tileBox.y = renderBox->y + renderBox->h - renderSize;
			tileBox.h = renderSize;
		
			SDL_RenderTextureTiled(Screen, sprite->texture, &spriteBox, inputData.size, &tileBox);
		}
		

		// render top right corner
		spriteBox.y = map->centerTileY - map->tileSize;

		tileBox.y = renderBox->y;
		tileBox.h = renderSize;

	  	SDL_RenderTextureTiled(Screen, sprite->texture, &spriteBox, inputData.size, &tileBox);
	}

	// render middle left line
	spriteBox.x = map->centerTileX - map->tileSize;
	spriteBox.y = map->centerTileY;

	tileBox.x = renderBox->x;
	tileBox.y = renderBox->y + renderSize;
	tileBox.w = renderSize;
	tileBox.h = renderBox->h - renderSize2;

	cullTiledSprite(&tileBox, inputCamera, map->tileSize, map->tileSize);

	if (renderBox->h > renderSize2)
	{
		SDL_RenderTextureTiled(Screen, sprite->texture, &spriteBox, inputData.size, &tileBox);
	}

	if (renderBox->h > renderSize)
	{
		// render bottom left corner
		spriteBox.y = map->centerTileY + map->tileSize;

		tileBox.y = renderBox->y + renderBox->h - renderSize;
		tileBox.h = renderSize;

		SDL_RenderTextureTiled(Screen, sprite->texture, &spriteBox, inputData.size, &tileBox);
	}
	

	// render top left corner
	spriteBox.y = map->centerTileY - map->tileSize;

	tileBox.y = renderBox->y;
	tileBox.h = renderSize;

	SDL_RenderTextureTiled(Screen, sprite->texture, &spriteBox, inputData.size, &tileBox);


	return LEMON_SUCCESS;
}

int renderBackGroundSprite(Camera inputCamera, BackgroundData *WorldBackground, SDL_Renderer *Screen)
{
	if (Screen == NULL || WorldBackground == NULL || WorldBackground->BackgroundSpriteBuffer == NULL)
	{ 
		return MISSING_DATA;
	}

	inputCamera.CameraX += (float)((inputCamera.width - inputCamera.zoomedWidth) >> 1);
	inputCamera.CameraY += (float)((inputCamera.zoomedHeight - inputCamera.height) >> 1);
	

	RenderMode bgRenderMode = WorldBackground->BackgroundRenderMode;

	if (bgRenderMode == DEFAULT_TO_SPRITE)
	{
		bgRenderMode = WorldBackground->BackgroundSpriteBuffer->RenderMode;
	}

	if (RenderSettings.drawBackGround == false)
	{
		return ACTION_DISABLED;
	}


	Sprite *backGround = WorldBackground->BackgroundSpriteBuffer;
	SDL_FRect renderTarget = {0};

	switch (bgRenderMode)
	{
	case STATIC_BACKGROUND:
		SDL_RenderTexture(Screen, backGround->texture, NULL, NULL);
		break;

	case SINGLE_BACKGROUND:
		renderTarget.w = (float)backGround->width;
		renderTarget.h = (float)backGround->height;
		renderTarget.x = (inputCamera.zoomedWidth >> 1) - inputCamera.CameraX * WorldBackground->bgParallax - (backGround->width >> 1);
		renderTarget.y = inputCamera.CameraY * WorldBackground->bgParallax + (inputCamera.zoomedHeight >> 1) - (backGround->height >> 1);

		SDL_RenderTexture(Screen, backGround->texture, NULL, &renderTarget);
		break;

	case TILE_BACKGROUND:
		renderTarget.w = (float)(inputCamera.zoomedWidth << 1);
		renderTarget.h = (float)(inputCamera.zoomedHeight << 1);
		renderTarget.x = (float)modulo((inputCamera.zoomedWidth >> 1) - inputCamera.CameraX * WorldBackground->bgParallax - (backGround->width >> 1), backGround->width) - backGround->width;
		renderTarget.y = (float)modulo(inputCamera.CameraY * WorldBackground->bgParallax + (inputCamera.zoomedHeight >> 1) - (backGround->height >> 1), backGround->height) - backGround->height;

		SDL_RenderTextureTiled(Screen, backGround->texture, NULL, 1.0, &renderTarget);
		break;

	case TILEPLANE_BACKGROUND:
		if (WorldBackground->tileBG.tiles == NULL)
		{
			break;
		}
		TilePlane *plane = &WorldBackground->tileBG;

		SDL_FRect tilePos = {0};
		tilePos.w = plane->tileWidth;
		tilePos.h = plane->tileHeight;
		renderTarget.w = tilePos.w * inputCamera.zoomX;
		renderTarget.h = tilePos.h * inputCamera.zoomY;

		int pitch = backGround->width / tilePos.w;
		int gridWidth = plane->GridWidth;
		int gridHeight = plane->GridHeight;
		int renderTilesX = (inputCamera.zoomedWidth / tilePos.w) + 2;
		int renderTilesY = (inputCamera.zoomedHeight / tilePos.h) + 2;

		Uint8 *tiles = plane->tiles;
		Uint8 tileID = 0;

		renderTarget.y = inputCamera.CameraY + (inputCamera.zoomedHeight >> 1) - plane->yPos - (gridHeight * tilePos.h);
		float xOrigin = -inputCamera.CameraX + plane->xPos + (inputCamera.zoomedWidth >> 1);

		int startIndexY = fClamp((-renderTarget.y / tilePos.h), 0, (float)gridHeight);
		renderTarget.y += startIndexY * (int)tilePos.h - (floor(inputCamera.CameraY) - inputCamera.CameraY);

		int startIndexX = fClamp((-xOrigin / tilePos.w), 0, (float)gridWidth);
		xOrigin += startIndexX * (int)tilePos.w + (floor(inputCamera.CameraX) - inputCamera.CameraX);

		int yIndex;

		for (int y = startIndexY; y < gridHeight && y < startIndexY + renderTilesY; y++)
		{
			renderTarget.x = xOrigin;

			for (int x = startIndexX; x < gridWidth && x < startIndexX + renderTilesX; x++)
			{
				tileID = tiles[(y * gridWidth) + x];
				if (tileID != 0)
				{
					yIndex = (tileID - 1) / pitch;
					tilePos.y = yIndex * tilePos.h;
					tilePos.x = ((tileID - 1) - (yIndex * pitch)) * tilePos.w;

					SDL_RenderTexture(Screen, backGround->texture, &tilePos, &renderTarget);
				}

				renderTarget.x += renderTarget.w;
			}

			renderTarget.y += renderTarget.h;
		}
		break;

	default:
		break;
	}


	return LEMON_SUCCESS;
}


int getTileAtPosition(float x, float y, TilePlane *input)
{
	if (input == NULL)
	{
		return -1;
	}

	int tileH = input->tileHeight;
	int tileW = input->tileWidth;
	int gridWidth = input->GridWidth;
	int gridHeight = input->GridHeight;

	int TileX = (int)(x - input->xPos) / tileW;
	int TileY = (int)(y - input->yPos) / tileH;

	//printf("\nxPos: %f yPos: %f  tileX: %d  tileY: %d\n", input->xPos, input->yPos, TileX, TileY);

	if (TileX < 0 || TileX >= gridWidth || TileY < 0 || TileY >= gridHeight)
	{
		return 0;
	}


	return input->tiles[(TileY * gridWidth) + TileX];
}


// Debug text functions
void DisplayDebugInfo(Camera renderCamera, World *GameWorld, SDL_Renderer *Screen)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return;
	}

	char text[DEBUG_TEXT_MAX_LENGTH] = {0};

	if (DebugSettings.DebugOverlay != 0)
	{
		if (DebugSettings.PauseEngine == 1)
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, "Object Count: %d \nEngine Pause Status: Paused", GameWorld->ObjectList->objectCount);
		}
		else
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, "Object Count: %d \nEngine Pause Status: Unpaused", GameWorld->ObjectList->objectCount);
		}

		snprintf(text + strlen(text), DEBUG_TEXT_MAX_LENGTH - strlen(text), "\nGame Pause: %d    Game State: %d", GameWorld->GamePaused, GameWorld->GameState);

		snprintf(text + strlen(text), DEBUG_TEXT_MAX_LENGTH - strlen(text), "\nCutsceneID: %d    Events Pending: %d", GameWorld->CurrentCutscene, GameWorld->GameEvents.eventsPending);

		Sprite *bgSprite = GameWorld->WorldBackground.BackgroundSpriteBuffer;
		if (bgSprite != NULL)
		{
			snprintf(text + strlen(text), DEBUG_TEXT_MAX_LENGTH - strlen(text), "\nBackground sprite Name: %s \nBackground Sprite ID: %d", bgSprite->name, bgSprite->spriteID);
		}

		snprintf(text + strlen(text), DEBUG_TEXT_MAX_LENGTH - strlen(text), "\nlevel: %d \nMouse x: %f  Mouse y: %f", 
			GameWorld->level, getMouseXCam(GameWorld->MainCamera), getMouseYCam(GameWorld->MainCamera));

		AddDebugText(text, 16 - (renderCamera.width >> 1), (renderCamera.height >> 1) - 60, 0, DTFORMAT_SCREEN_RELATIVE);
	}

	DisplaySoundChannelDebugInfo(DebugSettings.SoundInfo - 1);

	// Camera Information
	switch (DebugSettings.CameraInfo)
	{
	case 1:
		sprintf(text, "Camera X: %.2f  Camera Y: %.2f", renderCamera.CameraX, renderCamera.CameraY);
		AddDebugText(text, -140, (renderCamera.height >> 1), 0, DTFORMAT_SCREEN_RELATIVE);
		break;

	case 2: 
		sprintf(text, "Camera X: %.2f  Camera Y: %.2f \nCameraLatch: %d \nBuffer X: %.2f  Buffer Y: %.2f", 
			renderCamera.CameraX, renderCamera.CameraY, renderCamera.CameraLatch, renderCamera.CameraXBuffer, renderCamera.CameraYBuffer);
		AddDebugText(text, -140, (renderCamera.height >> 1), 0, DTFORMAT_SCREEN_RELATIVE);
		break;

	case 3: 
		sprintf(text, "Camera X: %.2f  Camera Y: %.2f \nX zoom: %.2f \nY zoom X: %.2f", 
			renderCamera.CameraX, renderCamera.CameraY, renderCamera.zoomX, renderCamera.zoomY);
		AddDebugText(text, -140, (renderCamera.height >> 1), 0, DTFORMAT_SCREEN_RELATIVE);
		break;

	default:
		break;
	}


	Object *currentObject = GameWorld->ObjectList->lastObject;
	int objCount = GameWorld->ObjectList->objectCount;

	switch(DebugSettings.DebugTextDisplayMode)
	{
		case DEBUG_TEXT_DISABLED:
			break;


		case DEBUG_TEXT_ENABLED:
			while(currentObject != NULL && objCount > 0 && !MouseOverlappingBox(currentObject, renderCamera))
			{
				currentObject = currentObject->prevObject;
				objCount--;
			}

			if (currentObject == NULL)
			{
				break;
			}

			if (getDisplayLayer(currentObject) == HUD)
			{
				Camera hudCam = {0};
				ResetCamera(&hudCam);
				SDL_SetRenderScale(Screen, 1.0, 1.0);
				renderHitbox(hudCam, currentObject->ObjectBox, Screen);
				SDL_SetRenderScale(ScreenData.Renderer, renderCamera.zoomX, renderCamera.zoomY);
			}
			else
			{
				renderCamera.CameraX += (float)((renderCamera.width - renderCamera.zoomedWidth) >> 1);
				renderCamera.CameraY += (float)((renderCamera.zoomedHeight - renderCamera.height) >> 1);

				renderHitbox(renderCamera, currentObject->ObjectBox, ScreenData.Renderer);
			}
			
			DisplayObjectDebugInfo(currentObject, objCount, true, renderCamera);

			if (MouseInput.LeftButton == 1)
			{
				AcknowledgeButton(MOUSE_LEFT);
				toggleHidden(currentObject);
			}

			if (MouseInput.RightButton == 1)
			{
				AcknowledgeButton(MOUSE_RIGHT);
				if (currentObject->State == TO_BE_DELETED)
				{
					UnmarkObjectForDeletion(currentObject);
				}
				else
				{
					MarkObjectForDeletion(currentObject);
				}
			}
			break;

		case ONLY_NONSTATIC_OBJECT_INFO:
		{
			while (currentObject != NULL && objCount > 0)
			{	
				if (currentObject->State != STATIC_STATE && onScreen(currentObject, GameWorld))
				{
					DisplayObjectDebugInfo(currentObject, objCount, false, renderCamera);
				}
				
				currentObject = currentObject->prevObject;
				objCount--;
			}
		} break;

		default:
		{
			while (currentObject != NULL && objCount > 0)
			{	
				if (onScreen(currentObject, GameWorld))
				{
					DisplayObjectDebugInfo(currentObject, objCount, false, renderCamera);
				}

				currentObject = currentObject->prevObject;
				objCount--;
			}
		} break;
	}

	snprintf(text, DEBUG_TEXT_MAX_LENGTH, "%s \nTick: %llu", LEMON_VERSION, TickNumber());
	AddDebugText(text, 16 - (renderCamera.width >> 1), 48 - (renderCamera.height >> 1), 0, DTFORMAT_SCREEN_RELATIVE);

	return;
}


int DisplayObjectDebugInfo(Object *input, int objectNumber, bool goToMouse, Camera renderCamera)
{
	if (input == NULL || input->ObjectBox == NULL)
	{
		return MISSING_DATA;
	}

	PhysicsBox *inputBox = input->ObjectBox;
	DisplayData *inputDisplay = getDisplay(input);

	// Create Lines
	char text[DEBUG_TEXT_MAX_LENGTH] = {0};

	
	switch(DebugSettings.DebugTextInfoPreset)
	{
	case 1:
		snprintf(text, DEBUG_TEXT_MAX_LENGTH, 
			"XPos: %.2f xPosRight: %.2f \nYPos: %.2f yPosTop: %.2f \nPrevious XPos: %.2f \nPrevious YPos: %.2f", 
			inputBox->xPos, inputBox->xPos + inputBox->xSize, inputBox->yPos, inputBox->yPos + inputBox->ySize, 
			inputBox->prevXPos, inputBox->prevYPos);
	break;

	case 2:
		snprintf(text, DEBUG_TEXT_MAX_LENGTH, 
			"ForwardVel: %.2f \nXVel: %.2f \nYVel: %.2f \nXPos: %.2lf \nYPos: %.2lf", 
			inputBox->forwardVelocity, inputBox->xVelocity, inputBox->yVelocity, inputBox->xPos, inputBox->yPos);
		break;

	case 3:
		snprintf(text, DEBUG_TEXT_MAX_LENGTH, 
			"xFlip: %d \nyFlip: %d \nBox Direction: %.2lf \nIn Air: %d \nPhysicsXVel: %.2f \nPhysicsYVel: %.2f", 
			inputBox->xFlip, inputBox->yFlip, inputBox->direction, inputBox->inAir, inputBox->PhysicsXVelocity, inputBox->PhysicsYVelocity);
		break;

	case 4:
		snprintf(text, DEBUG_TEXT_MAX_LENGTH, 
			"SolidType: %d (%s) \nSolidFlag: %d (%s) \nCollision Layer: %d \nObject List position: %d \nIndex: %d", 
			inputBox->solid, getSolidTypeName(inputBox->solid), inputBox->flag, getSolidFlagName(inputBox->flag), inputBox->collideLayer, objectNumber, input->index);
		break;

	case 5:
		if (inputBox->GroundBox == NULL)
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, "No ground detected \nIn Air: %d", inputBox->inAir);
		}
		else
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, 
			"GroundBox XPos: %.2f \nGroundBox YPos: %.2f \nGroundBox solid: %d \nGroundBox collide layer: %d \nIn Air: %d", 
			inputBox->GroundBox->xPos, inputBox->GroundBox->yPos, inputBox->GroundBox->solid, inputBox->GroundBox->collideLayer, inputBox->inAir);
		}
		break;


	case 6:
		if (inputDisplay->spriteBuffer == NULL)
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, "Sprite ID: %d\nNo Sprite in buffer", inputDisplay->currentSprite);
		}
		else
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, "Sprite ID: %d \nSprite name: %s \nSprite Rendermode: %s", 
				inputDisplay->currentSprite, inputDisplay->spriteBuffer->name, getRenderModeName(inputDisplay->spriteBuffer->RenderMode));
		}
		
		break;


	case 7:
		snprintf(text, DEBUG_TEXT_MAX_LENGTH, "Rendermode Override: %s \nTransparency: %f", 
			getRenderModeName(inputDisplay->RenderModeOverride), inputDisplay->transparency);

		int length = strlen(text);
		if (inputDisplay->hidden)
		{
			snprintf(text + length, DEBUG_TEXT_MAX_LENGTH - length, "\nHidden");
		}
		else
		{
			snprintf(text + length, DEBUG_TEXT_MAX_LENGTH - length, "\nNot Hidden");
		}

		length = strlen(text);
		SpriteSet *set = inputDisplay->spriteSetSource;
		if (set == NULL)
		{
			snprintf(text + length, DEBUG_TEXT_MAX_LENGTH - length, "\nNo sprite set");
		}
		else if (set->copies == NULL || set->setID == input->ObjectID)
		{
			if (set->copies != NULL && set->copyCount > 0)
			{
				snprintf(text + length, DEBUG_TEXT_MAX_LENGTH - length, "\nThis is a unique set \nUsed by: ");

				int i = 0;
				while (i < set->copyCount - 1)
				{
					length = strlen(text);
					snprintf(text + length, DEBUG_TEXT_MAX_LENGTH - length, "%d, ", set->copies[i]);
					i++;
				}

				length = strlen(text);
				snprintf(text + length, DEBUG_TEXT_MAX_LENGTH - length, "%d", set->copies[i]);
			}
			else
			{
				snprintf(text + length, DEBUG_TEXT_MAX_LENGTH - length, "\nThis is a unique set");
			}
		}
		else
		{
			snprintf(text + length, DEBUG_TEXT_MAX_LENGTH - length, "\nBased on set %d (%s)", 
				set->setID, getObjectIDName(set->setID));
		}
		break;


	case 8:
		if (inputDisplay->spriteBuffer == NULL)
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, "No Sprite to display");
			break;
		}
		snprintf(text, DEBUG_TEXT_MAX_LENGTH, "Sprite name: %s \nX Pos Offset: %f \nY Pos Offset: %f \nX Pixel Offset: %d \nY Pixel Offset: %d", 
				inputDisplay->spriteBuffer->name, inputDisplay->spriteXOffset, inputDisplay->spriteYOffset, inputDisplay->pixelXOffset, inputDisplay->pixelYOffset);
		break;


	case 9:
		if (inputDisplay->currentAnimation < 1 || inputDisplay->animationBuffer == NULL)
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, "No animation currently Playing");
			break;
		}

		if (inputDisplay->animationLoopCount == LOOP_INDEFINITELY)
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, 
			"Animation Name: %s \nCurrent Animation ID: %d \nFrameRate: %.2f \nAnimation Tick: %f \nLoops Remaining: Indefinite", 
			inputDisplay->animationBuffer->name, inputDisplay->currentAnimation, 
			1.0 / inputDisplay->animationBuffer->frameRate,
			inputDisplay->animationTick);
		}
		else if (inputDisplay->animationLoopCount == ONE_FRAME_INDEFINITE_ANIMATION)
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, 
			"Animation Name: %s \nCurrent Animation ID: %d \nFrameRate: %.2f \nAnimation Tick: %f \nPlayback halted; Single frame & indefinite", 
			inputDisplay->animationBuffer->name, inputDisplay->currentAnimation, 
			(float)EngineSettings.GameTicksPerSecond / inputDisplay->animationBuffer->frameRate,
			inputDisplay->animationTick);
		}
		else
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, 
			"Animation Name: %s \nCurrent Animation ID: %d \nFrameRate: %.2f \nAnimation Tick: %f \nLoops Remaining: %d", 
			inputDisplay->animationBuffer->name, inputDisplay->currentAnimation, 
			1.0 / inputDisplay->animationBuffer->frameRate,
			inputDisplay->animationTick, inputDisplay->animationLoopCount);
		}
		break;


	case 10:
		if (inputDisplay->currentAnimation < 1 || inputDisplay->animationBuffer == NULL)
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, "No animation currently Playing");
			break;
		}
		snprintf(text, DEBUG_TEXT_MAX_LENGTH, 
			"Animation Name: %s \nCurrent Animation ID: %d \nFrame XPos Offset: %f \nFrame YPos Offset: %f \nFrame Rotation Offset: %.2lf", 
			inputDisplay->animationBuffer->name, inputDisplay->currentAnimation, inputDisplay->frameBuffer->SpriteXOffset,
			inputDisplay->frameBuffer->SpriteYOffset, (double)inputDisplay->frameBuffer->rotation);
		break;


	case 11:
		if (inputDisplay->currentAnimation < 1 || inputDisplay->animationBuffer == NULL)
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, 
			"Box Direction: %.2lf \nDisplay rotateMode: %d \nNo animation currently Playing", 
			inputBox->direction, inputDisplay->rotateMode);
		}
		else
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, 
			"Box Direction: %.2lf \nDisplay rotateMode: %d \nFrame XPos Offset: %f \nFrame YPos Offset: %f \nFrame Rotation Offset: %.2lf", 
			inputBox->direction, inputDisplay->rotateMode, 
			inputDisplay->frameBuffer->SpriteXOffset, inputDisplay->frameBuffer->SpriteYOffset, (double)inputDisplay->frameBuffer->rotation);
		}
		break;


	case 12:
		snprintf(text, DEBUG_TEXT_MAX_LENGTH, 
			"Object State: %d (%s) \nCurrent Action: %d \nDisplayLayer: %d (%s)", 
			input->State, getObjectStateName(input->State), input->Action, getDisplayLayer(input), getLayerName(getDisplayLayer(input)));
		break;


	case 13:
		snprintf(text, DEBUG_TEXT_MAX_LENGTH, 
			"Arg1: %d \nArg2: %d \nArg3: %d \nArg4: %d", 
			input->arg1, input->arg2, input->arg3, input->arg4);
		break;


	case 14:
		if (input->Parent == NULL)
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, "ObjectID: %d \nNo Parent Object attached", input->ObjectID);
		}
		else if (input->Parent->ObjectBox == NULL)
		{
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, "ObjectID: %d \nParentID: %d \nParentLink: %x \nParent has no Object Box!", 
			input->ObjectID, input->Parent->ObjectID, input->ParentLink);
		}
		else
		{
			PhysicsBox *ParentBox = input->Parent->ObjectBox;
			snprintf(text, DEBUG_TEXT_MAX_LENGTH, "ObjectID: %d \nParentID: %d \nParentLink: %x \nParent XPos: %.2f \nParent YPos: %.2f", 
			input->ObjectID, input->Parent->ObjectID, input->ParentLink, ParentBox->xPos, ParentBox->yPos);
		}
		break;

	default:
		snprintf(text, DEBUG_TEXT_MAX_LENGTH, "ObjectID: %d (%s) \nObjectName: %s \nXPos: %.2f \nYPos: %.2f", 
			input->ObjectID, getObjectIDName(input->ObjectID), input->name, inputBox->xPos, inputBox->yPos);	
		break;
	}
	

	if(goToMouse)
	{
		AddDebugText(text, MouseInput.xPos, MouseInput.yPos - 16.0, 0, DTFORMAT_SCREEN_RELATIVE);
		return LEMON_SUCCESS;
	}

	if (getDisplayLayer(input) == HUD)
	{
		AddDebugText(text, inputBox->xPos, inputBox->yPos, 0, DTFORMAT_SCREEN_RELATIVE);
	}
	else
	{
		AddDebugText(text, inputBox->xPos + renderCamera.CameraX, inputBox->yPos + renderCamera.CameraY, 0, DTFORMAT_SCREEN_RELATIVE);
	}
	

	return LEMON_SUCCESS;
}


int AddDebugText(const char inputPhrase[], float x, float y, int wrapwidth, DebugTextFormatting format)
{
	TextList *list = &TextSettings.DebugTexts;
	if (inputPhrase == NULL || strlen(inputPhrase) < 1 || list->count >= MAX_TEXT_TEXTURES)
	{
		return INVALID_DATA;
	}

	Text *TextsArray = list->texts; 

	int index = 0;
	while (index < MAX_TEXT_TEXTURES && TextsArray[index].beingUsed)
	{
		index++;
	}

	if (index >= MAX_TEXT_TEXTURES)
	{
		list->count = MAX_TEXT_TEXTURES;
		return ACTION_DISABLED;
	}
	
	list->count++;

    if (format == DTFORMAT_LIST_SOUND)
    {
    	TextsArray[index].yPos = 50.0 - (y * 20.0);
    }
    else
    {
    	TextsArray[index].yPos = y;
    }

    TextsArray[index].xPos = x;

    TextsArray[index].CameraRelative = false;
    TextsArray[index].beingUsed = true;
    TextsArray[index].textBox = NULL;
    
    if (TextsArray[index].text == NULL)
    {
    	TextsArray[index].text = TTF_CreateText(ScreenData.textEngine, getFont("DebugFont"), inputPhrase, wrapwidth);
    }
    else
    {
    	TTF_SetTextString(TextsArray[index].text, inputPhrase, 0);
    	TTF_SetTextWrapWidth(TextsArray[index].text, wrapwidth);
    }

    if (format == DTFORMAT_JUSTIFY_TOP)
    {
    	int height = 0;
	    TTF_GetTextSize(TextsArray[index].text, NULL, &height);

	    TextsArray[index].yPos += (float)height;
    }
    
	return index;
}

float getCursorPos(void)
{
	if (DebugSettings.userInputIndex < 1)
	{
		return 0.0;
	}

	SDL_Surface *text = TTF_RenderText_Blended_Wrapped(getFont("DebugFont"), DebugSettings.userInputString, DebugSettings.userInputIndex, TextSettings.DebugTextColour, 0);

    if (text == NULL)
    {
    	return LEMON_ERROR;
    }

    float xPos = text->w;

    SDL_DestroySurface(text);

    return xPos;
}


void renderTexts(Camera renderCamera, World *GameWorld, SDL_Renderer *Screen)
{
	SDL_SetRenderScale(Screen, 1.0, 1.0);
	SDL_SetRenderLogicalPresentation(Screen, ScreenData.screenWidth, ScreenData.screenHeight, SDL_LOGICAL_PRESENTATION_STRETCH);

	// render in-game text (immune to camera zoom, centered on the screen)
	if (GameWorld->GamePaused == 0)
	{
		RenderTextList(&TextSettings.TextList, renderCamera);
	}
	
	// render console
	if (DebugSettings.TypingInConsole)
	{
		renderConsole(GameWorld, Screen);	
	}

	// load debug text to be rendered if in debug mode
	if (DebugSettings.DebugTextDisplayMode != DEBUG_TEXT_DISABLED)
    {
    	DisplayDebugInfo(renderCamera, GameWorld, Screen);	
    }

    RenderTextList(&TextSettings.DebugTexts, renderCamera);
    RemoveAllTexts(&TextSettings.DebugTexts);

    SDL_SetRenderScale(Screen, renderCamera.zoomX, renderCamera.zoomY);
    SDL_SetRenderLogicalPresentation(Screen, renderCamera.width, renderCamera.height, SDL_LOGICAL_PRESENTATION_STRETCH);

    return;
}

void RenderTextList(TextList *list, Camera inputCamera)
{
	if (list->count < 1)
	{
		return;
	}

	Text *array = list->texts; 

	float correctedX, correctedY;
	for (int i = 0; i < MAX_TEXT_TEXTURES; i++)
	{
		if (array[i].text == NULL || !array[i].beingUsed)
		{
			continue;
		}

		correctedX = (ScreenData.screenWidth >> 1) + array[i].xPos;
		correctedY = (ScreenData.screenHeight >> 1) - array[i].yPos;

		if (array[i].CameraRelative == true)
		{
			correctedX -= inputCamera.CameraX;
			correctedY += inputCamera.CameraY;
		}

		if (array[i].textBox != NULL)
		{
			TextBox *instance = array[i].textBox;
			PhysicsBox *box = instance->boxPtr->ObjectBox;
			correctedX += box->xPos;
			correctedY -= box->yPos + instance->TextSize + 10.0;
		}

		TTF_DrawRendererText(array[i].text, correctedX, correctedY);
	}

	return;
}

