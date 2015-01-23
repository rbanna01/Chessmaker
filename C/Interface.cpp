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
	game = GameParser::Parse(definition, boardSVG);

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
	strncpy_s(buffer, maxLen, boardSVG->c_str(), maxLen);
	delete boardSVG;
	boardSVG = 0;
}