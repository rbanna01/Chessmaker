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

	typedef enum { MoveComplete, GameComplete, MoveError } MoveResult_t;

	void Start();
	MoveResult_t PerformMove(Move *move);

	Board *GetBoard() { return board; }
	GameState *GetCurrentState() { return currentState; }
	TurnOrder *GetTurnOrder() { return turnOrder; }
	EndOfGame *GetEndOfGame() { return endOfGame; }
	std::list<Move*> *GetPossibleMoves() { return possibleMoves; }
	bool GetHoldCapturedPieces() { return holdCapturedPieces; }
	bool ShouldShowCapturedPieces() { return showCapturedPieces; }
	bool ShouldShowHeldPieces() { return showHeldPieces; }

	std::list<PieceType*> GetAllPieceTypes() { return allPieceTypes; }
	std::list<Player*> GetPlayers() { return players; }
private:
	bool StartNextTurn();
	bool EndTurn(GameState *newState, Move *move);
	void ProcessEndOfGame(EndOfGame::CheckType_t);
	void EndGame(Player *victor);

	void ClearPossibleMoves();
	void LogMove(Player *player, Move *move);

	Board *board;
	GameState *currentState;
	TurnOrder *turnOrder;
	EndOfGame *endOfGame;
	std::list<PieceType*> allPieceTypes;
	std::list<Player*> players;
	std::list<Move*> *possibleMoves;

	bool holdCapturedPieces, showCapturedPieces, showHeldPieces;
	
	friend class GameParser;
	friend class GameState;
	friend class Move;
};

