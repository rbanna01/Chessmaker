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

#ifdef EMSCRIPTEN
#define EXPOSE_METHOD extern "C" EMSCRIPTEN_KEEPALIVE 
#define BOOL_TYPE int
#define TRUE_VAL 1
#define FALSE_VAL 0
#else
#define EXPOSE_METHOD extern "C" __declspec(dllexport)
#define BOOL_TYPE bool
#define TRUE_VAL true
#define FALSE_VAL false
#endif

Game *game = 0;

#ifndef NO_SVG
std::string *boardSVG = 0;
#endif

EXPOSE_METHOD
BOOL_TYPE Parse(char* definition)
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
		return FALSE_VAL;

#ifdef CONSOLE
	printf("Definition parsed successfully\n");
#endif

	return TRUE_VAL;
}

EXPOSE_METHOD
void Start()
{
	game->Start();
}

#ifndef NO_SVG
EXPOSE_METHOD
const char *GetBoardSVG()
{
	return boardSVG->c_str();
}
#endif

EXPOSE_METHOD
BOOL_TYPE SetPlayerLocal(int number)
{
	auto players = game->GetPlayers();
	int i = 1;
	for (auto it = players.begin(); it != players.end(); it++)
	{
		if (i++ == number)
		{
			Player *p = *it;
			p->SetType(Player::Local);
			return TRUE_VAL;
		}
	}

	ReportError("Only got %i players, cannot set #%i as local\n", players.size(), number);
	return FALSE_VAL;
}

EXPOSE_METHOD
BOOL_TYPE SetPlayerRemote(int number)
{
	auto players = game->GetPlayers();
	int i = 1;
	for (auto it = players.begin(); it != players.end(); it++)
	{
		if (i++ == number)
		{
			Player *p = *it;
			p->SetType(Player::Remote);
			return TRUE_VAL;
		}
	}

	ReportError("Only got %i players, cannot set #%i as remote\n", players.size(), number);
	return FALSE_VAL;
}

EXPOSE_METHOD
BOOL_TYPE SetPlayerAI(int number, const char *aiName)
{
	PlayerAI *ai;
	if (strcmp(aiName, "random") == 0)
		ai = new AI_Random(game);
	else if (strcmp(aiName, "random capture") == 0)
		ai = new AI_RandomCapture(game);
	else if (strcmp(aiName, "alpha beta 3") == 0)
		ai = new AI_AlphaBeta(game, 3);
	else if (strcmp(aiName, "alpha beta 4") == 0)
		ai = new AI_AlphaBeta(game, 4);
	else if (strcmp(aiName, "alpha beta 5") == 0)
		ai = new AI_AlphaBeta(game, 5);
	else if (strcmp(aiName, "alpha beta 6") == 0)
		ai = new AI_AlphaBeta(game, 6);
	else
	{
		ReportError("AI \"%s\" not found, cannot set player #%i as AI\n", aiName, number);
		return FALSE_VAL;
	}

	auto players = game->GetPlayers();
	int i = 1;
	for (auto it = players.begin(); it != players.end(); it++)
	{
		if (i++ == number)
		{
			Player *p = *it;
			p->SetAI(ai);
			return TRUE_VAL;
		}
	}

	delete ai;
	ReportError("Only got %i players, cannot set #%i as AI\n", players.size(), number);
	return FALSE_VAL;
}

EXPOSE_METHOD
BOOL_TYPE ShouldShowCapturedPieces()
{
	return game->ShouldShowCapturedPieces() ? TRUE_VAL : FALSE_VAL;
}

EXPOSE_METHOD
BOOL_TYPE ShouldShowHeldPieces()
{
	return game->ShouldShowHeldPieces() ? TRUE_VAL : FALSE_VAL;
}

EXPOSE_METHOD
void Shutdown()
{
	delete game;
	game = 0;
}

EXPOSE_METHOD
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
				ReportError("An error occurred performing a move\n");
				return -1;
			}
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

#ifdef EMSCRIPTEN
	// todo: show a popup dialog to indicate that the game has encountered an error
#endif
}

#ifdef EMSCRIPTEN
namespace rapidxml
{
	void parse_error_handler(const char *what, void *where) { ReportError(what); printf("\n"); abort(); }
}
#endif