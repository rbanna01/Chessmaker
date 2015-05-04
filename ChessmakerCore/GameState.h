#pragma once

#include "Definitions.h"

class Game;
class Move;
class Piece;
class Player;

class GameState
{
public:
	GameState(Game *game, Player *currentPlayer, GameState *previousState);
	~GameState();

	std::list<Move*> *GetPossibleMoves();
	std::list<Move*> *DetermineThreatMoves();

	Game *GetGame() { return game; }
	Player *GetCurrentPlayer() { return currentPlayer; }
	int GetTurnNumber() { return turnNumber; }
	GameState *GetPreviousState() { return previousState; }
	void DiscardState();
	Move *GetSubsequentMove() { return subsequentMove; }
	void ClearPossibleMoves(Move *dontDelete = 0);
	const char *GetHash() { return hash.c_str(); }
	void DetermineHash();
	void MarkForPositionRepetition() { countedForPositionRepetition = true; }

private:
	Game *game;
	Player *currentPlayer;
	int turnNumber;
	GameState *previousState;
	Move *subsequentMove;
	std::list<Move*> *possibleMoves;
	std::string hash;
	bool countedForPositionRepetition;

	void CalculateMovesForPlayer(Player *player, std::list<Move*> *output);

	friend class StateLogic;
	friend class Game;
	friend class Move;
};

