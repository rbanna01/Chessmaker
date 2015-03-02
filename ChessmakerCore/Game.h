#pragma once

#include <list>
#include "EndOfGame.h"
#include "PieceType.h"
#include "Player.h"

class Board;
class GameState;
class Move;
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
	void StartNextTurn();
	void EndTurn(GameState *newState, Move *move);
	void ProcessEndOfGame(EndOfGame::CheckType_t);
	void EndGame(Player *victor);

	Board *board;
	GameState *currentState;
	TurnOrder *turnOrder;
	EndOfGame *endOfGame;
	std::list<PieceType*> allPieceTypes;
	std::list<Player*> players;
	
	friend class GameParser;
};

