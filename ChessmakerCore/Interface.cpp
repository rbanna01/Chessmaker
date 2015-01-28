#include <stdio.h>
#include <utility>
#include <string.h>
#include "Game.h"
#include "GameParser.h"

Game *game = 0;
std::string *boardSVG = 0;

extern "C" __declspec(dllexport)
bool Initialize(char* definition, int svgBufferLength)
{
	if (game != 0)
		delete game;
	if (boardSVG != 0)
		delete boardSVG;

	boardSVG = new std::string();
	GameParser *parser = new GameParser();
	game = parser->Parse(definition, boardSVG);
	delete parser;

	if (game != 0)
	{
		printf("Definition parsed successfully\n");
		return true;
	}
	return false;
}

extern "C" __declspec(dllexport)
void GetBoardSVG(char *buffer, int maxLen)
{
	strncpy(buffer, boardSVG->c_str(), maxLen);
	delete boardSVG;
	boardSVG = 0;
}