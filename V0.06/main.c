#include "LemonMain.h"


 
int main(void)
{
	runLemonEngine();

	return 0;
}



// gcc windows_main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c eventManager.c animations.c UIObjects.c cutsceneManager.c enemies.c -o LemonEngine.exe -lgdi32 -lwinmm -O3 -I C:\Libraries\i686-w64-mingw32\include -L C:\Libraries\i686-w64-mingw32\lib -lSDL3



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


/*
TO DO:.

(In order of importance)

- Add menu system 
- Add decoding/encoding function for txt -> lem files
- Develop cutscene manager

*\