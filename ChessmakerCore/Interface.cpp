#include "Definitions.h"

#include <stdarg.h>
#include <stdio.h>
#include <utility>
#include <string.h>
#include "ai.h"
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
bool SetPlayerLocal(const char *playerName)
{
	auto players = game->GetPlayers();
	for (auto it = players.begin(); it != players.end(); it++)
	{
		Player *p = *it;
		if (strcmp(p->GetName(), playerName) == 0)
		{
			p->SetType(Player::Local);
			return true;
		}
	}

	ReportError("Player \"%s\" not found, cannot set as local\n", playerName);
	return false;
}

extern "C" __declspec(dllexport)
bool SetPlayerRemote(const char *playerName)
{
	auto players = game->GetPlayers();
	for (auto it = players.begin(); it != players.end(); it++)
	{
		Player *p = *it;
		if (strcmp(p->GetName(), playerName) == 0)
		{
			p->SetType(Player::Remote);
			return true;
		}
	}

	ReportError("Player \"%s\" not found, cannot set as remote\n", playerName);
	return false;
}

extern "C" __declspec(dllexport)
bool SetPlayerAI(const char *playerName, const char *aiName)
{
	PlayerAI *ai;
	if (strcmp(aiName, "random") == 0)
		ai = new AI_Random(game);
	else if (strcmp(aiName, "random capture") == 0)
		ai = new AI_Random(game);
	else if (strcmp(aiName, "alpha beta") == 0)
		ai = new AI_AlphaBeta(game, 3);
	else
	{
		ReportError("AI \"%s\" not found, cannot set player \"%s\" as AI\n", aiName, playerName);
		return false;
	}

	auto players = game->GetPlayers();
	for (auto it = players.begin(); it != players.end(); it++)
	{
		Player *p = *it;
		if (strcmp(p->GetName(), playerName) == 0)
		{
			p->SetAI(ai);
			return true;
		}
	}

	delete ai;
	ReportError("Player \"%s\" not found, cannot set as AI\n", playerName);
	return false;
}

extern "C" __declspec(dllexport)
void Shutdown()
{
	delete game;
	game = 0;
}

extern "C" __declspec(dllexport)
std::string *ListPossibleMoves()
{
	std::string *output = new std::string("Possible moves:\n");

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
			do
			{
				auto result = game->PerformMove(move);
				switch (result)
				{
				case Game::GameComplete:
					return 0;
				case Game::MoveComplete:
					if (game->GetCurrentState()->GetCurrentPlayer()->GetType() == Player::AI)
					{
						move = game->GetCurrentState()->GetCurrentPlayer()->GetAI()->SelectMove();
						if (move == 0)
						{
							ReportError("AI failed to select a move\n");
							return -1;
						}
						continue;
					}
					else
						return 1;
				default:
					ReportError("An error occurred performing a move\n");
					return -1;
				}
			} while (true);
		}
	}
	return -1;
}


void ReportError(const char *msg, ...)
{
	printf("Error: ");

	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);

	// todo: if using emscripten, show a popup dialog to indicate that the game has encountered an error
}