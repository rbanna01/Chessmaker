#pragma once

#include "Definitions.h"
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

	void Start();

	Board *GetBoard() { return board; }
	GameState *GetCurrentState() { return currentState; }
	TurnOrder *GetTurnOrder() { return turnOrder; }
	EndOfGame *GetEndOfGame() { return endOfGame; }

	bool GetHoldCapturedPieces() { return holdCapturedPieces; }

	std::list<PieceType*> GetAllPieceTypes() { return allPieceTypes; }
	std::list<Player*> GetPlayers() { return players; }
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

	bool holdCapturedPieces;
	
	friend class GameParser;
	friend class GameState;
	friend class Move;
};

