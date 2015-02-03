#include "Game.h"
#include "Board.h"
#include "EndOfGame.h"
#include "GameState.h"
#include "TurnOrder.h"


Game::Game()
{
	board = 0;
	currentState = 0;
	endOfGame = 0;
	turnOrder = 0;
}


Game::~Game()
{
	if (board != 0)
		delete board;
	if (currentState != 0)
		delete currentState;
	if (endOfGame != 0)
		delete endOfGame;
	if (turnOrder != 0)
		delete turnOrder;

	auto it = players.begin();
	while (it != players.end())
	{
		delete (*it);
		it++;
	}

	auto it2 = allPieceTypes.begin();
	while (it2 != allPieceTypes.end())
	{
		delete (*it2);
		it2++;
	}
}

