#include "LemonEngine/LemonEngine.h"
#include "editor.h"


SDL_Window *editorWindow = NULL;
SDL_Renderer *editorScreen = NULL;
int windowWidth = 0;
int windowHeight = 0;

SDL_Texture *engineView = NULL;
SDL_FRect engineBox = {0};
float viewXPos = 150.0;
float viewYPos = 0.0;
bool showEngine = true;

 
int main(void)
{
	World GameWorld;
	if (startUpLemonInstance(&GameWorld) != LEMON_SUCCESS)
	{
		return LEMON_ERROR;
	}

	bool closeEditor = false;

	while (closeEditor == false)
	{
		getExternalInput(&GameWorld, NULL);
		if (GameWorld.GameState == CLOSE_GAME)
		{
			closeEditor = true;
		}

		moveEngineCamera(&GameWorld);

		renderEditor(&GameWorld, engineView);
	}

	CloseGame(&GameWorld, &ScreenData);

	return 0;
}


int startUpLemonInstance(World *GameWorld)
{
	if (StartUpLemonEngine() == LEMON_ERROR)
	{
		return LEMON_ERROR;
	}

	editorWindow = ScreenData.Window;
	editorScreen = ScreenData.Renderer;
	SDL_SetWindowResizable(editorWindow, true);

	// engine view
	engineBox.w = 600.0;
	engineBox.h = 450.0;
	engineView = SDL_CreateTexture(editorScreen, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)engineBox.w, (int)engineBox.h);
	if (engineView == NULL)
	{
		CloseGame(GameWorld, &ScreenData);

		return LEMON_ERROR;
	}

	SDL_SetRenderLogicalPresentation(ScreenData.Renderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);

	SDL_SetRenderTarget(ScreenData.Renderer, engineView);

	if (initialiseWorld(GameWorld) != LEMON_SUCCESS)
    {
    	return LEMON_ERROR;
    }

  	GameWorld->MainCamera.width = 960;
  	GameWorld->MainCamera.height = 720;
	StartGame(GameWorld);

	return LEMON_SUCCESS;
}

void renderEditor(World *GameWorld, SDL_Texture *engineView)
{
	SDL_RenderClear(editorScreen);

	if (showEngine)
	{
		SDL_SetRenderTarget(ScreenData.Renderer, engineView);
		Render(GameWorld, &ScreenData);
	}
	
	SDL_SetRenderTarget(ScreenData.Renderer, NULL);

	SDL_GetWindowSize(editorWindow, &windowWidth, &windowHeight);
	engineBox.x = (windowWidth >> 1) + viewXPos;
	engineBox.y = (windowHeight >> 1) - viewYPos - engineBox.h;
	if (engineBox.y < 0.0)
	{
		engineBox.y = 0.0;
	}

	SDL_RenderTexture(editorScreen, engineView, NULL, &engineBox);

	SDL_RenderPresent(editorScreen);

	return;
}

void moveEngineCamera(World *GameWorld)
{
	if (keyboard[LMN_LEFT])
	{
		GameWorld->MainCamera.CameraX -= 5.0;
	}

	if (keyboard[LMN_RIGHT])
	{
		GameWorld->MainCamera.CameraX += 5.0;
	}

	if (keyboard[LMN_UP])
	{
		GameWorld->MainCamera.CameraY += 5.0;
	}

	if (keyboard[LMN_DOWN])
	{
		GameWorld->MainCamera.CameraY -= 5.0;
	}
}