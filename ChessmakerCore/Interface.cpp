#include <stdio.h>
#include <utility>
#include <string.h>
#include "Game.h"
#include "GameParser.h"
#include "TurnOrder.h"

Game *game = 0;

#ifndef NO_SVG
std::string *boardSVG = 0;
#endif

extern "C" __declspec(dllexport)
bool Initialize(char* definition)
{
	if (game != 0)
		delete game;

#ifndef NO_SVG
	if (boardSVG != 0)
		delete boardSVG;

	boardSVG = new std::string();
#endif

	GameParser *parser = new GameParser();
#ifdef NO_SVG
	game = parser->Parse(definition);
#else
	game = parser->Parse(definition, boardSVG);
#endif
	delete parser;

	if (game == 0)
		return false;

	return true;
}

#ifndef NO_SVG
extern "C" __declspec(dllexport)
std::string *GetBoardSVG()
{
	return boardSVG;
}
#endif