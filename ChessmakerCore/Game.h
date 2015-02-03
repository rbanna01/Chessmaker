#pragma once

#include <list>
#include "PieceType.h"
#include "Player.h"

class Board;
class EndOfGame;
class GameState;
class TurnOrder;

class Game
{
public:
	Game();
	~Game();

	Board *GetBoard() { return board; }
	GameState *GetCurrentState() { return currentState; }
	TurnOrder *GetTurnOrder() { return turnOrder; }
	EndOfGame *GetEndOfGame() { return endOfGame; }

private:
	Board *board;
	GameState *currentState;
	TurnOrder *turnOrder;
	EndOfGame *endOfGame;
	std::list<PieceType*> allPieceTypes;
	std::list<Player*> players;
	
	friend class GameParser;
};

