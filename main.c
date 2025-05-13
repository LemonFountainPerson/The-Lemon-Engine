#include "LemonMain.h"



int main(void)
{
	runLemonEngine();

	return 0;
}



// gcc windows_main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c -o LemonEngine.exe -lgdi32 -lwinmm -O3 -I C:\Libraries\SDL3-3.2.10\i686-w64-mingw32\include -L C:\Libraries\SDL3-3.2.10\i686-w64-mingw32\lib -lSDL3



// git add main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c LemonMain.h gameObjects.h stb_image.h levelLoader.h drawScreen.h playerController.h spriteLoader.h soundProcessor.h LemonData windows_main.c LemonEngine.exe Documentation.md
