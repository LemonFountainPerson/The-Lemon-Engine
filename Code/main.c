#include "LemonMain.h"



int main(void)
{
	runLemonEngine();

	return 0;
}



// gcc windows_main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c -o LemonEngine.exe -lgdi32 -lwinmm -O3 -I C:\Libraries\i686-w64-mingw32\include -L C:\Libraries\i686-w64-mingw32\lib -lSDL3

// gcc windows_main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c -o LemonEngine_Unoptimised.exe -lgdi32 -lwinmm -I C:\Libraries\i686-w64-mingw32\include -L C:\Libraries\i686-w64-mingw32\lib -lSDL3



// git add Code Executables


/*
TO DO:

(In order of importance)

- Fix slope collision - NEW SYSTEM!!!!
- Add decoding/encoding function for txt -> lem files
- Design HUD/menu system (??)
- Add text box like rpg games
- Add cutscene manager

*\