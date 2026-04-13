#include "LemonEngine/LemonEngine.h"

 
int main(void)
{
	RunLemonEngine();

	return 0;
}


// 64 Bit SDL
/*
gcc main.c LemonMain.c gameObjects.c levelLoader.c drawScreen.c playerController.c spriteLoader.c soundProcessor.c eventManager.c animations.c UIObjects.c cutsceneManager.c -o stable\LemonEngine.exe -O3 -lSDL3 -lSDL3_ttf -lSDL3_image -lSDL3_mixer -Wall

*/