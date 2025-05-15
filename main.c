#include "LemonMain.h"



int main(void)
{
	runLemonEngine();

	return 0;
}



// gcc windows_main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c -o LemonEngine.exe -lgdi32 -lwinmm -O3 -I C:\Libraries\i686-w64-mingw32\include -L C:\Libraries\i686-w64-mingw32\lib -lSDL3



// git add data.h main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c LemonMain.h gameObjects.h stb_image.h levelLoader.h drawScreen.h playerController.h spriteLoader.h soundProcessor.h LemonData windows_main.c LemonEngine.exe Documentation.md



/*
TO DO:

(In order of importance)

- Fix collision on moving objects (maybe to do with floating point inaccuracies?)
- Fix render mode 1 on backgrounds
- Make basis for particle system: objects that use animation tick to determine when to delete and how to animate, being on particle layer marks it as unimportant for collision evaluation, EFFICIENCYEFFICIENCY
- Add modifiable offsets to rendermode 4 sprites/objects?
- Design HUD/menu system (??)
- Add object type enum? Only for seperating groups of objects for efficiency/readability; may not be worth it

*\