#include "LemonMain.h"

 
int main(void)
{
	RunLemonEngine();

	return 0;
}


// 64 Bit SDL
/*
gcc main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c eventManager.c animations.c UIObjects.c cutsceneManager.c enemies.c -o LemonEngine.exe -O3 -I C:\Libraries\gcc_64\include -L C:\Libraries\gcc_64\lib -lSDL3 -lSDL3_ttf -lSDL3_image -m64 -Wall -Werror


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