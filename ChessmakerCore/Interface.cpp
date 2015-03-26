#include "Definitions.h"

#include <stdio.h>
#include <utility>
#include <string.h>
#include "Game.h"
#include "GameParser.h"
#include "GameState.h"
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

	game->Start();

	return true;
}

#ifndef NO_SVG
extern "C" __declspec(dllexport)
std::string *GetBoardSVG()
{
	return boardSVG;
}
#endif

extern "C" __declspec(dllexport)
void Shutdown()
{
	delete game;
	game = 0;
}

extern "C" __declspec(dllexport)
std::string *ListPossibleMoves()
{
	std::string *output = new std::string(game->GetCurrentState()->GetCurrentPlayer()->GetName());
	output->append(" to move.\nPossible moves:\n");

	auto moves = game->GetPossibleMoves();
	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		Move *move = *it;
		output->append(move->GetNotation());
		output->append("\n");
	}

	return output;
}

extern "C" __declspec(dllexport)
int PerformMove(const char *notation)
{
	// See if this is a valid move, and if so, perform it. If not, return -1.
	// If the game has finished, return 0, otherwise, return 1.

	auto moves = game->GetPossibleMoves();
	for (auto it = moves->begin(); it != moves->end(); it++)
	{
		Move *move = *it;
		if (strcmp(notation, move->GetNotation()) == 0)
		{
			auto result = game->PerformMove(move);
			switch (result)
			{
			case Game::GameComplete:
				return 0;
			case Game::MoveComplete:
				return 1;
			default:
				return -1;
			}
		}
	}
	return -1;
}