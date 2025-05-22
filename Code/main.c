#include "LemonMain.h"



int main(void)
{
	runLemonEngine();

	return 0;
}



// gcc windows_main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c -o LemonEngine.exe -lgdi32 -lwinmm -O3 -I C:\Libraries\i686-w64-mingw32\include -L C:\Libraries\i686-w64-mingw32\lib -lSDL3
// gcc windows_main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c -o LemonEngine.exe_Unoptimised -lgdi32 -lwinmm -I C:\Libraries\i686-w64-mingw32\include -L C:\Libraries\i686-w64-mingw32\lib -lSDL3



// cd C:\Users\usern\Documents\C Projects\Lemon Engine
// git add documentation.md Code Executables .gitignore


/*
TO DO:

(In order of importance)

- Fix slope collision - NEW SYSTEM!!!!
- Add sprite render modes 5/6, tile/scale rendermodes without transparency
- Add decoding/encoding function for txt -> lem files
- Add modifiable offsets to rendermode 4 sprites/objects?
- Design HUD/menu system (??)

*\