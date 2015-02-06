#include <stdio.h>
#include <utility>
#include <string.h>
#include "Game.h"
#include "GameParser.h"
#include "TurnOrder.h"

Game *game = 0;
std::string *boardSVG = 0;

extern "C" __declspec(dllexport)
bool Initialize(char* definition)
{
	if (game != 0)
		delete game;
	if (boardSVG != 0)
		delete boardSVG;

	boardSVG = new std::string();
	GameParser *parser = new GameParser();
	game = parser->Parse(definition, boardSVG);
	delete parser;

	if (game == 0)
		return false;

	return true;
}

extern "C" __declspec(dllexport)
std::string *GetBoardSVG()
{
	return boardSVG;
}