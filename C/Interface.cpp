#include <stdio.h>
#include <string.h>
#include "Game.h"

Game *game = 0;

extern "C" __declspec(dllexport)
void Initialize(const char* definition)
{
	if (game != 0)
		delete game;

	game = new Game();
	printf("Got a new game definition:\n");
	printf(definition);
	printf("\n");
}

extern "C" __declspec(dllexport)
void GetBoardSVG(char *buffer, int maxLen)
{
	strncpy_s(buffer, maxLen, "this should be an SVG", maxLen);
}