#include <Windows.h>

#include "LemonMain.h"

// gcc main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c -o LemonEngine.exe -lgdi32 -lwinmm -O3 -I C:\Libraries\SDL3-3.2.10\i686-w64-mingw32\include -L C:\Libraries\SDL3-3.2.10\i686-w64-mingw32\lib -lSDL3
// git add main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c LemonMain.h gameObjects.h levelLoader.h drawScreen.h playerController.h spriteLoader.h soundProcessor.h

// Global variables
const char g_szClassName[] = "myWindowClass";

static BITMAPINFO frame_bitmap_info;
static HBITMAP frame_bitmap = 0;
static HDC frame_device_context = 0;

int keyboard[256] = {0};

RenderFrame frame = {0};


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Window Initialisation
	printf("Windows API initialised!\n\n");
	fflush(stdout);

    WNDCLASSEX wc;
    HWND handle;
    MSG Msg = { 0 };

    // Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Creating screen
    frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
    frame_bitmap_info.bmiHeader.biPlanes = 1;
    frame_bitmap_info.bmiHeader.biBitCount = 32;
    frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
    frame_device_context = CreateCompatibleDC(0);

    // Creating the Window
    handle = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "The Lemon Engine",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, H_RESOLUTION, V_RESOLUTION,
        NULL, NULL, hInstance, NULL);

    if(handle == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }


	// SDL initialisation
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0)
	{
		printf("Failed to initialise SDL\n");
		return -1;
	}


	initialiseAudio();


	// Core engine variables/data
	clock_t gameTick = clock();
    clock_t lastTick = clock();
    clock_t lastSecond = clock();

    int windowsFrames = 0;


    // Game initialisation
    PlayerData *player;
    World *GameWorld;

    GameWorld = initialiseGame(NULL);
    player = initialisePlayer(GameWorld);


    // test data
	loadLevel(GameWorld, 1);


	Object *testObject = GameWorld->objectList->firstObject;

    while(testObject->ObjectID != VERTICAL_GATE)
    {
        testObject = testObject->nextObject;
    }

	
   //LemonPlaySound("StartUp", "Music", LOOP_CHANNEL, 0.7);


    // Game Loop
   while(gameRunning == 1)
   {
        // Window messages
        while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) { DispatchMessage(&Msg); }


		// Timing
		gameTick = clock() - lastTick;
		deltaTime = ((double)(clock() - lastTick)) * 0.0625;
		lastTick = clock();
		deltaTime = 1.0;


		// World updates
		updateObjects(GameWorld, keyboard);


        // Player control
		updatePlayer(player, GameWorld, keyboard);


		worldCameraControl(frame.width, frame.height, player, GameWorld);


		// Render screen
		cleanRenderer(GameWorld, frame.screen, frame.width, frame.height);

		renderBackGroundSprite(frame.screen, frame.width, frame.height, GameWorld);

		drawObjects(BACKGROUND, frame.screen, frame.width, frame.height, GameWorld);

        drawObjects(MIDDLEGROUND, frame.screen, frame.width, frame.height, GameWorld);
		
		drawObjects(FOREGROUND, frame.screen, frame.width, frame.height, GameWorld);
		
	    drawObjects(PARTICLES, frame.screen, frame.width, frame.height, GameWorld);

        drawObjects(HUD, frame.screen, frame.width, frame.height, GameWorld);


        // Force window to update frame
        InvalidateRect(handle, NULL, FALSE);
        UpdateWindow(handle);


	    // Process sound
	    IterateAudio();
       

	    // Framerate control
	    if (keyboard['T'] == 0)
	    {
    	    frameRate(60, gameTick);
	    }

	    if (((double)(clock() - lastSecond) / (double)CLOCKS_PER_SEC) > 0.99)
	    {
            if (GameWorld->GamePaused == 0)
            {
                printf("%d at %lf\n", windowsFrames, ((double)(clock() - lastSecond) / (double)CLOCKS_PER_SEC) );
            }
           
   		    windowsFrames = 0;
   		    lastSecond = clock();
	    }
	    else
	    {
   		    windowsFrames++;
	    }


	    // Debug Controls
        DebugControls(GameWorld, player, keyboard, testObject);

        if (keyboard[LMN_ESCAPE] == 1)
	    {
    	    gameRunning = 0;
	    }
		
    }


	// Clear game data and cleanup
	clearGameData(GameWorld, player);

	CleanUpAudioData();

    SDL_Quit();


    return Msg.wParam;
}


// Window functions
LRESULT CALLBACK WndProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int hasFocus = 1;

    switch(msg)
    {
        case WM_KEYDOWN:
    	case WM_KEYUP:
    	{
    		if (hasFocus == 1)
    		{
    			static int keyIsDown, keyWasDown;

    			keyIsDown = ((lParam & (1 << 31)) == 0);
    			keyWasDown = ((lParam & (1 << 30)) != 0);

    			if (keyIsDown != keyWasDown)
    			{
    				keyboard[(uint8_t)wParam] = keyIsDown;
    			}
    		}
    	} break;


    	case WM_KILLFOCUS:
    	{
    		hasFocus = 0;
    		memset(keyboard, 0, sizeof(keyboard[0]));
    	} break;

    	case WM_SETFOCUS:
    		hasFocus = 1;
    		break;

        case WM_CLOSE:
            DestroyWindow(handle);
        break;
        case WM_DESTROY:
			gameRunning = 0;
            PostQuitMessage(0);
        break;

        case WM_PAINT:
                {
                	static PAINTSTRUCT paint;
                	static HDC device_context;
                	device_context = BeginPaint(handle, &paint);

                	BitBlt(device_context,
                			paint.rcPaint.left, paint.rcPaint.top,
        					paint.rcPaint.right - paint.rcPaint.left, paint.rcPaint.bottom - paint.rcPaint.top,
        					frame_device_context,
        					paint.rcPaint.left, paint.rcPaint.top,
        					SRCCOPY);
							
                	EndPaint(handle, &paint);
                } break;

        case WM_SIZE:
        {
        	frame_bitmap_info.bmiHeader.biWidth = H_RESOLUTION;
        	frame_bitmap_info.bmiHeader.biHeight = V_RESOLUTION;

        	if (frame_bitmap)
        	{
        		DeleteObject(frame_bitmap);
        	}

        	frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, (void**)&frame.screen, 0, 0);
        	SelectObject(frame_device_context, frame_bitmap);

        	frame.width = H_RESOLUTION;
        	frame.height = V_RESOLUTION;
        } break;


        default:
            return DefWindowProc(handle, msg, wParam, lParam);
    }
    return 0;
}

