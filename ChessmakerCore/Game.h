#pragma once

#include "Definitions.h"
#include "PieceType.h"
#include "Player.h"
#include "StateLogic.h"

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
	GameEnd* CheckStartOfTurn(GameState *state, bool canMove);
	GameEnd* CheckEndOfTurn(GameState *state);
	bool GetHoldCapturedPieces() { return holdCapturedPieces; }
	bool ShouldShowCapturedPieces() { return showCapturedPieces; }
	bool ShouldShowHeldPieces() { return showHeldPieces; }
	bool AnyIllegalMovesSpecified() { return illegalMovesSpecified; }

	std::list<PieceType*> GetAllPieceTypes() { return allPieceTypes; }
	std::list<Player*> GetPlayers() { return players; }
private:
	bool StartNextTurn();
	bool EndTurn(GameState *newState, Move *lastMove);
	void ProcessEndOfGame(GameEnd* result);
	void EndGame(Player *victor, const char *message);

	void ApplyUniqueNotation(std::list<Move*> *moves);
	void LogMove(Move *move, int turnNumber, const char *appendNotation);

	Board *board;
	GameState *currentState;
	TurnOrder *turnOrder;
	StateLogic *startOfTurnLogic, *endOfTurnLogic;
	std::list<PieceType*> allPieceTypes;
	std::list<Player*> players;
	const char *startOfTurnAppendNotation;

	bool holdCapturedPieces, showCapturedPieces, showHeldPieces, illegalMovesSpecified;
	
	friend class GameParser;
	friend class GameState;
	friend class Move;
};

