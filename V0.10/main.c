#include "LemonEngine.h"

 
int main(void)
{
	RunLemonEngine();

	return 0;
}



/*
exe
gcc LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c eventManager.c animations.c UIObjects.c cutsceneManager.c -o stable\LemonEngine.exe -O3 -L./stable -lSDL3 -lSDL3_ttf -lSDL3_image -lSDL3_mixer -Wall --all-warnings

dll
gcc LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c eventManager.c animations.c UIObjects.c cutsceneManager.c -o stable\LemonEngine.dll -O3 -L./stable -lSDL3 -lSDL3_ttf -lSDL3_image -lSDL3_mixer -Wall --all-warnings -shared
*/