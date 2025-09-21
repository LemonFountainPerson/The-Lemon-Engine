#include "LemonMain.h"

int Running_In_Windows_Mode = 0;

 
int main(void)
{
	RunLemonEngine();

	return 0;
}


// 32 Bit windows
// gcc windows_main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c eventManager.c animations.c UIObjects.c cutsceneManager.c enemies.c -o LemonEngine.exe -lgdi32 -lwinmm -O3 -I C:\Libraries\i686-w64-mingw32\include -L C:\Libraries\i686-w64-mingw32\lib -lSDL3

// 64 Bit windows
// gcc windows_main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c eventManager.c animations.c UIObjects.c cutsceneManager.c enemies.c -o LemonEngine.exe -lgdi32 -lwinmm -O3 -I C:\Libraries\gcc_64\include -L C:\Libraries\gcc_64\lib -lSDL3

// 64 Bit SDL
// gcc main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c eventManager.c animations.c UIObjects.c cutsceneManager.c enemies.c -o LemonEngine_SDL_64Bit.exe -lgdi32 -lwinmm -O3 -I C:\Libraries\gcc_64\include -L C:\Libraries\gcc_64\lib -lSDL3 -m64


/*

Control structure:

LemonMain.c <- data.h
|
| gameObjects.c
| | animations.c
| | UIObjects.c
| | spriteLoader.c
| | soundProcessor.c
|
| playerController.c
| | animations.c
| | soundProccessor.c
| | ...
| 
| drawScreen.c
| | ...
| 
| eventManager.c
| | gameObjects.c
| | ...
|
| cutsceneManager.c
| | gameObjects.c
| | animations.c
| | soundProcessor.c
| | ...


*/